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
#include "tab.utils.h"
#include "tab.macros.h"
#include "tab.tries.h"

#include "xsb.at.c"

static inline sg_fr_ptr
get_subgoal_frame_from_node(sg_node_ptr leaf_node, tab_ent_ptr tab_ent, yamop *code)
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
  } else {
    sg_fr = (sg_fr_ptr) TrNode_sg_fr(leaf_node);
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
variant_call_cont_insert(tab_ent_ptr tab_ent, sg_node_ptr current_node, int count_vars, int flags) {
  int node_type;
  Term symbol;
  Int li;
  Float flt;
  
  while(!TermStack_IsEmpty) {
    ProcessNextSubtermFromTrieStacks(symbol,node_type,count_vars)
    SUBGOAL_TOKEN_CHECK_INSERT(tab_ent, current_node, symbol, node_type | flags);
  }
  
  return current_node;
}

sg_fr_ptr
variant_call_search(yamop *code, CELL *local_stack, CELL **new_local_stack) {
  tab_ent_ptr tab_ent = CODE_TABLE_ENTRY(code);
  int arity = CODE_ARITY(code);
  sg_node_ptr top_node = TabEnt_subgoal_trie(tab_ent);
  sg_node_ptr leaf;
  sg_fr_ptr sg_fr;
  
  Trail_ResetTOS;
  TermStack_ResetTOS;
  TermStack_PushLowToHighVector(XREGS + 1, arity); /* push goal arguments */
  
  /* insert / check variant path */
  leaf = variant_call_cont_insert(tab_ent, top_node, 0, CALL_TRIE_NT);
  
  /* build substitution factor */
  *new_local_stack = extract_template_from_insertion(local_stack);
  
  /* get or create a subgoal frame */
  sg_fr = get_subgoal_frame_from_node(leaf, tab_ent, code);
  
  Trail_Unwind_All;
  
  return sg_fr;
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

void consume_variant_answer(ans_node_ptr current_ans_node, int subs_arity, CELL *subs_ptr) {
  CELL *stack_vars_base, *stack_vars, *stack_terms_base, *stack_terms;
  int i, vars_arity = 0;
  Term t;
#ifdef GLOBAL_TRIE
  gt_node_ptr current_node;
#else
#define current_node current_ans_node
#endif /* GLOBAL_TRIE */
#ifdef TRIE_COMPACT_PAIRS
  int stack_terms_pair_offset = 0;
#endif /* TRIE_COMPACT_PAIRS */

  if(subs_arity == 0)
    return;

#ifdef TABLING_ERRORS
  if (H < H_FZ)
    TABLING_ERROR_MESSAGE("H < H_FZ (consume_variant_answer)");
#endif /* TABLING_ERRORS */
  stack_vars_base = stack_vars = (CELL *)TR;
  stack_terms_base = stack_terms = (CELL *)Yap_TrailTop;
#ifdef GLOBAL_TRIE
  for (i = subs_arity; i >= 1; i--) {
    current_node = TrNode_entry(current_ans_node);
    current_ans_node = TrNode_parent(current_ans_node);
    t = TrNode_entry(current_node);
    current_node = TrNode_parent(current_node);
#else
  {
#endif /* GLOBAL_TRIE */
    while(!TrNode_is_root(current_node)) {
      t = TrNode_entry(current_node);
      
#ifdef TABLING_CALL_SUBSUMPTION
      if(TrNode_is_long(current_node)) {
        t = MkLongIntTerm(TSTN_long_int((long_tst_node_ptr)current_node));
    	  STACK_CHECK_EXPAND(stack_terms, stack_vars, stack_terms_base);
    	  STACK_PUSH_UP(t, stack_terms);
    	} else if(TrNode_is_float(current_node)) {
        t = MkFloatTerm(TSTN_float((float_tst_node_ptr)current_node));
        STACK_CHECK_EXPAND(stack_terms, stack_vars, stack_terms_base);
        STACK_PUSH_UP(t, stack_terms);
      } else
#endif /* TABLING_CALL_SUBSUMPTION */
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
            current_node = TrNode_parent(current_node);
            t = TrNode_entry(current_node);
            *t_dbl = t;
#if SIZEOF_DOUBLE == 2 * SIZEOF_INT_P
            current_node = TrNode_parent(current_node);
            t = TrNode_entry(current_node);
            *(t_dbl + 1) = t;
#endif /* SIZEOF_DOUBLE x SIZEOF_INT_P */
            current_node = TrNode_parent(current_node);
            t = MkFloatTerm(dbl);
            STACK_CHECK_EXPAND(stack_terms, stack_vars, stack_terms_base);
            STACK_PUSH_UP(t, stack_terms);
          } else if (f == FunctorLongInt) {
            current_node = TrNode_parent(current_node);
            
            Int li = TrNode_entry(current_node);
            
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
        
        current_node = TrNode_parent(current_node);
      }
    }

  for (i = subs_arity - 1; i >= 0; i--) {
    CELL *subs_var = (CELL *) *(subs_ptr + i);
    t = STACK_POP_DOWN(stack_terms);
    Bind(subs_var, t);
  }

#ifdef TABLING_ERRORS
  if (stack_terms != (CELL *)Yap_TrailTop)
    TABLING_ERROR_MESSAGE("stack_terms != Yap_TrailTop (consume_variant_answer)");
#endif /* TABLING_ERRORS */

  return;
#ifndef GLOBAL_TRIE
#undef current_node
#endif /* GLOBAL_TRIE */
}


#endif /* TABLING */
