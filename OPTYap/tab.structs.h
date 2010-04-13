/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   R. Rocha and NCC - University of Porto, Portugal
  File:        tab.structs.h
  version:     $Id: tab.structs.h,v 1.10 2005-08-04 15:45:56 ricroc Exp $   
                                                                     
**********************************************************************/

#ifndef TAB_STRUCTS_H
#define TAB_STRUCTS_H

#define Mode_SchedulingOn       0x00000001L  /* yap_flags[TABLING_MODE_FLAG] */
#define Mode_CompletedOn        0x00000002L  /* yap_flags[TABLING_MODE_FLAG] */
#define Mode_ChecksOn           0x00000004L  /* yap_flags[TABLING_MODE_FLAG] */

#define Mode_Local              0x10000000L  /* yap_flags[TABLING_MODE_FLAG] + struct table_entry */
#define Mode_LoadAnswers        0x20000000L  /* yap_flags[TABLING_MODE_FLAG] + struct table_entry */
#ifdef TABLING_CALL_SUBSUMPTION
#define Mode_Subsumptive        0x40000000L  /* yap_flags[TABLING_MODE_FLAG] + struct table_entry */
#define Mode_Grounded           0x80000000L  /* yap_flags[TABLING_MODE_FLAG] + struct table_entry */
#endif /* TABLING_CALL_SUBSUMPTION */

#define DefaultMode_Local       0x00000001L  /* struct table_entry */
#define DefaultMode_LoadAnswers 0x00000002L  /* struct table_entry */
#ifdef TABLING_CALL_SUBSUMPTION
#define DefaultMode_Subsumptive 0x00000004L  /* struct table_entry */
#define DefaultMode_Grounded    0x00000008L  /* struct table_entry */
#endif /* TABLING_CALL_SUBSUMPTION */

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
#ifdef TABLING_CALL_SUBSUMPTION
#define SetMode_Subsumptive(X)         (X) = ((X) | Mode_Subsumptive) & ~Mode_Grounded
#define SetMode_Variant(X)             (X) &= ~(Mode_Subsumptive | Mode_Grounded)
#define SetMode_Grounded(X)            (X) = ((X) | Mode_Grounded) & ~Mode_Subsumptive
#endif /* TABLING_CALL_SUBSUMPTION */
#define IsMode_Local(X)                ((X) & Mode_Local)
#define IsMode_Batched(X)              (!IsMode_Local(X))
#define IsMode_LoadAnswers(X)          ((X) & Mode_LoadAnswers)
#define IsMode_ExecAnswers(X)          (!IsMode_LoadAnswers(X))
#ifdef TABLING_CALL_SUBSUMPTION
#define IsMode_Variant(X)              (!IsMode_Subsumptive(X) && !IsMode_Grounded(X))
#define IsMode_Subsumptive(X)          ((X) & Mode_Subsumptive)
#define IsMode_Grounded(X)             ((X) & Mode_Grounded)
#endif /* TABLING_CALL_SUBSUMPTION */

#define SetDefaultMode_Local(X)        (X) |= DefaultMode_Local
#define SetDefaultMode_Batched(X)      (X) &= ~DefaultMode_Local
#define SetDefaultMode_LoadAnswers(X)  (X) |= DefaultMode_LoadAnswers
#define SetDefaultMode_ExecAnswers(X)  (X) &= ~DefaultMode_LoadAnswers
#ifdef TABLING_CALL_SUBSUMPTION
#define SetDefaultMode_Subsumptive(X)  (X) = ((X) | DefaultMode_Subsumptive) & ~DefaultMode_Grounded
#define SetDefaultMode_Variant(X)      (X) &= ~(DefaultMode_Subsumptive | DefaultMode_Grounded)
#define SetDefaultMode_Grounded(X)     (X) = ((X) | DefaultMode_Grounded) & ~DefaultMode_Subsumptive
#endif /* TABLING_CALL_SUBSUMPTION */
#define IsDefaultMode_Local(X)         ((X) & DefaultMode_Local)
#define IsDefaultMode_Batched(X)       (!IsDefaultMode_Local(X))
#define IsDefaultMode_LoadAnswers(X)   ((X) & DefaultMode_LoadAnswers)
#define IsDefaultMode_ExecAnswers(X)   (!IsDefaultMode_LoadAnswers(X))
#ifdef TABLING_CALL_SUBSUMPTION
#define IsDefaultMode_Subsumptive(X)   ((X) & DefaultMode_Subsumptive)
#define IsDefaultMode_Variant(X)       (!IsDefaultMode_Subsumptive(X) && !IsDefaultMode_Grounded(X))
#define IsDefaultMode_Grounded(X)      ((X) & DefaultMode_Grounded)
#endif /* TABLING_CALL_SUBSUMPTION */

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

#define TabEnt_is_load(X)         (IsMode_LoadAnswers(TabEnt_mode(X)))
#define TabEnt_is_exec(X)         (IsMode_ExecAnswers(TabEnt_mode(X)))
#define TabEnt_is_local(X)        (IsMode_Local(TabEnt_mode(X)))
#define TabEnt_is_empty(X)        (TabEnt_subgoal_trie(X) == NULL)
#ifdef TABLING_CALL_SUBSUMPTION
#define TabEnt_is_variant(X)      (IsMode_Variant(TabEnt_mode(X)))
#define TabEnt_is_subsumptive(X)  (IsMode_Subsumptive(TabEnt_mode(X)))
#define TabEnt_is_grounded(X)     (IsMode_Grounded(TabEnt_mode(X)))
#define TabEnt_set_variant(X)     { if(TabEnt_is_empty(X)) SetMode_Variant(TabEnt_mode(X)); }
#define TabEnt_set_subsumptive(X) { if(TabEnt_is_empty(X)) SetMode_Subsumptive(TabEnt_mode(X)); }
#define TabEnt_set_grounded(X)    { if(TabEnt_is_empty(X)) SetMode_Grounded(TabEnt_mode(X)); }
#define TabEnt_set_load(X)        { if(TabEnt_is_empty(X) || TabEnt_is_variant(X)) SetMode_LoadAnswers(TabEnt_mode(X)); }
#define TabEnt_set_exec(X)        { if(TabEnt_is_empty(X) || TabEnt_is_variant(X)) SetMode_ExecAnswers(TabEnt_mode(X)); }
#else
#define TabEnt_is_variant(X)      TRUE
#define TabEnt_set_load(X)        { SetMode_LoadAnswers(TabEnt_mode(X)); }
#define TabEnt_set_exec(X)        { SetMode_ExecAnswers(TabEnt_mode(X)); }
#endif /* TABLING_CALL_SUBSUMPTION */

/* ------------------------- **
**    Trie definitions       **
** ------------------------- */

enum Trie_Node_Flags {
  TRIE_ROOT_NT = 0x08,
  HASH_HEADER_NT = 0x04,
  LEAF_NT = 0x02,
  HASHED_LEAF_NT = 0x03,
  INTERIOR_NT = 0x00,
  HASHED_INTERIOR_NT = 0x01,
  LONG_INT_NT = 0x10,
  FLOAT_NT = 0x20,
  CALL_TRIE_NT = 0x00,
  ANSWER_TRIE_NT = 0x40,
  TST_TRIE_NT = 0x80,
  CALL_SUB_TRIE_NT = 0xC0
};

#define HASHED_NODE_MASK        0x01
#define LEAF_NODE_MASK          0x02
#define TRIE_TYPE_MASK          0xC0
#define IS_LONG_INT_FLAG(FLAG)  ((FLAG) & LONG_INT_NT)
#define IS_FLOAT_FLAG(FLAG)     ((FLAG) & FLOAT_NT)
#define IS_SUB_FLAG(FLAG)       ((FLAG) & CALL_SUB_TRIE_NT)

typedef unsigned long time_stamp;

struct basic_trie_info {
  OPCODE instr;
  OPCODE compiled;
  unsigned char node_type;
} __attribute__((__packed__));

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

/* Subgoal trie nodes */
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

#define EXTEND_STRUCT(BASE_STRUCT, BASE_POINTER, NAME, NEW_FIELD) \
  typedef struct NAME##_##BASE_STRUCT { \
    struct BASE_STRUCT base;  \
    NEW_FIELD;  \
  } * NAME##_##BASE_POINTER

EXTEND_STRUCT(subgoal_trie_node, sg_node_ptr, long, Int long_int);
EXTEND_STRUCT(subgoal_trie_node, sg_node_ptr, float, Float float_val);

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
#define TrNode_compiled(X)     ((X)->basic_info.compiled)
#define TrNode_node_type(X)    ((X)->basic_info.node_type)
#define TrNode_trie_type(X)    (TrNode_node_type(X) & TRIE_TYPE_MASK)
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
#define TrNode_is_var_call(X)  (TrNode_trie_type(X) == CALL_TRIE_NT)
#define TrNode_is_sub_call(X)  (TrNode_trie_type(X) == CALL_SUB_TRIE_NT)
#define TrNode_is_tst(X)       (TrNode_trie_type(X) == TST_TRIE_NT)
#define TrNode_is_answer(X)    (TrNode_trie_type(X) == ANSWER_TRIE_NT)
#define TrNode_is_root(X)      (TrNode_node_type(X) & TRIE_ROOT_NT)
#define TrNode_is_leaf(X)      (TrNode_node_type(X) & LEAF_NT)

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

/* ---------------------------------- */

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

typedef struct node_list {
  struct answer_trie_node *node;
  struct node_list *next;
} *node_list_ptr;

#define NodeList_node(X)       ((X)->node)
#define NodeList_next(X)       ((X)->next)

#ifdef TABLING_ANSWER_LIST

typedef node_list_ptr continuation_ptr;

#elif defined(TABLING_ANSWER_CHILD)

typedef ans_node_ptr continuation_ptr;

#elif defined(TABLING_ANSWER_BLOCKS)

typedef ans_node_ptr* continuation_ptr;

#endif /* TABLING_ANSWER_LIST */

/* ------------------------------- **
**     Subgoal frames data         **
** ------------------------------- */

enum SubgoalFrameType {
  VARIANT_PRODUCER_SFT        = 0x01, /* 0001 */
  SUBSUMPTIVE_PRODUCER_SFT    = 0x03, /* 0011 */
  SUBSUMED_CONSUMER_SFT       = 0x02, /* 0010 */
  GROUND_PRODUCER_SFT         = 0x05, /* 0101 */
  GROUND_CONSUMER_SFT         = 0x08  /* 1000 */
};

#define SUBGOAL_FRAME_TYPE_MASK 0x0F

typedef unsigned char subgoal_frame_type;

enum SubgoalState {
  incomplete      = 0,  /* INCOMPLETE_TABLING */
  ready           = 1,
  evaluating      = 2,
  complete        = 3,
  complete_in_use = 4,  /* LIMIT_TABLING */
  compiled        = 5,
  compiled_in_use = 6   /* LIMIT_TABLING */
}; /* do not change order !!! */

typedef unsigned char subgoal_state;

/* ------------------------------ **
**      Struct subgoal_frame      **
** ------------------------------ */

typedef struct subgoal_frame {
  subgoal_frame_type flags; /* subgoal type + other things */
  subgoal_state state_flag;
  
  yamop *code_of_subgoal;
  
#if defined(YAPOR) || defined(THREADS)
  lockvar lock;
#endif
  
  sg_node_ptr leaf_ptr;
  
  choiceptr choice_point; /* generator choice point */
  
  continuation_ptr first_answer;
  continuation_ptr last_answer;
    
  struct subgoal_frame *next;
#ifdef LIMIT_TABLING
  struct subgoal_frame *previous;
#endif /* LIMIT_TABLING */

#ifdef TABLING_CALL_SUBSUMPTION
  struct subgoal_frame *top_gen_sg;
  
  CELL executing;
  CELL start;
  choiceptr saved_cp;
#endif /* TABLING_CALL_SUBSUMPTION */

#ifdef INCOMPLETE_TABLING
  continuation_ptr try_answer;
#endif /* INCOMPLETE_TABLING */
  
  struct answer_trie_node *answer_trie;
  
#ifdef YAPOR
  int generator_worker;
  struct or_frame *top_or_frame_on_generator_branch;
#endif /* YAPOR */
} variant_sf;

#define variant_subgoal_frame subgoal_frame
typedef variant_sf *sg_fr_ptr;
typedef sg_fr_ptr variant_sf_ptr;

#define CAST_SF(X)             ((variant_sf_ptr)(X))

#define SgFr_flags(X)          (CAST_SF(X)->flags)
#define SgFr_type(X)           (SgFr_flags(X) & SUBGOAL_FRAME_TYPE_MASK)
#define SgFr_set_type(X, TYPE) (SgFr_flags(X) = (SgFr_flags(X) & ~(SUBGOAL_FRAME_TYPE_MASK)) | (TYPE))
#define SgFr_lock(X)           (CAST_SF(X)->lock)
#define SgFr_gen_worker(X)     (CAST_SF(X)->generator_worker)
#define SgFr_gen_top_or_fr(X)  (CAST_SF(X)->top_or_frame_on_generator_branch)
#define SgFr_code(X)           (CAST_SF(X)->code_of_subgoal)
#define SgFr_tab_ent(X)        ((CAST_SF(X)->code_of_subgoal)->u.Otapl.te)
#define SgFr_arity(X)          ((CAST_SF(X)->code_of_subgoal)->u.Otapl.s)
#define SgFr_state(X)          (CAST_SF(X)->state_flag)
#define SgFr_choice_point(X)   (CAST_SF(X)->choice_point)
#define SgFr_leaf(X)           (CAST_SF(X)->leaf_ptr)
#define SgFr_hash_chain(X)     (TrNode_next(SgFr_answer_trie(X))) /* sibling of root answer trie node */
#define SgFr_answer_trie(X)    (CAST_SF(X)->answer_trie)
#define SgFr_first_answer(X)   (CAST_SF(X)->first_answer)
#define SgFr_last_answer(X)    (CAST_SF(X)->last_answer)
#define SgFr_try_answer(X)     ((X)->try_answer)
#define SgFr_previous(X)       (CAST_SF(X)->previous)
#define SgFr_next(X)           ((X)->next)

#ifdef TABLING_CALL_SUBSUMPTION
#define SgFr_top_gen_sg(X)          (CAST_SF(X)->top_gen_sg)
#define SgFr_new_answer_cp(X)       ((choiceptr)SgFr_executing(X))
#define SgFr_saved_cp(X)            (CAST_SF(X)->saved_cp)
#define SgFr_executing(X)           (CAST_SF(X)->executing)
#define SgFr_start(X)               (CAST_SF(X)->start)
#define SgFr_started(X)             ((CELL *)SgFr_start(X) != &SgFr_start(X))
#define SgFr_got_answer(X)          ((CELL *)SgFr_executing(X) != &SgFr_executing(X))
#define SgFr_is_internal(X)         (SgFr_started(X) && !SgFr_got_answer(X))
#define SgFr_is_external(X)         ((SgFr_started(X) && SgFr_got_answer(X)) || (!SgFr_started(X)))
#endif

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
   SgFr_choice_point:  a pointer to the correspondent generator or first consumer choice point.
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

#ifdef TABLING_CALL_SUBSUMPTION
#define SgFr_is_variant(X)  \
  (SgFr_type(X) == VARIANT_PRODUCER_SFT)
#define SgFr_is_sub_producer(X)  \
  (SgFr_type(X) == SUBSUMPTIVE_PRODUCER_SFT)
#define SgFr_is_sub_consumer(X)   \
  (SgFr_type(X) == SUBSUMED_CONSUMER_SFT)
#else
#define SgFr_is_variant(X) TRUE
#define SgFr_is_sub_producer(X) FALSE
#define SgFr_is_sub_consumer(X) FALSE
#endif /* TABLING_CALL_SUBSUMPTION */

/* ------------------------------ **
**   Subsumption data structures  **
** ------------------------------ */

#include "tab.sub_structs.h"

/* --------------------------------- **
**      Struct dependency_frame      **
** --------------------------------- */

#define DEP_FR_FIRST_CONSUMER 0x01
#define DEP_FR_TOP_CONSUMER 0x02

typedef unsigned char dependency_type;
  
typedef struct dependency_frame {
  dependency_type flags;
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
#ifdef TABLING_CALL_SUBSUMPTION
  sg_fr_ptr top_gen_sg; /* NEW */
#endif /* TABLING_CALL_SUBSUMPTION */
  sg_fr_ptr sg_fr; /* NEW */
  struct dependency_frame *next;
} *dep_fr_ptr;

#define DepFr_type(X)                    ((X)->type)
#define DepFr_lock(X)                    ((X)->lock)
#define DepFr_leader_dep_is_on_stack(X)  ((X)->leader_dependency_is_on_stack)
#define DepFr_top_or_fr(X)               ((X)->top_or_frame)
#define DepFr_timestamp(X)               ((X)->timestamp)
#define DepFr_backchain_cp(X)            ((X)->backchain_choice_point)
#define DepFr_leader_cp(X)               ((X)->leader_choice_point)
#define DepFr_top_gen_sg(X)              ((X)->top_gen_sg)
#define DepFr_cons_cp(X)                 ((X)->consumer_choice_point)
#define DepFr_last_answer(X)             ((X)->last_consumed_answer)
#define DepFr_sg_fr(X)                   ((X)->sg_fr)
#define DepFr_next(X)                    ((X)->next)
#define DepFr_flags(X)                   ((X)->flags)
#define DepFr_set_flag(X, FLAG)          ((X)->flags |= (FLAG))
#define DepFr_is_first_consumer(X)       (DepFr_flags(X) & DEP_FR_FIRST_CONSUMER)
#define DepFr_set_first_consumer(X)      (DepFr_set_flag(X, DEP_FR_FIRST_CONSUMER))
#define DepFr_is_top_consumer(X)         (DepFr_flags(X) & DEP_FR_TOP_CONSUMER)
#define DepFr_set_top_consumer(X)        (DepFr_set_flag(X, DEP_FR_TOP_CONSUMER))

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
#ifdef LOW_LEVEL_TRACER
  struct pred_entry *cp_pred_entry;
#endif /* LOW_LEVEL_TRACER */
};

#endif
