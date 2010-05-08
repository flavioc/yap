/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   R. Rocha and NCC - University of Porto, Portugal
  File:        tab.subsumption.c 
                                                                     
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
#include "tab.tries.h"
#include "tab.specific.h"

#include "xsb.lookup.c"

#ifdef TABLING_COMPLETE_TABLE
void
transform_subsumptive_into_ground_trie(subprod_fr_ptr sg_fr)
{
  tab_ent_ptr tab_ent = SgFr_tab_ent(sg_fr);
  ans_node_ptr answer_trie = SgFr_answer_trie(sg_fr);
  
  /* compile answer trie */
  update_answer_trie((sg_fr_ptr)sg_fr);
  
  /* remove answer trie from subgoal */
  SgFr_answer_trie(sg_fr) = NULL;
  
  /* ... and put it on the table entry */
  TabEnt_ground_trie(tab_ent) = (sg_node_ptr)answer_trie;
  
  /* now remove the subgoal trie and its subgoals */
  free_subgoal_trie_from_ground_table(tab_ent);
}
#endif /* TABLING_COMPLETE_TABLE */

static inline sg_fr_ptr
create_new_consumer_subgoal(sg_node_ptr leaf_node, subprod_fr_ptr subsumer, tab_ent_ptr tab_ent, yamop *code) {
  subcons_fr_ptr sg_fr;
  
#if defined(TABLE_LOCK_AT_NODE_LEVEL)
  LOCK(TrNode_lock(leaf_node));
#elif defined(TABLE_LOCK_AT_WRITE_LEVEL)
  LOCK_TABLE(leaf_node);
#endif /* TABLE_LOCK_LEVEL */

  dprintf("CODE: %d\n", (int)code);
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
  
  dprintf("CODE: %d\n", (int)code);
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

/* answer template functions */
#include "xsb.at.c"

void
decrement_generator_path(sg_node_ptr node) {
  while(!TrNode_is_root(node)) {
    if(IsHashedNode(node)) {
      gen_index_ptr gen_index = TrNode_index_node((subg_node_ptr)node);
      if(GNIN_num_gen(gen_index) == 1) {
        gen_index_remove((subg_node_ptr)node, (subg_hash_ptr)TrNode_child(TrNode_parent(node)));
      } else
        GNIN_num_gen(gen_index)--;
    } else
      TrNode_num_gen((subg_node_ptr)node)--;
    
    node = TrNode_parent(node);
  }
  
  TrNode_num_gen((subg_node_ptr)node)--;
}

void
update_generator_path(sg_node_ptr node) {
  while(!TrNode_is_root(node)) {
    if(IsHashedNode(node)) {
      if(TrNode_num_gen((subg_node_ptr)node) == 0)
        gen_index_add((subg_node_ptr)node, (subg_hash_ptr)TrNode_child(TrNode_parent(node)), 1);
      else
        GNIN_num_gen((gen_index_ptr)TrNode_num_gen((subg_node_ptr)node))++;
    } else
      TrNode_num_gen((subg_node_ptr)node)++;
    
    node = TrNode_parent(node);
  }
  
  TrNode_num_gen((subg_node_ptr)node)++;
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
      variant_cont.bindings.num, CALL_SUB_TRIE_NT);
    
    *new_local_stack = extract_template_from_insertion(answer_template);
    sg_fr = create_new_producer_subgoal(leaf, tab_ent, code);

#ifdef TABLING_COMPLETE_TABLE
    /* determine if is most general */
    if(is_most_general_call(leaf, TabEnt_arity(tab_ent)))
      SgFr_set_most_general(sg_fr);
#endif /* TABLING_COMPLETE_TABLE */
    
    Trail_Unwind_All;
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
            btn = variant_call_cont_insert(tab_ent, (sg_node_ptr)stl_restore_variant_cont(), variant_cont.bindings.num, CALL_SUB_TRIE_NT);
            Trail_Unwind_All;
            sg_fr = create_new_consumer_subgoal(btn, subsumer, tab_ent, code);
            SgFr_answer_template((subcons_fr_ptr)sg_fr) = SgFr_at_block((subcons_fr_ptr)sg_fr);
            SgFr_at_size((subcons_fr_ptr)sg_fr) = copy_answer_template(*new_local_stack, SgFr_at_block((subcons_fr_ptr)sg_fr));
        } else
          sg_fr = (sg_fr_ptr)subsumer;
        break;
      default:
        /* NOT REACHED */
        break;
    }
  }

#ifdef TABLING_RETROACTIVE
  if(SgFr_is_sub_consumer(sg_fr) && SgFr_state(sg_fr) < complete)
  {
    int ans_size = (int)**new_local_stack;
    sg_node_ptr leaf = SgFr_leaf(sg_fr);
    int nvars = 0;
    
    while(!IsTrieRoot(leaf)) {
      if(IsNewTrieVar(TrNode_entry(leaf)))
        nvars++;
      leaf = TrNode_parent(leaf);
    }
    
    if(nvars > ans_size) {
      int diff = nvars - ans_size;
      /* must move answer template to give space for a possible generator */
      dprintf("Moved answer template\n");
      memmove(*new_local_stack - diff, *new_local_stack, sizeof(CELL)*(ans_size+1));
      *new_local_stack = *new_local_stack - diff;
    }
    
    dprintf("Num vars: %d Ans Size %d\n", nvars, ans_size);
  }
#endif /* TABLING_RETROACTIVE */
  return sg_fr;
}

/* vector de respostas é puxado de cima para baixo!! */
TSTNptr subsumptive_tst_search(CTXTdeclc TSTNptr tstRoot, int nTerms, CPtr termVector,
          xsbBool maintainTSI) {
  TSTNptr tstn = NULL;
  TriePathType path_type;
  
  if(nTerms > 0) {
    Trail_ResetTOS;
    TermStack_ResetTOS;
    TermStack_PushHighToLowVector(termVector,nTerms);
    
    if(IsEmptyTrie(tstRoot)) {
      tstn = tst_insert(CTXTc tstRoot, tstRoot, NO_INSERT_SYMBOL, maintainTSI);
    }
    else {
      TermStackLog_ResetTOS;
      tstn = iter_sub_trie_lookup(CTXTc tstRoot, &path_type);
      if(path_type == NO_PATH) {
        Trail_Unwind_All;
        tstn = tst_insert(CTXTc tstRoot, (TSTNptr)stl_restore_variant_cont(CTXT),
                NO_INSERT_SYMBOL, maintainTSI);
      }
    }
  } else
    return tstRoot;   
  
  return tstn;
}

#include "xsb.sub.c"

#endif /* TABLING_CALL_SUBSUMPTION */
