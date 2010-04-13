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
#ifdef TABLING_CALL_SUBSUMPTION
 
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

static void inline
remove_generator_stack(grounded_sf_ptr sg_fr)
{
  sg_fr_ptr top = LOCAL_top_sg_fr;
  sg_fr_ptr find = (sg_fr_ptr)sg_fr;
  sg_fr_ptr before;
  
  while(top && top != find) {
    before = top;
    top = SgFr_next(top);
  }
  
  dprintf("subgoal frame %d next points to %d\n", (int)before, (int)SgFr_next(find));
  SgFr_next(before) = SgFr_next(find);
  SgFr_next(find) = NULL;
}

static inline int
is_internal_choice_point(choiceptr cp, choiceptr limit)
{
  while(cp && cp <= limit) {
    if(cp == limit)
      return TRUE;
    cp = cp->cp_b;
  }
  
  return FALSE;
}

static inline int
is_internal_subgoal_frame(sg_fr_ptr sf, choiceptr limit)
{
  return is_internal_choice_point(SgFr_choice_point(sf), limit);
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
find_external_consumer(choiceptr min, choiceptr max, sg_fr_ptr gen)
{
  choiceptr cp = SgFr_choice_point(gen);
  dep_fr_ptr top = LOCAL_top_dep_fr;
  dep_fr_ptr found = NULL;
  
  while(top && YOUNGER_CP(DepFr_cons_cp(top), max)) {
    if(DepFr_sg_fr(top) == gen) {
      /*if(is_internal_dep_fr(top, max)) {
        dprintf("Found one external dep_fr %d\n", (int)top);
        found = top;
      }*/
    }
    /* XXX: subsumptive, ground */
    top = DepFr_next(top);
  }
  
  return found;
}

static inline void
abolish_generator_subgoals_between(choiceptr min, choiceptr max)
{
  sg_fr_ptr top = SgFr_next(LOCAL_top_sg_fr);
  sg_fr_ptr before = LOCAL_top_sg_fr;
  
  /* ignore younger generators */
  while(top && YOUNGER_CP(SgFr_choice_point(top), max)) {
    before = top;
    dprintf("Ignored one younger generator\n");
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
      SgFr_next(before) = top;
    } else
    if(is_internal_subgoal_frame(sg_fr, min)) {
      dprintf("Trying to abolish %d\n", (int)SgFr_choice_point(sg_fr));
      dep_fr_ptr external = find_external_consumer(min, max, sg_fr);
      if(external) {
        /* generator subgoal must be kept */
        dprintf("External dep_fr %d cp %d\n", external, DepFr_cons_cp(external));
        dprintf("Dependency frame kept\n");
        choiceptr gen_cp = SgFr_choice_point(sg_fr);
        
        if(SgFr_got_answer(sg_fr)) {
          SgFr_saved_cp(sg_fr) = SgFr_new_answer_cp(sg_fr);
          printf("Generator has generated an answer\n");
        }
        
        DepFr_set_first_consumer(external);
        gen_cp->cp_b = DepFr_cons_cp(external);
        SgFr_set_producer(sg_fr);
        
        before = sg_fr;
      } else {
        dprintf("REALLY ABOLISHED %d\n", (int)SgFr_choice_point(sg_fr));
        abolish_incomplete_producer_subgoal(sg_fr);
        SgFr_next(before) = top;
      }
    } else
      before = sg_fr;
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
abolish_subsumptive_subgoals_between(choiceptr min, choiceptr max)
{
  subcons_fr_ptr top = LOCAL_top_subcons_sg_fr;
  subcons_fr_ptr before = NULL;
  
  /* ignore younger subsumptive goals */
  while(top && YOUNGER_CP(SgFr_choice_point(top), max)) {
    before = top;
    dprintf("jumped younger subsumptive goal\n");
    top = SgFr_next(top);
  }
  
  while (top && YOUNGER_CP(SgFr_choice_point(top), min)) {
    subcons_fr_ptr sg_fr;
    
    sg_fr = top;
    top = SgFr_next(sg_fr);
    
    if(is_internal_subgoal_frame((sg_fr_ptr)sg_fr, min)) {
      dprintf("Internal cp %d\n", (int)SgFr_choice_point(sg_fr));
      choiceptr new = find_other_consumer(max, (sg_fr_ptr)sg_fr);
      if(new) {
        /* subgoal is running somewhere else */
        dprintf("Keeping subgoal %d\n", (int)SgFr_choice_point(sg_fr));
        SgFr_choice_point(sg_fr) = new;
        before = sg_fr;
      } else {
        dprintf("Removing subsumptive goal %d\n", (int)SgFr_choice_point(sg_fr));
        if(before == NULL)
          LOCAL_top_subcons_sg_fr = top;
        else
          SgFr_next(before) = top;
        LOCK(SgFr_lock(sg_fr));
        abolish_incomplete_subsumptive_consumer_subgoal(sg_fr);
        UNLOCK(SgFr_lock(sg_fr));
      }
    } else
      before = sg_fr;
  }
}

static inline void
abolish_ground_subgoals_between(choiceptr min, choiceptr max)
{
  grounded_sf_ptr top = LOCAL_top_groundcons_sg_fr;
  grounded_sf_ptr before = NULL;
  
  /* ignore younger ground goals */
  while(top && YOUNGER_CP(SgFr_choice_point(top), max)) {
    before = top;
    dprintf("Jumped younger ground subgoal\n");
    top = SgFr_next(top);
  }
  
  while(top && YOUNGER_CP(SgFr_choice_point(top), min)) {
    grounded_sf_ptr sg_fr;
    
    sg_fr = top;
    top = SgFr_next(sg_fr);
    
    if(is_internal_subgoal_frame((sg_fr_ptr)sg_fr, min)) {
      choiceptr new = find_other_consumer(max, (sg_fr_ptr)sg_fr);
      if(new) {
        dprintf("Keeping subgoal %d\n", (int)SgFr_choice_point(sg_fr));
        SgFr_choice_point(sg_fr) = new;
        before = sg_fr;
      } else {
        dprintf("Removing grounded goal %d\n", (int)SgFr_choice_point(sg_fr));
        if(before == NULL)
          LOCAL_top_groundcons_sg_fr = top;
        else
          SgFr_next(before) = top;
        LOCK(SgFr_lock(sg_fr));
        abolish_incomplete_ground_consumer_subgoal(sg_fr);
        UNLOCK(SgFr_lock(sg_fr));
      }
    } else
      before = sg_fr;
  }
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
      FREE_DEPENDENCY_FRAME(dep_fr);
    } else
      before = dep_fr;
  }
}

static inline void
abolish_subgoals_between(choiceptr min, choiceptr max)
{
  abolish_generator_subgoals_between(min, max);
  
  abolish_subsumptive_subgoals_between(min, max);
  
  abolish_ground_subgoals_between(min, max);
}

static inline void
adjust_generator_to_consumer_answer_template(choiceptr cp, sg_fr_ptr sg_fr)
{
  CELL* current_at = GENERATOR_ANSWER_TEMPLATE(cp, sg_fr);
  CELL* new_at = CONSUMER_NODE_ANSWER_TEMPLATE(cp);
  
  memmove(new_at, current_at, (1 + SgFr_arity(sg_fr)) * sizeof(CELL));
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
  
  if(gen_cp == limit_cp && B_FZ < limit_cp) {
    dprintf("ABOLISH B_FZ %d\n", (int)B_FZ);
    abolish_subgoals_between(gen_cp, B_FZ);
    abolish_dependency_frames_between((sg_fr_ptr)sg_fr, gen_cp, B_FZ);
  } else {
    abolish_subgoals_between(gen_cp, limit_cp);
    abolish_dependency_frames_between((sg_fr_ptr)sg_fr, gen_cp, limit_cp);
  }
  
  /* update generator choice point to point to RUN_COMPLETED */
  gen_cp->cp_ap = (yamop *)RUN_COMPLETED;
  /* use cp_dep_fr to put the subgoal frame */
  CONS_CP(gen_cp)->cp_dep_fr = (dep_fr_ptr)sg_fr;
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
  
  /* turn generator choice point as consumer */
  CONS_CP(cp)->cp_dep_fr = dep_fr;
  if(SgFr_try_answer(sg_fr))
    DepFr_last_answer(dep_fr) = SgFr_try_answer(sg_fr);
  if(SgFr_is_ground_local_producer(producer))
    DepFr_set_top_consumer(dep_fr);
}

void
add_ground_subgoal_stack(grounded_sf_ptr sg_fr, choiceptr cp)
{
  grounded_sf_ptr top = LOCAL_top_groundcons_sg_fr;
  grounded_sf_ptr before = NULL;
  
  while(top && YOUNGER_CP(SgFr_choice_point(top), cp)) {
    before = top;
    top = SgFr_next(top);
  }
  
  if(before == NULL) {
    LOCAL_top_groundcons_sg_fr = sg_fr;
    SgFr_next(sg_fr) = NULL;
  } else
    SgFr_next(before) = sg_fr;
}

#endif /* TABLING_CALL_SUBSUMPTION */
