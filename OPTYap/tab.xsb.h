/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   R. Rocha and NCC - University of Porto, Portugal
  File:        tab.xsb.h
                                                                     
**********************************************************************/

#ifndef TAB_XSB_H
#define TAB_XSB_H

#include "opt.config.h"

typedef CELL Cell;
typedef CELL *CPtr;
typedef int xsbBool;
typedef sg_node_ptr BTNptr;

#define CTXTdeclc
#define CTXTdecl void
#define CTXTc
#define CTXT
#define YES TRUE
#define NO  FALSE

#define XSB_Deref(X) ((X) = Deref(X))

#define StandardizeVariable(DerefedVar, Index)  \
    (*((CELL *)DerefedVar) = GLOBAL_table_var_enumerator(Index))
#define IsStandardizedVariable(DerefVar)  (IsTableVarTerm(DerefVar))

#define IndexOfStdVar(VAR_ENUM_ADDR)  VarIndexOfTerm(VAR_ENUM_ADDR)

#define bld_free(ADDR)      *((CELL *)ADDR) = (CELL)(ADDR)

#define int_val(SYMBOL) IntOfTerm(SYMBOL)
#define string_val(SYMBOL)  AtomName(AtomOfTerm(SYMBOL))
#define DecodeTrieFunctor(SYMBOL) ((Functor) RepAppl(SYMBOL))
#define DecodeTrieVar(VAR)      VarIndexOfTableTerm(VAR)
#define get_arity(FUNCTOR)  ArityOfFunctor(FUNCTOR)
#define get_name(FUNCTOR)   AtomName(NameOfFunctor(FUNCTOR))

#define IsTrieRoot(NODE)   TrNode_is_root(NODE)

#define BTN_Symbol(NODE)        TrNode_entry(NODE)
#define BTN_Parent(NODE)        TrNode_parent(NODE)

#define makeint(I)  (I)

#ifdef TABLING_CALL_SUBSUMPTION

typedef unsigned long int counter;
typedef sg_hash_ptr BTHTptr;
typedef Functor Psc;

#define get_str_psc(FUNCTOR) FunctorOfTerm(FUNCTOR)
/* FunctorOfTerm(t) === *RepAppl (t) */
#define clref_val(REF)      RepAppl(REF)
#define clrefp_val(REF)     RepPair(REF)
#define cs_val(REF)	RepAppl(REF)
#define bld_ref(ADDR, VAL)  *((CELL *)ADDR) = (CELL)(VAL)
#define cell(REF) *(REF)

#define IsNonNULL(ptr)   ( (ptr) != NULL )
#define IsNULL(ptr) ((ptr) == NULL)

#define XSB_STRING  TAG_ATOM
#define XSB_INT     TAG_INT
#define XSB_FLOAT   TAG_FLOAT
#define XSB_STRUCT  TAG_STRUCT
#define XSB_LIST    TAG_LIST
#define XSB_REF     TAG_REF
#define XSB_REF1    XSB_REF
#define XSB_TrieVar TAG_TrieVar

#define EncodeTrieConstant(Cell_Const) ((Cell)Cell_Const)
#define EncodeTrieVar(INDEX)      MakeTableVarTerm(INDEX)
#define EncodeNewTrieVar(INDEX)   MakeNewTableVarTerm(INDEX)
#define EncodeTrieFunctor(TERM)   AbsAppl((Term *)FunctorOfTerm(TERM))
#define EncodeTrieList(TERM)  AbsPair(NULL)

/* subgoal frames */
#define BTN_Child(NODE)         TrNode_child(NODE)
#define BTN_Sibling(NODE)       TrNode_next(NODE)
#define BTN_NodeType(NODE)      TrNode_node_type(NODE)

#define IsHashHeader(NODE)      (TrNode_node_type(NODE) & HASH_HEADER_NT)
#define IsHashedNode(NODE)		(TrNode_node_type(NODE) & HASHED_INTERIOR_NT)
#define BTHT_BucketArray(HASH)  Hash_buckets(HASH)
#define BTHT_GetHashSeed(HASH)  Hash_seed(HASH)
#define TrieHash(SYMBOL, SEED)  HASH_ENTRY(SYMBOL, SEED)
#define TRIEVAR_BUCKET          0
#define NUM_TRIEVARS MAX_TABLE_VARS

#define IsTrieVar(SYMBOL)       (IsVarTerm(SYMBOL))
#define IsNewTrieVar(SYMBOL)    (IsNewTableVarTerm(SYMBOL))

#define IsLeafNode(NODE)   TrNode_is_leaf(NODE)
#define IsEscapeNode(NODE)  FALSE
#define ESCAPE_NODE_SYMBOL    (long)0xFFFFFFF
#define IsTrieFunctor(SYMBOL) (cell_tag(SYMBOL) == TAG_STRUCT)
#define IsTrieList(SYMBOL)  IsPairTerm(SYMBOL)

#define xsb_abort(MSG, ...) Yap_Error(FATAL_ERROR, TermNil, MSG, ##__VA_ARGS__)

#define SUBSUMPTION_YAP 1
/* #define SUBSUMPTION_XSB 1 */

#define SubProdSF subprod_fr_ptr

#define subg_leaf_ptr(X)  SgFr_leaf(X)
#define subg_ans_root_ptr(X)  SgFr_answer_trie(X)
#define ProducerSubsumesSubgoals(X) SgFr_subsumes_subgoals(X)

#define TSTNptr tst_node_ptr
#define TSTHTptr tst_ans_hash_ptr
#define TSINptr tst_index_ptr

#define NO_INSERT_SYMBOL 0

#define TSTN_TrieType(X)  TSTN_trie_type(X)
#define TSTN_Child(X) TSTN_child(X)
#define TSTN_TimeStamp(X) TSTN_time_stamp(X)
#define TSTN_Sibling(X) TSTN_next(X)
#define TSTN_GetHashHdr(X)  (TSTHTptr)TN_GetHashHdr(X)
#define TSTN_SetHashHdr(pTSTN,pTSTHT)   TN_SetHashHdr(pTSTN,pTSTHT)
#define TSTN_Parent(X) TSTN_parent(X)
#define TSTN_Symbol(X)	TSTN_entry(X)

#define TN_Child(X) TSTN_child(X)
#define TN_Sibling(X) TSTN_next(X)
#define TN_Parent(X) TSTN_parent(X)
#define TN_Symbol(X)  TSTN_entry(X)
#define TN_Instr(X)   TSTN_instr(X)
#define TN_TrieType(X)  TSTN_trie_type(X)
#define TN_NodeType(X)  TSTN_node_type(X)
#define TN_SetHashHdr(pTN,pTHT)         TN_Child(pTN) = (void *)(pTHT)
#define TN_GetHashHdr(pTN)              TN_Child(pTN)

#define BTN_SetHashHdr(pBTN,pTHT)       TN_SetHashHdr(pBTN,pTHT)
#define BTN_GetHashHdr(pTN)             ((BTHTptr)TN_GetHashHdr(pTN))

#define TrieHT_BucketArray(X) TSTHT_buckets(X)
#define TrieHT_GetHashSeed(X) TSTHT_seed(X)
#define TrieHT_NumContents(X) TSTHT_num_nodes(X)
#define TrieHT_NumBuckets(X) TSTHT_num_buckets(X)
#define TrieHT_NewSize(X)  TSTHT_new_size(X)

#define TSC_NodeType(X) TSTN_node_type(X)

#define TSTHT_NumContents(X) TSTHT_num_nodes(X)
#define TSTHT_BucketArray(X) TSTHT_buckets(X)
#define TSTHT_IndexHead(X)  TSTHT_index_head(X)
#define TSTHT_IndexTail(X)  TSTHT_index_tail(X)
#define TSTHT_InternalLink(X) TSTHT_next(X)
#define TSTHT_GetHashSeed(X) TSTHT_seed(X)
#define TSTHT_Instr(X) TSTHT_instr(X)
#define TSTHT_TrieType(X) TSTHT_trie_type(X)
#define TSTHT_NodeType(X) TSTHT_node_type(X)
#define TSTHT_NumBuckets(X) TSTHT_num_buckets(X)

#define TSIN_TSTNode(X) TSIN_node(X)
#define TSIN_TimeStamp(X) TSIN_time_stamp(X)
#define TSIN_Prev(X)  TSIN_prev(X)
#define TSIN_Next(X)  TSIN_next(X)


#define BTHT_NumBuckets(X)  TSTHT_num_buckets(X)
#define BTHT_Instr(X) TSTHT_instr(X)
#define BTHT_TrieType(X)  TSTHT_trie_type(X)
#define BTHT_NodeType(X)  TSTHT_node_type(X)
#define BTHT_NumContents(X) TSTHT_num_nodes(X)


#define IsEmptyTrie(Root)      IsNULL(TN_Child(Root))

#define TimeStamp time_stamp

#define trie_root 0

#define BUCKET_CONTENT_THRESHOLD MAX_NODES_PER_BUCKET
#define MAX_SIBLING_LEN MAX_NODES_PER_TRIE_LEVEL
#define TrieHT_INIT_SIZE BASE_HASH_BUCKETS
#define hash_opcode _trie_do_hash

#define ALNptr ans_list_ptr
#define TabledCallInfo yamop

#define isref IsVarTerm
#define LOG_DEBUG 0 /* NOT USED */
#define xsb_warn printf
#define stddbg stderr
#define stdwarn stderr
#define xsb_dbgmsg(PARAM) my_xsb_debug PARAM
#define my_xsb_debug(LOG_LEVEL, MSG) printf("%s", MSG)
#define dbg_printAnswerTemplate(LOG_LEVEL, OUTPUT, TERM_VECTOR, NUMTERMS)
#define dbg_printTrieNode(LOG_LEVEL, OUTPUT, LEAF)

/* amiops.h */
#define conditional(Addr) (OUTSIDE(HBREG, Addr, B) || ((Addr) > (CELL *)B_FZ))
#define Sys_Trail_Unwind(TR0) trail_unwind(TR0)
#define pushtrail0 DO_TRAIL
#define bind_ref(A,D) Bind(A,D)

#endif /* TABLING_CALL_SUBSUMPTION */

#endif
