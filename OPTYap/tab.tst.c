
#include "Yap.h"
#include "Yatom.h"
#include "YapHeap.h"
#include "yapio.h"
#include "tab.tst.h"
#include "tab.xsb.h"
#include "tab.macros.h"

#define New_TSTN(TSTN,TrieType,NodeType,Symbol,Parent,Sibling)  \
  TSTN = new_tstn(CTXTc TrieType,NodeType,Symbol,Parent,Sibling)
  
#define TN_Init(TN,TrieType,NodeType,Symbol,Parent,Sibling) { \
  if(NodeType != TRIE_ROOT_NT)  { \
    TN_SetInstr(TN,Symbol); \
    TN_ResetInstrCPs(TN,Sibling); \
  } \
  else  \
    TN_Instr(TN) = trie_root; \
  TN_Status(TN) = VALID_NODE_STATUS;  \
  TN_TrieType(TN) = TrieType; \
  TN_NodeType(TN) = NodeType; \
  TN_Symbol(TN) = Symbol; \
  TN_Parent(TN) = (ans_node_ptr)Parent; \
  TN_Child(TN) = NULL;  \
  TN_Sibling(TN) = (ans_node_ptr)Sibling; \
  }

static TSTNptr new_tstn(CTXTdeclc int trie_t, int node_t, Cell symbol, TSTNptr parent,
    TSTNptr sibling) {
  void * tstn;
  
  ALLOC_TST_ANSWER_TRIE_NODE(tstn);
  TN_Init(((TSTNptr)tstn),trie_t,node_t,symbol,parent,sibling);
  TSTN_TimeStamp(((TSTNptr)tstn)) = TSTN_DEFAULT_TIMESTAMP;
  return (TSTNptr)tstn;        
}

/*
 * Adds a node containing 'symbol' below 'parent', which currentyle has
 * no children.
 */
inline static
TSTNptr tstnAddSymbol(CTXTdeclc TSTNptr parent, Cell symbol, int trieType) {
  TSTNptr newTSTN;
  
  New_TSTN(newTSTN, trieType, INTERIOR_NT, symbol, parent, NULL);
  TSTN_Child(parent) = (ans_node_ptr)newTSTN;
  return newTSTN;
}

TSTNptr tst_insert(CTXTdeclc TSTNptr tstRoot, TSTNptr lastMatch, Cell firstSymbol,
        xsbBool maintainTSI) {
  Cell symbol;
  int std_var_num,
      trieType;
      
  symbol = firstSymbol;
  std_var_num = Trail_NumBindings;
  trieType = TSTN_TrieType(tstRoot);
  
  /* Insert initial symbol
     --------------------- */
  if ( symbol == NO_INSERT_SYMBOL )
    ProcessNextSubtermFromTrieStacks(symbol, std_var_num);
  
  if(IsNULL(TSTN_Child(lastMatch)))
    lastMatch = tstnAddSymbol(CTXTc lastMatch, symbol, trieType);
  return NULL;
}