/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   R. Rocha and NCC - University of Porto, Portugal
  File:        tab.subsumption.C 
                                                                     
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
#include "tab.tst.h"
#include "tab.xsb.h"
#include "tab.utils.h"

#include "xsb.lookup.c"

static inline sg_fr_ptr
create_new_consumer_subgoal(sg_node_ptr leaf_node, subprod_fr_ptr subsumer, tab_ent_ptr tab_ent, yamop *code) {
  subcons_fr_ptr sg_fr;
  
#if defined(TABLE_LOCK_AT_NODE_LEVEL)
  LOCK(TrNode_lock(leaf_node));
#elif defined(TABLE_LOCK_AT_WRITE_LEVEL)
  LOCK_TABLE(leaf_node);
#endif /* TABLE_LOCK_LEVEL */

  new_subsumed_consumer_subgoal_frame(sg_fr, code, leaf_node, subsumer);
  
    /* unlock table entry */
#if defined(TABLE_LOCK_AT_ENTRY_LEVEL)
  UNLOCK(TabEnt_lock(tab_ent));
#elif defined(TABLE_LOCK_AT_NODE_LEVEL)
  UNLOCK(TrNode_lock(current_node));
#elif defined(TABLE_LOCK_AT_WRITE_LEVEL)
  UNLOCK_TABLE(current_node);
#endif /* TABLE_LOCK_LEVEL */

  return (sg_fr_ptr)sg_fr;
}

static inline sg_fr_ptr
create_new_producer_subgoal(sg_node_ptr leaf_node, tab_ent_ptr tab_ent, yamop *code)
{
  subprod_fr_ptr sg_fr;
  
#if defined(TABLE_LOCK_AT_NODE_LEVEL)
  LOCK(TrNode_lock(leaf_node));
#elif defined(TABLE_LOCK_AT_WRITE_LEVEL)
  LOCK_TABLE(leaf_node);
#endif /* TABLE_LOCK_LEVEL */
  
  new_subsumptive_producer_subgoal_frame(sg_fr, code, leaf_node);

  /* unlock table entry */
#if defined(TABLE_LOCK_AT_ENTRY_LEVEL)
  UNLOCK(TabEnt_lock(tab_ent));
#elif defined(TABLE_LOCK_AT_NODE_LEVEL)
  UNLOCK(TrNode_lock(current_node));
#elif defined(TABLE_LOCK_AT_WRITE_LEVEL)
  UNLOCK_TABLE(current_node);
#endif /* TABLE_LOCK_LEVEL */
  
  return (sg_fr_ptr)sg_fr;
}

static inline
CPtr construct_variant_answer_template_from_sub(CELL *var_vector) {
  CPtr *binding, termptr;
  int i;
  
  for(i = 0, binding = Trail_Base; binding < Trail_Top; binding++) {
    termptr = *binding;
    
    if(!IsUnboundTrieVar(termptr)) {
      *var_vector-- = (CELL)termptr;
      ++i;
    }
  }
  
  *var_vector = makeint(i);
  
  return var_vector;
}

static inline
CPtr extract_template_from_lookup(CTXTdeclc CPtr ans_tmplt) {
  int i;
  
  i = 0;
  while(TrieVarBindings[i] != (Cell)(& TrieVarBindings[i])) {
    *ans_tmplt-- = TrieVarBindings[i++];
  }
  *ans_tmplt = makeint(i);
  return ans_tmplt;
}

static inline
CPtr reconstruct_template_for_producer(CTXTdeclc TabledCallInfo *call_info, SubProdSF subsumer, CPtr ans_tmplt) {
  int sizeAnsTmplt;
  Cell subterm, symbol;
  
  /*
   * Store the symbols along the path of the more general call.
   */
  SymbolStack_ResetTOS;
  SymbolStack_PushPath(subg_leaf_ptr(subsumer));
  
  /*
   * Push the arguments of the subsumed call.
   */
  TermStack_ResetTOS;
  TermStack_PushLowToHighVector(CallInfo_arguments(call_info),
    CallInfo_arity(call_info));
    
  /*
   * Create the answer template while we process.  Since we know we have a
   * more general subsuming call, we can greatly simplify the "matching"
   * process: we know we either have exact matches of non-variable symbols
   * or a variable paired with some subterm of the current call.
   */
  sizeAnsTmplt = 0;
  while(!TermStack_IsEmpty) {
    TermStack_Pop(subterm);
    XSB_Deref(subterm);
    SymbolStack_Pop(symbol);
    if(IsTrieVar(symbol) && IsNewTrieVar(symbol)) {
      *ans_tmplt-- = subterm;
      sizeAnsTmplt++;
    }
    else if(IsTrieFunctor(symbol))
      TermStack_PushFunctorArgs(subterm)
    else if(IsTrieList(symbol))
      TermStack_PushListArgs(subterm)
  }
  *ans_tmplt = makeint(sizeAnsTmplt);
  return ans_tmplt;
}

sg_fr_ptr subsumptive_call_search(yamop *code, CELL *answer_template, CELL **new_local_stack)
{
  tab_ent_ptr tab_ent = CODE_TABLE_ENTRY(code);
  BTNptr btRoot = TabEnt_subgoal_trie(tab_ent);
  BTNptr btn;
  TriePathType path_type;
  sg_fr_ptr sg_fr = NULL;
  
  /* emu/sub_tables_xsb_i.h */
  TermStack_ResetTOS;
  TermStackLog_ResetTOS;
  Trail_ResetTOS;
  TermStack_PushLowToHighVector(CALL_ARGUMENTS(), CODE_ARITY(code));
  
  btn = iter_sub_trie_lookup(CTXTc btRoot, &path_type);
  
#ifdef FDEBUG
  if(btn) {
    dprintf("Subsumption call found: ");
    printSubgoalTriePath(stdout, btn, tab_ent);
    dprintf("\n");
  }
#endif
  
  if(path_type == NO_PATH) { /* new producer */
    Trail_Unwind_All;
    
    sg_node_ptr leaf = variant_call_cont_insert(tab_ent, (sg_node_ptr)stl_restore_variant_cont(),
      variant_cont.bindings.num);
    
    *new_local_stack = extract_template_from_insertion(answer_template);
    sg_fr = create_new_producer_subgoal(leaf, tab_ent, code);
    
    Trail_Unwind_All;
    
    //printSubstitutionFactor(stdout, CallResults_var_vector(results));
  } else { /* new consumer */
    subprod_fr_ptr subsumer;
    sg_fr_ptr found = (sg_fr_ptr)TrNode_sg_fr(btn);
    
    if(SgFr_is_sub_producer(found)) {
      /* consume from 'found' */
      subsumer = (subprod_fr_ptr)found;
      *new_local_stack = extract_template_from_lookup(answer_template);
      Trail_Unwind_All;
    } else {
      Trail_Unwind_All;
      subsumer = SgFr_producer((subcons_fr_ptr)found);
      *new_local_stack = reconstruct_template_for_producer(code, subsumer, answer_template);
    }
    
    switch(path_type) {
      case VARIANT_PATH:
        sg_fr = found;
        break;
      case SUBSUMPTIVE_PATH:
        if(SgFr_state(subsumer) < complete || TabEnt_is_load(tab_ent)) {
            btn = variant_call_cont_insert(tab_ent, (sg_node_ptr)stl_restore_variant_cont(), variant_cont.bindings.num);
            Trail_Unwind_All;
            sg_fr = create_new_consumer_subgoal(btn, subsumer, tab_ent, code);
            fix_answer_template(*new_local_stack);
        } else
          sg_fr = (sg_fr_ptr)subsumer;
        break;
      default:
        /* NOT REACHED */
        break;
    }
    /*
    dprintf("TEMPLATE: ");
    int tmplt_size = (int)*answer_template;
    printAnswerTemplate(stdout, answer_template+tmplt_size, tmplt_size);
    */
  }
  
  return sg_fr;
}

/* vector de respostas é puxado de cima para baixo!! */
TSTNptr subsumptive_tst_search(CTXTdeclc TSTNptr tstRoot, int nTerms, CPtr termVector,
          xsbBool maintainTSI, xsbBool *isNew) {
  TSTNptr tstn = NULL;
  TriePathType path_type;
  
  if(nTerms > 0) {
    Trail_ResetTOS;
    TermStack_ResetTOS;
    TermStack_PushHighToLowVector(termVector,nTerms);
    
    if(IsEmptyTrie(tstRoot)) {
      tstn = tst_insert(CTXTc tstRoot, tstRoot, NO_INSERT_SYMBOL, maintainTSI);
      *isNew = TRUE;
    }
    else {
      TermStackLog_ResetTOS;
      tstn = iter_sub_trie_lookup(CTXTc tstRoot, &path_type);
      if(path_type == NO_PATH) {
        Trail_Unwind_All;
        tstn = tst_insert(CTXTc tstRoot, (TSTNptr)stl_restore_variant_cont(CTXT),
                NO_INSERT_SYMBOL, maintainTSI);
        *isNew = TRUE;
      }
      else
        *isNew = FALSE;
    }
  } else {
    // XXX
    *isNew = TRUE;
    dprintf("nTerms = 0 !!!\n");
    return tstRoot;
  }        
  
  return tstn;
}

/*
 * Create an Empty Answer Set, represented as a Time-Stamped Trie.
 * Note that the root of the TST is labelled with a ret/n symbol,
 * where `n' is the number of terms in an answer.
 */
void *newTSTAnswerSet(void) {
  TSTNptr root;
  
  New_TSTN(root, TST_TRIE_NT, TRIE_ROOT_NT, 0, NULL, NULL);
  TSTN_TimeStamp(root) = EMPTY_TST_TIMESTAMP;
  
  return root;
}

ans_node_ptr subsumptive_answer_search(subprod_fr_ptr sf, CELL *subs_ptr) {
  TSTNptr root, tstn;
  int nTerms = *subs_ptr;
  CPtr answerVector = subs_ptr + nTerms;
  int new;
  int *isNew = &new;
  
  AnsVarCtr = 0;
  
  root = (TSTNptr)subg_ans_root_ptr(sf);
  tstn = subsumptive_tst_search(CTXTc root, nTerms, answerVector,
    (xsbBool)ProducerSubsumesSubgoals(sf), isNew );
  Trail_Unwind_All;
  
  return (ans_node_ptr)tstn;
}

#endif /* TABLING_CALL_SUBSUMPTION */
