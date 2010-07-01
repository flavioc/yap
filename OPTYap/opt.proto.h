/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   R. Rocha and NCC - University of Porto, Portugal
  File:        opt.proto.h
  version:     $Id: opt.proto.h,v 1.12 2005-11-04 01:17:17 vsc Exp $   
                                                                     
**********************************************************************/

/* -------------- **
**  opt.memory.c  **
** -------------- */

#ifdef YAPOR
#ifdef SHM_MEMORY_MAPPING_SCHEME
void shm_map_memory(int id, int size, void *shmaddr);
#else /* MMAP_MEMORY_MAPPING_SCHEME */
void open_mapfile(long);
void close_mapfile(void);
#endif /* MEMORY_MAPPING_SCHEME */
void map_memory(long HeapArea, long GlobalLocalArea, long TrailAuxArea, int n_workers);
void unmap_memory(void);
void remap_memory(void);
#endif /* YAPOR */


/* ------------ **
**  opt.misc.c  **
** ------------ */

void itos(int i, char *s);
void information_message(const char *mesg,...);
#if defined(YAPOR_ERRORS) || defined(TABLING_ERRORS)
void error_message(const char *mesg, ...);
#endif /* YAPOR_ERRORS || TABLING_ERRORS */


/* ------------ **
**  opt.init.c  **
** ------------ */

void Yap_init_global(int max_table_size, int n_workers, int sch_loop, int delay_load);
void Yap_init_local(void);
void make_root_frames(void);
#ifdef YAPOR
void init_workers(void);
#endif /* YAPOR */


/* ------------- **
**  opt.preds.c  **
** ------------- */

#ifdef YAPOR
void finish_yapor(void);
#endif /* YAPOR */


/* ------------- **
**  tab.tries.c  **
** ------------- */

#ifdef TABLING
sg_fr_ptr subgoal_search(yamop *preg, CELL **local_stack_ptr);
ans_node_ptr answer_search(sg_fr_ptr sg_fr, CELL *subs_ptr);
void delete_subgoal_path(sg_fr_ptr sg_fr);
#ifdef GLOBAL_TRIE
CELL *load_substitution_variable(gt_node_ptr current_node, CELL *aux_stack_ptr);
#endif /* GLOBAL_TRIE */
void complete_dependency_frame(dep_fr_ptr dep_fr);
void private_completion(sg_fr_ptr sg_fr);
#ifdef GLOBAL_TRIE
void free_subgoal_trie_branch(sg_node_ptr node, int nodes_left, int position);
#else
void free_subgoal_trie_branch(sg_node_ptr node, int position);
#endif /* GLOBAL_TRIE */
void free_answer_trie_branch(ans_node_ptr node, int position);
void update_answer_trie_root(ans_node_ptr root_child);
void update_answer_trie(sg_fr_ptr sg_fr);
void show_table(tab_ent_ptr tab_ent, int show_mode);
#ifdef GLOBAL_TRIE
void show_global_trie(void);
#endif /* GLOBAL_TRIE */
#endif /* TABLING */

/* --------------- **
**  tab.var.c      **
** --------------- */

#ifdef TABLING
sg_node_ptr variant_call_cont_insert(tab_ent_ptr tab_ent, sg_node_ptr current_node, int count_vars, int flags);
sg_fr_ptr variant_call_search(yamop *code, CELL *local_stack, CELL **new_local_stack);
ans_node_ptr variant_answer_search(sg_fr_ptr sg_fr, CELL *subs_ptr);
void consume_variant_answer(ans_node_ptr ans_node, int size, CELL *answer_template);
#endif /* TABLING */

/* ----------------- **
** tab.subsumption.c **
** ----------------- */

#ifdef TABLING_CALL_SUBSUMPTION
#ifdef TABLING_COMPLETE_TABLE
void transform_subsumptive_into_retroactive_trie(subprod_fr_ptr sg_fr);
#endif /* TABLING_COMPLETE_TABLE */
void *stl_restore_variant_cont(CTXTdecl);
void *iter_sub_trie_lookup(CTXTdeclc void *trieNode, TriePathType *pathType);
void initSubsumptiveLookup(CTXTdecl);
sg_fr_ptr subsumptive_call_search(yamop *code, CELL *local_stack, CELL **new_local_stack);
TSTNptr subsumptive_tst_search(CTXTdeclc TSTNptr tstRoot, int nTerms, CPtr termVector, xsbBool maintainTSI);
inline TSTNptr subsumptive_answer_search(CTXTdeclc SubProdSF sf, int nTerms, CPtr answerVector);
void *newTSTAnswerSet(void);
#endif /* TABLING_CALL_SUBSUMPTION */

/* --------------------- **
**   tab.retroactive.c   **
** --------------------- */

#ifdef TABLING_CALL_SUBSUMPTION
#ifdef TABLING_COMPLETE_TABLE
void free_subgoal_trie_from_retroactive_table(tab_ent_ptr tab_ent);
#endif /* TABLING_COMPLETE_TABLE */
int is_most_general_call(sg_node_ptr leaf, int arity);
#ifdef TABLING_RETROACTIVE
void update_generator_path(sg_node_ptr node);
void decrement_generator_path(sg_node_ptr start);
CELL* copy_arguments_as_the_answer_template(CELL *answer_template, int arity);
sg_fr_ptr retroactive_call_search(yamop *code, CELL *answer_template, CELL **new_local_stack);
void add_answer_pending(tst_node_ptr node, retroactive_fr_ptr sf);
inline TSTNptr retroactive_answer_search(retroactive_fr_ptr sf, CPtr answerVector);
#endif /* TABLING_RETROACTIVE */
#endif /* TABLING_CALL_SUBSUMPTION */

/* --------------- **
**   tab.unify.c   **
** --------------- */

#ifdef TABLING_CALL_SUBSUMPTION
void consume_subsumptive_answer(CTXTdeclc BTNptr pAnsLeaf, int sizeTmplt, CPtr pAnsTmplt);
#endif /* TABLING_CALL_SUBSUMPTION */

/* ---------------- **
**   tab.consumer.c **
** ---------------- */

#ifdef TABLING_RETROACTIVE
#ifdef RETRO_CHECKS
void check_dependency_frame(void);
#else
#define check_dependency_frame()
#endif
void move_subgoal_top(sg_fr_ptr sg_fr);
void process_pending_subgoal_list(node_list_ptr list, retroactive_fr_ptr sg_fr);
void add_dependency_frame(retroactive_fr_ptr sg_fr, choiceptr cp);
void reinsert_subgoal_frame(sg_fr_ptr sg_fr, choiceptr new_cp);
void reinsert_dependency_frame(dep_fr_ptr dep_fr);
void reorder_subgoal_frame(sg_fr_ptr sg_fr, choiceptr new_gen_cp);
void remove_from_restarted_gens(choiceptr cp);
void remove_sg_fr_from_restarted_gens(sg_fr_ptr sg_fr);
void transform_node_into_loader(choiceptr cp, sg_fr_ptr sg_fr,
    continuation_ptr last, yamop *load_instr);
void transform_consumer_answer_template(sg_fr_ptr sg_fr, choiceptr cp);
#endif /* TABLING_RETROACTIVE */

/* --------------- **
**  tab.suspend.c  **
** --------------- */

#if defined(TABLING) && defined(YAPOR)
void public_completion(void);
void complete_suspension_frames(or_fr_ptr or_fr);
void suspend_branch(void);
void resume_suspension_frame(susp_fr_ptr resume_fr, or_fr_ptr top_or_fr);
#endif /* TABLING && YAPOR */


/* ------------- **
**  or.*engine.c **
** ------------- */

#ifdef YAPOR
void make_root_choice_point(void);
void free_root_choice_point(void);
int q_share_work(int p);
int p_share_work(void);
#endif /* YAPOR */

/* ---------------- **
**  or.scheduler.c  **
** ---------------- */

#ifdef YAPOR
int get_work(void);
#endif /* YAPOR */


/* ---------- **
**  or.cut.c  **
** ---------- */

#ifdef YAPOR
void prune_shared_branch(choiceptr prune_cp);
#endif /* YAPOR */
