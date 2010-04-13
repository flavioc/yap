/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   Flávio Cruz - University of Porto, Portugal
  File:        tab.sub_macros.h
  version:       
                                                                     
**********************************************************************/

/* THIS FILE MUST BE INCLUDED THROUGH tab.macros.h */

#ifdef TABLING_CALL_SUBSUMPTION

STD_PROTO(static inline void mark_subsumptive_consumer_as_completed, (subcons_fr_ptr));
STD_PROTO(static inline void mark_subsumptive_producer_as_completed, (subprod_fr_ptr));
STD_PROTO(static inline void mark_ground_consumer_as_completed, (grounded_sf_ptr));
STD_PROTO(static inline void mark_ground_producer_as_completed, (grounded_sf_ptr));

STD_PROTO(static inline void gen_index_remove, (subg_node_ptr, subg_hash_ptr));
STD_PROTO(static inline void gen_index_add, (subg_node_ptr, subg_hash_ptr, int));
STD_PROTO(static inline void free_tst_hash_chain, (tst_ans_hash_ptr));
STD_PROTO(static inline void free_tst_hash_index, (tst_ans_hash_ptr));

STD_PROTO(static inline void free_producer_subgoal_data, (sg_fr_ptr, int));
STD_PROTO(static inline void free_consumer_subgoal_data, (subcons_fr_ptr));
STD_PROTO(static inline void free_ground_subgoal_data, (grounded_sf_ptr));
STD_PROTO(static inline void free_ground_trie, (tab_ent_ptr));

STD_PROTO(static inline void abolish_incomplete_subsumptive_producer_subgoal, (sg_fr_ptr));
STD_PROTO(static inline void abolish_incomplete_subsumptive_consumer_subgoal, (subcons_fr_ptr));
STD_PROTO(static inline void abolish_incomplete_ground_producer_subgoal, (sg_fr_ptr));
STD_PROTO(static inline void abolish_incomplete_ground_consumer_subgoal, (grounded_sf_ptr));

STD_PROTO(static inline int build_next_subsumptive_consumer_return_list, (subcons_fr_ptr));
STD_PROTO(static inline int build_next_ground_consumer_return_list, (grounded_sf_ptr));
STD_PROTO(static inline int build_next_ground_producer_return_list, (grounded_sf_ptr));

#define ground_trie_create_tsi(TAB_ENT) tstCreateTSIs((tst_node_ptr)TabEnt_ground_trie(TAB_ENT))

#define new_subsumptive_producer_subgoal_frame(SG_FR, CODE, LEAF) { \
        new_basic_subgoal_frame(SG_FR, CODE, LEAF,                  \
          SUBSUMPTIVE_PRODUCER_SFT, ALLOC_SUBPROD_SUBGOAL_FRAME);   \
        SgFr_prod_consumers(SG_FR) = NULL;                          \
        SgFr_answer_trie(SG_FR) = newTSTAnswerSet();                \
        RESET_VARIABLE(&SgFr_executing(SG_FR));                     \
        RESET_VARIABLE(&SgFr_start(SG_FR));                         \
    }
    
#define new_grounded_producer_subgoal_frame(SG_FR, CODE, LEAF) {  \
        new_basic_subgoal_frame(SG_FR, CODE, LEAF,                \
          GROUND_PRODUCER_SFT, ALLOC_GROUNDED_SUBGOAL_FRAME);     \
        init_ground_subgoal_frame(SG_FR);                         \
        SgFr_producer(SG_FR) = NULL;                              \
  }
  
#define init_ground_subgoal_frame(SG_FR)  \
        SgFr_choice_point(SG_FR) = NULL;  \
        SgFr_timestamp(SG_FR) = 0;      \
        SgFr_answer_template(SG_FR) = NULL;   \
        SgFr_num_ans(SG_FR) = 0;                \
        SgFr_saved_cp(SG_FR) = NULL;               \
        RESET_VARIABLE(&SgFr_executing(SG_FR));  \
        RESET_VARIABLE(&SgFr_start(SG_FR))
  
#define new_grounded_consumer_subgoal_frame(SG_FR, CODE, LEAF, PRODUCER) {  \
        new_basic_subgoal_frame(SG_FR, CODE, LEAF,                          \
          GROUND_CONSUMER_SFT, ALLOC_GROUNDED_SUBGOAL_FRAME);               \
        init_ground_subgoal_frame(SG_FR);                                   \
        SgFr_producer(SG_FR) = PRODUCER;                                    \
      }
    
#define new_subsumed_consumer_subgoal_frame(SG_FR, CODE, LEAF, PRODUCER) {  \
        new_basic_subgoal_frame(SG_FR, CODE, LEAF,                          \
            SUBSUMED_CONSUMER_SFT, ALLOC_SUBCONS_SUBGOAL_FRAME);            \
        add_answer_trie_subgoal_frame(SG_FR);                               \
        SgFr_timestamp(SG_FR) = 0;                                          \
        SgFr_choice_point(SG_FR) = NULL;                                         \
        SgFr_answer_template(SG_FR) = NULL;                                 \
        SgFr_producer(SG_FR) = PRODUCER;                                    \
        SgFr_consumers(SG_FR) = SgFr_prod_consumers(PRODUCER);              \
        if (!SgFr_prod_consumers(PRODUCER))                                 \
          tstCreateTSIs((tst_node_ptr)SgFr_answer_trie(PRODUCER));          \
        SgFr_prod_consumers(PRODUCER) = (subcons_fr_ptr)(SG_FR);            \
    }
    
#define init_sub_consumer_subgoal_frame(SG_FR)                      \
        { SgFr_state(SG_FR) = evaluating;                           \
          SgFr_choice_point(SG_FR) = B;                             \
          SgFr_next(SG_FR) = LOCAL_top_subcons_sg_fr;               \
          LOCAL_top_subcons_sg_fr = SG_FR;                          \
          SAVE_SG_TOP_GEN_SG(SG_FR);                                \
        }
        
#define init_ground_consumer_subgoal_frame(SG_FR)                   \
        { SgFr_state(SG_FR) = evaluating;                           \
          SgFr_choice_point(SG_FR) = B;                             \
          SgFr_next(SG_FR) = LOCAL_top_groundcons_sg_fr;            \
          LOCAL_top_groundcons_sg_fr = SG_FR;                       \
          increment_sugoal_path(SG_FR);                             \
          SAVE_SG_TOP_GEN_SG(SG_FR);                                \
        }
        
#define create_ground_answer_template(SG_FR, FROM)      \
        SgFr_answer_template(SG_FR) = SgFr_at_block(SG_FR); \
        SgFr_at_size(SG_FR) = copy_answer_template(FROM, SgFr_at_block(SG_FR))
        
#define ensure_ground_answer_template(SG_FR, FROM)      \
        if(SgFr_answer_template(SG_FR) == NULL) {       \
          create_ground_answer_template(SG_FR, FROM);   \
        }
        
#define show_consumer_subsumptive_answers(CONS_SG, PROD_SG, HEAP_SG) \
      {                                                     \
        continuation_ptr cont = SgFr_first_answer(CONS_SG); \
        CELL* ans_tmplt =                                   \
          reconstruct_template_for_producer_no_args(PROD_SG, \
                (HEAP_SG) - 1);  \
        tr_fr_ptr saved_TR = TR;  \
        CELL*     saved_HB = HB;  \
        const int ans_size = (int)*ans_tmplt; \
        ans_tmplt += ans_size;  \
        HB = H; \
        while(cont) { \
          tst_node_ptr ans = (tst_node_ptr)continuation_answer(cont); \
          consume_subsumptive_answer((BTNptr)ans, ans_size, ans_tmplt); \
          printSubsumptiveAnswer(Yap_stdout, HEAP_SG); \
          trail_unwind(saved_TR); \
          cont = continuation_next(cont); \
        } \
        /* restore TR and HB */ \
        TR = saved_TR;  \
        HB = saved_HB;  \
      }
      
#define show_ground_answers(SG_FR, ANS_TMPLT, VARS)         \
      {                                                     \
        continuation_ptr cont = SgFr_first_answer(SG_FR);   \
        tr_fr_ptr saved_TR = TR;                            \
        CELL *    saved_HB = HB;                            \
        int ans_size = SgFr_arity(SG_FR);                   \
                                                            \
        HB = H;                                             \
        while(cont) {                                       \
          tst_node_ptr ans = (tst_node_ptr) continuation_answer(cont);  \
          consume_subsumptive_answer((BTNptr)ans, ans_size, ANS_TMPLT); \
          printSubsumptiveAnswer(Yap_stdout, VARS);         \
          trail_unwind(saved_TR);                           \
          cont = continuation_next(cont);                   \
        }                                                   \
        /* restore TR and HB */                             \
        TR = saved_TR;                                      \
        HB = saved_HB;                                      \
      }

#define show_consumer_subsumptive_with_answers(CONS_SG, PROD_SG)    \
      {                                                             \
        CELL* vars = (CELL * )HeapTop - 1;                          \
        CELL* saved_H = construct_subgoal_heap(SgFr_leaf(CONS_SG),  \
              &vars, SgFr_arity(CONS_SG), TRUE, FALSE);             \
                                                                    \
        if((int)*vars == 0) {                                       \
          TrStat_answers_true++;                                    \
          SHOW_TABLE_STRUCTURE("    TRUE\n");                       \
        } else {                                                    \
          if(TrStat_show == SHOW_MODE_STRUCTURE) {                  \
            show_consumer_subsumptive_answers(CONS_SG, PROD_SG,vars);   \
          }                                                         \
        }                                                           \
                                                                    \
        /* restore H register */                                    \
        H = saved_H;                                                \
                                                                    \
        if (SgFr_state(CONS_SG) < complete) {                       \
          TrStat_sg_incomplete++;                                   \
          SHOW_TABLE_STRUCTURE("    ---> INCOMPLETE\n");            \
        }                                                           \
      }

#define show_ground_with_answers(SG_FR)                             \
      {                                                             \
        CELL* vars = (CELL *)HeapTop - 1;                           \
        CELL *saved_H = construct_subgoal_heap(SgFr_leaf(SG_FR),    \
          &vars, SgFr_arity(SG_FR), FALSE, TRUE);                   \
                                                                    \
        if((int)*vars == 0) {                                       \
          TrStat_answers_true++;                                    \
          SHOW_TABLE_STRUCTURE("    TRUE\n");                       \
        } else {                                                    \
          if(TrStat_show == SHOW_MODE_STRUCTURE) {                  \
            CELL *answer_template = saved_H + SgFr_arity(SG_FR) -1; \
            show_ground_answers(SG_FR, answer_template, vars);      \
          }                                                         \
        }                                                           \
        H = saved_H;                                                \
        if(SgFr_state(SG_FR) < complete) {                          \
          TrStat_sg_incomplete++;                                   \
          SHOW_TABLE_STRUCTURE("    ---> INCOMPLETE\n");            \
        }                                                           \
      }
      
#define increment_sugoal_path(SG_FR)                                \
      { subg_node_ptr leaf = (subg_node_ptr)SgFr_leaf(SG_FR);       \
        if(TrNode_num_gen(leaf) == 1) {                             \
        } else {                                                    \
          update_generator_path((sg_node_ptr)leaf);                 \
        }                                                           \
      }
      
#define decrement_subgoal_path(SG_FR)                               \
      { subg_node_ptr leaf = (subg_node_ptr)SgFr_leaf(SG_FR);       \
        if(TrNode_num_gen(leaf) > 1) {                              \
          Yap_Error(INTERNAL_ERROR, TermNil,                        \
            "leaf node be <= 1 (decrement_subgoal_path)");          \
        } else if(TrNode_num_gen(leaf) == 1) {                      \
          decrement_generator_path((sg_node_ptr)leaf);              \
        }                                                           \
      }
      
#define count_subgoal_hash_index(HASH, COUNTER)                     \
      { subg_hash_ptr subg_hash = (subg_hash_ptr)(HASH);            \
        gen_index_ptr index = Hash_index_head(subg_hash);           \
                                                                    \
        while(index) {                                              \
          (COUNTER)++;                                              \
          index = GNIN_next(index);                                 \
        }                                                           \
      }

#define count_tst_indices(HASH, COUNTER)                            \
      { tst_ans_hash_ptr tst_hash = (tst_ans_hash_ptr)(HASH);       \
        tst_index_ptr index = TSTHT_index_head(tst_hash);           \
                                                                    \
        while(index) {                                              \
          (COUNTER)++;                                              \
          index = TSIN_next(index);                                 \
        }                                                           \
      }

#define ensure_has_proper_consumers(TAB_ENT)                        \
      if(!TabEnt_proper_consumers(TAB_ENT)) {                       \
        TabEnt_set_proper_consumers(TAB_ENT);                       \
        ground_trie_create_tsi(TAB_ENT);                            \
      }
        
static inline
void mark_subsumptive_consumer_as_completed(subcons_fr_ptr sg_fr) {
  LOCK(SgFr_lock(sg_fr));
  SgFr_state(sg_fr) = complete;
  UNLOCK(SgFr_lock(sg_fr));
}

static inline
void mark_subsumptive_producer_as_completed(subprod_fr_ptr sg_fr) {
  /* SgFr_state = completed done early */
  
  if(TabEnt_is_exec(SgFr_tab_ent(sg_fr))) {
    free_tst_hash_index((tst_ans_hash_ptr)SgFr_hash_chain(sg_fr));
    /* answer list or blocks are not removed because of show_table */
    
#ifdef TABLING_COMPLETE_TABLE
    if(SgFr_is_most_general(sg_fr)) {
      dprintf("Completing most general\n");
      TabEnt_set_completed(SgFr_tab_ent(sg_fr));
      transform_subsumptive_into_ground_trie(sg_fr);
    }
#endif /* TABLING_COMPLETE_TABLE */
  }
}

static inline
void mark_ground_consumer_as_completed(grounded_sf_ptr sg_fr) {
  LOCK(SgFr_lock(sg_fr));
  SgFr_state(sg_fr) = complete;
  decrement_subgoal_path(sg_fr);
  UNLOCK(SgFr_lock(sg_fr));
}

static inline
void mark_ground_producer_as_completed(grounded_sf_ptr sg_fr) {
  decrement_subgoal_path(sg_fr);
#ifdef TABLING_COMPLETE_TABLE
  if(SgFr_is_most_general(sg_fr)) {
    tab_ent_ptr tab_ent = SgFr_tab_ent(sg_fr);
    
    TabEnt_set_completed(tab_ent);
    if(TabEnt_is_exec(tab_ent))
      free_subgoal_trie_from_ground_table(tab_ent);
  }
#endif /* TABLING_COMPLETE_TABLE */
}

static inline void
gen_index_remove(subg_node_ptr sub_node, subg_hash_ptr hash) {
  gen_index_ptr gen_index = TrNode_index_node(sub_node);
  gen_index_ptr previous = GNIN_prev(gen_index);
  gen_index_ptr next = GNIN_next(gen_index);
  
  if(previous)
    GNIN_next(previous) = next;
  else {
    if(Hash_index_head(hash) == gen_index)
      Hash_index_head(hash) = next;
  }
  
  FREE_GEN_INDEX_NODE(gen_index);
  
  if(next)
    GNIN_prev(next) = previous;
    
  TrNode_num_gen(sub_node) = 0;
}

static inline void
gen_index_add(subg_node_ptr sub_node, subg_hash_ptr hash, int num_gen) {
  gen_index_ptr index_node;
  
  ALLOC_GEN_INDEX_NODE(index_node);
  
  GNIN_prev(index_node) = NULL;
  GNIN_node(index_node) = sub_node;
  GNIN_num_gen(index_node) = num_gen;
  
  gen_index_ptr head = Hash_index_head(hash);
  
  GNIN_next(index_node) = head;
  Hash_index_head(hash) = index_node;
  if(head)
    GNIN_prev(head) = index_node;
    
  TrNode_num_gen(sub_node) = (unsigned int)index_node;
}

static inline
void free_producer_subgoal_data(sg_fr_ptr sg_fr, int delete_all) {
  tst_node_ptr answer_trie = (tst_node_ptr)SgFr_answer_trie(sg_fr);
  if(answer_trie) {
    /* hash chain inside answer trie */
    if(SgFr_hash_chain(sg_fr))
      free_tst_hash_chain((tst_ans_hash_ptr)SgFr_hash_chain(sg_fr));
    if(TrNode_child(answer_trie))
      free_answer_trie_branch((ans_node_ptr)TrNode_child(answer_trie), TRAVERSE_POSITION_FIRST);
    if(delete_all)
      FREE_TST_ANSWER_TRIE_NODE(answer_trie);
  }
  free_answer_continuation(SgFr_first_answer(sg_fr));
}

static inline void
free_consumer_subgoal_data(subcons_fr_ptr sg_fr) {
  free_answer_continuation(SgFr_first_answer(sg_fr));
}

static inline void
free_ground_subgoal_data(grounded_sf_ptr sg_fr) {
  free_answer_continuation(SgFr_first_answer(sg_fr));
}

static inline void
free_ground_trie(tab_ent_ptr tab_ent) {
  ans_node_ptr answer_trie = (ans_node_ptr)TabEnt_ground_trie(tab_ent);
  if(answer_trie) {
    if(TrNode_child(answer_trie))
      free_answer_trie_branch(TrNode_child(answer_trie), TRAVERSE_POSITION_FIRST);
    FREE_TST_ANSWER_TRIE_NODE(answer_trie);
  }
}

static inline void
abolish_incomplete_subsumptive_consumer_subgoal(subcons_fr_ptr sg_fr) {
  free_consumer_subgoal_data(sg_fr);
  delete_subgoal_path((sg_fr_ptr)sg_fr);
  FREE_SUBCONS_SUBGOAL_FRAME(sg_fr);
}

static inline void
abolish_incomplete_ground_consumer_subgoal(grounded_sf_ptr sg_fr) {
  SgFr_state(sg_fr) = ready;
}

static inline void
abolish_incomplete_subsumptive_producer_subgoal(sg_fr_ptr sg_fr) {
  free_producer_subgoal_data(sg_fr, TRUE);
  delete_subgoal_path(sg_fr);
  FREE_SUBPROD_SUBGOAL_FRAME(sg_fr);
}

static inline void
abolish_incomplete_ground_producer_subgoal(sg_fr_ptr sg_fr) {
  SgFr_state(sg_fr) = ready;
  dprintf("Abolish incomplete\n");
}

static inline
void free_tst_hash_chain(tst_ans_hash_ptr hash) {
  while(hash) {
    tst_node_ptr chain_node, *bucket, *last_bucket;
    tst_ans_hash_ptr next_hash;
    
    bucket = TSTHT_buckets(hash);
    last_bucket = bucket + TSTHT_num_buckets(hash);
    while(!*bucket)
      bucket++;
    chain_node = *bucket;
    TSTN_child(TSTN_parent(chain_node)) = chain_node;
    while(++bucket != last_bucket) {
      if(*bucket) {
        while(TrNode_next(chain_node))
          chain_node = TSTN_next(chain_node);
        TSTN_next(chain_node) = *bucket;
        chain_node = *bucket;
      }
    }
    next_hash = TSTHT_next(hash);
    tstht_remove_index(hash);
    FREE_TST_ANSWER_TRIE_HASH(hash);
    hash = next_hash;
  }
}

static inline
void free_tst_hash_index(tst_ans_hash_ptr hash) {
  while(hash) {
    tstht_remove_index(hash);
    hash = TSTHT_next(hash);
  }
}

#define STANDARDIZE_AT_PTR(ANS_TMPLT, SIZE) ((ANS_TMPLT) + (SIZE) - 1) 

static inline int
build_next_subsumptive_consumer_return_list(subcons_fr_ptr consumer_sg) {
  subprod_fr_ptr producer_sg = SgFr_producer(consumer_sg);
  const int producer_ts = SgFr_prod_timestamp(producer_sg);
  const int consumer_ts = SgFr_timestamp(consumer_sg);
  
  if(producer_ts == consumer_ts)
    return FALSE; /* no answers were inserted */
    
  CELL *answer_template = SgFr_answer_template(consumer_sg);
  const int size = SgFr_at_size(consumer_sg);
  tst_node_ptr trie = (tst_node_ptr)SgFr_answer_trie(producer_sg);

  SgFr_timestamp(consumer_sg) = producer_ts;
  
  AT = SgFr_at_block(consumer_sg);
  
  return tst_collect_relevant_answers(trie, consumer_ts, size,
          STANDARDIZE_AT_PTR(answer_template, size), (sg_fr_ptr)consumer_sg);
}

static inline int
build_next_ground_consumer_return_list(grounded_sf_ptr consumer_sg) {
  grounded_sf_ptr producer_sg = SgFr_producer(consumer_sg);
  const int producer_ts = SgFr_timestamp(producer_sg);
  const int consumer_ts = SgFr_timestamp(consumer_sg);
  
  if(producer_ts == consumer_ts)
    return FALSE; /* no answers were inserted */
  
  CELL *answer_template = SgFr_answer_template(consumer_sg);
  const int size = SgFr_at_size(consumer_sg);
  tab_ent_ptr tab_ent = SgFr_tab_ent(producer_sg);
  tst_node_ptr trie = (tst_node_ptr)TabEnt_ground_trie(tab_ent);
  
  SgFr_timestamp(consumer_sg) = producer_ts;
  
  AT = SgFr_at_block(consumer_sg);
  
  return tst_collect_relevant_answers(trie, consumer_ts, size,
    STANDARDIZE_AT_PTR(answer_template, size), (sg_fr_ptr)consumer_sg);
}

static inline int
build_next_ground_producer_return_list(grounded_sf_ptr producer_sg) {
  tab_ent_ptr tab_ent = SgFr_tab_ent(producer_sg);
  const int producer_ts = SgFr_timestamp(producer_sg);
  const int ground_ts = TabEnt_ground_time_stamp(tab_ent);
  
  if(producer_ts == ground_ts)
    return FALSE; /* no answers */
    
  CELL *answer_template = SgFr_answer_template(producer_sg);
  const int size = SgFr_at_size(producer_sg);
  tst_node_ptr trie = (tst_node_ptr)TabEnt_ground_trie(tab_ent);
  
  SgFr_timestamp(producer_sg) = ground_ts;
  AT = SgFr_at_block(producer_sg);
  
  return tst_collect_relevant_answers(trie, producer_ts, size,
    STANDARDIZE_AT_PTR(answer_template, size),
    (sg_fr_ptr)producer_sg);
}

#undef STANDARDIZE_AT_PTR

#endif /* TABLING_CALL_SUBSUMPTION */
