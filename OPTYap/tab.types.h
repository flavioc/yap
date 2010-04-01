#define new_root_subgoal_trie_node(NODE)                                  \
        ALLOC_SUBGOAL_TRIE_NODE(NODE);                                    \
        init_subgoal_trie_node(NODE, 0, NULL, NULL, NULL, TRIE_ROOT_NT)
        
#define new_root_sub_subgoal_trie_node(NODE)                              \
        { subg_node_ptr sub_node;                                         \
          ALLOC_SUB_SUBGOAL_TRIE_NODE(sub_node);                          \
          NODE = (sg_node_ptr)sub_node;                                   \
          init_subgoal_trie_node(NODE, 0, NULL, NULL, NULL,               \
              TRIE_ROOT_NT | CALL_SUB_TRIE_NT);                           \
          TrNode_num_gen(sub_node) = 0;                                   \
        }

#define new_subgoal_trie_node(NODE, ENTRY, CHILD, PARENT, NEXT, TYPE)               \
        INCREMENT_GLOBAL_TRIE_REFS(ENTRY);                                          \
        ALLOC_SUBGOAL_TRIE_NODE(NODE);                                              \
        init_subgoal_trie_node(NODE, ENTRY, CHILD, PARENT, NEXT, TYPE)
        
#define new_sub_subgoal_trie_node(NODE, ENTRY, CHILD, PARENT, NEXT, TYPE)     \
      { INCREMENT_GLOBAL_TRIE_REFS(ENTRY);                                    \
        subg_node_ptr sub_node;                                               \
        ALLOC_SUB_SUBGOAL_TRIE_NODE(sub_node);                                \
        NODE = (sg_node_ptr)sub_node;                                         \
        init_subgoal_trie_node(NODE, ENTRY,                                   \
            CHILD, PARENT, NEXT, TYPE | CALL_SUB_TRIE_NT);                    \
        TrNode_num_gen(sub_node) = 0;                                         \
      }
    
#define new_long_subgoal_trie_node(NODE, LONG, CHILD, PARENT, NEXT, TYPE) \
        { INCREMENT_GLOBAL_TRIE_REFS((Term)(LONG));                       \
          long_sg_node_ptr long_node;                                     \
          ALLOC_LONG_SUBGOAL_TRIE_NODE(long_node);                        \
          NODE = (sg_node_ptr)long_node;                                  \
          init_subgoal_trie_node(NODE, EncodedLongFunctor, CHILD,         \
              PARENT, NEXT, TYPE);                                        \
          TrNode_long_int(long_node) = LONG;                              \
        }
        
#define new_long_sub_subgoal_trie_node(NODE, LONG, CHILD, PARENT, NEXT, TYPE) \
        { INCREMENT_GLOBAL_TRIE_REFS((Term)(LONG));                           \
          long_subg_node_ptr long_node;                                       \
          ALLOC_LONG_SUB_SUBGOAL_TRIE_NODE(long_node);                        \
          NODE = (sg_node_ptr)long_node;                                      \
          init_subgoal_trie_node(NODE, EncodedLongFunctor, CHILD,             \
              PARENT, NEXT, TYPE);                                            \
          TrNode_long_int(long_node) = LONG;                                  \
          TrNode_num_gen((subg_node_ptr)long_node) = 0;                       \
        }
        
#define new_float_subgoal_trie_node(NODE, FLOAT, CHILD, PARENT, NEXT, TYPE) \
        { INCREMENT_GLOBAL_TRIE_REFS((Term)(FLOAT));                        \
          float_sg_node_ptr float_node;                                     \
          ALLOC_FLOAT_SUBGOAL_TRIE_NODE(float_node);                        \
          NODE = (sg_node_ptr)float_node;                                   \
          init_subgoal_trie_node(NODE, EncodedFloatFunctor, CHILD,          \
              PARENT, NEXT, TYPE);                                          \
          TrNode_float(float_node) = FLOAT;                                 \
        }

#define new_float_sub_subgoal_trie_node(NODE, FLOAT, CHILD, PARENT, NEXT, TYPE) \
        { INCREMENT_GLOBAL_TRIE_REFS((Term)(FLOAT));                            \
          float_subg_node_ptr float_node;                                       \
          ALLOC_FLOAT_SUB_SUBGOAL_TRIE_NODE(float_node);                        \
          NODE = (sg_node_ptr)float_node;                                       \
          init_subgoal_trie_node(NODE, EncodedFloatFunctor, CHILD,              \
            PARENT, NEXT, TYPE);                                                \
          TrNode_float(float_node) = FLOAT;                                     \
          TrNode_num_gen((subg_node_ptr)float_node) = 0;                        \
        }
        
#define init_subgoal_trie_node(NODE, ENTRY, CHILD, PARENT, NEXT, TYPE)  \
        TrNode_entry(NODE) = ENTRY;                                     \
        TrNode_init_lock_field(NODE);                                   \
        TrNode_child(NODE) = CHILD;                                     \
        TrNode_parent(NODE) = PARENT;                                   \
        TrNode_next(NODE) = NEXT;                                       \
        TrNode_node_type(NODE) = TYPE | CALL_TRIE_NT

STD_PROTO(static inline Int node_get_long_int, (sg_node_ptr node));
STD_PROTO(static inline Float node_get_float, (sg_node_ptr node));

#ifdef TABLING_CALL_SUBSUMPTION
static inline Int
node_get_long_int(sg_node_ptr node) {
  if(TrNode_is_sub_call(node)) {
    return TrNode_long_int((long_subg_node_ptr)node);
  } else if(TrNode_is_var_call(node)) {
    return TrNode_long_int((long_sg_node_ptr)node);
  } else {
    return TSTN_long_int((long_tst_node_ptr)node);
  }
}

static inline Float
node_get_float(sg_node_ptr node) {
  if(TrNode_is_sub_call(node)) {
    return TrNode_float((float_subg_node_ptr)node);
  } else if(TrNode_is_var_call(node)) {
    return TrNode_float((float_sg_node_ptr)node);
  } else {
    return TSTN_float((float_tst_node_ptr)node);
  }
}
#else
static inline Int
node_get_float(sg_node_ptr node) {
  return TrNode_long_int((long_sg_node_ptr)node);
}

static inline Float
node_get_double(sg_node_ptr node) {
  return TrNode_float(node);
}
#endif /* TABLING_CALL_SUBSUMPTION */
