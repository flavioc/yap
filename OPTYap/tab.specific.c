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

//#define DEBUG_SPECIFIC2

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

#ifdef TIME_SUBSUMED_BENCHMARK
int total_exec_collect_subsumed = 0;
int total_sg_fr_collect_subsumed = 0;
int total_nodes_touched_collect_subsumed = 0;

static int
list_length(ALNptr list)
{
  int ret = 0;
  
  while (list) {
    ++ret;
    list = NodeList_next(list);
  }
  
  return ret;
}
#endif

static inline
xsbBool
Unify_with_Variable(Cell symbol, Cell subterm, BTNptr node) {
  int type = TrieSymbolType(symbol);

  switch(type) {
    case XSB_REF:
#ifdef SUBSUMPTION_XSB
    case XSB_REF1:
#endif
      if(PrologVar_IsMarked(subterm)) {
        if(IsNewTrieVar(symbol))
          return FALSE;
        
        int prolog_index = PrologVar_Index(subterm);
        int trie_index = DecodeTrieVar(BTN_Symbol(node));
        
        if(prolog_index != trie_index)
          return FALSE;
      } else {
        /* new call variable */
        int trie_index = DecodeTrieVar(BTN_Symbol(node));
        
        PrologVar_MarkIt(subterm, trie_index);
      }
      return TRUE;
    default:
      break;
  }

  if(PrologVar_IsMarked(subterm))
    return FALSE;

  switch(type) {
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

static inline ALNptr
create_subgoal_trie_path_as_list(BTNptr node)
{
	ALNptr path = NULL;
	int n = 0;

	while(!IsTrieRoot(node)) {
		ALN_InsertAnswer(path, node);
		node = BTN_Parent(node);
		++n;
	}

#ifdef DEBUG_SPECIFIC2
	printf("Path with %d nodes\n", n);
#endif

	return path;
}

static inline void
free_subgoal_trie_path_list(ALNptr list)
{
	while(list != NULL) {
		ALNptr next = NodeList_next(list);
		FREE_NODE_LIST(list);
		list = next;
	}
}

static int
is_subgoal_frame_active_generator(sg_fr_ptr sg_fr, sg_fr_ptr self)
{
	switch(SgFr_state(sg_fr)) {
		case changed:
		case evaluating:
		case suspended:
			if(sg_fr != self && SgFr_is_retroactive_producer(sg_fr)) {
        return TRUE;
			}
			break;
	}
  return FALSE;
}

ALNptr create_trie_path_lists(ALNptr leafs, sg_fr_ptr self)
{
  ALNptr ret = NULL;
  
  while (leafs) {
    ALNptr next = NodeList_next(leafs);
    BTNptr leaf = NodeList_node(leafs);
    sg_fr_ptr sg_fr = TrNode_sg_fr(leaf);
    
#ifdef TIME_SUBSUMED_BENCHMARK
    total_nodes_touched_collect_subsumed++;
#endif
    
    if(is_subgoal_frame_active_generator(sg_fr, self)) {
      ALN_InsertAnswer(ret, create_subgoal_trie_path_as_list(leaf));
    }
    
    leafs = next;
  }
  
  return ret;
}

ALNptr collect_specific_generator_goals2(tab_ent_ptr tab_ent, int arity, CELL* template,
		retroactive_fr_ptr retro_fr_ptr)
{
	if(arity < 1)
		return NULL;

  ALNptr all_leafs = (ALNptr)tab_ent->subgoal_list;

	if(all_leafs == NULL)
		return NULL;
	
	ALNptr all_subgoals = create_trie_path_lists(all_leafs, (sg_fr_ptr)retro_fr_ptr);

#define CURRENT_NODE ((sg_node_ptr)NodeList_node(trie_path))
#define NEXT_NODE_TERM() \
		previous_path = trie_path; \
		trie_path = NodeList_next(trie_path)
#define MATCH_FAILED() goto failed
#define TRY_MATCH_HERE(TermStack_PushOp) \
		if(symbol == BTN_Symbol(CURRENT_NODE)) { \
			TermStack_PushOp;	\
			NEXT_NODE_TERM(); \
		}	else							\
			MATCH_FAILED()

  Cell symbol, subterm;
	ALNptr returnList = NULL;

	while (all_subgoals != NULL) {
		ALNptr next = NodeList_next(all_subgoals);
		ALNptr list = (ALNptr)NodeList_node(all_subgoals);
		ALNptr trie_path = list;
		ALNptr previous_path = NULL;

		/* initialize state */
		TermStack_ResetTOS;
		TermStack_PushHighToLowVector(template, arity);
  
		trail_base = top_of_trail;
		symbol = 0;
		Save_and_Set_WAM_Registers;

		/* do matching */
		while (!TermStack_IsEmpty) {
#ifdef TIME_SUBSUMED_BENCHMARK
			total_nodes_touched_collect_subsumed++;
#endif
			TermStack_Pop(subterm);
			XSB_Deref(subterm);
			switch(cell_tag(subterm)) {
				case XSB_INT:
#ifdef SUBSUMPTION_XSB
				case XSB_FLOAT:
#endif
				case XSB_STRING:
					symbol = EncodeTrieConstant(subterm);
					TRY_MATCH_HERE(TermStack_NOOP);
					break;
				case XSB_STRUCT:
					symbol = EncodeTrieFunctor(subterm);
					TRY_MATCH_HERE(TermStack_PushFunctorArgs(subterm));
					break;
				case XSB_LIST:
					symbol = EncodeTrieList(subterm);
					TRY_MATCH_HERE(TermStack_PushListArgs(subterm));
					break;
				case XSB_REF:
#ifdef SUBSUMPTION_XSB
				case XSB_REF1:
#endif
					symbol = BTN_Symbol(CURRENT_NODE);
					if(Unify_with_Variable(symbol, subterm, CURRENT_NODE)) {
						NEXT_NODE_TERM();
					} else {
						MATCH_FAILED();
					}
					break;
			}
		}

		/* success */
		sg_fr_ptr sg_fr = TrNode_sg_fr((sg_node_ptr)NodeList_node(previous_path));
		if(is_subgoal_frame_active_generator(sg_fr, (sg_fr_ptr)retro_fr_ptr)) {
#ifdef DEBUG_SPECIFIC2
			printf("SGFR %p is producer\n", sg_fr);
      printSubgoalTriePath(stdout, sg_fr);
#endif
			ALN_InsertAnswer(returnList, sg_fr);
		}

failed:
		/* restore WAM state */
		Sys_Trail_Unwind(trail_base);
		Restore_WAM_Registers;

		free_subgoal_trie_path_list(list);
		FREE_NODE_LIST(all_subgoals);
		all_subgoals = next;
	}

	return returnList;
}


ALNptr collect_specific_generator_goals(tab_ent_ptr tab_ent, int arity, CELL* template,
		retroactive_fr_ptr retro_sg_fr)
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
    
#ifdef TIME_SUBSUMED_BENCHMARK
  total_nodes_touched_collect_subsumed++;
#endif

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
#ifdef TIME_SUBSUMED_BENCHMARK
    total_nodes_touched_collect_subsumed++;
#endif
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

ALNptr collect_specific_generator_subgoals(tab_ent_ptr tab_ent, int arity,
		CELL* template,
		retroactive_fr_ptr retro_sg_fr)
{
	ALNptr ret;

  start_time_subsumed();
#ifdef EFFICIENT_SUBSUMED_COLLECT
	ret = collect_specific_generator_goals(tab_ent, arity, template, retro_sg_fr);
#else
	ret = collect_specific_generator_goals2(tab_ent, arity, template, retro_sg_fr);
#endif
  end_time_subsumed();
#ifdef TIME_SUBSUMED_BENCHMARK
  ++total_exec_collect_subsumed;
  total_sg_fr_collect_subsumed += list_length(ret);
#endif
  
	return ret;
}

#endif /* TABLING_CALL_SUBSUMPTION */
