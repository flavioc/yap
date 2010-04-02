/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   Flávio Cruz
  File:        tab.tst.h
                                                                     
**********************************************************************/

#ifndef TAB_TST_H
#define TAB_TST_H

#include "opt.config.h"
#ifdef TABLING_CALL_SUBSUMPTION
#include "tab.xsb.h"
#include "tab.utils.h"
#include "tab.structs.h"

/* ----------------------- **
** Time stamped trie nodes **
** ----------------------- */

typedef struct time_stamped_trie_node *tst_node_ptr;

struct time_stamped_trie_node {
  struct basic_trie_info basic_info;
#ifdef TABLE_LOCK_AT_NODE_LEVEL
  lockvar lock;
#endif /* TABLE_LOCK_AT_NODE_LEVEL */
  tst_node_ptr parent;
  tst_node_ptr child;
  tst_node_ptr next;
  Term entry;
  time_stamp ts; /* time stamp */
};

#define CAST_TSTN(X)        ((tst_node_ptr)(X))

#define TSTN_instr(X)       TrNode_instr(X)
#define TSTN_entry(X)       TrNode_entry(X)
#define TSTN_lock(X)        TrNode_lock(X)
#define TSTN_parent(X)      TrNode_parent(X)
#define TSTN_child(X)       TrNode_child(X)
#define TSTN_next(X)        TrNode_next(X)
#define TSTN_trie_type(X)   TrNode_trie_type(X)
#define TSTN_node_type(X)   TrNode_node_type(X)
#define TSTN_time_stamp(X)  (CAST_TSTN(X)->ts)

/* ---------------------------- **
** special nodes (long + float) **
** ---------------------------- */

EXTEND_STRUCT(time_stamped_trie_node, tst_node_ptr, long, Int long_int);
EXTEND_STRUCT(time_stamped_trie_node, tst_node_ptr, float, Float float_val);

#define TSTN_long_int(X)  TrNode_long_int(X)
#define TSTN_float(X)     TrNode_float(X)

/* ----------------------- **
** Time stamped indexes    **
** ----------------------- */

typedef struct tst_index_node *tst_index_ptr;

struct tst_index_node {
  tst_index_ptr prev;
  tst_index_ptr next;
  time_stamp ts;
  tst_node_ptr node;
};

#define TSIN_node(X)        ((X)->node)
#define TSIN_time_stamp(X)  ((X)->ts)
#define TSIN_prev(X)        ((X)->prev)
#define TSIN_next(X)        ((X)->next)

/* ----------------------- **
** Time stamped hash table **
** ----------------------- */

typedef struct tst_answer_trie_hash *tst_ans_hash_ptr;

struct tst_answer_trie_hash {
  /* equal to tst node */
  struct basic_trie_info basic_info;
  int number_of_buckets;
  tst_node_ptr *buckets;
  int number_of_nodes;
  tst_ans_hash_ptr next;
  
  tst_index_ptr index_head;
  tst_index_ptr index_tail;
};

#define TSTHT_num_buckets(X)    (Hash_num_buckets(X))
#define TSTHT_seed(X)           (Hash_seed(X))
#define TSTHT_buckets(X)        (Hash_buckets(X))
#define TSTHT_bucket(X,N)       (Hash_bucket(X, N))
#define TSTHT_num_nodes(X)      (Hash_num_nodes(X))
#define TSTHT_next(X)           (Hash_next(X))
#define TSTHT_index_head(X)     ((X)->index_head)
#define TSTHT_index_tail(X)     ((X)->index_tail)
#define TSTHT_new_size(X)       (TSTHT_num_buckets(X) << 1)
#define TSTHT_instr(X)          (TrNode_instr(X))
#define TSTHT_node_type(X)      (TrNode_node_type(X))
#define TSTHT_trie_type(X)      (TrNode_trie_type(X))

#include "xsb.tst.h"

TSTNptr tst_insert(CTXTdeclc TSTNptr tstRoot, TSTNptr lastMatch, Cell firstSymbol,
  xsbBool maintainTSI);
TSTNptr new_tstn(CTXTdeclc int trie_t, int node_t, Cell symbol, TSTNptr parent,
  TSTNptr sibling);
void tstCreateTSIs(CTXTdeclc TSTNptr pTST);
void tstht_remove_index(TSTHTptr ht);
void print_hash_table(TSTHTptr ht);

/* --------------------- **
**   Choice points       **
** --------------------- */

struct hash_choicept {
   struct choicept cp;
   tst_node_ptr *last_bucket;
   tst_node_ptr *final_bucket;
};

typedef struct hash_choicept *hash_cp_ptr;

#endif /* TABLING_CALL_SUBSUMPTION */

#endif
