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
#include "tab.utils.h"
#include "tab.tst.h"
#include "tab.retrv.h"
#include "tab.utils.h"
#include "tab.xsb_common.h"
#include "tab.specific.h"
#include "tab.collect.h"
#include "xsb.collect_utils.h"

#define GetGeneratorNode(Node) TrNode_num_gen((subg_node_ptr)(Node))
#define GetGeneratorIndex(Node) GNIN_num_gen((gen_index_ptr)TrNode_num_gen((subg_node_ptr)(Node)))
#define ValidGenerator(NumGenerator) ((NumGenerator) > 0)

#define NextIndexNode(Node) GNIN_next(TrNode_index_node((subg_node_ptr)(Node)))
#define NextValidIndexNode(Node) \
  (NextIndexNode(Node) ? (sg_node_ptr)GNIN_node(NextIndexNode(Node)) : NULL)

/* ------------------------------------------------------------------------- */

#define Chain_NextValidGenerator(Chain)               \
  while(IsNonNULL(Chain) && !ValidGenerator(GetGeneratorNode(Chain)))  \
    Chain = BTN_Sibling(Chain)
 
#define SetMatchChain(Symbol, SymChain) { \
 BTHTptr ht = (BTHTptr)SymChain;  \
 BTNptr *buckets = BTHT_BucketArray(ht);  \
  \
 SymChain = buckets[TrieHash(Symbol,BTHT_GetHashSeed(ht))];  \
}
 
#define SearchChain_ExactMatch(SearchChain,TrieEncodedSubterm,TermStack_PushOp) 		\
   while ( IsNonNULL(SearchChain) ) {					\
     if ( TrieEncodedSubterm == BTN_Symbol(SearchChain) ) { \
       if(ValidGenerator(GetGeneratorNode(SearchChain))) {  \
	       TermStackLog_PushFrame;					\
	       TermStack_PushOp;						\
	       Descend_Into_Node_and_Continue_Search;				\
       }                                  \
       else             \
        break;         \
     }									\
     SearchChain = BTN_Sibling(SearchChain);				\
   }
   

#define PrologVar_MarkIt(DerefedVar,Index)	\
   StandardizeVariable(DerefedVar,Index);	\
   pushtrail0(DerefedVar, 0)

#define PrologVar_IsMarked(pDerefedPrologVar)	\
   IsStandardizedVariable(pDerefedPrologVar)

#define PrologVar_Index(VarEnumAddr)  IndexOfStdVar(VarEnumAddr)

static inline
xsbBool
Unify_with_Variable(Cell symbol, Cell subterm, BTNptr node) {
  if(PrologVar_IsMarked(subterm) && TrieSymbolType(symbol) != XSB_REF) {
    return FALSE;
  }
  
  switch(TrieSymbolType(symbol)) {
    case XSB_INT:
#ifdef SUBSUMPTION_XSB
    case XSB_FLOAT:
#endif
    case XSB_STRING:
      Trie_bind_copy((CPtr)subterm, symbol);
      break;
    case XSB_STRUCT:
      Trie_bind_copy((CPtr)subterm, (Cell)hreg);
      CreateHeapFunctor(symbol);
      break;
    case XSB_LIST:
      Trie_bind_copy((CPtr)subterm, (Cell)hreg);
      CreateHeapList();
      break;
    case XSB_REF:
#ifdef SUBSUMPTION_XSB
    case XSB_REF1:
#endif
      if(PrologVar_IsMarked(subterm)) {
        if(IsNewTrieVar(symbol)) {
          return FALSE;
        }
        
        int prolog_index = PrologVar_Index(subterm);
        int trie_index = DecodeTrieVar(BTN_Symbol(node));
        
        if(prolog_index != trie_index) {
          return FALSE;
        }
      } else {
        /* new call variable */
        int trie_index = DecodeTrieVar(BTN_Symbol(node));
        
        PrologVar_MarkIt(subterm, trie_index);
      }
      break;
#ifdef SUBSUMPTION_YAP
    case TAG_LONG_INT:
      Trie_bind_copy((CPtr)subterm, (Cell)hreg);
      CreateHeapLongInt(TrNode_long_int((long_subg_node_ptr)node));
      break;
    case TAG_FLOAT:
      Trie_bind_copy((CPtr)subterm, (Cell)hreg);
      CreateHeapFloat(TrNode_float((float_subg_node_ptr)node));
      break;
#endif /* SUBSUMPTION_YAP */
    default:
      return FALSE;
  } /* END switch(symbol_tag) */
  return TRUE;
}

ALNptr collect_specific_generator_goals(tab_ent_ptr tab_ent, int arity, CELL* template)
{
  ALNptr returnList;
  Cell subterm;
  Cell symbol;
  BTNptr alt_chain;
  SearchMode mode;
  BTNptr parent_node = TabEnt_subgoal_trie(tab_ent);
  BTNptr cur_chain = BTN_Child(parent_node);
  
  if(arity < 1)
    return NULL;
  if(cur_chain == NULL)
    return NULL;
  if(TrNode_num_gen((subg_node_ptr)parent_node) == 0)
    return NULL;
  
  TermStackLog_ResetTOS;
  TermStack_ResetTOS;
  TermStack_PushHighToLowVector(template, arity);
  
  collectTop = collectBase;
  trail_base = top_of_trail;
  returnList = NULL;
  symbol = 0;
  mode = DESCEND_MODE;
  Save_and_Set_WAM_Registers;
  
While_TSnotEmpty:
  while (!TermStack_IsEmpty) {
    TermStack_Pop(subterm);
    XSB_Deref(subterm);
    switch(cell_tag(subterm)) {
      case XSB_INT:
#ifdef SUBSUMPTION_XSB
      case XSB_FLOAT:
#endif
      case XSB_STRING:
        symbol = EncodeTrieConstant(subterm);
        if ( IsHashHeader(cur_chain) ) {
	        SetMatchChain(symbol,cur_chain);
	        if ( IsNULL(cur_chain) )
	          backtrack;
        }
        SearchChain_ExactMatch(cur_chain,symbol,TermStack_NOOP);
        break;
      case XSB_STRUCT:
        symbol = EncodeTrieFunctor(subterm);
        if(IsHashHeader(cur_chain)) {
          SetMatchChain(symbol,cur_chain);
          if(IsNULL(cur_chain))
            backtrack;
        }
        SearchChain_ExactMatch(cur_chain,symbol,TermStack_PushFunctorArgs(subterm));
        break;
      case XSB_LIST:
        symbol = EncodeTrieList(subterm);
        if(IsHashHeader(cur_chain)) {
          SetMatchChain(symbol,cur_chain);
          if(IsNULL(cur_chain))
            backtrack;
        }
        SearchChain_ExactMatch(cur_chain,symbol,TermStack_PushListArgs(subterm));
        break;
      case XSB_REF:
#ifdef SUBSUMPTION_XSB
      case XSB_REF1:
#endif
        if(IsHashedNode(cur_chain))
          alt_chain = NextValidIndexNode(cur_chain);
        else if(IsHashHeader(cur_chain)) {
          gen_index_ptr index = Hash_index_head((subg_hash_ptr)cur_chain);
          
          if(IsNULL(index))
            backtrack; /* no generators on this hash table */
          
          cur_chain = (sg_node_ptr)GNIN_node(index);
          alt_chain = NextValidIndexNode(cur_chain);
        } else {
          /* simple linked list */
          Chain_NextValidGenerator(cur_chain);
          if(IsNULL(cur_chain))
            backtrack;
          
          alt_chain = BTN_Sibling(cur_chain);
          Chain_NextValidGenerator(alt_chain);
        }
        
        symbol = BTN_Symbol(cur_chain);
        TrieSymbol_Deref(symbol);
        CPStack_PushFrame(alt_chain);
        TermStackLog_PushFrame;
        if(!Unify_with_Variable(symbol, subterm, cur_chain))
          backtrack;
        Descend_Into_Node_and_Continue_Search;
        break;
#ifdef SUBSUMPTION_YAP
      case TAG_LONG_INT: {
        Int li = LongIntOfTerm(subterm);
        if(IsHashHeader(cur_chain)) {
          SetMatchChain((Term)li,cur_chain);
          if(IsNULL(cur_chain))
            backtrack;
        }
        while(IsNonNULL(cur_chain)) {
          if(TrNode_is_long(cur_chain) && TrNode_long_int((long_subg_node_ptr)cur_chain) == li) {
            if(ValidGenerator(GetGeneratorNode(cur_chain))) {
              TermStackLog_PushFrame;
              Descend_Into_Node_and_Continue_Search;
            } else
              break; /* no generators */
          }
          cur_chain = BTN_Sibling(cur_chain);
        }
        break;
      }
      case TAG_FLOAT: {
        Float flt = FloatOfTerm(subterm);
        if(IsHashHeader(cur_chain)) {
          SetMatchChain((Term)flt, cur_chain);
          if(IsNULL(cur_chain))
            backtrack;
        }
        while(IsNonNULL(cur_chain)) {
          if(TrNode_is_float(cur_chain) && TrNode_float((float_subg_node_ptr)cur_chain) == flt) {
            if(ValidGenerator(GetGeneratorNode(cur_chain))) {
              TermStackLog_PushFrame;
              Descend_Into_Node_and_Continue_Search;
            } else
              break; /* no generators */
          }
          cur_chain = BTN_Sibling(cur_chain);
        }
        break;
      }
#endif /* SUBSUMPTION_YAP */
      default:
        fprintf(stderr, "subterm: unknown (%ld),  symbol: ? (%ld)\n",
  	      (long int)cell_tag(subterm), (long int)TrieSymbolType(symbol));
        Collection_Error("Trie symbol with bogus tag!", RequiresCleanup);
        break;
    }
    
    if ( CPStack_IsEmpty ) {
      goto end_collect;
    }
    Collect_Backtrack;
  }
  
  /* new subgoal */
  
  ALN_InsertAnswer(returnList, TrNode_sg_fr(parent_node));
#if 0
#ifdef FDEBUG
  printf("Found subgoal ");
  printSubgoalTriePath(stdout, parent_node, tab_ent);
#endif
#endif

  if ( CPStack_IsEmpty ) {
    goto end_collect;
  }
  Collect_Backtrack;
  goto While_TSnotEmpty;
  
end_collect:
  Sys_Trail_Unwind(trail_base);
  Restore_WAM_Registers;
  return returnList;
}

#endif /* TABLING_CALL_SUBSUMPTION */
