/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   Flávio Cruz
  File:        tab.tst.h
                                                                     
**********************************************************************/

#ifndef TAB_TST_H
#define TAB_TST_H

#include "opt.config.h"
#ifdef TABLING
#include "tab.xsb.h"
#include "tab.utils.h"
#include "tab.structs.h"

/* ----------------------- **
** Time stamped trie nodes **
** ----------------------- */

typedef struct time_stamped_trie_node {
  struct answer_trie_node base; /* basic node info */
  time_stamp ts; /* time stamp */
} *tst_node_ptr;

#define CAST_AN(X)            ((ans_node_ptr)X)

#define TSTN_instr(X)       TrNode_instr(CAST_AN(X))
#define TSTN_or_arg(X)      TrNode_or_arg(CAST_AN(X))
#define TSTN_entry(X)       TrNode_entry(CAST_AN(X))
#define TSTN_lock(X)        TrNode_lock(CAST_AN(X))
#define TSTN_parent(X)      TrNode_parent(CAST_AN(X))
#define TSTN_child(X)       TrNode_child(CAST_AN(X))
#define TSTN_next(X)        TrNode_next(CAST_AN(X))
#define TSTN_time_stamp(X)  ((X)->ts)

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

/* ----------------------- **
** Time stamped hash table **
** ----------------------- */

typedef struct tst_answer_trie_hash {
  struct answer_trie_hash base;
  tst_index_ptr index_head;
  tst_index_ptr index_tail;
} *tst_ans_hash_ptr;

#define CAST_AH(X)        ((ans_hash_ptr)X)

#define TSTHT_mark(X)           (Hash_mark(CAST_AH(X))
#define TSTHT_num_buckets(X)    (Hash_num_buckets(CAST_AH(X)))
#define TSTHT_seed(X)           (Hash_seed(CAST_AH(X)))
#define TSTHT_buckets(X)        (Hash_buckets(CAST_AH(X)))
#define TSTHT_bucket(X,N)       (Hash_buckets(CAST_AH(X)))
#define TSTHT_num_nodes(X)      (Hash_num_nodes(CAST_AH(X))
#define TSTHT_next(X)           (Hash_next(CAST_AH(X)))
#define TSTHT_index_head(X)     ((X)->index_head)
#define TSTHT_index_tail(X)     ((X)->index_tail)

#endif /* TABLING */

#endif