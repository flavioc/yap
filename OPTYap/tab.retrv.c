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
#define ebreg ENV

#define Sys_Trail_Unwind(TR0) reset_trail(TR0)

static CPtr *trail_base;	/* ptr to topmost used Cell on the system Trail;
			       the beginning of our local trail for this
			       operation. */
				   
static CPtr *orig_trreg;
static CPtr orig_hreg;	/* Markers for noting original values of WAM */
static CPtr orig_hbreg;	/* registers so they can be reset upon exiting */
static CPtr orig_ebreg;

#define Restore_WAM_Registers	\
	trreg = orig_trreg;	\
	hreg = orig_hreg;	\
	hbreg = orig_hbreg;	\
	ebreg = orig_ebreg
	
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
	xsb_error(string);
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
  
	return NULL;
}

#endif /* TABLING */