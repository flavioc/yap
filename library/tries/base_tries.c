/*********************************************
  File:     base_tries.c
  Author:   Ricardo Rocha
  Comments: Tries base module for Yap Prolog
  version:  $ID$
*********************************************/



/* -------------------------- */
/*          Includes          */
/* -------------------------- */

#include <YapInterface.h>
#include <stdio.h>
#include <string.h>
#include "core_tries.h"
#include "base_tries.h"



/* -------------------------- */
/*       Local Variables      */
/* -------------------------- */

static TrEngine TRIE_ENGINE;
static TrEntry FIRST_TRIE, CURRENT_TRIE;



/* -------------------------- */
/*            API             */     
/* -------------------------- */

inline
void trie_init_module(void) {
  TRIE_ENGINE = core_trie_init_module();
  FIRST_TRIE = NULL;
  return;
}


inline
void trie_data_load(TrNode node, YAP_Int depth, FILE *file) {
  TrData data;

  new_trie_data(data, CURRENT_TRIE, node);
  PUT_DATA_IN_LEAF_TRIE_NODE(node, data);
  return;
}


inline
void trie_data_copy(TrNode node_dest, TrNode node_source) {
  TrData data_dest;

  new_trie_data(data_dest, CURRENT_TRIE, node_dest);
  PUT_DATA_IN_LEAF_TRIE_NODE(node_dest, data_dest);
  return;
}


inline
void trie_data_destruct(TrNode node) {
  TrEntry trie;
  TrData data;

  data = (TrData) GET_DATA_FROM_LEAF_TRIE_NODE(node);
  trie = TrData_trie(data);
  if (data == TrEntry_traverse_data(trie))
    TrEntry_traverse_data(trie) = TrData_previous(data);
  if (TrData_next(data)) {
    TrData_previous(TrData_next(data)) = TrData_previous(data);
    TrData_next(TrData_previous(data)) = TrData_next(data);
  } else {
    TrEntry_last_data(trie) = TrData_previous(data);
    TrData_next(TrData_previous(data)) = NULL;
  }
  free_trie_data(data);
  return;
}


inline
TrEntry trie_open(void) {
  TrEntry trie;
  TrNode node;

  node = core_trie_open(TRIE_ENGINE);
  new_trie_entry(trie, node);
  if (FIRST_TRIE)
    TrEntry_previous(FIRST_TRIE) = trie;
  FIRST_TRIE = trie;
  return trie;
}


inline
void trie_close(TrEntry trie) {
  core_trie_close(TRIE_ENGINE, TrEntry_trie(trie), &trie_data_destruct);
  if (TrEntry_next(trie)) {
    TrEntry_previous(TrEntry_next(trie)) = TrEntry_previous(trie);
    TrEntry_next(TrEntry_previous(trie)) = TrEntry_next(trie);
  } else
    TrEntry_next(TrEntry_previous(trie)) = NULL;
  free_trie_entry(trie);  
  return;
}


inline
void trie_close_all(void) {
  TrEntry trie;

  core_trie_close_all(TRIE_ENGINE, &trie_data_destruct);
  while (FIRST_TRIE) {
    trie = TrEntry_next(FIRST_TRIE);
    free_trie_entry(FIRST_TRIE);
    FIRST_TRIE = trie;
  }
  return;
}


inline
void trie_set_mode(YAP_Int mode) {
  core_trie_set_mode(mode);
  return;
}


inline
YAP_Int trie_get_mode(void) {
  return core_trie_get_mode();
}


inline
TrData trie_put_entry(TrEntry trie, YAP_Term entry) {
  TrData data;
  TrNode node;

  node = core_trie_put_entry(TRIE_ENGINE, TrEntry_trie(trie), entry, NULL);
  if (!(data = (TrData) GET_DATA_FROM_LEAF_TRIE_NODE(node))) {
    new_trie_data(data, trie, node);
    PUT_DATA_IN_LEAF_TRIE_NODE(node, data);
  }
  return data;
}


inline
TrData trie_check_entry(TrEntry trie, YAP_Term entry) {
  TrNode node;

  if (!(node = core_trie_check_entry(TrEntry_trie(trie), entry)))
    return NULL;
  return (TrData) GET_DATA_FROM_LEAF_TRIE_NODE(node);
}


inline
YAP_Term trie_get_entry(TrData data) {
  return core_trie_get_entry(TrData_leaf(data));
}


inline
TrData trie_get_first_entry(TrEntry trie) {
  TrData data;
  
  data = TrEntry_first_data(trie);
  return data;
}


inline
TrData trie_get_last_entry(TrEntry trie) {
  TrData data;
  
  data = TrEntry_last_data(trie);
  if (data == AS_TR_DATA_NEXT(&TrEntry_first_data(trie)))
    return NULL;
  return data;
}


inline
TrData trie_traverse_init(TrEntry trie, TrData init_data) {
  TrData data;

  if (init_data) {
    data = TrData_next(init_data);
  } else {
    data = TrEntry_first_data(trie);
  }
  TrEntry_traverse_data(trie) = data;
  return data;
}


inline
TrData trie_traverse_cont(TrEntry trie) {
  TrData data;

  data = TrEntry_traverse_data(trie);
  if (data) {
    data = TrData_next(data);
    TrEntry_traverse_data(trie) = data;
  }
  return data;
}


inline
void trie_remove_entry(TrData data) {
  core_trie_remove_entry(TRIE_ENGINE, TrData_leaf(data), &trie_data_destruct);
  return;
}


inline
void trie_remove_subtree(TrData data) {
  core_trie_remove_subtree(TRIE_ENGINE, TrData_leaf(data), &trie_data_destruct);
  return;
}


inline
void trie_join(TrEntry trie_dest, TrEntry trie_source) {
  CURRENT_TRIE = trie_dest;
  core_trie_join(TRIE_ENGINE, TrEntry_trie(trie_dest), TrEntry_trie(trie_source), NULL, &trie_data_copy);
  return;
}


inline
void trie_intersect(TrEntry trie_dest, TrEntry trie_source) {
  core_trie_intersect(TRIE_ENGINE, TrEntry_trie(trie_dest), TrEntry_trie(trie_source), NULL, &trie_data_destruct);
  return;
}


inline
YAP_Int trie_count_join(TrEntry trie1, TrEntry trie2) {
  return core_trie_count_join(TrEntry_trie(trie1), TrEntry_trie(trie2));
}


inline
YAP_Int trie_count_intersect(TrEntry trie1, TrEntry trie2) {
  return core_trie_count_intersect(TrEntry_trie(trie1), TrEntry_trie(trie2));
}


inline
void trie_save(TrEntry trie, FILE *file) {
  core_trie_save(TrEntry_trie(trie), file, NULL);
  return;
}


inline
TrEntry trie_load(FILE *file) {
  TrEntry trie;
  TrNode node;

  new_trie_entry(trie, NULL);
  CURRENT_TRIE = trie;
  if (!(node = core_trie_load(TRIE_ENGINE, file, &trie_data_load))) {
    free_trie_entry(trie);  
    return NULL;
  }
  TrEntry_trie(trie) = node;
  if (FIRST_TRIE)
    TrEntry_previous(FIRST_TRIE) = trie;
  FIRST_TRIE = trie;
  return trie;
}


inline
void trie_stats(YAP_Int *memory, YAP_Int *tries, YAP_Int *entries, YAP_Int *nodes) {
  core_trie_stats(TRIE_ENGINE, memory, tries, entries, nodes);
  return;
}


inline
void trie_max_stats(YAP_Int *memory, YAP_Int *tries, YAP_Int *entries, YAP_Int *nodes) {
  core_trie_max_stats(TRIE_ENGINE, memory, tries, entries, nodes);
  return;
}


inline
void trie_usage(TrEntry trie, YAP_Int *entries, YAP_Int *nodes, YAP_Int *virtual_nodes) {
  core_trie_usage(TrEntry_trie(trie), entries, nodes, virtual_nodes);
  return;
}


inline
void trie_print(TrEntry trie) {
  core_trie_print(TrEntry_trie(trie), NULL);
  return;
}

inline
YAP_Term trie_tree(TrEntry trie) {
  TrNode node = TrEntry_trie(trie);
  return core_trie_tree(node);
}