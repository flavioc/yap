/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   R. Rocha and NCC - University of Porto, Portugal
  File:        or.engine.c
  version:     $Id: or.engine.c,v 1.11 2008-03-25 16:45:53 vsc Exp $   
                                                                     
**********************************************************************/

/* ------------------ **
**      Includes      **
** ------------------ */

#include "Yap.h"
#ifdef ENV_COPY
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#include "Yatom.h"
#include "YapHeap.h"
#include "or.macros.h"
#ifdef TABLING
#include "tab.macros.h"
#else
#include "opt.mavar.h"
#endif /* TABLING */



/* ------------------------------------- **
**      Local functions declaration      **
** ------------------------------------- */

static void share_private_nodes(int worker_q);



/* ---------------------- **
**      Local macros      **
** ---------------------- */

#define INCREMENTAL_COPY 1
#if INCREMENTAL_COPY
#define COMPUTE_SEGMENTS_TO_COPY_TO(Q)                                   \
        REMOTE_start_global_copy(Q) = (CELL) (REMOTE_top_cp(Q)->cp_h);   \
        REMOTE_end_global_copy(Q)   = (CELL) (B->cp_h);                  \
        REMOTE_start_local_copy(Q)  = (CELL) (B);                        \
        REMOTE_end_local_copy(Q)    = (CELL) (REMOTE_top_cp(Q));         \
        REMOTE_start_trail_copy(Q)  = (CELL) (REMOTE_top_cp(Q)->cp_tr);  \
        REMOTE_end_trail_copy(Q)    = (CELL) (TR)
#else
#define COMPUTE_SEGMENTS_TO_COPY_TO(Q)                                   \
        REMOTE_start_global_copy(Q) = (CELL) (H0);   \
        REMOTE_end_global_copy(Q)   = (CELL) (H);                  \
        REMOTE_start_local_copy(Q)  = (CELL) (B);                        \
        REMOTE_end_local_copy(Q)    = (CELL) (LCL0);         \
        REMOTE_start_trail_copy(Q)  = (CELL) (Yap_TrailBase);  \
        REMOTE_end_trail_copy(Q)    = (CELL) (TR)
#endif

#define P_COPY_GLOBAL_TO(Q)                                                         \
        memcpy((void *) (worker_offset(Q) + REMOTE_start_global_copy(Q)),           \
               (void *) REMOTE_start_global_copy(Q),                                \
               (size_t) (REMOTE_end_global_copy(Q) - REMOTE_start_global_copy(Q)))
#define Q_COPY_GLOBAL_FROM(P)                                                       \
        memcpy((void *) LOCAL_start_global_copy,                                    \
               (void *) (worker_offset(P) + LOCAL_start_global_copy),               \
               (size_t) (LOCAL_end_global_copy - LOCAL_start_global_copy))

#define P_COPY_LOCAL_TO(Q)                                                          \
        memcpy((void *) (worker_offset(Q) + REMOTE_start_local_copy(Q)),            \
               (void *) REMOTE_start_local_copy(Q),                                 \
               (size_t) (REMOTE_end_local_copy(Q) - REMOTE_start_local_copy(Q)))
#define Q_COPY_LOCAL_FROM(P)                                                        \
        memcpy((void *) LOCAL_start_local_copy,                                     \
               (void *) (worker_offset(P) + LOCAL_start_local_copy),                \
               (size_t) (LOCAL_end_local_copy - LOCAL_start_local_copy))

#define P_COPY_TRAIL_TO(Q)                                                          \
        memcpy((void *) (worker_offset(Q) + REMOTE_start_trail_copy(Q)),            \
               (void *) REMOTE_start_trail_copy(Q),                                 \
               (size_t) (REMOTE_end_trail_copy(Q) - REMOTE_start_trail_copy(Q)))
#define Q_COPY_TRAIL_FROM(P)                                                        \
        memcpy((void *) LOCAL_start_trail_copy,                                     \
               (void *) (worker_offset(P) + LOCAL_start_trail_copy),                \
               (size_t) (LOCAL_end_trail_copy - LOCAL_start_trail_copy))



/* -------------------------- **
**      Global functions      **
** -------------------------- */

void make_root_choice_point(void) {
  if (worker_id == 0) {
    LOCAL_top_cp = GLOBAL_root_cp = OrFr_node(GLOBAL_root_or_fr) = B;
  } else {
    B = LOCAL_top_cp = GLOBAL_root_cp;
    B->cp_tr = TR = ((choiceptr) (worker_offset(0) + (CELL)(B)))->cp_tr;
  }
  B->cp_h = H0;
  B->cp_ap = GETWORK;
  B->cp_or_fr = GLOBAL_root_or_fr;
  LOCAL_top_or_fr = GLOBAL_root_or_fr;
  LOCAL_load = 0;
  Set_LOCAL_prune_request(NULL);
  BRANCH(worker_id, 0) = 0;
#ifdef TABLING_INNER_CUTS
  LOCAL_pruning_scope = NULL;
#endif /* TABLING_INNER_CUTS */
#ifdef TABLING
  LOCAL_top_cp_on_stack = LOCAL_top_cp;
  adjust_freeze_registers();
#endif /* TABLING */
  return;
}


void free_root_choice_point(void) {
  B = LOCAL_top_cp->cp_b;
#ifdef TABLING
  LOCAL_top_cp_on_stack =
#endif /* TABLING */
  LOCAL_top_cp = GLOBAL_root_cp = OrFr_node(GLOBAL_root_or_fr) = (choiceptr) Yap_LocalBase;
  return;
}


int p_share_work(void) {
  int worker_q = LOCAL_share_request;

  if (! BITMAP_member(OrFr_members(REMOTE_top_or_fr(worker_q)), worker_id) ||
      B == REMOTE_top_cp(worker_q) ||
      (LOCAL_load <= DELAYED_RELEASE_LOAD && OrFr_nearest_livenode(LOCAL_top_or_fr) == NULL)) {
    /* refuse sharing request */
    REMOTE_reply_signal(LOCAL_share_request) = no_sharing;
    LOCAL_share_request = MAX_WORKERS;
    PUT_OUT_REQUESTABLE(worker_id);
    return 0;
  }
  /* sharing request accepted */
  COMPUTE_SEGMENTS_TO_COPY_TO(worker_q);
  REMOTE_q_fase_signal(worker_q) = Q_idle;
  REMOTE_p_fase_signal(worker_q) = P_idle;
#ifndef TABLING
  /* wait for incomplete installations */
  while (LOCAL_reply_signal != worker_ready);
#endif /* TABLING */
  LOCAL_reply_signal = sharing;
  REMOTE_reply_signal(worker_q) = sharing;
  share_private_nodes(worker_q);
  REMOTE_reply_signal(worker_q) = nodes_shared;
  /* copy local stack ? */
  LOCK(REMOTE_lock_signals(worker_q));
  if (REMOTE_q_fase_signal(worker_q) < local) {
    REMOTE_p_fase_signal(worker_q) = local;              
    UNLOCK(REMOTE_lock_signals(worker_q));
    P_COPY_LOCAL_TO(worker_q);
  } else {
    UNLOCK(REMOTE_lock_signals(worker_q));
    goto sync_with_q;
  }
  /* copy global stack ? */
  LOCK(REMOTE_lock_signals(worker_q));
  if (REMOTE_q_fase_signal(worker_q) < global) {
    REMOTE_p_fase_signal(worker_q) = global;
    UNLOCK(REMOTE_lock_signals(worker_q));
    P_COPY_GLOBAL_TO(worker_q);
  } else {
    UNLOCK(REMOTE_lock_signals(worker_q));
    goto sync_with_q;
  }
  /* copy trail stack ? */
  LOCK(REMOTE_lock_signals(worker_q));
  if (REMOTE_q_fase_signal(worker_q) < trail) {
    REMOTE_p_fase_signal(worker_q) = trail;
    UNLOCK(REMOTE_lock_signals(worker_q));
    P_COPY_TRAIL_TO(worker_q);
  } else UNLOCK(REMOTE_lock_signals(worker_q));

sync_with_q:
  REMOTE_reply_signal(worker_q) = copy_done;
  while (LOCAL_reply_signal == sharing);
  while (REMOTE_reply_signal(worker_q) != worker_ready);
  LOCAL_share_request = MAX_WORKERS;
  PUT_IN_REQUESTABLE(worker_id);

  return 1;
}


int q_share_work(int worker_p) {
  register tr_fr_ptr aux_tr;
  register CELL aux_cell;

  LOCK_OR_FRAME(LOCAL_top_or_fr);
  if (REMOTE_prune_request(worker_p)) {
    /* worker p with prune request */
    UNLOCK_OR_FRAME(LOCAL_top_or_fr);
    return FALSE;
  }
#ifdef YAPOR_ERRORS
  if (Get_OrFr_pend_prune_cp(LOCAL_top_or_fr) &&
      BRANCH_LTT(worker_p, OrFr_depth(LOCAL_top_or_fr)) < OrFr_pend_prune_ltt(LOCAL_top_or_fr))
    YAPOR_ERROR_MESSAGE("prune ltt > worker_p branch ltt (q_share_work)");
#endif /* YAPOR_ERRORS */
  /* there is no pending prune with worker p at right --> safe move to worker p branch */
  BRANCH(worker_id, OrFr_depth(LOCAL_top_or_fr)) = BRANCH(worker_p, OrFr_depth(LOCAL_top_or_fr));
  LOCAL_prune_request = NULL;
  UNLOCK_OR_FRAME(LOCAL_top_or_fr);

  /* unbind variables */
  aux_tr = LOCAL_top_cp->cp_tr;
#ifdef TABLING_ERRORS
  if (TR < aux_tr)
    TABLING_ERROR_MESSAGE("TR < aux_tr (q_share_work)");
#endif /* TABLING_ERRORS */
  while (aux_tr != TR) {
    aux_cell = TrailTerm(--TR);
    /* check for global or local variables */
    if (IsVarTerm(aux_cell)) {
      RESET_VARIABLE(aux_cell);
#ifdef TABLING
    } else if (IsPairTerm(aux_cell)) {
      aux_cell = (CELL) RepPair(aux_cell);
      if (IN_BETWEEN(Yap_TrailBase, aux_cell, Yap_TrailTop)) {
	/* avoid frozen segments */
        TR = (tr_fr_ptr) aux_cell;
#ifdef TABLING_ERRORS
        if (TR > (tr_fr_ptr) Yap_TrailTop)
          TABLING_ERROR_MESSAGE("TR > Yap_TrailTop (q_share_work)");
        if (TR < aux_tr)
          TABLING_ERROR_MESSAGE("TR < aux_tr (q_share_work)");
#endif /* TABLING_ERRORS */
      }
#endif /* TABLING */
#ifdef MULTI_ASSIGNMENT_VARIABLES
    } else if (IsApplTerm(aux_cell)) {
      CELL *aux_ptr = RepAppl(aux_cell);
      Term aux_val = TrailTerm(--aux_tr);
      *aux_ptr = aux_val;
#endif /* MULTI_ASSIGNMENT_VARIABLES */
    }
  }

#ifdef OPTYAP_ERRORS
  if (LOCAL_top_cp != LOCAL_top_cp_on_stack)
    OPTYAP_ERROR_MESSAGE("LOCAL_top_cp != LOCAL_top_cp_on_stack (q_share_work)");
  if (YOUNGER_CP(B_FZ, LOCAL_top_cp))
    OPTYAP_ERROR_MESSAGE("YOUNGER_CP(B_FZ, LOCAL_top_cp) (q_share_work)");
#endif /* OPTYAP_ERRORS */
#ifdef YAPOR_ERRORS
  if (LOCAL_reply_signal != ready)
    YAPOR_ERROR_MESSAGE("LOCAL_reply_signal != ready (q_share_work)");
#endif /* YAPOR_ERRORS */

  /* make sharing request */
  LOCK_WORKER(worker_p);
  if (BITMAP_member(GLOBAL_bm_idle_workers, worker_p) || 
      REMOTE_share_request(worker_p) != MAX_WORKERS) {
    /* worker p is idle or has another request */
    UNLOCK_WORKER(worker_p);
    return FALSE;
  }
  REMOTE_share_request(worker_p) = worker_id;
  UNLOCK_WORKER(worker_p);

  /* wait for an answer */
  while (LOCAL_reply_signal == worker_ready);
  if (LOCAL_reply_signal == no_sharing) {
    /* sharing request refused */
    LOCAL_reply_signal = worker_ready;
    return FALSE;
  }

  /* copy trail stack ? */
  LOCK(LOCAL_lock_signals);
  if (LOCAL_p_fase_signal > trail) {
    LOCAL_q_fase_signal = trail;
    UNLOCK(LOCAL_lock_signals);
    Q_COPY_TRAIL_FROM(worker_p);
  } else {
    UNLOCK(LOCAL_lock_signals);
    goto sync_with_p;
  }

  /* copy global stack ? */
  LOCK(LOCAL_lock_signals);
  if (LOCAL_p_fase_signal > global) {
    LOCAL_q_fase_signal = global;
    UNLOCK(LOCAL_lock_signals);
    Q_COPY_GLOBAL_FROM(worker_p);
  } else {
    UNLOCK(LOCAL_lock_signals);
    goto sync_with_p;
  }

  /* copy local stack ? */
  while (LOCAL_reply_signal < nodes_shared);
  LOCK(LOCAL_lock_signals);
  if (LOCAL_p_fase_signal > local) {
    LOCAL_q_fase_signal = local;
    UNLOCK(LOCAL_lock_signals);
    Q_COPY_LOCAL_FROM(worker_p);
  } else UNLOCK(LOCAL_lock_signals);

sync_with_p:
#ifdef TABLING
  REMOTE_reply_signal(worker_p) = ready;
#else
  REMOTE_reply_signal(worker_p) = copy_done;
#endif /* TABLING */
  while (LOCAL_reply_signal != copy_done);

#if INCREMENTAL_COPY
  /* install fase --> TR and LOCAL_top_cp->cp_tr are equal */
  aux_tr = ((choiceptr) LOCAL_start_local_copy)->cp_tr;
  TR = ((choiceptr) LOCAL_end_local_copy)->cp_tr;
  Yap_NEW_MAHASH((ma_h_inner_struct *)H);
  while (TR != aux_tr) {
    aux_cell = TrailTerm(--aux_tr);
    if (IsVarTerm(aux_cell)) {
      if (aux_cell < LOCAL_start_global_copy || 
          EQUAL_OR_YOUNGER_CP((choiceptr)LOCAL_end_local_copy, (choiceptr)aux_cell)) {
#ifdef YAPOR_ERRORS
        if ((CELL *)aux_cell < H0)
          YAPOR_ERROR_MESSAGE("aux_cell < H0 (q_share_work)");
        if ((ADDR)aux_cell > Yap_LocalBase)
          YAPOR_ERROR_MESSAGE("aux_cell > LocalBase (q_share_work)");
#endif /* YAPOR_ERRORS */
#ifdef TABLING
        *((CELL *) aux_cell) = TrailVal(aux_tr);
#else
        *((CELL *) aux_cell) = *((CELL *) (worker_offset(worker_p) + aux_cell));
#endif /* TABLING */
      }
#ifdef TABLING 
    } else if (IsPairTerm(aux_cell)) {
      aux_cell = (CELL) RepPair(aux_cell);
      if (IN_BETWEEN(Yap_TrailBase, aux_cell, Yap_TrailTop)) {
        /* avoid frozen segments */
        aux_tr = (tr_fr_ptr) aux_cell;
#endif /* TABLING */
#ifdef MULTI_ASSIGNMENT_VARIABLES
    } else if (IsApplTerm(aux_cell)) {
      CELL *cell_ptr = RepAppl(aux_cell);
      if (((CELL *)aux_cell < LOCAL_top_cp->cp_h || 
          EQUAL_OR_YOUNGER_CP(LOCAL_top_cp, (choiceptr)aux_cell)) &&
	  !Yap_lookup_ma_var(cell_ptr)) {
	/* first time we found the variable, let's put the new value */
#ifdef TABLING
        *cell_ptr = TrailVal(aux_tr);
#else
        *cell_ptr = *((CELL *) (worker_offset(worker_p) + (CELL)cell_ptr));
#endif /* TABLING */
      }
      /* skip the old value */
      aux_tr--;
#endif /* MULTI_ASSIGNMENT_VARIABLES */
    }
  }
#endif /* incremental */

  /* update registers and return */
#ifndef TABLING
  REMOTE_reply_signal(worker_p) = worker_ready;
#endif /* TABLING */
  LOCAL_reply_signal = worker_ready;
  PUT_IN_REQUESTABLE(worker_id);
  TR = (tr_fr_ptr) LOCAL_end_trail_copy;
#ifdef TABLING
  adjust_freeze_registers();
#endif /* TABLING */
  return TRUE;
}



/* ------------------------- **
**      Local functions      **
** ------------------------- */

static
void share_private_nodes(int worker_q) {
  choiceptr sharing_node = B;

#ifdef OPTYAP_ERRORS
  if (YOUNGER_CP(LOCAL_top_cp, LOCAL_top_cp_on_stack)) {
    OPTYAP_ERROR_MESSAGE("YOUNGER_CP(LOCAL_top_cp, LOCAL_top_cp_on_stack) (share_private_nodes)");
  } else {
    choiceptr aux_cp = B;
    while (aux_cp != LOCAL_top_cp) {
      if (YOUNGER_CP(LOCAL_top_cp, aux_cp)) {
        OPTYAP_ERROR_MESSAGE("LOCAL_top_cp not in branch (share_private_nodes)");
        break;
      }
      if (EQUAL_OR_YOUNGER_CP(LOCAL_top_cp_on_stack, aux_cp)) {
        OPTYAP_ERROR_MESSAGE("shared frozen segments in branch (share_private_nodes)");
        break;
      }
      aux_cp = aux_cp->cp_b;
    }
  }
#endif /* OPTYAP_ERRORS */

#ifdef TABLING
  /* check if the branch is already shared */
  if (EQUAL_OR_YOUNGER_CP(LOCAL_top_cp_on_stack, sharing_node)) {
    or_fr_ptr or_frame;
    sg_fr_ptr sg_frame;
    dep_fr_ptr dep_frame;

#ifdef OPTYAP_ERRORS
    { or_fr_ptr aux_or_fr;
      aux_or_fr = LOCAL_top_or_fr;
      while (aux_or_fr != REMOTE_top_or_fr(worker_q)) {
        if (YOUNGER_CP(OrFr_node(REMOTE_top_or_fr(worker_q)), OrFr_node(aux_or_fr))) {
          OPTYAP_ERROR_MESSAGE("YOUNGER_CP(OrFr_node(REMOTE_top_or_fr(worker_q)), OrFr_node(aux_or_fr)) (share_private_nodes)");
          break; 
        }
        aux_or_fr = OrFr_next_on_stack(aux_or_fr);
      }
    }
#endif /* OPTYAP_ERRORS */

    /* update old shared nodes */
    or_frame = LOCAL_top_or_fr;
    while (or_frame != REMOTE_top_or_fr(worker_q)) {
      LOCK_OR_FRAME(or_frame);
      BRANCH(worker_q, OrFr_depth(or_frame)) = BRANCH(worker_id, OrFr_depth(or_frame));
      OrFr_owners(or_frame)++;
      if (BITMAP_member(OrFr_members(or_frame), worker_id))
        BITMAP_insert(OrFr_members(or_frame), worker_q);
      UNLOCK_OR_FRAME(or_frame);
      or_frame = OrFr_next_on_stack(or_frame);
    }

    /* update worker Q top subgoal frame */
    sg_frame = LOCAL_top_sg_fr;
    while (sg_frame && YOUNGER_CP(SgFr_choice_point(sg_frame), sharing_node)) {
      sg_frame = SgFr_next(sg_frame);
    }
    REMOTE_top_sg_fr(worker_q) = sg_frame;

    /* update worker Q top dependency frame */
    dep_frame = LOCAL_top_dep_fr;
    while (YOUNGER_CP(DepFr_cons_cp(dep_frame), sharing_node)) {
      dep_frame = DepFr_next(dep_frame);
    }
    REMOTE_top_dep_fr(worker_q) = dep_frame;

    /* update worker Q top shared nodes */
    REMOTE_top_cp_on_stack(worker_q) = REMOTE_top_cp(worker_q) = LOCAL_top_cp;
    REMOTE_top_or_fr(worker_q) = LOCAL_top_or_fr;
  } else
#endif /* TABLING */
  {
    int depth;
    bitmap bm_workers;
    or_fr_ptr or_frame, previous_or_frame;
#ifdef TABLING
    choiceptr consumer_cp, next_node_on_branch;
    dep_fr_ptr dep_frame;
    sg_fr_ptr sg_frame;
    CELL *stack, *stack_base, *stack_limit;

    /* find top dependency frame above current choice point */
    dep_frame = LOCAL_top_dep_fr;
    while (EQUAL_OR_YOUNGER_CP(DepFr_cons_cp(dep_frame), sharing_node)) {
      dep_frame = DepFr_next(dep_frame);
    }
    /* initialize tabling auxiliary variables */ 
    consumer_cp = DepFr_cons_cp(dep_frame);
    next_node_on_branch = NULL;
    stack_limit = (CELL *)TR;
    stack_base = stack = (CELL *)Yap_TrailTop;
#endif /* TABLING */

    /* initialize auxiliary variables */
    BITMAP_clear(bm_workers);
    BITMAP_insert(bm_workers, worker_id);
    BITMAP_insert(bm_workers, worker_q);
    previous_or_frame = NULL;
    depth = OrFr_depth(LOCAL_top_or_fr);

    /* sharing loop */
#ifdef TABLING
    while (YOUNGER_CP(sharing_node, LOCAL_top_cp_on_stack)) {
#else
    while (sharing_node != LOCAL_top_cp) {
#endif /* TABLING */

#ifdef OPTYAP_ERRORS
      if (next_node_on_branch) {
        choiceptr aux_cp = B;
        while (aux_cp != next_node_on_branch) {
          if (sharing_node == aux_cp)
            OPTYAP_ERROR_MESSAGE("sharing_node on branch (share_private_nodes)");
          if (YOUNGER_CP(next_node_on_branch, aux_cp)) {
            OPTYAP_ERROR_MESSAGE("next_node_on_branch not in branch (share_private_nodes)");
            break;
  	  }
          aux_cp = aux_cp->cp_b;
        }
      } else {
        choiceptr aux_cp = B;
        while (aux_cp != sharing_node) {
          if (YOUNGER_CP(sharing_node, aux_cp)) {
            OPTYAP_ERROR_MESSAGE("sharing_node not in branch (share_private_nodes)");
            break;
	  }
          aux_cp = aux_cp->cp_b;
        }
      }
#endif /* OPTYAP_ERRORS */

      ALLOC_OR_FRAME(or_frame);
      if (previous_or_frame) {
#ifdef TABLING
        OrFr_next_on_stack(previous_or_frame) =
#endif /* TABLING */
        OrFr_nearest_livenode(previous_or_frame) = OrFr_next(previous_or_frame) = or_frame;
      }
      previous_or_frame = or_frame;
      depth++;
      INIT_LOCK(OrFr_lock(or_frame));
      OrFr_node(or_frame) = sharing_node;
      OrFr_alternative(or_frame) = sharing_node->cp_ap;
      OrFr_pend_prune_cp(or_frame) = NULL;
      OrFr_nearest_leftnode(or_frame) = LOCAL_top_or_fr;
      OrFr_qg_solutions(or_frame) = NULL;
#ifdef TABLING_INNER_CUTS
      OrFr_tg_solutions(or_frame) = NULL;
#endif /* TABLING_INNER_CUTS */
#ifdef TABLING
      OrFr_suspensions(or_frame) = NULL;
      OrFr_nearest_suspnode(or_frame) = or_frame;
      OrFr_owners(or_frame) = 2;
      if (next_node_on_branch)
        BITMAP_clear(OrFr_members(or_frame));
      else
#endif /* TABLING */
        OrFr_members(or_frame) = bm_workers;

#ifdef YAPOR_ERRORS
      if (sharing_node->cp_ap == GETWORK || sharing_node->cp_ap == GETWORK_SEQ)
        YAPOR_ERROR_MESSAGE("choicepoint already shared (share_private_nodes)");
#endif /* YAPOR_ERRORS */
      if (sharing_node->cp_ap && YAMOP_SEQ(sharing_node->cp_ap)) {
        sharing_node->cp_ap = GETWORK_SEQ;
      } else {
        sharing_node->cp_ap = GETWORK;
      }
      sharing_node->cp_or_fr = or_frame;
      sharing_node = sharing_node->cp_b;

#ifdef TABLING
      /* when next_node_on_branch is not NULL the **
      ** sharing_node belongs to a frozen branch. */   
      if (YOUNGER_CP(consumer_cp, sharing_node)) {
        /* frozen stack segment */
        if (! next_node_on_branch)
          next_node_on_branch = sharing_node;
        STACK_PUSH_UP(or_frame, stack);
        STACK_CHECK_EXPAND1(stack, stack_limit, stack_base);
        STACK_PUSH(sharing_node, stack);
        STACK_CHECK_EXPAND1(stack, stack_limit, stack_base);
        sharing_node = consumer_cp;
        dep_frame = DepFr_next(dep_frame);
        consumer_cp = DepFr_cons_cp(dep_frame);
      } else if (consumer_cp == sharing_node) {
        dep_frame = DepFr_next(dep_frame);
        consumer_cp = DepFr_cons_cp(dep_frame);
      }
      if (next_node_on_branch == sharing_node)
        next_node_on_branch = NULL;
#endif /* TABLING */
#ifdef OPTYAP_ERRORS
      if (next_node_on_branch && YOUNGER_CP(next_node_on_branch, sharing_node))
        OPTYAP_ERROR_MESSAGE("frozen node greater than next_node_on_branch (share_private_nodes)");
#endif /* OPTYAP_ERRORS */
    }

    /* initialize last or-frame pointer */
    or_frame = sharing_node->cp_or_fr;
    if (previous_or_frame) {
#ifdef TABLING
      OrFr_next_on_stack(previous_or_frame) =
#endif /* TABLING */
      OrFr_nearest_livenode(previous_or_frame) = OrFr_next(previous_or_frame) = or_frame;
    }

#ifdef TABLING
    /* update or-frames stored in auxiliary stack */
    while (STACK_NOT_EMPTY(stack, stack_base)) {
      next_node_on_branch = (choiceptr) STACK_POP_DOWN(stack);
      or_frame = (or_fr_ptr) STACK_POP_DOWN(stack);
      OrFr_nearest_livenode(or_frame) = OrFr_next(or_frame) = next_node_on_branch->cp_or_fr;
    }
#endif /* TABLING */

    /* update depth */
    if (depth >= MAX_BRANCH_DEPTH)
      Yap_Error(INTERNAL_ERROR, TermNil, "maximum depth exceded (share_private_nodes)");
    or_frame = B->cp_or_fr;
#ifdef TABLING
    previous_or_frame = LOCAL_top_cp_on_stack->cp_or_fr;
    while (or_frame != previous_or_frame) {
#else
    while (or_frame != LOCAL_top_or_fr) {
#endif /* TABLING */
      unsigned int branch;
      if (OrFr_alternative(or_frame)) {
        branch = YAMOP_OR_ARG(OrFr_alternative(or_frame)) + 1;
      } else {
        branch = 1;
      }
      branch |= YAMOP_CUT_FLAG;  /* in doubt, assume cut */
      BRANCH(worker_id, depth) = BRANCH(worker_q, depth) = branch;
      OrFr_depth(or_frame) = depth--;
      or_frame = OrFr_next_on_stack(or_frame);
    }

#ifdef YAPOR_ERRORS
    if (depth != OrFr_depth(LOCAL_top_or_fr))
      YAPOR_ERROR_MESSAGE("incorrect depth value (share_private_nodes)");
#endif /* YAPOR_ERRORS */

#ifdef OPTYAP_ERRORS
    { or_fr_ptr aux_or_fr = B->cp_or_fr;
      choiceptr aux_cp;
      while (aux_or_fr != LOCAL_top_cp_on_stack->cp_or_fr) {
        aux_cp = OrFr_node(aux_or_fr);
        if (OrFr_next(aux_or_fr) != aux_cp->cp_b->cp_or_fr)
          OPTYAP_ERROR_MESSAGE("OrFr_next not in branch (share_private_nodes)");
        if (OrFr_nearest_livenode(aux_or_fr) != aux_cp->cp_b->cp_or_fr)
          OPTYAP_ERROR_MESSAGE("OrFr_nearest_livenode not in branch (share_private_nodes)");
        aux_or_fr = OrFr_next_on_stack(aux_or_fr);
      }
      aux_or_fr = B->cp_or_fr;
      while (aux_or_fr != LOCAL_top_cp_on_stack->cp_or_fr) {
        or_fr_ptr nearest_leftnode = OrFr_nearest_leftnode(aux_or_fr);
        aux_cp = OrFr_node(aux_or_fr);
        while (OrFr_node(nearest_leftnode) != aux_cp) {
          if (YOUNGER_CP(OrFr_node(nearest_leftnode), aux_cp)) {
            OPTYAP_ERROR_MESSAGE("OrFr_nearest_leftnode not in branch (share_private_nodes)");
            break;
          }
          aux_cp = aux_cp->cp_b;
        }
        aux_or_fr = OrFr_next_on_stack(aux_or_fr);
      }
    }
#endif /* OPTYAP_ERRORS */

    /* update old shared nodes */
    while (or_frame != REMOTE_top_or_fr(worker_q)) {
      LOCK_OR_FRAME(or_frame);
      BRANCH(worker_q, OrFr_depth(or_frame)) = BRANCH(worker_id, OrFr_depth(or_frame));
#ifdef TABLING
      OrFr_owners(or_frame)++;
      if (BITMAP_member(OrFr_members(or_frame), worker_id))
#endif /* TABLING */
        BITMAP_insert(OrFr_members(or_frame), worker_q);
      UNLOCK_OR_FRAME(or_frame);
      or_frame = OrFr_next_on_stack(or_frame);
    }

#ifdef TABLING
    /* update subgoal frames in the maintained private branches */
    sg_frame = LOCAL_top_sg_fr;
    while (sg_frame && YOUNGER_CP(SgFr_choice_point(sg_frame), B)) {
      choiceptr top_cp_on_branch;
      top_cp_on_branch = SgFr_choice_point(sg_frame);
      while (YOUNGER_CP(top_cp_on_branch, B)) {
        top_cp_on_branch = top_cp_on_branch->cp_b;
      }
      SgFr_gen_top_or_fr(sg_frame) = top_cp_on_branch->cp_or_fr;
      sg_frame = SgFr_next(sg_frame);
    }
    /* update worker Q top subgoal frame */
    REMOTE_top_sg_fr(worker_q) = sg_frame;
    /* update subgoal frames in the recently shared branches */
    while (sg_frame && YOUNGER_CP(SgFr_choice_point(sg_frame), LOCAL_top_cp_on_stack)) {
      SgFr_gen_worker(sg_frame) = MAX_WORKERS;
      SgFr_gen_top_or_fr(sg_frame) = SgFr_choice_point(sg_frame)->cp_or_fr;
      sg_frame = SgFr_next(sg_frame);
    }

    /* update dependency frames in the maintained private branches */
    dep_frame = LOCAL_top_dep_fr;
    while (YOUNGER_CP(DepFr_cons_cp(dep_frame), B)) {
      choiceptr top_cp_on_branch;
      top_cp_on_branch = DepFr_cons_cp(dep_frame);
      while (YOUNGER_CP(top_cp_on_branch, B)) {
        top_cp_on_branch = top_cp_on_branch->cp_b;
      }
      DepFr_top_or_fr(dep_frame) = top_cp_on_branch->cp_or_fr;
      dep_frame = DepFr_next(dep_frame);
    }
    /* update worker Q top dependency frame */
    REMOTE_top_dep_fr(worker_q) = dep_frame;
    /* update dependency frames in the recently shared branches */
    while (YOUNGER_CP(DepFr_cons_cp(dep_frame), LOCAL_top_cp_on_stack)) {
      DepFr_top_or_fr(dep_frame) = DepFr_cons_cp(dep_frame)->cp_or_fr;
      dep_frame = DepFr_next(dep_frame);
    }
#endif /* TABLING */

#ifdef OPTYAP_ERRORS
    { dep_fr_ptr aux_dep_fr = LOCAL_top_dep_fr;
      while(aux_dep_fr != GLOBAL_root_dep_fr) {
        choiceptr top_cp_on_branch;
        top_cp_on_branch = DepFr_cons_cp(aux_dep_fr);
        while (YOUNGER_CP(top_cp_on_branch, B)) {
          top_cp_on_branch = top_cp_on_branch->cp_b;
        }
        if (top_cp_on_branch->cp_or_fr != DepFr_top_or_fr(aux_dep_fr))
          OPTYAP_ERROR_MESSAGE("Error on DepFr_top_or_fr (share_private_nodes)");
        aux_dep_fr = DepFr_next(aux_dep_fr);
      }
    }
#endif /* OPTYAP_ERRORS */

    /* update top shared nodes */
#ifdef TABLING
    REMOTE_top_cp_on_stack(worker_q) = LOCAL_top_cp_on_stack = 
#endif /* TABLING */
    REMOTE_top_cp(worker_q) = LOCAL_top_cp = B;
    REMOTE_top_or_fr(worker_q) = LOCAL_top_or_fr = LOCAL_top_cp->cp_or_fr;
  }

#ifdef TABLING_INNER_CUTS
  /* update worker Q pruning scope */
  if (LOCAL_pruning_scope && EQUAL_OR_YOUNGER_CP(LOCAL_top_cp, LOCAL_pruning_scope)) {
    REMOTE_pruning_scope(worker_q) = LOCAL_pruning_scope;
    PUT_IN_PRUNING(worker_q);
  } else {
    PUT_OUT_PRUNING(worker_q);
    REMOTE_pruning_scope(worker_q) = NULL;
  }
#endif /* TABLING_INNER_CUTS */

  /* update worker Q prune request */
  if (LOCAL_prune_request) {
    CUT_send_prune_request(worker_q, LOCAL_prune_request);
  }

  /* update load and return */
  REMOTE_load(worker_q) = LOCAL_load = 0;
  return;
}
#endif /* ENV_COPY */
