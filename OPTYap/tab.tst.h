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
  int node_type;
  int trie_type;
  int status;
  time_stamp ts; /* time stamp */
} *tst_node_ptr;

#define CAST_AN(X)          ((ans_node_ptr)(X))
#define CAST_TSTN(X)        ((tst_node_ptr)(X))

#define TSTN_instr(X)       TrNode_instr(CAST_AN(X))
#define TSTN_or_arg(X)      TrNode_or_arg(CAST_AN(X))
#define TSTN_entry(X)       TrNode_entry(CAST_AN(X))
#define TSTN_lock(X)        TrNode_lock(CAST_AN(X))
#define TSTN_parent(X)      TrNode_parent(CAST_AN(X))
#define TSTN_child(X)       TrNode_child(CAST_AN(X))
#define TSTN_next(X)        TrNode_next(CAST_AN(X))
#define TSTN_time_stamp(X)  (CAST_TSTN(X)->ts)
#define TSTN_status(X)      (CAST_TSTN(X)->status)
#define TSTN_trie_type(X)   (CAST_TSTN(X)->trie_type)
#define TSTN_node_type(X)   (CAST_TSTN(X)->node_type)

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

typedef struct tst_answer_trie_hash {
  /* equal to tst node */
  struct answer_trie_hash base;
  int node_type;
  int trie_type;
  int status;
  
  tst_index_ptr index_head;
  tst_index_ptr index_tail;
} *tst_ans_hash_ptr;

#define CAST_AH(X)        ((ans_hash_ptr)X)

#define TSTHT_mark(X)           (Hash_mark(CAST_AH(X))
#define TSTHT_num_buckets(X)    (Hash_num_buckets(CAST_AH(X)))
#define TSTHT_seed(X)           (Hash_seed(CAST_AH(X)))
#define TSTHT_buckets(X)        ((TSTNptr*)Hash_buckets(CAST_AH(X)))
#define TSTHT_bucket(X,N)       (Hash_bucket(CAST_AH(X), N))
#define TSTHT_num_nodes(X)      (Hash_num_nodes(CAST_AH(X)))
#define TSTHT_next(X)           (Hash_next(CAST_AH(X)))
#define TSTHT_index_head(X)     ((X)->index_head)
#define TSTHT_index_tail(X)     ((X)->index_tail)
#define TSTHT_new_size(X)       (TSTHT_num_buckets(X) << 1)
#define TSTHT_instr(X)          (TSTN_instr(CAST_TSTN(X)))
#define TSTHT_node_type(X)      (TSTN_node_type(CAST_TSTN(X)))
#define TSTHT_trie_type(X)      (TSTN_trie_type(CAST_TSTN(X)))
#define TSTHT_status(X)         (TSTN_status(CAST_TSTN(X)))

TSTNptr tst_insert(CTXTdeclc TSTNptr tstRoot, TSTNptr lastMatch, Cell firstSymbol,
  xsbBool maintainTSI);

#endif /* TABLING */

#endif