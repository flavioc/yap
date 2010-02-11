/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   Flávio Cruz
  File:        tab.retrv.c
  version:        
                                                                     
**********************************************************************/

/* ------------------ **
**      Includes      **
** ------------------ */

#include "Yap.h"
#ifdef TABLING
#include <stdio.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#include "Yatom.h"
#include "YapHeap.h"
#include "yapio.h"
#include "tab.macros.h"
#include "tab.utils.h"
#include "tab.tst.h"
#include "tab.retrv.h"

// CONFIRMAR XXX
#define trreg TR
#define hreg H
#define hbreg HB
#define ereg ENV
#define ebreg YENV
#define trfreg TR_FZ
#define efreg B_FZ
#define cpreg CP
#define top_of_trail ((trreg > trfreg) ? trreg : trfreg)
#define top_of_localstk ASP
#define Sys_Trail_Unwind(TR0) reset_trail(TR0)

static tr_fr_ptr trail_base;	/* ptr to topmost used Cell on the system Trail;
			       the beginning of our local trail for this
			       operation. */
				   
static tr_fr_ptr orig_trreg;
static CPtr orig_hreg;	/* Markers for noting original values of WAM */
static CPtr orig_hbreg;	/* registers so they can be reset upon exiting */
static CPtr orig_ebreg;

/* 
 * Set backtrack registers to the tops of stacks to force trailing in
 * unify().  Save hreg as the heap may be used to construct bindings.
 */
#define Save_and_Set_WAM_Registers	\
	orig_hbreg = hbreg;	\
	orig_hbreg = hbreg = hreg;	\
	orig_ebreg = ebreg;	\
	ebreg = top_of_localstk;	\
	orig_trreg = trreg;	\
	trreg = top_of_trail
	
#define Restore_WAM_Registers	\
	trreg = orig_trreg;	\
	hreg = orig_hreg;	\
	hbreg = orig_hbreg;	\
	ebreg = orig_ebreg
	
/*
 *  Create a binding and conditionally trail it.  TrieVarBindings[] cells
 *  are always trailed, while those in the WAM stacks are trailed based on
 *  the traditional trailing test.  As we traverse the TST and lay choice
 *  points, we update the hbreg as in the WAM since structures may be
 *  built on the heap for binding.  Therefore, this condition serves as in
 *  the WAM.
 */
#define Trie_bind_copy(Addr,Val)	\
	Trie_Conditionally_Trail(Addr,Val);	\
	*(Addr) = Val
	
/* amiops.h XXX */
#define conditional(Addr)	\
	(OUTSIDE(HBREG, Addr, B) || ((Addr) > (CELL *)B_FZ))
#define pushtrail0 DO_TRAIL
	
#define Trie_Conditionally_Trail(Addr,Val)	\
	if(IsUnboundTrieVar(Addr) || conditional(Addr))	\
	{ pushtrail0(Addr,Val) }
	
#define Bind_and_Conditionally_Trail(Addr,Val) Trie_bind_copy(Addr,Val)
	
/* ------------------------------------------------------------------------- */

/*
 *  tstCPStack
 *  ----------
 *  For saving state information so the search may resume from that
 *  point down an alternate path in the time-stamped trie.
 */
 
 /* Use these to access the frame to which `top' points */
#define tstCPF_AlternateNode ((tstCPStack.top)->alt_node)
#define tstCPF_TermStackTopIndex ((tstCPStack.top)->ts_top_index)
#define tstCPF_TSLogTopIndex ((tstCPStack.top)->log_top_index)
#define tstCPF_TrailTop ((tstCPStack.top)->trail_top)
#define tstCPF_HBreg ((tstCPStack.top)->heap_bktrk)

#define CPStack_IsEmpty (tstCPStack.top == tstCPStack.base)
#define CPStack_IsFull (tstCPStack.top == tstCPStack.ceiling)

#define CPStack_PushFrame(AlternateTSTN)	\
	if(IsNonNULL(AlternateTSTN))	{	\
		CPStack_OverflowCheck	\
		tstCPF_AlternateNode = AlternateTSTN;	\
		tstCPF_TermStackTopIndex = TermStack_Top - TermStack_Base + 1;	\
		tstCPF_TrailTop = trreg;	\
		tstCPF_HBreg = hbreg;	\
		tstCPStack.top++;	\
	}
	
#define CPStack_Pop tstCPStack.top--
 
/*
 *  For continuing with forward execution.  When we match, we continue
 *  the search with the next subterm on the tstTermStack and the children
 *  of the trie node that was matched.
 */
 #define Descend_Into_TST_and_Continue_Search	\
	parentTSTN = cur_chain;	\
	cur_chain = TSTN_Child(cur_chain);	\
	goto While_TSnotEmpty
	
#define CPStack_OverflowCheck	\
	if(CPStack_IsFull)	\
		TST_Collection_Error("tstCPStack overflow.", RequiresCleanup)
 
/* ========================================================================= */

/*  General Macro Utilities
    =======================  */

/*
 *  Determine whether the TSTN's timestamp allows it to be selected.
 */
#define IsValidTS(SymbolTS,CutoffTS)      (SymbolTS > CutoffTS)
	
/*
 *  Error handler for the collection algorithm.
 */
#define RequiresCleanup    TRUE
#define NoCleanupRequired  FALSE

#define TST_Collection_Error(String, DoesRequireCleanup) {	\
		tstCollectionError(CTXTc String, DoesRequireCleanup);	\
		return NULL;	\
	}

static void tstCollectionError(CTXTdeclc char* string, xsbBool cleanup_required) {
	fprintf(stderr, "Error encountered in Time-Stamped Trie"
		"Collection algorithm!\n");
	if(cleanup_required) {
		Sys_Trail_Unwind(trail_base);
		Restore_WAM_Registers;
	}
	xsb_abort(string);
}

/* ========================================================================= */

/*
 *  Algorithmic Shorthands
 *  ======================
 */


#define backtrack      break

/*
 *  Return the first TSTN in a chain with a valid timestamp (if one exists),
 *  otherwise return NULL.
 */
#define Chain_NextValidTSTN(Chain,TS,tsAccessMacro)	\
	while(IsNonNULL(Chain) && (!IsValidTS(tsAccessMacro(Chain),TS)))	\
		Chain = TSTN_Sibling(Chain)

/* ---------------------------------------------------- */

#define SetMatchAndUnifyChains(Symbol,SymChain,VarChain) {	\
	TSTHTptr ht = (TSTHTptr)SymChain;	\
	TSTNptr *buckets = TSTHT_BucketArray(ht);	\
	SymChain = buckets[TrieHash(Symbol,TSTHT_GetHashSeed(ht))];	\
	VarChain = buckets[TRIEVAR_BUCKET];	\
}

/* ------------------------------------------------------------------------- */

/* 
 *  Exact matches are only looked for in cases where the TSTN is hashed
 *  and the subterm hashes to a bucket different from TRIEVAR_BUCKET.
 *  Once a match has been found, there is no need to search further since
 *  there is only one occurrance of a symbol in any chain.  If the node's
 *  timestamp is valid then the state is saved, with bucket TRIEVAR_BUCKET
 *  as the TSTN continuation, and we branch back to the major loop of the
 *  algorithm.  If the timestamp is not valid, then we exit the loop.  If
 *  no match is found, then execution exits this block with a NULL
 *  cur_chain.  cur_chain may be NULL at block entry.
 *
 *  TermStack_PushOp is provided so that this macro can be used for all
 *  nonvariable symbols.  Constants pass in TermStack_NOOP, functors use
 *  TermStack_PushFunctorArgs(), and lists useTermStack_PushListArgs().
 */
#define SearchChain_ExactMatch(SearchChain,TrieEncodedSubterm,TS,	\
			ContChain,TermStack_PushOp)	\
	while(IsNonNULL(SearchChain)) {	\
		if(TrieEncodedSubterm == TSTN_Symbol(SearchChain)) {	\
			if(IsValidTS(TSTN_GetTSfromTSIN(SearchChain),TS)) {	\
				Chain_NextValidTSTN(ContChain,TS,TSTN_GetTSfromTSIN);	\
				CPStack_PushFrame(ContChain);	\
				TermStackLog_PushFrame;	\
				TermStack_PushOp;	\
				Descend_Into_TST_and_Continue_Search;	\
			}	\
			else	\
				break; /* matching symbol's TS is too old */	\
		}	\
		SearchChain = TSTN_Sibling(SearchChain);	\
	}
	
/* ------------------------------------------------------------------------- */

/*
 *  Overview:
 *  --------
 *  There are 4 cases when this operation should be used:
 *   1) Searching an unhashed chain.
 *   2) Searching bucket TRIEVAR_BUCKET after searching the hashed-to bucket.
 *   3) Searching bucket TRIEVAR_BUCKET which is also the hashed-to bucket.
 *   4) Searching some hashed chain that has been restored through
 *        backtracking.
 *
 *  (1) and (3) clearly require a general algorithm, capable of dealing
 *  with vars and nonvars alike.  (4) must use this since we may be
 *  continuing an instance of (3).  (2) also requires a deref followed by
 *  inspection, since a derefed variable may (or may not) lead to the
 *  symbol we are interested in.
 *
 *  Detail:
 *  --------
 *  'cur_chain' should be non-NULL upon entry.  Get_TS_Op allows
 *  this code to be used for both hashed and unhashed node chains as
 *  each requires a different procedure for locating a node's timestamp.
 *
 *  Nodes are first pruned by timestamp validity.  If the node's timestamp
 *  is valid and a unification is possible, the state is saved, with
 *  cur_chain's sibling as the TSTN continuation, and we branch back to
 *  the major loop of the algorithm.  Otherwise the chain is searched to
 *  completion, exiting the block when cur_chain is NULL.
 */
#define SearchChain_UnifyWithConstant(Chain,Subterm,TS,Get_TS_Op) {	\
	Chain_NextValidTSTN(Chain,TS,Get_TS_Op);	\
	while ( IsNonNULL(Chain) ) {	\
		alt_chain = TSTN_Sibling(Chain);	\
		Chain_NextValidTSTN(alt_chain,TS,Get_TS_Op);	\
		symbol = TSTN_Symbol(Chain);	\
		TrieSymbol_Deref(symbol);	\
		if(isref(symbol)) {	\
			/*	\
			 * Either an unbound TrieVar or some unbound prolog var.	\
			 */	\
			CPStack_PushFrame(alt_chain);	\
			Bind_and_Conditionally_Trail((CPtr)symbol, Subterm);	\
			TermStackLog_PushFrame;	\
			Descend_Into_TST_and_Continue_Search;	\
		}	\
		else if(symbol == Subterm) {	\
			CPStack_PushFrame(alt_chain);	\
			TermStackLog_PushFrame;	\
			Descend_Into_TST_and_Continue_Search;	\
		}	\
		Chain = alt_chain;	\
	}	\
}

/*
 * Purpose:
 * -------
 *  From a given Time-Stamped Answer Trie, collect those answers with
 *  timestamps greater than a given value and which unify with a given
 *  template.  The unifying answers are returned in a chain of Answer
 *  List Nodes.
 *  Note that this algorithm relies on the Time Stamp Indices of the
 *  TST (which are reclaimed from the table when a subgoal completes).
 *
 * Method:
 * ------
 *  Backtrack through the entire TST, using the TimeStamp to prune paths.
 *
 * Nefarious Detail (not integral to general understanding)
 * ----------------
 *  Only when we succeed with a match do we push a subterm onto the
 *  tstTermStackLog.  This is because if we don't succeed, we backtrack,
 *  which would mean we pushed it onto the tstTermStackLog just to be
 *  popped off and stored back in the tstTermStack, and in fact back to
 *  the same location where it already resides (it wouldn't have had a
 *  chance to be overwritten).
 *
 *  When we do succeed, we would like to record the subterm just
 *  consumed, but not any bindings created as a result of the match.
 *  In the code, we push a CPF before doing any of this recording.
 *  However, the log info is, in fact, saved.  */
 
ALNptr tst_collect_relevant_answers(CTXTdeclc TSTNptr tstRoot, TimeStamp ts,
				    int numTerms, CPtr termsRev)
{
	/* numTerms -- size of Answer Template */
	/* termsRev -- Answer template (on heap) */
  
	ALNptr tstAnswerList; /* for collecting leaves to be returned */
	
	TSTNptr cur_chain;     /* main ptr for stepping through siblings; under
			    normal (non-hashed) circumstances, variable and
			    non-variable symbols will appear in the same
			    chain */
	TSTNptr alt_chain;     /* special case ptr used for stepping through
			      siblings while looking for a match with
			      subterm */
	TSTNptr parentTSTN;    /* the parent of TSTNs in cur_ and alt_chain */

	Cell subterm; /* the part of the term we are inspecting */
	Cell symbol;
	
	/* Check that a term was passed in
	   ------------------------------- */
	if(numTerms < 1)
		TST_Collection_Error("Called with < 1 terms",NoCleanupRequired);
  
	/* Initialize data structures
	   -------------------------- */
	TermStack_ResetTOS;
	TermStack_PushHighToLowVector(termsRev,numTerms);
	TermStackLog_ResetTOS;
	tstCPStack.top = tstCPStack.base;
	trail_base = top_of_trail;
	Save_and_Set_WAM_Registers;
	
	parentTSTN = tstRoot;
	cur_chain = TSTN_Child(tstRoot);
	tstAnswerList = NULL;
	symbol = 0; /* suppress compiler warning */
	
	/* Major loop of the algorithm
	   --------------------------- */
	
While_TSnotEmpty:
	while(!TermStack_IsEmpty) {
		TermStack_Pop(subterm);
		XSB_Deref(subterm);
		switch(cell_tag(subterm)) {
		
		/* SUBTERM IS A CONSTANT
		   --------------------- */
		case XSB_INT:
		case XSB_FLOAT:
		case XSB_STRING:
			/*
			 * NOTE: A Trie constant looks like a Prolog constant.
			 */
			if(IsHashHeader(cur_chain)) {
				symbol = EncodeTrieConstant(subterm);
				SetMatchAndUnifyChains(symbol,cur_chain,alt_chain);
				if(cur_chain != alt_chain) {
					SearchChain_ExactMatch(cur_chain,symbol,ts,alt_chain,
						TermStack_NOOP);
					cur_chain = alt_chain;
				}
				if(IsNULL(cur_chain))
					backtrack;
			}
			if(IsHashedNode(cur_chain))
				SearchChain_UnifyWithConstant(cur_chain,subterm,ts,
						TSTN_GetTSfromTSIN)
			else
				SearchChain_UnifyWithConstant(cur_chain,subterm,ts,TSTN_TimeStamp)
			break;
		} /* switch */
	} /* while */
	
	
	return NULL;
}

#endif /* TABLING */