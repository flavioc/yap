/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   Flávio Cruz - University of Porto, Portugal
  File:        tab.consumer.c
                                                                     
**********************************************************************/

/* ------------------ **
**      Includes      **
** ------------------ */

#include "Yap.h"
#ifdef TABLING_GROUNDED
 
#include <stdio.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#include "Yatom.h"
#include "YapHeap.h"
#include "yapio.h"
#include "tab.macros.h"
#include "tab.stack.h"
#include "tab.utils.h"
#include "tab.tries.h"

static int
is_internal_subgoal_frame(sg_fr_ptr specific_sg, sg_fr_ptr sf, choiceptr limit)
{
  sg_fr_ptr top_gen = SgFr_top_gen_sg(sf);
  
  while(top_gen && SgFr_choice_point(top_gen) <= limit) {
    if(top_gen == specific_sg)
      return TRUE;
      
    top_gen = SgFr_top_gen_sg(top_gen);
  }
  
  return FALSE;
}

static inline int
is_internal_dep_fr(sg_fr_ptr specific_sg, dep_fr_ptr dep_fr, choiceptr limit)
{
  sg_fr_ptr top_gen = DepFr_top_gen_sg(dep_fr);
  
  while(top_gen && SgFr_choice_point(top_gen) <= limit) {
    if(top_gen == specific_sg)
      return TRUE;
    
    top_gen = SgFr_top_gen_sg(top_gen);
  }
  
  return FALSE;
}

static inline dep_fr_ptr
find_external_consumer(choiceptr max, sg_fr_ptr gen, dep_fr_ptr *external_before)
{
  dep_fr_ptr top = LOCAL_top_dep_fr;
  dep_fr_ptr found = NULL;
  dep_fr_ptr before = NULL;
  
  /* find first running consumer starting from max */
  while(top && YOUNGER_CP(DepFr_cons_cp(top), max)) {
    if(DepFr_sg_fr(top) == gen) {
      dprintf("Found one external dep_fr %d\n", (int)top);
      *external_before = before;
      found = top;
    }
    /* XXX: subsumptive, ground */
    before = top;
    top = DepFr_next(top);
  }
  
  return found;
}

static inline void
update_leader_fields(choiceptr old_leader, choiceptr new_leader, choiceptr max)
{
  dep_fr_ptr top = LOCAL_top_dep_fr;
  
  while(top && YOUNGER_CP(DepFr_cons_cp(top), max)) {
    if(DepFr_leader_cp(top) == old_leader) {
      dprintf("Changed leader from %d to %d on cp %d dep_fr %d\n", (int)old_leader, (int)new_leader, (int)DepFr_cons_cp(top), (int)top);
      DepFr_leader_cp(top) = new_leader;
    }
    
    top = DepFr_next(top);
  }
}

/* assumes there's something up in the stack (prev != NULL) */
#define remove_subgoal_frame_from_stack(SG_FR)      \
  SgFr_next(SgFr_prev(SG_FR)) = SgFr_next(SG_FR);   \
  if(SgFr_next(SG_FR))                              \
    SgFr_prev(SgFr_next(SG_FR)) = SgFr_prev(SG_FR)

static inline void
reorder_subgoal_frame(sg_fr_ptr sg_fr, choiceptr new_gen_cp)
{
  sg_fr_ptr prev = SgFr_prev(sg_fr);
  
  dprintf("Reordering subgoal frame on the stack\n");
  
  if(prev) {
    if(new_gen_cp > SgFr_choice_point(prev))
      return; /* new_gen_cp is still older! */
    
    /* prepare to move the subgoal frame up in the stack */
    remove_subgoal_frame_from_stack(sg_fr);
    
    prev = SgFr_prev(prev);
    
    while(prev && SgFr_choice_point(prev) > new_gen_cp)
      /* while new_gen_cp is newer than the current sg frame ... */
      prev = SgFr_prev(prev); /* .. move up */
    
    /* now put the subgoal frame here, with SgFr_prev(sg_fr) = prev */
    if(prev == NULL) {
      /* prev goes to the top! */
      if(LOCAL_top_sg_fr)
        SgFr_prev(LOCAL_top_sg_fr) = sg_fr;
      SgFr_next(sg_fr) = LOCAL_top_sg_fr;
      LOCAL_top_sg_fr = sg_fr;
    } else {
      sg_fr_ptr before = SgFr_next(prev);
      
      if(before)
        SgFr_prev(before) = sg_fr;
      SgFr_next(prev) = sg_fr;
      SgFr_next(sg_fr) = before;
    }
    SgFr_prev(sg_fr) = prev; /* prev can be NULL */
  }
}

/* note that the general subgoal is always on the top of the generator stack ;-) */
static inline void
abolish_generator_subgoals_between(sg_fr_ptr specific_sg, choiceptr min, choiceptr max)
{
  sg_fr_ptr top = SgFr_next(LOCAL_top_sg_fr);
  dep_fr_ptr external, external_before = NULL;
  
  /* ignore younger generators */
  while(top && YOUNGER_CP(SgFr_choice_point(top), max)) {
    dprintf("Ignored one younger generator cp %d\n", (int)SgFr_choice_point(top));
    top = SgFr_next(top);
  }
  
  /* abolish generators */
  while(top && EQUAL_OR_YOUNGER_CP(SgFr_choice_point(top), min))
  {
    sg_fr_ptr sg_fr;
    
    sg_fr = top;
    
    top = SgFr_next(sg_fr);
    
    if(SgFr_choice_point(sg_fr) == min) {
      abolish_incomplete_producer_subgoal(sg_fr);
      remove_subgoal_frame_from_stack(sg_fr);
    } else if(is_internal_subgoal_frame(specific_sg, sg_fr, min)) {
      dprintf("Trying to abolish %d\n", (int)sg_fr);
      external = find_external_consumer(max, sg_fr, &external_before);
      if(external) {
        /* generator subgoal must be kept */
        dprintf("External dep_fr %d cp %d (REMOVED)\n", (int)external, (int)DepFr_cons_cp(external));
        
        choiceptr cons_cp = DepFr_cons_cp(external);
        choiceptr gen_cp = SgFr_choice_point(sg_fr);
        
        /* delete dependency frame from dependency space */
        if(external_before == NULL)
          LOCAL_top_dep_fr = DepFr_next(external);
        else
          DepFr_next(external_before) = DepFr_next(external);
        
        /* execute RESTART_GENERATOR on backtracking */
        cons_cp->cp_ap = RESTART_GENERATOR;
        CONS_CP(cons_cp)->cp_sg_fr = sg_fr;
        /* don't know if the subgoal frame uses local scheduling,
           but this should work */
        CONS_CP(cons_cp)->cp_dep_fr = GEN_CP(gen_cp)->cp_dep_fr;        
        
        /* update leader information to point to this choice point */
        update_leader_fields(SgFr_choice_point(sg_fr), cons_cp, max);
        
        /* change generator choice point */
        SgFr_choice_point(sg_fr) = cons_cp;
        
        /* reorder this generator on the generator stack
           possibly moving it back, so it's pretty
           safe for the next iteration of this loop */
        reorder_subgoal_frame(sg_fr, cons_cp);
        
        SgFr_try_answer(sg_fr) = DepFr_last_answer(external);
        abolish_dependency_frame(external);
      } else {
        dprintf("REALLY ABOLISHED %d\n", (int)sg_fr);
        abolish_incomplete_producer_subgoal(sg_fr);
        remove_subgoal_frame_from_stack(sg_fr);
      }
    }
  }
}

static inline choiceptr
find_other_consumer(choiceptr min, sg_fr_ptr cons)
{
  dep_fr_ptr top = LOCAL_top_dep_fr;
  
  while(top && YOUNGER_CP(DepFr_cons_cp(top), min)) {
    if(DepFr_sg_fr(top) == cons) {
      return DepFr_cons_cp(top);
    }
    top = DepFr_next(top);
  }
  
  return NULL;
}

static inline void
abolish_dependency_frames_between(sg_fr_ptr specific_sg, choiceptr min, choiceptr max)
{
  dep_fr_ptr top = LOCAL_top_dep_fr;
  dep_fr_ptr before = NULL;
  
  while(top && YOUNGER_CP(DepFr_cons_cp(top), max)) {	 	
    before = top;	 	
    dprintf("Skipped one consumer %d\n", (int)DepFr_cons_cp(top));	 	
    top = DepFr_next(top);
  }
  
  dprintf("min=%d max=%d\n", (int)min, (int)max);
  
  while(top && YOUNGER_CP(DepFr_cons_cp(top), min)) {
    dep_fr_ptr dep_fr = top;
    top = DepFr_next(dep_fr);
    
    if(is_internal_dep_fr(specific_sg, dep_fr, min)) {
      if(before == NULL)
        LOCAL_top_dep_fr = top;
      else
        DepFr_next(before) = top;
      dprintf("Removing consumer choice point %d\n", (int)DepFr_cons_cp(dep_fr));
      abolish_dependency_frame(dep_fr);
    } else
      before = dep_fr;
  }
}

static inline void
adjust_generator_to_consumer_answer_template(choiceptr cp, sg_fr_ptr sg_fr)
{
#if 0
  /* consumer choice points have the same configuration *for now* */
  CELL* current_at = GENERATOR_ANSWER_TEMPLATE(cp, sg_fr);
  CELL* new_at = CONSUMER_NODE_ANSWER_TEMPLATE(cp);
  
  memmove(new_at, current_at, (1 + SgFr_arity(sg_fr)) * sizeof(CELL));
#endif
}

static inline choiceptr
locate_after_answer(choiceptr new_ans, choiceptr cp)
{
  choiceptr before = cp;
  cp = cp->cp_b;

  while(cp != new_ans) {
    before = cp;
    cp = cp->cp_b;
  }
  
  return before;
}

static inline void
update_top_gen_sg_fields(sg_fr_ptr specific_sg, choiceptr limit)
{
  sg_fr_ptr new_top = SgFr_top_gen_sg(specific_sg);
 
  /* generator subgoal frames */
  sg_fr_ptr top_gen = LOCAL_top_sg_fr;
  while(top_gen && SgFr_choice_point(top_gen) <= limit) {
    if(SgFr_top_gen_sg(top_gen) == specific_sg) {
      //printf("Updated one top gen sg\n");
      SgFr_top_gen_sg(top_gen) = new_top;
    }
    top_gen = SgFr_next(top_gen);
  }
  
  /* dependency frames */
  dep_fr_ptr top_dep = LOCAL_top_dep_fr;
  while(top_dep && DepFr_cons_cp(top_dep) <= limit) {
    if(DepFr_top_gen_sg(top_dep) == specific_sg) {
      dprintf("Update one top dep fr sg\n");
      DepFr_top_gen_sg(top_dep) = new_top;
    }
    
    top_dep = DepFr_next(top_dep);
  }
}

static inline void
producer_to_consumer(grounded_sf_ptr sg_fr, grounded_sf_ptr producer)
{
#ifdef FDEBUG
  if(SgFr_is_external(sg_fr))
    printf("external\n");
  else
    printf("internal\n");
#endif
  
  choiceptr gen_cp = SgFr_choice_point(sg_fr);
  choiceptr limit_cp;
  
  if(SgFr_is_external(sg_fr))
    limit_cp = SgFr_new_answer_cp(sg_fr);
  else
    limit_cp = B->cp_b;
  
  dprintf("gen_cp=%d limit_cp=%d\n", (int)gen_cp, (int)limit_cp);
  
  choiceptr min = gen_cp;
  choiceptr max;
  
  if(SgFr_is_internal(sg_fr))
    max = B_FZ < limit_cp ? B_FZ : limit_cp;
  else
    max = SgFr_saved_max(sg_fr);
    
  dprintf("min=%d max=%d\n", (int)min, (int)max);
  
  abolish_generator_subgoals_between((sg_fr_ptr)sg_fr, min, max);
  abolish_dependency_frames_between((sg_fr_ptr)sg_fr, min, max);
  /* update top generator subgoal */
  /* use min to include internal subgoal frames that have external consumers */
  update_top_gen_sg_fields((sg_fr_ptr)sg_fr, min);
  
  /* update generator choice point to point to RUN_COMPLETED */
  gen_cp->cp_ap = (yamop *)RUN_COMPLETED;
  /* use cp_dep_fr to put the subgoal frame */
  CONS_CP(gen_cp)->cp_sg_fr = sg_fr;
  CONS_CP(gen_cp)->cp_dep_fr = NULL;
  adjust_generator_to_consumer_answer_template(gen_cp, (sg_fr_ptr)sg_fr);
  /* set last answer consumed for load answers */
  SgFr_try_answer(sg_fr) = SgFr_last_answer(sg_fr);
  
  if(SgFr_is_internal(sg_fr)) {
    dprintf("set as local producer\n");
    SgFr_set_local_producer(producer);
    SgFr_set_local_consumer(sg_fr);
    B->cp_b = gen_cp;
  } else {
    choiceptr cp = locate_after_answer(limit_cp, B);
    
    cp->cp_b = gen_cp;
  }
}

void
process_pending_subgoal_list(node_list_ptr list, grounded_sf_ptr sg_fr) {
  node_list_ptr orig = list;
  
  while(list) {
    grounded_sf_ptr pending = (grounded_sf_ptr)NodeList_node(list);
    
    if(pending != sg_fr) { /* ignore self! */
      if(SgFr_state(pending) == ready) {
        /*
         * this subgoal is incomplete
         * change the producer of the subgoal so
         * that it can be seen as completed if
         * it is called again in the future
         */
        grounded_sf_ptr producer = SgFr_producer(pending);
        if(producer && SgFr_state(producer) >= complete) {
          /* already completed */
          mark_ground_consumer_as_completed(pending);
        } else {
          SgFr_producer(pending) = sg_fr;
          SgFr_set_type(pending, GROUND_CONSUMER_SFT);
          dprintf("MARKED AS CONSUMER\n");
        }
      } else if(SgFr_state(pending) == evaluating && SgFr_is_ground_producer(pending)) {
        /*
         * this is a consumer subgoal but is running
         * as a generator node, we must
         * change the WAM stacks and tabling data
         * structures to change its status
         */
        SgFr_producer(pending) = sg_fr;
        SgFr_set_type(pending, GROUND_CONSUMER_SFT);
        /* ensure creation of TST indices */
        ensure_has_proper_consumers(SgFr_tab_ent(sg_fr));
#ifdef FDEBUG
        printf("Found a specific subgoal already running\n");
        printSubgoalTriePath(stdout, SgFr_leaf(pending), SgFr_tab_ent(pending));
        printf("\n");  
#endif
        producer_to_consumer(pending, sg_fr);
      }
    }
    
    list = NodeList_next(list);
  }
  free_node_list(orig);
  
  dprintf("ok\n");
}

static inline void
size_dep_space(void)
{
  dep_fr_ptr top = LOCAL_top_dep_fr;
  int i = 0;
  
  while(top) {
    ++i;
    dprintf("Top: %d\n", (int)top);
    top = DepFr_next(top);
  }
  
  dprintf("TOTAL: %d\n", i);
}

static inline void
find_next_dep_frame(dep_fr_ptr dep_fr, choiceptr cp)
{
  size_dep_space();
  
  if(LOCAL_top_dep_fr == NULL) {
    LOCAL_top_dep_fr = dep_fr;
  }
    
  dep_fr_ptr top = LOCAL_top_dep_fr;
  dep_fr_ptr before = NULL;
  
  while(top && YOUNGER_CP(DepFr_cons_cp(top), cp)) {
    before = top;
    dprintf("One dep_fr %d\n", (int)top);
    top = DepFr_next(top);
  }
  
  if(before == NULL) {
    dprintf("set to top\n");
    LOCAL_top_dep_fr = dep_fr;
  } else {
    dprintf("set to next\n");
    SgFr_next(before) = dep_fr;
  }
  
  SgFr_next(dep_fr) = top;
  dprintf("Now\n");
  size_dep_space();
}

void
add_dependency_frame(grounded_sf_ptr sg_fr, choiceptr cp)
{
  dep_fr_ptr dep_fr;
  grounded_sf_ptr producer = SgFr_producer(sg_fr);
  
  new_dependency_frame(dep_fr, TRUE, LOCAL_top_or_fr, SgFr_choice_point(producer), cp, (sg_fr_ptr)sg_fr, NULL);
  find_next_dep_frame(dep_fr, cp);
  SgFr_num_deps(sg_fr)++;
  
  /* turn generator choice point as consumer */
  CONS_CP(cp)->cp_dep_fr = dep_fr;
  CONS_CP(cp)->cp_sg_fr = NULL;
  if(SgFr_try_answer(sg_fr))
    DepFr_last_answer(dep_fr) = SgFr_try_answer(sg_fr);
  if(SgFr_is_ground_local_producer(producer))
    DepFr_set_top_consumer(dep_fr);
}

#endif /* TABLING_GROUNDED */
