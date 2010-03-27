/*=========================================================================*/

/*
 *		     Iterative Subsumptive Trie Lookup
 *		     =================================
 */

/*-------------------------------------------------------------------------*/


/*
 *			  Variant Continuation
 *			  --------------------
 *
 *  A subsumption-based search operation specifies that a variant entry
 *  be made in case no subsuming term set is present in the trie.  Since
 *  the lookup operation favors variant selection from the trie, we can
 *  utilize the work performed during the initial insurgence, up to the
 *  point where the trie deviated from (a variant of) the terms.  We
 *  note the last node which was successfully matched, the contents of
 *  the TermStack, and the bindings made to the term variables.  This
 *  information is sufficient to proceed, from the last successful
 *  pairing, with the insertion of new nodes into the trie to create a
 *  variant entry of the terms, if need be.
 */

#ifndef MULTI_THREAD
static struct VariantContinuation variant_cont;
#endif

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#define ContStack_ExpandOnOverflow(Stack,StackSize,NeededSize) {	\
									\
   size_t new_size;	/* in number of stack elements */		\
   void *p;								\
									\
   if ( StackSize < NeededSize ) {					\
     new_size = 2 * StackSize;						\
     if ( new_size < NeededSize )					\
       new_size = new_size + NeededSize;				\
     p = realloc(Stack,(new_size * sizeof(Stack[0])));			\
     if ( IsNULL(p) )							\
       return FALSE;							\
     Stack = p;								\
     StackSize = new_size;						\
   }									\
 }


/*
 * This assumes that the state of the search is being saved when its
 * ONLY modification since the last successful match is the pop of the
 * failed-to-match subterm off of the TermStack (and a note in the
 * TermStackLog).  No additional bindings, stack ops, etc., have
 * occurred.  This assumes some knowledge of the workings of the
 * TermStack and Trail: that the top pointer points to the next
 * available location in the stack.
 */

static xsbBool save_variant_continuation(CTXTdeclc BTNptr last_node_match) {

  int i;
  CPtr termptr, *binding;

  variant_cont.last_node_matched = last_node_match;

  /*
   * Include the subterm that couldn't be matched.
   */
  ContStack_ExpandOnOverflow(variant_cont.subterms.stack.ptr,
			     variant_cont.subterms.stack.size,
			     TermStack_NumTerms + 1);
  for ( termptr = TermStack_Base, i = 0;
	termptr <= TermStack_Top;
	termptr++, i++ )
    variant_cont.subterms.stack.ptr[i] = *termptr;
  variant_cont.subterms.num = i;

  ContStack_ExpandOnOverflow(variant_cont.bindings.stack.ptr,
			     variant_cont.bindings.stack.size,
			     Trail_NumBindings);
  i = 0;
  for ( binding = Trail_Base;  binding < Trail_Top;  binding++ ) {
    termptr = *binding;
    /*
     * Take only those bindings made to the call variables.
     * (Finding the value through a single deref is only possible if the
     *  original subterm was deref'ed before trailing.  Currently, this is
     *  the case, with trailing of unmarked callvars occuring in only one
     *  place in the code.)
     */
    if ( ! IsUnboundTrieVar(termptr) ) {
      variant_cont.bindings.stack.ptr[i].var = termptr;
      variant_cont.bindings.stack.ptr[i].value = *termptr;
      i++;
    }
  }
  variant_cont.bindings.num = i;
  return TRUE;
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*
 * Restores the state of a trie search to the point from which insertion
 * is to begin.  This includes readying the term stack with the remaining
 * portion of the term set, restandardizing any variables already
 * encountered, and noting these bindings on the trail.
 */

void *stl_restore_variant_cont(CTXTdecl) {

  int i;

  TermStack_ResetTOS;
  TermStack_PushArray(variant_cont.subterms.stack.ptr,
		      variant_cont.subterms.num);

  Trail_ResetTOS;
  for (i = 0; i < (int) variant_cont.bindings.num; i++) {
    Trail_Push(variant_cont.bindings.stack.ptr[i].var);
    bld_ref(variant_cont.bindings.stack.ptr[i].var,
	    variant_cont.bindings.stack.ptr[i].value);
  }
  return (variant_cont.last_node_matched);
}

/*-------------------------------------------------------------------------*/

/*
 *		     Points of Choice During the Search
 *		     ----------------------------------
 *
 *  State information is saved so the search may resume from that point
 *  down an alternate path in the trie.  This allows for an efficient
 *  iterative traversal of the trie.
 */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* Choice Point Stack and Operations
   --------------------------------- */

#ifndef MULTI_THREAD
static struct tstCCPStack_t tstCCPStack;
#endif

/**** Use these to access the frame to which `top' points ****/
#define CPF_AlternateNode	((tstCCPStack.top)->alt_node)
#define CPF_VariableChain	((tstCCPStack.top)->var_chain)
#define CPF_TermStackTopIndex	((tstCCPStack.top)->termstk_top_index)
#define CPF_TermStackLogTopIndex	((tstCCPStack.top)->log_top_index)
#define CPF_TrailTopIndex	((tstCCPStack.top)->trail_top_index)

/**** TST CP Stack Operations ****/
#define CPStack_ResetTOS     tstCCPStack.top = tstCCPStack.base
#define CPStack_IsEmpty      (tstCCPStack.top == tstCCPStack.base)
#define CPStack_IsFull       (tstCCPStack.top == tstCCPStack.ceiling)
#define CPStack_OverflowCheck				\
   if (CPStack_IsFull)					\
     SubsumptiveTrieLookupError("tstCCPStack overflow!")

/*
 *  All that is assumed on CP creation is that the term stack and log have
 *  been altered for the subterm under consideration.  Any additional changes
 *  necessary to continue the search, e.g., pushing functor args or trailing
 *  a variable, must be made AFTER a CPF is pushed.  In this way, the old
 *  state is still accessible.
 */

#define CPStack_PushFrame(AltNode, VarChain) {	\
   CPStack_OverflowCheck;			\
   CPF_AlternateNode = AltNode;			\
   CPF_VariableChain = VarChain;		\
   CPF_TermStackTopIndex =			\
     TermStack_Top - TermStack_Base + 1;	\
   CPF_TermStackLogTopIndex =			\
     TermStackLog_Top - TermStackLog_Base - 1;	\
   CPF_TrailTopIndex = Trail_Top - Trail_Base;	\
   tstCCPStack.top++;				\
 }

/*
 *  Resume the state of a saved point of choice.
 */

#define CPStack_PopFrame(CurNode,VarChain) {		\
   tstCCPStack.top--;					\
   CurNode = CPF_AlternateNode;				\
   VarChain = CPF_VariableChain;			\
   TermStackLog_Unwind(CPF_TermStackLogTopIndex);	\
   TermStack_SetTOS(CPF_TermStackTopIndex);		\
   Trail_Unwind(CPF_TrailTopIndex);			\
 }

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* Manipulating Choice Points During the Search
   -------------------------------------------- */

/*
 * Conditional Choice Points are used after successfully finding an
 * exact match between a non-variable Prolog subterm and a symbol in the
 * trie.  The condition is whether a TrieVar exists among the siblings
 * which could alternately be unified with the subterm.  This check can
 * be performed quickly and may result in the avoidance of choice point
 * creation.  (Whether the discovered variable is relevant is another
 * matter...)
 */

#define Conditionally_Create_ChoicePoint(VariableChain) {	\
   BTNptr alternateBTN = VariableChain;				\
								\
   while ( IsNonNULL(alternateBTN) ) {				\
     if ( IsTrieVar(BTN_Symbol(alternateBTN)) ) {		\
       CPStack_PushFrame(alternateBTN, VariableChain)		\
       break;							\
     }								\
     alternateBTN = BTN_Sibling(alternateBTN);			\
   }								\
 }


/*
 * We unconditionally lay a Choice Point whenever we've found a trievar
 * binding identical to the call's subterm.  A computation which may
 * result in avoiding CPF creation would be expensive (and useless if we
 * never return to this CPF), so it might be best to just lay one and
 * take your chances that we never use it.  If we do use it, however,
 * the additional overhead of having the main algorithm discover that it
 * is (nearly) useless isn't too high compared to the computation we
 * could have done.  The benefit is that we don't do it unless we have to.
 */

#define Create_ChoicePoint(AlternateBTN,VariableChain)	\
   CPStack_PushFrame(AlternateBTN,VariableChain)


/*
 * Resuming a point of choice additionally involves indicating to the
 * search algorithm what sort of pairings should be sought.
 */

#define BacktrackSearch {			\
   CPStack_PopFrame(pCurrentBTN,variableChain);	\
   search_mode = MATCH_WITH_TRIEVAR;		\
 }

/*-------------------------------------------------------------------------*/

/*
 * Initialize the search's choice point stack, the structure for housing
 * the variant continuation, and the elements of the TrieVarBindings[]
 * array.
 */

void initSubsumptiveLookup(CTXTdecl) {

  int i;

  tstCCPStack.ceiling = tstCCPStack.base + CALL_CPSTACK_SIZE;

  variant_cont.subterms.stack.ptr = NULL;
  variant_cont.bindings.stack.ptr = NULL;
  variant_cont.subterms.stack.size =
    variant_cont.bindings.stack.size = 0;
  
  /* set entries to unbound */
  for (i = 0; i < NUM_TRIEVARS; i++)
    TrieVarBindings[i] = (Cell)(TrieVarBindings + i);
}

/*-------------------------------------------------------------------------*/

/* Error Handling
   -------------- */

#define SubsumptiveTrieLookupError(String)   sub_trie_lookup_error(CTXTc String)

static void sub_trie_lookup_error(CTXTdeclc char *string) {
  Trail_Unwind_All;
  xsb_abort("Subsumptive Check/Insert Operation:\n\t%s\n", string);
}

/*-------------------------------------------------------------------------*/

/* Trie- and Prolog-Variable Handling
   ---------------------------------- */

/*
 * A subsumptive trie-search algorithm requires that we track both
 * variables appearing in the Prolog terms (for handling nonlinearity of
 * these terms in case we have to insert them) and bindings made to the
 * trie variables during processing (for handling nonlinearity in a trie
 * path).  Therefore we take the following approach.  Prolog variables are
 * bound to cells of the VarEnumerator[] array, just as is done during
 * term interning.  For noting the bindings made to variables appearing in
 * the trie, we employ another array, called TrieVarBindings[], which will
 * maintain pointers to dereferenced subterms of the term set.  The Ith
 * TrieVar along a particular path is associated with TrieVarBindings[I].
 *
 * Given that I is the ID of the next unique trievar, upon encountering
 * the Jth never-before seen Prolog variable, this Prolog variable is
 * bound to the *Ith* cell of VarEnumerator, thereby standardizing it.
 * Notice that as a result of using subsumption, variables in the Prolog
 * terms can only be "matched" with variables in the trie, and hence there
 * will be at least as many variables in a subsuming path of the trie as
 * in the Prolog terms themselves.  Therefore, J <= I. As indicated above,
 * TrieVarBindings[I] gets the address of the Prolog variable.  By using
 * the same array indexer, I, we can determine from a standardized Prolog
 * variable the first trievar that has been bound to it, and this enables
 * us to find the actual address of this variable so that additional trie
 * variables may bind themselves to it, if necessary.  In this way, the
 * cells of TrieVarBindings[] is guaranteed to contain DEREFERENCED Prolog
 * terms.  (Note that additional dereferencing may lead one to a nonProlog
 * (non-Heap or non- Local Stack residing) variable.)
 */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*
 *  Given a trievar number and a prolog subterm (not a VarEnumerator
 *  addr), bind the trievar to that subterm and trail the trievar.
 */
#define TrieVar_BindToSubterm(TrieVarNum,Subterm)	\
   TrieVarBindings[TrieVarNum] = Subterm;		\
   Trail_Push(&TrieVarBindings[TrieVarNum])

/*
 *  Given a TrieVar number and a marked PrologVar (bound to a
 *  VarEnumerator cell), bind the TrieVar to the variable subterm
 *  represented by the marked PrologVar, and trail the TrieVar.
 */
#define TrieVar_BindToMarkedPrologVar(TrieVarNum,PrologVarMarker)	\
   TrieVarBindings[TrieVarNum] =					\
     TrieVarBindings[PrologVar_Index(PrologVarMarker)];			\
   Trail_Push(&TrieVarBindings[TrieVarNum])

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*
 *  Given an unmarked, dereferenced Prolog variable and a TrieVar number,
 *  mark the variable with this number by setting it to point to the
 *  Index-th cell of the VarEnumerator array, and trail the variable.
 */
#define PrologVar_MarkIt(DerefedVar,Index)	\
   StandardizeVariable(DerefedVar,Index);	\
   Trail_Push((CPtr)DerefedVar)

/*
 *  Given a dereferenced Prolog variable, determine whether it has already
 *  been marked, i.e. seen during prior processing and hence bound to a
 *  VarEnumerator cell.
 */
#define PrologVar_IsMarked(pDerefedPrologVar)	\
   IsStandardizedVariable(pDerefedPrologVar)

/*
 *  Given an address into VarEnumerator, determine its index in this array.
 *  (This index will also correspond to the first trie variable that bound
 *   itself to it.)
 */
#define PrologVar_Index(VarEnumAddr)  IndexOfStdVar(VarEnumAddr)


/*-------------------------------------------------------------------------*/

/* Algorithmic Shorthands
   ---------------------- */

/*
 *  When first stepping onto a particular trie level, we may find
 *  ourselves either looking at a hash table header or a trie node in a
 *  simple chain.  Given the object first encountered at this level
 *  (pointed to by 'pCurrentBTN') and a trie-encoded symbol, determine
 *  the node chains on this level which would contain that symbol or
 *  contain any trie variables, should either exist.
 */

#define Set_Matching_and_TrieVar_Chains(Symbol,MatchChain,VarChain)	\
   if ( IsNonNULL(pCurrentBTN) && IsHashHeader(pCurrentBTN) ) {		\
     BTNptr *buckets;							\
     BTHTptr pBTHT;							\
									\
     pBTHT = (BTHTptr)pCurrentBTN;					\
     buckets = BTHT_BucketArray(pBTHT);					\
     MatchChain = buckets[TrieHash(Symbol,BTHT_GetHashSeed(pBTHT))];	\
     VarChain = buckets[TRIEVAR_BUCKET];				\
   }									\
   else	   /* simple chain of nodes */					\
     VarChain = MatchChain = pCurrentBTN

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*
 *  As soon as it is known that no variant set of terms exists in the   
 *  trie, that component of the current state of the search which is
 *  useful for later insertion of the given set of terms is saved in
 *  the global VariantContinuation structure.
 */

#define SetNoVariant(LastNodeMatched)				\
   if (variant_path == YES) {					\
     if ( ! save_variant_continuation(CTXTc LastNodeMatched) )	\
       SubsumptiveTrieLookupError("Memory exhausted.");		\
     variant_path = NO;						\
   }

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* 
 *  Exact Matches for Non-Variable Subterms
 *  ---------------------------------------
 *  Exact matches for non-variable subterms of the call are always looked
 *  for first.  Once an exact match has been found, there is no need to
 *  search further, since there is at most one occurrence of a symbol at
 *  a level below a particular node.  Further exploration will
 *  concentrate on pairing the subterm with trie variables.
 *
 *  After a successful match, a choice point frame is laid, with
 *  VariableChain providing the trie-path continuation.  We step down onto
 *  the next level of the trie, below the matched node, and then branch
 *  back to the major loop of the algorithm.  If no match is found, then
 *  execution exits this block with a NULL MatchChain.  MatchChain may be
 *  NULL at block entry.
 *
 *  TermStack_PushOp is provided so that this macro can be used for
 *  constants, functor symbols, and lists.  Constants pass in a
 *  TermStack_NOOP, while functors use TermStack_PushFunctorArgs(), and
 *  lists use TermStack_PushListArgs().
 */

#define NonVarSearchChain_ExactMatch(Symbol,MatchChain,VariableChain,	\
				     TermStack_PushOp)			\
									\
   while ( IsNonNULL(MatchChain) ) {					\
     if (Symbol == BTN_Symbol(MatchChain)) {				\
       Conditionally_Create_ChoicePoint(VariableChain)			\
       TermStack_PushOp							\
       Descend_In_Trie_and_Continue(MatchChain);			\
     }									\
     MatchChain = BTN_Sibling(MatchChain);				\
   }

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* 
 *  Matching Non-Variable Subterms with Bound Trievars
 *  --------------------------------------------------
 *  After having either (1) failed to find an exact match for the
 *  call's non-variable subterm, or (2) backtracked to explore other
 *  unifications, we proceed to check for equality of the subterm and
 *  the bindings made to previously seen trievars.  (A trie variable
 *  whose first occurrence (along the path from the root to here) is
 *  at this level would, of course, not have a binding.)  There may be
 *  several trievars at a level with such a binding.
 *
 *  We check for true equality, meaning that variables appearing in both
 *  must be identical.  (This is because no part of the call may become
 *  futher instantiated as a result of the check/insert operation.)  If
 *  they are the same, we succeed, else we continue to look in the chain
 *  for a suitable trievar.  Processing a success entails (1) laying a
 *  choice point since there may be other pairings in this node chain, (2)
 *  moving to the child of the matching node, (3) resetting our search mode
 *  to exact matches, and (4) branching back to the major loop of the
 *  algorithm.
 *
 *  If no match is found, then execution exits this block with a NULL
 *  'CurNODE'.  'CurNODE' may be NULL at block entry.
 */

#define NonVarSearchChain_BoundTrievar(Subterm,CurNODE,VarChain) {	\
									\
   int trievar_index;							\
									\
   while ( IsNonNULL(CurNODE) ) {					\
     if ( IsTrieVar(BTN_Symbol(CurNODE)) &&				\
          ! IsNewTrieVar(BTN_Symbol(CurNODE)) ) {			\
       trievar_index = DecodeTrieVar(BTN_Symbol(CurNODE));		\
       if ( are_identical_terms(TrieVarBindings[trievar_index],		\
				Subterm) ) {				\
	 Create_ChoicePoint(BTN_Sibling(CurNODE),VarChain)		\
	 Descend_In_Trie_and_Continue(CurNODE);				\
       }								\
     }									\
     CurNODE = BTN_Sibling(CurNODE);					\
   }									\
 }

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* 
 *  Matching Non-Variable Subterms with Unbound Trievars
 *  ----------------------------------------------------
 *  Unifying a call's non-variable subterm with an unbound trievar is
 *  the last pairing operation explored.  Only one unbound trievar may
 *  occur below a particular node; it is a new (first occurrence of this
 *  particular) variable lying along the subpath from the root to the
 *  current position in the trie.  Such trievars are tagged with a
 *  "first occurrence" marker.  If we don't find one, we exit with a
 *  NULL 'VarChain'.  (Node: Because of the hashing scheme, VarChain may
 *  contain symbols other than variables; furthermore, it may be empty
 *  altogether.)
 */

#define NonVarSearchChain_UnboundTrievar(Subterm,VarChain) {	\
								\
   int trievar_index;						\
								\
   while ( IsNonNULL(VarChain) ) {				\
     if ( IsTrieVar(BTN_Symbol(VarChain)) &&			\
          IsNewTrieVar(BTN_Symbol(VarChain)) ) {		\
       trievar_index = DecodeTrieVar(BTN_Symbol(VarChain));	\
       TrieVar_BindToSubterm(trievar_index,Subterm);		\
       Descend_In_Trie_and_Continue(VarChain);			\
     }								\
     VarChain = BTN_Sibling(VarChain);				\
   }								\
 }

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* On a Successful Unification
 * ---------------------------
 * For continuing with forward execution.  When we find a successful
 * pairing, we continue the search with the next subterm on the
 * tstTermStack and the children of the trie node that was paired.
 */

#define Descend_In_Trie_and_Continue(PairedBTN)      \
   pParentBTN = PairedBTN;                           \
   pCurrentBTN = BTN_Child(PairedBTN);               \
   search_mode = MATCH_SYMBOL_EXACTLY;               \
   goto While_TermStack_NotEmpty

/*--------------------------------------------------------------------------*/

/*
 *		Subsumptive Trie Lookup Operation (Iterative)
 *		---------------------------------------------
 *
 * Search a branch of a NON-EMPTY trie for a path representing terms
 * which subsume the term set present on the TermStack.  The branch is
 * identified by a non-NULL pointer to a trie node which roots the
 * branch.  If such a path can be found, return a pointer to the leaf of
 * the trie representing this path and indicate the relationship between
 * the two sets of terms in the argument `pathType'.  If no such path
 * exists, then return NULL in addition to a corresponding value for
 * `pathType'.
 *
 * During the course of the search, once the given term set is known to
 * differ from any path in the trie, state information is noted in a
 * global structure to enable fast insertion of the given term set at a
 * later time, if desired.  Because of this ability, this function
 * serves as the basis of the *_subsumptive_search() routines.
 *
 * Stacks are assumed initialized at invocation and are left dirty upon
 * return.  It is up to the caller to prime the stacks before invocation
 * and clean up afterwards.  This provides a standard format for
 * accepting target terms and allows the caller to inspect the stacks
 * afterwards.  In particular, the variables appearing in the terms set,
 * as well as the bindings made to the trie variables which permit the
 * terms' subsumption by a trie path, are available to the caller.
 *
 * We can think of a subsumptive algorithm as only allowing one-way
 * bindings: from trie variable to subterm.  This particular algorithm
 * attempts to minimize the number of bindings needed to match terms
 * existing in the trie with those that are given.  This approach allows
 * it to immediately converge on a variant entry, should one exist.
 * Note, however, that this strategy is greedy, and so a global minimum
 * could be missed.  The general strategy for pairing the given term
 * set's subterms to trie symbols favors, in order:
 *   1) exact matches
 *   2) matches to bindings of previously bound trie variables
 *   3) pairings with unbound trie variables (causing new bindings)
 * The search terminates once the first such subsuming path is
 * encountered.
 */

typedef enum Search_Strategy_Mode {
  MATCH_SYMBOL_EXACTLY, MATCH_WITH_TRIEVAR
} SearchMode;

void *iter_sub_trie_lookup(CTXTdeclc void *trieNode, TriePathType *pathType) {

  BTNptr pParentBTN;

  BTNptr pCurrentBTN;        /* Used for stepping through siblings while
				looking for a match with subterm */

  BTNptr variableChain;      /* Chain of nodes in which to search for
				variables in case we cannot find an exact
				match in the chain headed by pCurrentBTN */

  Cell subterm;              /* The part of the call we are inspecting */

  Cell symbol;               /* subterm, as it would appear in a trie node */

  int trievar_index;         /* temp for converting trievar num encoding */

  xsbBool variant_path;      /* Denotes whether the call is a variant of that
				represented by the current path.  Search state
				info is saved when its value changes from YES
				to NO.  This may only occur once as this
				procedure begins by looking for a variant
				representation of the call in the trie. */

  SearchMode search_mode;    /* Depending on the subterm, indicates which type
			        of unifications we are interested in */


  pParentBTN = trieNode;
  CPStack_ResetTOS;
  pCurrentBTN = BTN_Child(pParentBTN);
  variableChain = NULL;   /* suppress compiler warning */
  variant_path = YES;
  search_mode = MATCH_SYMBOL_EXACTLY;

  //  printf("in iter_sub_trie_lookup\n");
  //  printSymbolStack(CTXTc stddbg, "  TermStack", tstTermStack);
  
  /* Major loop of the algorithm
     --------------------------- */
 While_TermStack_NotEmpty:

  while ( ! TermStack_IsEmpty ) {
    TermStack_Pop(subterm);
    TermStackLog_PushFrame;
    XSB_Deref(subterm);
    //    printf("   ");printterm(stddbg,subterm,25);printf(" %x\n",subterm);
    switch(cell_tag(subterm)) {
    /* SUBTERM IS A CONSTANT
       --------------------- */
    case XSB_STRING:
    case XSB_INT:
#ifdef SUBSUMPTION_XSB
    case XSB_FLOAT:
#endif
      /*
       *  NOTE:  A Trie constant looks like a Heap constant.
       */
      if (search_mode == MATCH_SYMBOL_EXACTLY) {
	      symbol = EncodeTrieConstant(subterm);
	      Set_Matching_and_TrieVar_Chains(symbol,pCurrentBTN,variableChain);
	      NonVarSearchChain_ExactMatch(symbol, pCurrentBTN, variableChain,
				     TermStack_NOOP)
	      /*
	       *  We've failed to find an exact match of the constant in a node
	       *  of the trie, so now we consider bound trievars whose bindings
	       *  exactly match the constant.
	       */
	      pCurrentBTN = variableChain;
	      SetNoVariant(pParentBTN);
      }
      NonVarSearchChain_BoundTrievar(subterm,pCurrentBTN,variableChain);
	/*
	 *  We've failed to find an exact match of the constant with a
	 *  binding of a trievar.  Our last alternative is to bind an
	 *  unbound trievar to this constant.
	 */
      NonVarSearchChain_UnboundTrievar(subterm,variableChain);
      break;


    /* SUBTERM IS A STRUCTURE
       ---------------------- */
    case XSB_STRUCT:
      /*
       *  NOTE:  A trie XSB_STRUCT is a XSB_STRUCT-tagged PSC ptr, while a heap
       *      	  XSB_STRUCT is a XSB_STRUCT-tagged ptr to a PSC ptr.
       */
      if (search_mode == MATCH_SYMBOL_EXACTLY) {
	      symbol = EncodeTrieFunctor(subterm);
	      Set_Matching_and_TrieVar_Chains(symbol,pCurrentBTN,variableChain);
	      NonVarSearchChain_ExactMatch(symbol, pCurrentBTN, variableChain,
				     TermStack_PushFunctorArgs(subterm))
	      /*
	       *  We've failed to find an exact match of the functor's name in
	       *  a node of the trie, so now we consider bound trievars whose
	       *  bindings exactly match the subterm.
	       */
	      pCurrentBTN = variableChain;
	      SetNoVariant(pParentBTN);
      }
      NonVarSearchChain_BoundTrievar(subterm,pCurrentBTN,variableChain);
	    /*
	     *  We've failed to find an exact match of the function expression
	     *  with a binding of a trievar.  Our last alternative is to bind
	     *  an unbound trievar to this subterm.
	     */
      NonVarSearchChain_UnboundTrievar(subterm,variableChain);
      break;

      
    /* SUBTERM IS A LIST
       ----------------- */
    case XSB_LIST:
      /*
       *  NOTE:  A trie LIST uses a plain LIST tag wherever a recursive
       *         substructure begins, while a heap LIST uses a LIST-
       *         tagged ptr to a pair of Cells, the first being the head
       *         and the second being the recursive tail, possibly another
       *         LIST-tagged ptr.
       */
      if (search_mode == MATCH_SYMBOL_EXACTLY) {
	      symbol = EncodeTrieList(subterm);
	      Set_Matching_and_TrieVar_Chains(symbol,pCurrentBTN,variableChain);
	      NonVarSearchChain_ExactMatch(symbol, pCurrentBTN, variableChain,
				     TermStack_PushListArgs(subterm))
	      /*
	       *  We've failed to find a node in the trie with a XSB_LIST symbol, so
	       *  now we consider bound trievars whose bindings exactly match the
	       *  actual list subterm.
	       */
	      pCurrentBTN = variableChain;
	      SetNoVariant(pParentBTN);
      }
      NonVarSearchChain_BoundTrievar(subterm,pCurrentBTN,variableChain);
	    /*
	     *  We've failed to find an exact match of the list with a binding
	     *  of a trievar.  Our last alternative is to bind an unbound
	     *  trievar to this subterm.
	     */
      NonVarSearchChain_UnboundTrievar(subterm,variableChain);
      break;
      

    /* SUBTERM IS AN UNBOUND VARIABLE
       ------------------------------ */
    case XSB_REF:
#ifdef SUBSUMPTION_XSB
    case XSB_REF1:
#endif
      /*
       *  A never-before-seen variable in the call must always match a
       *  free variable in the trie.  We can determine this by checking
       *  for a "first occurrence" tag in the trievar encoding.  Let Num
       *  be the index of this trievar variable.  Then we bind
       *  TrieVarBindings[Num] to 'subterm', the address of the deref'ed
       *  unbound call variable.  We also bind the call variable to
       *  VarEnumerator[Num] so that we can recognize that the call
       *  variable has already been seen.
       *
       *  When such a call variable is re-encountered, we know which
       *  trievar was the first to bind itself to this call variable: we
       *  used its index in marking the call variable when we bound it
       *  to VarEnumerator[Num].  This tagging scheme allows us to match
       *  additional unbound trie variables to it.  Recall that the
       *  TrieVarBindings array should contain *real* subterms, and not
       *  the callvar tags that we've constructed (the pointers into the
       *  VarEnumerator array).  So we must reconstruct a
       *  previously-seen variable's *real* address in order to bind a
       *  new trievar to it.  We can do this by computing the index of
       *  the trievar that first bound itself to it, and look in that
       *  cell of the TrieVarBindings array to get the call variable's
       *  *real* address.
       *
       *  Notice that this allows us to match variants.  For if we have
       *  a variant up to the point where we encounter a marked callvar,
       *  there can be at most one trievar which exactly matches it.  An
       *  unbound callvar, then, matches exactly only with an unbound
       *  trievar.  Therefore, only when a previously seen callvar must
       *  be paired with an unbound trievar to continue the search
       *  operation do we say that no variant exists.  (Just as is the
       *  case for other call subterm types, the lack of an exact match
       *  and its subsequent pairing with an unbound trievar destroys
       *  the possibility of a variant.)
       */
      if (search_mode == MATCH_SYMBOL_EXACTLY) {
	      
	      if ( IsNonNULL(pCurrentBTN) && IsHashHeader(pCurrentBTN) )
	        pCurrentBTN = variableChain =
	          BTHT_BucketArray((BTHTptr)pCurrentBTN)[TRIEVAR_BUCKET];
	      else
	        variableChain = pCurrentBTN;

	      if ( ! PrologVar_IsMarked(subterm) ) {
	        AnsVarCtr++;
	        /*
	         *  The subterm is a call variable that has not yet been seen
	         *  (and hence is not tagged).  Therefore, it can only be paired
	         *  with an unbound trievar, and there can only be one of these
	         *  in a chain.  If we find it, apply the unification, mark the
	         *  callvar, trail them both, and continue.  Otherwise, fail.
	         *  Note we don't need to lay a CPF since this is the only
	         *  possible pairing that could result.
	         */
	       while( IsNonNULL(pCurrentBTN) ) {
	         if ( IsTrieVar(BTN_Symbol(pCurrentBTN)) &&
		          IsNewTrieVar(BTN_Symbol(pCurrentBTN)) ) {
	            trievar_index = DecodeTrieVar(BTN_Symbol(pCurrentBTN));
	            TrieVar_BindToSubterm(trievar_index,subterm);
	            PrologVar_MarkIt(subterm,trievar_index);
	            Descend_In_Trie_and_Continue(pCurrentBTN);
	          }
	          pCurrentBTN = BTN_Sibling(pCurrentBTN);
	        }
	        SetNoVariant(pParentBTN);
	        break;     /* no pairing, so backtrack */
	      }
      }
      /*
       *  We could be in a forward or backward execution mode.  In either
       *  case, the call variable has been seen before, and we first look
       *  to pair this occurrence of the callvar with a trievar that was
       *  previously bound to this particular callvar.  Note that there
       *  could be several such trievars.  Once we have exhausted this
       *  possibility, either immediately or through backtracking, we then
       *  allow the binding of an unbound trievar to this callvar.
       */
      while ( IsNonNULL(pCurrentBTN) ) {
	      if ( IsTrieVar(BTN_Symbol(pCurrentBTN)) &&
	        ! IsNewTrieVar(BTN_Symbol(pCurrentBTN)) ) {
	        trievar_index = DecodeTrieVar(BTN_Symbol(pCurrentBTN));
	        if ( are_identical_terms(TrieVarBindings[trievar_index],
				        subterm) ) {
	          Create_ChoicePoint(BTN_Sibling(pCurrentBTN),variableChain);
	          Descend_In_Trie_and_Continue(pCurrentBTN);
	        }
	      }
	      pCurrentBTN = BTN_Sibling(pCurrentBTN);
      }
      /*
       *  We may have arrived here under several circumstances, but notice
       *  that the path we are on cannot be a variant one.  In case the
       *  possibility of a variant entry being present was still viable up
       *  to now, we save state info in case we need to create a variant
       *  entry later.  We now go to our last alternative, that of
       *  checking for an unbound trievar to pair with the marked callvar.
       *  If one is found, we trail the trievar, create the binding, and
       *  continue.  No CPF need be created since there can be at most one
       *  new trievar below any given node.
       */
      SetNoVariant(pParentBTN);

      while( IsNonNULL(variableChain) ) {
	      if ( IsTrieVar(BTN_Symbol(variableChain)) &&
	        IsNewTrieVar(BTN_Symbol(variableChain)) ) {
	        trievar_index = DecodeTrieVar(BTN_Symbol(variableChain));
	        TrieVar_BindToMarkedPrologVar(trievar_index,subterm);
	        Descend_In_Trie_and_Continue(variableChain);
	      }
	      variableChain = BTN_Sibling(variableChain);
      }
      break;

#ifdef SUBSUMPTION_XSB
    case XSB_ATTV:
      xsb_table_error(CTXTc 
	      "Attributed variables not yet implemented in calls to subsumptive tables.");
      break;
#endif /* SUBSUMPTION_XSB */
#ifdef SUBSUMPTION_YAP
      case TAG_FLOAT:
        if(search_mode == MATCH_SYMBOL_EXACTLY) {
          Float flt = FloatOfTerm(subterm);
          
          Set_Matching_and_TrieVar_Chains((Term)flt, pCurrentBTN, variableChain);
          
          while(IsNonNULL(pCurrentBTN)) {
            if(TrNode_is_float(pCurrentBTN))
            {
              int go = FALSE;
              
              if(TrNode_is_call(pCurrentBTN))
                go = (flt == TrNode_float((float_sg_node_ptr)pCurrentBTN));
              else
                go = (flt == TSTN_float((float_tst_node_ptr)pCurrentBTN));
              
              if(go) {
                Conditionally_Create_ChoicePoint(variableChain)
                Descend_In_Trie_and_Continue(pCurrentBTN);
              }
            }
            pCurrentBTN = BTN_Sibling(pCurrentBTN);
          }
          
          /* failed to find a float */
          pCurrentBTN = variableChain;
          SetNoVariant(pParentBTN);
        }
        NonVarSearchChain_BoundTrievar(subterm, pCurrentBTN, variableChain);
        NonVarSearchChain_UnboundTrievar(subterm, variableChain);
        break;
      case TAG_LONG_INT:
        if(search_mode == MATCH_SYMBOL_EXACTLY) {
          Int li = LongIntOfTerm(subterm);
          Set_Matching_and_TrieVar_Chains((Term)li, pCurrentBTN, variableChain);
          
          while(IsNonNULL(pCurrentBTN)) {
            if(TrNode_is_long(pCurrentBTN))
            {
              int go = FALSE;
              
              if(TrNode_is_call(pCurrentBTN))
                go = (li == TrNode_long_int((long_sg_node_ptr)pCurrentBTN));
              else
                go = (li == TSTN_long_int((long_tst_node_ptr)pCurrentBTN));
              
              if(go) {
                Conditionally_Create_ChoicePoint(variableChain)
                Descend_In_Trie_and_Continue(pCurrentBTN);
              }
            }
            pCurrentBTN = BTN_Sibling(pCurrentBTN);
          }
          
          /* failed to find a long int */
          pCurrentBTN = variableChain;
          SetNoVariant(pParentBTN);
        }
        NonVarSearchChain_BoundTrievar(subterm, pCurrentBTN, variableChain);
        NonVarSearchChain_UnboundTrievar(subterm, variableChain);
      break;
#endif /* SUBSUMPTION_YAP */
    /* SUBTERM HAS UNKNOWN CELL TAG
       ---------------------------- */
    default:
      TrieError_UnknownSubtermTag(subterm);
      break;
    } /* END switch(cell_tag(subterm)) */

    /*
     *  We've reached a dead-end since we were unable to match the
     *  current subterm to a trie node.  Therefore, we backtrack to
     *  continue the search, or, if there are no more choice point
     *  frames -- in which case the trie has been completely searched --
     *  we return and indicate that no subsuming path was found.
     */
    if (! CPStack_IsEmpty)
      BacktrackSearch
    else {
      *pathType = NO_PATH;
      return NULL;
    }

  } /* END while( ! TermStack_IsEmpty ) */

  /*
   *  The TermStack is empty, so we've reached a leaf node representing
   *  term(s) which subsumes the given term(s).  Return this leaf and an
   *  indication as to whether this path is a variant of or properly
   *  subsumes the given term(s).
   */
  if ( variant_path )
    *pathType = VARIANT_PATH;
  else
    *pathType = SUBSUMPTIVE_PATH;
  return pParentBTN;
}
