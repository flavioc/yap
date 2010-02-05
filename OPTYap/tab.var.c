/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   R. Rocha and NCC - University of Porto, Portugal
  File:        tab.var.c
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
#include "tab.var.h"
#include "tab.tries.h"

inline
CPtr extract_template_from_insertion(CTXTdeclc CPtr ans_tmplt) {
  int i;
  
  i = 0;
  while(i < (int)Trail_NumBindings)
    *ans_tmplt-- = (Cell)Trail_Base[i++];
  *ans_tmplt = makeint(i);
  return ans_tmplt;
}

static inline
sg_fr_ptr
get_subgoal_frame_from_node(sg_node_ptr leaf_node, tab_ent_ptr tab_ent, yamop *code, int* new)
{
  sg_fr_ptr sg_fr;
  
#if defined(TABLE_LOCK_AT_NODE_LEVEL)
  LOCK(TrNode_lock(leaf_node));
#elif defined(TABLE_LOCK_AT_WRITE_LEVEL)
  LOCK_TABLE(leaf_node);
#endif /* TABLE_LOCK_LEVEL */

  if (TrNode_sg_fr(leaf_node) == NULL) {
    /* new tabled subgoal */
    new_variant_subgoal_frame(sg_fr, code, leaf_node);
    //printf("New subgoal frame... %x at node %x\n", sg_fr, current_node);
    *new = TRUE;
  } else {
    *new = FALSE;
    sg_fr = (sg_fr_ptr) TrNode_sg_fr(leaf_node);
    //printf("Node already present... %x\n", sg_fr);
#ifdef LIMIT_TABLING
    if (SgFr_state(sg_fr) <= ready) {  /* incomplete or ready */
      remove_from_global_sg_fr_list(sg_fr);
    }
#endif /* LIMIT_TABLING */
  }
  
  /* unlock table entry */
#if defined(TABLE_LOCK_AT_ENTRY_LEVEL)
  UNLOCK(TabEnt_lock(tab_ent));
#elif defined(TABLE_LOCK_AT_NODE_LEVEL)
  UNLOCK(TrNode_lock(current_node));
#elif defined(TABLE_LOCK_AT_WRITE_LEVEL)
  UNLOCK_TABLE(current_node);
#endif /* TABLE_LOCK_LEVEL */

  return sg_fr;
}

/* trail stack must be unwind later!! */
sg_node_ptr
variant_call_cont_insert(tab_ent_ptr tab_ent, sg_node_ptr current_node, int count_vars) {
  Term t;
  int j;
  
  while(!TermStack_IsEmpty) {
    TermStack_Pop(t);
    t = Deref(t);

    if (IsVarTerm(t)) {
      if (IsTableVarTerm(t)) {
        t = MakeTableVarTerm(VarIndexOfTerm(t));
        SUBGOAL_TOKEN_CHECK_INSERT(tab_ent, current_node, t);
      } else {
        if (count_vars == MAX_TABLE_VARS)
          Yap_Error(INTERNAL_ERROR, TermNil, "MAX_TABLE_VARS exceeded (subgoal_search)");
        Trail_Push(t);
        *((CELL *)t) = GLOBAL_table_var_enumerator(count_vars);
        t = MakeNewTableVarTerm(count_vars); /* new variable */
        count_vars++;
        SUBGOAL_TOKEN_CHECK_INSERT(tab_ent, current_node, t);
      }
    } else if (IsAtomOrIntTerm(t)) {
      SUBGOAL_TOKEN_CHECK_INSERT(tab_ent, current_node, t);
    } else if (IsPairTerm(t)) {
      SUBGOAL_TOKEN_CHECK_INSERT(tab_ent, current_node, AbsPair(NULL));

      TermStack_Push(*(RepPair(t) + 1));
      TermStack_Push(*(RepPair(t)));
    } else if (IsApplTerm(t)) {
      Functor f = FunctorOfTerm(t);
      SUBGOAL_TOKEN_CHECK_INSERT(tab_ent, current_node, AbsAppl((Term *)f));
      if (f == FunctorDouble) {
        volatile Float dbl = FloatOfTerm(t);
        volatile Term *t_dbl = (Term *)((void *) &dbl);
#if SIZEOF_DOUBLE == 2 * SIZEOF_INT_P
        SUBGOAL_TOKEN_CHECK_INSERT(tab_ent, current_node, *(t_dbl + 1));
#endif /* SIZEOF_DOUBLE x SIZEOF_INT_P */
        SUBGOAL_TOKEN_CHECK_INSERT(tab_ent, current_node, *t_dbl);
      } else if (f == FunctorLongInt) {
        Int li = LongIntOfTerm(t);
        SUBGOAL_TOKEN_CHECK_INSERT(tab_ent, current_node, li);
      } else if (f == FunctorDBRef) {
        Yap_Error(INTERNAL_ERROR, TermNil, "unsupported type tag (FunctorDBRef in subgoal_search)");
      } else if (f == FunctorBigInt) {
        Yap_Error(INTERNAL_ERROR, TermNil, "unsupported type tag (FunctorBigInt in subgoal_search)");	  
      } else {
        for (j = ArityOfFunctor(f); j >= 1; j--)
          TermStack_Push(*(RepAppl(t) + j));
      }
    } else {
      Yap_Error(INTERNAL_ERROR, TermNil, "unknown type tag (subgoal_search)");
    }
  }
  
  return current_node;
}

void
variant_call_search(TabledCallInfo *call_info, CallLookupResults *results) {
  tab_ent_ptr tab_ent = CallInfo_table_entry(call_info);
  int arity = CallInfo_arity(call_info);
  sg_node_ptr top_node = TabEnt_subgoal_trie(tab_ent);
  
  Trail_ResetTOS;
  TermStack_ResetTOS;
  TermStack_PushLowToHighVector(XREGS + 1, arity); /* push goal arguments */
  
  /* insert / check variant path */
  CallResults_leaf(results) = variant_call_cont_insert(tab_ent, top_node, 0);
  
  /* build substitution factor */
  CallResults_var_vector(results) = extract_template_from_insertion(CallInfo_var_vector(call_info));
  
  /* get or create a subgoal frame */
  CallResults_subgoal_frame(results) = get_subgoal_frame_from_node(CallResults_leaf(results), tab_ent,
        CallInfo_code(call_info), &CallResults_variant_found(results));
  
  Trail_Unwind_All;
}

ans_node_ptr variant_answer_search(sg_fr_ptr sg_fr, CELL *subs_ptr) {
  int i, j, count_vars, subs_arity;
  CELL *stack_vars, *stack_terms_base, *stack_terms;
  ans_node_ptr current_node;

  count_vars = 0;
  subs_arity = *subs_ptr;
  stack_vars = (CELL *)TR;
  stack_terms_base = stack_terms = (CELL *)Yap_TrailTop;
  current_node = SgFr_answer_trie(sg_fr);
  
  for (i = subs_arity; i >= 1; i--) {
    STACK_CHECK_EXPAND(stack_terms, stack_vars, stack_terms_base);
    STACK_PUSH_UP(Deref(*(subs_ptr + i)), stack_terms);
    do {
      Term t = STACK_POP_DOWN(stack_terms);
      if (IsVarTerm(t)) {
	      t = Deref(t);
	      if (IsTableVarTerm(t)) {
	        t = MakeTableVarTerm(VarIndexOfTerm(t));
          ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, t, _trie_retry_val);
	      } else {
	        if (count_vars == MAX_TABLE_VARS)
	          Yap_Error(INTERNAL_ERROR, TermNil, "MAX_TABLE_VARS exceeded (answer_search)");
	        STACK_PUSH_DOWN(t, stack_vars);
	        *((CELL *)t) = GLOBAL_table_var_enumerator(count_vars);
	        t = MakeTableVarTerm(count_vars);
	        count_vars++;
          ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, t, _trie_retry_var);
	      }
      } else if (IsAtomOrIntTerm(t)) {
        ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, t, _trie_retry_atom);
      } else if (IsPairTerm(t)) {
        ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, AbsPair(NULL), _trie_retry_pair);
	      STACK_CHECK_EXPAND(stack_terms, stack_vars + 1, stack_terms_base);
	      STACK_PUSH_UP(Deref(*(RepPair(t) + 1)), stack_terms);
	      STACK_PUSH_UP(Deref(*(RepPair(t))), stack_terms);
      } else if (IsApplTerm(t)) {
	      Functor f = FunctorOfTerm(t);
	      if (f == FunctorDouble) {
	        volatile Float dbl = FloatOfTerm(t);
	        volatile Term *t_dbl = (Term *)((void *) &dbl);
          ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, AbsAppl((Term *)f), _trie_retry_null);
#if SIZEOF_DOUBLE == 2 * SIZEOF_INT_P
          ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, *(t_dbl + 1), _trie_retry_extension);
#endif /* SIZEOF_DOUBLE x SIZEOF_INT_P */
          ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, *t_dbl, _trie_retry_extension);
          ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, AbsAppl((Term *)f), _trie_retry_float);
	      } else if (f == FunctorLongInt) {
	        Int li = LongIntOfTerm (t);
          ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, AbsAppl((Term *)f), _trie_retry_null);
          ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, li, _trie_retry_extension);
          ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, AbsAppl((Term *)f), _trie_retry_long);
	      } else if (f == FunctorDBRef) {
	        Yap_Error(INTERNAL_ERROR, TermNil, "unsupported type tag (FunctorDBRef in answer_search)");
	      } else if (f == FunctorBigInt) {
	        Yap_Error(INTERNAL_ERROR, TermNil, "unsupported type tag (FunctorBigInt in answer_search)");
	      } else {
          ANSWER_TOKEN_CHECK_INSERT(sg_fr, current_node, AbsAppl((Term *)f), _trie_retry_struct);
          STACK_CHECK_EXPAND(stack_terms, stack_vars + ArityOfFunctor(f) - 1, stack_terms_base);
	        for (j = ArityOfFunctor(f); j >= 1; j--)
	          STACK_PUSH_UP(Deref(*(RepAppl(t) + j)), stack_terms);
	      }
      } else {
	      Yap_Error(INTERNAL_ERROR, TermNil, "unknown type tag (answer_search)");
      }
    } while (STACK_NOT_EMPTY(stack_terms, stack_terms_base));
  }

  /* reset variables */
  while (count_vars--) {
    Term t = STACK_POP_UP(stack_vars);
    RESET_VARIABLE(t);
  }

  return current_node;
}


#endif /* TABLING */