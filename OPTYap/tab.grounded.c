/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   Flávio Cruz - University of Porto, Portugal
  File:        tab.grounded.c
                                                                     
**********************************************************************/

/* ------------------ **
**      Includes      **
** ------------------ */

#include "Yap.h"
#ifdef TABLING_RETROACTIVE
 
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
#include "tab.tries.h"

static inline retroactive_fr_ptr
create_new_retroactive_producer_subgoal(sg_node_ptr leaf_node, tab_ent_ptr tab_ent, yamop *code) {
  retroactive_fr_ptr sg_fr;
  
#if defined(TABLE_LOCK_AT_NODE_LEVEL)
  LOCK(TrNode_lock(leaf_node));
#elif defined(TABLE_LOCK_AT_WRITE_LEVEL)
  LOCK_TABLE(leaf_node);
#endif /* TABLE_LOCK_LEVEL */

  new_retroactive_producer_subgoal_frame(sg_fr, code, leaf_node);
  
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

static inline retroactive_fr_ptr
create_new_retroactive_consumer_subgoal(sg_node_ptr leaf_node, tab_ent_ptr tab_ent, yamop *code, retroactive_fr_ptr subsumer) {
  retroactive_fr_ptr sg_fr;
  
#if defined(TABLE_LOCK_AT_NODE_LEVEL)
  LOCK(TrNode_lock(leaf_node));
#elif defined(TABLE_LOCK_AT_WRITE_LEVEL)
  LOCK_TABLE(leaf_node);
#endif /* TABLE_LOCK_LEVEL */

  new_retroactive_consumer_subgoal_frame(sg_fr, code, leaf_node, subsumer);
  
  /* XXX MUST REMOVE THIS */
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

int
is_most_general_call(sg_node_ptr leaf, int arity)
{
  int i, index;
  Term symbol;
  
  for(i = 0; i < arity; ++i) {
    symbol = TrNode_entry(leaf);
    
    if(IsNewTableVarTerm(symbol)) {
      if(IsVarTerm(symbol)) {
        index = VarIndexOfTableTerm(symbol);
        
        if(index != (arity - i - 1))
          return FALSE;
      } else
        return FALSE;
    } else
      return FALSE;
    
    leaf = TrNode_parent(leaf);
  }
  
  return TrNode_is_root(leaf);
}

#ifdef TABLING_COMPLETE_TABLE
void
free_subgoal_trie_from_retroactive_table(tab_ent_ptr tab_ent)
{
  sg_hash_ptr hash;
  sg_node_ptr sg_node;

  hash = TabEnt_hash_chain(tab_ent);
  TabEnt_hash_chain(tab_ent) = NULL;
  free_subgoal_trie_hash_chain(hash);
  
  if(TabEnt_subgoal_trie(tab_ent)) {
    sg_node = TrNode_child(TabEnt_subgoal_trie(tab_ent));
    
    /* keep the root node */
    TrNode_child(TabEnt_subgoal_trie(tab_ent)) = NULL;
    
    if (sg_node) {
      free_subgoal_trie_branch(sg_node, TabEnt_arity(tab_ent), 0, TRAVERSE_POSITION_FIRST);
    }
  }
}
#endif /* TABLING_COMPLETE_TABLE */

CELL*
copy_arguments_as_the_answer_template(CELL *answer_template, int arity)
{
  int i;
  
  for(i = 1; i <= arity; ++i) {
    *answer_template-- = (CELL)Deref(XREGS[i]);
  }
  *answer_template = makeint(arity);
  
  return answer_template;
}

sg_fr_ptr retroactive_call_search(yamop *code, CELL *answer_template, CELL **new_local_stack)
{
  tab_ent_ptr tab_ent = CODE_TABLE_ENTRY(code);
  BTNptr btRoot = TabEnt_subgoal_trie(tab_ent);
  BTNptr btn;
  TriePathType path_type;
  retroactive_fr_ptr sg_fr = NULL;
  int arity = CODE_ARITY(code);
  
  /* create answer template on local stack */
  *new_local_stack = copy_arguments_as_the_answer_template(answer_template, arity);
  
  if(arity == 0) {
    sg_node_ptr leaf = TrNode_child(btRoot);
    
    if(leaf == NULL) {
      leaf = (sg_node_ptr)create_new_retroactive_producer_subgoal(btRoot, tab_ent, code);
    }
    
    return (sg_fr_ptr)leaf;
  }
  
  TermStack_ResetTOS;
  TermStackLog_ResetTOS;
  Trail_ResetTOS;
  TermStack_PushLowToHighVector(CALL_ARGUMENTS(), arity);
  
  btn = iter_sub_trie_lookup(CTXTc btRoot, &path_type);
  Trail_Unwind_All;
  
#ifdef FDEBUG
  if(btn) {
    printf("Subsumption call found: ");
    printSubgoalTriePath(stdout, btn, tab_ent);
    printf("\n");
  }
#endif
  
  if(path_type == NO_PATH) { /* new producer */
    sg_node_ptr leaf = variant_call_cont_insert(tab_ent, (sg_node_ptr)stl_restore_variant_cont(),
      variant_cont.bindings.num, CALL_SUB_TRIE_NT);
      
    sg_fr = create_new_retroactive_producer_subgoal(leaf, tab_ent, code);
    
    Trail_Unwind_All;
    dprintf("New producer\n");
    
    /* determine if is most general */
    if(is_most_general_call(leaf, arity))
      SgFr_set_most_general(sg_fr);
    
    create_retroactive_answer_template(sg_fr, *new_local_stack);
    
  } else { /* new consumer */
    
    retroactive_fr_ptr subsumer;
    retroactive_fr_ptr found = (retroactive_fr_ptr)TrNode_sg_fr(btn);
    
    if(SgFr_is_retroactive_producer(found))
      /* 'found' is generating answers */
      subsumer = found;
    else
      subsumer = SgFr_producer(found);
    
    switch(path_type) {
      case VARIANT_PATH:
        sg_fr = found;
        dprintf("variant path\n");
        break;
      case SUBSUMPTIVE_PATH:
        if(SgFr_state(subsumer) < complete || TabEnt_is_load(tab_ent)) {
            btn = variant_call_cont_insert(tab_ent, (sg_node_ptr)stl_restore_variant_cont(), variant_cont.bindings.num, CALL_SUB_TRIE_NT);
            Trail_Unwind_All;
            
            sg_fr = create_new_retroactive_consumer_subgoal(btn, tab_ent, code, subsumer);
            
            create_retroactive_answer_template(sg_fr, *new_local_stack);
            
            ensure_has_proper_consumers(tab_ent);
        } else
          sg_fr = subsumer;
        break;
      default:
        /* NOT REACHED */
        break;
    }
    
    if(SgFr_is_retroactive_consumer(sg_fr)) {
      retroactive_fr_ptr producer = SgFr_producer(sg_fr);
      if(SgFr_state(producer) < evaluating) {
        /* turn into producer */
        SgFr_set_type(sg_fr, RETROACTIVE_PRODUCER_SFT); 
        SgFr_producer(sg_fr) = NULL;
        dprintf("Producer not evaluating\n");
      }
    }
  }
  
  return (sg_fr_ptr)sg_fr;
}

inline
TSTNptr retroactive_answer_search(retroactive_fr_ptr sf, CPtr answerVector) {

  TSTNptr root, tstn;
  tab_ent_ptr tab_ent = SgFr_tab_ent(sf);
  int arity = TabEnt_arity(tab_ent);

  AnsVarCtr = 0;
  root = (TSTNptr)TabEnt_retroactive_trie(tab_ent);
  
  /* ver TS pois podem ser inseridas respostas por outros geradores... XXX */
  
  if ( IsNULL(root) ) {
    TabEnt_retroactive_trie(tab_ent) = (sg_node_ptr)newTSTAnswerSet();
    root = (TSTNptr)TabEnt_retroactive_trie(tab_ent);
  }
  
  auto_update_instructions = TRUE;
  
  tstn = subsumptive_tst_search(root, arity, answerVector, (int)TabEnt_proper_consumers(tab_ent));
  
  /* update time stamp */
  SgFr_timestamp(sf) = TabEnt_retroactive_time_stamp(tab_ent);
       
  Trail_Unwind_All;
  return tstn;
}

#endif /* TABLING_RETROACTIVE */
