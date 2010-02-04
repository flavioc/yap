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
#include "tab.macros.h"
#include "tab.utils.h"
#include "tab.subsumption.h"
#include "tab.tries.h"
#include "tab.var.h"

/* -------------------------- **
**      Global functions      **
** -------------------------- */

sg_fr_ptr subgoal_search(yamop *preg, CELL **Yaddr) {
  
  tab_ent_ptr tab_ent = preg->u.Otapl.te;
  
#ifdef TABLE_LOCK_AT_ENTRY_LEVEL
  LOCK(TabEnt_lock(tab_ent));
#endif /* TABLE_LOCK_LEVEL */
  
  printf("subgoal_search for ");
  printCalledSubgoal(stdout, preg);
  printf("\n");
  
  TabledCallInfo call_info;
  CallLookupResults results;
  
  CallInfo_table_entry(&call_info) = tab_ent;
  CallInfo_arity(&call_info) = preg->u.Otapl.s;
  CallInfo_code(&call_info) = preg;
  CallInfo_var_vector(&call_info) = *Yaddr;
  
  if(TabEnt_is_variant(tab_ent)) {
    printf("IS VARIANT\n");
    variant_call_search(&call_info, &results);
  } else {
    // subsumptive subgoal
    printf("IS SUBSUMPTIVE\n");
    subsumptive_call_search(&call_info, &results);
  }
  
  *Yaddr = CallResults_var_vector(&results)++;
  
  return CallResults_subgoal_frame(&results);
}


ans_node_ptr answer_search(sg_fr_ptr sg_fr, CELL *subs_ptr) {
  int i, j, count_vars, subs_arity;
  CELL *stack_vars, *stack_terms_base, *stack_terms;
#ifdef GLOBAL_TRIE
  gt_node_ptr current_node;
  ans_node_ptr current_ans_node;
#else
  ans_node_ptr current_node;
#define current_ans_node current_node
#endif /* GLOBAL_TRIE */
#ifdef TRIE_COMPACT_PAIRS
  int in_new_pair = 0;
#else
#define in_new_pair 0
#endif /* TRIE_COMPACT_PAIRS */

  count_vars = 0;
  subs_arity = *subs_ptr;
  stack_vars = (CELL *)TR;
  stack_terms_base = stack_terms = (CELL *)Yap_TrailTop;
  current_ans_node = SgFr_answer_trie(sg_fr);
  for (i = subs_arity; i >= 1; i--) {
#ifdef GLOBAL_TRIE
    current_node = GLOBAL_root_gt;
#endif /* GLOBAL_TRIE */
#ifdef TABLING_ERRORS
    if (IsNonVarTerm(*(subs_ptr + i)))
      TABLING_ERROR_MESSAGE("IsNonVarTem(*(subs_ptr + i)) (answer_search)");
#endif /* TABLING_ERRORS */
    STACK_CHECK_EXPAND(stack_terms, stack_vars, stack_terms_base);
    STACK_PUSH_UP(Deref(*(subs_ptr + i)), stack_terms);
    do {
      Term t = STACK_POP_DOWN(stack_terms);
      if (IsVarTerm(t)) {
	t = Deref(t);
	if (IsTableVarTerm(t)) {
	  t = MakeTableVarTerm(VarIndexOfTerm(t));
          ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, t, _trie_retry_val + in_new_pair);
	} else {
	  if (count_vars == MAX_TABLE_VARS)
	    Yap_Error(INTERNAL_ERROR, TermNil, "MAX_TABLE_VARS exceeded (answer_search)");
	  STACK_PUSH_DOWN(t, stack_vars);
	  *((CELL *)t) = GLOBAL_table_var_enumerator(count_vars);
	  t = MakeTableVarTerm(count_vars);
	  count_vars++;
          ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, t, _trie_retry_var + in_new_pair);
	}
#ifdef TRIE_COMPACT_PAIRS
	in_new_pair = 0;
#endif /* TRIE_COMPACT_PAIRS */
      } else if (IsAtomOrIntTerm(t)) {
        ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, t, _trie_retry_atom + in_new_pair);
#ifdef TRIE_COMPACT_PAIRS
	in_new_pair = 0;
#endif /* TRIE_COMPACT_PAIRS */
      } else if (IsPairTerm(t)) {
#ifdef TRIE_COMPACT_PAIRS
	CELL *aux = RepPair(t);
	if (aux == PairTermMark) {
	  t = STACK_POP_DOWN(stack_terms);
	  if (IsPairTerm(t)) {
	    aux = RepPair(t);
	    t = Deref(*(aux + 1));
	    if (t == TermNil) {
              ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, CompactPairEndList, _trie_retry_pair);
	    } else {
	      /* STACK_CHECK_EXPAND(stack_terms, stack_vars + 2, stack_terms_base); */
	      /* STACK_CHECK_EXPAND is not necessary here because the situation of pushing **
	      ** up 3 terms has already initially checked for the CompactPairInit term */
	      STACK_PUSH_UP(t, stack_terms);
	      STACK_PUSH_UP(AbsPair(PairTermMark), stack_terms);
	      in_new_pair = 4;
	    }
	    STACK_PUSH_UP(Deref(*aux), stack_terms);
	  } else {
            ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, CompactPairEndTerm, _trie_retry_null);
	    STACK_PUSH_UP(t, stack_terms);
	  }         
	} else {
          ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, CompactPairInit, _trie_retry_null + in_new_pair);
	  t = Deref(*(aux + 1));
	  if (t == TermNil) {
            ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, CompactPairEndList, _trie_retry_pair);
	    in_new_pair = 0;
	  } else {
	    STACK_CHECK_EXPAND(stack_terms, stack_vars + 2, stack_terms_base);
	    STACK_PUSH_UP(t, stack_terms);
	    STACK_PUSH_UP(AbsPair(PairTermMark), stack_terms);
	    in_new_pair = 4;
	  }
	  STACK_PUSH_UP(Deref(*aux), stack_terms);
	}
#else
        ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, AbsPair(NULL), _trie_retry_pair);
	STACK_CHECK_EXPAND(stack_terms, stack_vars + 1, stack_terms_base);
	STACK_PUSH_UP(Deref(*(RepPair(t) + 1)), stack_terms);
	STACK_PUSH_UP(Deref(*(RepPair(t))), stack_terms);
#endif /* TRIE_COMPACT_PAIRS */
      } else if (IsApplTerm(t)) {
	Functor f = FunctorOfTerm(t);
	if (f == FunctorDouble) {
	  volatile Float dbl = FloatOfTerm(t);
	  volatile Term *t_dbl = (Term *)((void *) &dbl);
          ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, AbsAppl((Term *)f), _trie_retry_null + in_new_pair);
#if SIZEOF_DOUBLE == 2 * SIZEOF_INT_P
          ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, *(t_dbl + 1), _trie_retry_extension);
#endif /* SIZEOF_DOUBLE x SIZEOF_INT_P */
          ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, *t_dbl, _trie_retry_extension);
          ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, AbsAppl((Term *)f), _trie_retry_float);
	} else if (f == FunctorLongInt) {
	  Int li = LongIntOfTerm (t);
          ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, AbsAppl((Term *)f), _trie_retry_null + in_new_pair);
          ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, li, _trie_retry_extension);
          ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, AbsAppl((Term *)f), _trie_retry_long);
	} else if (f == FunctorDBRef) {
	  Yap_Error(INTERNAL_ERROR, TermNil, "unsupported type tag (FunctorDBRef in answer_search)");
	} else if (f == FunctorBigInt) {
	  Yap_Error(INTERNAL_ERROR, TermNil, "unsupported type tag (FunctorBigInt in answer_search)");
	} else {
          ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, AbsAppl((Term *)f), _trie_retry_struct + in_new_pair);
          STACK_CHECK_EXPAND(stack_terms, stack_vars + ArityOfFunctor(f) - 1, stack_terms_base);
	  for (j = ArityOfFunctor(f); j >= 1; j--)
	    STACK_PUSH_UP(Deref(*(RepAppl(t) + j)), stack_terms);
	}
#ifdef TRIE_COMPACT_PAIRS
	in_new_pair = 0;
#endif /* TRIE_COMPACT_PAIRS */
      } else {
	Yap_Error(INTERNAL_ERROR, TermNil, "unknown type tag (answer_search)");
      }
    } while (STACK_NOT_EMPTY(stack_terms, stack_terms_base));
#ifdef GLOBAL_TRIE
    current_ans_node = answer_trie_node_check_insert(sg_fr, current_ans_node, current_node, _trie_retry_atom);
#endif /* GLOBAL_TRIE */
  }

  /* reset variables */
  while (count_vars--) {
    Term t = STACK_POP_UP(stack_vars);
    RESET_VARIABLE(t);
  }

  return current_ans_node;
#ifndef GLOBAL_TRIE
#undef current_ans_node
#endif /* GLOBAL_TRIE */
#ifndef TRIE_COMPACT_PAIRS
#undef in_new_pair
#endif /* TRIE_COMPACT_PAIRS */
}


void load_answer(ans_node_ptr current_ans_node, CELL *subs_ptr) {
  CELL *stack_vars_base, *stack_vars, *stack_terms_base, *stack_terms;
  int i, subs_arity, vars_arity = 0;
  Term t;
#ifdef GLOBAL_TRIE
  gt_node_ptr current_node;
#else
#define current_node current_ans_node
#endif /* GLOBAL_TRIE */
#ifdef TRIE_COMPACT_PAIRS
  int stack_terms_pair_offset = 0;
#endif /* TRIE_COMPACT_PAIRS */

  if ((subs_arity = *subs_ptr) == 0)
    return;

#ifdef TABLING_ERRORS
  if (H < H_FZ)
    TABLING_ERROR_MESSAGE("H < H_FZ (load_answer)");
#endif /* TABLING_ERRORS */
  stack_vars_base = stack_vars = (CELL *)TR;
  stack_terms_base = stack_terms = (CELL *)Yap_TrailTop;
#ifdef GLOBAL_TRIE
  for (i = subs_arity; i >= 1; i--) {
    current_node = TrNode_entry(current_ans_node);
    current_ans_node = UNTAG_ANSWER_LEAF_NODE(TrNode_parent(current_ans_node));
    t = TrNode_entry(current_node);
    current_node = TrNode_parent(current_node);
#else
  {
    t = TrNode_entry(current_node);
    current_node = UNTAG_ANSWER_LEAF_NODE(TrNode_parent(current_node));
#endif /* GLOBAL_TRIE */
    do {
      if (IsVarTerm(t)) {
	int var_index = VarIndexOfTableTerm(t);
	STACK_CHECK_EXPAND(stack_terms, stack_vars_base + var_index + 1, stack_terms_base);
	if (var_index >= vars_arity) {
	  while (vars_arity < var_index)
	    stack_vars_base[vars_arity++] = 0; 
	  stack_vars_base[vars_arity++] = MkVarTerm(); 
	  stack_vars = stack_vars_base + vars_arity;
	} else if (stack_vars_base[var_index] == 0)
	  stack_vars_base[var_index] = MkVarTerm(); 
	STACK_PUSH_UP(stack_vars_base[var_index], stack_terms);
      } else if (IsAtomOrIntTerm(t)) {
	STACK_CHECK_EXPAND(stack_terms, stack_vars, stack_terms_base);
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
	  STACK_CHECK_EXPAND(stack_terms, stack_vars + 1, stack_terms_base);
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
	  STACK_CHECK_EXPAND(stack_terms, stack_vars, stack_terms_base);
	  STACK_PUSH_UP(t, stack_terms);
	} else if (f == FunctorLongInt) {
	  Int li = TrNode_entry(current_node);
	  current_node = TrNode_parent(current_node);
	  current_node = TrNode_parent(current_node);
	  t = MkLongIntTerm(li);
	  STACK_CHECK_EXPAND(stack_terms, stack_vars, stack_terms_base);
	  STACK_PUSH_UP(t, stack_terms);
        } else {
	  int f_arity = ArityOfFunctor(f);
	  t = Yap_MkApplTerm(f, f_arity, stack_terms);
	  stack_terms += f_arity;
	  STACK_CHECK_EXPAND(stack_terms, stack_vars, stack_terms_base);
	  STACK_PUSH_UP(t, stack_terms);
	}
      }
      t = TrNode_entry(current_node);
      current_node = TrNode_parent(current_node);
    } while (current_node);
  }

  for (i = subs_arity; i >= 1; i--) {
    CELL *subs_var = (CELL *) *(subs_ptr + i);
    t = STACK_POP_DOWN(stack_terms);
    Bind(subs_var, t);
  }

#ifdef TABLING_ERRORS
  if (stack_terms != (CELL *)Yap_TrailTop)
    TABLING_ERROR_MESSAGE("stack_terms != Yap_TrailTop (load_answer)");
#endif /* TABLING_ERRORS */

  return;
#ifndef GLOBAL_TRIE
#undef current_node
#endif /* GLOBAL_TRIE */
}


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
  /* complete subgoals */
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
    LOCAL_top_sg_fr = SgFr_next(LOCAL_top_sg_fr);
  }
  mark_as_completed(LOCAL_top_sg_fr);
  LOCAL_top_sg_fr = SgFr_next(LOCAL_top_sg_fr);
#endif /* LIMIT_TABLING */

  /* release dependency frames */
  while (EQUAL_OR_YOUNGER_CP(DepFr_cons_cp(LOCAL_top_dep_fr), B)) {  /* never equal if batched scheduling */
    dep_fr_ptr dep_fr = DepFr_next(LOCAL_top_dep_fr);
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
      if (f == FunctorDouble)
#if SIZEOF_DOUBLE == 2 * SIZEOF_INT_P
	nodes_extra = 2;
#else
        nodes_extra = 1;
#endif /* SIZEOF_DOUBLE x SIZEOF_INT_P */
      else if (f == FunctorLongInt)
	nodes_extra = 1;
      else
	nodes_left += ArityOfFunctor(f) - 1;
    }
  }
  if (nodes_left)
    free_subgoal_trie_branch(TrNode_child(current_node), nodes_left, nodes_extra, TRAVERSE_POSITION_FIRST);
#endif /* GLOBAL_TRIE */
  else {
    sg_fr_ptr sg_fr;
    ans_node_ptr ans_node;
    sg_fr = (sg_fr_ptr) TrNode_sg_fr(current_node);
    free_answer_trie_hash_chain(SgFr_hash_chain(sg_fr));
    free_answer_continuation(SgFr_first_answer(sg_fr));
    ans_node = SgFr_answer_trie(sg_fr);
    if (TrNode_child(ans_node))
      free_answer_trie_branch(TrNode_child(ans_node), TRAVERSE_POSITION_FIRST);
    FREE_ANSWER_TRIE_NODE(ans_node);
#ifdef LIMIT_TABLING
    remove_from_global_sg_fr_list(sg_fr);
#endif /* LIMIT_TABLING */
    free_subgoal_frame(sg_fr);
  }

  if (position == TRAVERSE_POSITION_FIRST) {
    sg_node_ptr next_node = TrNode_next(current_node);
    DECREMENT_GLOBAL_TRIE_REFS(TrNode_entry(current_node));
    FREE_SUBGOAL_TRIE_NODE(current_node);
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
    FREE_SUBGOAL_TRIE_NODE(current_node);
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
    FREE_ANSWER_TRIE_NODE(current_node);
    while (next_node) {
      current_node = next_node;
      next_node = TrNode_next(current_node);
      free_answer_trie_branch(current_node, TRAVERSE_POSITION_NEXT);
    }
  } else {
    DECREMENT_GLOBAL_TRIE_REFS(TrNode_entry(current_node));
    FREE_ANSWER_TRIE_NODE(current_node);
  }
  return;
}


void update_answer_trie(sg_fr_ptr sg_fr) {
  ans_node_ptr current_node;

  free_answer_trie_hash_chain(SgFr_hash_chain(sg_fr));
  SgFr_hash_chain(sg_fr) = NULL;
  SgFr_state(sg_fr) += 2;  /* complete --> compiled : complete_in_use --> compiled_in_use */
  current_node = TrNode_child(SgFr_answer_trie(sg_fr));
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
  return;
}


static struct trie_statistics{
  int show;
  long subgoals;
  long subgoals_incomplete;
  long subgoal_trie_nodes;
  long answers;
#ifdef TABLING_INNER_CUTS
  long answers_pruned;
#endif /* TABLING_INNER_CUTS */
  long answers_true;
  long answers_no;
  long answer_trie_nodes;
#ifdef GLOBAL_TRIE
  long global_trie_terms;
  long global_trie_nodes;
#endif /* GLOBAL_TRIE */
} trie_stats;

#define TrStat_show              trie_stats.show
#define TrStat_subgoals          trie_stats.subgoals
#define TrStat_sg_incomplete     trie_stats.subgoals_incomplete
#define TrStat_sg_nodes          trie_stats.subgoal_trie_nodes
#define TrStat_answers           trie_stats.answers
#define TrStat_answers_true      trie_stats.answers_true
#define TrStat_answers_no        trie_stats.answers_no
#define TrStat_answers_pruned    trie_stats.answers_pruned
#define TrStat_ans_nodes         trie_stats.answer_trie_nodes
#define TrStat_gt_terms          trie_stats.global_trie_terms
#define TrStat_gt_nodes          trie_stats.global_trie_nodes

#define SHOW_TABLE_STRUCTURE(MESG, ARGS...)  if (TrStat_show == SHOW_MODE_STRUCTURE) fprintf(Yap_stdout, MESG, ##ARGS)
#define STR_ARRAY_SIZE  100000
#define ARITY_ARRAY_SIZE 10000


void show_table(tab_ent_ptr tab_ent, int show_mode) {
  sg_node_ptr sg_node;

  TrStat_show = show_mode;
  if (show_mode == SHOW_MODE_STATISTICS) {
    TrStat_subgoals = 0;
    TrStat_sg_incomplete = 0;
    TrStat_sg_nodes = 1;
    TrStat_answers = 0;
    TrStat_answers_true = 0;
    TrStat_answers_no = 0;
#ifdef TABLING_INNER_CUTS
    TrStat_answers_pruned = 0;
#endif /* TABLING_INNER_CUTS */
    TrStat_ans_nodes = 0;
    fprintf(Yap_stdout, "Table statistics for predicate '%s/%d'\n", AtomName(TabEnt_atom(tab_ent)), TabEnt_arity(tab_ent));
  } else { /* show_mode == SHOW_MODE_STRUCTURE */
    fprintf(Yap_stdout, "Table structure for predicate '%s/%d'\n", AtomName(TabEnt_atom(tab_ent)), TabEnt_arity(tab_ent));
  }
  sg_node = TrNode_child(TabEnt_subgoal_trie(tab_ent));
  if (sg_node) {
    if (TabEnt_arity(tab_ent)) {
      char *str = (char *) malloc(sizeof(char) * STR_ARRAY_SIZE);
      int str_index = sprintf(str, "  ?- %s(", AtomName(TabEnt_atom(tab_ent)));
      int *arity = (int *) malloc(sizeof(int) * ARITY_ARRAY_SIZE);
      arity[0] = 1;
      arity[1] = TabEnt_arity(tab_ent);
      traverse_subgoal_trie(sg_node, str, str_index, arity, TRAVERSE_MODE_NORMAL, TRAVERSE_POSITION_FIRST);
      free(str);
      free(arity);
    } else {
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
    fprintf(Yap_stdout, "  Subgoal trie structure\n");
    fprintf(Yap_stdout, "    Subgoals: %ld (%ld incomplete)\n", TrStat_subgoals, TrStat_sg_incomplete);
    fprintf(Yap_stdout, "    Subgoal trie nodes: %ld\n", TrStat_sg_nodes);
    fprintf(Yap_stdout, "  Answer trie structure(s)\n");
#ifdef TABLING_INNER_CUTS
    fprintf(Yap_stdout, "    Answers: %ld (%ld pruned)\n", TrStat_answers, TrStat_answers_pruned);
#else
    fprintf(Yap_stdout, "    Answers: %ld\n", TrStat_answers);
#endif /* TABLING_INNER_CUTS */
    fprintf(Yap_stdout, "    Answers 'TRUE': %ld\n", TrStat_answers_true);
    fprintf(Yap_stdout, "    Answers 'NO': %ld\n", TrStat_answers_no);
    fprintf(Yap_stdout, "    Answer trie nodes: %ld\n", TrStat_ans_nodes);
    fprintf(Yap_stdout, "  Total memory in use: %ld bytes\n",
	    sizeof(struct table_entry) + TrStat_sg_nodes * sizeof(struct subgoal_trie_node) +
	    TrStat_ans_nodes * sizeof(struct answer_trie_node) + TrStat_subgoals * sizeof(struct subgoal_frame)); 
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
      FREE_ANSWER_TRIE_NODE(current_node);
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
      FREE_ANSWER_TRIE_NODE(current_node);
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
void traverse_subgoal_trie(sg_node_ptr current_node, char *str, int str_index, int *arity, int mode, int position) {
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
    do {
      if (*bucket) {
        traverse_subgoal_trie(*bucket, str, str_index, arity, mode, TRAVERSE_POSITION_FIRST);
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
  TrStat_sg_nodes++;
#ifdef GLOBAL_TRIE
  traverse_global_trie_for_subgoal(TrNode_entry(current_node), str, &str_index, arity, &mode);
#else
  traverse_trie_node(TrNode_entry(current_node), str, &str_index, arity, &mode, TRAVERSE_TYPE_SUBGOAL);
#endif /* GLOBAL_TRIE */

  /* continue with child node ... */
  if (arity[0] != 0)
    traverse_subgoal_trie(TrNode_child(current_node), str, str_index, arity, mode, TRAVERSE_POSITION_FIRST);
  /* ... or show answers */
  else {
    sg_fr_ptr sg_fr = (sg_fr_ptr) TrNode_sg_fr(current_node);
    
    TrStat_subgoals++;
    str[str_index] = 0;
    SHOW_TABLE_STRUCTURE("%s.\n", str);
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
      traverse_answer_trie(TrNode_child(SgFr_answer_trie(sg_fr)), &str[str_index], 0, arity, 0, TRAVERSE_MODE_NORMAL, TRAVERSE_POSITION_FIRST);
      if (SgFr_state(sg_fr) < complete) {
	      TrStat_sg_incomplete++;
	      SHOW_TABLE_STRUCTURE("    ---> INCOMPLETE\n");
      }
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
      traverse_subgoal_trie(current_node, str, str_index, arity, mode, TRAVERSE_POSITION_NEXT);
      current_node = TrNode_next(current_node);
    }
    free(current_arity);
  }

  return;
}


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
  TrStat_ans_nodes++;
#ifdef GLOBAL_TRIE
  traverse_global_trie_for_answer(TrNode_entry(current_node), str, &str_index, arity, &mode);
#else
  traverse_trie_node(TrNode_entry(current_node), str, &str_index, arity, &mode, TRAVERSE_TYPE_ANSWER);
#endif /* GLOBAL_TRIE */

  /* show answer .... */
  if (IS_ANSWER_LEAF_NODE(current_node)) {
    TrStat_answers++;
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


static
void traverse_trie_node(Term t, char *str, int *str_index_ptr, int *arity, int *mode_ptr, int type) {
  int mode = *mode_ptr;
  int str_index = *str_index_ptr;

  /* test the node type */
  if (mode == TRAVERSE_MODE_FLOAT) {
#if SIZEOF_DOUBLE == 2 * SIZEOF_INT_P
    arity[0]++;
    arity[arity[0]] = (int) t;
    mode = TRAVERSE_MODE_FLOAT2;
  } else if (mode == TRAVERSE_MODE_FLOAT2) {
    volatile Float dbl;
    volatile Term *t_dbl = (Term *)((void *) &dbl);
    *t_dbl = t;
    *(t_dbl + 1) = (Term) arity[arity[0]];
    arity[0]--;
#else /* SIZEOF_DOUBLE == SIZEOF_INT_P */
    volatile Float dbl;
    volatile Term *t_dbl = (Term *)((void *) &dbl);
    *t_dbl = t;
#endif /* SIZEOF_DOUBLE x SIZEOF_INT_P */
    str_index += sprintf(& str[str_index], "%.15g", dbl);
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
    if (type == TRAVERSE_TYPE_SUBGOAL)
      mode = TRAVERSE_MODE_NORMAL;
    else  /* type == TRAVERSE_TYPE_ANSWER */
#endif /* GLOBAL_TRIE */
      mode = TRAVERSE_MODE_FLOAT_END;
  } else if (mode == TRAVERSE_MODE_FLOAT_END) {
    mode = TRAVERSE_MODE_NORMAL;
  } else if (mode == TRAVERSE_MODE_LONG) {
    Int li = (Int) t;
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
    if (type == TRAVERSE_TYPE_SUBGOAL)
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
  traverse_trie_node(TrNode_entry(current_node), str, &str_index, arity, &mode, TRAVERSE_TYPE_SUBGOAL);

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
  traverse_trie_node(TrNode_entry(current_node), str, str_index, arity, mode, TRAVERSE_TYPE_SUBGOAL);
  return;
}


static
void traverse_global_trie_for_answer(gt_node_ptr current_node, char *str, int *str_index, int *arity, int *mode) {
  if (TrNode_parent(current_node) != GLOBAL_root_gt)
    traverse_global_trie_for_answer(TrNode_parent(current_node), str, str_index, arity, mode);
  traverse_trie_node(TrNode_entry(current_node), str, str_index, arity, mode, TRAVERSE_TYPE_ANSWER);
  return;
}
#endif /* GLOBAL_TRIE */
#endif /* TABLING */
