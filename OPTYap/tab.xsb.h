/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   R. Rocha and NCC - University of Porto, Portugal
  File:        tab.xsb.h
                                                                     
**********************************************************************/

#ifndef TAB_XSB_H
#define TAB_XSB_H

#include "opt.config.h"

#ifdef TABLING_CALL_SUBSUMPTION

typedef CELL Cell;
typedef Cell *CPtr;
typedef unsigned long int counter;
typedef sg_node_ptr BTNptr;
typedef int xsbBool;
typedef sg_hash_ptr BTHTptr;
typedef Functor Psc;

#define get_str_psc(FUNCTOR) FunctorOfTerm(FUNCTOR)
#define get_arity(FUNCTOR)  ArityOfFunctor(FUNCTOR)
#define get_name(FUNCTOR)   AtomName(NameOfFunctor(FUNCTOR))
/* FunctorOfTerm(t) === *RepAppl (t) */
#define clref_val(REF)      RepAppl(REF)
#define clrefp_val(REF)     RepPair(REF)
#define cs_val(REF)	RepAppl(REF)
#define bld_free(ADDR)      *((CELL *)ADDR) = (CELL)(ADDR)
#define bld_ref(ADDR, VAL)  *((CELL *)ADDR) = (CELL)(VAL)
#define cell(REF) *(REF)

#define IsNonNULL(ptr)   ( (ptr) != NULL )
#define IsNULL(ptr) ((ptr) == NULL)
#define CTXTdeclc
#define CTXTdecl void
#define CTXTc
#define CTXT
#define YES TRUE
#define NO  FALSE

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

#define XSB_Deref(X) ((X) = Deref(X))

#define SubsumptiveTrieLookupError(MSG) \
  Yap_Error(FATAL_ERROR, TermNil, MSG);


/* subgoal frames */
#define BTN_Child(NODE)         TrNode_child(NODE)
#define BTN_Symbol(NODE)        TrNode_entry(NODE)
#define BTN_Sibling(NODE)       TrNode_next(NODE)
#define BTN_Parent(NODE)        TrNode_parent(NODE)
#define BTN_NodeType(NODE)      TrNode_node_type(NODE)

#define IsHashHeader(NODE)      (TrNode_node_type(NODE) & HASH_HEADER_NT)
#define IsHashedNode(NODE)		(TrNode_node_type(NODE) & HASHED_INTERIOR_NT)
#define BTHT_BucketArray(HASH)  Hash_buckets(HASH)
#define BTHT_GetHashSeed(HASH)  Hash_seed(HASH)
#define TrieHash(SYMBOL, SEED)  HASH_ENTRY(SYMBOL, SEED)
#define TRIEVAR_BUCKET          0 /* ??? */

#define IsTrieVar(SYMBOL)       (IsVarTerm(SYMBOL))
#define IsNewTrieVar(SYMBOL)    (IsNewTableVarTerm(SYMBOL))
#define DecodeTrieVar(VAR)      VarIndexOfTableTerm(VAR)

#define StandardizeVariable(DerefedVar, Index)  \
    (*((CELL *)DerefedVar) = GLOBAL_table_var_enumerator(Index))
#define IsStandardizedVariable(DerefVar)  (IsTableVarTerm(DerefVar))

#define IndexOfStdVar(VAR_ENUM_ADDR)  VarIndexOfTerm(VAR_ENUM_ADDR)
#define IsTrieRoot(NODE)   (TrNode_parent(NODE) == NULL || TrNode_node_type(NODE) == TRIE_ROOT_NT)
#define IsLeafNode(NODE)   (TrNode_child(NODE) == NULL) /// XXX
#define IsEscapeNode(NODE)  (FALSE) // XXX
#define ESCAPE_NODE_SYMBOL    (long)0xFFFFFFF // XXX
#define IsTrieFunctor(SYMBOL) (cell_tag(SYMBOL) == TAG_STRUCT)
#define IsTrieList(SYMBOL)  IsPairTerm(SYMBOL)

#define xsb_abort(MSG, ...) Yap_Error(PURE_ABORT, TermNil, MSG, ##__VA_ARGS__)

#ifdef BITS64
#define IntegerFormatString	"%ld"
#else
#define IntegerFormatString	"%d"
#endif

#if SHORT_INTS
#define LongIntFormatString "%ld"
#else
#define LongIntFormatString "%d"
#endif

#if SIZEOF_DOUBLE == 2 * SIZEOF_INT_P
#define FloatFormatString "%lf"
#else
#define FloatFormatString "%f"
#endif

#define int_val(SYMBOL) IntOfTerm(SYMBOL)
#define string_val(SYMBOL)  AtomName(AtomOfTerm(SYMBOL))
#define DecodeTrieFunctor(SYMBOL) ((Functor) RepAppl(SYMBOL))

#define SUBSUMPTION_YAP 1
/* #define SUBSUMPTION_XSB 1 */

#define makeint(I)  (I)
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
#define TN_Child(X) TSTN_child(X)
#define TN_Sibling(X) TSTN_next(X)
#define TN_Parent(X) TSTN_parent(X)
#define TN_Symbol(X)  TSTN_entry(X)
#define TN_Instr(X)   TSTN_instr(X)
#define TN_TrieType(X)  TSTN_trie_type(X)
#define TN_NodeType(X)  TSTN_node_type(X)
#define TSTN_GetHashHdr(X)  (TSTHTptr)TN_GetHashHdr(X)
#define TrieHT_BucketArray(X) TSTHT_buckets(X)
#define TrieHT_GetHashSeed(X) TSTHT_seed(X)
#define TSC_NodeType(X) TSTN_node_type(X)
#define TSTHT_NumContents(X) TSTHT_num_nodes(X)
#define TSTHT_BucketArray(X) TSTHT_buckets(X)
#define TSIN_TSTNode(X) TSIN_node(X)
#define TSIN_TimeStamp(X) TSIN_time_stamp(X)
#define TSTHT_IndexHead(X)  TSTHT_index_head(X)
#define TSTHT_IndexTail(X)  TSTHT_index_tail(X)
#define TrieHT_NumContents(X) TSTHT_num_nodes(X)
#define TrieHT_NumBuckets(X) TSTHT_num_buckets(X)
#define TSIN_Prev(X)  TSIN_prev(X)
#define TSIN_Next(X)  TSIN_next(X)
#define TrieHT_NewSize(X)  TSTHT_new_size(X)
#define BTHT_NumBuckets(X)  TSTHT_num_buckets(X)
#define BTHT_Instr(X) TSTHT_instr(X)
#define BTHT_TrieType(X)  TSTHT_trie_type(X)
#define BTHT_NodeType(X)  TSTHT_node_type(X)
#define BTHT_NumContents(X) TSTHT_num_nodes(X)
#define TSTHT_Instr(X) TSTHT_instr(X)
#define TSTHT_TrieType(X) TSTHT_trie_type(X)
#define TSTHT_NodeType(X) TSTHT_node_type(X)
#define TSTHT_NumBuckets(X) TSTHT_num_buckets(X)
#define TSTN_Parent(X) TSTN_parent(X)
#define TSTHT_InternalLink(X) TSTHT_next(X)
#define TSTHT_GetHashSeed(X) TSTHT_seed(X)
#define TSTN_Symbol(X)	TSTN_entry(X)

#define IsEmptyTrie(Root)      IsNULL(TN_Child(Root))

#define TimeStamp time_stamp

#define trie_root 0

#define BUCKET_CONTENT_THRESHOLD MAX_NODES_PER_BUCKET
#define MAX_SIBLING_LEN MAX_NODES_PER_TRIE_LEVEL
#define TrieHT_INIT_SIZE BASE_HASH_BUCKETS
#define hash_opcode _trie_do_hash

#define ALNptr ans_list_ptr

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
