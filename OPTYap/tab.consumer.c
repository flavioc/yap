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

STD_PROTO(static inline void update_top_gen_sg_fields, (sg_fr_ptr, choiceptr, sg_fr_ptr));
STD_PROTO(static inline void to_run_completed_node, (sg_fr_ptr, choiceptr));

void
check_dependency_frame(void)
{
#ifdef FDEBUG
  dep_fr_ptr top = LOCAL_top_dep_fr;
  
  if(top) {
    if(DepFr_prev(top)) {
      dprintf("prev of LOCAL_top_dep_fr must be == NULL\n");
      exit(1);
    }
  }
  while(top) {
    dep_fr_ptr before = top;
    top = DepFr_next(top);
    
    if(top) {
      if(DepFr_prev(top) != before) {
        dprintf("prev(top) must be == before\n");
        exit(1);
      }
    }
    if(top == before)
    {
      dprintf("top can't be equal to before\n");
      exit(1);
    }
  }
#endif
}

static int
is_internal_subgoal_frame(sg_fr_ptr specific_sg, sg_fr_ptr sf, choiceptr min)
{
  sg_fr_ptr top_gen = SgFr_get_top_gen_sg(sf);
  
  while(top_gen && EQUAL_OR_YOUNGER_CP(SgFr_choice_point(top_gen), min)) {
    if(top_gen == specific_sg)
      return SgFr_is_top_internal(sf);
    
    sf = top_gen;
    top_gen = SgFr_get_top_gen_sg(top_gen);
  }
  
  return FALSE;
}

static inline int
is_internal_dep_fr(sg_fr_ptr specific_sg, dep_fr_ptr dep_fr, choiceptr limit)
{
  sg_fr_ptr top_gen = DepFr_get_top_gen_sg(dep_fr);
  
  if(top_gen == specific_sg)
    return DepFr_is_top_internal(dep_fr);
  
  sg_fr_ptr sf = top_gen;
  top_gen = SgFr_get_top_gen_sg(top_gen);
  
  while(top_gen && SgFr_choice_point(top_gen) <= limit) {
    if(top_gen == specific_sg)
      return SgFr_is_top_internal(sf);
    
    sf = top_gen;
    top_gen = SgFr_get_top_gen_sg(top_gen);
  }
  
  return FALSE;
}

static inline dep_fr_ptr
find_external_consumer(sg_fr_ptr specific_sg, choiceptr min, choiceptr max, sg_fr_ptr gen)
{
  dep_fr_ptr top = LOCAL_top_dep_fr;
  dep_fr_ptr found = NULL;
  choiceptr found_cp = NULL;
  choiceptr realmin = SgFr_choice_point(specific_sg);
  int first_external_leader = FALSE;
  
  /* find first running consumer starting from min */
  while(top && YOUNGER_CP(DepFr_cons_cp(top), min)) {
    if(DepFr_sg_fr(top) == gen) {
      dprintf("found a dep fr for gencp %d\n", (int)SgFr_choice_point(gen));
      if(YOUNGER_CP(DepFr_cons_cp(top), max) || !is_internal_dep_fr(specific_sg, top, min)) {
        choiceptr cp = DepFr_cons_cp(top);
        
        dprintf("Found one external dep_fr %d cp %d\n", (int)top, (int)DepFr_cons_cp(top));
        if(!found) {
          choiceptr leader_cp = DepFr_leader_cp(top);
          sg_fr_ptr leader_sg = GEN_CP(leader_cp)->cp_sg_fr;
          
          found = top;
          found_cp = cp;
          cp->cp_ap = RESTART_GENERATOR;
          
          if(leader_sg == gen ||
            is_internal_subgoal_frame(specific_sg, leader_sg, realmin))
          {
            DepFr_leader_cp(top) = cp;
          } else { /* external leader */
            first_external_leader = TRUE;
            /* keep leader */
          }
        } else {
          cp->cp_ap = RUN_COMPLETED;
          
          if(!first_external_leader) {
            /* leader must be internal! */
            DepFr_leader_cp(top) = found_cp;
          } else {
            /* try to locate external leader */
            choiceptr leader_cp = DepFr_leader_cp(top);
            sg_fr_ptr leader_sg = GEN_CP(leader_cp)->cp_sg_fr;
            if(leader_sg == gen ||
                is_internal_subgoal_frame(specific_sg, leader_sg, realmin))
            {
              DepFr_leader_cp(top) = found_cp;
            }
          }
        }
        
        CONS_CP(cp)->cp_sg_fr = gen;
      }
    }
    top = DepFr_next(top);
  }
  
  return found;
}

static inline void
update_leader_fields(choiceptr old_leader, choiceptr new_leader, choiceptr min)
{
  dprintf("where leader is %d change to %d within min=%d\n", (int)old_leader, (int)new_leader, (int)min);
  dep_fr_ptr top = LOCAL_top_dep_fr;
  
  while(top && YOUNGER_CP(DepFr_cons_cp(top), min)) {
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

void
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

/* for a subgoal 'sg_fr' on the generator stack, push it unto the top of the stack */
void
move_subgoal_top(sg_fr_ptr sg_fr)
{
  if(LOCAL_top_sg_fr == sg_fr)
    return;
  
  /* remove from its current position ... */
  remove_subgoal_frame_from_stack(sg_fr);
  
  /* ... and add to top */
  SgFr_next(sg_fr) = LOCAL_top_sg_fr;
  SgFr_prev(sg_fr) = NULL;
  if(LOCAL_top_sg_fr) {
    SgFr_prev(LOCAL_top_sg_fr) = sg_fr;
  }
  LOCAL_top_sg_fr = sg_fr;
}

static inline void
add_new_restarted_generator(choiceptr cp, sg_fr_ptr sf)
{
  node_list_ptr top = LOCAL_restarted_gens;
  node_list_ptr before = NULL;

  while(top && YOUNGER_CP(SgFr_choice_point((sg_fr_ptr)NodeList_node(top)), cp)) {
    before = top;
    top = NodeList_next(top);
  }

  node_list_ptr new_node;

  ALLOC_NODE_LIST(new_node);
  NodeList_node(new_node) = sf;

  if(before) {
    dprintf("not top\n");
    NodeList_next(before) = new_node;
  } else {
    LOCAL_restarted_gens = new_node;
    dprintf("to top\n");
  }

  NodeList_next(new_node) = top;

  dprintf("add_new_restarted_generator\n");
}

static inline void
change_generator_subgoal_frame(sg_fr_ptr sg_fr, dep_fr_ptr external, choiceptr min, choiceptr max)
{
  /* generator subgoal must be kept */
  dprintf("External dep_fr %d cp %d (REMOVED)\n", (int)external, (int)DepFr_cons_cp(external));

  choiceptr cons_cp = DepFr_cons_cp(external);
  choiceptr gen_cp = SgFr_choice_point(sg_fr);

  /* abolish local scheduling dependency frame */
  dep_fr_ptr local_dep = GEN_CP(gen_cp)->cp_dep_fr;
  if(local_dep) {
    REMOVE_DEP_FR_FROM_STACK(local_dep);
  }

  add_new_restarted_generator(cons_cp, sg_fr);
  
  /* update generator choice point */
  SgFr_choice_point(sg_fr) = cons_cp;
  SgFr_state(sg_fr) = suspended;
  reorder_subgoal_frame(sg_fr, cons_cp);
}

static inline void
update_subsumed_before_dependencies(sg_fr_ptr sg_fr, choiceptr min, sg_fr_ptr specific_sg)
{
  choiceptr limit = SgFr_choice_point(sg_fr);
  dep_fr_ptr top = LOCAL_top_dep_fr;
  
  while(top && EQUAL_OR_YOUNGER_CP(DepFr_cons_cp(top), limit)) {
    dprintf("Skipped one possible subsumptive consumer cp %d\n", (int)DepFr_cons_cp(top));
    top = DepFr_next(top);
  }
  
  while(top && YOUNGER_CP(DepFr_cons_cp(top), min)) {
    if(SgFr_is_sub_consumer(DepFr_sg_fr(top)) &&
        (sg_fr_ptr)SgFr_producer((subcons_fr_ptr)DepFr_sg_fr(top)) == sg_fr &&
        !is_internal_dep_fr(specific_sg, top, min))
    {
      sg_fr_ptr cons_sg_fr = DepFr_sg_fr(top);
      choiceptr cp = DepFr_cons_cp(top);
      dprintf("Found one subsumed dependency! cp %d\n", (int)DepFr_cons_cp(top));
      
      cp->cp_ap = (yamop *)RUN_COMPLETED;

      CONS_CP(cp)->cp_sg_fr = cons_sg_fr;
      CONS_CP(cp)->cp_dep_fr = top;
      
      REMOVE_DEP_FR_FROM_STACK(top);
    }

    top = DepFr_next(top);
  }
}

static inline node_list_ptr
find_external_subsumed_consumers(choiceptr min, sg_fr_ptr sg_fr, sg_fr_ptr specific_sg)
{
  dep_fr_ptr top = LOCAL_top_dep_fr;
  node_list_ptr list = NULL;
  
  while(top && YOUNGER_CP(DepFr_cons_cp(top), min)) {
    sg_fr_ptr dep = DepFr_sg_fr(top);
    if(SgFr_is_sub_consumer(dep)) {
      if((sg_fr_ptr)SgFr_producer((subcons_fr_ptr)dep) == sg_fr) {
        if(!is_internal_dep_fr(specific_sg, top, min)) {
          /* add to list */
          node_list_ptr ptr = list;
          int found = FALSE;
          
          /* try to update a repeated element */
          while(ptr) {
            
            if(DepFr_sg_fr((dep_fr_ptr)NodeList_node(ptr)) == dep)
            {
              NodeList_node(ptr) = top;
              found = TRUE;
              break;
            }
            
            ptr = NodeList_next(ptr);
          }
          
          DepFr_sg_fr(top) = dep;
          
          if(!found) {
            /* really insert it on the list */
            dprintf("add to list\n");
            node_list_ptr new_list;
            ALLOC_NODE_LIST(new_list);
            NodeList_node(new_list) = top;
            NodeList_next(new_list) = list;
            list = new_list;
          }
        }
      }
    }
    
    top = DepFr_next(top);
  }
  
  return list;
}

static inline void
transform_sub_ans_tmplt_to_generator(CELL *at, CELL *stack_at)
{
  int size = (int)*stack_at;
  CELL *stack_pt = stack_at + size + 1;
  CELL *at_pt;
  int i;

  dprintf("Answer template size: %d\n", size);


  for(i = 0; i < size; ++i) {
    
  }
}

static inline void
transform_specific_consumer_into_generator(dep_fr_ptr dep_fr)
{
  subcons_fr_ptr cons_sg = (subcons_fr_ptr)DepFr_sg_fr(dep_fr);
  CELL *ans_tmplt = SgFr_answer_template(cons_sg);
  CELL *stack_ans_tmplt = (CELL *)(CONS_CP(DepFr_cons_cp(dep_fr)) + 1) + SgFr_arity(cons_sg);

  transform_sub_ans_tmplt_to_generator(ans_tmplt, stack_ans_tmplt);
}

/* note that the general subgoal is always on the top of the generator stack ;-) */
static inline void
abolish_generator_subgoals_between(sg_fr_ptr specific_sg, choiceptr min, choiceptr max)
{
  sg_fr_ptr top = StackState_sg_fr(SgFr_stack_state((grounded_sf_ptr)specific_sg));
  sg_fr_ptr bottom = SgFr_next(specific_sg);
  sg_fr_ptr sg_fr;
  dep_fr_ptr external;
  
  /* abolish generators */
  while(top != bottom)
  {
    sg_fr = top; 
    top = SgFr_next(sg_fr);
    
    if(sg_fr == specific_sg) {
      abolish_incomplete_producer_subgoal(sg_fr);
      remove_subgoal_frame_from_stack(sg_fr);
      dprintf("ABOLISH SPECIFIC GENERATOR %d\n", (int)min);
    } else if(is_internal_subgoal_frame(specific_sg, sg_fr, min)) {
      dprintf("Trying to abolish %d cp %d\n", (int)sg_fr, (int)SgFr_choice_point(sg_fr));
      switch(SgFr_type(sg_fr)) {
        case VARIANT_PRODUCER_SFT:
          external = find_external_consumer(specific_sg, SgFr_choice_point(sg_fr), max, sg_fr);
          if(external) {
            dprintf("Could find external dep fr!\n");
            change_generator_subgoal_frame(sg_fr, external, min, max);
          } else {
            dprintf("Could not find external dep fr!\n");
            dprintf("REALLY ABOLISHED %d\n", (int)sg_fr);
            abolish_incomplete_producer_subgoal(sg_fr);
            remove_subgoal_frame_from_stack(sg_fr);
          }
          break;
        case SUBSUMPTIVE_PRODUCER_SFT: {
          external = find_external_consumer(specific_sg, SgFr_choice_point(sg_fr), max, sg_fr);
          if(external) {
            dprintf("found external variant subsumptive consumer\n");
            /* variant consumer can generate answers for proper subsumptive consumers */
            change_generator_subgoal_frame(sg_fr, external, min, max);
            update_subsumed_before_dependencies(sg_fr, min, specific_sg);
          } else {
            dprintf("no variant external subsumptive consumer\n");
            node_list_ptr dep_list = find_external_subsumed_consumers(min, sg_fr, specific_sg);
            if(dep_list) {
              dprintf("standalone subsumed consumers\n");
              node_list_ptr list = dep_list;
              while(list) {
                dep_fr_ptr tochange = (dep_fr_ptr)NodeList_node(list);
                transform_specific_consumer_into_generator(tochange);
                list = NodeList_next(list);
              }
              free_node_list(dep_list);
            }
            dprintf("REALLY ABOLISHED %d\n", (int)sg_fr);
            abolish_incomplete_producer_subgoal(sg_fr);
            remove_subgoal_frame_from_stack(sg_fr);
          }
          break;
        }
        default:
          dprintf("REALLY ABOLISHED %d\n", (int)sg_fr);
          abolish_incomplete_producer_subgoal(sg_fr);
          remove_subgoal_frame_from_stack(sg_fr);
          break;
      }
    }
  }
}

static inline void
abolish_dependency_frames_between(sg_fr_ptr specific_sg, choiceptr min, choiceptr max)
{
  dep_fr_ptr top = StackState_dep_fr(SgFr_stack_state((grounded_sf_ptr)specific_sg));
  dep_fr_ptr dep_fr;
  dprintf("min=%d max=%d\n", (int)min, (int)max);
  
  while(top && YOUNGER_CP(DepFr_cons_cp(top), min)) {
    dep_fr = top;
    top = DepFr_next(dep_fr);
    
    if(is_internal_dep_fr(specific_sg, dep_fr, min)) {
      REMOVE_DEP_FR_FROM_STACK_NEXT(dep_fr, top);
      dprintf("Removing consumer choice point %d\n", (int)DepFr_cons_cp(dep_fr));
      abolish_dependency_frame(dep_fr);
    }
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

/* for every generator that appears after 'limit' in the local stack
   that has 'subgoal' as the top generator change the top to 'new_top' */
static inline void
update_top_gen_sg_fields(sg_fr_ptr subgoal, choiceptr limit, sg_fr_ptr new_top)
{
  /* generator subgoal frames */
  sg_fr_ptr top_gen = LOCAL_top_sg_fr;
  while(top_gen && SgFr_choice_point(top_gen) <= limit) {
    if(SgFr_get_top_gen_sg(top_gen) == subgoal) {
      dprintf("Updated one top gen sg\n");
      SgFr_top_gen_sg(top_gen) = new_top;
    }
    top_gen = SgFr_next(top_gen);
  }
  
  /* dependency frames */
  dep_fr_ptr top_dep = LOCAL_top_dep_fr;
  while(top_dep && DepFr_cons_cp(top_dep) <= limit) {
    if(DepFr_get_top_gen_sg(top_dep) == subgoal) {
      dprintf("Update one top dep fr sg %d\n", (int)DepFr_cons_cp(top_dep));
      DepFr_top_gen_sg(top_dep) = new_top;
    }
    
    top_dep = DepFr_next(top_dep);
  }
}

static inline void
to_run_completed_node(sg_fr_ptr sg_fr, choiceptr cp)
{
  cp->cp_ap = (yamop *)RUN_COMPLETED;
  
  CONS_CP(cp)->cp_sg_fr = sg_fr;
  CONS_CP(cp)->cp_dep_fr = NULL;
}

static inline void
internal_producer_to_consumer(grounded_sf_ptr sg_fr, grounded_sf_ptr producer)
{
  dprintf("internal_producer_to_consumer\n");
  
  choiceptr gen_cp = SgFr_choice_point(sg_fr);
  choiceptr limit_cp = B->cp_b;
  choiceptr min = gen_cp;
  choiceptr max = B_FZ < limit_cp ? B_FZ : limit_cp;
  
  abolish_dependency_frames_between((sg_fr_ptr)sg_fr, min, max);
  abolish_generator_subgoals_between((sg_fr_ptr)sg_fr, min, max);
  
  adjust_generator_to_consumer_answer_template(gen_cp, (sg_fr_ptr)sg_fr);
  
  /* set last answer consumed to load answers */
  SgFr_try_answer(sg_fr) = SgFr_last_answer(sg_fr);
  
  /* update generator choice point to point to RUN_COMPLETED */
  to_run_completed_node((sg_fr_ptr)sg_fr, gen_cp);
  
  dprintf("set as local producer\n");
  SgFr_set_local_producer(producer);
  SgFr_set_local_consumer(sg_fr);
  
  B->cp_b = gen_cp;
}

static inline void
external_producer_to_consumer(grounded_sf_ptr sg_fr, grounded_sf_ptr producer)
{
  dprintf("external_producer_to_consumer\n");
  
  choiceptr gen_cp = SgFr_choice_point(sg_fr);
  choiceptr limit_cp = SgFr_new_answer_cp(sg_fr);
  choiceptr min = gen_cp;
  choiceptr max = SgFr_saved_max(sg_fr);

  dprintf("gen_cp->cp_ap %d COMPLETION %d\n", (int)gen_cp->cp_ap, (int)COMPLETION);
  if(gen_cp->cp_ap == COMPLETION || gen_cp->cp_ap == NULL)
    dprintf("to run completion\n");
  else
    dprintf("to not run completion\n");
  
  abolish_dependency_frames_between((sg_fr_ptr)sg_fr, min, max);
  abolish_generator_subgoals_between((sg_fr_ptr)sg_fr, min, max);
  /* update top generator subgoal */
  /* use min to include internal subgoal frames that have external consumers */
  update_top_gen_sg_fields((sg_fr_ptr)sg_fr, min, SgFr_top_gen_sg(sg_fr));
  
  adjust_generator_to_consumer_answer_template(gen_cp, (sg_fr_ptr)sg_fr);
  
  /* set last answer consumed for load answers */
  SgFr_try_answer(sg_fr) = SgFr_last_answer(sg_fr);

  if(SgFr_started(sg_fr)) {
      if(gen_cp->cp_ap == NULL) {
        /* generator already exhausted its clauses
           before reaching the general subgoal and tried to complete
           but failed.
           there is a change that this new-consumer could be
           lost when the dependency frame is created lazily,
         */
        add_dependency_frame((grounded_sf_ptr)sg_fr, gen_cp);
        dprintf("EAGER DEP FRAME\n");
      } else {
        /* update generator choice point to point to RUN_COMPLETED */
        gen_cp->cp_ap = (yamop *)RUN_COMPLETED;

        /* cp_dep_fr is only set when the generator was evaluating
         * using local scheduling, to be sure set as NULL */
        CONS_CP(gen_cp)->cp_dep_fr = NULL;
    
        if(gen_cp != limit_cp) {
          limit_cp->cp_b = gen_cp;
          limit_cp->cp_ap = TRUSTFAILCODE;
        }
        
        dprintf("Started\n");
      }
  } else {
    /* we are out of the execution path of the specific subgoal
       that means that it already has reached the completion operation
       and has attempted to complete, without much success
       -> create a dependency frame and turn this node into a consumer
    */
    dprintf("Getting a new dependency frame!\n");
    add_dependency_frame((grounded_sf_ptr)sg_fr, gen_cp);
  }
}

static inline int
is_internal_to_set(sg_fr_ptr pending, node_list_ptr all)
{
  while(all) {
    sg_fr_ptr sg_fr = (sg_fr_ptr)NodeList_node(all);
    if(sg_fr != pending &&
        is_internal_subgoal_frame(sg_fr, pending, SgFr_choice_point(pending)))
    {
      return TRUE;
    }
    all = NodeList_next(all);
  }
  
  return FALSE;
}

void
process_pending_subgoal_list(node_list_ptr list, grounded_sf_ptr sg_fr) {
  node_list_ptr orig = list;
  
  if(list == NULL)
    return;
    
#define REMOVE_PENDING_NODE() {                   \
    node_list_ptr next = NodeList_next(list);     \
    FREE_NODE_LIST(list);                         \
    if(before == NULL)                            \
      orig = next;                                \
    else                                          \
      NodeList_next(before) = next;               \
    list = next;                                  \
  }
    
  /* search specific subgoals that are running and the general is internal */
  choiceptr min_internal = NULL;
  node_list_ptr before = NULL;
  grounded_sf_ptr min_sg = NULL;
  
  while(list) {
    grounded_sf_ptr pending = (grounded_sf_ptr)NodeList_node(list);
    
    if(pending != sg_fr) {
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
        
        REMOVE_PENDING_NODE();
      } else if(SgFr_state(pending) == evaluating && SgFr_is_ground_producer(pending)) {
#ifdef FDEBUG
        printf("Found a specific subgoal already running: ");
        printSubgoalTriePath(stdout, SgFr_leaf(pending), SgFr_tab_ent(pending));
        printf("\n");
#endif
        SgFr_set_type(pending, GROUND_CONSUMER_SFT);
        SgFr_producer(pending) = sg_fr;
        ensure_has_proper_consumers(SgFr_tab_ent(sg_fr));
        
        if(SgFr_is_internal(pending)) {
          choiceptr min = SgFr_choice_point(pending);
          
          if(min_internal == NULL || min > min_internal) {
            min_internal = min;
            min_sg = pending;
            dprintf("found new min\n");
          }
          REMOVE_PENDING_NODE();
        } else {
          dprintf("skipping external node\n");
          /* skip this subgoal */
          before = list;
          list = NodeList_next(list);
        }
      } else {
        /* should not be here */
        REMOVE_PENDING_NODE();
      }
    } else {
      dprintf("removed self\n");
      /* remove self */
      REMOVE_PENDING_NODE();
    }
  }

  if(!min_internal && !orig) {
    /* no relevant nodes */
    dprintf("No relevant nodes\n");
    return;
  }

  if(min_internal && !orig) {
    /* only internal nodes */
    dprintf("Only internal nodes\n");
    internal_producer_to_consumer(min_sg, sg_fr);
    return;
  }
  
  if(min_internal) {
    /* filter external nodes that are internal to min_internal */
    before = NULL;
    list = orig;
    dprintf("Filtering external nodes for internal cp %d\n", (int)min_internal);
    
    while(list) {
      sg_fr_ptr pending = (sg_fr_ptr)NodeList_node(list);
      
      if(is_internal_subgoal_frame((sg_fr_ptr)min_sg, pending, min_internal)) {
        /* this subgoal wil be cut by min_sg */
        REMOVE_PENDING_NODE();
      } else {
        before = list;
        list = NodeList_next(list);
      }
    }
  }
  
  if(min_internal) {
    dprintf("doing a min internal\n");
    internal_producer_to_consumer(min_sg, sg_fr);
  }
  
  if(orig) {
    before = NULL;
    list = orig;
    node_list_ptr all = orig;
    dprintf("removing external internal of externals\n");
    
    /* look for external subgoals */
    while(list) {
      sg_fr_ptr pending = (sg_fr_ptr)NodeList_node(list);
      
      if(is_internal_to_set(pending, all)) {
        /* some specific subgoal already includes this subgoal */
        REMOVE_PENDING_NODE();
        dprintf("One internal to set deleted\n");
      } else {
        dprintf("not internal\n");
        before = list;
        list = NodeList_next(list);
      }
    }
    
    dprintf("prune external computations\n");
    list = orig;
    /* prune external computations */
    while(list) {
      grounded_sf_ptr pending = (grounded_sf_ptr)NodeList_node(list);
      
      external_producer_to_consumer(pending, sg_fr);

      list = NodeList_next(list);
    }
    
    free_node_list(orig);
  }
  
#undef REMOVE_PENDING_NODE
  
  dprintf("ok\n");
}

void
reinsert_subgoal_frame(sg_fr_ptr sg_fr, choiceptr new_cp)
{
  if(LOCAL_top_sg_fr == NULL) {
    LOCAL_top_sg_fr = sg_fr;
    SgFr_next(sg_fr) = NULL;
    SgFr_prev(sg_fr) = NULL;
  }

  sg_fr_ptr top = LOCAL_top_sg_fr;
  sg_fr_ptr before = NULL;

  while(top && YOUNGER_CP(SgFr_choice_point(top), new_cp)) {
    before = top;
    top = SgFr_next(top);
  }

  SgFr_prev(sg_fr) = before;
  if(before)
    SgFr_next(before) = sg_fr;
  if(top)
    SgFr_prev(top) = sg_fr;
  SgFr_next(sg_fr) = top;
}

static inline void
reinsert_dep_fr(dep_fr_ptr dep_fr, choiceptr cp)
{
  check_dependency_frame();
  dprintf("reinsert dep fr %d\n", (int)dep_fr);
  if(LOCAL_top_dep_fr == NULL) {
    LOCAL_top_dep_fr = dep_fr;
    DepFr_prev(dep_fr) = NULL;
    DepFr_next(dep_fr) = NULL;
  }
    
  dep_fr_ptr top = LOCAL_top_dep_fr;
  dep_fr_ptr before = NULL;
  
  while(top && YOUNGER_CP(DepFr_cons_cp(top), cp)) {
    dprintf("One dep_fr %d\n", (int)top);
    before = top;
    top = DepFr_next(top);
  }
  
  dprintf("set to next\n");
  if(before)
    DepFr_next(before) = dep_fr;
  if(top)
    DepFr_prev(top) = dep_fr;
  DepFr_prev(dep_fr) = before;
  DepFr_next(dep_fr) = top;
  
  check_dependency_frame();
}

void
add_dependency_frame(grounded_sf_ptr sg_fr, choiceptr cp)
{
  dep_fr_ptr dep_fr;
  grounded_sf_ptr producer = SgFr_producer(sg_fr);
  dprintf("add_dependency_frame leader=%d\n", (int)SgFr_choice_point(producer));
  
  new_dependency_frame(dep_fr, TRUE, LOCAL_top_or_fr, SgFr_choice_point(producer), cp, (sg_fr_ptr)sg_fr, NULL);
  reinsert_dep_fr(dep_fr, cp);
  SgFr_num_deps(sg_fr)++;
  
  /* turn generator choice point as consumer */
  CONS_CP(cp)->cp_dep_fr = dep_fr;
  CONS_CP(cp)->cp_sg_fr = NULL;
  if(SgFr_try_answer(sg_fr))
    DepFr_last_answer(dep_fr) = SgFr_try_answer(sg_fr);
  if(SgFr_is_ground_local_producer(producer))
    DepFr_set_top_consumer(dep_fr);
  
  cp->cp_ap = ANSWER_RESOLUTION;

  dprintf("add_dependency_frame finished\n");
}

void
reinsert_dependency_frame(dep_fr_ptr dep_fr)
{
  dprintf("reinsert_dependency_frame\n");
  reinsert_dep_fr(dep_fr, DepFr_cons_cp(dep_fr));
}

void
remove_from_restarted_gens(choiceptr cp)
{
  node_list_ptr top = LOCAL_restarted_gens;
  node_list_ptr before = NULL;

  if(!top)
    return;

  while(top && EQUAL_OR_YOUNGER_CP(SgFr_choice_point((sg_fr_ptr)NodeList_node(top)), cp))
  {
    sg_fr_ptr sg_fr = (sg_fr_ptr)NodeList_node(top);

    if(SgFr_choice_point(sg_fr) == cp) {
      dprintf("Removing restarted gen: %d\n", (int)cp);
      if(before)
        NodeList_next(before) = NodeList_next(top);
      else
        LOCAL_restarted_gens = NodeList_next(top);
      FREE_NODE_LIST(top);
      return;
    }

    before = top;
    top = NodeList_next(top);
  }

  dprintf("remove from restarted gens\n");
}

void
remove_sg_fr_from_restarted_gens(sg_fr_ptr sg_fr)
{
  dprintf("remove_sg_fr_from_restarted_gens\n");
  remove_from_restarted_gens(SgFr_choice_point(sg_fr));
}

void
transform_node_into_loader(choiceptr cp, sg_fr_ptr sg_fr,
    continuation_ptr last, yamop *load_instr)
{
  memmove(LOADER_ANSWER_TEMPLATE(cp), GENERATOR_ANSWER_TEMPLATE(cp, sg_fr),
      (1 + SgFr_arity(sg_fr)) * sizeof(CELL));
  LOAD_CP(cp)->cp_last_answer = last;
  cp->cp_ap = load_instr;
}

#endif /* TABLING_GROUNDED */
