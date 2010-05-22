/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   Flávio Cruz - University of Porto, Portugal
  File:        tab.retroactive.c
                                                                     
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
    printSubgoalTriePathAll(stdout, btn, tab_ent);
    printf("\n");
  }
#endif
  
  if(path_type == NO_PATH) { /* new producer */
    sg_node_ptr leaf = variant_call_cont_insert(tab_ent, (sg_node_ptr)stl_restore_variant_cont(),
      variant_cont.bindings.num, CALL_SUB_TRIE_NT);
      
    sg_fr = create_new_retroactive_producer_subgoal(leaf, tab_ent, code);
    
    Trail_Unwind_All;
    
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
        break;
      case SUBSUMPTIVE_PATH:
        if(SgFr_state(subsumer) < complete || retroactive_table_load(tab_ent)) {
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
      }
    }
  }
  
  return (sg_fr_ptr)sg_fr;
}

#define THRESHOLD_HASHTABLE   8
#define THRESHOLD_BUCKET      (THRESHOLD_HASHTABLE / 2)
#define HASH_ELEM(ELEM, SEED) ((unsigned int)(ELEM) & (SEED))

static inline int
find_elem_list(node_list_ptr list, ans_node_ptr ans, int *total) {
  /* try to locate answer */
  while(list) {
    ans_node_ptr elem = NodeList_node(list);
    
    if(elem == ans) {
      break;
    }
    
    *total = *total + 1;
    list = NodeList_next(list);
  }
  
  return list != NULL;
}

static inline void
insert_list_into_hash(node_list_ptr list, retro_leaf_ptr hash) {
  node_list_ptr *bucket;
  
  while(list) {
    node_list_ptr next = NodeList_next(list);
    
    /* add 'list' to hash table */
    bucket = Hash_bucket(hash, HASH_ELEM(NodeList_node(list), Hash_seed(hash)));
    NodeList_next(list) = *bucket;
    *bucket = list;
    list = next;
  }
}

static inline retro_leaf_ptr
create_hash_table(node_list_ptr list, int total) {
  retro_leaf_ptr hash;
  
  ALLOC_RETRO_LEAF_INDEX(hash);
  LeafIndex_mark_hash(hash);
  Hash_num_buckets(hash) = BASE_HASH_BUCKETS;
  Hash_num_sgs(hash) = total;
  ALLOC_HASH_BUCKETS(Hash_buckets(hash), BASE_HASH_BUCKETS);
  
  /* insert nodes */
  insert_list_into_hash(list, hash);
  
  return hash;
}

static node_list_ptr
new_list_elem(ans_node_ptr elem, node_list_ptr next) {
  node_list_ptr new_node;
  
  ALLOC_NODE_LIST(new_node);
    
  NodeList_next(new_node) = next;
  NodeList_node(new_node) = elem;
  
  return new_node;
}

static inline void
expand_hash_table(retro_leaf_ptr hash) {
  node_list_ptr *old_bucket = Hash_buckets(hash);
  node_list_ptr *save_old = old_bucket;
  node_list_ptr *end_old_bucket = old_bucket + Hash_num_buckets(hash);
  
  Hash_num_buckets(hash) *= 2;
  ALLOC_HASH_BUCKETS(Hash_buckets(hash), Hash_num_buckets(hash));
  
  while(old_bucket != end_old_bucket) {
    if(*old_bucket)
      insert_list_into_hash(*old_bucket, hash);
    ++old_bucket;
  }
  
  FREE_HASH_BUCKETS(save_old);
}

static inline int
mark_answer_hash(tst_node_ptr node, retroactive_fr_ptr sf) {
  retro_leaf_ptr hash = (retro_leaf_ptr)TrNode_child(node);
  node_list_ptr *bucket;
  
  bucket = Hash_bucket(hash, HASH_ELEM(sf, Hash_seed(hash)));
  
  int total = 0;
  if(find_elem_list(*bucket, (ans_node_ptr)sf, &total))
    return FALSE;
  
  node_list_ptr new_node = new_list_elem((ans_node_ptr)sf, *bucket);
  ++total;
  *bucket = new_node;
  
  Hash_num_sgs(hash)++;
  
  if(total > THRESHOLD_BUCKET)
    expand_hash_table((retro_leaf_ptr)TrNode_child(node));
  
  return TRUE;
}

/* returns TRUE if new answer */
int
mark_answer_subgoal(tst_node_ptr node, retroactive_fr_ptr sf) {
  node_list_ptr list = (node_list_ptr)TSTN_child(node);
  
  if(list != NULL && LeafIndex_is_hash((retro_leaf_ptr)list))
    return mark_answer_hash(node, sf);

  int total = 0;
  
  if(find_elem_list(list, (ans_node_ptr)sf, &total))
    return FALSE;
  
  /* not found, add */
  node_list_ptr new_node = new_list_elem((ans_node_ptr)sf, (node_list_ptr)TSTN_child(node));
  TSTN_child(node) = (tst_node_ptr)new_node;
  ++total;
  
  if(total > THRESHOLD_HASHTABLE) {
    TSTN_child(node) =
        (tst_node_ptr)create_hash_table((node_list_ptr)TSTN_child(node), total);
  }
  
  return TRUE;
}

static inline int
locate_pending_answers_hash(tst_node_ptr node, retro_leaf_ptr hash)
{
  node_list_ptr *bucket;
  
  bucket = Hash_bucket(hash, HASH_ELEM(node, Hash_seed(hash)));
  
  node_list_ptr list = *bucket;
  node_list_ptr before = NULL;
  
  while(list) {
    tst_node_ptr elem = (tst_node_ptr)NodeList_node(list);
    
    if(elem == node) {
      /* found */
      if(before == NULL)
        *bucket = NodeList_next(list);
      else
        NodeList_next(before) = NodeList_next(list);
      
      FREE_NODE_LIST(list);
      return TRUE;
    }
    
    before = list;
    list = NodeList_next(list);
  }
  
  return FALSE;
}

static inline int
locate_pending_answers(tst_node_ptr node, retroactive_fr_ptr sf)
{
  node_list_ptr list = SgFr_pending_answers(sf);
  
  if(!list) {
    return FALSE;
  }
    
  if(LeafIndex_is_hash((retro_leaf_ptr)list))
    return locate_pending_answers_hash(node, (retro_leaf_ptr)list);
    
  node_list_ptr before = NULL;
  
  while(list) {
    tst_node_ptr elem = (tst_node_ptr)NodeList_node(list);
    
    if(elem == node) {
      /* found */
      dprintf("found answer\n");
      if(before == NULL)
        SgFr_pending_answers(sf) = NodeList_next(list);
      else
        NodeList_next(before) = NodeList_next(list);
      if(SgFr_pending_answers(sf) == NULL)
        dprintf("pending is NULL\n");
      FREE_NODE_LIST(list);
      return TRUE;
    }
    
    before = list;
    list = NodeList_next(list);
  }
  
  dprintf("not found answer\n");
  return FALSE;
}

static inline void
add_answer_hash(tst_node_ptr node, retroactive_fr_ptr sf)
{
  retro_leaf_ptr hash = (retro_leaf_ptr)SgFr_pending_answers(sf);
  node_list_ptr *bucket;
  
  bucket = Hash_bucket(hash, HASH_ELEM((ans_node_ptr)node, Hash_seed(hash)));
  
  node_list_ptr list = *bucket;
  int total = 0;
  
  while(list) {
    ++total;
    
    if(!NodeList_next(list))
      break;
    
    list = NodeList_next(list);
  }
  
  if(list)
    NodeList_next(list) = new_list_elem((ans_node_ptr)node, NULL);
  else
    *bucket = new_list_elem((ans_node_ptr)node, NULL);
  
  ++total;
  Hash_num_sgs(hash)++;
  
  if(total > THRESHOLD_BUCKET)
    expand_hash_table((retro_leaf_ptr)SgFr_pending_answers(sf));
}

void
add_answer_pending(tst_node_ptr node, retroactive_fr_ptr sf)
{
  dprintf("Add answer pending!\n");
  node_list_ptr list = SgFr_pending_answers(sf);
  
  if(list != NULL && LeafIndex_is_hash((retro_leaf_ptr)list))
  {
    add_answer_hash(node, sf);
    return;
  }
  
  int total = 0;
  
  while(list) {
    ++total;
    
    if(!NodeList_next(list))
      break;

    list = NodeList_next(list);
  }
  
  if(list)
    NodeList_next(list) = new_list_elem((ans_node_ptr)node, NULL);
  else
    SgFr_pending_answers(sf) = new_list_elem((ans_node_ptr)node, NULL);
  
  ++total;
  
  if(total > THRESHOLD_HASHTABLE) {
    SgFr_pending_answers(sf) =
        (node_list_ptr)create_hash_table(SgFr_pending_answers(sf), total);
  }
}

#define GetTimeStamp(NODE, TAB_ENT) ((IsHashedNode(NODE) && TabEnt_proper_consumers(TAB_ENT))  ?  \
            TSIN_time_stamp((tst_index_ptr)(TSTN_time_stamp(NODE))) :                             \
                TSTN_time_stamp(NODE))

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
  
  auto_update_instructions = FALSE;
  
  time_stamp old_timestamp = TabEnt_retroactive_time_stamp(tab_ent);
  time_stamp new_timestamp;

  tstn = subsumptive_tst_search(root, arity, answerVector, (int)TabEnt_proper_consumers(tab_ent));
  
  new_timestamp = TabEnt_retroactive_time_stamp(tab_ent);
  
#if 1
  /*dprintf("old_timestamp %d new_timestamp %d sf timestamp %d ans %d ts %lu\n",
    (int)old_timestamp, (int)new_timestamp, (int)SgFr_timestamp(sf), (int)tstn, GetTimeStamp(tstn, tab_ent));*/

  if(old_timestamp == new_timestamp-1 && SgFr_timestamp(sf) == old_timestamp) {
    /* ok answer! */
    dprintf("ok answer\n");
    TrNode_unset_is_ans(tstn);
    SgFr_timestamp(sf) = new_timestamp;
  } else if(old_timestamp == new_timestamp &&
      SgFr_timestamp(sf) == old_timestamp-1 && GetTimeStamp(tstn, tab_ent) == new_timestamp)
  {
    dprintf("one answer inserted by someone else\n");
    TrNode_unset_is_ans(tstn);
    SgFr_timestamp(sf) = new_timestamp;
  } else if(GetTimeStamp(tstn, tab_ent) == SgFr_timestamp(sf)) {
    TrNode_set_ans(tstn);
  } else if(GetTimeStamp(tstn, tab_ent) < SgFr_timestamp(sf)) {
    dprintf("answer old for trie\n");
    /* answer is old for the trie */
    if(locate_pending_answers(tstn, sf))
      TrNode_unset_is_ans(tstn); /* new answer */
    else
      TrNode_set_ans(tstn); /* old answer */
  } else {
    /* multiple answers were inserted */
    ensure_has_proper_consumers(tab_ent);

    dprintf("multiple answers were inserted\n");
    CELL *answer_template = SgFr_answer_template(sf);
    const int size = SgFr_at_size(sf);

    AT = answer_template;
    AT_SIZE = SgFr_at_full_size(sf);
    
    tst_collect_relevant_answers(root, SgFr_timestamp(sf), size,
            STANDARDIZE_AT_PTR(answer_template, size), (sg_fr_ptr)sf, (int)tstn);
    
    /* approve this answer */
    TrNode_unset_is_ans(tstn);
    SgFr_timestamp(sf) = new_timestamp;
  }
#else
  SgFr_timestamp(sf) = new_timestamp;
#endif
  
  Trail_Unwind_All;
  return tstn;
}

#endif /* TABLING_RETROACTIVE */
