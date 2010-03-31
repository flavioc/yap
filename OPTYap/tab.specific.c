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

/* ------------------------------------------------------------------------- */

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
	     CPStack_PushFrame(ContChain);					\
	     TermStackLog_PushFrame;					\
	     TermStack_PushOp;						\
	     Descend_Into_Node_and_Continue_Search;				\
     }									\
     SearchChain = BTN_Sibling(SearchChain);				\
   }
   
#define SearchChain_UnifyWithConstant(Chain,Subterm) {	\
   while ( IsNonNULL(Chain) ) {						\
     alt_chain = BTN_Sibling(Chain);					\
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

ALNptr collect_specific_generator_goals(yamop *code)
{
  ALNptr returnList = NULL;
  Cell subterm;
  Cell symbol;
  BTNptr alt_chain;
  SearchMode mode;
  tab_ent_ptr tab_ent = CODE_TABLE_ENTRY(code);
  BTNptr parent_node = TabEnt_subgoal_trie(tab_ent);
  BTNptr cur_chain = BTN_Child(parent_node);
  int arity = CODE_ARITY(code);
  
  if(arity < 1)
    return NULL;
  
  TermStack_ResetTOS;
  /* push arguments */
  TermStack_PushLowToHighVector(CALL_ARGUMENTS(),arity);
  TermStackLog_ResetTOS;
  collectTop = collectBase;
  trail_base = top_of_trail;
  Save_and_Set_WAM_Registers;
  mode = DESCEND_MODE;
  
  cur_chain = BTN_Child(parent_node);
  symbol = 0;   /* suppress compiler warning */
  
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
      case XSB_REF:
#ifdef SUBSUMPTION_XSB
      case XSB_REF1:
#endif
        
        break;
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
  sg_fr_ptr sg_fr = (sg_fr_ptr)TrNode_sg_fr(parent_node);
  printf("Found subgoal ");
  printSubgoalTriePath(stdout, parent_node, tab_ent);
  
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