/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   Flávio Cruz - University of Porto, Portugal
  File:        tab.sub_structs.h
  version:      
                                                                     
**********************************************************************/

/* THIS FILE MUST BE INCLUDED THROUGH tab.structs.h */

#ifdef TABLING_CALL_SUBSUMPTION

typedef struct sub_subgoal_trie_node {
  struct subgoal_trie_node base;
  unsigned num_generators;
} *subg_node_ptr;

#define TrNode_num_gen(X)     ((X)->num_generators)
#define TrNode_get_num_gen(X) (TrNode_num_gen(X) == 0 ? 0 : \
                                (IsHashedNode((sg_node_ptr)X) ? \
                                  GNIN_num_gen(TrNode_index_node(X)) : TrNode_num_gen(X)))
#define TrNode_index_node(X)  ((gen_index_ptr)TrNode_num_gen(X))

EXTEND_STRUCT(sub_subgoal_trie_node, subg_node_ptr, long, Int long_int);
EXTEND_STRUCT(sub_subgoal_trie_node, subg_node_ptr, float, Float float_val);

/* ----------------------- **
** generator indexes       **
** ----------------------- */

typedef struct gen_index_node *gen_index_ptr;
struct gen_index_node {
  gen_index_ptr prev;
  gen_index_ptr next;
  unsigned num_generators;
  subg_node_ptr node;
};

#define GNIN_node(X)        ((X)->node)
#define GNIN_num_gen(X)     ((X)->num_generators)
#define GNIN_prev(X)        ((X)->prev)
#define GNIN_next(X)        ((X)->next)

/* ---------------------------- **
** subgoal hash for subsumption **
** ---------------------------- */
typedef struct sub_subgoal_trie_hash {
  struct subgoal_trie_hash base;
  gen_index_ptr index_head;
} *subg_hash_ptr;

#define Hash_index_head(X)    ((X)->index_head)

/* ---------------------------- **
**       subgoal frames         **
** ---------------------------- */

typedef struct subsumed_consumer_subgoal_frame subsumptive_consumer_sf;
typedef struct subsumptive_producer_subgoal_frame {
  variant_sf var_sf;
  subsumptive_consumer_sf *consumers; /* List of properly subsumed subgoals */
#ifdef TABLING_RETROACTIVE
  int num_proper_deps;
#endif /* TABLING_RETROACTIVE */
} subsumptive_producer_sf;

typedef subsumptive_producer_sf *subprod_fr_ptr;

#define SgFr_prod_consumers(X) ((X)->consumers)
#define SgFr_prod_timestamp(X) TSTN_time_stamp((tst_node_ptr)SgFr_answer_trie(X))
#define SgFr_subsumes_subgoals(X) \
  (SgFr_is_sub_producer(X) && SgFr_prod_consumers(X) != NULL)
#define SgFr_num_proper_deps(X) ((X)->num_proper_deps)


struct subsumed_consumer_subgoal_frame {
  subgoal_frame_type flags;
  subgoal_state state_flag;
  
  yamop *code_of_subgoal;
  
#if defined(YAPOR) || defined(THREADS)
  lockvar lock;
#endif
  
  sg_node_ptr leaf_ptr;
  
  /* first consumer choice point */
  choiceptr choice_point;

  continuation_ptr first_answer;
  continuation_ptr last_answer;
  
  struct subsumed_consumer_subgoal_frame *next;
#ifdef TABLING_RETROACTIVE
  sg_fr_ptr top_gen_sg;
#endif /* TABLING_RETROACTIVE */
  
  int num_deps;
  
  time_stamp ts;
  subprod_fr_ptr producer;
  CELL* answer_template;
  int at_size;
  int at_full_size;
  
  /* Chain link for properly subsumed subgoals */
  subsumptive_consumer_sf *consumers;
};

typedef subsumptive_consumer_sf *subcons_fr_ptr;

#define SgFr_timestamp(X)       ((X)->ts)
#define SgFr_producer(X)        ((X)->producer)
#define SgFr_consumers(X)       ((X)->consumers)
#define SgFr_answer_template(X) ((X)->answer_template)
#define SgFr_at_size(X)         ((X)->at_size)
#define SgFr_at_full_size(X)    ((X)->at_full_size)
#define SgFr_num_deps(X)        ((X)->num_deps)

/* --------------------------------------------------- */

#ifdef TABLING_RETROACTIVE

/* --------------------------- **
**  retroactive subgoal frame  **
** --------------------------- */

typedef struct retroactive_subgoal_frame *retroactive_fr_ptr;

#define SUBGOAL_FRAME_TYPE_OTHER_MASK 0xF0
#define SG_FR_MOST_GENERAL 0x80
#define SG_FR_LOCAL_PRODUCER 0x10
#define SG_FR_LOCAL_CONSUMER 0x20

struct retroactive_subgoal_frame {
  subgoal_frame_type flags;
  subgoal_state state_flag;
  
  yamop *code_of_subgoal;
  
#if defined(YAPOR) || defined(THREADS)
  lockvar lock;
#endif

  sg_node_ptr leaf_ptr;
  
  choiceptr choice_point;
  
  continuation_ptr first_answer;
  continuation_ptr last_answer;
  
  struct retroactive_subgoal_frame *next;
  
  sg_fr_ptr prev;
  sg_fr_ptr top_gen_sg;
  
  continuation_ptr try_answer;

  int num_deps;
  
  CELL executing;
  CELL start;
  choiceptr saved_cp;
  choiceptr saved_max;
  
  int num_ans;
  
  time_stamp ts;
  retroactive_fr_ptr producer;
  CELL* answer_template;
  int at_size;
  int at_full_size;
  
  node_list_ptr pending_answers;
};

#define SgFr_num_ans(X)             ((X)->num_ans)
#define SgFr_pending_answers(X)     ((X)->pending_answers)

#define SgFr_start_cp(X)            ((choiceptr)SgFr_start(X))
#define SgFr_new_answer_cp(X)       ((choiceptr)SgFr_executing(X))
#define SgFr_saved_cp(X)            ((X)->saved_cp)
#define SgFr_executing(X)           ((X)->executing)
#define SgFr_start(X)               ((X)->start)
#define SgFr_started(X)             ((CELL *)SgFr_start(X) != &SgFr_start(X))
#define SgFr_got_answer(X)          ((CELL *)SgFr_executing(X) != &SgFr_executing(X))
#define SgFr_is_internal(X)         (SgFr_started(X) && !SgFr_got_answer(X))
#define SgFr_saved_max(X)           ((X)->saved_max)

#define SgFr_is_most_general(X)     (SgFr_flags(X) & SG_FR_MOST_GENERAL)
#define SgFr_set_most_general(X)    (SgFr_flags(X) |= SG_FR_MOST_GENERAL)

#define SgFr_set_local_producer(X)  (SgFr_flags(X) |= SG_FR_LOCAL_PRODUCER)
#define SgFr_is_local_producer(X)   (SgFr_flags(X) & SG_FR_LOCAL_PRODUCER)
#define SgFr_set_local_consumer(X)  (SgFr_flags(X) |= SG_FR_LOCAL_CONSUMER)
#define SgFr_is_local_consumer(X)   (SgFr_flags(X) & SG_FR_LOCAL_CONSUMER)
#define SgFr_is_producer(X)         (SgFr_flags(X) & SG_FR_PRODUCER)
#define SgFr_set_producer(X)        (SgFr_flags(X) |= SG_FR_PRODUCER)

#define SgFr_set_saved_max(X, VAL) \
      SgFr_saved_max(X) = VAL

#define SgFr_update_saved_max(X)    { \
    if(B < SgFr_saved_max(X) || B_FZ < SgFr_saved_max(X)) { \
      SgFr_set_saved_max(X, B_FZ < B ? B_FZ : B); \
    } \
}

#define TabEnt_retroactive_trie(X)        (TrNode_next(TabEnt_subgoal_trie(X)))
#define TabEnt_has_retroactive_trie(X)    (TabEnt_retroactive_trie(X) != NULL)
#define TabEnt_retroactive_time_stamp(X)  (TabEnt_has_retroactive_trie(X) ? \
                                            TSTN_time_stamp((tst_node_ptr)TabEnt_retroactive_trie(X))  \
                                            : 0)
#define TabEnt_retroactive_hash_chain(X)  ((tst_ans_hash_ptr)TrNode_next(TabEnt_retroactive_trie(X)))
                                      
/* ------------------------------------------------------ */

/* table entry flags */
#define TABLE_ENTRY_COMPLETED 0x01
#define TABLE_ENTRY_PROPER_CONSUMERS 0x02
#define TABLE_ENTRY_COMPILED 0x04

#define TabEnt_flags(X)                 (TrNode_parent(TabEnt_subgoal_trie(X)))
#define TabEnt_set_flag(X, FLAG)        (TabEnt_flags(X) = (sg_node_ptr)((unsigned int)TabEnt_flags(X) | (unsigned int)FLAG))
#define TabEnt_has_flag(X, FLAG)        ((unsigned int)TabEnt_flags(X) & (unsigned int)(FLAG))
#define TabEnt_set_proper_consumers(X)  TabEnt_set_flag(X, TABLE_ENTRY_PROPER_CONSUMERS)
#define TabEnt_proper_consumers(X)      TabEnt_has_flag(X, TABLE_ENTRY_PROPER_CONSUMERS)
#define TabEnt_compiled(X)              TabEnt_has_flag(X, TABLE_ENTRY_COMPILED)
#define TabEnt_set_compiled(X)          TabEnt_set_flag(X, TABLE_ENTRY_COMPILED)
#define TabEnt_set_completed(X)         TabEnt_set_flag(X, TABLE_ENTRY_COMPLETED)
#define TabEnt_completed(X)             (TabEnt_subgoal_trie(X) && TabEnt_has_flag(X, TABLE_ENTRY_COMPLETED))

#define TabEnt_retroactive_yes(X)    (TabEnt_retroactive_trie(X) && TrNode_is_leaf(TabEnt_retroactive_trie(X)))

#define RETROACTIVE_SUBGOAL_FRAME_MASK 0x0C

#define SgFr_is_retroactive(X)                 (SgFr_type(X) & RETROACTIVE_SUBGOAL_FRAME_MASK)
#define SgFr_is_retroactive_producer(X)        (SgFr_type(X) == RETROACTIVE_PRODUCER_SFT)
#define SgFr_is_retroactive_consumer(X)        (SgFr_type(X) == RETROACTIVE_CONSUMER_SFT)
#define SgFr_is_retroactive_local_producer(X)  (SgFr_is_retroactive_producer(X) && SgFr_is_local_producer((retroactive_fr_ptr)(X)))
#define SgFr_is_retroactive_local_consumer(X)  (SgFr_is_retroactive_consumer(X) && SgFr_is_local_consumer((retroactive_fr_ptr)(X)))

/* ------------------------------------------------------ */

typedef struct retro_leaf_index {
  int marker; /* must always be zero */
  node_list_ptr *buckets;
  int number_of_buckets;
  int number_of_sgs;
} *retro_leaf_ptr;

#define LeafIndex_is_hash(X)    (!(retro_leaf_ptr)(X)->marker)
#define LeafIndex_mark_hash(X)  ((X)->marker = 0)
#define Hash_num_sgs(X)         ((X)->number_of_sgs)

#endif /* TABLING_RETROACTIVE */

#endif /* TABLING_CALL_SUBSUMPTION */
