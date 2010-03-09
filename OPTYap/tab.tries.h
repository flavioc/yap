/* ------------------------------------- **
**      Local functions declaration      **
** ------------------------------------- */

#ifdef YAPOR
#ifdef TABLING_INNER_CUTS
static int update_answer_trie_branch(ans_node_ptr previous_node, ans_node_ptr current_node);
#else
static int update_answer_trie_branch(ans_node_ptr current_node);
#endif /* TABLING_INNER_CUTS */
#else
static void update_answer_trie_branch(ans_node_ptr current_node, int position);
#endif /* YAPOR */
static void traverse_subgoal_trie(sg_node_ptr current_node, char *str, int str_index, int *arity, int mode, int position, tab_ent_ptr tab_ent);
static void traverse_answer_trie(ans_node_ptr current_node, char *str, int str_index, int *arity, int var_index, int mode, int position);
static void traverse_trie_node(Term t, char *str, int *str_index_ptr, int *arity, int *mode_ptr, int type);
#ifdef GLOBAL_TRIE
static void free_global_trie_branch(gt_node_ptr current_node);
static void traverse_global_trie(gt_node_ptr current_node, char *str, int str_index, int *arity, int mode, int position);
static void traverse_global_trie_for_subgoal(gt_node_ptr current_node, char *str, int *str_index, int *arity, int *mode);
static void traverse_global_trie_for_answer(gt_node_ptr current_node, char *str, int *str_index, int *arity, int *mode);
#endif /* GLOBAL_TRIE */



/* ----------------------- **
**      Local inlines      **
** ----------------------- */

#define IS_LONG_INT_FLAG(FLAG) ((FLAG) & LONG_INT_NT)

#ifdef GLOBAL_TRIE
STD_PROTO(static inline gt_node_ptr global_trie_node_check_insert, (gt_node_ptr, Term));
STD_PROTO(static inline sg_node_ptr subgoal_trie_node_check_insert, (tab_ent_ptr, sg_node_ptr, gt_node_ptr));
STD_PROTO(static inline ans_node_ptr answer_trie_node_check_insert, (sg_fr_ptr, ans_node_ptr, gt_node_ptr, int));
#else
STD_PROTO(static inline sg_node_ptr subgoal_trie_node_check_insert, (tab_ent_ptr, sg_node_ptr, Term, int));
STD_PROTO(static inline ans_node_ptr answer_trie_node_check_insert, (sg_fr_ptr, ans_node_ptr, Term, int));
#endif /* GLOBAL_TRIE */


#if defined(TABLE_LOCK_AT_WRITE_LEVEL)
#define LOCK_NODE(NODE)    LOCK_TABLE(NODE)
#define UNLOCK_NODE(NODE)  UNLOCK_TABLE(NODE)
#elif defined(TABLE_LOCK_AT_NODE_LEVEL)
#define LOCK_NODE(NODE)    TRIE_LOCK(TrNode_lock(NODE))
#define UNLOCK_NODE(NODE)  UNLOCK(TrNode_lock(NODE))
#else
#define LOCK_NODE(NODE)
#define UNLOCK_NODE(NODE)
#endif /* TABLE_LOCK_LEVEL */


#ifdef GLOBAL_TRIE
#define SUBGOAL_TOKEN_CHECK_INSERT(TAB_ENT, NODE, TOKEN)                \
        NODE = global_trie_node_check_insert(NODE, TOKEN)
#define ANSWER_TOKEN_CHECK_INSERT(SG_FR, NODE, TOKEN, INSTR)            \
        NODE = global_trie_node_check_insert(NODE, TOKEN)
#else
#define SUBGOAL_TOKEN_CHECK_INSERT(TAB_ENT, NODE, TOKEN, FLAGS)         \
        NODE = subgoal_trie_node_check_insert(TAB_ENT, NODE, TOKEN, FLAGS)
#define ANSWER_TOKEN_CHECK_INSERT(SG_FR, NODE, TOKEN, INSTR)	        \
        NODE = answer_trie_node_check_insert(SG_FR, NODE, TOKEN, INSTR)
#endif /* GLOBAL_TRIE */


#ifdef TABLE_LOCK_AT_WRITE_LEVEL

static inline sg_node_ptr
find_subgoal_node(sg_node_ptr chain_node, sg_node_ptr child_node, sg_node_ptr parent_node, Term t, int flags) {
  do {
    if (TrNode_entry(chain_node) == t) {
#ifdef ALLOC_BEFORE_CHECK
      FREE_SUBGOAL_TRIE_NODE(child_node);
#endif /* ALLOC_BEFORE_CHECK */
      UNLOCK_NODE(parent_node);
      return chain_node;
    }
    chain_node = TrNode_next(chain_node);
  } while (chain_node);

  return NULL;
}

static inline sg_node_ptr
find_subgoal_node_and_count(sg_node_ptr chain_node, sg_node_ptr first_node,
      sg_node_ptr child_node, sg_node_ptr parent_node, int *count, Term t, int flags)
{
  do {
    if (TrNode_entry(chain_node) == t) {
#ifdef ALLOC_BEFORE_CHECK
      FREE_SUBGOAL_TRIE_NODE(child_node);
#endif /* ALLOC_BEFORE_CHECK */
      UNLOCK_NODE(parent_node);
      return chain_node;
    }
    *count++;
    chain_node = TrNode_next(chain_node);
  } while (chain_node != first_node);

  return NULL;
}

static inline sg_node_ptr
find_subgoal_node_count_simple(sg_node_ptr chain_node, int *count, Term t, int flags) {
  while(chain_node) {
    if (TrNode_entry(chain_node) == t) {
      return chain_node;
    }
    *count++;
    chain_node = TrNode_next(chain_node);
  }

  return NULL;
}


static inline
sg_node_ptr subgoal_trie_node_check_insert(tab_ent_ptr tab_ent, sg_node_ptr parent_node, Term t, int flags) {
  sg_node_ptr child_node;
  sg_hash_ptr hash;

  child_node = TrNode_child(parent_node);

  if (child_node == NULL) {
#ifdef ALLOC_BEFORE_CHECK
    new_subgoal_trie_node(child_node, t, NULL, parent_node, NULL, flags);
#endif /* ALLOC_BEFORE_CHECK */
    LOCK_NODE(parent_node);
    if (TrNode_child(parent_node)) {
      sg_node_ptr chain_node = TrNode_child(parent_node);
      if (IS_SUBGOAL_TRIE_HASH(chain_node)) {
#ifdef ALLOC_BEFORE_CHECK
        FREE_SUBGOAL_TRIE_NODE(child_node);
#endif /* ALLOC_BEFORE_CHECK */
        UNLOCK_NODE(parent_node);
        hash = (sg_hash_ptr) chain_node;
        goto subgoal_trie_hash;
      }
      chain_node = find_subgoal_node(chain_node, child_node, parent_node, t, flags);
      if(chain_node)
        return chain_node;
#ifdef ALLOC_BEFORE_CHECK
      TrNode_next(child_node) = TrNode_child(parent_node);
#else
      new_subgoal_trie_node(child_node, t, NULL, parent_node, TrNode_child(parent_node), flags);
    } else {
      new_subgoal_trie_node(child_node, t, NULL, parent_node, NULL, flags);
#endif /* ALLOC_BEFORE_CHECK */
    }
    TrNode_child(parent_node) = child_node;
    UNLOCK_NODE(parent_node);
    return child_node;
  } 

  if (! IS_SUBGOAL_TRIE_HASH(child_node)) {
    sg_node_ptr first_node = child_node;
    int count_nodes = 0;
    child_node = find_subgoal_node_count_simple(child_node, &count_nodes, t, flags);
    if(child_node)
      return child_node;
    
#ifdef ALLOC_BEFORE_CHECK
    new_subgoal_trie_node(child_node, t, NULL, parent_node, first_node, flags);
#endif /* ALLOC_BEFORE_CHECK */
    LOCK_NODE(parent_node);
    if (first_node != TrNode_child(parent_node)) {
      sg_node_ptr chain_node = TrNode_child(parent_node);
      if (IS_SUBGOAL_TRIE_HASH(chain_node)) {
#ifdef ALLOC_BEFORE_CHECK
        FREE_SUBGOAL_TRIE_NODE(child_node);
#endif /* ALLOC_BEFORE_CHECK */
        UNLOCK_NODE(parent_node);
        hash = (sg_hash_ptr) chain_node;
        goto subgoal_trie_hash;
      }
      chain_node = find_subgoal_node_and_count(chain_node, first_node, child_node, parent_node, &count_nodes, t, flags);
      if(chain_node)
        return chain_node;
#ifdef ALLOC_BEFORE_CHECK
      TrNode_next(child_node) = TrNode_child(parent_node);
#else
      new_subgoal_trie_node(child_node, t, NULL, parent_node, TrNode_child(parent_node), flags);
    } else {
      new_subgoal_trie_node(child_node, t, NULL, parent_node, first_node, flags);
#endif /* ALLOC_BEFORE_CHECK */
    }
    count_nodes++;
    if (count_nodes >= MAX_NODES_PER_TRIE_LEVEL) {
      /* alloc a new hash */
      sg_node_ptr chain_node, next_node, *bucket;
      int entry;
      new_subgoal_trie_hash(hash, count_nodes, tab_ent);
      chain_node = child_node;
      do {
        entry = HASH_ENTRY(TrNode_entry(chain_node), BASE_HASH_BUCKETS - 1);
        bucket = Hash_bucket(hash, entry);
        next_node = TrNode_next(chain_node);
        TrNode_node_type(chain_node) |= HASHED_INTERIOR_NT;
        TrNode_next(chain_node) = *bucket;
        *bucket = chain_node;
        chain_node = next_node;
      } while (chain_node);
      TrNode_child(parent_node) = (sg_node_ptr) hash;
    } else {
      TrNode_child(parent_node) = child_node;
    }
    UNLOCK_NODE(parent_node);
    return child_node;
  }

  hash = (sg_hash_ptr) child_node;
subgoal_trie_hash:
  { /* trie nodes with hashing */
    sg_node_ptr *bucket, first_node;
    int seed, count_nodes = 0;

    seed = Hash_seed(hash);
    bucket = Hash_bucket(hash, HASH_ENTRY(t, seed));
    first_node = child_node = *bucket;
    child_node = find_subgoal_node_count_simple(child_node, &count_nodes, t, flags);
    
    if(child_node)
      return child_node;
      
#ifdef ALLOC_BEFORE_CHECK
    new_subgoal_trie_node(child_node, t, NULL, parent_node, first_node, HASHED_INTERIOR_NT | flags);
#endif /* ALLOC_BEFORE_CHECK */
    LOCK_NODE(parent_node);
    if (seed != Hash_seed(hash)) {
      /* the hash has been expanded */ 
#ifdef ALLOC_BEFORE_CHECK
      FREE_SUBGOAL_TRIE_NODE(child_node);
#endif /* ALLOC_BEFORE_CHECK */
      UNLOCK_NODE(parent_node);
      goto subgoal_trie_hash;
    }
    if (first_node != *bucket) {
      sg_node_ptr chain_node = *bucket;
      chain_node = find_subgoal_node_and_count(chain_node, first_node, child_node, parent_node, &count_nodes, t, flags);
      if(chain_node)
        return chain_node;
#ifdef ALLOC_BEFORE_CHECK
      TrNode_next(child_node) = *bucket;
#else
      new_subgoal_trie_node(child_node, t, NULL, parent_node, *bucket, HASHED_INTERIOR_NT | flags);
    } else {
      new_subgoal_trie_node(child_node, t, NULL, parent_node, first_node, HASHED_INTERIOR_NT | flags);
#endif /* ALLOC_BEFORE_CHECK */
    }
    *bucket = child_node;
    Hash_num_nodes(hash)++;
    count_nodes++;
    if (count_nodes >= MAX_NODES_PER_BUCKET && Hash_num_nodes(hash) > Hash_num_buckets(hash)) {
      /* expand current hash */ 
      sg_node_ptr chain_node, next_node, *first_old_bucket, *old_bucket;
      first_old_bucket = Hash_buckets(hash);
      old_bucket = first_old_bucket + Hash_num_buckets(hash);
      Hash_num_buckets(hash) *= 2;
      ALLOC_HASH_BUCKETS(Hash_buckets(hash), Hash_num_buckets(hash));
      seed = Hash_seed(hash);
      do {
        if (*--old_bucket) {
          chain_node = *old_bucket;
          do {
            bucket = Hash_bucket(hash, HASH_ENTRY(TrNode_entry(chain_node), seed));
            next_node = TrNode_next(chain_node);
            TrNode_next(chain_node) = *bucket;
            *bucket = chain_node;
            chain_node = next_node;
          } while (chain_node);
        }
      } while (old_bucket != first_old_bucket);
      FREE_HASH_BUCKETS(first_old_bucket);
    }
    UNLOCK_NODE(parent_node);
    return child_node;
  }
}


static inline
ans_node_ptr answer_trie_node_check_insert(sg_fr_ptr sg_fr, ans_node_ptr parent_node, Term t, int instr) {
  ans_node_ptr child_node;
  ans_hash_ptr hash;

#ifdef TABLING_ERRORS
  if (IS_ANSWER_LEAF_NODE(parent_node))
    TABLING_ERROR_MESSAGE("IS_ANSWER_LEAF_NODE(parent_node) (answer_trie_node_check_insert)");
#endif /* TABLING_ERRORS */

  child_node = TrNode_child(parent_node);

  if (child_node == NULL) {
#ifdef ALLOC_BEFORE_CHECK
    new_answer_trie_node(child_node, instr, t, NULL, parent_node, NULL);
#endif /* ALLOC_BEFORE_CHECK */
    LOCK_NODE(parent_node);
    if (TrNode_child(parent_node)) {
      ans_node_ptr chain_node = TrNode_child(parent_node);
      if (IS_ANSWER_TRIE_HASH(chain_node)) {
#ifdef ALLOC_BEFORE_CHECK
        FREE_ANSWER_TRIE_NODE(child_node);
#endif /* ALLOC_BEFORE_CHECK */
        UNLOCK_NODE(parent_node);
        hash = (ans_hash_ptr) chain_node;
        goto answer_trie_hash;
      }
      do {
        if (TrNode_entry(chain_node) == t) {
#ifdef ALLOC_BEFORE_CHECK
          FREE_ANSWER_TRIE_NODE(child_node);
#endif /* ALLOC_BEFORE_CHECK */
          UNLOCK_NODE(parent_node);
          return chain_node;
        }
        chain_node = TrNode_next(chain_node);
      } while (chain_node);
#ifdef ALLOC_BEFORE_CHECK
      TrNode_next(child_node) = TrNode_child(parent_node);
#else
      new_answer_trie_node(child_node, instr, t, NULL, parent_node, TrNode_child(parent_node));
    } else {
      new_answer_trie_node(child_node, instr, t, NULL, parent_node, NULL);
#endif /* ALLOC_BEFORE_CHECK */
    }
    TrNode_child(parent_node) = child_node;
    UNLOCK_NODE(parent_node);
    return child_node;
  } 

  if (! IS_ANSWER_TRIE_HASH(child_node)) {
    ans_node_ptr first_node = child_node;
    int count_nodes = 0;
    do {
      if (TrNode_entry(child_node) == t) {
        return child_node;
      }
      count_nodes++;
      child_node = TrNode_next(child_node);
    } while (child_node);
#ifdef ALLOC_BEFORE_CHECK
    new_answer_trie_node(child_node, instr, t, NULL, parent_node, first_node);
#endif /* ALLOC_BEFORE_CHECK */
    LOCK_NODE(parent_node);
    if (first_node != TrNode_child(parent_node)) {
      ans_node_ptr chain_node = TrNode_child(parent_node);
      if (IS_ANSWER_TRIE_HASH(chain_node)) {
#ifdef ALLOC_BEFORE_CHECK
        FREE_ANSWER_TRIE_NODE(child_node);
#endif /* ALLOC_BEFORE_CHECK */
        UNLOCK_NODE(parent_node);
        hash = (ans_hash_ptr) chain_node; 
        goto answer_trie_hash;
      }
      do {
        if (TrNode_entry(chain_node) == t) {
#ifdef ALLOC_BEFORE_CHECK
          FREE_ANSWER_TRIE_NODE(child_node);
#endif /* ALLOC_BEFORE_CHECK */
          UNLOCK_NODE(parent_node);
          return chain_node;
        }
        count_nodes++;
        chain_node = TrNode_next(chain_node);
      } while (chain_node != first_node);
#ifdef ALLOC_BEFORE_CHECK
      TrNode_next(child_node) = TrNode_child(parent_node);
#else
      new_answer_trie_node(child_node, instr, t, NULL, parent_node, TrNode_child(parent_node));
    } else {
      new_answer_trie_node(child_node, instr, t, NULL, parent_node, first_node);
#endif /* ALLOC_BEFORE_CHECK */
    }
    count_nodes++;
    if (count_nodes >= MAX_NODES_PER_TRIE_LEVEL) {
      /* alloc a new hash */
      ans_node_ptr chain_node, next_node, *bucket;
      new_answer_trie_hash(hash, count_nodes, sg_fr);
      chain_node = child_node;
      do {
        bucket = Hash_bucket(hash, HASH_ENTRY(TrNode_entry(chain_node), BASE_HASH_BUCKETS - 1));
        next_node = TrNode_next(chain_node);
        TrNode_next(chain_node) = *bucket;
        *bucket = chain_node;
        chain_node = next_node;
      } while (chain_node);
      TrNode_child(parent_node) = (ans_node_ptr) hash;
    } else {
      TrNode_child(parent_node) = child_node;
    }
    UNLOCK_NODE(parent_node);
    return child_node;
  }

  hash = (ans_hash_ptr) child_node;
answer_trie_hash:
  { /* trie nodes with hashing */
    ans_node_ptr *bucket, first_node;
    int seed, count_nodes = 0;

    seed = Hash_seed(hash);
    bucket = Hash_bucket(hash, HASH_ENTRY(t, seed));
    first_node = child_node = *bucket;
    while (child_node) {
      if (TrNode_entry(child_node) == t) {
        return child_node;
      }
      count_nodes++;
      child_node = TrNode_next(child_node);
    }
#ifdef ALLOC_BEFORE_CHECK
    new_answer_trie_node(child_node, instr, t, NULL, parent_node, first_node);
#endif /* ALLOC_BEFORE_CHECK */
    LOCK_NODE(parent_node);
    if (seed != Hash_seed(hash)) {
      /* the hash has been expanded */ 
#ifdef ALLOC_BEFORE_CHECK
      FREE_ANSWER_TRIE_NODE(child_node);
#endif /* ALLOC_BEFORE_CHECK */
      UNLOCK_NODE(parent_node);
      goto answer_trie_hash;
    }
    if (first_node != *bucket) {
      ans_node_ptr chain_node = *bucket;
      do {
        if (TrNode_entry(chain_node) == t) {
#ifdef ALLOC_BEFORE_CHECK
          FREE_ANSWER_TRIE_NODE(child_node);
#endif /* ALLOC_BEFORE_CHECK */
          UNLOCK_NODE(parent_node);
          return chain_node;
        }
        count_nodes++;
        chain_node = TrNode_next(chain_node);
      } while (chain_node != first_node);
#ifdef ALLOC_BEFORE_CHECK
      TrNode_next(child_node) = *bucket;
#else
      new_answer_trie_node(child_node, instr, t, NULL, parent_node, *bucket);
    } else {
      new_answer_trie_node(child_node, instr, t, NULL, parent_node, first_node);
#endif /* ALLOC_BEFORE_CHECK */
    }
    *bucket = child_node;
    Hash_num_nodes(hash)++;
    count_nodes++;
    if (count_nodes >= MAX_NODES_PER_BUCKET && Hash_num_nodes(hash) > Hash_num_buckets(hash)) {
      /* expand current hash */ 
      ans_node_ptr chain_node, next_node, *first_old_bucket, *old_bucket;
      first_old_bucket = Hash_buckets(hash);
      old_bucket = first_old_bucket + Hash_num_buckets(hash);
      Hash_num_buckets(hash) *= 2;
      ALLOC_HASH_BUCKETS(Hash_buckets(hash), Hash_num_buckets(hash));
      seed = Hash_seed(hash);
      do {
        if (*--old_bucket) {
          chain_node = *old_bucket;
          do {
            bucket = Hash_bucket(hash, HASH_ENTRY(TrNode_entry(chain_node), seed));
            next_node = TrNode_next(chain_node);
            TrNode_next(chain_node) = *bucket;
            *bucket = chain_node;
            chain_node = next_node;
          } while (chain_node);
        }
      } while (old_bucket != first_old_bucket);
      FREE_HASH_BUCKETS(first_old_bucket);
    }
    UNLOCK_NODE(parent_node);
    return child_node;
  }
}
#else  /* TABLE_LOCK_AT_ENTRY_LEVEL || TABLE_LOCK_AT_NODE_LEVEL || ! YAPOR */
#ifdef GLOBAL_TRIE
static inline
gt_node_ptr global_trie_node_check_insert(gt_node_ptr parent_node, Term t) {   
  gt_node_ptr child_node;
    
  LOCK_NODE(parent_node);
  child_node = TrNode_child(parent_node);

  if (child_node == NULL) {
    new_global_trie_node(child_node, t, NULL, parent_node, NULL);
    TrNode_child(parent_node) = child_node;
    UNLOCK_NODE(parent_node);
    return child_node;
  }

  if (! IS_GLOBAL_TRIE_HASH(child_node)) {
    int count_nodes = 0;
    do {
      if (TrNode_entry(child_node) == t) {
        UNLOCK_NODE(parent_node);
        return child_node;
      }
      count_nodes++;
      child_node = TrNode_next(child_node);
    } while (child_node);
    new_global_trie_node(child_node, t, NULL, parent_node, TrNode_child(parent_node));
    count_nodes++;
    if (count_nodes >= MAX_NODES_PER_TRIE_LEVEL) {
      /* alloc a new hash */
      gt_hash_ptr hash;
      gt_node_ptr chain_node, next_node, *bucket;
      new_global_trie_hash(hash, count_nodes);
      chain_node = child_node;
      do {
        bucket = Hash_bucket(hash, HASH_ENTRY(TrNode_entry(chain_node), BASE_HASH_BUCKETS - 1));
        next_node = TrNode_next(chain_node);
        TrNode_next(chain_node) = *bucket;
        *bucket = chain_node;
        chain_node = next_node;
      } while (chain_node);
      TrNode_child(parent_node) = (gt_node_ptr) hash;
    } else {
      TrNode_child(parent_node) = child_node;
    }
    UNLOCK_NODE(parent_node);
    return child_node;
  }

  { /* trie nodes with hashing */
    gt_hash_ptr hash;
    gt_node_ptr *bucket;
    int count_nodes = 0;
    hash = (gt_hash_ptr) child_node; 
    bucket = Hash_bucket(hash, HASH_ENTRY(t, Hash_seed(hash)));
    child_node = *bucket;
    while (child_node) { 
      if (TrNode_entry(child_node) == t) {
        UNLOCK_NODE(parent_node);
        return child_node;
      }
      count_nodes++;
      child_node = TrNode_next(child_node);
    } 
    new_global_trie_node(child_node, t, NULL, parent_node, *bucket);
    *bucket = child_node;
    Hash_num_nodes(hash)++;
    count_nodes++;
    if (count_nodes >= MAX_NODES_PER_BUCKET && Hash_num_nodes(hash) > Hash_num_buckets(hash)) {
      /* expand current hash */
      gt_node_ptr chain_node, next_node, *first_old_bucket, *old_bucket;
      int seed;
      first_old_bucket = Hash_buckets(hash);
      old_bucket = first_old_bucket + Hash_num_buckets(hash);
      Hash_num_buckets(hash) *= 2;
      ALLOC_HASH_BUCKETS(Hash_buckets(hash), Hash_num_buckets(hash)); 
      seed = Hash_seed(hash);
      do {
        if (*--old_bucket) {
          chain_node = *old_bucket;
          do {
            bucket = Hash_bucket(hash, HASH_ENTRY(TrNode_entry(chain_node), seed));
            next_node = TrNode_next(chain_node);
            TrNode_next(chain_node) = *bucket;
            *bucket = chain_node;
            chain_node = next_node;
          } while (chain_node);
        }
      } while (old_bucket != first_old_bucket);
      FREE_HASH_BUCKETS(first_old_bucket);
    }
    UNLOCK_NODE(parent_node);
    return child_node;
  }
}
#endif /* GLOBAL_TRIE */

static inline sg_node_ptr
find_subgoal_node(sg_node_ptr child_node, sg_node_ptr parent_node, int *count_nodes, Term t, int flags) {
  while(child_node) {
    if(TrNode_entry(child_node) == t) {
      UNLOCK_NODE(parent_node);
      return child_node;
    }
    
    *count_nodes++;
    child_node = TrNode_next(child_node);
  }
  
  return NULL;
}

static inline
#ifdef GLOBAL_TRIE
sg_node_ptr subgoal_trie_node_check_insert(tab_ent_ptr tab_ent, sg_node_ptr parent_node, gt_node_ptr t) {
#else
sg_node_ptr subgoal_trie_node_check_insert(tab_ent_ptr tab_ent, sg_node_ptr parent_node, Term t, int flags) {
#endif /* GLOBAL_TRIE */
  sg_node_ptr child_node;
  
  LOCK_NODE(parent_node);
  child_node = TrNode_child(parent_node);

  if (child_node == NULL) {
    new_subgoal_trie_node(child_node, t, NULL, parent_node, NULL, flags);
    TrNode_child(parent_node) = child_node;
    UNLOCK_NODE(parent_node);
    return child_node;
  }

  if (! IS_SUBGOAL_TRIE_HASH(child_node)) {
    int count_nodes = 0;
    child_node = find_subgoal_node(child_node, parent_node, &count_nodes, t, flags);
    if(child_node)
      return child_node;
    new_subgoal_trie_node(child_node, t, NULL, parent_node, TrNode_child(parent_node), flags);
    count_nodes++;
    if (count_nodes >= MAX_NODES_PER_TRIE_LEVEL) {
      /* alloc a new hash */
      sg_hash_ptr hash;
      sg_node_ptr chain_node, next_node, *bucket;
      int entry;
      new_subgoal_trie_hash(hash, count_nodes, tab_ent);
      chain_node = child_node;
      do {
        entry = HASH_ENTRY(TrNode_entry(chain_node), BASE_HASH_BUCKETS - 1);
        bucket = Hash_bucket(hash, entry);
        next_node = TrNode_next(chain_node);
        TrNode_node_type(chain_node) |= HASHED_INTERIOR_NT;
        TrNode_next(chain_node) = *bucket;
        *bucket = chain_node;
        chain_node = next_node;
      } while (chain_node);
      TrNode_child(parent_node) = (sg_node_ptr) hash;
    } else {
      TrNode_child(parent_node) = child_node;
    }
    UNLOCK_NODE(parent_node);
    return child_node;
  }

  { /* trie nodes with hashing */
    sg_hash_ptr hash;
    sg_node_ptr *bucket;
    int count_nodes = 0;
    hash = (sg_hash_ptr) child_node;
    bucket = Hash_bucket(hash, HASH_ENTRY(t, Hash_seed(hash)));
    child_node = *bucket;
    child_node = find_subgoal_node(child_node, parent_node, &count_nodes, t, flags);
    if(child_node)
      return child_node;
    new_subgoal_trie_node(child_node, t, NULL, parent_node, *bucket, HASHED_INTERIOR_NT | flags);
    *bucket = child_node;
    Hash_num_nodes(hash)++;
    count_nodes++;
    if (count_nodes >= MAX_NODES_PER_BUCKET && Hash_num_nodes(hash) > Hash_num_buckets(hash)) {
      /* expand current hash */
      sg_node_ptr chain_node, next_node, *first_old_bucket, *old_bucket;
      int seed;
      first_old_bucket = Hash_buckets(hash);
      old_bucket = first_old_bucket + Hash_num_buckets(hash);
      Hash_num_buckets(hash) *= 2;
      ALLOC_HASH_BUCKETS(Hash_buckets(hash), Hash_num_buckets(hash));
      seed = Hash_seed(hash);
      do {
        if (*--old_bucket) {
          chain_node = *old_bucket;
          do {
            bucket = Hash_bucket(hash, HASH_ENTRY(TrNode_entry(chain_node), seed));
            next_node = TrNode_next(chain_node);
            TrNode_next(chain_node) = *bucket;
            *bucket = chain_node;
            chain_node = next_node;
          } while (chain_node);
        }
      } while (old_bucket != first_old_bucket);
      FREE_HASH_BUCKETS(first_old_bucket);
    }
    UNLOCK_NODE(parent_node);
    return child_node;
  }
}


static inline
#ifdef GLOBAL_TRIE
ans_node_ptr answer_trie_node_check_insert(sg_fr_ptr sg_fr, ans_node_ptr parent_node, gt_node_ptr t, int instr) {
#else
ans_node_ptr answer_trie_node_check_insert(sg_fr_ptr sg_fr, ans_node_ptr parent_node, Term t, int instr) {
#endif /* GLOBAL_TRIE */
  ans_node_ptr child_node;

#ifdef TABLING_ERRORS
  if (IS_ANSWER_LEAF_NODE(parent_node))
    TABLING_ERROR_MESSAGE("IS_ANSWER_LEAF_NODE(parent_node) (answer_trie_node_check_insert)");
#endif /* TABLING_ERRORS */

  LOCK_NODE(parent_node);
  child_node = TrNode_child(parent_node);

  if (child_node == NULL) {
    new_answer_trie_node(child_node, instr, t, NULL, parent_node, NULL);
    TrNode_child(parent_node) = child_node;
    UNLOCK_NODE(parent_node);
    return child_node;
  }

  if (! IS_ANSWER_TRIE_HASH(child_node)) {
    int count_nodes = 0;
    do {
      if (TrNode_entry(child_node) == t) {
        UNLOCK_NODE(parent_node);
        return child_node;
      }
      count_nodes++;
      child_node = TrNode_next(child_node);
    } while (child_node);
    new_answer_trie_node(child_node, instr, t, NULL, parent_node, TrNode_child(parent_node));
    count_nodes++;
    if (count_nodes >= MAX_NODES_PER_TRIE_LEVEL) {
      /* alloc a new hash */
      ans_hash_ptr hash;
      ans_node_ptr chain_node, next_node, *bucket;
      new_answer_trie_hash(hash, count_nodes, sg_fr);
      chain_node = child_node;
      do {
        bucket = Hash_bucket(hash, HASH_ENTRY(TrNode_entry(chain_node), BASE_HASH_BUCKETS - 1));
        next_node = TrNode_next(chain_node);
        TrNode_next(chain_node) = *bucket;
        *bucket = chain_node;
        chain_node = next_node;
      } while (chain_node);
      TrNode_child(parent_node) = (ans_node_ptr) hash;
    } else {
      TrNode_child(parent_node) = child_node;
    }
    UNLOCK_NODE(parent_node);
    return child_node;
  }

  { /* trie nodes with hashing */
    ans_hash_ptr hash;
    ans_node_ptr *bucket;
    int count_nodes = 0;
    hash = (ans_hash_ptr) child_node;
    bucket = Hash_bucket(hash, HASH_ENTRY(t, Hash_seed(hash)));
    child_node = *bucket;
    while (child_node) {
      if (TrNode_entry(child_node) == t) {
        UNLOCK_NODE(parent_node);
        return child_node;
      }
      count_nodes++;
      child_node = TrNode_next(child_node);
    }
    new_answer_trie_node(child_node, instr, t, NULL, parent_node, *bucket);
    *bucket = child_node;
    Hash_num_nodes(hash)++;
    count_nodes++;
    if (count_nodes >= MAX_NODES_PER_BUCKET && Hash_num_nodes(hash) > Hash_num_buckets(hash)) {
      /* expand current hash */ 
      ans_node_ptr chain_node, next_node, *first_old_bucket, *old_bucket;
      int seed;
      first_old_bucket = Hash_buckets(hash);
      old_bucket = first_old_bucket + Hash_num_buckets(hash);
      Hash_num_buckets(hash) *= 2;
      ALLOC_HASH_BUCKETS(Hash_buckets(hash), Hash_num_buckets(hash));
      seed = Hash_seed(hash);
      do {
        if (*--old_bucket) {
          chain_node = *old_bucket;
          do {
            bucket = Hash_bucket(hash, HASH_ENTRY(TrNode_entry(chain_node), seed));
            next_node = TrNode_next(chain_node);
            TrNode_next(chain_node) = *bucket;
            *bucket = chain_node;
            chain_node = next_node;
          } while (chain_node);
        }
      } while (old_bucket != first_old_bucket);
      FREE_HASH_BUCKETS(first_old_bucket);
    }
    UNLOCK_NODE(parent_node);
    return child_node;
  }
}
#endif /* TABLE_LOCK_LEVEL */