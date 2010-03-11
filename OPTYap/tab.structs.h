/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   R. Rocha and NCC - University of Porto, Portugal
  File:        tab.structs.h
  version:     $Id: tab.structs.h,v 1.10 2005-08-04 15:45:56 ricroc Exp $   
                                                                     
**********************************************************************/

#ifndef TAB_STRUCTS_H
#define TAB_STRUCTS_H

/* ---------------------------- **
**      Tabling mode flags      **
**  C/stdpreds.c                **
** pl/directives.yap            **
** pl/tabling.yap               **
** OPTYap/opt.preds.c           **
** ---------------------------- */

#define Mode_SchedulingOn       0x00000001L  /* yap_flags[TABLING_MODE_FLAG] */
#define Mode_CompletedOn        0x00000002L  /* yap_flags[TABLING_MODE_FLAG] */
#define Mode_ChecksOn           0x00000004L  /* yap_flags[TABLING_MODE_FLAG] */

#define Mode_Local              0x10000000L  /* yap_flags[TABLING_MODE_FLAG] + struct table_entry */
#define Mode_LoadAnswers        0x20000000L  /* yap_flags[TABLING_MODE_FLAG] + struct table_entry */
#define Mode_Subsumptive        0x40000000L  /* yap_flags[TABLING_MODE_FLAG] + struct table_entry */

#define DefaultMode_Local       0x00000001L  /* struct table_entry */
#define DefaultMode_LoadAnswers 0x00000002L  /* struct table_entry */
#define DefaultMode_Subsumptive 0x00000004L  /* struct table_entry */

#define SetMode_SchedulingOn(X)        (X) |= Mode_SchedulingOn
#define SetMode_CompletedOn(X)         (X) |= Mode_CompletedOn
#define SetMode_ChecksOn(X)            (X) |= Mode_ChecksOn
#define IsMode_SchedulingOn(X)         ((X) & Mode_SchedulingOn)
#define IsMode_SchedulingOff(X)        (!IsMode_SchedulingOn(X))
#define IsMode_CompletedOn(X)          ((X) & Mode_CompletedOn)
#define IsMode_CompletedOff(X)         (!IsMode_CompletedOn(X))
#define IsMode_ChecksOn(X)             ((X) & Mode_ChecksOn)
#define IsMode_ChecksOff(X)            (!IsMode_ChecksOn(X))

#define SetMode_Local(X)               (X) |= Mode_Local
#define SetMode_Batched(X)             (X) &= ~Mode_Local
#define SetMode_LoadAnswers(X)         (X) |= Mode_LoadAnswers
#define SetMode_ExecAnswers(X)         (X) &= ~Mode_LoadAnswers
#define SetMode_Subsumptive(X)         (X) |= Mode_Subsumptive
#define SetMode_Variant(X)             (X) &= ~Mode_Subsumptive
#define IsMode_Local(X)                ((X) & Mode_Local)
#define IsMode_Batched(X)              (!IsMode_Local(X))
#define IsMode_LoadAnswers(X)          ((X) & Mode_LoadAnswers)
#define IsMode_ExecAnswers(X)          (!IsMode_LoadAnswers(X))
#define IsMode_Subsumptive(X)          ((X) & Mode_Subsumptive)
#define IsMode_Variant(X)              (!IsMode_Subsumptive(X))

#define SetDefaultMode_Local(X)        (X) |= DefaultMode_Local
#define SetDefaultMode_Batched(X)      (X) &= ~DefaultMode_Local
#define SetDefaultMode_LoadAnswers(X)  (X) |= DefaultMode_LoadAnswers
#define SetDefaultMode_ExecAnswers(X)  (X) &= ~DefaultMode_LoadAnswers
#define SetDefaultMode_Subsumptive(X)  (X) |= DefaultMode_Subsumptive
#define SetDefaultMode_Variant(X)      (X) &= ~DefaultMode_Subsumptive
#define IsDefaultMode_Local(X)         ((X) & DefaultMode_Local)
#define IsDefaultMode_Batched(X)       (!IsDefaultMode_Local(X))
#define IsDefaultMode_LoadAnswers(X)   ((X) & DefaultMode_LoadAnswers)
#define IsDefaultMode_ExecAnswers(X)   (!IsDefaultMode_LoadAnswers(X))
#define IsDefaultMode_Subsumptive(X)   ((X) & DefaultMode_Subsumptive)
#define IsDefaultMode_Variant(X)       (!IsDefaultMode_Subsumptive(X))


/* ---------------------------- **
**      Struct table_entry      **
** ---------------------------- */

typedef struct table_entry {
#if defined(YAPOR) || defined(THREADS)
  lockvar lock;
#endif /* YAPOR */
  struct pred_entry *pred_entry;
  Atom pred_atom;
  int pred_arity;
  int mode_flags;
  struct subgoal_trie_node *subgoal_trie;
  struct subgoal_trie_hash *hash_chain;
  struct table_entry *next;
} *tab_ent_ptr;

#define TabEnt_lock(X)            ((X)->lock)
#define TabEnt_pe(X)              ((X)->pred_entry)
#define TabEnt_atom(X)            ((X)->pred_atom)
#define TabEnt_arity(X)           ((X)->pred_arity)
#define TabEnt_mode(X)            ((X)->mode_flags)
#define TabEnt_subgoal_trie(X)    ((X)->subgoal_trie)
#define TabEnt_hash_chain(X)      ((X)->hash_chain)
#define TabEnt_next(X)            ((X)->next)

#define TabEnt_is_variant(X)      (IsMode_Variant(TabEnt_mode(X)))
#define TabEnt_is_subsumptive(X)  (IsMode_Subsumptive(TabEnt_mode(X)))
#define TabEnt_is_load(X)         (IsMode_LoadAnswers(TabEnt_mode(X)))
#define TabEnt_is_exec(X)         (IsMode_ExecAnswers(TabEnt_mode(X)))
#define TabEnt_is_empty(X)        (TrNode_child(TabEnt_subgoal_trie(X)) == NULL)
#define TabEnt_set_variant(X)     { if(TabEnt_is_empty(X)) SetMode_Variant(TabEnt_mode(X)); }
#define TabEnt_set_subsumptive(X) { if(TabEnt_is_empty(X)) SetMode_Subsumptive(TabEnt_mode(X)); }
#define TabEnt_set_load(X)        { if(TabEnt_is_empty(X) || TabEnt_is_variant(X)) SetMode_LoadAnswers(TabEnt_mode(X)); }
#define TabEnt_set_exec(X)        { if(TabEnt_is_empty(X) || TabEnt_is_variant(X)) SetMode_ExecAnswers(TabEnt_mode(X)); }


/* ------------------------- **
**    Trie definitions       **
** ------------------------- */

enum Types_of_Tries {
 CALL_TRIE_TT              = 0x01,     /* binary:  0001 */
 BASIC_ANSWER_TRIE_TT      = 0x02,     /* binary:  0010 */
 TS_ANSWER_TRIE_TT         = 0x04,     /* binary:  0100 */
};

enum Types_of_Trie_Nodes {
  TRIE_ROOT_NT = 0x08,
  HASH_HEADER_NT = 0x04,
  LEAF_NT = 0x02,
  HASHED_LEAF_NT = 0x03,
  INTERIOR_NT = 0x00,
  HASHED_INTERIOR_NT = 0x01,
  LONG_INT_NT = 0x10,
  FLOAT_NT = 0x20
};

#define IS_LONG_INT_FLAG(FLAG) ((FLAG) & LONG_INT_NT)
#define IS_FLOAT_FLAG(FLAG) ((FLAG) & FLOAT_NT)

typedef unsigned long time_stamp;

struct basic_trie_info {
  OPCODE instr;
  unsigned char trie_type;
  unsigned char node_type;
};

/* -------------------------------------------------------------------------- **
**      Structs global_trie_node, subgoal_trie_node and answer_trie_node      **
** -------------------------------------------------------------------------- */

#ifdef GLOBAL_TRIE
typedef struct global_trie_node {
  Term entry;
  struct global_trie_node *parent;
  struct global_trie_node *child;
  struct global_trie_node *next;
} *gt_node_ptr;
#endif /* GLOBAL_TRIE */

typedef struct subgoal_trie_node {
  struct basic_trie_info basic_info;
#ifdef TABLE_LOCK_AT_NODE_LEVEL
  lockvar lock;
#endif /* TABLE_LOCK_AT_NODE_LEVEL */

  struct subgoal_trie_node *parent;
  struct subgoal_trie_node *child;
  struct subgoal_trie_node *next;
    
#ifdef GLOBAL_TRIE
  struct global_trie_node *entry;
#else
  Term entry;
#endif /* GLOBAL_TRIE */

} *sg_node_ptr;

typedef struct long_subgoal_trie_node {
  struct subgoal_trie_node base;
  Int long_int;
} *long_sg_node_ptr;

typedef struct float_subgoal_trie_node {
  struct subgoal_trie_node base;
  Float float_val;
} *float_sg_node_ptr;

typedef struct answer_trie_node {
  struct basic_trie_info basic_info;
  
#ifdef TABLE_LOCK_AT_NODE_LEVEL
  lockvar lock;
#endif /* TABLE_LOCK_AT_NODE_LEVEL */
  
  struct answer_trie_node *parent;
  struct answer_trie_node *child;
  struct answer_trie_node *next;
    
#ifdef GLOBAL_TRIE
  struct global_trie_node *entry;
#else
  Term entry;
#endif /* GLOBAL_TRIE */
  
#ifdef YAPOR
  int or_arg;               /* u.Otapl.or_arg */
#endif /* YAPOR */
} *ans_node_ptr;

#define TrNode_instr(X)        ((X)->basic_info.instr)
#define TrNode_node_type(X)    ((X)->basic_info.node_type)
#define TrNode_trie_type(X)    ((X)->basic_info.trie_type)
#define TrNode_or_arg(X)       ((X)->or_arg)
#define TrNode_entry(X)        ((X)->entry)
#define TrNode_lock(X)         ((X)->lock)
#define TrNode_parent(X)       ((X)->parent)
#define TrNode_child(X)        ((X)->child)
#define TrNode_sg_fr(X)        ((X)->child)
#define TrNode_next(X)         ((X)->next)
#define TrNode_is_hash(X)      (TrNode_node_type(X) & HASH_HEADER_NT)
#define TrNode_is_hashed(X)    (TrNode_node_type(X) & HASHED_INTERIOR_NT)
#define TrNode_is_long(X)      (IS_LONG_INT_FLAG(TrNode_node_type(X)))
#define TrNode_is_float(X)     (IS_FLOAT_FLAG(TrNode_node_type(X)))
#define TrNode_long_int(X)     ((X)->long_int)
#define TrNode_float(X)        ((X)->float_val)
#define TrNode_is_call(X)      (TrNode_trie_type(X) & CALL_TRIE_TT)
#define TrNode_is_root(X)      (TrNode_node_type(X) & TRIE_ROOT_NT)

/* -------------------------------------------------------------------------- **
**      Structs global_trie_hash, subgoal_trie_hash and answer_trie_hash      **
** -------------------------------------------------------------------------- */

#ifdef GLOBAL_TRIE
typedef struct global_trie_hash {
  /* the first field is used for compatibility **
  ** with the global_trie_node data structure */
  Term mark;
  int number_of_buckets;
  struct global_trie_node **buckets;
  int number_of_nodes;
} *gt_hash_ptr;
#endif /* GLOBAL_TRIE */

typedef struct subgoal_trie_hash {
  /* the first field is used for compatibility **
  ** with the subgoal_trie_node data structure */
  struct basic_trie_info basic_info;
  int number_of_buckets;
  struct subgoal_trie_node **buckets;
  int number_of_nodes;
  struct subgoal_trie_hash *next;
} *sg_hash_ptr;

typedef struct answer_trie_hash {
  /* the first field is used for compatibility **
  ** with the answer_trie_node data structure  */
  struct basic_trie_info basic_info;
  int number_of_buckets;
  struct answer_trie_node **buckets;
  int number_of_nodes;
  struct answer_trie_hash *next;
} *ans_hash_ptr;

#define Hash_num_buckets(X)     ((X)->number_of_buckets)
#define Hash_seed(X)            ((X)->number_of_buckets - 1)
#define Hash_buckets(X)         ((X)->buckets)
#define Hash_bucket(X,N)        ((X)->buckets + N)
#define Hash_num_nodes(X)       ((X)->number_of_nodes)
#define Hash_next(X)            ((X)->next)

/* ------------------------------ **
**      Struct answer_list        **
** ------------------------------ */

#ifdef TABLING_ANSWER_LIST
typedef struct answer_list {
  struct answer_trie_node *answer;
  struct answer_list *next;
} *ans_list_ptr;

#define AnsList_answer(X)       ((X)->answer)
#define AnsList_next(X)         ((X)->next)

typedef ans_list_ptr continuation_ptr;
#define ContPtr_next(X)   AnsList_next(X)
#define ContPtr_answer(X) AnsList_answer(X)

#else

typedef ans_node_ptr continuation_ptr;
#define ContPtr_next(X)   TrNode_child(X)
#define ContPtr_answer(X) (X)

#endif /* TABLING_ANSWER_LIST */

/* ------------------------------- **
**     Subgoal frames data         **
** ------------------------------- */
typedef unsigned char subgoal_frame_type;

enum SubgoalFrameType {
  VARIANT_PRODUCER_SFT        = 0x01,
  SUBSUMPTIVE_PRODUCER_SFT    = 0x02,
  SUBSUMED_CONSUMER_SFT       = 0x03
};

/* ------------------------------ **
**      Struct subgoal_frame      **
** ------------------------------ */

typedef struct subgoal_frame {
  subgoal_frame_type type; /* subgoal frame type */
#if defined(YAPOR) || defined(THREADS)
  lockvar lock;
#endif
#ifdef YAPOR
  int generator_worker;
  struct or_frame *top_or_frame_on_generator_branch;
#endif /* YAPOR */
  yamop *code_of_subgoal;
  enum {
    incomplete      = 0,  /* INCOMPLETE_TABLING */
    ready           = 1,
    evaluating      = 2,
    complete        = 3,
    complete_in_use = 4,  /* LIMIT_TABLING */
    compiled        = 5,
    compiled_in_use = 6   /* LIMIT_TABLING */
  } state_flag;  /* do not change order !!! */
  
  choiceptr generator_choice_point;
  
  sg_node_ptr leaf_ptr;
  
  struct answer_trie_node *answer_trie;
  
  continuation_ptr first_answer;
  continuation_ptr last_answer;

#ifdef INCOMPLETE_TABLING
  continuation_ptr try_answer;
#endif /* INCOMPLETE_TABLING */

#ifdef LIMIT_TABLING
  struct subgoal_frame *previous;
#endif /* LIMIT_TABLING */
  struct subgoal_frame *next;
} variant_sf;

#define variant_subgoal_frame subgoal_frame
typedef variant_sf *sg_fr_ptr;
typedef sg_fr_ptr variant_sf_ptr;

#define CAST_SF(X)             ((variant_sf_ptr)(X))

#define SgFr_type(X)           (CAST_SF(X)->type)
#define SgFr_lock(X)           (CAST_SF(X)->lock)
#define SgFr_gen_worker(X)     (CAST_SF(X)->generator_worker)
#define SgFr_gen_top_or_fr(X)  (CAST_SF(X)->top_or_frame_on_generator_branch)
#define SgFr_code(X)           (CAST_SF(X)->code_of_subgoal)
#define SgFr_tab_ent(X)        ((CAST_SF(X)->code_of_subgoal)->u.Otapl.te)
#define SgFr_arity(X)          ((CAST_SF(X)->code_of_subgoal)->u.Otapl.s)
#define SgFr_state(X)          (CAST_SF(X)->state_flag)
#define SgFr_gen_cp(X)         (CAST_SF(X)->generator_choice_point)
#define SgFr_leaf(X)           (CAST_SF(X)->leaf_ptr)
#define SgFr_hash_chain(X)     (TrNode_next(CAST_SF(X)->answer_trie)) /* sibling of root answer trie node */
#define SgFr_answer_trie(X)    (CAST_SF(X)->answer_trie)
#define SgFr_first_answer(X)   (CAST_SF(X)->first_answer)
#define SgFr_last_answer(X)    (CAST_SF(X)->last_answer)
#define SgFr_try_answer(X)     (CAST_SF(X)->try_answer)
#define SgFr_previous(X)       (CAST_SF(X)->previous)
#define SgFr_next(X)           (CAST_SF(X)->next)

/* ------------------------------------------------------------------------------------------- **
   SgFr_lock:          spin-lock to modify the frame fields.
   SgFr_gen_worker:    the id of the worker that had allocated the frame.
   SgFr_gen_top_or_fr: a pointer to the top or-frame in the generator choice point branch. 
                       When the generator choice point is shared the pointer is updated 
                       to its or-frame. It is used to find the direct dependency node for 
                       consumer nodes in other workers branches.
   SgFr_code           initial instruction of the subgoal's compiled code.
   SgFr_tab_ent        a pointer to the correspondent table entry.
   SgFr_arity          the arity of the subgoal.
   SgFr_state:         a flag that indicates the subgoal state.
   SgFr_gen_cp:        a pointer to the correspondent generator choice point.
   SgFr_hash_chain:    a pointer to the first answer_trie_hash struct for the subgoal in hand.
   SgFr_leaf:          a pointer to the leaf in the call trie.
   SgFr_answer_trie:   a pointer to the top answer trie node.
                       It is used to check for/insert new answers.
   SgFr_first_answer:  a pointer to the bottom answer trie node of the first available answer.
   SgFr_last_answer:   a pointer to the bottom answer trie node of the last available answer.
   SgFr_try_answer:    a pointer to the bottom answer trie node of the last tried answer.
                       It is used when a subgoal was not completed during the previous evaluation.
                       Not completed subgoals start by trying the answers already found.
   SgFr_previous:      a pointer to the previous subgoal frame on the chain.
   SgFr_next:          a pointer to the next subgoal frame on the chain.
** ------------------------------------------------------------------------------------------- */

typedef struct subsumed_consumer_subgoal_frame subsumptive_consumer_sf;
typedef struct subsumptive_producer_subgoal_frame {
  variant_sf var_sf;
  subsumptive_consumer_sf *consumers; /* List of properly subsumed subgoals */
} subsumptive_producer_sf;

typedef subsumptive_producer_sf *subprod_fr_ptr;

#define CAST_SUBPRODSF(X)   ((subprod_fr_ptr)(X))

#define SgFr_prod_consumers(X) ((X)->consumers)
#define SgFr_prod_timestamp(X) TSTN_time_stamp((tst_node_ptr)SgFr_answer_trie(X))

struct subsumed_consumer_subgoal_frame {
  variant_sf var_sf;
  time_stamp ts;
  subprod_fr_ptr producer;
  choiceptr cons_cp;
  subsumptive_consumer_sf *consumers; /* Chain link for properly subsumed subgoals */
};

typedef subsumptive_consumer_sf *subcons_fr_ptr;

#define CAST_SUBCONSSF(X)   ((subcons_fr_ptr)(X))

#define SgFr_timestamp(X)       ((X)->ts)
#define SgFr_producer(X)        ((X)->producer)
#define SgFr_consumers(X)       ((X)->consumers)
#define SgFr_cons_cp(X)         ((X)->cons_cp)
#define SgFr_answer_template(X) (SgFr_cons_cp(X)->cp_h - 1)

/* ------------------------------- **
** Subgoal frame types             **
** ------------------------------- */

#define SgFr_is_variant(X)  \
  (SgFr_type(X) == VARIANT_PRODUCER_SFT)
#define SgFr_is_sub_producer(X)  \
  (SgFr_type(X) == SUBSUMPTIVE_PRODUCER_SFT)
#define SgFr_is_sub_consumer(X)   \
  (SgFr_type(X) == SUBSUMED_CONSUMER_SFT)
  
#define SgFr_subsumes_subgoals(X) \
  (SgFr_is_sub_producer(X) && SgFr_prod_consumers(X) != NULL)

/* --------------------------------- **
**      Struct dependency_frame      **
** --------------------------------- */

typedef struct dependency_frame {
#if defined(YAPOR) || defined(THREADS)
  lockvar lock;
#endif
#ifdef YAPOR
  int leader_dependency_is_on_stack;
  struct or_frame *top_or_frame;
#ifdef TIMESTAMP_CHECK
  long timestamp;
#endif /* TIMESTAMP_CHECK */
#endif /* YAPOR */
  choiceptr backchain_choice_point;
  choiceptr leader_choice_point;
  choiceptr consumer_choice_point;
  continuation_ptr last_consumed_answer;
  sg_fr_ptr sg_fr; /* NEW */
  struct dependency_frame *next;
} *dep_fr_ptr;

#define DepFr_lock(X)                    ((X)->lock)
#define DepFr_leader_dep_is_on_stack(X)  ((X)->leader_dependency_is_on_stack)
#define DepFr_top_or_fr(X)               ((X)->top_or_frame)
#define DepFr_timestamp(X)               ((X)->timestamp)
#define DepFr_backchain_cp(X)            ((X)->backchain_choice_point)
#define DepFr_leader_cp(X)               ((X)->leader_choice_point)
#define DepFr_cons_cp(X)                 ((X)->consumer_choice_point)
#define DepFr_last_answer(X)             ((X)->last_consumed_answer)
#define DepFr_sg_fr(X)                   ((X)->sg_fr)
#define DepFr_next(X)                    ((X)->next)
#define DepFr_H(X)                       (DepFr_cons_cp(X)->cp_h)

/* ---------------------------------------------------------------------------------------------------- **
   DepFr_lock:                   lock variable to modify the frame fields.
   DepFr_leader_dep_is_on_stack: the generator choice point for the correspondent consumer choice point 
                                 is on the worker's stack (FALSE/TRUE).
   DepFr_top_or_fr:              a pointer to the top or-frame in the consumer choice point branch. 
                                 When the consumer choice point is shared the pointer is updated to 
                                 its or-frame. It is used to update the LOCAL_top_or_fr when a worker 
                                 backtracks through answers.
   DepFr_timestamp:              a timestamp used to optimize the search for suspension frames to be 
                                 resumed.
   DepFr_backchain_cp:           a pointer to the nearest choice point with untried alternatives.
                                 It is used to efficiently return (backtrack) to the leader node where 
                                 we perform the last backtracking through answers operation.
   DepFr_leader_cp:              a pointer to the leader choice point.
   DepFr_cons_cp:                a pointer to the correspondent consumer choice point.
   DepFr_last_answer:            a pointer to the last consumed answer.
   DepFr_next:                   a pointer to the next dependency frame on the chain.  
** ---------------------------------------------------------------------------------------------------- */



/* --------------------------------- **
**      Struct suspension_frame      **
** --------------------------------- */

#ifdef YAPOR
typedef struct suspension_frame {
  struct or_frame *top_or_frame_on_stack;
  struct dependency_frame *top_dependency_frame;
  struct subgoal_frame *top_subgoal_frame;
  struct suspended_block {
    void *resume_register;
    void *block_start;
    long block_size;
  } global_block, local_block, trail_block;
  struct suspension_frame *next;
} *susp_fr_ptr;
#endif /* YAPOR */

#define SuspFr_top_or_fr_on_stack(X)  ((X)->top_or_frame_on_stack)
#define SuspFr_top_dep_fr(X)          ((X)->top_dependency_frame)
#define SuspFr_top_sg_fr(X)           ((X)->top_subgoal_frame)
#define SuspFr_global_reg(X)          ((X)->global_block.resume_register)
#define SuspFr_global_start(X)        ((X)->global_block.block_start)
#define SuspFr_global_size(X)         ((X)->global_block.block_size)
#define SuspFr_local_reg(X)           ((X)->local_block.resume_register)
#define SuspFr_local_start(X)         ((X)->local_block.block_start)
#define SuspFr_local_size(X)          ((X)->local_block.block_size)
#define SuspFr_trail_reg(X)           ((X)->trail_block.resume_register)
#define SuspFr_trail_start(X)         ((X)->trail_block.block_start)
#define SuspFr_trail_size(X)          ((X)->trail_block.block_size)
#define SuspFr_next(X)                ((X)->next)


/* -------------------------------- **
** Structure to pass information    **
** about call                       **
** -------------------------------- */
typedef struct Tabled_Call_Info_Record {
  yamop *code;
  CELL *var_vector; /* location to store the call var vector */
} TabledCallInfo;

#define CallInfo_table_entry(CALL)	((CALL)->code->u.Otapl.te)
#define CallInfo_arity(CALL)        ((CALL)->code->u.Otapl.s)
#define CallInfo_var_vector(CALL)   ((CALL)->var_vector)
#define CallInfo_code(CALL)         ((CALL)->code)
#define CallInfo_arguments(CALL)    (XREGS + 1)

typedef struct Call_Check_Insert_Results {
  CELL *var_vector;         /* pointer to the vector of call variables */
  sg_fr_ptr subgoal_frame;
  sg_fr_ptr subsumer;
  int variant_found;
  sg_node_ptr leaf;
} CallLookupResults;


#define CallResults_var_vector(X)     ((X)->var_vector)
#define CallResults_subsumer(X)       ((X)->subsumer)
#define CallResults_subgoal_frame(X)  ((X)->subgoal_frame)
#define CallResults_variant_found(X)  ((X)->variant_found)
#define CallResults_leaf(X)           ((X)->leaf)


/* ------------------------------- **
**      Structs choice points      **
** ------------------------------- */

struct generator_choicept {
  struct choicept cp;
  struct dependency_frame *cp_dep_fr;  /* always NULL if batched scheduling */
  struct subgoal_frame *cp_sg_fr;
#ifdef LOW_LEVEL_TRACER
  struct pred_entry *cp_pred_entry;
#endif /* LOW_LEVEL_TRACER */
};

#ifdef DETERMINISTIC_TABLING
struct deterministic_generator_choicept {
  struct deterministic_choicept cp;
  struct subgoal_frame *cp_sg_fr;
#ifdef LOW_LEVEL_TRACER
  struct pred_entry *cp_pred_entry;
#endif /* LOW_LEVEL_TRACER */
};
#endif /* DETERMINISTIC_TABLING */

struct consumer_choicept {
  struct choicept cp;
  struct dependency_frame *cp_dep_fr;
#ifdef LOW_LEVEL_TRACER
  struct pred_entry *cp_pred_entry;
#endif /* LOW_LEVEL_TRACER */
};

struct loader_choicept {
  struct choicept cp;
  continuation_ptr cp_last_answer;
  sg_fr_ptr sg_fr;
#ifdef LOW_LEVEL_TRACER
  struct pred_entry *cp_pred_entry;
#endif /* LOW_LEVEL_TRACER */
};

#endif
