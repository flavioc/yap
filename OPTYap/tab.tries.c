/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   R. Rocha and NCC - University of Porto, Portugal
  File:        tab.tries.C
  version:     $Id: tab.tries.c,v 1.24 2008-05-20 18:25:37 ricroc Exp $
                                                                     
**********************************************************************/

/* ------------------ **
**      Includes      **
** ------------------ */

#include "Yap.h"
#ifdef TABLING
#include <stdio.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#include "Yatom.h"
#include "YapHeap.h"
#include "yapio.h"
#include "tab.utils.h"
#include "tab.macros.h"
#include "tab.tries.h"

/* ------------------------------------- **
**      Local functions declaration      **
** ------------------------------------- */

#ifdef YAPOR
#ifdef TABLING_INNER_CUTS
static int update_answer_trie_branch(ans_node_ptr previous_node, ans_node_ptr current_node);
#else
static int update_answer_trie_branch(ans_node_ptr current_node);
#endif /* TABLING_INNER_CUTS */
#else
static void update_answer_trie_branch(ans_node_ptr current_node, int position);
#endif /* YAPOR */
static void traverse_subgoal_trie(sg_node_ptr current_node, char *str, int str_index, int *arity, int mode, int position, tab_ent_ptr tab_ent);
static void traverse_answer_trie(ans_node_ptr current_node, char *str, int str_index, int *arity, int var_index, int mode, int position);
static void traverse_trie_node(void *node, char *str, int *str_index_ptr, int *arity, int *mode_ptr, int type);
#ifdef GLOBAL_TRIE
static void free_global_trie_branch(gt_node_ptr current_node);
static void traverse_global_trie(gt_node_ptr current_node, char *str, int str_index, int *arity, int mode, int position);
static void traverse_global_trie_for_subgoal(gt_node_ptr current_node, char *str, int *str_index, int *arity, int *mode);
static void traverse_global_trie_for_answer(gt_node_ptr current_node, char *str, int *str_index, int *arity, int *mode);
#endif /* GLOBAL_TRIE */
#ifdef TABLING_CALL_SUBSUMPTION
static void ground_trie_statistics(tab_ent_ptr tab_ent);
#endif /* TABLING_CALL_SUBSUMPTION */

/* -------------------------- **
**      Global functions      **
** -------------------------- */

sg_fr_ptr subgoal_search(yamop *preg, CELL **local_stack_ptr)
{
  tab_ent_ptr tab_ent = CODE_TABLE_ENTRY(preg);
  CELL *local_stack = *local_stack_ptr - 1;
  sg_fr_ptr sg_fr = NULL;
  
#ifdef TABLE_LOCK_AT_ENTRY_LEVEL
  LOCK(TabEnt_lock(tab_ent));
#endif /* TABLE_LOCK_LEVEL */

  if(TabEnt_subgoal_trie(tab_ent) == NULL) {
    sg_node_ptr root;
#ifdef TABLING_CALL_SUBSUMPTION
    if(TabEnt_is_variant(tab_ent)) {
      new_root_subgoal_trie_node(root);
    } else {
      new_root_sub_subgoal_trie_node(root);
    }
#else
    new_root_subgoal_trie_node(root);
#endif /* TABLING_CALL_SUBSUMPTION */
    TabEnt_subgoal_trie(tab_ent) = root;
  }
  
#ifdef FDEBUG
  dprintf("subgoal_search for ");
  printCalledSubgoal(stdout, preg);
  dprintf("\n");
#endif
  
#ifdef TABLING_CALL_SUBSUMPTION
  if(TabEnt_is_variant(tab_ent)) {
    sg_fr = variant_call_search(preg, local_stack, local_stack_ptr);
  } else if(TabEnt_is_subsumptive(tab_ent)) {
    sg_fr = subsumptive_call_search(preg, local_stack, local_stack_ptr);
  } else if(TabEnt_is_grounded(tab_ent)) {
    sg_fr = grounded_call_search(preg, local_stack, local_stack_ptr);
  }
#else
  sg_fr = variant_call_search(preg, local_stack, local_stack_ptr);
#endif /* TABLING_CALL_SUBSUMPTION */
  
#ifdef FDEBUG
  dprintf("SUBGOAL IS: ");
  printSubgoalTriePath(stdout, SgFr_leaf(sg_fr), tab_ent);
  printf("\n");
#endif

  return sg_fr;
}

ans_node_ptr answer_search(sg_fr_ptr sg_fr, CELL *subs_ptr) {
  if(SgFr_is_variant(sg_fr))
    return variant_answer_search(sg_fr, subs_ptr);
#ifdef TABLING_CALL_SUBSUMPTION
  else if(SgFr_is_sub_producer(sg_fr)) {
    int nTerms = *subs_ptr;
    CELL* answerVector = subs_ptr + nTerms;
    return (ans_node_ptr)subsumptive_answer_search((subprod_fr_ptr)sg_fr, nTerms, answerVector);
  } else if(SgFr_is_ground_producer(sg_fr)) {
    int nTerms = *subs_ptr;
    CELL* answerVector = subs_ptr + nTerms;
    return (ans_node_ptr)grounded_answer_search((grounded_sf_ptr)sg_fr, answerVector);
  }
#endif /* TABLING_CALL_SUBSUMPTION */
  return NULL;
}

#ifdef TABLING_CALL_SUBSUMPTION
void delete_subgoal_path(sg_fr_ptr sg_fr) {
  sg_node_ptr node = SgFr_leaf(sg_fr);
  sg_node_ptr parent, first_child, old_node, *bucket;
  sg_hash_ptr hash = NULL;
  int update_generators = TrNode_is_sub_call(node) && TrNode_num_gen((subg_node_ptr)node) > 0;

  TrNode_child(node) = NULL;
  
  while(!TrNode_is_root(node) && TrNode_child(node) == NULL) {
    
    parent = TrNode_parent(node);
    first_child = TrNode_child(parent);

    if(IS_SUBGOAL_TRIE_HASH(first_child)) {
      hash = (sg_hash_ptr) first_child;
      bucket = Hash_bucket(hash, HASH_ENTRY(TrNode_entry(node), Hash_seed(hash)));

      --Hash_num_nodes(hash);
      
      first_child = *bucket;
      if(first_child == node) {
        *bucket = TrNode_next(first_child);
        goto process_next;
      }
    } else {
      /* simple linked list */
      if(first_child == node) {
        TrNode_child(parent) = TrNode_next(node);
        goto process_next;
      }
    }
    
    /* node is somewhere in the middle of the linked list */
    while(TrNode_next(first_child) != node)
      first_child = TrNode_next(first_child);
    TrNode_next(first_child) = TrNode_next(node);
  
process_next:
    
    if(hash) {
      if(Hash_num_nodes(hash) == 0) {
        /* hash without nodes! */
        TrNode_child(parent) = NULL;
        free_subgoal_trie_hash(hash);
        dprintf("FREE SUBGOAL TRIE HASH\n");
      } else if(update_generators)
        /* if we got here then the index exists and GNIN_num_gen(index) == 1 */
        gen_index_remove((subg_node_ptr)node, (subg_hash_ptr)hash);
    }

    old_node = node;
    node = parent;
    free_subgoal_trie_node(old_node);
    
    hash = NULL;
  }
  
  if(update_generators)
    decrement_generator_path(node);
}
#endif /* TABLING_CALL_SUBSUMPTION */

#ifdef GLOBAL_TRIE
CELL *load_substitution_variable(gt_node_ptr current_node, CELL *aux_stack_ptr) {
  CELL *subs_ptr, *stack_terms_top, *stack_terms_base, *stack_terms;
  int vars_arity;
  Term t;
#ifdef TRIE_COMPACT_PAIRS
  int stack_terms_pair_offset = 0;
#endif /* TRIE_COMPACT_PAIRS */

  vars_arity = (int) *aux_stack_ptr;
  stack_terms_top = (CELL *) TR;
  stack_terms_base = stack_terms = (CELL *) Yap_TrailTop;
  t = TrNode_entry(current_node);
  current_node = TrNode_parent(current_node);
  do {
    if (IsVarTerm(t)) {
      int var_index = VarIndexOfTableTerm(t);
      t = MkVarTerm();
      if (var_index >= vars_arity) {
	while (vars_arity < var_index) {
	  *aux_stack_ptr-- = 0;
	  vars_arity++;
	}
	*aux_stack_ptr-- = t;
	vars_arity++;
	*aux_stack_ptr = vars_arity;
      } else {
	/* do the same as in macro stack_trie_val_instr() */
	CELL aux_sub, aux_var, *vars_ptr;
	vars_ptr = aux_stack_ptr + vars_arity - var_index;
	aux_sub = *((CELL *) t);
	aux_var = *vars_ptr;
	if (aux_var == 0) {
	  *vars_ptr = t;
	} else {
	  if (aux_sub > aux_var) {
	    if ((CELL *) aux_sub <= H) {
	      Bind_Global((CELL *) aux_sub, aux_var);
	    } else if ((CELL *) aux_var <= H) {
	      Bind_Local((CELL *) aux_sub, aux_var);
	    } else {
	      Bind_Local((CELL *) aux_var, aux_sub);
              *vars_ptr = aux_sub;
	    }
	  } else {            
	    if ((CELL *) aux_var <= H) {
	      Bind_Global((CELL *) aux_var, aux_sub);
              *vars_ptr = aux_sub;
	    } else if ((CELL *) aux_sub <= H) {
	      Bind_Local((CELL *) aux_var, aux_sub);
              *vars_ptr = aux_sub;
	    } else {
	      Bind_Local((CELL *) aux_sub, aux_var);
	    }
	  }
	}
      }
      STACK_CHECK_EXPAND(stack_terms, stack_terms_top, stack_terms_base);
      STACK_PUSH_UP(t, stack_terms);
    } else if (IsAtomOrIntTerm(t)) {
      STACK_CHECK_EXPAND(stack_terms, stack_terms_top, stack_terms_base);
      STACK_PUSH_UP(t, stack_terms);
    } else if (IsPairTerm(t)) {
#ifdef TRIE_COMPACT_PAIRS
      if (t == CompactPairInit) {
	Term *stack_aux = stack_terms_base - stack_terms_pair_offset;
	Term head, tail = STACK_POP_UP(stack_aux);
	while (STACK_NOT_EMPTY(stack_aux, stack_terms)) {
	  head = STACK_POP_UP(stack_aux);
	  tail = MkPairTerm(head, tail);
	}
	stack_terms = stack_terms_base - stack_terms_pair_offset;
	stack_terms_pair_offset = (int) STACK_POP_DOWN(stack_terms);
	STACK_PUSH_UP(tail, stack_terms);
      } else {  /* CompactPairEndList / CompactPairEndTerm */
	Term last;
	STACK_CHECK_EXPAND(stack_terms, stack_terms_top + 1, stack_terms_base);
	last = STACK_POP_DOWN(stack_terms);
	STACK_PUSH_UP(stack_terms_pair_offset, stack_terms);
	stack_terms_pair_offset = (int) (stack_terms_base - stack_terms);
	if (t == CompactPairEndList)
	  STACK_PUSH_UP(TermNil, stack_terms);
	STACK_PUSH_UP(last, stack_terms);
      }
#else
      Term head = STACK_POP_DOWN(stack_terms);
      Term tail = STACK_POP_DOWN(stack_terms);
      t = MkPairTerm(head, tail);
      STACK_PUSH_UP(t, stack_terms);
#endif /* TRIE_COMPACT_PAIRS */
    } else if (IsApplTerm(t)) {
      Functor f = (Functor) RepAppl(t);
      if (f == FunctorDouble) {
	volatile Float dbl;
	volatile Term *t_dbl = (Term *)((void *) &dbl);
	t = TrNode_entry(current_node);
	current_node = TrNode_parent(current_node);
	*t_dbl = t;
#if SIZEOF_DOUBLE == 2 * SIZEOF_INT_P
	t = TrNode_entry(current_node);
	current_node = TrNode_parent(current_node);
	*(t_dbl + 1) = t;
#endif /* SIZEOF_DOUBLE x SIZEOF_INT_P */
	current_node = TrNode_parent(current_node);
	t = MkFloatTerm(dbl);
	STACK_CHECK_EXPAND(stack_terms, stack_terms_top, stack_terms_base);
	STACK_PUSH_UP(t, stack_terms);
      } else if (f == FunctorLongInt) {
	Int li = TrNode_entry(current_node);
	current_node = TrNode_parent(current_node);
	current_node = TrNode_parent(current_node);
	t = MkLongIntTerm(li);
	STACK_CHECK_EXPAND(stack_terms, stack_terms_top, stack_terms_base);
	STACK_PUSH_UP(t, stack_terms);
      } else {
	int f_arity = ArityOfFunctor(f);
	t = Yap_MkApplTerm(f, f_arity, stack_terms);
	stack_terms += f_arity;
	STACK_CHECK_EXPAND(stack_terms, stack_terms_top, stack_terms_base);
	STACK_PUSH_UP(t, stack_terms);
      }
    }
    t = TrNode_entry(current_node);
    current_node = TrNode_parent(current_node);
  } while (current_node);

  subs_ptr = aux_stack_ptr + vars_arity + 1;
  *subs_ptr = *subs_ptr - 1;
  subs_ptr += *subs_ptr + 1;
  t = STACK_POP_DOWN(stack_terms);
  Bind((CELL *) *subs_ptr, t);

#ifdef TABLING_ERRORS
  if (stack_terms != (CELL *)Yap_TrailTop)
    TABLING_ERROR_MESSAGE("stack_terms != Yap_TrailTop (load_substitution_variable)");
#endif /* TABLING_ERRORS */

  return aux_stack_ptr;
}
#endif /* GLOBAL_TRIE */


void private_completion(sg_fr_ptr sg_fr) {
  dprintf("Complete by choice point %d\n", (int)B);
  /* complete generator subgoals */
#ifdef LIMIT_TABLING
  sg_fr_ptr aux_sg_fr;
  while (LOCAL_top_sg_fr != sg_fr) {
    aux_sg_fr = LOCAL_top_sg_fr;
    LOCAL_top_sg_fr = SgFr_next(aux_sg_fr);
    mark_as_completed(aux_sg_fr);
    insert_into_global_sg_fr_list(aux_sg_fr);
  }
  aux_sg_fr = LOCAL_top_sg_fr;
  LOCAL_top_sg_fr = SgFr_next(aux_sg_fr);
  mark_as_completed(aux_sg_fr);
  insert_into_global_sg_fr_list(aux_sg_fr);
#else
  while (LOCAL_top_sg_fr != sg_fr) {
    mark_as_completed(LOCAL_top_sg_fr);
    dprintf("One top sg fr completed\n");
    LOCAL_top_sg_fr = SgFr_next(LOCAL_top_sg_fr);
  }
  mark_as_completed(LOCAL_top_sg_fr);
  LOCAL_top_sg_fr = SgFr_next(LOCAL_top_sg_fr);
#endif /* LIMIT_TABLING */

#ifdef TABLING_CALL_SUBSUMPTION
  /* complete subsumptive consumer subgoals */
  while(LOCAL_top_subcons_sg_fr && YOUNGER_CP(SgFr_choice_point(LOCAL_top_subcons_sg_fr), B)) {
    mark_subsumptive_consumer_as_completed(LOCAL_top_subcons_sg_fr);
    dprintf("One subsumptive consumer completed\n");
    LOCAL_top_subcons_sg_fr = SgFr_next(LOCAL_top_subcons_sg_fr);
  }
  
  /* complete ground consumer subgoals */
  while(LOCAL_top_groundcons_sg_fr && YOUNGER_CP(SgFr_choice_point(LOCAL_top_groundcons_sg_fr), B)) {
    mark_ground_consumer_as_completed(LOCAL_top_groundcons_sg_fr);
    dprintf("One ground consumer completed\n");
    LOCAL_top_groundcons_sg_fr = SgFr_next(LOCAL_top_groundcons_sg_fr);
  }
#endif /* TABLING_CALL_SUBSUMPTION */

  /* release dependency frames */
  while (EQUAL_OR_YOUNGER_CP(DepFr_cons_cp(LOCAL_top_dep_fr), B)) {  /* never equal if batched scheduling */
    dep_fr_ptr dep_fr = DepFr_next(LOCAL_top_dep_fr);
    dprintf("ONE DEPENDENCY FRAME FREED\n");
    FREE_DEPENDENCY_FRAME(LOCAL_top_dep_fr);
    LOCAL_top_dep_fr = dep_fr;
  }
  
  /* adjust freeze registers */
  adjust_freeze_registers();

  return;
}


#ifdef GLOBAL_TRIE
void free_subgoal_trie_branch(sg_node_ptr current_node, int nodes_left, int position) {
  if (nodes_left != 1)
    free_subgoal_trie_branch(TrNode_child(current_node), nodes_left - 1, TRAVERSE_POSITION_FIRST);
#else
void free_subgoal_trie_branch(sg_node_ptr current_node, int nodes_left, int nodes_extra, int position) {
  int current_nodes_left = 0, current_nodes_extra = 0;

  /* save current state if first sibling node */
  if (position == TRAVERSE_POSITION_FIRST) {
    current_nodes_left = nodes_left;
    current_nodes_extra = nodes_extra;
  }

  if (nodes_extra) {
#ifdef TRIE_COMPACT_PAIRS
    if (nodes_extra < 0) {
      Term t = TrNode_entry(current_node);
      if (IsPairTerm(t)) {
	if (t == CompactPairInit)
	  nodes_extra--;
	else  /* CompactPairEndList / CompactPairEndTerm */
	  nodes_extra++;
      }
    } else 
#endif /* TRIE_COMPACT_PAIRS */
    if (--nodes_extra == 0)
      nodes_left--;
  } else {
    Term t = TrNode_entry(current_node);
    if (IsVarTerm(t) || IsAtomOrIntTerm(t))
      nodes_left--;
    else if (IsPairTerm(t))
#ifdef TRIE_COMPACT_PAIRS
      /* CompactPairInit */
      nodes_extra = -1;
#else
      nodes_left++;
#endif /* TRIE_COMPACT_PAIRS */
    else if (IsApplTerm(t)) {
      Functor f = (Functor) RepAppl(t);
      if(f != FunctorDouble && f != FunctorLongInt) {
        nodes_left += ArityOfFunctor(f) - 1;
      } else {
        nodes_left--;
      }
    }
  }
  if (nodes_left)
    free_subgoal_trie_branch(TrNode_child(current_node), nodes_left, nodes_extra, TRAVERSE_POSITION_FIRST);
#endif /* GLOBAL_TRIE */
  else {
    sg_fr_ptr sg_fr = (sg_fr_ptr) TrNode_sg_fr(current_node);
    
#ifdef LIMIT_TABLING
    remove_from_global_sg_fr_list(sg_fr);
#endif /* LIMIT_TABLING */
    
    /* delete this subgoal data structures */
    switch(SgFr_type(sg_fr)) {
      case VARIANT_PRODUCER_SFT:
        free_variant_subgoal_data(sg_fr, TRUE);
        FREE_VARIANT_SUBGOAL_FRAME(sg_fr);
        break;
#ifdef TABLING_CALL_SUBSUMPTION
      case SUBSUMPTIVE_PRODUCER_SFT:
        free_producer_subgoal_data(sg_fr, TRUE);
        FREE_SUBPROD_SUBGOAL_FRAME(sg_fr);
        break;
      case SUBSUMED_CONSUMER_SFT:
        free_consumer_subgoal_data((subcons_fr_ptr)sg_fr);
        FREE_SUBCONS_SUBGOAL_FRAME(sg_fr);
        break;
      case GROUND_PRODUCER_SFT:
      case GROUND_CONSUMER_SFT:
        free_ground_subgoal_data((grounded_sf_ptr)sg_fr);
        FREE_GROUNDED_SUBGOAL_FRAME(sg_fr);
        break;
#endif /* TABLING_CALL_SUBSUMPTION */
      default:
#ifdef TABLING_ERRORS
        TABLING_ERROR_MESSAGE("unrecognized subgoal frame type (free_subgoal_trie_branch)");
#endif
        break;
    }
  }

  if (position == TRAVERSE_POSITION_FIRST) {
    sg_node_ptr next_node = TrNode_next(current_node);
    DECREMENT_GLOBAL_TRIE_REFS(TrNode_entry(current_node));
    free_subgoal_trie_node(current_node);
#ifndef GLOBAL_TRIE
    /* restore the initial state */
    nodes_left = current_nodes_left;
    nodes_extra = current_nodes_extra;
#endif /* GLOBAL_TRIE */
    while (next_node) {
      current_node = next_node;
      next_node = TrNode_next(current_node);
#ifdef GLOBAL_TRIE
      free_subgoal_trie_branch(current_node, nodes_left, TRAVERSE_POSITION_NEXT);
#else
      free_subgoal_trie_branch(current_node, nodes_left, nodes_extra, TRAVERSE_POSITION_NEXT);
#endif /* GLOBAL_TRIE */
    }
  } else {
    DECREMENT_GLOBAL_TRIE_REFS(TrNode_entry(current_node));
    free_subgoal_trie_node(current_node);
  }
  return;
}


void free_answer_trie_branch(ans_node_ptr current_node, int position) {
#ifdef TABLING_INNER_CUTS
  if (TrNode_child(current_node) && ! IS_ANSWER_LEAF_NODE(current_node))
#else
  if (! IS_ANSWER_LEAF_NODE(current_node))
#endif /* TABLING_INNER_CUTS */
    free_answer_trie_branch(TrNode_child(current_node), TRAVERSE_POSITION_FIRST);

  if (position == TRAVERSE_POSITION_FIRST) {
    ans_node_ptr next_node = TrNode_next(current_node);
    DECREMENT_GLOBAL_TRIE_REFS(TrNode_entry(current_node));
    free_answer_trie_node(current_node);
    while (next_node) {
      current_node = next_node;
      next_node = TrNode_next(current_node);
      free_answer_trie_branch(current_node, TRAVERSE_POSITION_NEXT);
    }
  } else {
    DECREMENT_GLOBAL_TRIE_REFS(TrNode_entry(current_node));
    free_answer_trie_node(current_node);
  }
  return;
}


void update_answer_trie(sg_fr_ptr sg_fr) {
  /* complete --> compiled : complete_in_use --> compiled_in_use */
  SgFr_state(sg_fr) += 2;
  
  /* compile only with variants */
  if(SgFr_is_variant(sg_fr) || SgFr_is_sub_producer(sg_fr)) {
    ans_node_ptr current_node = TrNode_child(SgFr_answer_trie(sg_fr));
    if (current_node) {
#ifdef YAPOR
      TrNode_instr(current_node) -= 1;
#ifdef TABLING_INNER_CUTS
      update_answer_trie_branch(NULL, current_node);
#else
      update_answer_trie_branch(current_node);
#endif /* TABLING_INNER_CUTS */
#else /* TABLING */
      update_answer_trie_branch(current_node, TRAVERSE_POSITION_FIRST);
#endif /* YAPOR */
    }
  }
  
  return;
}


static struct trie_statistics{
  int show;
  long subgoals;
  long sub_consumer_subgoals;
  long subgoals_incomplete;
  long subgoal_trie_nodes;
  long long_subgoal_trie_nodes;
  long float_subgoal_trie_nodes;
  long subgoal_hash;
  long hash_buckets;
  long answers;
#ifdef TABLING_CALL_SUBSUMPTION
  long subgoal_indexes;
  long tst_indexes;
#endif /* TABLING_CALL_SUBSUMPTION */
#ifdef TABLING_INNER_CUTS
  long answers_pruned;
#endif /* TABLING_INNER_CUTS */
  long answers_true;
  long answers_no;
  long answer_trie_nodes;
  long float_answer_trie_nodes;
  long long_answer_trie_nodes;
  long answers_trie_hash;
#ifdef GLOBAL_TRIE
  long global_trie_terms;
  long global_trie_nodes;
#endif /* GLOBAL_TRIE */
} trie_stats;

#define TrStat_show              trie_stats.show
#define TrStat_subgoals          trie_stats.subgoals
#define TrStat_sg_incomplete     trie_stats.subgoals_incomplete
#define TrStat_sg_nodes          trie_stats.subgoal_trie_nodes
#define TrStat_float_sg_nodes    trie_stats.float_subgoal_trie_nodes
#define TrStat_long_sg_nodes     trie_stats.long_subgoal_trie_nodes
#define TrStat_answers           trie_stats.answers
#define TrStat_answers_true      trie_stats.answers_true
#define TrStat_answers_no        trie_stats.answers_no
#define TrStat_answers_pruned    trie_stats.answers_pruned
#define TrStat_ans_nodes         trie_stats.answer_trie_nodes
#define TrStat_float_ans_nodes   trie_stats.float_answer_trie_nodes
#define TrStat_long_ans_nodes    trie_stats.long_answer_trie_nodes
#define TrStat_gt_terms          trie_stats.global_trie_terms
#define TrStat_gt_nodes          trie_stats.global_trie_nodes
#define TrStat_sg_hash           trie_stats.subgoal_hash
#define TrStat_hash_buckets      trie_stats.hash_buckets
#define TrStat_sg_indexes        trie_stats.subgoal_indexes
#define TrStat_subcons_subgoals  trie_stats.sub_consumer_subgoals
#define TrStat_tst_indexes       trie_stats.tst_indexes
#define TrStat_ans_hash          trie_stats.answers_trie_hash

#define SHOW_TABLE_STRUCTURE(MESG, ARGS...)  if (TrStat_show == SHOW_MODE_STRUCTURE) fprintf(Yap_stdout, MESG, ##ARGS)
#define STR_ARRAY_SIZE  100000
#define ARITY_ARRAY_SIZE 10000


void show_table(tab_ent_ptr tab_ent, int show_mode) {
  sg_node_ptr sg_node;

  TrStat_show = show_mode;
  
  if (show_mode == SHOW_MODE_STATISTICS) {
    TrStat_subgoals = 0;
    TrStat_sg_incomplete = 0;
    TrStat_sg_nodes = 0;
    TrStat_answers = 0;
    TrStat_float_sg_nodes = 0;
    TrStat_long_sg_nodes = 0;
    TrStat_sg_hash = 0;
    TrStat_hash_buckets = 0;
#ifdef TABLING_CALL_SUBSUMPTION
    TrStat_sg_indexes = 0;
    TrStat_tst_indexes = 0;
#endif /* TABLING_CALL_SUBSUMPTION */
    TrStat_answers_true = 0;
    TrStat_answers_no = 0;
#ifdef TABLING_INNER_CUTS
    TrStat_answers_pruned = 0;
#endif /* TABLING_INNER_CUTS */
    TrStat_ans_nodes = 0;
    TrStat_float_ans_nodes = 0;
    TrStat_long_sg_nodes = 0;
    TrStat_ans_hash = 0;
  }
  
  fprintf(Yap_stdout, "Table %s for predicate '%s/%d'\n",
    (show_mode == SHOW_MODE_STATISTICS ? "statistics" : "structure"),
    AtomName(TabEnt_atom(tab_ent)), TabEnt_arity(tab_ent));
    
  sg_node = TabEnt_subgoal_trie(tab_ent);
  if (sg_node && TrNode_child(sg_node)) {
    sg_node = TrNode_child(sg_node);
    if (TabEnt_arity(tab_ent) > 0) {
      char *str = (char *) malloc(sizeof(char) * STR_ARRAY_SIZE);
      int str_index = sprintf(str, "  ?- %s(", AtomName(TabEnt_atom(tab_ent)));
      int *arity = (int *) malloc(sizeof(int) * ARITY_ARRAY_SIZE);
      arity[0] = 1;
      arity[1] = TabEnt_arity(tab_ent);
      traverse_subgoal_trie(sg_node, str, str_index, arity, TRAVERSE_MODE_NORMAL, TRAVERSE_POSITION_FIRST, tab_ent);
      free(str);
      free(arity);
    } else if(TabEnt_arity(tab_ent) == 0) {
      sg_fr_ptr sg_fr = (sg_fr_ptr) sg_node;
      TrStat_subgoals++;
      SHOW_TABLE_STRUCTURE("  ?- %s.\n", AtomName(TabEnt_atom(tab_ent)));
      TrStat_ans_nodes++;

      if (SgFr_has_no_answers(sg_fr)) {
	      if (SgFr_state(sg_fr) < complete) {
	        TrStat_sg_incomplete++;
	        SHOW_TABLE_STRUCTURE("    ---> INCOMPLETE\n");
	      } else {
	        TrStat_answers_no++;
	        SHOW_TABLE_STRUCTURE("    NO\n");
	      }
      } else {  /* yes answer */
	      TrStat_answers_true++;
	      SHOW_TABLE_STRUCTURE("    TRUE\n");
      }
    }
  } else
    SHOW_TABLE_STRUCTURE("  EMPTY\n");
    
  if (show_mode == SHOW_MODE_STATISTICS) {
    long bytes = 0;
    
    bytes += sizeof(struct table_entry);
    
    if(TabEnt_subgoal_trie(tab_ent))
      TrStat_sg_nodes++;

    if(TabEnt_is_variant(tab_ent)) {
      bytes += TrStat_sg_nodes * sizeof(struct subgoal_trie_node);
      bytes += TrStat_float_sg_nodes * sizeof(struct float_subgoal_trie_node);
      bytes += TrStat_long_sg_nodes * sizeof(struct long_subgoal_trie_node);
      bytes += TrStat_sg_hash * sizeof(struct subgoal_trie_hash);
      bytes += TrStat_subgoals * sizeof(struct subgoal_frame);
      bytes += TrStat_ans_nodes * sizeof(struct answer_trie_node);
      bytes += TrStat_ans_hash * sizeof(struct answer_trie_hash);
#ifdef TABLING_CALL_SUBSUMPTION
    } else {
      if(TabEnt_is_grounded(tab_ent)) {
        bytes += TrStat_subgoals * sizeof(struct grounded_subgoal_frame);
        ground_trie_statistics(tab_ent);
      } else {
        bytes += TrStat_subcons_subgoals * sizeof(struct subsumed_consumer_subgoal_frame);
        bytes += TrStat_subgoals * sizeof(struct subsumptive_producer_subgoal_frame);
        if(TabEnt_completed(tab_ent))
          ground_trie_statistics(tab_ent);
      }
      
      bytes += TrStat_sg_nodes * sizeof(struct sub_subgoal_trie_node);
      bytes += TrStat_float_sg_nodes * sizeof(struct float_sub_subgoal_trie_node);
      bytes += TrStat_long_sg_nodes * sizeof(struct long_sub_subgoal_trie_node);
      bytes += TrStat_sg_hash * sizeof(struct sub_subgoal_trie_hash);
      bytes += TrStat_sg_indexes * sizeof(struct gen_index_node);
      bytes += TrStat_ans_nodes * sizeof(struct time_stamped_trie_node);
      bytes += TrStat_float_ans_nodes * sizeof(struct float_time_stamped_trie_node);
      bytes += TrStat_long_ans_nodes * sizeof(struct long_time_stamped_trie_node);
      bytes += TrStat_ans_hash * sizeof(struct tst_answer_trie_hash);
      bytes += TrStat_tst_indexes * sizeof(struct tst_index_node);
#endif /* TABLING_CALL_SUBSUMPTION */
    }

    bytes += TrStat_hash_buckets * sizeof(void *);

#ifdef TABLING_ANSWER_CHILD
    /* do nothing */
#elif defined(TABLING_ANSWER_LIST)
    bytes += TrStat_answers * sizeof(struct node_list);
#else /* TABLING_ANSWER_BLOCKS */
    bytes += ((TrStat_answers + ANSWER_BLOCK_SIZE) / ANSWER_BLOCK_SIZE) * (sizeof(void *) * (ANSWER_BLOCK_SIZE + 1));
#endif /* TABLING_ANSWER_CHILD */
    
    fprintf(Yap_stdout, "  Subgoal trie structure\n");
    fprintf(Yap_stdout, "    Subgoals: %ld (%ld incomplete)\n", TrStat_subgoals + TrStat_subcons_subgoals, TrStat_sg_incomplete);
    fprintf(Yap_stdout, "    Subgoal trie nodes: %ld\n", TrStat_sg_nodes + TrStat_float_sg_nodes + TrStat_long_sg_nodes);
    fprintf(Yap_stdout, "  Answer trie structure(s)\n");
#ifdef TABLING_INNER_CUTS
    fprintf(Yap_stdout, "    Answers: %ld (%ld pruned)\n", TrStat_answers, TrStat_answers_pruned);
#else
    fprintf(Yap_stdout, "    Answers: %ld\n", TrStat_answers);
#endif /* TABLING_INNER_CUTS */
    fprintf(Yap_stdout, "    Answers 'TRUE': %ld\n", TrStat_answers_true);
    fprintf(Yap_stdout, "    Answers 'NO': %ld\n", TrStat_answers_no);
    fprintf(Yap_stdout, "    Answer trie nodes: %ld\n", TrStat_ans_nodes + TrStat_float_ans_nodes + TrStat_long_ans_nodes);

    /* XXX */
    fprintf(Yap_stdout, "  Total memory in use: %ld bytes\n", bytes);
  }
  return;
}


#ifdef GLOBAL_TRIE
void show_global_trie(void) {
  TrStat_show = SHOW_MODE_STRUCTURE;
  TrStat_gt_terms = 0;
  TrStat_gt_nodes = 1;
  fprintf(Yap_stdout, "Global trie structure\n");
  if (TrNode_child(GLOBAL_root_gt)) {
    char *str = (char *) malloc(sizeof(char) * STR_ARRAY_SIZE);
    int *arity = (int *) malloc(sizeof(int) * ARITY_ARRAY_SIZE);
    arity[0] = 0;
    traverse_global_trie(TrNode_child(GLOBAL_root_gt), str, 0, arity, TRAVERSE_MODE_NORMAL, TRAVERSE_POSITION_FIRST);
    free(str);
    free(arity);
  } else
    SHOW_TABLE_STRUCTURE("  EMPTY\n");
  fprintf(Yap_stdout, "Global trie statistics\n");
  fprintf(Yap_stdout, "  Terms: %ld\n", TrStat_gt_terms);
  fprintf(Yap_stdout, "  Global trie nodes: %ld\n", TrStat_gt_nodes);
  fprintf(Yap_stdout, "  Total memory in use: %ld bytes\n", TrStat_gt_nodes * sizeof(struct global_trie_node));
  return;
}
#endif /* GLOBAL_TRIE */



/* ------------------------- **
**      Local functions      **
** ------------------------- */

#ifdef YAPOR
#ifdef TABLING_INNER_CUTS
static
int update_answer_trie_branch(ans_node_ptr previous_node, ans_node_ptr current_node) {
  int ltt;
  
  if (! IS_ANSWER_LEAF_NODE(current_node)) {
    if (TrNode_child(current_node)) {
      TrNode_instr(TrNode_child(current_node)) -= 1;  /* retry --> try */
      update_answer_trie_branch(NULL, TrNode_child(current_node));
      if (TrNode_child(current_node))
        goto update_next_trie_branch;
    }
    /* node belonging to a pruned answer */
    if (previous_node) {
      TrNode_next(previous_node) = TrNode_next(current_node);
      free_answer_trie_node(current_node);
      if (TrNode_next(previous_node)) {
        return update_answer_trie_branch(previous_node, TrNode_next(previous_node));
      } else {
        TrNode_instr(previous_node) -= 2;  /* retry --> trust : try --> do */
        return 0;
      }
    } else {
      TrNode_child(TrNode_parent(current_node)) = TrNode_next(current_node);
      if (TrNode_next(current_node)) {
        TrNode_instr(TrNode_next(current_node)) -= 1;  /* retry --> try */
        update_answer_trie_branch(NULL, TrNode_next(current_node));          
      }
      free_answer_trie_node(current_node);
      return 0;
    }
  }
update_next_trie_branch:
  if (TrNode_next(current_node)) {
    ltt = 1 + update_answer_trie_branch(current_node, TrNode_next(current_node));
  } else {
    TrNode_instr(current_node) -= 2;  /* retry --> trust : try --> do */
    ltt = 1;
  }

  TrNode_or_arg(current_node) = ltt;
  TrNode_instr(current_node) = Yap_opcode(TrNode_instr(current_node));
  return ltt;
}
#else
static
int update_answer_trie_branch(ans_node_ptr current_node) {
  int ltt;
  
  if (! IS_ANSWER_LEAF_NODE(current_node)) {
    TrNode_instr(TrNode_child(current_node)) -= 1;  /* retry --> try */
    update_answer_trie_branch(TrNode_child(current_node));
  }
  if (TrNode_next(current_node)) {
    ltt = 1 + update_answer_trie_branch(TrNode_next(current_node));
  } else {
    TrNode_instr(current_node) -= 2;  /* retry --> trust : try --> do */
    ltt = 1;
  }
  TrNode_or_arg(current_node) = ltt;
  TrNode_instr(current_node) = Yap_opcode(TrNode_instr(current_node));
  return ltt;
}
#endif /* TABLING_INNER_CUTS */
#else /* TABLING */
static
void update_answer_trie_branch(ans_node_ptr current_node, int position) {
#ifdef TABLING_CALL_SUBSUMPTION
  if(IS_ANSWER_TRIE_HASH(current_node)) {
    /* we can only be here if the answer trie is a time stamped trie! */
    tst_node_ptr chain_node, *bucket, *last_bucket;
    tst_ans_hash_ptr hash = (tst_ans_hash_ptr)current_node;
    
    bucket = TSTHT_buckets(hash);
    last_bucket = bucket + TSTHT_num_buckets(hash);
    
    while(bucket != last_bucket) {
      if(*bucket) {
        chain_node = *bucket;
        update_answer_trie_branch((ans_node_ptr)chain_node, TRAVERSE_POSITION_FIRST); /* retry --> try */
      }
      bucket++;
    }
    return;
  }
#endif /* TABLING_CALL_SUBSUMPTION */

  if (! IS_ANSWER_LEAF_NODE(current_node))
    update_answer_trie_branch(TrNode_child(current_node), TRAVERSE_POSITION_FIRST);  /* retry --> try */
  if (position == TRAVERSE_POSITION_FIRST) {
    ans_node_ptr next = TrNode_next(current_node);
    if (next) {
      while (TrNode_next(next)) {
        update_answer_trie_branch(next, TRAVERSE_POSITION_NEXT);  /* retry --> retry */
        next = TrNode_next(next);
      }
      update_answer_trie_branch(next, TRAVERSE_POSITION_LAST);  /* retry --> trust */
    } else
      position += TRAVERSE_POSITION_LAST;  /* try --> do */
  }
  TrNode_instr(current_node) = Yap_opcode(TrNode_instr(current_node) - position);
  return;
}
#endif /* YAPOR */


static
void traverse_subgoal_trie(sg_node_ptr current_node, char *str, int str_index, int *arity, int mode, int position, tab_ent_ptr tab_ent) {
  int *current_arity = NULL, current_str_index = 0, current_mode = 0;

  /* test if hashing */
  if (IS_SUBGOAL_TRIE_HASH(current_node)) {
    sg_node_ptr *bucket, *last_bucket;
    sg_hash_ptr hash;
    hash = (sg_hash_ptr) current_node;
    bucket = Hash_buckets(hash);
    last_bucket = bucket + Hash_num_buckets(hash);
    current_arity = (int *) malloc(sizeof(int) * (arity[0] + 1));
    memcpy(current_arity, arity, sizeof(int) * (arity[0] + 1));
    
    TrStat_sg_hash++;
    TrStat_hash_buckets += Hash_num_buckets(hash);
    
#ifdef TABLING_CALL_SUBSUMPTION
    if(!TabEnt_is_variant(tab_ent)) {
      count_subgoal_hash_index(hash, TrStat_sg_indexes);
    }
#endif
    
    do {
      if (*bucket) {
        traverse_subgoal_trie(*bucket, str, str_index, arity, mode, TRAVERSE_POSITION_FIRST, tab_ent);
	memcpy(arity, current_arity, sizeof(int) * (current_arity[0] + 1));
#ifdef TRIE_COMPACT_PAIRS
	if (arity[arity[0]] == -2 && str[str_index - 1] != '[')
	  str[str_index - 1] = ',';
#else
	if (arity[arity[0]] == -1)
	  str[str_index - 1] = '|';
#endif /* TRIE_COMPACT_PAIRS */
      }
    } while (++bucket != last_bucket);
    free(current_arity);
    return;
  }

  /* save current state if first sibling node */
  if (position == TRAVERSE_POSITION_FIRST) {
    current_arity = (int *) malloc(sizeof(int) * (arity[0] + 1));
    memcpy(current_arity, arity, sizeof(int) * (arity[0] + 1));
    current_str_index = str_index;
    current_mode = mode;
  }

  /* process current trie node */
  if(TrNode_is_float(current_node))
    TrStat_float_sg_nodes++;
  else if(TrNode_is_long(current_node))
    TrStat_long_sg_nodes++;
  else
    TrStat_sg_nodes++;
    
#ifdef GLOBAL_TRIE
  traverse_global_trie_for_subgoal(TrNode_entry(current_node), str, &str_index, arity, &mode);
#else
  traverse_trie_node(current_node, str, &str_index, arity, &mode, TRAVERSE_TYPE_SUBGOAL);
#endif /* GLOBAL_TRIE */

  /* continue with child node ... */
  if (arity[0] != 0) {
    traverse_subgoal_trie(TrNode_child(current_node), str, str_index, arity, mode, TRAVERSE_POSITION_FIRST, tab_ent);
  } else {
    /* ... or show answers */
    sg_fr_ptr sg_fr = (sg_fr_ptr) TrNode_sg_fr(current_node);
    
    str[str_index] = 0;
    
    SHOW_TABLE_STRUCTURE("%s.\n", str);
    
    if(TrStat_show == SHOW_MODE_STATISTICS) {
      /* count continuations */
      continuation_ptr cont = SgFr_first_answer(sg_fr);
    
      while(cont) {
        TrStat_answers++;
        cont = continuation_next(cont);
      }
    }
    
    /* show subgoal */
    switch(SgFr_type(sg_fr)) {
      case VARIANT_PRODUCER_SFT:
      case SUBSUMPTIVE_PRODUCER_SFT:
        TrStat_subgoals++;
        
        /* root node */
        TrStat_ans_nodes++;
      
        if (SgFr_has_no_answers(sg_fr)) {
          if (SgFr_state(sg_fr) < complete) {
	          TrStat_sg_incomplete++;
	          SHOW_TABLE_STRUCTURE("    ---> INCOMPLETE\n");
          } else {
	          TrStat_answers_no++;
	          SHOW_TABLE_STRUCTURE("    NO\n");
          }
        } else if (SgFr_has_yes_answer(sg_fr)) {
          TrStat_answers_true++;
          SHOW_TABLE_STRUCTURE("    TRUE\n");
        } else {
          arity[0] = 0;
          traverse_answer_trie(TrNode_child(SgFr_answer_trie(sg_fr)), &str[str_index],
                  0, arity, 0, TRAVERSE_MODE_NORMAL, TRAVERSE_POSITION_FIRST);
      
          if (SgFr_state(sg_fr) < complete) {
	          TrStat_sg_incomplete++;
	          SHOW_TABLE_STRUCTURE("    ---> INCOMPLETE\n");
          }
        }
        break;
#ifdef TABLING_CALL_SUBSUMPTION
      case SUBSUMED_CONSUMER_SFT:
        {
          TrStat_subcons_subgoals++;
          
          subcons_fr_ptr cons_sg = (subcons_fr_ptr)sg_fr;
          subprod_fr_ptr prod_sg = SgFr_producer(cons_sg);
      
          if(SgFr_has_no_answers(sg_fr)) {
            if(SgFr_state(sg_fr) < complete) {
              TrStat_sg_incomplete++;
              SHOW_TABLE_STRUCTURE("    ---> INCOMPLETE\n");
            } else {
              TrStat_answers_no++;
              SHOW_TABLE_STRUCTURE("    NO\n");
            }
          } else { // has answers
            show_consumer_subsumptive_with_answers(cons_sg, prod_sg);
          }
        }
        break;
      case GROUND_PRODUCER_SFT:
      case GROUND_CONSUMER_SFT:
        {
          TrStat_subgoals++;
          
          if(SgFr_is_ground_consumer(sg_fr)) {
            grounded_sf_ptr ground_sg = (grounded_sf_ptr)sg_fr;
            grounded_sf_ptr producer_sg = (grounded_sf_ptr)SgFr_producer(ground_sg);
            
            if(SgFr_state(ground_sg) < complete && SgFr_state(producer_sg) >= complete) {
              mark_ground_consumer_as_completed(ground_sg);
            }
            
            build_next_ground_consumer_return_list(ground_sg);
          }
          if(SgFr_has_no_answers(sg_fr)) {
            if(SgFr_state(sg_fr) < complete) {
              TrStat_sg_incomplete++;
              SHOW_TABLE_STRUCTURE("    ---> INCOMPLETE\n");
            } else {
              TrStat_answers_no++;
              SHOW_TABLE_STRUCTURE("    NO\n");
            }
          } else {
            show_ground_with_answers(sg_fr);
          }
        }
        break;
#endif /* TABLING_CALL_SUBSUMPTION */
      default: break;
    }
  }

  /* restore the initial state and continue with sibling nodes */
  if (position == TRAVERSE_POSITION_FIRST) {
    str_index = current_str_index;
    mode = current_mode;
    current_node = TrNode_next(current_node);
    while (current_node) {
      memcpy(arity, current_arity, sizeof(int) * (current_arity[0] + 1));
#ifdef TRIE_COMPACT_PAIRS
      if (arity[arity[0]] == -2 && str[str_index - 1] != '[')
	str[str_index - 1] = ',';
#else
      if (arity[arity[0]] == -1)
	str[str_index - 1] = '|';
#endif /* TRIE_COMPACT_PAIRS */
      traverse_subgoal_trie(current_node, str, str_index, arity, mode, TRAVERSE_POSITION_NEXT, tab_ent);
      current_node = TrNode_next(current_node);
    }
    free(current_arity);
  }

  return;
}

#ifdef TABLING_CALL_SUBSUMPTION
static void
traverse_ground_trie(ans_node_ptr node)
{
  if(IS_ANSWER_TRIE_HASH(node)) {
    ans_node_ptr *bucket, *last_bucket;
    ans_hash_ptr hash;
    hash = (ans_hash_ptr) node;
    bucket = Hash_buckets(hash);
    last_bucket = bucket + Hash_num_buckets(hash);
    
    TrStat_ans_hash++;
    TrStat_hash_buckets += Hash_num_buckets(hash);
    
    /* count TST indices */
    count_tst_indices(hash, TrStat_tst_indexes);
    
    do {
      if (*bucket)
        traverse_ground_trie(*bucket);
    } while (++bucket != last_bucket);
    return;
  }
  
  /* process tst node */
  if(TrNode_is_float(node))
    TrStat_float_ans_nodes++;
  else if(TrNode_is_long(node))
    TrStat_long_ans_nodes++;
  else
    TrStat_ans_nodes++;
    
  if(!IS_ANSWER_LEAF_NODE(node)) {
    if(TrNode_next(node))
      traverse_ground_trie(TrNode_next(node));
    traverse_ground_trie(TrNode_child(node));
  }
}

static void
ground_trie_statistics(tab_ent_ptr tab_ent)
{
  if(TabEnt_subgoal_trie(tab_ent) == NULL)
    return;
    
  ans_node_ptr root = (ans_node_ptr)TabEnt_ground_trie(tab_ent);
  
  traverse_ground_trie(root);
}
#endif /* TABLING_CALL_SUBSUMPTION */

static
void traverse_answer_trie(ans_node_ptr current_node, char *str, int str_index, int *arity, int var_index, int mode, int position) {
  int *current_arity = NULL, current_str_index = 0, current_var_index = 0, current_mode = 0;

  /* test if hashing */
  if (IS_ANSWER_TRIE_HASH(current_node)) {
    ans_node_ptr *bucket, *last_bucket;
    ans_hash_ptr hash;
    hash = (ans_hash_ptr) current_node;
    bucket = Hash_buckets(hash);
    last_bucket = bucket + Hash_num_buckets(hash);
    current_arity = (int *) malloc(sizeof(int) * (arity[0] + 1));
    memcpy(current_arity, arity, sizeof(int) * (arity[0] + 1));
    
    TrStat_ans_hash++;
    TrStat_hash_buckets += Hash_num_buckets(hash);
    
    /* count TST indices */
    if(TrStat_show == SHOW_MODE_STATISTICS && TrNode_is_tst(current_node)) {
      count_tst_indices(hash, TrStat_tst_indexes);
    }
    
    do {
      if (*bucket) {
        traverse_answer_trie(*bucket, str, str_index, arity, var_index, mode, TRAVERSE_POSITION_FIRST);
	memcpy(arity, current_arity, sizeof(int) * (current_arity[0] + 1));
#ifdef TRIE_COMPACT_PAIRS
	if (arity[arity[0]] == -2 && str[str_index - 1] != '[')
	  str[str_index - 1] = ',';
#else
	if (arity[arity[0]] == -1)
	  str[str_index - 1] = '|';
#endif /* TRIE_COMPACT_PAIRS */
      }
    } while (++bucket != last_bucket);
    free(current_arity);
    return;
  }

  /* save current state if first sibling node */
  if (position == TRAVERSE_POSITION_FIRST) {
    current_arity = (int *) malloc(sizeof(int) * (arity[0] + 1));
    memcpy(current_arity, arity, sizeof(int) * (arity[0] + 1));
    current_str_index = str_index;
    current_var_index = var_index;
    current_mode = mode;
  }

  /* print VAR if starting a term */
  if (arity[0] == 0 && mode == TRAVERSE_MODE_NORMAL) {
    str_index += sprintf(& str[str_index], "    VAR%d: ", var_index);
    var_index++;
  }

  /* process current trie node */
  if(TrNode_is_float(current_node))
    TrStat_float_ans_nodes++;
  else if(TrNode_is_long(current_node))
    TrStat_long_ans_nodes++;
  else
    TrStat_ans_nodes++;
    
#ifdef GLOBAL_TRIE
  traverse_global_trie_for_answer(TrNode_entry(current_node), str, &str_index, arity, &mode);
#else
  traverse_trie_node(current_node, str, &str_index, arity, &mode, TRAVERSE_TYPE_ANSWER);
#endif /* GLOBAL_TRIE */

  /* show answer .... */
  if (IS_ANSWER_LEAF_NODE(current_node)) {
    str[str_index] = 0;
    SHOW_TABLE_STRUCTURE("%s\n", str);
  }
#ifdef TABLING_INNER_CUTS
  /* ... or continue with pruned node */
  else if (TrNode_child(current_node) == NULL)
    TrStat_answers++;
    TrStat_answers_pruned++;
#endif /* TABLING_INNER_CUTS */
  /* ... or continue with child node */
  else
    traverse_answer_trie(TrNode_child(current_node), str, str_index, arity, var_index, mode, TRAVERSE_POSITION_FIRST);

  /* restore the initial state and continue with sibling nodes */
  if (position == TRAVERSE_POSITION_FIRST) {
    str_index = current_str_index;
    var_index = current_var_index;
    mode = current_mode;
    current_node = TrNode_next(current_node);
    while (current_node) {
      memcpy(arity, current_arity, sizeof(int) * (current_arity[0] + 1));
#ifdef TRIE_COMPACT_PAIRS
      if (arity[arity[0]] == -2 && str[str_index - 1] != '[')
	str[str_index - 1] = ',';
#else
      if (arity[arity[0]] == -1)
	str[str_index - 1] = '|';
#endif /* TRIE_COMPACT_PAIRS */
      traverse_answer_trie(current_node, str, str_index, arity, var_index, mode, TRAVERSE_POSITION_NEXT);
      current_node = TrNode_next(current_node);
    }
    free(current_arity);
  }

  return;
}


/* this code is awful */
static
void traverse_trie_node(void* node, char *str, int *str_index_ptr, int *arity, int *mode_ptr, int type) {
  int mode = *mode_ptr;
  int str_index = *str_index_ptr;
  Term t = TrNode_entry((sg_node_ptr)node);
  int flags = TrNode_node_type((sg_node_ptr)node);

  /* test the node type */
#if SIZEOF_DOUBLE == 2 * SIZEOF_INT_P
  if (mode == TRAVERSE_MODE_FLOAT) {
    arity[0]++;
    arity[arity[0]] = (int) t;
    mode = TRAVERSE_MODE_FLOAT2;
  } else if (mode == TRAVERSE_MODE_FLOAT2 || IS_FLOAT_FLAG(flags)) {
    volatile Float dbl;
    if(IS_FLOAT_FLAG(flags))
      dbl = node_get_float((sg_node_ptr)node);
    else {
      volatile Term *t_dbl = (Term *)((void *) &dbl);
      *t_dbl = t;
      *(t_dbl + 1) = (Term) arity[arity[0]];
      arity[0]--;
    }
    str_index += sprintf(& str[str_index], "%.15g", dbl);
#else /* SIZEOF_DOUBLE == SIZEOF_INT_P */
  if (mode == TRAVERSE_MODE_FLOAT || IS_FLOAT_FLAG(flags)) {
    volatile Float dbl;
    if(IS_FLOAT_FLAG(flags))
      flt = node_get_float((sg_node_ptr)node);
    else {
      volatile Term *t_dbl = (Term *)((void *) &dbl);
      *t_dbl = t;
    }
    str_index += sprintf(& str[str_index], "%.15g", dbl);
#endif /* SIZEOF_DOUBLE x SIZEOF_INT_P */
    while (arity[0]) {
      if (arity[arity[0]] > 0) {
	      arity[arity[0]]--;
	      if (arity[arity[0]] == 0) {
	        str_index += sprintf(& str[str_index], ")");
	        arity[0]--;
	      } else {
	        str_index += sprintf(& str[str_index], ",");
	        break;
	      }
      } else {
	      if (arity[arity[0]] == -2) {
#ifdef TRIE_COMPACT_PAIRS
	        str_index += sprintf(& str[str_index], ",");
#else
	        str_index += sprintf(& str[str_index], "|");
	        arity[arity[0]] = -1;
#endif /* TRIE_COMPACT_PAIRS */
	        break;
	      } else {
	        str_index += sprintf(& str[str_index], "]");
	        arity[0]--;
	      }
      }
    }
#ifndef GLOBAL_TRIE
    if (type == TRAVERSE_TYPE_SUBGOAL || IS_FLOAT_FLAG(flags))
      mode = TRAVERSE_MODE_NORMAL;
    else  /* type == TRAVERSE_TYPE_ANSWER */
#endif /* GLOBAL_TRIE */
      mode = TRAVERSE_MODE_FLOAT_END;
  } else if (mode == TRAVERSE_MODE_FLOAT_END) {
    mode = TRAVERSE_MODE_NORMAL;
  } else if (IS_LONG_INT_FLAG(flags) || mode == TRAVERSE_MODE_LONG) {
    Int li = 0;
    
    if(IS_LONG_INT_FLAG(flags))
      li = node_get_long_int((sg_node_ptr)node);
    else
      li = (Int) t;
      
    str_index += sprintf(& str[str_index], LongIntFormatString, li);
    while (arity[0]) {
      if (arity[arity[0]] > 0) {
	      arity[arity[0]]--;
	      if (arity[arity[0]] == 0) {
	        str_index += sprintf(& str[str_index], ")");
	        arity[0]--;
	      } else {
	        str_index += sprintf(& str[str_index], ",");
	        break;
	      }
      } else {
	      if (arity[arity[0]] == -2) {
#ifdef TRIE_COMPACT_PAIRS
	        str_index += sprintf(& str[str_index], ",");
#else
	        str_index += sprintf(& str[str_index], "|");
	        arity[arity[0]] = -1;
#endif /* TRIE_COMPACT_PAIRS */
	        break;
	      } else {
	        str_index += sprintf(& str[str_index], "]");
	        arity[0]--;
	      }
      }
    }
#ifndef GLOBAL_TRIE
    if (type == TRAVERSE_TYPE_SUBGOAL || IS_LONG_INT_FLAG(flags))
      mode = TRAVERSE_MODE_NORMAL;
    else  /* type == TRAVERSE_TYPE_ANSWER */
#endif /* GLOBAL_TRIE */
      mode = TRAVERSE_MODE_LONG_END;
  } else if (mode == TRAVERSE_MODE_LONG_END) {
    mode = TRAVERSE_MODE_NORMAL;
  } else if (IsVarTerm(t)) {
    if (type == TRAVERSE_TYPE_SUBGOAL)
      str_index += sprintf(& str[str_index], "VAR%d", VarIndexOfTableTerm(t));
    else  /* type == TRAVERSE_TYPE_ANSWER */
      str_index += sprintf(& str[str_index], "ANSVAR%d", VarIndexOfTableTerm(t));
    while (arity[0]) {
      if (arity[arity[0]] > 0) {
	arity[arity[0]]--;
	if (arity[arity[0]] == 0) {
	  str_index += sprintf(& str[str_index], ")");
	  arity[0]--;
	} else {
	  str_index += sprintf(& str[str_index], ",");
	  break;
	}
      } else {
	if (arity[arity[0]] == -2) {
#ifdef TRIE_COMPACT_PAIRS
	  str_index += sprintf(& str[str_index], ",");
#else
	  str_index += sprintf(& str[str_index], "|");
	  arity[arity[0]] = -1;
#endif /* TRIE_COMPACT_PAIRS */
	  break;
	} else {
	  str_index += sprintf(& str[str_index], "]");
	  arity[0]--;
	}
      }
    }
  } else if (IsIntTerm(t)) {
    str_index += sprintf(& str[str_index], LongIntFormatString, IntOfTerm(t));
    while (arity[0]) {
      if (arity[arity[0]] > 0) {
	arity[arity[0]]--;
	if (arity[arity[0]] == 0) {
	  str_index += sprintf(& str[str_index], ")");
	  arity[0]--;
	} else {
	  str_index += sprintf(& str[str_index], ",");
	  break;
	}
      } else {
	if (arity[arity[0]] == -2) {
#ifdef TRIE_COMPACT_PAIRS
	  str_index += sprintf(& str[str_index], ",");
#else
	  str_index += sprintf(& str[str_index], "|");
	  arity[arity[0]] = -1;
#endif /* TRIE_COMPACT_PAIRS */
	  break;
	} else {
	  str_index += sprintf(& str[str_index], "]");
	  arity[0]--;
	}
      }
    }
  } else if (IsAtomTerm(t)) {
#ifndef TRIE_COMPACT_PAIRS
    if (arity[arity[0]] == -1 && t == TermNil) {
      str[str_index - 1] = ']';
      arity[0]--;
    } else
#endif /* TRIE_COMPACT_PAIRS */
      str_index += sprintf(& str[str_index], "%s", AtomName(AtomOfTerm(t)));
    while (arity[0]) {
      if (arity[arity[0]] > 0) {
	arity[arity[0]]--;
	if (arity[arity[0]] == 0) {
	  str_index += sprintf(& str[str_index], ")");
	  arity[0]--;
	} else {
	  str_index += sprintf(& str[str_index], ",");
	  break;
	}
      } else {
	if (arity[arity[0]] == -2) {
#ifdef TRIE_COMPACT_PAIRS
	  str_index += sprintf(& str[str_index], ",");
#else
	  str_index += sprintf(& str[str_index], "|");
	  arity[arity[0]] = -1;
#endif /* TRIE_COMPACT_PAIRS */
	  break;
	} else {
	  str_index += sprintf(& str[str_index], "]");
	  arity[0]--;
	}
      }
    }
  } else if (IsPairTerm(t)) {
#ifdef TRIE_COMPACT_PAIRS
    if (t == CompactPairEndList)
      arity[arity[0]] = -1;
    else if (t == CompactPairEndTerm) {
      str[str_index - 1] = '|';
      arity[arity[0]] = -1;
#else
    if (arity[arity[0]] == -1) {
      str[str_index - 1] = ',';
      arity[arity[0]] = -2;
#endif /* TRIE_COMPACT_PAIRS */
    } else {
      str_index += sprintf(& str[str_index], "[");
      arity[0]++;
      arity[arity[0]] = -2;
    }
  } else if (IsApplTerm(t)) {
    Functor f = (Functor) RepAppl(t);
    if (f == FunctorDouble) {
      mode = TRAVERSE_MODE_FLOAT;
    } else if (f == FunctorLongInt) {
      mode = TRAVERSE_MODE_LONG;
    } else {
      str_index += sprintf(& str[str_index], "%s(", AtomName(NameOfFunctor(f)));
      arity[0]++;
      arity[arity[0]] = ArityOfFunctor(f);
    }
  }

  *mode_ptr = mode;
  *str_index_ptr = str_index;
  return;
}


#ifdef GLOBAL_TRIE
static
void free_global_trie_branch(gt_node_ptr current_node) {
  gt_node_ptr parent_node, child_node;

  parent_node = TrNode_parent(current_node);
  child_node  = TrNode_child(parent_node);
  if (IS_GLOBAL_TRIE_HASH(child_node)) {
    gt_hash_ptr hash;
    gt_node_ptr *bucket;
    hash = (gt_hash_ptr) child_node;
    Hash_num_nodes(hash)--;
    bucket = Hash_bucket(hash, HASH_ENTRY(TrNode_entry(current_node), Hash_seed(hash)));
    child_node = *bucket;
    if (child_node != current_node) {
      while (TrNode_next(child_node) != current_node)
	child_node = TrNode_next(child_node);
      TrNode_next(child_node) = TrNode_next(current_node);
      FREE_GLOBAL_TRIE_NODE(current_node);
    } else {
      *bucket = TrNode_next(current_node);
      FREE_GLOBAL_TRIE_NODE(current_node);
      if (Hash_num_nodes(hash) == 0) {
	FREE_HASH_BUCKETS(Hash_buckets(hash));
	FREE_GLOBAL_TRIE_HASH(hash);
	if (parent_node != GLOBAL_root_gt)
	  free_global_trie_branch(parent_node);
	else
	  TrNode_child(parent_node) = NULL;
      }
    }
  } else if (child_node != current_node) {
    while (TrNode_next(child_node) != current_node)
      child_node = TrNode_next(child_node);
    TrNode_next(child_node) = TrNode_next(current_node);
    FREE_GLOBAL_TRIE_NODE(current_node);
  } else if (TrNode_next(current_node) == NULL) {
    FREE_GLOBAL_TRIE_NODE(current_node);
    if (parent_node != GLOBAL_root_gt)
      free_global_trie_branch(parent_node);
    else
      TrNode_child(parent_node) = NULL;
  } else {
    TrNode_child(parent_node) = TrNode_next(current_node);
    FREE_GLOBAL_TRIE_NODE(current_node);
  }
  return;
}


static
void traverse_global_trie(gt_node_ptr current_node, char *str, int str_index, int *arity, int mode, int position) {
  int *current_arity = NULL, current_str_index = 0, current_mode = 0;

  /* test if hashing */
  if (IS_GLOBAL_TRIE_HASH(current_node)) {
    gt_node_ptr *bucket, *last_bucket;
    gt_hash_ptr hash;
    hash = (gt_hash_ptr) current_node;
    bucket = Hash_buckets(hash);
    last_bucket = bucket + Hash_num_buckets(hash);
    current_arity = (int *) malloc(sizeof(int) * (arity[0] + 1));
    memcpy(current_arity, arity, sizeof(int) * (arity[0] + 1));
    do {
      if (*bucket) {
        traverse_global_trie(*bucket, str, str_index, arity, mode, TRAVERSE_POSITION_FIRST);
	memcpy(arity, current_arity, sizeof(int) * (current_arity[0] + 1));
#ifdef TRIE_COMPACT_PAIRS
	if (arity[arity[0]] == -2 && str[str_index - 1] != '[')
	  str[str_index - 1] = ',';
#else
	if (arity[arity[0]] == -1)
	  str[str_index - 1] = '|';
#endif /* TRIE_COMPACT_PAIRS */
      }
    } while (++bucket != last_bucket);
    free(current_arity);
    return;
  }

  /* save current state if first sibling node */
  if (position == TRAVERSE_POSITION_FIRST) {
    current_arity = (int *) malloc(sizeof(int) * (arity[0] + 1));
    memcpy(current_arity, arity, sizeof(int) * (arity[0] + 1));
    current_str_index = str_index;
    current_mode = mode;
  }

  /* process current trie node */
  TrStat_gt_nodes++;
  traverse_trie_node(current_node, str, &str_index, arity, &mode, TRAVERSE_TYPE_SUBGOAL);

  /* continue with child node ... */
  if (arity[0] != 0)
    traverse_global_trie(TrNode_child(current_node), str, str_index, arity, mode, TRAVERSE_POSITION_FIRST);
  /* ... or show term */
  else {
    TrStat_gt_terms++;
    str[str_index] = 0;
    SHOW_TABLE_STRUCTURE("  TERM (x%ld): %s\n", (unsigned long int) TrNode_child(current_node), str);
  }

  /* restore the initial state and continue with sibling nodes */
  if (position == TRAVERSE_POSITION_FIRST) {
    str_index = current_str_index;
    mode = current_mode;
    current_node = TrNode_next(current_node);
    while (current_node) {
      memcpy(arity, current_arity, sizeof(int) * (current_arity[0] + 1));
#ifdef TRIE_COMPACT_PAIRS
      if (arity[arity[0]] == -2 && str[str_index - 1] != '[')
	str[str_index - 1] = ',';
#else
      if (arity[arity[0]] == -1)
	str[str_index - 1] = '|';
#endif /* TRIE_COMPACT_PAIRS */
      traverse_global_trie(current_node, str, str_index, arity, mode, TRAVERSE_POSITION_NEXT);
      current_node = TrNode_next(current_node);
    }
    free(current_arity);
  }

  return;
}


static
void traverse_global_trie_for_subgoal(gt_node_ptr current_node, char *str, int *str_index, int *arity, int *mode) {
  if (TrNode_parent(current_node) != GLOBAL_root_gt)
    traverse_global_trie_for_subgoal(TrNode_parent(current_node), str, str_index, arity, mode);
  traverse_trie_node(current_node, str, str_index, arity, mode, TRAVERSE_TYPE_SUBGOAL);
  return;
}


static
void traverse_global_trie_for_answer(gt_node_ptr current_node, char *str, int *str_index, int *arity, int *mode) {
  if (TrNode_parent(current_node) != GLOBAL_root_gt)
    traverse_global_trie_for_answer(TrNode_parent(current_node), str, str_index, arity, mode);
  traverse_trie_node(current_node, str, str_index, arity, mode, TRAVERSE_TYPE_ANSWER);
  return;
}
#endif /* GLOBAL_TRIE */
#endif /* TABLING */
