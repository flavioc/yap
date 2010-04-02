
#include "Yap.h"
#include "Yatom.h"
#include "YapHeap.h"
#include "yapio.h"
#include "tab.tst.h"
#include "tab.xsb.h"
#include "tab.utils.h"
#include "tab.macros.h"

#ifdef TABLING_CALL_SUBSUMPTION

/* instruction macros */
#define TN_ForceInstrCPtoTRY(NODE)
#define TN_RotateInstrCPtoRETRYorTRUST(NODE)
#define TN_ForceInstrCPtoNOCP(NODE)
#define TN_ResetInstrCPs(NODE,SIBLING)

/* prototypes */
STD_PROTO(static inline void expand_trie_ht, (CTXTdeclc BTHTptr));
STD_PROTO(static inline TSTHTptr New_BTHT, (int));
STD_PROTO(static inline Term hash_time_stamped_node, (tst_node_ptr));

/* definitions */

#define MakeHashedNode(pTN) \
 TN_NodeType(pTN) = TN_NodeType(pTN) | HASHED_NODE_MASK

#define IsLongSiblingChain(ChainLength) (ChainLength > MAX_SIBLING_LEN)
#define CalculateBucketForSymbol(pHT,Symbol)  \
  (TrieHT_BucketArray(pHT) + TrieHash(Symbol, TrieHT_GetHashSeed(pHT)))
  
#define TrieHT_ExpansionCheck(pHT,NumBucketContents) {    \
  if((NumBucketContents > BUCKET_CONTENT_THRESHOLD) &&    \
      (TrieHT_NumContents(pHT) > TrieHT_NumBuckets(pHT))) \
      expand_trie_ht(CTXTc (BTHTptr)pHT);                 \
}

#define TrieHT_InsertNode(pBucketArray,HashSeed,pTN) {                    \
  void **pBucket;                                                         \
  pBucket = (void**)(pBucketArray +                                       \
          TrieHash(hash_time_stamped_node((tst_node_ptr)pTN),HashSeed));  \
  if ( IsNonNULL(*pBucket) ) {						                                \
    TN_ForceInstrCPtoTRY(pTN);						                                \
    TN_RotateInstrCPtoRETRYorTRUST((BTNptr)*pBucket);			                \
  }									                                                      \
  else									                                                  \
    TN_ForceInstrCPtoNOCP(pTN);					                                  \
  TN_Sibling(pTN) = *pBucket;                                             \
  *pBucket = pTN;                                                         \
}

#define New_TSTHT(TSTHT,TrieType,TST) {                   \
  TSTHT = New_BTHT(TrieType);                             \
  TSTHT_InternalLink(TSTHT) = TSTRoot_GetHTList(TST);     \
  TSTRoot_SetHTList(TST,TSTHT);                           \
  TSTHT_IndexHead(TSTHT) = TSTHT_IndexTail(TSTHT) = NULL; \
}

#define New_TSIN(TSIN, TSTN) {                  \
  void *t;                                      \
  ALLOC_TST_INDEX_NODE(t);                      \
  TSIN = (TSINptr)t;                            \
  TSIN_TSTNode(TSIN) = TSTN;                    \
  TSIN_TimeStamp(TSIN) = TSTN_TimeStamp(TSTN);  \
}

static inline Term
hash_time_stamped_node(tst_node_ptr node) {
 int flags = TrNode_node_type(node);

 if(IS_LONG_INT_FLAG(flags)) {
   Int li = TSTN_long_int((long_tst_node_ptr)node);

   return (Term)li;
 } else if(IS_FLOAT_FLAG(flags)) {
   Float flt = TSTN_float((float_tst_node_ptr)node);

   return (Term)flt;
 } else
   return TSTN_entry(node);
}

#define TN_SetInstr(pTN,Symbol)                               \
  switch(TrieSymbolType(Symbol))  {                           \
    case XSB_STRUCT:                                          \
      TN_Instr(pTN) = _trie_retry_struct;                     \
      break;                                                  \
    case XSB_INT:                                             \
    case XSB_STRING:                                          \
      TN_Instr(pTN) = _trie_retry_atom;                       \
      break;                                                  \
    case XSB_TrieVar:                                         \
      if(IsNewTrieVar(Symbol))                                \
        TN_Instr(pTN) = _trie_retry_var;                      \
      else                                                    \
        TN_Instr(pTN) = _trie_retry_val;                      \
      break;                                                  \
    case XSB_LIST:                                            \
      TN_Instr(pTN) = _trie_retry_pair;                       \
      break;                                                  \
    case TAG_LONG_INT:                                        \
      TN_Instr(pTN) = _trie_retry_long_int;                   \
      break;                                                  \
    case TAG_FLOAT:                                           \
      TN_Instr(pTN) = _trie_retry_float_val;                  \
      break;                                                  \
    default:                                                  \
      xsb_abort("Trie Node creation: Bad tag in symbol %lx",  \
                  Symbol);  \
  }

#define TN_Init(TN,TrieType,NodeType,Symbol,Parent,Sibling) \
{                                                           \
  if ( NodeType != TRIE_ROOT_NT ) {				                  \
     TN_SetInstr(TN,Symbol);					                      \
     TN_ResetInstrCPs(TN,Sibling);				                  \
   }								                                        \
   else								                                      \
     TN_Instr(TN) = trie_root;					                    \
  TN_NodeType(TN) = NodeType | TrieType;                    \
  TN_Symbol(TN) = Symbol;                                   \
  TN_Parent(TN) = Parent;                                   \
  TN_Child(TN) = NULL;                                      \
  TN_Sibling(TN) = Sibling;                                 \
}

TSTNptr new_tstn(CTXTdeclc int trie_t, int node_t, Cell symbol, TSTNptr parent,
    TSTNptr sibling) {
  void * tstn;
  
  if(IS_LONG_INT_FLAG(node_t)) {
    ALLOC_LONG_TST_NODE(tstn);
    TSTN_long_int((long_tst_node_ptr)tstn) = *(Int *)symbol;
    TN_Init(((TSTNptr)tstn),trie_t,node_t,EncodedLongFunctor,parent,sibling);
  } else if(IS_FLOAT_FLAG(node_t)) {
    ALLOC_FLOAT_TST_NODE(tstn);
    TSTN_float((float_tst_node_ptr)tstn) = *(Float *)symbol;
    TN_Init(((TSTNptr)tstn),trie_t,node_t,EncodedFloatFunctor,parent,sibling);
  } else {
    ALLOC_TST_ANSWER_TRIE_NODE(tstn);
    TN_Init(((TSTNptr)tstn),trie_t,node_t,symbol,parent,sibling);
  }
  TSTN_TimeStamp(((TSTNptr)tstn)) = TSTN_DEFAULT_TIMESTAMP;
  return (TSTNptr)tstn;
}

static inline TSTHTptr New_BTHT(int TrieType) {
  TSTHTptr btht;
  
  ALLOC_TST_ANSWER_TRIE_HASH(btht);
  ALLOC_HASH_BUCKETS(TSTHT_BucketArray(btht), TrieHT_INIT_SIZE);
  TSTHT_Instr(btht) = hash_opcode;
  TSTHT_NodeType(btht) = HASH_HEADER_NT | TrieType;
  TSTHT_NumContents(btht) = MAX_SIBLING_LEN + 1;
  TSTHT_NumBuckets(btht) = TrieHT_INIT_SIZE;
  
  return btht;
}

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

#include "xsb.tst.c"

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

#endif /* TABLING_CALL_SUBSUMPTION */