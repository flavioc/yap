/* Use these to access the frame to which `top' points */
#define collectCPF_AlternateNode    (collectTop->alt_node)
#define collectCPF_TermStackTopIndex     (collectTop->ts_top_index)
#define collectCPF_TSLogTopIndex         (collectTop->log_top_index)
#define collectCPF_TrailTop         (collectTop->trail_top)
#define collectCPF_HBreg            (collectTop->heap_bktrk)

#define CPStack_IsEmpty    (collectTop == collectBase)
#define CPStack_IsFull     (collectTop == collectCeiling)

#define CPStack_PushFrame(AlternateTrieNode)				\
   if ( IsNonNULL(AlternateTrieNode) ) {					\
     CPStack_OverflowCheck						\
     collectCPF_AlternateNode = AlternateTrieNode;				\
     if(mode == DESCEND_MODE) {                 \
       collectCPF_TermStackTopIndex = TermStack_Top - TermStack_Base + 1;	\
       collectCPF_TSLogTopIndex = TermStackLog_Top - TermStackLog_Base;	\
       collectCPF_TrailTop = trreg;						\
       collectCPF_HBreg = hbreg;						\
     }  \
     hbreg = hreg;							\
     collectTop++;							\
   }

#define CPStack_Pop     collectTop--

/*
 *  Error handler for the collection algorithm.
 */
#define RequiresCleanup    TRUE
#define NoCleanupRequired  FALSE

#ifdef SUBSUMPTION_YAP
#define ReturnErrorCode FALSE
#else
#define ReturnErrorCode NULL
#endif /* SUBSUMPTION_YAP */

#define Collection_Error(String, DoesRequireCleanup) {	\
   fprintf(stderr, "Error encountered in Collection algorithm!\n"); \
   if (DoesRequireCleanup) {  \
     Sys_Trail_Unwind(trail_base);  \
     Restore_WAM_Registers; \
   }  \
   xsb_abort(String); \
   return ReturnErrorCode;							\
 }

/*
 *  Trailing
 *  --------
 *  Record bindings made during the search through the trie so that
 *  these variables can be unbound when an alternate path is explored.
 *  We will use XSB's system Trail to accomodate XSB's unification
 *  algorithm, which is needed at certain points in the collection
 *  process.
 */

#ifdef SUBSUMPTION_XSB

#ifndef MULTI_THREAD
static CPtr *trail_base;    /* ptr to topmost used Cell on the system Trail;
			       the beginning of our local trail for this
			       operation. */

static CPtr *orig_trreg;            
static CPtr orig_hreg;      /* Markers for noting original values of WAM */
static CPtr orig_hbreg;     /* registers so they can be reset upon exiting */
static CPtr orig_ebreg;
#endif

/* 
 * Set backtrack registers to the tops of stacks to force trailing in
 * unify().  Save hreg as the heap may be used to construct bindings.
 */
#define Save_and_Set_WAM_Registers	\
   orig_hbreg = hbreg;			\
   orig_hreg = hbreg = hreg;		\
   orig_ebreg = ebreg;			\
   ebreg = top_of_localstk;		\
   orig_trreg = trreg;			\
   trreg = top_of_trail

#define Restore_WAM_Registers		\
   trreg = orig_trreg;			\
   hreg = orig_hreg;			\
   hbreg = orig_hbreg;			\
   ebreg = orig_ebreg
   
#define Sys_Trail_Unwind(UnwindBase)      table_undo_bindings(UnwindBase)

#else /* YAP */

static tr_fr_ptr trail_base;
static tr_fr_ptr orig_trreg;
static CPtr orig_hreg;
static CPtr orig_hbreg;

#define Save_and_Set_WAM_Registers  \
  orig_hbreg = hbreg; \
  orig_hreg = hbreg = hreg; \
  orig_trreg = trreg;  \
  trreg = top_of_trail
  
#define Restore_WAM_Registers \
  trreg = orig_trreg; \
  hreg = orig_hreg; \
  hbreg = orig_hbreg

#endif /* SUBSUMPTION_XSB */

#ifdef SUBSUMPTION_XSB	
/*
 *  Create a binding and trail it.
 */
#define Bind_and_Trail(Addr, Val) pushtrail0(Addr, Val) \
   *(Addr) = Val
#endif /* SUBSUMPTION_XSB */

/*
 *  Backtracking to a previous juncture in the trie.
 */
#define Collect_Backtrack				\
   mode = BACKTRACK_MODE; \
   CPStack_Pop;					\
   ResetParentAndCurrentNodes;			\
   RestoreTermStack;				\
   Sys_Trail_Unwind(collectCPF_TrailTop);		\
   ResetHeap_fromCPF

/*
 *  For continuing with forward execution.  When we match, we continue
 *  the search with the next subterm on the tstTermStack and the children
 *  of the trie node that was matched.
 */
#define Descend_Into_Node_and_Continue_Search	\
   parent_node = cur_chain;			\
   cur_chain = BTN_Child(cur_chain);		\
   mode = DESCEND_MODE;                 \
   goto While_TSnotEmpty

/*
 * Not really necessary to set the parent since it is only needed once a
 * leaf is reached and we step (too far) down into the trie, but that's
 * when its value is set.
 */
#define ResetParentAndCurrentNodes		      \
   cur_chain = collectCPF_AlternateNode;		\
   parent_node = BTN_Parent(cur_chain)


#define RestoreTermStack			\
   TermStackLog_Unwind(collectCPF_TSLogTopIndex);	\
   TermStack_SetTOS(collectCPF_TermStackTopIndex)

#define ResetHeap_fromCPF			\
   hreg = hbreg;				\
   hbreg = collectCPF_HBreg

#define CPStack_OverflowCheck						\
   if (CPStack_IsFull)							\
     Collection_Error("collectCPStack overflow.", RequiresCleanup)
     
#define backtrack      break
     
/*
 *  Create a new answer-list node, set it to point to an answer,  
 *  and place it at the head of a chain of answer-list nodes.
 *  For MT engine: use only for private,subsumed tables.
 */
#define ALN_InsertAnswer(pAnsListHead,pAnswerNode) {			            \
    ALNptr newAnsListNode;						                                \
    New_Private_ALN(newAnsListNode,(void *)pAnswerNode,pAnsListHead);	\
    pAnsListHead = newAnsListNode;					                          \
  }
  
#define Trie_bind_copy(Addr,Val)		    \
  Trie_Conditionally_Trail(Addr,Val);		\
  *(Addr) = Val
  
#define Bind_and_Conditionally_Trail(Addr, Val)	Trie_bind_copy(Addr,Val)

/*
 *  Create a binding and conditionally trail it.  TrieVarBindings[] cells
 *  are always trailed, while those in the WAM stacks are trailed based on
 *  the traditional trailing test.  As we traverse the TST and lay choice
 *  points, we update the hbreg as in the WAM since structures may be
 *  built on the heap for binding.  Therefore, this condition serves as in
 *  the WAM.
 */

#define Trie_Conditionally_Trail(Addr, Val)		\
   if ( IsAnswerTemplateVar(Addr) || IsUnboundTrieVar(Addr) || conditional(Addr) )	\
     { pushtrail0(Addr, Val) }

/*******************************************
OLD VERSIONS
 * #define Bind_and_Trail(Addr, Val)	pushtrail0(Addr, Val)

 * #define Bind_and_Conditionally_Trail(Addr, Val)	\
 *  if ( IsUnboundTrieVar(Addr) || conditional(Addr) )	\
 *    { pushtrail0(Addr, Val) }
 *******************************************/
 
