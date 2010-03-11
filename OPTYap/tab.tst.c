
#include "Yap.h"
#include "Yatom.h"
#include "YapHeap.h"
#include "yapio.h"
#include "tab.tst.h"
#include "tab.xsb.h"
#include "tab.macros.h"

/* prototypes */
STD_PROTO(static inline void expand_trie_ht, (CTXTdeclc BTHTptr));
STD_PROTO(static inline TSTHTptr New_BTHT, (int));

#define New_TSIN(TSIN, TSTN) {  \
  void *t;  \
  ALLOC_TST_INDEX_NODE(t);  \
  TSIN = (TSINptr)t;  \
  TSIN_TSTNode(TSIN) = TSTN;  \
  TSIN_TimeStamp(TSIN) = TSTN_TimeStamp(TSTN);  \
}

#define New_TSTHT(TSTHT,TrieType,TST) { \
  TSTHT = New_BTHT(TrieType); \
  TSTHT_InternalLink(TSTHT) = TSTRoot_GetHTList(TST);  \
  TSTRoot_SetHTList(TST,TSTHT); \
  TSTHT_IndexHead(TSTHT) = TSTHT_IndexTail(TSTHT) = NULL; \
}

#define IsLongSiblingChain(ChainLength) (ChainLength > MAX_SIBLING_LEN)
  
#define TN_Init(TN,TrieType,NodeType,Symbol,Parent,Sibling) { \
  if(NodeType != TRIE_ROOT_NT)  { \
    TN_SetInstr(TN,Symbol); \
    TN_ResetInstrCPs(TN,Sibling); \
  } \
  else  \
    TN_Instr(TN) = trie_root; \
  TN_TrieType(TN) = TrieType; \
  TN_NodeType(TN) = NodeType; \
  TN_Symbol(TN) = Symbol; \
  TN_Parent(TN) = Parent; \
  TN_Child(TN) = NULL;  \
  TN_Sibling(TN) = Sibling; \
}
  
#define CalculateBucketForSymbol(pHT,Symbol)  \
  (TrieHT_BucketArray(pHT) + TrieHash(Symbol, TrieHT_GetHashSeed(pHT)))

#define TSTN_DEFAULT_TIMESTAMP 1
#define VALID_NODE_STATUS 0
#define HASHED_NODE_MASK 0x01
#define LEAF_NODE_MASK 0x02

#define TN_SetInstr(pTN,Symbol) \
  switch(TrieSymbolType(Symbol))  { \
    case XSB_STRUCT:  \
      TN_Instr(pTN) = _trie_retry_struct; \
      break;  \
    case XSB_INT: \
    case XSB_STRING:  \
      TN_Instr(pTN) = _trie_retry_atom; \
      break;  \
    case XSB_TrieVar: \
      if(IsNewTrieVar(Symbol))  \
        TN_Instr(pTN) = _trie_retry_var;  \
      else  \
        TN_Instr(pTN) = _trie_retry_val;  \
      break;  \
    case XSB_LIST:  \
      TN_Instr(pTN) = _trie_retry_pair; \
      break;  \
    case TAG_LONG_INT:  \
      TN_Instr(pTN) = _trie_retry_long_int; \
      break;  \
    case TAG_FLOAT: \
      TN_Instr(pTN) = _trie_retry_float_val;  \
      break;                \
    default:  \
      xsb_abort("Trie Node creation: Bad tag in symbol %lx", \
                  Symbol);  \
  }
  
#define TN_ResetInstrCPs(pHead, pSibling)
#define TN_ForceInstrCPtoNOCP(pTN)
#define TN_ForceInstrCPtoTRY(pTN)
#define TN_UpgradeInstrTypeToSUCCESS(pTN,SymbolTag)
#define TN_RotateInstrCPtoRETRYorTRUST(pTN)

#define TSTRoot_SetHTList(pTST,pTSTHT) TSTN_Sibling(pTST) = (TSTNptr)pTSTHT
#define TSTRoot_GetHTList(pTST) ((TSTHTptr)TSTN_Sibling(pTST))

#define TSTN_SetHashHdr(pTSTN,pTSTHT) TN_SetHashHdr(pTSTN,pTSTHT)

#define TrieHT_InsertNode(pBucketArray,HashSeed,pTN) {                  \
  void **pBucket;                                                       \
                                                                        \
  pBucket = (void**)(pBucketArray + TrieHash(TN_Symbol(pTN),HashSeed)); \
  if(IsNonNULL(*pBucket)) {                                             \
    TN_ForceInstrCPtoTRY(pTN);                                          \
    TN_RotateInstrCPtoRETRYorTRUST((BTNptr)*pBucket);                   \
  }                                                                     \
  else                                                                  \
    TN_ForceInstrCPtoNOCP(pTN);                                         \
  TN_Sibling(pTN) = *pBucket;                                           \
  *pBucket = pTN;                                                       \
}

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
 
#ifdef SUBSUMPTION_XSB
/*
 *  From an INTERIOR-typed node, create a LEAF-typed node, keeping
 *  the hashing status in-tact.  All nodes are assigned a status of
 *  INTERIOR at allocation time.  Leaf status isn't known until
 *  some time afterwards.
 */
#define MakeLeafNode(pTN) \
 TN_NodeType(pTN) = TN_NodeType(pTN) | LEAF_NODE_MASK
#else
#define MakeLeafNode(pTN)
#endif

#define TN_SetHashHdr(pTN,pTHT) TN_Child(pTN) = (void *)(pTHT)
#define BTN_SetHashHdr(pBTN,pTHT) TN_SetHashHdr(pBTN,pTHT)
#define TN_GetHashHdr(pTN)    TN_Child(pTN)
#define BTN_GetHashHdr(pTN)   ((BTHTptr)TN_GetHashHdr(pTN))

TSTNptr new_tstn(CTXTdeclc int trie_t, int node_t, Cell *symbol, TSTNptr parent,
    TSTNptr sibling) {
  void * tstn;
  
  dprintf("TrieType %d\n", trie_t);
  
  if(IS_LONG_INT_FLAG(node_t)) {
    ALLOC_LONG_TST_NODE(tstn);
    TSTN_long_int((long_tst_node_ptr)tstn) = (Int)*symbol;
    TN_Init(((TSTNptr)tstn),trie_t,node_t,EncodedLongFunctor,parent,sibling);
    dprintf("New tst long int %ld\n", *(Int *)symbol);
  } else if(IS_FLOAT_FLAG(node_t)) {
    ALLOC_FLOAT_TST_NODE(tstn);
    TSTN_float((float_tst_node_ptr)tstn) = *(Float *)symbol;
    TN_Init(((TSTNptr)tstn),trie_t,node_t,EncodedFloatFunctor,parent,sibling);
    dprintf("new tst float %lf\n", *(Float *)symbol);
  } else {
    dprintf("new normal tst node\n");
    ALLOC_TST_ANSWER_TRIE_NODE(tstn);
    TN_Init(((TSTNptr)tstn),trie_t,node_t,symbol == 0 ? 0 : *symbol,parent,sibling);
  }
  TSTN_TimeStamp(((TSTNptr)tstn)) = TSTN_DEFAULT_TIMESTAMP;
  return (TSTNptr)tstn;
}

/*
 * Adds a node containing 'symbol' below 'parent', which currently has
 * no children.
 */
inline static
TSTNptr tstnAddSymbol(CTXTdeclc TSTNptr parent, Cell *symbol, int trieType, int nodeType) {
  TSTNptr newTSTN;
  New_TSTN(newTSTN, trieType, nodeType, symbol, parent, NULL);
  TSTN_Child(parent) = newTSTN;
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

static inline TSTHTptr New_BTHT(int TrieType) {
  TSTHTptr btht;
  
  ALLOC_TST_ANSWER_TRIE_HASH(btht);
  ALLOC_HASH_BUCKETS(TSTHT_BucketArray(btht), TrieHT_INIT_SIZE);
  TSTHT_Instr(btht) = hash_opcode;
  //TSTHT_Status(btht) = VALID_NODE_STATUS;
  TSTHT_TrieType(btht) = TrieType;
  TSTHT_NodeType(btht) = HASH_HEADER_NT;
  TSTHT_NumContents(btht) = MAX_SIBLING_LEN + 1;
  TSTHT_NumBuckets(btht) = TrieHT_INIT_SIZE;
  
  return btht;
}

/*
 *  Used during the creation of a Time-Stamp Index, allocates a TSI node
 *  for a given TST node and inserts it into the TSI in (decreasing)
 *  timestamp order.
 *
 *  NOTE: We cannot assume that the time stamp of the incoming node is  
 *  greater than that of all of the nodes already present in the TSI.
 *  Although this is the norm once the TSI is established, when a
 *  sibling list is moved to a hashing format, Entries are created for
 *  the nodes one at a time, but this node-processing order is not
 *  guaranteed to coincide with time stamp order.
 */
inline static TSINptr tsiOrderedInsert(CTXTdeclc TSTHTptr ht, TSTNptr tstn) {
  TSINptr nextTSIN;
  TSINptr newTSIN;
  
  New_TSIN(newTSIN, tstn);
  
  /* Determine proper position for insertion
     --------------------------------------- */
  nextTSIN = TSTHT_IndexHead(ht);
  while(IsNonNULL(nextTSIN) &&
    (TSIN_TimeStamp(newTSIN) < TSIN_TimeStamp(nextTSIN)))
    nextTSIN = TSIN_Next(nextTSIN);
  
  /* Splice newTSIN between nextTSIN and its predecessor
     --------------------------------------------------- */
  if(IsNonNULL(nextTSIN)) {
    TSIN_Prev(newTSIN) = TSIN_Prev(nextTSIN);
    TSIN_Next(newTSIN) = nextTSIN;
    if(IsTSindexHead(nextTSIN))
      TSTHT_IndexHead(ht) = newTSIN;
    else
      TSIN_Next(TSIN_Prev(nextTSIN)) = newTSIN;
    TSIN_Prev(nextTSIN) = newTSIN;
  }
  else { /* Insertion is at the end of the TSIN list */
    TSIN_Prev(newTSIN) = TSTHT_IndexTail(ht);
    TSIN_Next(newTSIN) = NULL;
    if(IsNULL(TSTHT_IndexHead(ht))) /* First insertion into TSI */
      TSTHT_IndexHead(ht) = newTSIN;
    else
      TSIN_Next(TSTHT_IndexTail(ht)) = newTSIN;
    TSTHT_IndexTail(ht) = newTSIN;
  }
  
  return newTSIN;
}

/*
 * This function may be called externally, and is made available to
 * support lazy creation of Time-Stamp Indices.
 *
 * An example of this use is for incomplete subsumptive Answer Sets.
 * TSIs are created only once a properly subsumed subgoal is issued.
 */
void tstCreateTSIs(CTXTdeclc TSTNptr pTST) {
  TSTNptr *pBucket, tstn;
  TSTHTptr ht;
  int bucketNum;
  
  dprintf("Creating TST indices\n");
  if(IsNULL(pTST))
    return;
  
  /*** For each hash table ... ***/
  for(ht = TSTRoot_GetHTList(pTST); IsNonNULL(ht);
      ht = TSTHT_InternalLink(ht) ) {
    
    /*** For each bucket in this hash table ... ***/
    for( pBucket = TSTHT_BucketArray(ht), bucketNum = 0;
        (unsigned int)bucketNum < TSTHT_NumBuckets(ht);
        pBucket++, bucketNum++ )
        
        /*** For each TSTN in a bucket... ***/
          for(tstn = *pBucket; IsNonNULL(tstn); tstn = TSTN_Sibling(tstn))
          
            /*** Create a TSIN for each symbol (TSTN) ***/
            TSTN_SetTSIN(tstn,tsiOrderedInsert(CTXTc ht,tstn));
  }
}

/*
 * The number of children of 'parent' has increased beyond the threshold
 * and requires a hashing structure.  This function creates a hash table
 * and inserts the children into it.  The value of the third argument
 * determines whether a TSI is also created for the children.
 *
 * After its creation, the hash table is referenced through the `Child'
 * field of the parent.  Hash tables in a TST are also linked to one
 * another through the TST's root.
 */
inline static
void tstnHashifyChildren(CTXTdeclc TSTNptr parent, TSTNptr root, xsbBool createTSI) {
  TSTNptr children;
  TSTNptr tstn;
  TSTHTptr ht;
  TSTNptr *tablebase;
  unsigned long hashseed;
  
  New_TSTHT(ht,TSTN_TrieType(root),root);
  children = TSTN_Child(parent);
  TSTN_SetHashHdr(parent,ht);
  tablebase = (TSTNptr*)TSTHT_BucketArray(ht);
  hashseed = TSTHT_GetHashSeed(ht);
  for(tstn = children; IsNonNULL(tstn); tstn = children) {
    children = TSTN_Sibling(tstn);
    TrieHT_InsertNode(tablebase, hashseed, tstn);
    MakeHashedNode(tstn);
    if( createTSI )
      TSTN_SetTSIN(tstn, tsiOrderedInsert(CTXTc ht, tstn));
  }
}

/*
 * Inserts a node containing 'symbol' in the appropriate bucket of the
 * hash table maintained by 'parent' and returns a pointer to this node.
 * If this addition causes the chain to become "too long", then expand
 * the hash table.
 */
inline static
TSTNptr tsthtInsertSymbol(CTXTdeclc TSTNptr parent, Cell *symbol, int trieType,
  int nodeType, xsbBool maintainsTSI) {
  
  TSTHTptr ht;
  TSTNptr tstn, chain, *bucket;
  int chain_length;
  
  ht = TSTN_GetHashHdr(parent);
#ifdef SUBSUMPTION_YAP
  bucket = CalculateBucketForSymbol(ht, GET_HASH_SYMBOL(symbol[0], nodeType));
#else
  bucket = CalculateBucketForSymbol(ht, symbol[0]);
#endif
  chain = *bucket;
  New_TSTN(tstn,trieType,HASHED_INTERIOR_NT | nodeType,symbol,parent,chain);
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

/*
 * Inserts a node containing 'symbol' at the head of the sibling chain
 * below 'parent' and returns a pointer to this node.  If this addition
 * causes the chain to become "too long", then creates a hashing
 * environment for the children.
 */
inline static
TSTNptr tstnInsertSymbol(CTXTdeclc TSTNptr parent, Cell *symbol, int trieType,
  int nodeType, TSTNptr root, xsbBool createTSI) {
  TSTNptr tstn, chain;
  int chain_length;
  chain = TSTN_Child(parent);
  New_TSTN(tstn,trieType,nodeType,symbol,parent,chain);
  TSTN_Child(parent) = tstn;
  chain_length = 1;
  while(IsNonNULL(chain)) {
    chain_length++;
    chain = TSTN_Sibling(chain);
  }
  if(IsLongSiblingChain(chain_length))
    tstnHashifyChildren(CTXTc parent,root,createTSI);
  return tstn;
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
#ifdef SUBSUMPTION_YAP
#if SIZEOF_DOUBLE == 2 * SIZEOF_INT_P
  Cell symbol[2]; /* for floats */
#else
  Cell symbol[1];
#endif 
#else
  Cell symbol[1];
#endif /* SUBSUMPTION_YAP */
  int std_var_num,
      trieType,
      nodeType;

  symbol[0] = firstSymbol;
  std_var_num = Trail_NumBindings;
  trieType = TSTN_TrieType(tstRoot);
  
  /* Insert initial symbol
     --------------------- */
  if ( firstSymbol == NO_INSERT_SYMBOL )
    ProcessNextSubtermFromTrieStacks(symbol[0], nodeType, std_var_num);
  
  if(IsNULL(TSTN_Child(lastMatch)))
    lastMatch = tstnAddSymbol(CTXTc lastMatch, symbol, trieType, nodeType);
  else if(IsHashHeader(TSTN_Child(lastMatch)))
    lastMatch = tsthtInsertSymbol(CTXTc lastMatch, symbol, trieType, nodeType, maintainTSI);
  else
    lastMatch = tstnInsertSymbol(CTXTc lastMatch, symbol, trieType, nodeType,
          tstRoot, maintainTSI);
  
  /* insert remaining symbols */
  while(!TermStack_IsEmpty) {
    ProcessNextSubtermFromTrieStacks(symbol[0],nodeType,std_var_num);
    lastMatch = tstnAddSymbol(CTXTc lastMatch,symbol,trieType,nodeType);
  }
  update_timestamps(lastMatch,tstRoot,maintainTSI);
  MakeLeafNode(lastMatch);
#ifdef SUBSUMPTION_XSB
  TN_UpgradeInstrTypeToSUCCESS(lastMatch,TrieSymbolType(symbol[0]));
#endif
  AnsVarCtr = AnsVarCtr + std_var_num;
  return lastMatch;
}

/* remove tst indices from the hash table */
void tstht_remove_index(TSTHTptr ht) {
  TSINptr head = TSTHT_IndexHead(ht);
  TSINptr saved_head;
  
  while(head) {
    saved_head = TSIN_Next(head);
    
    FREE_TST_INDEX_NODE(head);
    
    head = saved_head;
  }
  
  TSTHT_IndexHead(ht) = NULL;
  TSTHT_IndexTail(ht) = NULL;
}

void print_hash_table(TSTHTptr ht) {
  tst_node_ptr *bucket = TSTHT_buckets(ht);
  tst_node_ptr *last_bucket = bucket + TSTHT_num_buckets(ht);
  dprintf("Num buckets: %d\n", TSTHT_num_buckets(ht));
  dprintf("Num nodes: %d\n", TSTHT_num_nodes(ht));
  
  int i = 0;
  while(bucket != last_bucket) {
    ++i;
    
    if(*bucket) {
      // count
      int count = 0;
      tst_node_ptr link = *bucket;
      
      while(link) {
        count++;
        link = TSTN_next(link);
      }
      
      dprintf("Bucket %d with %d\n", i, count);
    }
    
    ++bucket;
  }
  
}
