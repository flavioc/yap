
#include "Yap.h"
#include "Yatom.h"
#include "YapHeap.h"
#include "yapio.h"
#include "tab.tst.h"
#include "tab.xsb.h"
#include "tab.macros.h"

/* prototypes */
STD_PROTO(static inline void expand_trie_ht, (CTXTdeclc BTHTptr));
STD_PROTO(static inline BTHTptr New_BTHT, (int));
STD_PROTO(void hashify_children, (CTXTdeclc BTNptr, int));

#define New_TSIN(TSIN, TSTN) {  \
  void *t;  \
  ALLOC_TST_INDEX_NODE(t);  \
  TSIN = (TSINptr)t;  \
  TSIN_TSTNode(TSIN) = TSTN;  \
  TSIN_TimeStamp(TSIN) = TSTN_TimeStamp(TSTN);  \
}

#define IsLongSiblingChain(ChainLength) (ChainLength > MAX_SIBLING_LEN)

#define New_TSTN(TSTN,TrieType,NodeType,Symbol,Parent,Sibling)  \
  TSTN = new_tstn(CTXTc TrieType,NodeType,Symbol,Parent,Sibling)
  
#define New_BTN(BTN,TrieType,NodeType,Symbol,Parent,Sibling)  \
    BTN = new_btn(CTXTc TrieType,NodeType,Symbol,(BTNptr)Parent, (BTNptr)Sibling);
  
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
  
#define CalculateBucketForSymbol(pHT,Symbol)  \
  (TrieHT_BucketArray(pHT) + TrieHash(Symbol, TrieHT_GetHashSeed(pHT)))

#undef IsHashHeader /* remove previous definition for yap hash tables */
#define IsHashedNode(pTSC)  (TSC_NodeType(pTSC) & HASHED_NODE_MASK)
#define IsHashHeader(pTSC)  (TSC_NodeType(pTSC) & HASH_HEADER_NT)

#define TSTN_DEFAULT_TIMESTAMP 1
#define VALID_NODE_STATUS 0
#define HASHED_NODE_MASK 0x01
#define LEAF_NODE_MASK 0x02

enum Types_of_Trie_Nodes {
  TRIE_ROOT_NT = 0x08,
  HASH_HEADER_NT = 0x04,
  LEAF_NT = 0x02,
  HASHED_LEAF_NT = 0x03,
  INTERIOR_NT = 0x00,
  HASHED_INTERIOR_NT = 0x01
};

#define TN_SetInstr(pTN,Symbol) \
  switch(TrieSymbolType(Symbol))  { \
    case XSB_STRUCT:  \
      TN_Instr(pTN) = _trie_try_struct; \
      break;  \
    case XSB_INT: \
    case XSB_STRING:  \
      TN_Instr(pTN) = _trie_try_atom; \
      break;  \
    case XSB_TrieVar: \
      if(IsNewTrieVar(Symbol))  \
        TN_Instr(pTN) = _trie_try_var;  \
      else  \
        TN_Instr(pTN) = _trie_try_val;  \
      break;  \
    case XSB_LIST:  \
      TN_Instr(pTN) = _trie_try_pair; \
      break;  \
    default:  \
      xsb_abort("Trie Node creation: Bad tag in symbol %lx", \
                  Symbol);  \
  }
  /// XXX: float e long int
  
#define TN_ResetInstrCPs(pHead, pSibling) { \
  if(IsNonNULL(pSibling)) \
    TN_RotateInstrCPtoRETRYorTRUST(pSibling); \
  else  \
    TN_Instr(pHead) -= 2;  /* try -> do */\
  }
  
#define TN_ForceInstrCPtoNOCP(pTN)  // XXX
#define TN_ForceInstrCPtoTRY(pTN) // XXX
#define TN_UpgradeInstrTypeToSUCCESS(pTN,SymbolTag) /// XXX

/* XXX : not sure about instrs */
/* try -> retry : do -> trust */
#define TN_RotateInstrCPtoRETRYorTRUST(pTN) TN_Instr(pTN) += 1

#define TrieHT_InsertNode(pBucketArray,HashSeed,pTN) {  \
  void **pBucket; \
    \
  pBucket = (void**)(pBucketArray + TrieHash(TN_Symbol(pTN),HashSeed)); \
  if(IsNonNULL(*pBucket)) { \
    TN_ForceInstrCPtoTRY(pTN); \
    TN_RotateInstrCPtoRETRYorTRUST((BTNptr)*pBucket); \
  } \
  else  \
    TN_ForceInstrCPtoNOCP(pTN);  \
  TN_Sibling(pTN) = *pBucket; \
  *pBucket = pTN; \
}

/* For Hashed TSTNs
   ---------------- */
#define TSTN_SetTSIN(pTSTN,TSIN) TSTN_TimeStamp(pTSTN) = (TimeStamp)(TSIN)
#define TSTN_GetTSIN(pTSTN) ((TSINptr)TSTN_TimeStamp(pTSTN))
#define TSTN_GetTSfromTSIN(pTSIN) TSIN_TimeStamp(TSTN_GetTSIN(pTSTN))

/* for indices */
#define IsTSindexHead(TSIN) IsNULL(TSIN_Prev(TSIN))
#define IsTSindexTail(TSIN) IsNULL(TSIN_Next(TSIN))

#define TrieHT_ExpansionCheck(pHT,NumBucketContents) {  \
  if((NumBucketContents > BUCKET_CONTENT_THRESHOLD) && \
      (TrieHT_NumContents(pHT) > TrieHT_NumBuckets(pHT))) \
      expand_trie_ht(CTXTc (BTHTptr)pHT); \
}

/*
 *  From an unHASHED-typed node, create a HASHED-typed node, keeping the
 *  LEAF/INTERIOR status in-tact.  Used when converting from a sibling
 *  chain to a hash structure.
 */
#define MakeHashedNode(pTN) \
 TN_NodeType(pTN) = TN_NodeType(pTN) | HASHED_NODE_MASK
 
/*
 *  From an INTERIOR-typed node, create a LEAF-typed node, keeping
 *  the hashing status in-tact.  All nodes are assigned a status of
 *  INTERIOR at allocation time.  Leaf status isn't known until
 *  some time afterwards.
 */
#define MakeLeafNode(pTN) \
 TN_NodeType(pTN) = TN_NodeType(pTN) | LEAF_NODE_MASK

#define TN_SetHashHdr(pTN,pTHT) TN_Child(pTN) = (void *)(pTHT)
#define BTN_SetHashHdr(pBTN,pTHT) TN_SetHashHdr(pBTN,pTHT)
#define TN_GetHashHdr(pTN)    TN_Child(pTN)
#define BTN_GetHashHdr(pTN)   ((BTHTptr)TN_GetHashHdr(pTN))

static TSTNptr new_tstn(CTXTdeclc int trie_t, int node_t, Cell symbol, TSTNptr parent,
    TSTNptr sibling) {
  void * tstn;
  
  ALLOC_TST_ANSWER_TRIE_NODE(tstn);
  TN_Init(((TSTNptr)tstn),trie_t,node_t,symbol,parent,sibling);
  TSTN_TimeStamp(((TSTNptr)tstn)) = TSTN_DEFAULT_TIMESTAMP;
  return (TSTNptr)tstn;        
}

static BTNptr new_btn(CTXTdeclc int trie_t, int node_t, Cell symbol, BTNptr parent,
  BTNptr sibling) {
  void *btn;
  
  ALLOC_TST_ANSWER_TRIE_NODE(btn);
  TN_Init(((BTNptr)btn),trie_t,node_t,symbol,parent,sibling);
  return (BTNptr)btn;
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

/*
 * Allocate a TSI node, associate it with a TST node 'tstn', and place
 * it at the head of the Time-Stamp Index managed by the hash table 'ht'.
 *
 * This operation is used for symbols inserted into an established hash
 * table.  The timestamp of this new entry will be set at the end of the
 * TST Insert operation when we walk back up the trie adjusting all
 * timestamps to a new max as we go.  Hence, the head of the entry list
 * is where this new entry belongs.
 */
inline static TSINptr tsiHeadInsert(CTXTdeclc TSTHTptr ht, TSTNptr tstn) {
  TSINptr pTSIN;
  
  New_TSIN(pTSIN, tstn);
  TSIN_Prev(pTSIN) = NULL;
  TSIN_Next(pTSIN) = TSTHT_IndexHead(ht);
  TSIN_Prev(TSTHT_IndexHead(ht)) = pTSIN;
  TSTHT_IndexHead(ht) = pTSIN;
  return pTSIN;
}

/*
 *  Expand the hash table pointed to by 'pHT'.  Note that we can do this
 *  in place by using realloc() and noticing that, since the hash tables
 *  and hashing function are based on powers of two, a node existing in
 *  a bucket will either remain in that bucket -- in the lower part of
 *  the new table -- or jump to a corresponding bucket in the upper half
 *  of the expanded table.  This function can serve for all types of
 *  tries since only fields contained in a Basic Trie Hash Table are
 *  manipulated.
 *
 *  As expansion is a method for reducing access time and is not a
 *  critical operation, if the table cannot be expanded at this time due
 *  to memory limitations, then simply return.  Otherwise, initialize
 *  the top half of the new area, and rehash each node in the buckets of
 *  the lower half of the table.
 */

static inline void expand_trie_ht(CTXTdeclc BTHTptr pHT) {
  BTNptr *bucket_array;
  BTNptr *upper_buckets;
  BTNptr *bucket;
  BTNptr curNode;
  BTNptr nextNode;
  
  unsigned long new_size;
  
  new_size = TrieHT_NewSize(pHT);
  
  ALLOC_HASH_BUCKETS(bucket_array, new_size);
  
  if(IsNULL(bucket_array)) return;
  
  upper_buckets = BTHT_BucketArray(pHT) + BTHT_NumBuckets(pHT);
  
  BTHT_NumBuckets(pHT) = new_size;
  new_size--;
  
  for(bucket = BTHT_BucketArray(pHT); bucket < upper_buckets; bucket++) {
    curNode = *bucket;
    while(IsNonNULL(curNode)) {
      nextNode = (BTNptr)TN_Sibling(curNode);
      TrieHT_InsertNode(bucket_array, new_size, curNode);
      curNode = nextNode;
    }
  }
  
  FREE_HASH_BUCKETS(BTHT_BucketArray(pHT));
  BTHT_BucketArray(pHT) = bucket_array;
}

static inline BTHTptr New_BTHT(int TrieType) {
  TSTHTptr btht;
  
  ALLOC_TST_ANSWER_TRIE_HASH(btht);
  ALLOC_HASH_BUCKETS(btht, TrieHT_INIT_SIZE);
  TSTHT_Instr(btht) = hash_opcode;
  TSTHT_Status(btht) = VALID_NODE_STATUS;
  TSTHT_TrieType(btht) = TrieType;
  TSTHT_NodeType(btht) = HASH_HEADER_NT;
  TSTHT_NumContents(btht) = MAX_SIBLING_LEN + 1;
  TSTHT_NumBuckets(btht) = TrieHT_INIT_SIZE;
  
  return (BTHTptr)btht;
}

void hashify_children(CTXTdeclc BTNptr parent, int trieType) {
  BTNptr children; /* child list of the parent */
  BTNptr btn; /* current child for processing */
  BTHTptr ht; /* HT header struct */
  BTNptr *tablebase; /* first bucket of allocated HT */
  unsigned long hashseed; /* needed for hashing of BTNs */
  
  ht = New_BTHT(trieType);
  children = BTN_Child(parent);
  BTN_SetHashHdr(parent,ht);
  tablebase = BTHT_BucketArray(ht);
  hashseed = BTHT_GetHashSeed(ht);
  for(btn = children; IsNonNULL(btn); btn = children) {
    children = BTN_Sibling(btn);
    TrieHT_InsertNode(tablebase, hashseed, btn);
    MakeHashedNode(btn);
  }
}

/*
 * Inserts a node containing 'symbol' in the appropriate bucket of the
 * hash table maintained by 'parent' and returns a pointer to this node.
 * If this addition causes the chain to become "too long", then expand
 * the hash table.
 */
inline static
TSTNptr tsthtInsertSymbol(CTXTdeclc TSTNptr parent, Cell symbol, int trieType,
  xsbBool maintainsTSI) {
  
  TSTHTptr ht;
  TSTNptr tstn, chain, *bucket;
  int chain_length;
  
  ht = TSTN_GetHashHdr(parent);
  bucket = CalculateBucketForSymbol(ht, symbol);
  chain = *bucket;
  New_TSTN(tstn,trieType,HASHED_INTERIOR_NT,symbol,parent,chain);
  *bucket = tstn;
  TSTHT_NumContents(ht)++;
  if(maintainsTSI)
    TSTN_SetTSIN(tstn, tsiHeadInsert(CTXTc ht, tstn));
  chain_length = 1;
  while(IsNonNULL(chain)) {
    chain_length++;
    chain = (TSTNptr)TSTN_Sibling(chain);
  }
  
  TrieHT_ExpansionCheck(ht, chain_length);
  return tstn;
}

inline static
BTNptr btnInsertSymbol(CTXTdeclc BTNptr parent, Cell symbol, int trieType) {
  BTNptr btn, chain;
  int chain_length;
  
  chain = BTN_Child(parent);
  New_BTN(btn,trieType,INTERIOR_NT,symbol,parent,chain);
  BTN_Child(parent) = btn;
  chain_length = 1;
  
  while(IsNonNULL(chain)) {
    chain_length++;
    chain = BTN_Sibling(chain);
  }
  
  if(IsLongSiblingChain(chain_length))
    hashify_children(CTXTc parent, trieType);
  return btn;
}

/*
 * Increase the time stamp of a hashed TSTN to that which is greater
 * than any other.  Hence, its TSI entry must be moved to the head of
 * the list to maintain our ordering property.
 */
inline static void tsiPromoteEntry(TSTNptr tstn, TimeStamp ts) {
  TSINptr tsin;
  TSTHTptr ht;
  
  tsin = TSTN_GetTSIN(tstn);
  TSIN_TimeStamp(tsin) = ts;
  if(IsTSindexHead(tsin))
    return;
  
  /* Splice out the TSIN from the Index
     ---------------------------------- */
  ht = TSTN_GetHashHdr(TSTN_Parent(tstn));
  TSIN_Next(TSIN_Prev(tsin)) = TSIN_Next(tsin);
  if(IsTSindexTail(tsin))
    TSTHT_IndexTail(ht) = TSIN_Prev(tsin);
  else
    TSIN_Prev(TSIN_Next(tsin)) = TSIN_Prev(tsin);
  
  /* Place the TSIN at the head of the Index
     --------------------------------------- */
  TSIN_Prev(tsin) = NULL;
  TSIN_Next(tsin) = TSTHT_IndexHead(ht);
  TSIN_Prev(TSTHT_IndexHead(ht)) = tsin;
  TSTHT_IndexHead(ht) = tsin;   
}


/*
 *                 Updating Time Stamps Along a New Path
 *                 =====================================
 *
 *  Given a pointer to a leaf TSTN for a newly inserted set of terms,
 *  update the timestamps of all nodes lying along the path from this
 *  leaf to the root.  Updates effect the TSIs, if they exist.
 */
inline static void update_timestamps(TSTNptr tstLeaf, TSTNptr tstRoot,
              xsbBool containsTSIs) {
  TimeStamp tsNewAnswer;
  
  tsNewAnswer = TSTN_TimeStamp(tstRoot) + 1;
  if(containsTSIs)
    do {
      if(IsHashedNode(tstLeaf))
        tsiPromoteEntry(tstLeaf, tsNewAnswer);
      else
        TSTN_TimeStamp(tstLeaf) = tsNewAnswer;
      tstLeaf = (TSTNptr)TSTN_Parent(tstLeaf);
    } while(tstLeaf != tstRoot);
  else
    do {
      TSTN_TimeStamp(tstLeaf) = tsNewAnswer;
      tstLeaf = (TSTNptr)TSTN_Parent(tstLeaf);
    } while(tstLeaf != tstRoot);
  TSTN_TimeStamp(tstRoot) = tsNewAnswer;
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
  else if(IsHashHeader(TSTN_Child(lastMatch)))
    lastMatch = tsthtInsertSymbol(CTXTc lastMatch, symbol, trieType, maintainTSI);
  else
    lastMatch = (TSTNptr)btnInsertSymbol(CTXTc (BTNptr)lastMatch, symbol, trieType);
  
  /* insert remaining symbols */
  while(!TermStack_IsEmpty) {
    ProcessNextSubtermFromTrieStacks(symbol,std_var_num);
    lastMatch = tstnAddSymbol(CTXTc lastMatch,symbol,trieType);
  }
  update_timestamps(lastMatch,tstRoot,maintainTSI);
  MakeLeafNode(lastMatch);
  TN_UpgradeInstrTypeToSUCCESS(lastMatch,TrieSymbolType(symbol));
  AnsVarCtr = AnsVarCtr + std_var_num;
  return lastMatch;
}