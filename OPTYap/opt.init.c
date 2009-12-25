/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   R. Rocha and NCC - University of Porto, Portugal
  File:        opt.init.c  
  version:     $Id: opt.init.c,v 1.16 2008-04-11 16:26:18 ricroc Exp $   
                                                                     
**********************************************************************/

/* ------------------ **
**      Includes      **
** ------------------ */

#include "Yap.h"
#if defined(YAPOR) || defined(TABLING)
#define OPT_MAVAR_STATIC
#include "Yatom.h"
#include "YapHeap.h"
#include <unistd.h>
#include <signal.h>
#ifdef YAPOR
#include "or.macros.h"
#endif	/* YAPOR */
#ifdef TABLING
#include "tab.macros.h"
#endif /* TABLING */
#if defined(TABLING) || !defined(ACOW)
#ifndef TABLING
#include "opt.mavar.h"
#endif /* !TABLING */
#ifdef MULTI_ASSIGNMENT_VARIABLES
ma_hash_entry Yap_ma_hash_table[MAVARS_HASH_SIZE];
UInt Yap_timestamp;    /* an unsigned int */
ma_h_inner_struct *Yap_ma_h_top;
#endif /* MULTI_ASSIGNMENT_VARIABLES */
#endif /* TABLING || !ACOW */
#ifdef ACOW
#include "sys/wait.h"
#endif /* ACOW */



/* ---------------------- **
**      Local macros      **
** ---------------------- */

#ifdef SHM_MEMORY_ALLOC_SCHEME
#define STRUCTS_PER_PAGE(STR_TYPE)  ((Yap_page_size - STRUCT_SIZE(struct page_header)) / STRUCT_SIZE(STR_TYPE))

#define INIT_PAGES(PG, STR_TYPE)                         \
        INIT_LOCK(Pg_lock(PG));                          \
        Pg_pg_alloc(PG) = 0;                             \
        Pg_str_in_use(PG) = 0;                           \
        Pg_str_per_pg(PG) = STRUCTS_PER_PAGE(STR_TYPE);  \
        Pg_free_pg(PG) = NULL
#else
#define INIT_PAGES(PG, STR_TYPE)  Pg_str_in_use(PG) = 0
#endif /* SHM_MEMORY_ALLOC_SCHEME */



/* -------------------------- **
**      Global functions      **
** -------------------------- */

void Yap_init_global(int max_table_size, int n_workers, int sch_loop, int delay_load) {
  int i;

  /* global data related to memory management */
#ifdef LIMIT_TABLING
  if (max_table_size)
    GLOBAL_MAX_PAGES = ((max_table_size - 1) * 1024 * 1024 / SHMMAX + 1) * SHMMAX / Yap_page_size;
  else
    GLOBAL_MAX_PAGES = -1;
#endif /* LIMIT_TABLING */
  INIT_PAGES(GLOBAL_PAGES_void, void *);      
#ifdef YAPOR
  INIT_PAGES(GLOBAL_PAGES_or_fr, struct or_frame);
  INIT_PAGES(GLOBAL_PAGES_qg_sol_fr, struct query_goal_solution_frame);
  INIT_PAGES(GLOBAL_PAGES_qg_ans_fr, struct query_goal_answer_frame);
#endif /* YAPOR */
#ifdef TABLING_INNER_CUTS
  INIT_PAGES(GLOBAL_PAGES_tg_sol_fr, struct table_subgoal_solution_frame);
  INIT_PAGES(GLOBAL_PAGES_tg_ans_fr, struct table_subgoal_answer_frame);
#endif /* TABLING_INNER_CUTS */
#ifdef TABLING
#ifdef GLOBAL_TRIE
  INIT_PAGES(GLOBAL_PAGES_gt_node, struct global_trie_node);
  INIT_PAGES(GLOBAL_PAGES_gt_hash, struct global_trie_hash);
#endif /* GLOBAL_TRIE */
#ifdef TABLING_ANSWER_LIST_SCHEME
  INIT_PAGES(GLOBAL_PAGES_ans_list, struct answer_list);
#endif /* TABLING_ANSWER_LIST_SCHEME */
  INIT_PAGES(GLOBAL_PAGES_tab_ent, struct table_entry);
  INIT_PAGES(GLOBAL_PAGES_sg_fr, struct subgoal_frame);
  INIT_PAGES(GLOBAL_PAGES_sg_node, struct subgoal_trie_node);
  INIT_PAGES(GLOBAL_PAGES_ans_node, struct answer_trie_node);
  INIT_PAGES(GLOBAL_PAGES_sg_hash, struct subgoal_trie_hash);
  INIT_PAGES(GLOBAL_PAGES_ans_hash, struct answer_trie_hash);
  INIT_PAGES(GLOBAL_PAGES_dep_fr, struct dependency_frame);
#endif /* TABLING */
#if defined(YAPOR) && defined(TABLING)
  INIT_PAGES(GLOBAL_PAGES_susp_fr, struct suspension_frame);
#endif /* YAPOR && TABLING */

#ifdef YAPOR
  /* global static data */
  number_workers = n_workers;
  worker_pid(0) = getpid();
  for (i = 1; i < number_workers; i++) worker_pid(i) = 0;
  SCHEDULER_LOOP = sch_loop;
  DELAYED_RELEASE_LOAD = delay_load;

  /* global data related to or-performance */
  GLOBAL_number_goals = 0;
  GLOBAL_best_times(0) = 0;
  GLOBAL_performance_mode = PERFORMANCE_OFF;

  /* global data related to or-parallelism */
  BITMAP_clear(GLOBAL_bm_present_workers);
  for (i = 0; i < number_workers; i++) 
    BITMAP_insert(GLOBAL_bm_present_workers, i);
  BITMAP_copy(GLOBAL_bm_idle_workers, GLOBAL_bm_present_workers);
  BITMAP_clear(GLOBAL_bm_root_cp_workers);
  BITMAP_clear(GLOBAL_bm_invisible_workers);
  BITMAP_clear(GLOBAL_bm_requestable_workers);
  BITMAP_clear(GLOBAL_bm_executing_workers);
  BITMAP_copy(GLOBAL_bm_finished_workers, GLOBAL_bm_present_workers);
  INIT_LOCK(GLOBAL_LOCKS_bm_idle_workers);
  INIT_LOCK(GLOBAL_LOCKS_bm_root_cp_workers);
  INIT_LOCK(GLOBAL_LOCKS_bm_invisible_workers);
  INIT_LOCK(GLOBAL_LOCKS_bm_requestable_workers);
  INIT_LOCK(GLOBAL_LOCKS_bm_executing_workers);
  INIT_LOCK(GLOBAL_LOCKS_bm_finished_workers);
#ifdef TABLING_INNER_CUTS
  INIT_LOCK(GLOBAL_LOCKS_bm_pruning_workers);
#endif /* TABLING_INNER_CUTS */
  GLOBAL_LOCKS_who_locked_heap = MAX_WORKERS;
  INIT_LOCK(GLOBAL_LOCKS_heap_access);
  INIT_LOCK(GLOBAL_LOCKS_alloc_block);
#if defined(YAPOR_ERRORS) || defined(TABLING_ERRORS)
  INIT_LOCK(GLOBAL_LOCKS_stderr_messages);
#endif /* YAPOR_ERRORS || TABLING_ERRORS */
  if (number_workers == 1)
    PARALLEL_EXECUTION_MODE = FALSE;
  else
    PARALLEL_EXECUTION_MODE = TRUE;
#endif /* YAPOR */

#ifdef TABLING
  /* global data related to tabling */
  GLOBAL_root_tab_ent = NULL;
#ifdef GLOBAL_TRIE
  new_global_trie_node(GLOBAL_root_gt, 0, NULL, NULL, NULL);
#endif /* GLOBAL_TRIE */
#ifdef LIMIT_TABLING
  GLOBAL_first_sg_fr = NULL;
  GLOBAL_last_sg_fr = NULL;
  GLOBAL_check_sg_fr = NULL;
#endif /* LIMIT_TABLING */
  GLOBAL_root_dep_fr = NULL;
  for (i = 0; i < MAX_TABLE_VARS; i++) {
    CELL *pt = GLOBAL_table_var_enumerator_addr(i);
    RESET_VARIABLE(pt);
  }
#ifdef TABLE_LOCK_AT_WRITE_LEVEL
  for (i = 0; i < TABLE_LOCK_BUCKETS; i++)
    INIT_LOCK(GLOBAL_table_lock(i));
#endif /* TABLE_LOCK_AT_WRITE_LEVEL */
#endif /* TABLING */

  return;
}


void Yap_init_local(void) {
#ifdef YAPOR
  /* local data related to or-parallelism */
  LOCAL = REMOTE + worker_id;
  LOCAL_top_cp = B_BASE;
  LOCAL_top_or_fr = GLOBAL_root_or_fr;
  LOCAL_load = 0;
  LOCAL_share_request = MAX_WORKERS;
  LOCAL_reply_signal = ready;
#ifdef ENV_COPY
  INIT_LOCK(LOCAL_lock_signals);
#endif /* ENV_COPY */
  LOCAL_prune_request = NULL;
#endif /* YAPOR */
  INIT_LOCK(LOCAL_lock);
#ifdef TABLING
  /* local data related to tabling */
  LOCAL_next_free_ans_node = NULL;
  LOCAL_top_sg_fr = NULL; 
  LOCAL_top_dep_fr = GLOBAL_root_dep_fr; 
#ifdef YAPOR
  LOCAL_top_cp_on_stack = B_BASE; /* ??? */
  LOCAL_top_susp_or_fr = GLOBAL_root_or_fr;
#endif /* YAPOR */
#endif /* TABLING */
  return;
}


void make_root_frames(void) {
#ifdef YAPOR
  /* root or frame */
  or_fr_ptr or_fr;

  ALLOC_OR_FRAME(or_fr);   
  INIT_LOCK(OrFr_lock(or_fr));
  OrFr_alternative(or_fr) = NULL;
  BITMAP_copy(OrFr_members(or_fr), GLOBAL_bm_present_workers);
  OrFr_node(or_fr) = B_BASE;
  OrFr_nearest_livenode(or_fr) = NULL;
  OrFr_depth(or_fr) = 0;
  OrFr_pend_prune_cp(or_fr) = NULL;
  OrFr_nearest_leftnode(or_fr) = or_fr;
  OrFr_qg_solutions(or_fr) = NULL;
#ifdef TABLING_INNER_CUTS
  OrFr_tg_solutions(or_fr) = NULL;
#endif /* TABLING_INNER_CUTS */
#ifdef TABLING
  OrFr_owners(or_fr) = number_workers;
  OrFr_next_on_stack(or_fr) = NULL;
  OrFr_suspensions(or_fr) = NULL;
  OrFr_nearest_suspnode(or_fr) = or_fr;
#endif /* TABLING */
  OrFr_next(or_fr) = NULL;
  GLOBAL_root_or_fr = or_fr;
#endif /* YAPOR */

#ifdef TABLING
  /* root dependency frame */
  if (!GLOBAL_root_dep_fr)
    new_dependency_frame(GLOBAL_root_dep_fr, FALSE, NULL, NULL, NULL, NULL, NULL);
#endif /* TABLING */
}


#ifdef YAPOR
void init_workers(void) {
  int proc;
  NOfThreads = number_workers;
#ifdef ACOW
  if (number_workers > 1) {
    int son;
    son = fork();
    if (son == -1)
      Yap_Error(FATAL_ERROR, TermNil, "fork error (init_workers)");
    if (son > 0) {
      /* I am the father, I must stay here and wait for my children to all die */
      struct sigaction sigact;

      GLOBAL_master_worker = getpid();
      sigact.sa_handler = SIG_DFL;
      sigemptyset(&sigact.sa_mask);
      sigact.sa_flags = SA_RESTART;
      sigaction(SIGINT, &sigact, NULL);
      pause();
      exit(0);
    } else worker_pid(0) = getpid();
  }
#endif /* ACOW */
  for (proc = 1; proc < number_workers; proc++) {
    int son;
    son = fork();
    if (son == -1)
      Yap_Error(FATAL_ERROR, TermNil, "fork error (init_workers)");
    if (son == 0) { 
      /* new worker */
      worker_id = proc;
      remap_memory();
      break;
    }
    else worker_pid(proc) = son;
  }
}
#endif /* YAPOR */
#endif /* YAPOR || TABLING */
