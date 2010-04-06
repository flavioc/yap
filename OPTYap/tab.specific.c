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

#define SetMatchAndUnifyChains(Symbol,SymChain,VarChain) {	\
								\
   BTHTptr ht = (BTHTptr)SymChain;				\
   BTNptr *buckets = BTHT_BucketArray(ht);			\
								\
   SymChain = buckets[TrieHash(Symbol,BTHT_GetHashSeed(ht))];	\
   VarChain = buckets[TRIEVAR_BUCKET];				\
 }
 
#define SearchChain_ExactMatch(SearchChain,TrieEncodedSubterm,ContChain,TermStack_PushOp) 		\
   while ( IsNonNULL(SearchChain) ) {					\
     if ( TrieEncodedSubterm == BTN_Symbol(SearchChain) ) { \
       if(ValidGenerator(GetGeneratorNode(SearchChain))) {  \
         Chain_NextValidGenerator(ContChain); \
	       CPStack_PushFrame(ContChain);					\
	       TermStackLog_PushFrame;					\
	       TermStack_PushOp;						\
	       Descend_Into_Node_and_Continue_Search;				\
       }                                  \
       else             \
        break;         \
     }									\
     SearchChain = BTN_Sibling(SearchChain);				\
   }
   
#define SearchChain_UnifyWithConstant(Chain,Subterm) {	\
   Chain_NextValidGenerator(Chain);                     \
   while ( IsNonNULL(Chain) ) {						\
     alt_chain = BTN_Sibling(Chain);					\
     Chain_NextValidGenerator(alt_chain);     \
     symbol = BTN_Symbol(Chain);					\
     TrieSymbol_Deref(symbol);						\
     if ( isref(symbol) ) {						\
       /*								\
	      *  Either an unbound TrieVar or some unbound prolog var.	\
	      */								\
       CPStack_PushFrame(alt_chain);					\
       Bind_and_Conditionally_Trail((CPtr)symbol, Subterm);		\
       TermStackLog_PushFrame;						\
       Descend_Into_Node_and_Continue_Search;				\
     }									\
     else if (symbol == Subterm) {					\
       CPStack_PushFrame(alt_chain);					\
       TermStackLog_PushFrame;						\
       Descend_Into_Node_and_Continue_Search;				\
     }									\
     Chain = alt_chain;							\
   }									\
 }
 
#define SearchChain_UnifyWithFunctor(Chain,Subterm) { \
  Cell sym_tag;  \
  Chain_NextValidGenerator(Chain);  \
  while(IsNonNULL(Chain)) {         \
    alt_chain = BTN_Sibling(Chain); \
    Chain_NextValidGenerator(alt_chain);  \
    symbol = BTN_Symbol(Chain);           \
    sym_tag = TrieSymbolType(symbol);   \
    TrieSymbol_Deref(symbol);           \
    if(isref(symbol)) { \
      CPStack_PushFrame(alt_chain); \
      Bind_and_Conditionally_Trail((CPtr)symbol, Subterm);  \
      TermStackLog_PushFrame; \
      Descend_Into_Node_and_Continue_Search;  \
    } else if(IsTrieFunctor(symbol)) {  \
      if(sym_tag == XSB_STRUCT) { \
        if(get_str_psc(Subterm) == DecodeTrieFunctor(symbol)) { \
          CPStack_PushFrame(alt_chain); \
          TermStackLog_PushFrame; \
          TermStack_PushFunctorArgs(Subterm); \
          Descend_Into_Node_and_Continue_Search;  \
        } \
      } else {  \
        if(unify(Subterm, symbol)) {  \
          CPStack_PushFrame(alt_chain); \
          TermStackLog_PushFrame; \
          Descend_Into_Node_and_Continue_Search;  \
        } \
      } \
    } \
    Chain = alt_chain;  \
  } \
}

#define SearchChain_UnifyWithList(Chain,Subterm) {  \
  Cell sym_tag; \
  Chain_NextValidGenerator(Chain);  \
  while(IsNonNULL(Chain)) {         \
    alt_chain = BTN_Sibling(Chain); \
    Chain_NextValidGenerator(alt_chain);  \
    symbol = BTN_Symbol(Chain);           \
    sym_tag = TrieSymbolType(symbol);     \
    TrieSymbol_Deref(symbol);             \
    if(isref(symbol)) {                   \
      CPStack_PushFrame(alt_chain);       \
      Bind_and_Conditionally_Trail((CPtr)symbol,Subterm); \
      TermStackLog_PushFrame;       \
      Descend_Into_Node_and_Continue_Search;  \
    } else if(IsTrieList(symbol)) {           \
      if(sym_tag == XSB_LIST) {               \
        CPStack_PushFrame(alt_chain);         \
        TermStackLog_PushFrame;               \
        TermStack_PushListArgs(Subterm);      \
        Descend_Into_Node_and_Continue_Search;  \
      } else {                                  \
        if(unify(Subterm, symbol)) {            \
          CPStack_PushFrame(alt_chain);         \
          TermStackLog_PushFrame;               \
          Descend_Into_Node_and_Continue_Search;  \
        } \
      } \
    } \
    Chain = alt_chain;  \
  } \
}

#define SearchChain_UnifyWithLong(Chain,Subterm) {  \
  Int li = LongIntOfTerm(Subterm);                  \
  Chain_NextValidGenerator(Chain);                  \
  while(IsNonNULL(Chain)) {                         \
    alt_chain = BTN_Sibling(Chain);                 \
    Chain_NextValidGenerator(alt_chain);            \
    symbol = BTN_Symbol(Chain);                     \
    TrieSymbol_Deref(symbol);                       \
    if(isref(symbol)) {                             \
      CPStack_PushFrame(alt_chain);                 \
      Bind_and_Conditionally_Trail((CPtr)symbol, Subterm);  \
      TermStackLog_PushFrame;                       \
      Descend_Into_Node_and_Continue_Search;        \
    }                                               \
    else {                                          \
      int go = FALSE;                               \
      if(TrNode_is_long(Chain))                     \
        go = TrNode_long_int((long_subg_node_ptr)Chain) == li;  \
      else {                                        \
        if(TrieSymbolType(symbol) == TAG_LONG_INT)  \
          go = LongIntOfTerm(symbol) == li;         \
      }                                             \
      if(go) {                                      \
        CPStack_PushFrame(alt_chain);               \
        TermStackLog_PushFrame;                     \
        Descend_Into_Node_and_Continue_Search;      \
      }                                             \
    }                                               \
    Chain = alt_chain;                              \
  }                                                 \
}

#define SearchChain_UnifyWithFloat(Chain,Subterm) { \
  Float flt = FloatOfTerm(Subterm);                 \
  Chain_NextValidGenerator(Chain);                  \
  while(IsNonNULL(Chain)) {                         \
    alt_chain = BTN_Sibling(Chain);                 \
    Chain_NextValidGenerator(alt_chain);            \
    symbol = BTN_Symbol(Chain);                     \
    TrieSymbol_Deref(symbol);                       \
    if(isref(symbol)) {                             \
      CPStack_PushFrame(alt_chain);                 \
      Bind_and_Conditionally_Trail((CPtr)symbol, Subterm);  \
      TermStackLog_PushFrame;                       \
      Descend_Into_Node_and_Continue_Search;        \
    }                                               \
    else {                                          \
      int go = FALSE;                               \
      if(TrNode_is_float(Chain))                    \
        go = TrNode_float((float_subg_node_ptr)Chain) == flt; \
      else {                                        \
        if(TrieSymbolType(symbol) == TAG_FLOAT)     \
          go = FloatOfTerm(symbol) == flt;          \
      }                                             \
      if(go) {                                      \
        CPStack_PushFrame(alt_chain);               \
        TermStackLog_PushFrame;                     \
        Descend_Into_Node_and_Continue_Search;      \
      }                                             \
    }                                               \
    Chain = alt_chain;                              \
  }                                                 \
}
 
static inline
xsbBool
Unify_with_Variable(Cell symbol, Cell subterm, BTNptr node) {
  switch(TrieSymbolType(symbol)) {
    case XSB_INT:
#ifdef SUBSUMPTION_XSB
    case XSB_FLOAT:
#endif
    case XSB_STRING:
      Trie_bind_copy((CPtr)subterm, symbol);
      break;
    case XSB_STRUCT:
      if(IsTrieFunctor(BTN_Symbol(node))) {
        Trie_bind_copy((CPtr)subterm, (Cell)hreg);
        CreateHeapFunctor(symbol);
      }
      else {
        Trie_bind_copy((CPtr)subterm, symbol);
      }
      break;
    case XSB_LIST:
      if(IsTrieList(BTN_Symbol(node))) {
        Trie_bind_copy((CPtr)subterm, (Cell)hreg);
        CreateHeapList();
      } else {
        Trie_bind_copy((CPtr)subterm, symbol);
      }
      break;
    case XSB_REF:
#ifdef SUBSUMPTION_XSB
    case XSB_REF1:
#endif
      if(IsUnboundTrieVar(symbol)) {
        Bind_and_Trail((CPtr)symbol,subterm);
      } else
        unify(subterm, symbol);
      break;
#ifdef SUBSUMPTION_YAP
    case TAG_LONG_INT:
      if(TrNode_is_long(node)) {
        Trie_bind_copy((CPtr)subterm, (Cell)hreg);
        CreateHeapLongInt(TrNode_long_int((long_subg_node_ptr)node));
      } else {
        Trie_bind_copy((CPtr)subterm, symbol);
      }
      break;
    case TAG_FLOAT:
      if(TrNode_is_float(node)) {
        Trie_bind_copy((CPtr)subterm, (Cell)hreg);
        CreateHeapFloat(TrNode_float((float_subg_node_ptr)node));
      } else {
        Trie_bind_copy((CPtr)subterm, symbol);
      }
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
        if ( IsHashHeader(cur_chain) ) {
	        symbol = EncodeTrieConstant(subterm);
	        SetMatchAndUnifyChains(symbol,cur_chain,alt_chain);
	        if ( cur_chain != alt_chain ) {
	          SearchChain_ExactMatch(cur_chain,symbol,alt_chain,TermStack_NOOP);
	          cur_chain = alt_chain;
	        }
	        if ( IsNULL(cur_chain) )
	          backtrack;
        }
	      SearchChain_UnifyWithConstant(cur_chain,subterm)
        break;
      case XSB_STRUCT:
        if(IsHashHeader(cur_chain)) {
          symbol = EncodeTrieFunctor(subterm);
          SetMatchAndUnifyChains(symbol,cur_chain,alt_chain);
          if(cur_chain != alt_chain) {
            SearchChain_ExactMatch(cur_chain,symbol,alt_chain,
              TermStack_PushFunctorArgs(subterm));
            cur_chain = alt_chain;
          }
          if(IsNULL(cur_chain))
            backtrack;
        }
        SearchChain_UnifyWithFunctor(cur_chain,subterm);
        break;
      case XSB_LIST:
        if(IsHashHeader(cur_chain)) {
          symbol = EncodeTrieList(subterm);
          SetMatchAndUnifyChains(symbol,cur_chain,alt_chain);
          if(cur_chain != alt_chain) {
            SearchChain_ExactMatch(cur_chain,symbol,alt_chain,TermStack_PushListArgs(subterm));
            cur_chain = alt_chain;
          }
          if(IsNULL(cur_chain))
            backtrack;
        }
        SearchChain_UnifyWithList(cur_chain,subterm);
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
        CPStack_PushFrame(alt_chain);
        TermStackLog_PushFrame;
        symbol = BTN_Symbol(cur_chain);
        TrieSymbol_Deref(symbol);
        if(!Unify_with_Variable(symbol, subterm, cur_chain)) {
          fprintf(stderr, "subterm: unbound var (%ld), symbol: unknown "
            "(%ld)\n", (long int)cell_tag(subterm), (long int)TrieSymbolType(symbol));
          Collection_Error("Trie symbol with bogus tag!", RequiresCleanup);
        }
        Descend_Into_Node_and_Continue_Search;
        break;
#ifdef SUBSUMPTION_YAP
      case TAG_LONG_INT:
        if(IsHashHeader(cur_chain)) {
          Int li = LongIntOfTerm(subterm);
          
          SetMatchAndUnifyChains((Term)li,cur_chain,alt_chain);
          
          if(cur_chain != alt_chain) {
            while(IsNonNULL(cur_chain)) {
              if(TrNode_is_long(cur_chain) && TrNode_long_int((long_subg_node_ptr)cur_chain) == li) {
                if(ValidGenerator(GetGeneratorNode(cur_chain))) {
                  Chain_NextValidGenerator(alt_chain);
                  CPStack_PushFrame(alt_chain);
                  TermStackLog_PushFrame;
                  Descend_Into_Node_and_Continue_Search;
                } else
                  break; /* no generators */
              }
              cur_chain = BTN_Sibling(cur_chain);
            }
            cur_chain = alt_chain;
          }
          
          if(IsNULL(cur_chain))
            backtrack;
        }
        SearchChain_UnifyWithLong(cur_chain,subterm);
        break;
      case TAG_FLOAT:
        if(IsHashHeader(cur_chain)) {
          Float flt = FloatOfTerm(subterm);
          
          SetMatchAndUnifyChains((Term)flt, cur_chain, alt_chain);
          
          if(cur_chain != alt_chain) {
            while(IsNonNULL(cur_chain)) {
              if(TrNode_is_float(cur_chain) && TrNode_float((float_subg_node_ptr)cur_chain) == flt) {
                if(ValidGenerator(GetGeneratorNode(cur_chain))) {
                  Chain_NextValidGenerator(alt_chain);
                  CPStack_PushFrame(alt_chain);
                  TermStackLog_PushFrame;
                  Descend_Into_Node_and_Continue_Search;
                } else
                  break; /* no generators */
              }
              cur_chain = BTN_Sibling(cur_chain);
            }
            cur_chain = alt_chain;
          }
          if(IsNULL(cur_chain))
            backtrack;
        }
        SearchChain_UnifyWithFloat(cur_chain,subterm);
        break;
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
#ifdef FDEBUG
  printf("Found subgoal ");
  printSubgoalTriePath(stdout, parent_node, tab_ent);
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