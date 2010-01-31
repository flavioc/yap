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

#define get_arity(FUNCTOR)  ArityOfFunctor(FUNCTOR)
#define get_name(FUNCTOR)   AtomName(NameOfFunctor(FUNCTOR))
/* FunctorOfTerm(t) === *RepAppl (t) */
#define clref_val(REF)      RepAppl(REF)
#define clrefp_val(REF)     RepPair(REF)
#define bld_free(ADDR)      *((CELL *)ADDR) = (CELL)(ADDR)
#define bld_ref(ADDR, VAL)  *((CELL *)ADDR) = (CELL)(VAL)

#define IsNonNULL(ptr)   ( (ptr) != NULL )
#define IsNULL(ptr) ((ptr) == NULL)
#define CTXTdeclc
#define CTXTc
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

#define IsHashHeader(NODE)      IS_SUBGOAL_TRIE_HASH(NODE)
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
#define IsTrieRoot(NODE)   (TrNode_parent(NODE) == NULL)
#define IsLeafNode(NODE)   (TrNode_child(NODE) == NULL) /// XXX
#define IsEscapeNode(NODE)  (FALSE) // XXX
#define ESCAPE_NODE_SYMBOL    (long)0xFFFFFFF // XXX
#define IsTrieFunctor(SYMBOL) (cell_tag(SYMBOL) == TAG_STRUCT)

#define xsb_abort(MSG, ...) Yap_Error(PURE_ABORT, TermNil, MSG, __VA_ARGS__)

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

#endif /* TABLING_CALL_SUBSUMPTION */

#endif