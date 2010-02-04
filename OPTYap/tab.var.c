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

Cell* construct_variant_answer_template(Cell *var_vector) {
  CPtr *binding;
  int i;
  
  for(i = 0, binding = Trail_Base; binding < Trail_Top; binding++, i++) {
    *--var_vector = (CELL)*binding;
  }
  *--var_vector = i;
  
  return var_vector;
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
    new_variant_subgoal_frame(sg_fr, code);
    TrNode_sg_fr(leaf_node) = (sg_node_ptr) sg_fr;
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
  CallResults_var_vector(results) = construct_variant_answer_template(CallInfo_var_vector(call_info));
  
  /* get or create a subgoal frame */
  CallResults_subgoal_frame(results) = get_subgoal_frame_from_node(CallResults_leaf(results), tab_ent,
        CallInfo_code(call_info), &CallResults_variant_found(results));
  
  Trail_Unwind_All;
}


#endif /* TABLING */