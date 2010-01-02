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
#define clref_val(REF)  RepAppl(REF)
#define clrefp_val(REF) RepPair(REF)
#define bld_free(addr) RESET_VARIABLE(addr) /*cell(addr) = (Cell)(addr) CPtr => XSB_FREE cell ??? */

#define IsNonNULL(ptr)   ( (ptr) != NULL )
#define CTXTdeclc
#define CTXTc
#define YES TRUE
#define NO  FALSE

#define XSB_STRING TAG_ATOM
#define XSB_INT TAG_INT
#define XSB_FLOAT TAG_FLOAT
#define XSB_STRUCT TAG_STRUCT
#define XSB_LIST TAG_LIST

#define EncodeTrieConstant(Cell_Const) ((Cell)Cell_Const)

#define XSB_Deref(X) Deref(X)

#define SubsumptiveTrieLookupError(MSG) \
  Yap_Error(FATAL_ERROR, TermNil, MSG);


/* subgoal frames */
#define BTN_Child(NODE)         TrNode_child(NODE)
#define BTN_Symbol(NODE)        TrNode_entry(NODE)
#define BTN_Sibling(NODE)       TrNode_next(NODE)

#define IsHashHeader(NODE)      IS_SUBGOAL_TRIE_HASH(NODE)
#define BTHT_BucketArray(HASH)  Hash_buckets(HASH)
#define BTHT_GetHashSeed(HASH)  Hash_seed(HASH)
#define TrieHash(SYMBOL, SEED)  HASH_ENTRY(SYMBOL, SEED)
#define TRIEVAR_BUCKET          0 /* ??? */

#define IsTrieVar(SYMBOL)       IsVarTerm(SYMBOL)
#define IsNewTrieVar(SYMBOL)    (!IsTableVarTerm(SYMBOL))
#define DecodeTrieVar(VAR)      VarIndexOfTerm(VAR)

#define StandardizeVariable(DerefedVar, Index)  \
    (*((CELL *)DerefedVar) = GLOBAL_table_var_enumerator(Index))
#define EncodeTrieVar(INDEX)      MakeTableVarTerm(INDEX)
#define EncodeTrieFunctor(TERM)   AbsAppl((Term *)FunctorOfTerm(TERM))

#endif /* TABLING_CALL_SUBSUMPTION */

#endif