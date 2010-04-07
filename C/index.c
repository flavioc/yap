/*************************************************************************
*									 *
*	 Yap Prolog 							 *
*									 *
*	Yap Prolog was developed at NCCUP - Universidade do Porto	 *
*									 *
* Copyright L.Damas, V.S.Costa and Universidade do Porto 1985-1997	 *
*									 *
**************************************************************************
*									 *
* File:		index.c							 *
* comments:	Indexing a Prolog predicate				 *
*									 *
* Last rev:     $Date: 2008-08-07 20:51:21 $,$Author: vsc $						 *
* $Log: not supported by cvs2svn $
* Revision 1.202  2008/07/11 17:02:07  vsc
* fixes by Bart and Tom: mostly libraries but nasty one in indexing
* compilation.
*
* Revision 1.201  2008/05/10 23:24:11  vsc
* fix threads and LU
*
* Revision 1.200  2008/04/16 17:16:47  vsc
* make static_clause only commit to a lause if it is sure that is the true
* clause. Otherwise, search for the clause.
*
* Revision 1.199  2008/04/14 21:20:35  vsc
* fixed a bug in static_clause (thanks to Jose Santos)
*
* Revision 1.198  2008/03/25 16:45:53  vsc
* make or-parallelism compile again
*
* Revision 1.197  2008/02/14 14:35:13  vsc
* fixes for indexing code.
*
* Revision 1.196  2008/01/30 10:35:43  vsc
* fix indexing in 64 bits (it would split ints from atoms :( ).
*
* Revision 1.195  2008/01/24 10:20:42  vsc
* clause should not try to discover who is fail.
*
* Revision 1.194  2008/01/24 00:11:59  vsc
* garbage collector was not asking for space.
* avoid 0 sized calls to mmap.
*
* Revision 1.193  2008/01/23 17:57:46  vsc
* valgrind it!
* enable atom garbage collection.
*
* Revision 1.192  2007/11/26 23:43:08  vsc
* fixes to support threads and assert correctly, even if inefficiently.
*
* Revision 1.191  2007/11/08 15:52:15  vsc
* fix some bugs in new dbterm code.
*
* Revision 1.190  2007/11/07 09:25:27  vsc
* speedup meta-calls
*
* Revision 1.189  2007/11/06 17:02:12  vsc
* compile ground terms away.
*
* Revision 1.188  2007/10/28 11:23:40  vsc
* fix overflow
*
* Revision 1.187  2007/09/22 08:38:05  vsc
* nb_ extra stuff plus an indexing overflow fix.
*
* Revision 1.186  2007/06/20 13:48:45  vsc
* fix bug in index emulator
*
* Revision 1.185  2007/05/02 11:01:37  vsc
* get rid of type punning warnings.
*
* Revision 1.184  2007/03/26 15:18:43  vsc
* debugging and clause/3 over tabled predicates would kill YAP.
*
* Revision 1.183  2007/03/21 23:23:46  vsc
* fix excessive trail cleaning in gc tr overflow.
*
* Revision 1.182  2007/01/28 14:26:36  vsc
* WIN32 support
*
* Revision 1.181  2007/01/08 08:27:19  vsc
* fix restore (Trevor)
* make indexing a bit faster on IDB
*
* Revision 1.180  2006/12/27 01:32:37  vsc
* diverse fixes
*
* Revision 1.179  2006/11/27 17:42:02  vsc
* support for UNICODE, and other bug fixes.
*
* Revision 1.178  2006/11/21 16:21:31  vsc
* fix I/O mess
* fix spy/reconsult mess
*
* Revision 1.177  2006/11/15 00:13:36  vsc
* fixes for indexing code.
*
* Revision 1.176  2006/11/08 01:53:08  vsc
* avoid generating suspensions on static code.
*
* Revision 1.175  2006/11/06 18:35:04  vsc
* 1estranha
*
* Revision 1.174  2006/10/25 02:31:07  vsc
* fix emulation of trust_logical
*
* Revision 1.173  2006/10/18 13:47:31  vsc
* index.c implementation of trust_logical was decrementing the wrong
* cp_tr
*
* Revision 1.172  2006/10/16 17:12:48  vsc
* fixes for threaded version.
*
* Revision 1.171  2006/10/11 14:53:57  vsc
* fix memory leak
* fix overflow handling
* VS: ----------------------------------------------------------------------
*
* Revision 1.170  2006/10/10 14:08:16  vsc
* small fixes on threaded implementation.
*
* Revision 1.169  2006/09/20 20:03:51  vsc
* improve indexing on floats
* fix sending large lists to DB
*
* Revision 1.168  2006/05/16 18:37:30  vsc
* WIN32 fixes
* compiler bug fixes
* extend interface
*
* Revision 1.167  2006/05/02 16:44:11  vsc
* avoid uninitialised memory at overflow.
*
* Revision 1.166  2006/05/02 16:39:06  vsc
* bug in indexing code
* fix warning messages for write.c
*
* Revision 1.165  2006/04/27 17:04:08  vsc
* don't use <= to compare with block top (libc may not have block header).
*
* Revision 1.164  2006/04/27 14:10:36  rslopes
* *** empty log message ***
*
* Revision 1.163  2006/04/20 15:28:08  vsc
* more graph stuff.
*
* Revision 1.162  2006/04/12 18:56:50  vsc
* fix bug in clause: a trust_me followed by a try should be implemented by
* reusing the choice-point.
*
* Revision 1.161  2006/04/05 00:16:54  vsc
* Lots of fixes (check logfile for details
*
* Revision 1.160  2006/03/24 17:13:41  rslopes
* New update to BEAM engine.
* BEAM now uses YAP Indexing (JITI)
*
* Revision 1.159  2006/03/22 20:07:28  vsc
* take better care of zombies
*
* Revision 1.158  2006/03/21 21:30:54  vsc
* avoid looking around when expanding for statics too.
*
* Revision 1.157  2006/03/21 19:20:34  vsc
* fix fix on index expansion
*
* Revision 1.156  2006/03/21 17:11:39  vsc
* prevent breakage
*
* Revision 1.155  2006/03/21 15:06:35  vsc
* fixes to handle expansion of dyn amic predicates more efficiently.
*
* Revision 1.154  2006/03/20 19:51:43  vsc
* fix indexing and tabling bugs
*
* Revision 1.153  2006/02/22 11:55:36  vsc
* indexing code would get confused about size of float/1, db_reference1.
*
* Revision 1.152  2006/02/19 02:55:46  vsc
* disable indexing on bigints
*
* Revision 1.151  2006/01/16 02:57:51  vsc
* fix bug with very large integers
* fix bug where indexing code was looking at code after a cut.
*
* Revision 1.150  2005/12/23 00:20:13  vsc
* updates to gprof
* support for __POWER__
* Try to saveregs before longjmp.
*
* Revision 1.149  2005/12/17 03:25:39  vsc
* major changes to support online event-based profiling
* improve error discovery and restart on scanner.
*
* Revision 1.148  2005/11/24 15:33:52  tiagosoares
* removed some compilation warnings related to the cut-c code
*
* Revision 1.147  2005/11/18 18:48:52  tiagosoares
* support for executing c code when a cut occurs
*
* Revision 1.146  2005/10/29 02:21:47  vsc
* people should be able to disable indexing.
*
* Revision 1.145  2005/09/08 22:06:44  rslopes
* BEAM for YAP update...
*
* Revision 1.144  2005/08/17 18:48:35  vsc
* fix bug in processing overflows of expand_clauses.
*
* Revision 1.143  2005/08/02 03:09:50  vsc
* fix debugger to do well nonsource predicates.
*
* Revision 1.142  2005/08/01 15:40:37  ricroc
* TABLING NEW: better support for incomplete tabling
*
* Revision 1.141  2005/07/19 16:54:20  rslopes
* fix for older compilers...
*
* Revision 1.140  2005/07/18 17:41:16  vsc
* Yap should respect single argument indexing.
*
* Revision 1.139  2005/07/06 19:33:53  ricroc
* TABLING: answers for completed calls can now be obtained by loading (new option) or executing (default) them from the trie data structure.
*
* Revision 1.138  2005/07/05 18:32:32  vsc
* ifix some wierd cases in indexing code:
* would not look at next argument
* problem with pvar as last clause (R Camacho).
*
* Revision 1.137  2005/06/04 07:27:34  ricroc
* long int support for tabling
*
* Revision 1.136  2005/06/03 08:26:32  ricroc
* float support for tabling
*
* Revision 1.135  2005/06/01 20:25:23  vsc
* == and \= should not need a choice-point in ->
*
* Revision 1.134  2005/06/01 16:42:30  vsc
* put switch_list_nl back
*
* Revision 1.133  2005/06/01 14:02:50  vsc
* get_rid of try_me?, retry_me? and trust_me? instructions: they are not
* significantly used nowadays.
*
* Revision 1.132  2005/05/31 20:04:17  vsc
* fix cleanup of expand_clauses: make sure we have everything with NULL afterwards.
*
* Revision 1.131  2005/05/31 19:42:27  vsc
* insert some more slack for indices in LU
* Use doubly linked list for LU indices so that updating is less cumbersome.
*
* Revision 1.130  2005/05/31 04:46:06  vsc
* fix expand_index on tabled code.
*
* Revision 1.129  2005/05/31 02:15:53  vsc
* fix SYSTEM_ERROR messages
*
* Revision 1.128  2005/05/30 05:26:49  vsc
* fix tabling
* allow atom gc again for now.
*
* Revision 1.127  2005/05/27 21:44:00  vsc
* Don't try to mess with sequences that don't end with a trust.
* A fix for the atom garbage collector actually ignore floats ;-).
*
* Revision 1.126  2005/05/25 18:58:37  vsc
* fix another bug in nth_instance, thanks to Pat Caldon
*
* Revision 1.125  2005/04/28 14:50:45  vsc
* clause should always deref before testing type
*
* Revision 1.124  2005/04/27 20:09:25  vsc
* indexing code could get confused with suspension points
* some further improvements on oveflow handling
* fix paths in Java makefile
* changs to support gibbs sampling in CLP(BN)
*
* Revision 1.123  2005/04/21 13:53:05  vsc
* fix bug with (var(X) -> being interpreted as var(X) by indexing code
*
* Revision 1.122  2005/04/10 04:01:12  vsc
* bug fixes, I hope!
*
* Revision 1.121  2005/04/07 17:48:54  ricroc
* Adding tabling support for mixed strategy evaluation (batched and local scheduling)
*   UPDATE: compilation flags -DTABLING_BATCHED_SCHEDULING and -DTABLING_LOCAL_SCHEDULING removed. To support tabling use -DTABLING in the Makefile or --enable-tabling in configure.
*   NEW: yap_flag(tabling_mode,MODE) changes the tabling execution mode of all tabled predicates to MODE (batched, local or default).
*   NEW: tabling_mode(PRED,MODE) changes the default tabling execution mode of predicate PRED to MODE (batched or local).
*
* Revision 1.120  2005/03/15 18:29:23  vsc
* fix GPL
* fix idb: stuff in coroutines.
*
* Revision 1.119  2005/03/04 20:30:12  ricroc
* bug fixes for YapTab support
*
* Revision 1.118  2005/03/01 22:25:08  vsc
* fix pruning bug
* make DL_MALLOC less enthusiastic about walking through buckets.
*
* Revision 1.117  2005/02/25 00:09:06  vsc
* fix fix, otherwise I'd remove two choice-points :-(.
*
* Revision 1.116  2005/02/24 21:46:39  vsc
* Improve error handling routine, trying to make it more robust.
* Improve hole handling in stack expansion
* Clause interrpeter was supposed to prune _trust_me
* Wrong messages for acos and atanh
*
* Revision 1.115  2005/02/21 16:50:00  vsc
* amd64 fixes
* library fixes
*
* Revision 1.114  2005/01/28 23:14:36  vsc
* move to Yap-4.5.7
* Fix clause size
*
* Revision 1.113  2005/01/15 05:21:36  vsc
* fix bug in clause emulator
*
* Revision 1.112  2004/12/28 22:20:35  vsc
* some extra bug fixes for trail overflows: some cannot be recovered that easily,
* some can.
*
* Revision 1.111  2004/12/21 17:17:15  vsc
* miscounting of variable-only clauses in groups might lead to bug in indexing
* code.
*
* Revision 1.110  2004/12/06 04:50:22  vsc
* fix bug in removing first clause of a try sequence (lu preds)
*
* Revision 1.109  2004/12/05 05:01:24  vsc
* try to reduce overheads when running with goal expansion enabled.
* CLPBN fixes
* Handle overflows when allocating big clauses properly.
*
* Revision 1.108  2004/11/19 22:08:42  vsc
* replace SYSTEM_ERROR by out OUT_OF_WHATEVER_ERROR whenever appropriate.
*
* Revision 1.107  2004/11/19 17:14:14  vsc
* a few fixes for 64 bit compiling.
*
* Revision 1.106  2004/11/18 22:32:36  vsc
* fix situation where we might assume nonextsing double initialisation of C predicates (use
* Hidden Pred Flag).
* $host_type was double initialised.
*
* Revision 1.105  2004/11/04 18:22:32  vsc
* don't ever use memory that has been freed (that was done by LU).
* generic fixes for WIN32 libraries
*
* Revision 1.104  2004/10/27 15:56:33  vsc
* bug fixes on memory overflows and on clauses :- fail being ignored by clause.
*
* Revision 1.103  2004/10/22 16:53:19  vsc
* bug fixes
*
* Revision 1.102  2004/10/04 18:56:19  vsc
* fixes for thread support
* fix indexing bug (serious)
*
* Revision 1.101  2004/09/30 21:37:41  vsc
* fixes for thread support
*
* Revision 1.100  2004/09/30 19:51:54  vsc
* fix overflow from within clause/2
*
* Revision 1.99  2004/09/27 20:45:03  vsc
* Mega clauses
* Fixes to sizeof(expand_clauses) which was being overestimated
* Fixes to profiling+indexing
* Fixes to reallocation of memory after restoring
* Make sure all clauses, even for C, end in _Ystop
* Don't reuse space for Streams
* Fix Stream_F on StreaNo+1
*
* Revision 1.98  2004/09/14 03:30:06  vsc
* make sure that condor version always grows trail!
*
* Revision 1.97  2004/09/03 03:11:09  vsc
* memory management fixes
*
* Revision 1.96  2004/08/27 20:18:52  vsc
* more small fixes
*
* Revision 1.95  2004/08/11 16:14:52  vsc
* whole lot of fixes:
*   - memory leak in indexing
*   - memory management in WIN32 now supports holes
*   - extend Yap interface, more support for SWI-Interface
*   - new predicate mktime in system
*   - buffer console I/O in WIN32
*
* Revision 1.94  2004/07/29 18:15:18  vsc
* fix severe bug in indexing of floating point numbers
*
* Revision 1.93  2004/07/23 19:01:14  vsc
* fix bad ref count in expand_clauses when copying indexing block
*
* Revision 1.92  2004/06/29 19:04:42  vsc
* fix multithreaded version
* include new version of Ricardo's profiler
* new predicat atomic_concat
* allow multithreaded-debugging
* small fixes
*
* Revision 1.91  2004/06/17 22:07:23  vsc
* bad bug in indexing code.
*
* Revision 1.90  2004/04/29 03:44:04  vsc
* fix bad suspended clause counter
*
* Revision 1.89  2004/04/27 15:03:43  vsc
* more fixes for expand_clauses
*
* Revision 1.88  2004/04/22 03:24:17  vsc
* trust_logical should protect the last clause, otherwise it cannot
* jump there.
*
* Revision 1.87  2004/04/21 04:01:53  vsc
* fix bad ordering when inserting second clause
*
* Revision 1.86  2004/04/20 22:08:23  vsc
* fixes for corourining
*
* Revision 1.85  2004/04/16 19:27:31  vsc
* more bug fixes
*
* Revision 1.84  2004/04/14 19:10:38  vsc
* expand_clauses: keep a list of clauses to expand
* fix new trail scheme for multi-assignment variables
*
* Revision 1.83  2004/04/07 22:04:04  vsc
* fix memory leaks
*
* Revision 1.82  2004/03/31 01:02:18  vsc
* if number of left-over < 1/5 keep list of clauses to expand around
* fix call to stack expander
*
* Revision 1.81  2004/03/25 02:19:10  pmoura
* Removed debugging line to allow compilation.
*
* Revision 1.80  2004/03/19 11:35:42  vsc
* trim_trail for default machine
* be more aggressive about try-retry-trust chains.
*    - handle cases where block starts with a wait
*    - don't use _killed instructions, just let the thing rot by itself.
*                                                                  *
*									 *
*************************************************************************/
#ifdef SCCS
static char     SccsId[] = "%W% %G%";
#endif

/*
 * This file compiles and removes the indexation code for the prolog compiler 
 *
 * Some remarks: *try_me always point to inside the code;
 * try always points to outside 
 *

 Algorithm:

 - fetch info on all clauses
 - if #clauses =1  return
 - compute groups:
    seq of variable only clauses
    seq: of one or more type instructions
         bound clauses
 - sort group
 - select constant
          --> type instructions
          --> count constants
          --> switch
	       for all arguments:
	       select new argument 

 */

#include "absmi.h"
#include "compile.h"
#include "index.h"
#ifdef DEBUG
#include "yapio.h"
#endif
#ifndef NULL
#define NULL (void *)0
#endif
#if HAVE_STRING_H
#include <string.h>
#endif
#ifdef CUT_C
#include "cut_c.h"
#endif

UInt STATIC_PROTO(do_index, (ClauseDef *,ClauseDef *,struct intermediates *,UInt,UInt,int,int,CELL *));
UInt STATIC_PROTO(do_compound_index, (ClauseDef *,ClauseDef *,Term *t,struct intermediates *,UInt,UInt,UInt,UInt,int,int,int,CELL *,int));
UInt STATIC_PROTO(do_dbref_index, (ClauseDef *,ClauseDef *,Term,struct intermediates *,UInt,UInt,int,int,CELL *));
UInt STATIC_PROTO(do_blob_index, (ClauseDef *,ClauseDef *,Term,struct intermediates *,UInt,UInt,int,int,CELL *,int));

static UInt
cleanup_sw_on_clauses(CELL larg, UInt sz, OPCODE ecls)
{
  if (larg & 1) {
    return sz;
  } else {
    yamop *xp = (yamop *)larg;
    if (xp->opc == ecls) {
      if (xp->u.sssllp.s3 == 1) {
	UInt nsz = sz + (UInt)(NEXTOP((yamop *)NULL,sssllp))+xp->u.sssllp.s1*sizeof(yamop *);
	LOCK(ExpandClausesListLock);
	if (ExpandClausesFirst == xp)
	  ExpandClausesFirst = xp->u.sssllp.snext;
	if (ExpandClausesLast == xp) {
	  ExpandClausesLast = xp->u.sssllp.sprev;
	}
	if (xp->u.sssllp.sprev) {
	  xp->u.sssllp.sprev->u.sssllp.snext = xp->u.sssllp.snext;
	}
	if (xp->u.sssllp.snext) {
	  xp->u.sssllp.snext->u.sssllp.sprev = xp->u.sssllp.sprev;
	}
	UNLOCK(ExpandClausesListLock);
#if DEBUG
	Yap_ExpandClauses--;
	Yap_expand_clauses_sz -= (UInt)(NEXTOP((yamop *)NULL,sssllp))+xp->u.sssllp.s1*sizeof(yamop *);
#endif
	if (xp->u.sssllp.p->PredFlags & LogUpdatePredFlag) {
	  Yap_LUIndexSpace_EXT -= (UInt)NEXTOP((yamop *)NULL,sssllp)+xp->u.sssllp.s1*sizeof(yamop *);
	} else
	  Yap_IndexSpace_EXT -= (UInt)(NEXTOP((yamop *)NULL,sssllp))+xp->u.sssllp.s1*sizeof(yamop *);
	Yap_FreeCodeSpace((char *)xp);
	return nsz;
      } else {
	xp->u.sssllp.s3--;
	return sz;
      }
    } else {
      return sz;
    }
  }
}

static UInt
recover_from_failed_susp_on_cls(struct intermediates *cint, UInt sz)
{
  /* we have to recover all allocated blocks,
     just follow the code through. */
  struct PSEUDO *cpc = cint->CodeStart;
  OPCODE ecls = Yap_opcode(_expand_clauses);
  UInt log_upd_pred = cint->CurrentPred->PredFlags & LogUpdatePredFlag;

  while (cpc) {
    switch(cpc->op) {
    case enter_lu_op:
      if (cpc->rnd4) {
	yamop *code_p = (yamop *)cpc->rnd4;
	yamop *first = code_p->u.Ills.l1;
	yamop *last = code_p->u.Ills.l2;
	while (first) {
	  yamop *next = first->u.OtaLl.n;
	  LogUpdClause *cl = first->u.OtaLl.d;
	  cl->ClRefCount--;
	  Yap_FreeCodeSpace((char *)first);
	  if (first == last) 
	    break;
	  first = next;
	}
      }
      cpc->rnd4 = Zero;
      break;
    case jump_v_op:
    case jump_nv_op:
      sz = cleanup_sw_on_clauses(cpc->rnd1, sz, ecls);
      break;
    case switch_on_type_op:
      {
	TypeSwitch *type_sw = (TypeSwitch *)(cpc->arnds);
	sz = cleanup_sw_on_clauses(type_sw->PairEntry, sz, ecls);
	sz = cleanup_sw_on_clauses(type_sw->ConstEntry, sz, ecls);
	sz = cleanup_sw_on_clauses(type_sw->FuncEntry, sz, ecls);
	sz = cleanup_sw_on_clauses(type_sw->VarEntry, sz, ecls);
      }
      break;
    case switch_c_op:
    case if_c_op:
      {
	AtomSwiEntry *target = (AtomSwiEntry *)(cpc->rnd2);
	int cases = cpc->rnd1, i;

	for (i = 0; i < cases; i++) {
	  sz = cleanup_sw_on_clauses(target[i].u.Label, sz, ecls);
	}
	if (log_upd_pred) {
	  LogUpdIndex *lcl = ClauseCodeToLogUpdIndex(cpc->rnd2);
	  sz += sizeof(LogUpdIndex)+cases*sizeof(AtomSwiEntry);
	  Yap_LUIndexSpace_SW -= sizeof(LogUpdIndex)+cases*sizeof(AtomSwiEntry);
	  Yap_FreeCodeSpace((char *)lcl);
	} else {
	  StaticIndex *scl = ClauseCodeToStaticIndex(cpc->rnd2);
	  sz += sizeof(StaticIndex)+cases*sizeof(AtomSwiEntry);
	  Yap_IndexSpace_SW -= sizeof(StaticIndex)+cases*sizeof(AtomSwiEntry);
	  Yap_FreeCodeSpace((char *)scl);
	}
      }
      break;
    case switch_f_op:
    case if_f_op:
      {
	FuncSwiEntry *target = (FuncSwiEntry *)(cpc->rnd2);
	int cases = cpc->rnd1, i;
	
	for (i = 0; i < cases; i++) {
	  sz = cleanup_sw_on_clauses(target[i].u.Label, sz, ecls);
	}
	if (log_upd_pred) {
	  LogUpdIndex *lcl = ClauseCodeToLogUpdIndex(cpc->rnd2);
	  sz += sizeof(LogUpdIndex)+cases*sizeof(FuncSwiEntry);
	  Yap_LUIndexSpace_SW -= sizeof(LogUpdIndex)+cases*sizeof(FuncSwiEntry);
	  Yap_FreeCodeSpace((char *)lcl);
	} else {
	  StaticIndex *scl = ClauseCodeToStaticIndex(cpc->rnd2);
	  Yap_IndexSpace_SW -= sizeof(StaticIndex)+cases*sizeof(FuncSwiEntry);
	  sz += sizeof(StaticIndex)+cases*sizeof(FuncSwiEntry);
	  Yap_FreeCodeSpace((char *)scl);
	}
      }
      break;
    default:
      break;
    }
    cpc = cpc->nextInst;
  }
  if (cint->code_addr) {
    Yap_FreeCodeSpace((char *)cint->code_addr);
    cint->code_addr = NULL;
  }
  return sz;
}


static inline int
smaller(Term t1, Term t2)
{
  CELL tg1 = LowTagOf(t1), tg2 = LowTagOf(t2);
  if (tg1 == tg2) {
    return t1 < t2;
  } else
    return tg1 < tg2;
}

static inline int
smaller_or_eq(Term t1, Term t2)
{
  CELL tg1 = LowTagOf(t1), tg2 = LowTagOf(t2);
  if (tg1 == tg2) {
    return t1 <= t2;
  } else
    return tg1 < tg2;
}

static inline void
clcpy(ClauseDef *d, ClauseDef *s)
{
  memcpy((void *)d, (void *)s, sizeof(ClauseDef));
}

static void
insort(ClauseDef base[], CELL *p, CELL *q, int my_p)
{
  CELL *j;

  if (my_p) {
    p[1] = p[0];
    for (j = p; j < q; j += 2) {
      Term key;
      Int off = *j;
      CELL *i;

      key = base[off].Tag;
      i = j+1;
    
      /* we are at offset 1 */
      while (i > p+1 && smaller(key,base[i[-2]].Tag)) {
	i[0] = i[-2];
	i -= 2;
      }
      i[0] = off;
    }
  } else {
    for (j = p+2; j < q; j += 2) {
      Term key;
      Int off = *j;
      CELL *i;

      key = base[off].Tag;
      i = j;
    
      /* we are at offset 1 */
      while (i > p && smaller(key,base[i[-2]].Tag)) {
	i[0] = i[-2];
	i -= 2;
      }
      i[0] = off;
    }
  }
}


/* copy to a new list of terms */
static
void msort(ClauseDef *base, CELL *pt, Int size, int my_p)
{

  if (size > 2) {
    Int half_size = size / 2;
    CELL *pt_left, *pt_right, *end_pt, *end_pt_left;
    int left_p, right_p;

    if (size < 50) {
       insort(base, pt, pt+2*size, my_p);
       return;
    }
    pt_right = pt + half_size*2;
    left_p = my_p^1;
    right_p = my_p;
    msort(base, pt, half_size, left_p);
    msort(base, pt_right, size-half_size, right_p);
    /* now implement a simple merge routine */
    
    /* pointer to after the end of the list */
    end_pt = pt + 2*size;
    /* pointer to the element after the last element to the left */
    end_pt_left = pt+half_size*2;
    /* where is left list */
    pt_left = pt+left_p;
    /* where is right list */
    pt_right += right_p;
    /* where is new list */
    pt += my_p;
    /* while there are elements in the left or right vector do compares */
    while (pt_left < end_pt_left && pt_right < end_pt) {
      /* if the element to the left is larger than the one to the right */
      if (smaller_or_eq(base[pt_left[0]].Tag, base[pt_right[0]].Tag)) {
	/* copy the one to the left */
	pt[0] = pt_left[0];
	/* and avance the two pointers */
	pt += 2;
	pt_left += 2;
      } else {
	/* otherwise, copy the one to the right */
	pt[0] = pt_right[0];
	pt += 2;
	pt_right += 2;
      }
    }
    /* if any elements were left in the left vector just copy them */
    while (pt_left < end_pt_left) {
      pt[0] = pt_left[0];
      pt += 2;
      pt_left += 2;
    }
    /* if any elements were left in the right vector
       and they are in the wrong place, just copy them */
    if (my_p != right_p) {
      while(pt_right < end_pt) {
	pt[0] = pt_right[0];
	pt += 2;
	pt_right += 2;
      }
    }
  } else {
    if (size > 1 && smaller(base[pt[2]].Tag,base[pt[0]].Tag)) {
      CELL t = pt[2];
      pt[2+my_p] = pt[0];
      pt[my_p] = t;
    } else if (my_p) {
      pt[1] = pt[0];
      if (size > 1)
	pt[3] = pt[2];
    }
  }
}

static void
copy_back(ClauseDef *dest, CELL *pt, int max) {
  /* first need to say that we had no need to make a copy */
  int i;
  CELL *tmp = pt;
  for (i=0; i < max; i++) {
    if (*tmp != i) {
      ClauseDef cl;
      int j = i;
      CELL *pnt = tmp;

      /* found a chain */
      /* make a backup copy */
      clcpy(&cl, dest+i);
      do {
	/* follow the chain */
	int k = *pnt;

	*pnt = j;
	/*	printf("i=%d, k = %d, j = %d\n",i,j,k); */
	if (k == i) {
	  clcpy(dest+j, &cl);
	  break;
	} else {
	  clcpy(dest+j, dest+k);
	}
	pnt = pt+2*k;
	j = k;
      } while (TRUE);
    }
    /* we don't need to do swap */
    tmp += 2;
  }
}

/* sort a group of clauses by using their tags */
static void
sort_group(GroupDef *grp, CELL *top, struct intermediates *cint)
{
  int max = (grp->LastClause-grp->FirstClause)+1, i;
  CELL *pt = top;

  while (top+2*max > (CELL *)Yap_TrailTop) {
#if USE_SYSTEM_MALLOC
    Yap_Error_Size = 2*max*sizeof(CELL);
    /* grow stack */
    save_machine_regs();
    longjmp(cint->CompilerBotch,4);
#else
    if (!Yap_growtrail(2*max*CellSize, TRUE)) {
      save_machine_regs();
      longjmp(cint->CompilerBotch,4);
      return;
    }
#endif
  }
  /* initialise vector */
  for (i=0; i < max; i++) {
    *pt = i;
    pt += 2;
  }
#define M_EVEN  0
  msort(grp->FirstClause, top, max, M_EVEN);
  copy_back(grp->FirstClause, top, max);
}

/* add copy to register stack for original reg */
static int
init_regcopy(wamreg regs[MAX_REG_COPIES], wamreg copy)
{
  regs[0] = copy;
  return 1;
}

/* add copy to register stack for original reg */
static int
is_regcopy(wamreg regs[MAX_REG_COPIES], int regs_count, wamreg copy)
{
  int i = 0;
  while (i < regs_count) {
    if (regs[i] == copy) {
      return TRUE;
    }
    i++;
  }
  /* this copy had overflowed, or it just was not there */
  return FALSE;
}

/* add copy to register stack for original reg */
static int
delete_regcopy(wamreg regs[MAX_REG_COPIES], int regs_count, wamreg copy)
{
  int i = 0;
  while (i < regs_count) {
    if (regs[i] == copy) {
      /* we found it */
      regs[i] = regs[regs_count-1];
      return regs_count-1;
    }
    i++;
  }
  /* this copy had overflowed, or it just was not there */
  return regs_count;
}

/* add copy to register stack for original reg */
static int
add_regcopy(wamreg regs[MAX_REG_COPIES], int regs_count, Int source, Int copy)
{
  int i = 0;
  while (i < regs_count) {
    if (regs[i] == source) {
      /* we found it, add new element as last element */
      if (regs_count == MAX_REG_COPIES) {
	return regs_count;
      }
      regs[regs_count] = copy;
      return regs_count+1;
    }
    i++;
  }
  /* be careful: we may overwrite an existing copy */
  return delete_regcopy(regs, regs_count, copy);
}



/* add copy to register stack for original reg */
inline static int
link_regcopies(wamreg regs[MAX_REG_COPIES], int regs_count, Int c1, Int c2)
{
  int i;
  for (i=0; i<regs_count; i++) {
    if (regs[i] == c1) {
      return add_regcopy(regs, regs_count, c1, c2);
    }
    if (regs[i] == c2) {
      return add_regcopy(regs, regs_count, c2, c1);
    }
  }
  /* this copy could not be found */
  regs_count = delete_regcopy(regs, regs_count, c1);
  return delete_regcopy(regs, regs_count, c2);
}

/* Restores a prolog clause, in its compiled form */
#if YAPOR
static int 
has_cut(yamop *pc)
/*
 * Cl points to the start of the code, IsolFlag tells if we have a single
 * clause for this predicate or not 
 */
{
  do {
    op_numbers op = Yap_op_from_opcode(pc->opc);
    switch (op) {
    case _unify_idb_term:
    case _copy_idb_term:
    case _Ystop:
    case _Nstop:
      return FALSE;
      /* instructions type ld */
    case _cut:
    case _cut_t:
    case _cut_e:
    case _p_cut_by_y:
    case _p_cut_by_x:
    case _commit_b_y:
    case _commit_b_x:
      return TRUE;
    case _try_me:
    case _retry_me:
    case _trust_me:
    case _profiled_retry_me:
    case _profiled_trust_me:
    case _count_retry_me:
    case _count_trust_me:
    case _spy_or_trymark:
    case _try_and_mark:
    case _profiled_retry_and_mark:
    case _count_retry_and_mark:
    case _retry_and_mark:
    case _try_clause:
    case _retry:
    case _trust:
#ifdef YAPOR
    case _getwork:
    case _getwork_seq:
    case _sync:
#endif /* YAPOR */
#ifdef TABLING
    case _table_load_answer:
    case _table_load_cons_answer:
    case _table_try_answer:
    case _table_try_ground_answer:
    case _table_try_single:
    case _table_try_me:
    case _table_retry_me:
    case _table_trust_me:
    case _table_try:
    case _table_retry:
    case _table_trust:
    case _table_answer_resolution:
    case _table_completion:
    case _table_run_completed:
#endif /* TABLING */
      pc = NEXTOP(pc,Otapl);
      break;
      /* instructions type Ills */
    case _enter_lu_pred:
      pc = pc->u.Ills.l1;
      break;
    case _execute:
    case _dexecute:
    case _execute_cpred:
      pc = NEXTOP(pc,pp);
      break;
    case _native_me:
      pc = NEXTOP(pc,aFlp);
      break;
      /* instructions type l */
    case _enter_profiling:
    case _count_call:
    case _retry_profiled:
    case _count_retry:
    case _jump:
    case _move_back:
    case _skip:
    case _jump_if_var:
    case _try_in:
    case _try_clause2:
    case _try_clause3:
    case _try_clause4:
    case _retry2:
    case _retry3:
    case _retry4:
    case _p_eq:
    case _p_dif:
      pc = NEXTOP(pc,l);
      break;
    case _jump_if_nonvar:
      pc = NEXTOP(pc,xll);
      break;
      /* instructions type EC */
    case _alloc_for_logical_pred:
      pc = NEXTOP(pc,L);
      break;
      /* instructions type e */
    case _lock_lu:
    case _unlock_lu:
    case _trust_fail:
    case _op_fail:
    case _allocate:
    case _write_void:
    case _write_list:
    case _write_l_list:
#if !defined(YAPOR)
    case _or_last:
#endif /* !YAPOR */
    case _pop:
    case _index_pred:
    case _lock_pred:
#if THREADS
    case _thread_local:
#endif
    case _expand_index:
    case _undef_p:
    case _spy_pred:
    case _p_equal:
    case _p_functor:
    case _p_execute_tail:
    case _index_dbref:
    case _index_blob:
    case _index_long:
#ifdef YAPOR
    case _getwork_first_time:
#endif /* YAPOR */
#ifdef TABLING
    case _trie_do_null:
    case _trie_trust_null:
    case _trie_try_null:
    case _trie_retry_null:
    case _trie_do_null_in_new_pair:
    case _trie_trust_null_in_new_pair:
    case _trie_try_null_in_new_pair:
    case _trie_retry_null_in_new_pair:
    case _trie_do_var:
    case _trie_trust_var:
    case _trie_try_var:
    case _trie_retry_var:
    case _trie_do_var_in_new_pair:
    case _trie_trust_var_in_new_pair:
    case _trie_try_var_in_new_pair:
    case _trie_retry_var_in_new_pair:
    case _trie_do_val:
    case _trie_trust_val:
    case _trie_try_val:
    case _trie_retry_val:
    case _trie_do_val_in_new_pair:
    case _trie_trust_val_in_new_pair:
    case _trie_try_val_in_new_pair:
    case _trie_retry_val_in_new_pair:
    case _trie_do_atom:
    case _trie_trust_atom:
    case _trie_try_atom:
    case _trie_retry_atom:
    case _trie_do_atom_in_new_pair:
    case _trie_trust_atom_in_new_pair:
    case _trie_try_atom_in_new_pair:
    case _trie_retry_atom_in_new_pair:
    case _trie_do_pair:
    case _trie_trust_pair:
    case _trie_try_pair:
    case _trie_retry_pair:
    case _trie_do_struct:
    case _trie_trust_struct:
    case _trie_try_struct:
    case _trie_retry_struct:
    case _trie_do_struct_in_new_pair:
    case _trie_trust_struct_in_new_pair:
    case _trie_try_struct_in_new_pair:
    case _trie_retry_struct_in_new_pair:
    case _trie_do_extension:
    case _trie_trust_extension:
    case _trie_try_extension:
    case _trie_retry_extension:
    case _trie_do_float:
    case _trie_trust_float:
    case _trie_try_float:
    case _trie_retry_float:
    case _trie_do_long:
    case _trie_trust_long:
    case _trie_try_long:
    case _trie_retry_long:
    case _trie_do_hash:
    case _trie_retry_hash:
    case _trie_try_long_int:
    case _trie_retry_long_int:
    case _trie_trust_long_int:
    case _trie_do_long_int:
    case _trie_try_float_val:
    case _trie_retry_float_val:
    case _trie_trust_float_val:
    case _trie_do_float_val:
#endif /* TABLING */
      pc = NEXTOP(pc,e);
      break;
    case _expand_clauses:
      pc = NEXTOP(pc,sssllp);
      break;
      /* instructions type x */
    case _save_b_x:
    case _get_list:
    case _put_list:
    case _write_x_var:
    case _write_x_val:
    case _write_x_loc:
      pc = NEXTOP(pc,x);
      break;
      /* instructions type xl */
    case _p_atom_x:
    case _p_atomic_x:
    case _p_integer_x:
    case _p_nonvar_x:
    case _p_number_x:
    case _p_var_x:
    case _p_db_ref_x:
    case _p_primitive_x:
    case _p_compound_x:
    case _p_float_x:
      pc = NEXTOP(pc,xl);
      break;
      /* instructions type y */
    case _save_b_y:
    case _write_y_var:
    case _write_y_val: 
    case _write_y_loc:
      pc = NEXTOP(pc,y);
      break;
      /* instructions type yl */
    case _p_atom_y:
    case _p_atomic_y:
    case _p_integer_y:
    case _p_nonvar_y:
    case _p_number_y:
    case _p_var_y:
    case _p_db_ref_y:
    case _p_primitive_y:
    case _p_compound_y:
    case _p_float_y:
      pc = NEXTOP(pc,yl);
      break;
      /* instructions type sbpp */
    case _p_execute:
      pc = NEXTOP(pc,Osbmp);
      break;
    case _p_execute2:
    case _fcall:
    case _call:
    case _call_cpred:
    case _call_usercpred:
      pc = NEXTOP(pc,Osbpp);
      break;
      /* instructions type sblp */
#ifdef YAPOR
    case _or_last:
#endif /* YAPOR */
    case _either:
    case _or_else:
      pc = NEXTOP(pc,Osblp);
      break;
      /* instructions type xx */
    case _get_x_var:
    case _get_x_val:
    case _glist_valx:
    case _gl_void_varx:
    case _gl_void_valx:
    case _put_x_var:
    case _put_x_val:
      pc = NEXTOP(pc,xx);
      break;
    case _put_xx_val:
      pc = NEXTOP(pc,xxxx);
      break;
      /* instructions type yx */
    case _get_y_var:
    case _get_y_val:
    case _put_y_var:
    case _put_y_val:
    case _put_unsafe:
    case _glist_valy:
    case _gl_void_vary:
    case _gl_void_valy:
      pc = NEXTOP(pc,yx);
      break;
      /* instructions type xd */
    case _get_float:
    case _put_float:
      pc = NEXTOP(pc,xd);
      break;
      /* instructions type xi */
    case _get_longint:
    case _put_longint:
      pc = NEXTOP(pc,xi);
      break;
      /* instructions type xc */
    case _get_atom:
    case _put_atom:
    case _get_bigint:
    case _get_dbterm:
      pc = NEXTOP(pc,xc);
      break;
      /* instructions type cc */
    case _get_2atoms:
      pc = NEXTOP(pc,cc);
      break;
      /* instructions type ccc */
    case _get_3atoms:
      pc = NEXTOP(pc,ccc);
      break;
      /* instructions type cccc */
    case _get_4atoms:
      pc = NEXTOP(pc,cccc);
      break;
      /* instructions type ccccc */
    case _get_5atoms:
      pc = NEXTOP(pc,ccccc);
      break;
      /* instructions type cccccc */
    case _get_6atoms:
      pc = NEXTOP(pc,cccccc);
      break;
      /* instructions type xfa */
    case _get_struct:
    case _put_struct:
      pc = NEXTOP(pc,xfa);
      break;
      /* instructions type yx */
      /* instructions type ox */
    case _unify_x_var:
    case _unify_x_var_write:
    case _unify_l_x_var:
    case _unify_l_x_var_write:
    case _unify_x_val_write:
    case _unify_x_val:
    case _unify_l_x_val_write:
    case _unify_l_x_val:
    case _unify_x_loc_write:
    case _unify_x_loc:
    case _unify_l_x_loc_write:
    case _unify_l_x_loc:
    case _save_pair_x_write:
    case _save_pair_x:
    case _save_appl_x_write:
    case _save_appl_x:
      pc = NEXTOP(pc,ox);
      break;
      /* instructions type oxx */
    case _unify_x_var2:
    case _unify_x_var2_write:
    case _unify_l_x_var2:
    case _unify_l_x_var2_write:
      pc = NEXTOP(pc,oxx);
      break;
      /* instructions type oy */
    case _unify_y_var:
    case _unify_y_var_write:
    case _unify_l_y_var:
    case _unify_l_y_var_write:
    case _unify_y_val_write:
    case _unify_y_val:
    case _unify_l_y_val_write:
    case _unify_l_y_val:
    case _unify_y_loc_write:
    case _unify_y_loc:
    case _unify_l_y_loc_write:
    case _unify_l_y_loc:
    case _save_pair_y_write:
    case _save_pair_y:
    case _save_appl_y_write:
    case _save_appl_y:
      pc = NEXTOP(pc,oy);
      break;
      /* instructions type o */
    case _unify_void_write:
    case _unify_void:
    case _unify_l_void_write:
    case _unify_l_void:
    case _unify_list_write:
    case _unify_list:
    case _unify_l_list_write:
    case _unify_l_list:
      pc = NEXTOP(pc,o);
      break;
      /* instructions type os */
    case _unify_n_voids_write:
    case _unify_n_voids:
    case _unify_l_n_voids_write:
    case _unify_l_n_voids:
      pc = NEXTOP(pc,os);
      break;
      /* instructions type od */
    case _unify_float:
    case _unify_l_float:
    case _unify_float_write:
    case _unify_l_float_write:
      pc = NEXTOP(pc,od);
      break;
      /* instructions type d */
    case _write_float:
      pc = NEXTOP(pc,d);
      break;
      /* instructions type oi */
    case _unify_longint:
    case _unify_l_longint:
    case _unify_longint_write:
    case _unify_l_longint_write:
      pc = NEXTOP(pc,oi);
      break;
      /* instructions type i */
    case _write_longint:
      pc = NEXTOP(pc,i);
      break;
      /* instructions type oc */
    case _unify_atom_write:
    case _unify_atom:
    case _unify_l_atom_write:
    case _unify_l_atom:
    case _unify_bigint:
    case _unify_l_bigint:
    case _unify_dbterm:
    case _unify_l_dbterm:
      pc = NEXTOP(pc,oc);
      break;
      /* instructions type osc */
    case _unify_n_atoms_write:
    case _unify_n_atoms:
      pc = NEXTOP(pc,osc);
      break;
      /* instructions type of */
    case _unify_struct_write:
    case _unify_struct:
    case _unify_l_struc_write:
    case _unify_l_struc:
      pc = NEXTOP(pc,ofa);
      break;
      /* instructions type s */
    case _write_n_voids:
    case _pop_n:
#ifdef TABLING
    case _table_new_answer:
#endif /* TABLING */
      pc = NEXTOP(pc,s);
      break;
      /* instructions type ps */
   case _write_atom:
      pc = NEXTOP(pc,c);
      break;
      /* instructions type p */
    case _user_switch:
      return FALSE;
    case _deallocate:
    case _procceed:
      pc = NEXTOP(pc,p);
      break;
      /* instructions type sc */
   case _write_n_atoms:
      pc = NEXTOP(pc,sc);
      break;
      /* instructions type f */
   case _write_struct:
   case _write_l_struc:
      pc = NEXTOP(pc,fa);
      break;
      /* instructions type slp */
    case _call_c_wfail:
      pc = NEXTOP(pc,slp);
      break;
      /* instructions type lds */
    case _try_c:
    case _try_userc:
      pc = NEXTOP(pc,OtapFs);
      break;
      /* instructions type OtaLl,OtILl */
    case _try_logical:
    case _retry_logical:
    case _count_retry_logical:
    case _profiled_retry_logical:
      pc = pc->u.OtaLl.n;
      break;
    case _trust_logical:
    case _count_trust_logical:
    case _profiled_trust_logical:
      pc = pc->u.OtILl.n;
      break;
    case _retry_c:
    case _retry_userc:
      pc = NEXTOP(pc,OtapFs);
      break;
      /* instructions type llll */
    case _switch_on_type:
      return FALSE;
      break;
    case _switch_list_nl:
      return FALSE;
      break;
    case _switch_on_arg_type:
      return FALSE;
      break;
    case _switch_on_sub_arg_type:
      return FALSE;
      /* instructions type lll */
      /* instructions type cll */
    case _if_not_then:
      return FALSE;
      /* instructions type sl */
    case _switch_on_func:
    case _switch_on_cons:
    case _go_on_func:
    case _go_on_cons:
    case _if_func:
    case _if_cons:
      return FALSE;
      /* instructions type xxx */
    case _p_plus_vv:
    case _p_minus_vv:
    case _p_times_vv:
    case _p_div_vv:
    case _p_and_vv:
    case _p_or_vv:
    case _p_sll_vv:
    case _p_slr_vv:
    case _p_arg_vv:
    case _p_func2s_vv:
    case _p_func2f_xx:
      pc = NEXTOP(pc,xxx);
      break;
      /* instructions type xxn */
    case _p_plus_vc:
    case _p_minus_cv:
    case _p_times_vc:
    case _p_div_cv:
    case _p_and_vc:
    case _p_or_vc:
    case _p_sll_vc:
    case _p_slr_vc:
    case _p_func2s_vc:
      pc = NEXTOP(pc,xxn);
      break;
    case _p_div_vc:
    case _p_sll_cv:
    case _p_slr_cv:
    case _p_arg_cv:
      pc = NEXTOP(pc,xxn);
      break;
    case _p_func2s_cv:
      pc = NEXTOP(pc,xxn);
      break;
      /* instructions type xxy */
    case _p_func2f_xy:
      pc = NEXTOP(pc,xxy);
      break;
      /* instructions type yxx */
    case _p_plus_y_vv:
    case _p_minus_y_vv:
    case _p_times_y_vv:
    case _p_div_y_vv:
    case _p_and_y_vv:
    case _p_or_y_vv:
    case _p_sll_y_vv:
    case _p_slr_y_vv:
    case _p_arg_y_vv:
    case _p_func2s_y_vv:
    case _p_func2f_yx:
      pc = NEXTOP(pc,yxx);
      break;
      /* instructions type yyx */
    case _get_yy_var:
    case _put_y_vals:
      pc = NEXTOP(pc,yyxx);
      break;
      /* instructions type yyx */
    case _p_func2f_yy:
      pc = NEXTOP(pc,yyx);
      break;
      /* instructions type yxn */
    case _p_plus_y_vc:
    case _p_minus_y_cv:
    case _p_times_y_vc:
    case _p_div_y_vc:
    case _p_div_y_cv:
    case _p_and_y_vc:
    case _p_or_y_vc:
    case _p_sll_y_vc:
    case _p_slr_y_vc:
    case _p_func2s_y_vc:
      pc = NEXTOP(pc,yxn);
      break;
      /* instructions type yxn */
    case _p_sll_y_cv:
    case _p_slr_y_cv:
    case _p_arg_y_cv:
      pc = NEXTOP(pc,yxn);
      break;
      /* instructions type yxn */
    case _p_func2s_y_cv:
      pc = NEXTOP(pc,yxn);
      break;
      /* instructions type plxxs */
    case _call_bfunc_xx:
      pc = NEXTOP(pc,plxxs);
      break;
      /* instructions type plxys */
    case _call_bfunc_yx:
    case _call_bfunc_xy:
      pc = NEXTOP(pc,plxys);
      break;
    case _call_bfunc_yy:
      pc = NEXTOP(pc,plyys);
      break;
    }
  } while (TRUE);
}
#else
#define has_cut(pc) 0
#endif /* YAPOR */

static void 
add_info(ClauseDef *clause, UInt regno)
{
  wamreg myregs[MAX_REG_COPIES];
  int nofregs;
  yamop *cl;
  
  nofregs = init_regcopy(myregs, Yap_regnotoreg(regno));
  cl = clause->CurrentCode;
#include "findclause.h"
}

static void 
add_head_info(ClauseDef *clause, UInt regno)
{
  wamreg iarg = Yap_regnotoreg(regno);

  yamop *cl = clause->CurrentCode;
#include "headclause.h"
}

static void 
move_next(ClauseDef *clause, UInt regno)
{
  yamop *cl = clause->CurrentCode;
  wamreg wreg = Yap_regnotoreg(regno);
  op_numbers op = Yap_op_from_opcode(cl->opc);

  switch (op) {
  case _native_me:
    return;
  case _p_db_ref_x:
  case _p_float_x:
    if (wreg == cl->u.xl.x) {
      clause->CurrentCode = NEXTOP(cl,xl);
    }	
    return;
  case _get_list:
    if (wreg == cl->u.x.x) {
      clause->CurrentCode = NEXTOP(cl,x);
    }	
    return;
  case _glist_valx:
  case _gl_void_vary:
  case _gl_void_valy:
  case _gl_void_varx:
  case _gl_void_valx:
  case _glist_valy:
    return;
  case _get_atom:
    if (wreg == cl->u.xc.x) {
      clause->CurrentCode = NEXTOP(cl,xc);
    }	
    return;
  case _get_2atoms:
    return;
  case _get_3atoms:
    return;
  case _get_4atoms:
    return;
  case _get_5atoms:
    return;
  case _get_6atoms:
    return;
    /*
      matching is not guaranteed:
  case _get_float:
  case _get_longint:
  case _get_bigint:
    */
  case _get_struct:
    if (wreg == cl->u.xfa.x) {
      clause->CurrentCode = NEXTOP(cl,xfa);
    }	
  default:
    clause->CurrentCode = clause->Code;
    return;
  }
}

static void
add_arg_info(ClauseDef *clause, PredEntry *ap, UInt argno)
{
  yamop *cl;
  if (ap->ModuleOfPred == IDB_MODULE) {
    cl = clause->Code;
  } else {
    cl = clause->u.WorkPC;
  }
  while (TRUE) {
    op_numbers op = Yap_op_from_opcode(cl->opc);
    switch (op) {
    case _glist_valx:
      if (argno == 1) {
	clause->Tag = (CELL)NULL;
	return;
      }
      argno--;
      cl = NEXTOP(cl,xx);
      break;
    case _gl_void_vary:
    case _gl_void_valy:
    case _gl_void_varx:
    case _gl_void_valx:
      clause->Tag = (CELL)NULL;
      return;
    case _glist_valy:
      if (argno == 1) {
	clause->Tag = (CELL)NULL;
	return;
      }
      argno = 2;
      cl = NEXTOP(cl,yx);
      break;
    case _unify_l_x_var:
    case _unify_l_x_val:
    case _unify_l_x_loc:
    case _unify_x_var:
    case _unify_x_val:
    case _unify_x_loc:
      if (argno == 1) {
	clause->Tag = (CELL)NULL;
	return;
      }
      argno--;
    case _unify_l_x_var_write:
    case _unify_l_x_val_write:
    case _unify_l_x_loc_write:
    case _unify_x_var_write:
    case _unify_x_val_write:
    case _unify_x_loc_write:
      cl = NEXTOP(cl,ox);
      break;
    case _save_pair_x_write:
    case _save_pair_x:
    case _save_appl_x_write:
    case _save_appl_x:
      cl = NEXTOP(cl,ox);
      break;
    case _unify_l_x_var2:
    case _unify_x_var2:
      if (argno == 1 || argno == 2) {
	clause->Tag = (CELL)NULL;
	return;
      }
      argno -= 2;
    case _unify_l_x_var2_write:
    case _unify_x_var2_write:
      cl = NEXTOP(cl,oxx);
      break;
    case _unify_y_var:
    case _unify_y_val:
    case _unify_y_loc:
    case _unify_l_y_var:
    case _unify_l_y_val:
    case _unify_l_y_loc:
      /* we're just done with the head of a list, but there
	 is nothing inside.
       */
      if (argno == 1) {
	clause->Tag = (CELL)NULL;
	return;
      }
      argno--;
    case _unify_y_var_write:
    case _unify_y_val_write:
    case _unify_y_loc_write:
    case _unify_l_y_var_write:
    case _unify_l_y_val_write:
    case _unify_l_y_loc_write:
      cl = NEXTOP(cl,oy);
      break;
    case _save_pair_y_write:
    case _save_pair_y:
    case _save_appl_y_write:
    case _save_appl_y:
      cl = NEXTOP(cl,oy);
      break;
    case _unify_l_void:
    case _unify_void:
      if (argno == 1) {
	clause->Tag = (CELL)NULL;
	return;
      }
      argno--;
    case _unify_l_void_write:
    case _unify_void_write:
      cl = NEXTOP(cl,o);
      break;
    case _unify_list:
    case _unify_l_list:
      if (argno == 1) {
	clause->Tag = AbsPair(NULL);
	clause->u.WorkPC = NEXTOP(cl,o);
	return;
      }
      argno += 1; /* 2-1: have two extra arguments to skip */
    case _unify_list_write:
    case _unify_l_list_write:
      cl = NEXTOP(cl,o);
      break;
    case _unify_n_voids:
    case _unify_l_n_voids:
      if (argno <= cl->u.os.s) {
	clause->Tag = (CELL)NULL;
	return;
      }
      argno -= cl->u.os.s;
    case _unify_n_voids_write:
    case _unify_l_n_voids_write:
      cl = NEXTOP(cl,os);
      break;      
    case _unify_atom:
    case _unify_l_atom:
      if (argno == 1) {
	clause->Tag = cl->u.oc.c;
	return;
      }
      argno--;
    case _unify_atom_write:
    case _unify_l_atom_write:
      cl = NEXTOP(cl,oc);
      break;      
    case _unify_float_write:
    case _unify_l_float_write:
      cl = NEXTOP(cl,od);
      break;      
    case _unify_float:
    case _unify_l_float:
      if (argno == 1) {
	clause->Tag = AbsAppl((CELL *)FunctorDouble);
	clause->u.t_ptr = AbsAppl(cl->u.od.d);
	return;
      }
      cl = NEXTOP(cl,od);
      argno--;
      break;
    case _unify_longint:
    case _unify_l_longint:
      if (argno == 1) {
	clause->Tag = AbsAppl((CELL *)FunctorLongInt);
	clause->u.t_ptr = AbsAppl(cl->u.oi.i);
	return;
      }
      argno--;
      cl = NEXTOP(cl,oi);
      break;
    case _unify_bigint:
    case _unify_l_bigint:
      if (argno == 1) {
#ifdef USE_GMP
	clause->Tag = AbsAppl((CELL *)FunctorBigInt);
#else
	clause->Tag = AbsAppl((CELL *)FunctorLongInt);	
#endif
	clause->u.t_ptr = cl->u.oc.c;
	return;
      }
      argno--;
      break;
    case _unify_n_atoms:
      if (argno <= cl->u.osc.s) {
	clause->Tag = cl->u.osc.c;
	return;
      }
      argno -= cl->u.osc.s;
    case _unify_n_atoms_write:
      cl = NEXTOP(cl,osc);
      break;      
    case _unify_struct:
    case _unify_l_struc:
      if (argno == 1) {
	clause->Tag = AbsAppl((CELL *)cl->u.ofa.f);
	clause->u.WorkPC = NEXTOP(cl,ofa);
	return;
      }
      /* must skip next n arguments */
      argno += cl->u.ofa.a-1;
    case _unify_l_struc_write:
    case _unify_struct_write:
      cl = NEXTOP(cl,ofa);
      break;      
    case _pop:
      cl = NEXTOP(cl,e);
      break;            
    case _pop_n:
      cl = NEXTOP(cl,s);
      break;   
#ifdef BEAM
    case _run_eam:
      cl = NEXTOP(cl,os);
      break;
#endif   
    case _get_dbterm:
      cl = NEXTOP(cl,xc);
      break;      
    case _unify_dbterm:
    case _unify_l_dbterm:
      cl = NEXTOP(cl,oc);
      break;      
    case _unify_idb_term:
    case _copy_idb_term:
      {
	Term t = clause->u.c_sreg[argno];

	if (IsVarTerm(t)) {
	  clause->Tag = (CELL)NULL;
	} else if (IsApplTerm(t)) {
	  CELL *pt = RepAppl(t);

	  clause->Tag = AbsAppl((CELL *)pt[0]);
	  if (IsExtensionFunctor(FunctorOfTerm(t))) {
	    clause->u.t_ptr = t;
	  } else {
	    clause->u.c_sreg = pt;
	  }
	} else if (IsPairTerm(t)) {
	  CELL *pt = RepPair(t);

	  clause->Tag = AbsPair(NULL);
	  clause->u.c_sreg = pt-1;
	} else {
	  clause->Tag = t;
	}
      }
      return;
    default:
      return;
    }
  }
}

static void
skip_to_arg(ClauseDef *clause, PredEntry *ap, UInt argno, int at_point)
{
  yamop *cl;
  int done = FALSE;
  if (ap->ModuleOfPred == IDB_MODULE) {
    return;
  } else {
    cl = clause->CurrentCode;
  }

  if (!at_point) {
    clause->CurrentCode = clause->Code;
    return;
  }

  while (!done) {
    op_numbers op = Yap_op_from_opcode(cl->opc);
    switch (op) {
#ifdef BEAM
    case _run_eam:
	clause->CurrentCode = clause->Code;
	return;
#endif
    case _unify_void:
      if (argno == 1) {
	clause->CurrentCode = clause->Code;
	return;
      } else {
	argno--;
      }
    case _unify_void_write:
      cl = NEXTOP(cl,o);
      break;
    case _unify_list:
    case _unify_l_list:
    case _unify_atom:
    case _unify_l_atom:
      /*
	unification is not guaranteed
	case _unify_longint:
	case _unify_l_longint:
	case _unify_bigint:
	case _unify_l_bigint:
	case _unify_l_float:
      */
    case _unify_struct:
    case _unify_l_struc:
      if (cl == clause->u.WorkPC) {
	clause->CurrentCode = cl;
      } else {
	clause->CurrentCode = clause->Code;
      }
      return;
    case _unify_list_write:
    case _unify_l_list_write:
      cl = NEXTOP(cl,o);
      break;
    case _unify_n_voids:
    case _unify_l_n_voids:
      if (argno <= cl->u.os.s) {
	clause->CurrentCode = clause->Code;
	return;
      } else {
	argno -= cl->u.os.s;
      }
    case _unify_n_voids_write:
    case _unify_l_n_voids_write:
      cl = NEXTOP(cl,os);
      break;      
    case _unify_atom_write:
    case _unify_l_atom_write:
      cl = NEXTOP(cl,oc);
      break;      
    case _unify_float_write:
    case _unify_l_float_write:
      cl = NEXTOP(cl,od);
      break;      
    case _unify_l_struc_write:
    case _unify_struct_write:
      cl = NEXTOP(cl,ofa);
      break;      
    case _pop:
      cl = NEXTOP(cl,e);
      break;            
    case _pop_n:
      cl = NEXTOP(cl,s);
      break;      
    default:
      clause->CurrentCode = clause->Code;
      return;      
    }
  }
}


static UInt
groups_in(ClauseDef *min, ClauseDef *max, GroupDef *grp, struct intermediates *cint)
{
  UInt groups = 0;

  while(min <= max) {
    grp->FirstClause = min;
    grp->AtomClauses = 0;
    grp->PairClauses = 0;
    grp->StructClauses = 0;
    grp->TestClauses = 0;
    if (min->Tag == (_var+1)*sizeof(CELL)) {
      min++;
      continue;
    }
    /* only do this for the first clauses in a group */
    if (IsVarTerm(min->Tag)) {
      ClauseDef *clp = min+1;

      grp->VarClauses = 1;
      do {
	if (clp > max ||
	    !IsVarTerm(clp->Tag)) {
	  grp->LastClause = (min = clp)-1;
	  break;
	}
	if (clp->Tag != (_var+1)*sizeof(CELL))
	  grp->VarClauses++;
	clp++;
      } while (TRUE);
    } else {
      grp->VarClauses = 0;
      do {
      restart_loop:
	if (IsAtomTerm(min->Tag) || IsIntTerm(min->Tag)) {
	  grp->AtomClauses++;
	} else if (IsPairTerm(min->Tag)) {
	  grp->PairClauses++;
	} else if (IsApplTerm(min->Tag)) {
	  grp->StructClauses++;
	} else {
	  grp->TestClauses++;
	}
	min++;
      } while (min <= max &&
	       (!IsVarTerm(min->Tag)));
      if (min <= max && min->Tag == (_var+1)*sizeof(CELL)) {
	min++;
	if (min < max)
	  goto restart_loop;
      }
      grp->LastClause = min-1;
    }
    groups++;
    grp++;
    while (grp+16 > (GroupDef *)Yap_TrailTop) {
      UInt sz = (groups+16)*sizeof(GroupDef);
#if USE_SYSTEM_MALLOC
      Yap_Error_Size = sz;
      /* grow stack */
      save_machine_regs();
      longjmp(cint->CompilerBotch,4);
#else
      if (!Yap_growtrail(sz, TRUE)) {
	save_machine_regs();
	longjmp(cint->CompilerBotch,4);
	return 0;
      }
#endif
    }
  }
  return groups;
}

static UInt
new_label(struct intermediates *cint)
{
  UInt lbl = cint->i_labelno;
  cint->i_labelno += 2;
  return lbl;
}

static void
emit_trust(ClauseDef *cl, struct intermediates *cint, UInt nxtlbl, int clauses)
{
  PredEntry *ap = cint->CurrentPred;
  yamop *clcode = cl->Code;

  if (ap->PredFlags & TabledPredFlag)
    clcode = NEXTOP(clcode,Otapl);
  if (!(ap->PredFlags & LogUpdatePredFlag)) {
    /* this should not be generated for logical update predicates!! */
    if (ap->PredFlags & ProfiledPredFlag) {
      Yap_emit(retry_profiled_op, Unsigned(ap), Zero, cint);
    }
    if (ap->PredFlags & CountPredFlag) {
      Yap_emit(count_retry_op, Unsigned(ap), Zero, cint);
    }
  }
  if (clauses == 0) {
    Yap_emit(trust_op, (CELL)clcode, has_cut(cl->CurrentCode) , cint);
  } else {
    Yap_emit(retry_op, (CELL)clcode, (clauses << 1) | has_cut(cl->CurrentCode) , cint);
    Yap_emit(jumpi_op, nxtlbl, Zero, cint);
  }
}

static void
emit_retry(ClauseDef *cl, struct intermediates *cint, int clauses)
{
  PredEntry *ap = cint->CurrentPred;
  yamop *clcode = cl->Code;

  if (ap->PredFlags & TabledPredFlag)
    clcode = NEXTOP(clcode,Otapl);
  if (!(ap->PredFlags & LogUpdatePredFlag)) {
    /* this should not be generated for logical update predicates!! */
    if (ap->PredFlags & ProfiledPredFlag) {
      Yap_emit(retry_profiled_op, Unsigned(ap), Zero, cint);
    }
    if (ap->PredFlags & CountPredFlag) {
      Yap_emit(count_retry_op, Unsigned(ap), Zero, cint);
    }
  }
  Yap_emit(retry_op, (CELL)clcode, (clauses << 1) | has_cut(cl->CurrentCode), cint);
}

static compiler_vm_op
emit_optry(int var_group, int first, int clauses, int clleft, PredEntry *ap)
{
  /* var group */
  if (var_group || clauses == 0) {
    if (first) {
      return try_op;
    } else if (clleft+clauses) {
      return retry_op;
    } else {
      return trust_op;
    }
  } else if (clleft == 0) {
#ifdef TABLING
    if (ap->PredFlags & TabledPredFlag && !first) {
      /* we never actually get to remove the last choice-point in this case */
      return retry_op;
    } else
#endif /* TABLING */
    {
      /* last group */
      return try_op;
    }
  } else {
    /* nonvar group */
    return try_in_op;
  }
}


static void
emit_try(ClauseDef *cl, struct intermediates *cint, int var_group, int first, int clauses, int clleft, UInt nxtlbl)
{
  PredEntry *ap = cint->CurrentPred;
  yamop *clcode;
  compiler_vm_op comp_op;

  if (ap->PredFlags & LogUpdatePredFlag) {
    clcode = cl->Code;
  } else if (ap->PredFlags & TabledPredFlag) {
    clcode = NEXTOP(cl->Code,Otapl);
  } else {
    clcode = cl->CurrentCode;
  }

  comp_op = emit_optry(var_group, first, clauses, clleft, cint->CurrentPred);
  Yap_emit(comp_op, (CELL)clcode, ((clauses+clleft) << 1) | has_cut(cl->CurrentCode), cint);
}

static TypeSwitch *
emit_type_switch(compiler_vm_op op, struct intermediates *cint)
{
 return (TypeSwitch *)Yap_emit_extra_size(op, 0, sizeof(TypeSwitch), cint);
}


static yamop *
emit_switch_space(UInt n, UInt item_size, struct intermediates *cint, CELL func_mask)
{
  PredEntry *ap = cint->CurrentPred;

  if (ap->PredFlags & LogUpdatePredFlag) {
    UInt sz = sizeof(LogUpdIndex)+n*item_size;
    LogUpdIndex *cl = (LogUpdIndex *)Yap_AllocCodeSpace(sz);
    if (cl == NULL) {
      /* grow stack */
      save_machine_regs();
      longjmp(cint->CompilerBotch,2);
    }
    Yap_LUIndexSpace_SW += sz;
    cl->ClFlags = SwitchTableMask|LogUpdMask|func_mask;
    cl->ClSize = sz;
    cl->ClPred = cint->CurrentPred;
    /* insert into code chain */
#ifdef LOW_PROF
    if (ProfilerOn &&
	Yap_OffLineProfiler) {
      Yap_inform_profiler_of_clause(cl->ClCode, (yamop*)((CODEADDR)cl+sz), ap, 1); 
    }
#endif /* LOW_PROF */
    return cl->ClCode;
  } else {
    UInt sz = sizeof(StaticIndex)+n*item_size;
    StaticIndex *cl = (StaticIndex *)Yap_AllocCodeSpace(sz);
    if (cl == NULL) {
      /* grow stack */
      save_machine_regs();
      longjmp(cint->CompilerBotch,2);
    }
    Yap_IndexSpace_SW += sz;
    cl->ClFlags = SwitchTableMask;
    cl->ClSize = sz;
    cl->ClPred = cint->CurrentPred;
#ifdef LOW_PROF
    if (ProfilerOn &&
	Yap_OffLineProfiler) {
      Yap_inform_profiler_of_clause(cl->ClCode, (yamop*)((CODEADDR)cl+sz), ap, 1); 
    }
#endif /* LOW_PROF */
    return cl->ClCode;
    /* insert into code chain */
  }  
}

static AtomSwiEntry *
emit_cswitch(COUNT n, yamop *fail_l, struct intermediates *cint)
{
  compiler_vm_op op;
  AtomSwiEntry *target;

  if (n > MIN_HASH_ENTRIES) {
    COUNT cases = MIN_HASH_ENTRIES, i;
    n += 1+n/4;
    while (cases < n) cases *= 2;
    n = cases;
    op = switch_c_op;
    target = (AtomSwiEntry *)emit_switch_space(n, sizeof(AtomSwiEntry), cint, 0);
    for (i=0; i<n; i++) {
      target[i].Tag = Zero;
      target[i].u.labp = fail_l;
    }
    Yap_emit(op, Unsigned(n), (CELL)target, cint);
  } else {
    UInt i;

    op = if_c_op;
    target = (AtomSwiEntry *)emit_switch_space(n+1, sizeof(AtomSwiEntry), cint, 0);

    for (i=0; i<n; i++) {
      target[i].u.labp = fail_l;
    }
    target[n].Tag = Zero;
    target[n].u.labp = fail_l;
    Yap_emit(op, Unsigned(n), (CELL)target, cint);
  }
  return target;
}

static AtomSwiEntry *
lookup_c_hash(Term t, yamop *tab, COUNT entries)
{
  AtomSwiEntry *cebase = (AtomSwiEntry *)tab;
  int hash, d;
  AtomSwiEntry *centry;

  hash = (t >> HASH_SHIFT) & (entries-1);
  centry = cebase + hash;
  d = (entries-1) & (t|1);
  while (centry->Tag != t) {
    if (centry->Tag == 0L)
      return centry;
    hash = (hash + d) & (entries-1);
    centry = cebase + hash;
  }
  return centry;
}

static AtomSwiEntry *
fetch_centry(AtomSwiEntry *cebase, Term wt, int i, int n)
{
  if (n > MIN_HASH_ENTRIES) {
    int cases = MIN_HASH_ENTRIES;

    n += 1+n/4;
    while (cases < n) cases *= 2;
    return lookup_c_hash(wt, (yamop *)cebase, cases);
  } else {
    return cebase + i;
  }
}

static FuncSwiEntry *
emit_fswitch(COUNT n, yamop *fail_l, struct intermediates *cint)
{
  compiler_vm_op op;
  FuncSwiEntry *target;

  if (n > MIN_HASH_ENTRIES) {
    int cases = MIN_HASH_ENTRIES, i;
    n += 1+n/4;
    while (cases < n) cases *= 2;
    n = cases;
    op = switch_f_op;
    target = (FuncSwiEntry *)emit_switch_space(n, sizeof(FuncSwiEntry), cint, FuncSwitchMask);
    for (i=0; i<n; i++) {
      target[i].Tag = NULL;
      target[i].u.labp = fail_l;
    }
    Yap_emit(op, Unsigned(n), (CELL)target, cint);
  } else {
    UInt i;

    op = if_f_op;
    target = (FuncSwiEntry *)emit_switch_space(n+1, sizeof(FuncSwiEntry), cint, FuncSwitchMask);
    for (i=0; i<n; i++) {
      target[i].u.labp = fail_l;
    }
    target[n].Tag = NULL;
    target[n].u.labp = fail_l;
    Yap_emit(op, Unsigned(n), (CELL)target, cint);
  }
  return target;
}

static FuncSwiEntry *
lookup_f_hash(Functor f, yamop *tab, COUNT entries)
{
  FuncSwiEntry *febase = (FuncSwiEntry *)tab;
  int hash, d;
  FuncSwiEntry *fentry;
  Term wt = (Term)f;

  hash = (wt >> HASH_SHIFT) & (entries-1);
  fentry = febase + hash;
  d = (entries-1) & (wt|1);
  while (fentry->Tag != f) {
    if (fentry->Tag == NULL)
      return fentry;
    hash = (hash + d) & (entries-1);
    fentry = febase + hash;
  }
  return fentry;
}

static FuncSwiEntry *
fetch_fentry(FuncSwiEntry *febase, Functor ft, int i, int n)
{
  if (n > MIN_HASH_ENTRIES) {
    int cases = MIN_HASH_ENTRIES;

    n += 1+n/4;
    while (cases < n) cases *= 2;
    return lookup_f_hash(ft, (yamop *)febase, cases);
  } else {
    return febase + i;
  }
}

/* we assume there is at least one clause, that is, c0 < cf */
static UInt
do_var_clauses(ClauseDef *c0, ClauseDef *cf, int var_group, struct intermediates *cint, int first, int clleft, UInt nxtlbl, UInt argno0) {
  UInt labl;
  UInt labl_dyn0 = 0, labl_dynf = 0;

  labl = new_label(cint);
  Yap_emit(label_op, labl, Zero, cint);
  /*
    add expand_node if var_group == TRUE (jump on var) ||
		       var_group == FALSE (leaf node)
   */
  if (first &&
      cint->CurrentPred->PredFlags & LogUpdatePredFlag) {
    UInt ncls;
    labl_dyn0 = new_label(cint);
    if (clleft)
      labl_dynf = labl_dyn0;
    else
      labl_dynf = new_label(cint);
    if (clleft == 0) /* trust*/
      ncls = (cf-c0)+1;
    else
      ncls = 0;
    Yap_emit_4ops(enter_lu_op, labl_dyn0, labl_dynf, ncls, Zero, cint);
    Yap_emit(label_op, labl_dyn0, Zero, cint); 
  }
  if (c0 == cf) {
    emit_try(c0, cint, var_group, first, 0, clleft, nxtlbl);
  } else {

    if (c0 < cf) {
      emit_try(c0, cint, var_group, first, cf-c0, clleft, nxtlbl);
    }
    c0++;
    while (c0 < cf) {
      emit_retry(c0, cint, clleft+(cf-c0));
      c0++;
    }
    if (c0 == cf) {
      emit_trust(c0, cint, nxtlbl, clleft);
      if (!clleft && 
	  cint->CurrentPred->PredFlags & LogUpdatePredFlag) {
	Yap_emit(label_op, labl_dynf, Zero, cint); 
      }
    }
  }
  return labl;
}

static UInt
do_var_group(GroupDef *grp, struct intermediates *cint, int var_group, int first, int clleft, UInt nxtlbl, UInt argno0) {
  return do_var_clauses(grp->FirstClause, grp->LastClause, var_group, cint, first, clleft, nxtlbl, argno0);
}


/* count the number of different constants */
static UInt
count_consts(GroupDef *grp)
{
  Term current = MkAtomTerm(AtomFoundVar);
  UInt i = 0;
  ClauseDef *cl = grp->FirstClause;
    
  while (IsAtomTerm(cl->Tag) || IsIntTerm(cl->Tag)) {
    if (current != cl->Tag) {
      i++;
      current = cl->Tag;
    }
    if (cl == grp->LastClause) {
      return i;
    }
    cl++;
  }
  return i;
}

static UInt
count_blobs(GroupDef *grp)
{
  Term current = MkAtomTerm(AtomFoundVar);
  UInt i = 0;
  ClauseDef *cl = grp->FirstClause;
    
  while (TRUE) {
    if (current != cl->Tag) {
      i++;
      current = cl->Tag;
    }
    if (cl == grp->LastClause) {
      return i;
    }
    cl++;
  }
  return i;
}

/* count the number of different constants */
static UInt
count_funcs(GroupDef *grp)
{
  Term current = MkAtomTerm(AtomFoundVar);
  UInt i = 0;
  ClauseDef *cl = grp->FirstClause;
    
  while (IsApplTerm(cl->Tag)) {
    if (current != cl->Tag) {
      i++;
      current = cl->Tag;
    }
    if (cl == grp->LastClause) {
      return i;
    }
    cl++;
  }
  return i;
}

static UInt
emit_single_switch_case(ClauseDef *min, struct intermediates *cint, int first, int clleft, UInt nxtlbl)
{
  if (cint->CurrentPred->PredFlags & TabledPredFlag) {
    /* with tabling we don't clean trust at the very end of computation.
    */
    if (clleft || !first) {
      /*
	if we still have clauses left, means we already created a CP,
	so I should avoid creating again 
      */
      return (UInt)NEXTOP(min->Code,Otapl);
    } else {
      return (UInt)min->Code;
    }    
  }
  if (cint->CurrentPred->PredFlags & LogUpdatePredFlag) {
    return (UInt)(min->Code);
  } else {
    return (UInt)(min->CurrentCode);
  }
}

static UInt
suspend_indexing(ClauseDef *min, ClauseDef *max, PredEntry *ap, struct intermediates *cint)
{
  UInt tcls = ap->cs.p_code.NOfClauses;
  UInt cls = (max-min)+1;

  if (cint->expand_block &&
      cint->expand_block != (yamop *)(&(ap->cs.p_code.ExpandCode)) &&
      cint->expand_block->u.sssllp.s2 < 2*(max-min)) {
    cint->expand_block->u.sssllp.s3++;
    return (UInt)(cint->expand_block);
  }
  if (cls < tcls/8) {
    yamop *ncode;
    yamop **st;
    UInt tels;
    UInt sz;

    if (ap->PredFlags & LogUpdatePredFlag) {
      /* give it some slack */
      tels = cls + 4;
    } else {
      tels = cls;
    }
    sz = (UInt)NEXTOP((yamop *)NULL,sssllp)+tels*sizeof(yamop *), sz;
    if ((ncode = (yamop *)Yap_AllocCodeSpace(sz)) == NULL) {
      save_machine_regs();
      longjmp(cint->CompilerBotch, 2);
    }
#if DEBUG
    Yap_ExpandClauses++;
    Yap_expand_clauses_sz += sz;
#endif
    if (ap->PredFlags & LogUpdatePredFlag) {
      Yap_LUIndexSpace_EXT += sz;
    } else {
      Yap_IndexSpace_EXT += sz;
    }
#ifdef LOW_PROF
    if (ProfilerOn &&
	Yap_OffLineProfiler) {
      Yap_inform_profiler_of_clause(ncode, NEXTOP(ncode,sssllp), ap, 1); 
    }
#endif /* LOW_PROF */
    /* create an expand_block */
    ncode->opc = Yap_opcode(_expand_clauses);
    ncode->u.sssllp.p = ap;
    ncode->u.sssllp.s1 = tels;
    ncode->u.sssllp.s2 = cls;
    ncode->u.sssllp.s3 = 1;
    st = (yamop **)NEXTOP(ncode,sssllp);
    while (min <= max) {
      *st++ = min->Code;
      min++;
    }
    while (cls < tels) {
      *st++ = NULL;
      cls++;
    }
    LOCK(ExpandClausesListLock);
    ncode->u.sssllp.snext = ExpandClausesFirst;
    ncode->u.sssllp.sprev = NULL;
    if (ExpandClausesFirst)
      ExpandClausesFirst->u.sssllp.sprev = ncode;
    ExpandClausesFirst = ncode;
    if (ExpandClausesLast == NULL)
      ExpandClausesLast = ncode;
    UNLOCK(ExpandClausesListLock);
    return (UInt)ncode;
  }
  return (UInt)&(ap->cs.p_code.ExpandCode);
}

static void
recover_ecls_block(yamop *ipc)
{
  ipc->u.sssllp.s3--;
  if (!ipc->u.sssllp.s3) {
    LOCK(ExpandClausesListLock);
    if (ExpandClausesFirst == ipc)
      ExpandClausesFirst = ipc->u.sssllp.snext;
    if (ExpandClausesLast == ipc) {
      ExpandClausesLast = ipc->u.sssllp.sprev;
    }
    if (ipc->u.sssllp.sprev) {
      ipc->u.sssllp.sprev->u.sssllp.snext = ipc->u.sssllp.snext;
    }
    if (ipc->u.sssllp.snext) {
      ipc->u.sssllp.snext->u.sssllp.sprev = ipc->u.sssllp.sprev;
    }
    UNLOCK(ExpandClausesListLock);
#if DEBUG
    Yap_ExpandClauses--;
    Yap_expand_clauses_sz -= (UInt)(NEXTOP((yamop *)NULL,sssllp))+ipc->u.sssllp.s1*sizeof(yamop *);
#endif
    /* no dangling pointers for gprof */
    Yap_InformOfRemoval((CODEADDR)ipc);
    if (ipc->u.sssllp.p->PredFlags & LogUpdatePredFlag) {
      Yap_LUIndexSpace_EXT -= (UInt)NEXTOP((yamop *)NULL,sssllp)+ipc->u.sssllp.s1*sizeof(yamop *);
    } else
      Yap_IndexSpace_EXT -= (UInt)NEXTOP((yamop *)NULL,sssllp)+ipc->u.sssllp.s1*sizeof(yamop *);
    Yap_FreeCodeSpace((char *)ipc);
  }
}

static UInt
do_var_entries(GroupDef *grp, Term t, struct intermediates *cint, UInt argno, int first, int clleft, UInt nxtlbl){
  PredEntry *ap = cint->CurrentPred;

  if (!IsVarTerm(t) || t != 0L) {
    return suspend_indexing(grp->FirstClause, grp->LastClause, ap, cint);
  }
  return do_var_group(grp, cint, FALSE, first, clleft, nxtlbl, ap->ArityOfPE+1);
}

static UInt
do_consts(GroupDef *grp, Term t, struct intermediates *cint, int compound_term, CELL *sreg, UInt arity, int last_arg, UInt argno, int first, UInt nxtlbl, int clleft, CELL *top)
{
  COUNT n;
  ClauseDef *min = grp->FirstClause;
  COUNT i;
  UInt lbl;
  /* generate a switch */
  AtomSwiEntry *cs;
  PredEntry *ap = cint->CurrentPred;

  if (!IsAtomTerm(min->Tag) && !IsIntTerm(min->Tag)) {
    /* no clauses, just skip */
    return nxtlbl;
  }
  n = count_consts(grp);
  lbl = new_label(cint);
  Yap_emit(label_op, lbl, Zero, cint);
  cs = emit_cswitch(n, FAILCODE, cint);
  for (i = 0; i < n; i++) {
    AtomSwiEntry *ics;
    ClauseDef *max = min;

    ics = fetch_centry(cs, min->Tag, i, n);
    ics->Tag = min->Tag;
    while ((max+1)->Tag == min->Tag &&
	   max != grp->LastClause) max++;
    if (min != max) {
      if (sreg != NULL) {
	if (ap->PredFlags & LogUpdatePredFlag && max > min) {
	  ics->u.Label = suspend_indexing(min, max, ap, cint);
	} else {
	    ics->u.Label = do_compound_index(min, max, sreg, cint, compound_term, arity, argno, nxtlbl, first, last_arg, clleft, top, TRUE);
	}
      } else if (ap->PredFlags & LogUpdatePredFlag) {
	ics->u.Label = suspend_indexing(min, max, cint->CurrentPred, cint);
      } else {
	ics->u.Label = do_index(min, max, cint, argno+1, nxtlbl, first, clleft, top);
      }
    } else {
      ics->u.Label = do_index(min, max, cint, argno+1, nxtlbl, first, clleft, top);
    }
    grp->FirstClause = min = max+1;
  }
  return lbl;
}

static void
do_blobs(GroupDef *grp, Term t, struct intermediates *cint, UInt argno, int first, UInt nxtlbl, int clleft, CELL *top)
{
  COUNT n;
  ClauseDef *min = grp->FirstClause;
  COUNT i;
  /* generate a switch */
  AtomSwiEntry *cs;
  PredEntry *ap = cint->CurrentPred;

  n = count_blobs(grp);
  cs = emit_cswitch(n, (yamop *)nxtlbl, cint);
  for (i = 0; i < n; i++) {
    AtomSwiEntry *ics;
    ClauseDef *max = min;

    ics = fetch_centry(cs, min->Tag, i, n);
    ics->Tag = min->Tag;
    while ((max+1)->Tag == min->Tag &&
	   max != grp->LastClause) max++;
    if (min != max &&
	(ap->PredFlags & LogUpdatePredFlag)) {
      ics->u.Label = suspend_indexing(min, max, ap, cint);
    } else {
      ics->u.Label = do_index(min, max, cint, argno+1, nxtlbl, first, clleft, top);
    }
    grp->FirstClause = min = max+1;
  }
}

static UInt
do_funcs(GroupDef *grp, Term t, struct intermediates *cint, UInt argno, int first, int last_arg, UInt nxtlbl, int clleft, CELL *top)
{
  COUNT n = count_funcs(grp);
  ClauseDef *min = grp->FirstClause;
  COUNT i;
  FuncSwiEntry *fs;
  UInt lbl;

  if (min > grp->LastClause || n == 0) {
    /* no clauses, just skip */
    return nxtlbl;
  }
  lbl = new_label(cint);
  Yap_emit(label_op, lbl, Zero, cint);
  /* generate a switch */
  fs = emit_fswitch(n, FAILCODE, cint);
  for (i = 0; i < n ; i++) {
    Functor f = (Functor)RepAppl(min->Tag);
    FuncSwiEntry *ifs;
    ClauseDef *max = min;

    ifs = fetch_fentry(fs, f, i, n);
    ifs->Tag = f;
    while ((max+1)->Tag == min->Tag &&
	   max != grp->LastClause) max++; 
    /* delay non-trivial indexing  
       if (min != max &&
       !IsExtensionFunctor(f)) {
       ifs->u.Label = suspend_indexing(min, max, ap, cint);
       } else 
    */

    if (IsExtensionFunctor(f)) {
      if (f == FunctorDBRef) 
	ifs->u.Label = do_dbref_index(min, max, t, cint, argno, nxtlbl, first, clleft, top);
      else if (f == FunctorLongInt) 
	ifs->u.Label = do_blob_index(min, max, t, cint, argno, nxtlbl, first, clleft, top, FALSE);
      else
	ifs->u.Label = do_blob_index(min, max, t, cint, argno, nxtlbl, first, clleft, top, TRUE);
	
    } else {
      CELL *sreg;

      if (!IsVarTerm(t) && IsApplTerm(t) && FunctorOfTerm(t) == f) {
	sreg = RepAppl(t)+1;
      } else {
	sreg = NULL;
      }
      ifs->u.Label = do_compound_index(min, max, sreg, cint, 0, ArityOfFunctor(f), argno, nxtlbl, first, last_arg, clleft, top, TRUE);
    }
    grp->FirstClause = min = max+1;
  }
  return lbl;
}

static UInt
do_pair(GroupDef *grp, Term t, struct intermediates *cint, UInt argno, int first, int last_arg, UInt nxtlbl, int clleft, CELL *top)
{
  ClauseDef *min = grp->FirstClause;
  ClauseDef *max = grp->FirstClause;

  while (IsPairTerm(max->Tag) && max != grp->LastClause) {
    max++;
  }
  if (!IsPairTerm(max->Tag)) {
    max--;
  }
  if (min > grp->LastClause) {
    /* no clauses, just skip */
    return nxtlbl;
  }
  grp->FirstClause = max+1;
  if (min == max) {
    /* single clause, no need to do indexing, but we do know it is a list */ 
    if (cint->CurrentPred->PredFlags & LogUpdatePredFlag) {
      return (UInt)(min->Code);
    } else {
      return (UInt)(min->CurrentCode);
    }
  }
  if (min != max && !IsPairTerm(t)) {
    return suspend_indexing(min, max, cint->CurrentPred, cint);
  }
  return do_compound_index(min, max, (IsPairTerm(t) ? RepPair(t) : NULL), cint, 0, 2, argno, nxtlbl, first, last_arg, clleft, top, TRUE);
}

static void
group_prologue(int compound_term, UInt argno, int first, struct intermediates *cint)
{
  if (compound_term) {
    Yap_emit(cache_sub_arg_op, compound_term-1, compound_term-1, cint);
  } else {
    if (!first || argno != 1) {
      Yap_emit(cache_arg_op, argno, argno, cint);
    }
  }
}

/* make sure that we can handle failure correctly */
static void
emit_protection_choicepoint(int first, int clleft, UInt nxtlbl, struct intermediates *cint)
{

  if (first) {
    if (clleft) {
      if (cint->CurrentPred->PredFlags & LogUpdatePredFlag) {
	UInt labl = new_label(cint);

	Yap_emit_4ops(enter_lu_op, labl, labl, 0, Zero, cint);
	Yap_emit(label_op, labl, Zero, cint);
      }
      Yap_emit(tryme_op, nxtlbl, (clleft << 1), cint);
    }
  } else {
    /* !first */
    if (clleft) {
      Yap_emit(retryme_op, nxtlbl, (clleft << 1), cint);
    } else {
      Yap_emit(trustme_op, 0, 0, cint);
    }
  }
}


static ClauseDef *
cls_move(ClauseDef *min, PredEntry *ap, ClauseDef *max, int compound_term, UInt argno, int last_arg)
{
  ClauseDef *cl=min;

  cl = min;
  if (compound_term) {
    while (cl <= max) {
      skip_to_arg(cl, ap, compound_term, last_arg );
      cl++;
    }
  } else {
    while (cl <= max) {
      if (cl->Tag == (_var+1)*sizeof(CELL)) {
	ClauseDef *cli = cl;
	while (cli < max) {
	  clcpy(cli,cli+1);
	  cli++;
	}
	max--;
      } else {
	move_next(cl, argno);
      }
      cl++;
    }
  }
  return max;
}

static void
purge_pvar(GroupDef *group) {
  ClauseDef *max = group->LastClause;
  ClauseDef *cl = group->FirstClause;

  while (cl <= max) {
    if (cl->Tag == (_var+1)*sizeof(CELL)) {
      ClauseDef *cli = cl;
      while (cli < max) {
	clcpy(cli,cli+1);
	cli++;
      }
      group->VarClauses--;
      max--;
    }
    cl++;
  }
  group->LastClause = max;
}


static UInt *
do_nonvar_group(GroupDef *grp, Term t, UInt compound_term, CELL *sreg, UInt arity, UInt labl, struct intermediates *cint, UInt argno, int first, int last_arg, UInt nxtlbl, int clleft, CELL *top) {
  TypeSwitch *type_sw;
  PredEntry *ap = cint->CurrentPred;
  

  /* move cl pointer */
  if (grp->AtomClauses + grp->PairClauses + grp->StructClauses > 1) {
    Yap_emit(label_op, labl, Zero, cint);
    if (argno == 1 && !compound_term) {
      emit_protection_choicepoint(first, clleft, nxtlbl, cint);
    }
    group_prologue(compound_term, argno, first, cint);
    if (grp->LastClause < grp->FirstClause) { /* only tests */
      return NULL;
    }
    type_sw = emit_type_switch(switch_on_type_op, cint);
    /* have these first so that we will have something initialised here */
    type_sw->ConstEntry = 
      type_sw->FuncEntry = 
      type_sw->PairEntry = 
      type_sw->VarEntry =
      nxtlbl;
    type_sw->VarEntry = do_var_entries(grp, t, cint, argno, first, clleft, nxtlbl);
    grp->LastClause = cls_move(grp->FirstClause, ap, grp->LastClause, compound_term, argno, last_arg);
    sort_group(grp,top,cint);
    while (grp->FirstClause <= grp->LastClause) {
      if (IsAtomOrIntTerm(grp->FirstClause->Tag)) {
	type_sw->ConstEntry = do_consts(grp, t, cint, compound_term, sreg, arity, last_arg, argno, first, nxtlbl, clleft, top);
      } else if (IsApplTerm(grp->FirstClause->Tag)) {
	type_sw->FuncEntry = do_funcs(grp, t, cint, argno, first, last_arg, nxtlbl, clleft, top);
      } else {
	type_sw->PairEntry = do_pair(grp, t, cint, argno, first, last_arg, nxtlbl, clleft, top);
      }
    }
    return &(type_sw->VarEntry);
  } else {
    Yap_emit(label_op,labl,Zero, cint);
    do_var_group(grp, cint, TRUE, first, clleft, nxtlbl, ap->ArityOfPE+1);
    return NULL;
  }
}

static UInt
do_optims(GroupDef *group, int ngroups, UInt fail_l, ClauseDef *min, struct intermediates *cint)
{
  if (ngroups==2 && group[0].FirstClause ==  group[0].LastClause &&
      group[0].AtomClauses == 1 && group[1].VarClauses == 1) {
    CELL *sp;
    UInt labl;

    labl = new_label(cint);
    sp = Yap_emit_extra_size(if_not_op, Zero, 4*CellSize, cint);
    sp[0] = (CELL)(group[0].FirstClause->Tag);
    sp[1] = (CELL)(group[1].FirstClause->Code);
    sp[2] = do_var_clauses(group[0].FirstClause, group[1].LastClause, FALSE, cint, TRUE, 0, (CELL)FAILCODE, cint->CurrentPred->ArityOfPE+1);      
    sp[3] = do_var_clauses(min, group[1].LastClause, FALSE, cint, TRUE, 0, (CELL)FAILCODE, cint->CurrentPred->ArityOfPE+1);
    return labl;
  }
  return fail_l;
}

static int
cls_info(ClauseDef *min, ClauseDef *max, UInt argno)
{
  ClauseDef *cl=min;
  int found_pvar = FALSE;

  while (cl <= max) {
    add_info(cl, argno);
    if (cl->Tag == (_var+1)*sizeof(CELL)) {
      found_pvar = TRUE;
    }
    /*    if (IsVarTerm(cl->Tag)) cl->Tag = (CELL)NULL; */
    cl++;
  }
  return found_pvar;
}

static int
cls_head_info(ClauseDef *min, ClauseDef *max, UInt argno, int in_idb)
{
  ClauseDef *cl=min;

  if (in_idb) {
    if (argno != 2) {
      while (cl <= max) {
	cl->Tag = (CELL)NULL;
	cl++;
      } 
    } else {
      while (cl <= max) {
	LogUpdClause *lcl = ClauseCodeToLogUpdClause(cl->CurrentCode);
	Term t = lcl->ClSource->Entry;
      
	if (IsVarTerm(t)) {
	  cl->Tag = (CELL)NULL;
	} else if (IsApplTerm(t)) {
	  CELL *pt = RepAppl(t);
	
	  cl->Tag = AbsAppl((CELL *)pt[0]);
	  if (IsExtensionFunctor(FunctorOfTerm(t))) {
	    cl->u.t_ptr = t;
	  } else {
	    cl->u.c_sreg = pt;
	  }
	} else if (IsPairTerm(t)) {
	  CELL *pt = RepPair(t);

	  cl->Tag = AbsPair(NULL);
	  cl->u.c_sreg = pt-1;
	} else {
	  cl->Tag = t;
	}
	cl++;
      }
    }
  } else {
      while (cl <= max) {
	add_head_info(cl, argno);
	/*    if (IsVarTerm(cl->Tag)) cl->Tag = (CELL)NULL; */
	cl++;
      }
  }
  return FALSE;
}

static UInt
do_index(ClauseDef *min, ClauseDef* max, struct intermediates *cint, UInt argno, UInt fail_l, int first, int clleft, CELL *top)
{
  UInt ngroups, found_pvar = FALSE;
  UInt i = 0;
  GroupDef *group = (GroupDef *)top;
  UInt labl, labl0, lablx;
  Term t;
  /* remember how we entered here */
  UInt argno0 = argno;
  PredEntry *ap = cint->CurrentPred;
  yamop *eblk = cint->expand_block;

  if (min == max) {
    /* base case, just commit to the current code */
    return emit_single_switch_case(min, cint, first, clleft, fail_l);
  }
  if ((argno > 1 && yap_flags[INDEXING_MODE_FLAG] == INDEX_MODE_SINGLE && ap->PredFlags & LogUpdatePredFlag) ||
      yap_flags[INDEXING_MODE_FLAG] == INDEX_MODE_OFF ||
      ap->ArityOfPE < argno) {
    return do_var_clauses(min, max, FALSE, cint, first, clleft, fail_l, ap->ArityOfPE+1);
  }
  t = Deref(XREGS[argno]);
  if (ap->PredFlags & LogUpdatePredFlag) {
    found_pvar = cls_head_info(min, max, argno, (ap->ModuleOfPred == IDB_MODULE));
  } else {
    found_pvar = cls_info(min, max, argno);
  }
  ngroups = groups_in(min, max, group, cint);
  if (IsVarTerm(t)) {
    lablx = new_label(cint);
    Yap_emit(label_op, lablx, Zero, cint);
    while (IsVarTerm(t)) {
      if (ngroups > 1 || !group->VarClauses) {
	UInt susp_lab = suspend_indexing(min, max, ap, cint);
	if (!cint->expand_block) {
	  cint->expand_block = (yamop *)susp_lab;
	}
	Yap_emit(jump_nv_op, susp_lab, argno, cint);
      }
      if (argno == ap->ArityOfPE ||
	  (yap_flags[INDEXING_MODE_FLAG] == INDEX_MODE_SINGLE &&
	   ap->PredFlags & LogUpdatePredFlag)) {
	do_var_clauses(min, max, FALSE, cint, first, clleft, fail_l, argno0);
	cint->expand_block = eblk;
	return lablx;
      }
      argno++;
      t = Deref(XREGS[argno]);
      if (ap->PredFlags & LogUpdatePredFlag) {
	found_pvar = cls_head_info(min, max, argno, (ap->ModuleOfPred == IDB_MODULE) );
      } else {
	found_pvar = cls_info(min, max, argno);
      }
      ngroups = groups_in(min, max, group, cint);
    } 
    labl0 = labl = new_label(cint);
  } else {
    lablx = labl0 = labl = new_label(cint);
  }
  cint->expand_block = eblk;
  top = (CELL *)(group+ngroups);
  if (argno > 1) {
    /* don't try being smart for other arguments than the first */
    if (ngroups > 1 || group->VarClauses != 0 || found_pvar) {
      if (ap->ArityOfPE == argno) {
	return do_var_clauses(min, max, FALSE, cint, first, clleft, fail_l, ap->ArityOfPE+1);
      } else {
	return do_index(min, max, cint, argno+1, fail_l, first, clleft, top);
      }
    } else {
      ClauseDef *cl = min;
      /*
	need to reset the code pointer, otherwise I could be in
	the middle of a compound term.
       */
      while (cl <= max) {
	cl->CurrentCode = cl->Code;
	cl++;
      }    
    }
  } else {
    UInt special_options;

    if ((ap->PredFlags & LogUpdatePredFlag) && ngroups > 1) {
      if (ngroups > 1) {
	group[0].VarClauses = ap->cs.p_code.NOfClauses;
	group[0].AtomClauses =
	  group[0].PairClauses = 
	  group[0].StructClauses = 
	  group[0].TestClauses = 0;
	group[0].LastClause = group[ngroups-1].LastClause;
	ngroups = 1;	
      }
    } else if ((special_options = do_optims(group, ngroups, fail_l, min, cint)) != fail_l) {
      return special_options;
    }
    if (ngroups == 1 && group->VarClauses && !found_pvar) {
      return do_index(min, max, cint, argno+1, fail_l, first, clleft, top);
    } else if (found_pvar ||
	       (ap->PredFlags & LogUpdatePredFlag && group[0].VarClauses)) {
      /* make sure we know where to suspend */
      Yap_emit(label_op, labl0, Zero, cint);
      labl = new_label(cint);
      Yap_emit(jump_v_op, suspend_indexing(min, max, ap, cint), Zero, cint); 
    }
  }
  for (i=0; i < ngroups; i++) {
    UInt nextlbl;
    int left_clauses = clleft+(max-group->LastClause);
    /* a group may end up not having clauses*/

    if (i < ngroups-1) {
      nextlbl = new_label(cint);
    } else {
      nextlbl = fail_l;
    }
    if (found_pvar && argno == 1) {
      purge_pvar(group);
    }
    if (group->FirstClause==group->LastClause && first && left_clauses == 0) {
      Yap_emit(jumpi_op, (CELL)(group->FirstClause->Code), Zero, cint);
    } else {
      if (group->VarClauses) {
	Yap_emit(label_op,labl,Zero, cint);
	do_var_group(group, cint, argno == 1, first, left_clauses, nextlbl, ap->ArityOfPE+1);
      } else {
	do_nonvar_group(group, t, 0, NULL, 0, labl, cint, argno, first, TRUE, nextlbl, left_clauses, top);
      }
    }
    first = FALSE;
    group++;
    labl = nextlbl;
  }
  return lablx;
}

static ClauseDef *
copy_clauses(ClauseDef *max0, ClauseDef *min0, CELL *top, struct intermediates *cint)
{
  UInt sz = ((max0+1)-min0)*sizeof(ClauseDef);
  if ((char *)top + sz >= Yap_TrailTop-4096) {
    Yap_Error_Size = sz;
    /* grow stack */
    save_machine_regs();
    longjmp(cint->CompilerBotch,4);
  }
  memcpy((void *)top, (void *)min0, sz);
  return (ClauseDef *)top;
}


/* execute an index inside a structure */
static UInt
do_compound_index(ClauseDef *min0, ClauseDef* max0, Term* sreg, struct intermediates *cint, UInt i, UInt arity, UInt argno, UInt fail_l, int first, int last_arg, int clleft, CELL *top, int done_work)
{
  UInt ret_lab = 0, *newlabp;
  CELL *top0 = top;
  ClauseDef *min, *max;
  PredEntry *ap = cint->CurrentPred;
  int found_index = FALSE, lu_pred = ap->PredFlags & LogUpdatePredFlag;

  newlabp = & ret_lab;
  if (min0 == max0) {
    /* base case, just commit to the current code */
    return emit_single_switch_case(min0, cint, first, clleft, fail_l);
  }
  if (yap_flags[INDEXING_MODE_FLAG] == INDEX_MODE_SINGLE && ap->PredFlags & LogUpdatePredFlag) {
    *newlabp = 
      do_var_clauses(min0, max0, FALSE, cint, first, clleft, fail_l, ap->ArityOfPE+1);
    return ret_lab;
  }
  if (sreg == NULL) {
    return suspend_indexing(min0, max0, ap, cint);
  }
  while (i < arity && !found_index) { 
    ClauseDef *cl;
    GroupDef *group;
    UInt ngroups;
    int isvt = IsVarTerm(Deref(sreg[i]));

    min = copy_clauses(max0, min0, top, cint);
    max = min+(max0-min0);
    top = (CELL *)(max+1);
    cl = min;
    /* search for a subargument */
    while (cl <= max) {
      add_arg_info(cl, ap, i+1);
      cl++;
    }
    group = (GroupDef *)top;
    ngroups = groups_in(min, max, group, cint);    
    if (ngroups == 1 && group->VarClauses == 0) {
      /* ok, we are doing a sub-argument */
      /* process group */

      found_index = TRUE;
      ret_lab = new_label(cint);
      top = (CELL *)(group+1);
      if (do_nonvar_group(group, (sreg == NULL ? 0L : Deref(sreg[i])), i+1, (isvt ? NULL : sreg), arity, *newlabp, cint, argno, first, (last_arg && i+1 == arity), fail_l, clleft, top) == NULL) {
	top = top0;
	break;
      }
    }
    top = top0;
    i++;
  }
  if (!found_index) {
    if (!lu_pred || !done_work)
      *newlabp = do_index(min0, max0, cint, argno+1, fail_l, first, clleft, top);
    else
      *newlabp = suspend_indexing(min0, max0, ap, cint);
  }
  return ret_lab;
}

static UInt
do_dbref_index(ClauseDef *min, ClauseDef* max, Term t, struct intermediates *cint, UInt argno, UInt fail_l, int first, int clleft, CELL *top)
{
  UInt ngroups;
  GroupDef *group;
  ClauseDef *cl = min;

  group = (GroupDef *)top;
  cl = min;
  
  while (cl <= max) {
    cl->Tag = cl->u.t_ptr;
    cl++;
  }
  ngroups = groups_in(min, max, group, cint);
  if (ngroups > 1 || group->VarClauses) {
    return do_index(min, max, cint, argno+1, fail_l, first, clleft, top);
  } else {
    int labl = new_label(cint);

    Yap_emit(label_op, labl, Zero, cint);
    Yap_emit(index_dbref_op, Zero, Zero, cint);
    sort_group(group,(CELL *)(group+1),cint);
    do_blobs(group, t, cint, argno, first, fail_l, clleft, (CELL *)group+1);
    return labl;
  }
}

static UInt
do_blob_index(ClauseDef *min, ClauseDef* max, Term t, struct intermediates *cint, UInt argno, UInt fail_l, int first, int clleft, CELL *top, int blob)
{
  UInt ngroups;
  GroupDef *group;
  ClauseDef *cl = min;

  group = (GroupDef *)top;
  cl = min;
  
  while (cl <= max) {
    if (cl->u.t_ptr == (CELL)NULL) { /* check whether it is a builtin */
      cl->Tag = Zero;
    } else if (blob) {
      CELL *pt = RepAppl(cl->u.t_ptr);
#if SIZEOF_DOUBLE == 2*SIZEOF_LONG_INT
      cl->Tag = MkIntTerm(pt[1]^pt[2]);
#else
      cl->Tag = MkIntTerm(pt[1]);
#endif
    } else {
      CELL *pt = RepAppl(cl->u.t_ptr);
      cl->Tag = MkIntTerm((pt[1] & (MAX_ABS_INT-1)));
    }
    cl++;
  }
  ngroups = groups_in(min, max, group, cint);
  if (ngroups > 1 || group->VarClauses) {
    return do_index(min, max, cint, argno+1, fail_l, first, clleft, top);
  } else {
    int labl = new_label(cint);

    Yap_emit(label_op, labl, Zero, cint);
    if (blob)
      Yap_emit(index_blob_op, Zero, Zero, cint);
    else
      Yap_emit(index_long_op, Zero, Zero, cint);
    sort_group(group,(CELL *)(group+1),cint);
    do_blobs(group, t, cint, argno, first, fail_l, clleft, (CELL *)group+1);
    return labl;
  }
}

static void
init_clauses(ClauseDef *cl, PredEntry *ap)
{
  if (ap->PredFlags & MegaClausePredFlag) {
    MegaClause *mcl = ClauseCodeToMegaClause(ap->cs.p_code.FirstClause);
    yamop *end = (yamop *)((char *)mcl->ClCode+mcl->ClSize);
    yamop *cd = mcl->ClCode;
    while (cd < end) {
      cl->Code = cl->CurrentCode = cd;
      cd = (yamop *)((char *)cd+mcl->ClItemSize);
      cl++;
    }
  } else {
    StaticClause *scl;

    scl = ClauseCodeToStaticClause(ap->cs.p_code.FirstClause);
    do {
      cl->Code = cl->CurrentCode = scl->ClCode;
      cl++;
      if (scl->ClCode == ap->cs.p_code.LastClause)
	return;
      scl = scl->ClNext;
    } while (TRUE);
  }
}

static void
init_log_upd_clauses(ClauseDef *cl, PredEntry *ap)
{
  LogUpdClause *lcl = ClauseCodeToLogUpdClause(ap->cs.p_code.FirstClause);

  do {
    cl->Code = cl->CurrentCode = lcl->ClCode;
    cl++;
    lcl = lcl->ClNext;
  } while (lcl != NULL);
}

static UInt
compile_index(struct intermediates *cint)
{
  PredEntry *ap = cint->CurrentPred;
  int NClauses = ap->cs.p_code.NOfClauses;
  ClauseDef *cls = (ClauseDef *)H;
  CELL *top = (CELL *) TR;

  /* only global variable I use directly */
  cint->i_labelno = 1;

  Yap_Error_Size = 0;
  /* reserve double the space for compiler */
  if (cls+2*NClauses > (ClauseDef *)(ASP-4096)) {
    /* tell how much space we need */
    Yap_Error_Size += NClauses*sizeof(ClauseDef);
    /* grow stack */
    save_machine_regs();
    longjmp(cint->CompilerBotch,3);
  }
  cint->freep = (char *)(cls+NClauses);
  if (ap->PredFlags & LogUpdatePredFlag) {
    /* throw away a label */
    new_label(cint);
    init_log_upd_clauses(cls,ap);
  } else if (ap->PredFlags & UDIPredFlag) {
    UInt lbl = new_label(cint);
    Yap_emit(user_switch_op, Unsigned(ap), Unsigned(&(ap->cs.p_code.ExpandCode)), cint);
    return lbl;
  } else {
    /* prepare basic data structures */ 
    init_clauses(cls,ap);
  }
  return do_index(cls, cls+(NClauses-1), cint, 1, (UInt)FAILCODE, TRUE, 0, top);
}


yamop *
Yap_PredIsIndexable(PredEntry *ap, UInt NSlots, yamop *next_pc)
{
  yamop *indx_out;
  int setjres;
  struct intermediates cint;


  cint.CurrentPred = ap;
  cint.code_addr = NULL;
  Yap_Error_Size = 0;

  if ((setjres = setjmp(cint.CompilerBotch)) == 3) {
    restore_machine_regs();
    recover_from_failed_susp_on_cls(&cint, 0);
    if (!Yap_gcl(Yap_Error_Size, ap->ArityOfPE+NSlots, ENV, next_pc)) {
      Yap_Error(OUT_OF_STACK_ERROR, TermNil, Yap_ErrorMessage);
      return FAILCODE;
    }
  } else if (setjres == 2) {
    restore_machine_regs();
    Yap_Error_Size = recover_from_failed_susp_on_cls(&cint, Yap_Error_Size);
    if (!Yap_growheap(FALSE, Yap_Error_Size, NULL)) {
      Yap_Error(OUT_OF_HEAP_ERROR, TermNil, Yap_ErrorMessage);
      return FAILCODE;
    }
  } else if (setjres == 4) {
    restore_machine_regs();
    recover_from_failed_susp_on_cls(&cint, 0);
    if (!Yap_growtrail(Yap_Error_Size, FALSE)) {
      Yap_Error(OUT_OF_TRAIL_ERROR, TermNil, Yap_ErrorMessage);
      return FAILCODE;
    }
  } else if (setjres != 0) {
    restore_machine_regs();
    recover_from_failed_susp_on_cls(&cint, 0);
    if (!Yap_growheap(FALSE, Yap_Error_Size, NULL)) {
      Yap_Error(OUT_OF_HEAP_ERROR, TermNil, Yap_ErrorMessage);
      return FAILCODE;
    }
  }
 restart_index:
  Yap_BuildMegaClause(ap);
  cint.CodeStart = cint.BlobsStart = cint.cpc = cint.icpc = NULL;
  cint.expand_block = NULL;
  Yap_ErrorMessage = NULL;
  if (compile_index(&cint) == (UInt)FAILCODE) {
    return FAILCODE;
  }
#ifdef DEBUG
  if (Yap_Option['i' - 'a' + 1]) {
    Yap_LockStream(Yap_c_error_stream);
    Yap_ShowCode(&cint);
    Yap_UnLockStream(Yap_c_error_stream);
  }
#endif
  /* globals for assembler */
  IPredArity = ap->ArityOfPE;
  if (cint.CodeStart) {
    if ((indx_out = Yap_assemble(ASSEMBLING_INDEX, TermNil, ap, FALSE, &cint)) == NULL) {
      if (!Yap_growheap(FALSE, Yap_Error_Size, NULL)) {
	Yap_Error(OUT_OF_HEAP_ERROR, TermNil, Yap_ErrorMessage);
	return NULL;
      }
      goto restart_index;
    }
  } else {
    return NULL;
  }
  if (ap->PredFlags & LogUpdatePredFlag) {
    LogUpdIndex *cl = ClauseCodeToLogUpdIndex(indx_out);
    cl->ClFlags |= SwitchRootMask;
  }
  return(indx_out);
}

static istack_entry *
push_stack(istack_entry *sp, Int arg, Term Tag, Term extra, struct intermediates *cint)
{
  if (sp+1 > (istack_entry *)Yap_TrailTop) {
    save_machine_regs();
    longjmp(cint->CompilerBotch,4);    
  }
  sp->pos = arg;
  sp->val = Tag;
  sp->extra = extra;
  sp++;
  sp->pos = 0;
  return sp;
}

static istack_entry *
install_clause(ClauseDef *cls, PredEntry *ap, istack_entry *stack)
{
  int last_arg = TRUE;

  istack_entry *sp = stack;
  last_arg = TRUE;
  while (sp->pos) {
    if ((Int)(sp->pos) > 0) {
      add_info(cls, sp->pos);
    } else if (sp->pos) {
      UInt argno = -sp->pos;
      add_arg_info(cls, ap, argno);
    }
    /* if we are not talking about a variable */
    if (cls->Tag != sp->val) {
      if (sp->val == 0L) {
	sp++;
      }
      break;
    } else {
      if (IsApplTerm(cls->Tag)) {
	Functor f = (Functor)RepAppl(cls->Tag);
	if (IsExtensionFunctor(f)) {
	  if (f == FunctorDBRef) {
	    if (cls->u.t_ptr != sp->extra) break;
	  } else if (f == FunctorDouble) {
	    CELL *pt = RepAppl(sp->extra);
	    if (cls->u.t_ptr) {
	      CELL *pt1 = RepAppl(cls->u.t_ptr);
#if SIZEOF_DOUBLE == 2*SIZEOF_LONG_INT
	      Term t = MkIntTerm(pt[1]^pt[2]),
		t1 = MkIntTerm(pt1[1]^pt1[2]);
#else
	      Term t = MkIntTerm(pt[1]),
		t1 = MkIntTerm(pt1[1]);
#endif
	      if (t != t1) break;
	    }
	  } else {
	    CELL *pt = RepAppl(sp->extra);
	    CELL *pt1 = RepAppl(cls->u.t_ptr);
	    Term t = MkIntTerm(pt[1] & (MAX_ABS_INT-1)),
	      t1 = MkIntTerm(pt1[1] & (MAX_ABS_INT-1));
	    if (t != t1) break;
	  }
	}
      }
      if ((Int)(sp->pos) > 0) {
	move_next(cls, sp->pos);
      } else if (sp->pos) {
	UInt argno = -sp->pos;
	skip_to_arg(cls, ap, argno, FALSE);
	if (ArityOfFunctor((Functor)RepAppl(sp[-1].val))
	    != argno+1) {
	  last_arg = FALSE;
	}
      }
    }
    sp++;
  }
  return sp;
}

static ClauseDef *
install_clauses(ClauseDef *cls, PredEntry *ap, istack_entry *stack, yamop *beg, yamop *end)
{
  istack_entry *sp = stack;
  if (ap->PredFlags & MegaClausePredFlag) {
    MegaClause *mcl = ClauseCodeToMegaClause(beg);
    yamop *end = (yamop *)((char *)mcl->ClCode+mcl->ClSize);
    yamop *cd = mcl->ClCode;

    if (stack[0].pos == 0) {
      while (TRUE) {
	cls->Code =  cls->CurrentCode = cd;
	cls->Tag =  0;
	cls++;
	cd = (yamop *)((char *)cd+mcl->ClItemSize);
	if (cd == end) {
	  return cls-1;
	}
      }
    }
    while (TRUE) {
      cls->Code =  cls->CurrentCode = cd;
      sp = install_clause(cls, ap, stack);
      /* we reached a matching clause */
      if (!sp->pos && (sp[-1].val == 0L || cls->Tag == sp[-1].val)) {
	cls++;
      }
      cd = (yamop *)((char *)cd+mcl->ClItemSize);
      if (cd == end) {
	return cls-1;
      }
    }
  } else {
    StaticClause *cl = ClauseCodeToStaticClause(beg);

    if (stack[0].pos == 0) {
      while (TRUE) {
	cls->Code =  cls->CurrentCode = cl->ClCode;
	cls->Tag =  0;
	cls++;
	if (cl->ClCode == end) {
	  return cls-1;
	}
	cl = cl->ClNext;
      }
    }
    while (TRUE) {
      cls->Code =  cls->CurrentCode = cl->ClCode;
      sp = install_clause(cls, ap, stack);
      /* we reached a matching clause */
      if (!sp->pos && (sp[-1].val == 0L || cls->Tag == sp[-1].val)) {
	cls++;
      }
      if (cl->ClCode == end || cl->ClCode == NULL) {
	return cls-1;
      }
      cl = cl->ClNext;
    }
  }
}

static ClauseDef *
install_clauseseq(ClauseDef *cls, PredEntry *ap, istack_entry *stack, yamop **beg, yamop **end)
{
  istack_entry *sp = stack;

  if (stack[0].pos == 0) {
    while (TRUE) {
      if (*beg) { 
	cls->Code =  cls->CurrentCode = *beg;
	cls->Tag =  0;
	cls++;
      }
      beg++;
      if (beg == end) {
	return cls-1;
      }
    }
  }
  while (TRUE) {
    if (*beg) {
      cls->Code =  cls->CurrentCode = *beg;
      sp = install_clause(cls, ap, stack);
      /* we reached a matching clause */
      if (!sp->pos && (sp[-1].val == 0L || cls->Tag == sp[-1].val)) {
	cls++;
      }
    }
    beg++;
    if (beg == end) {
      return cls-1;
    }
  }
}

static void
reinstall_clauses(ClauseDef *cls, ClauseDef *end, PredEntry *ap, istack_entry *stack)
{
  do {
    cls->CurrentCode = cls->Code;
    install_clause(cls, ap, stack);
  } while (cls++ != end);
}

static istack_entry *
install_log_upd_clause(ClauseDef *cls, PredEntry *ap, istack_entry *stack)
{
  int last_arg = TRUE;

  istack_entry *sp = stack;
  last_arg = TRUE;
  while (sp->pos) {
    if ((Int)(sp->pos) > 0) {
      add_head_info(cls, sp->pos);
    } else if (sp->pos) {
      UInt argno = -sp->pos;
      add_arg_info(cls, ap, argno);
    }
    /* if we are not talking about a variable */
    if (cls->Tag != sp->val) {
      if (sp->val == 0L) {
	sp++;
      }
      break;
    } else {
      if (IsApplTerm(cls->Tag)) {
	Functor f = (Functor)RepAppl(cls->Tag);
	if (IsExtensionFunctor(f)) {
	  if (f == FunctorDBRef) {
	    if (cls->u.t_ptr != sp->extra) break;
	  } else if (f == FunctorDouble) {
	    CELL *pt = RepAppl(sp->extra);
	    CELL *pt1 = RepAppl(cls->u.t_ptr);
#if SIZEOF_DOUBLE == 2*SIZEOF_LONG_INT
	    Term t = MkIntTerm(pt[1]^pt[2]),
	      t1 = MkIntTerm(pt1[1]^pt1[2]);
#else
	    Term t = MkIntTerm(pt[1]),
	      t1 = MkIntTerm(pt1[1]);
#endif
	    if (t != t1) break;
	  } else {
	    CELL *pt = RepAppl(sp->extra);
	    CELL *pt1 = RepAppl(cls->u.t_ptr);
	    Term t = MkIntTerm(pt[1] & (MAX_ABS_INT-1)),
	      t1 = MkIntTerm(pt1[1] & (MAX_ABS_INT-1));
	    if (t != t1) break;
	  }
	}
      }
      if ((Int)(sp->pos) > 0) {
	move_next(cls, sp->pos);
      } else if (sp->pos) {
	UInt argno = -sp->pos;
	skip_to_arg(cls, ap, argno, FALSE);
	if (ArityOfFunctor((Functor)RepAppl(sp[-1].val))
	    != argno+1) {
	  last_arg = FALSE;
	}
      }
    }
    sp++;
  }
  return sp;
}

static ClauseDef *
install_log_upd_clauses(ClauseDef *cls, PredEntry *ap, istack_entry *stack, yamop *beg, yamop *end)
{
  istack_entry *sp = stack;

  if (stack[0].pos == 0) {
    while (TRUE) {
      cls->Code =  cls->CurrentCode = beg;
      cls->Tag =  0;
      cls++;
      if (beg == end || beg == NULL) {
	return cls-1;
      }
      beg = ClauseCodeToLogUpdClause(beg)->ClNext->ClCode;
    }
  }
  while (TRUE) {
    cls->Code =  cls->CurrentCode = beg;
    sp = install_log_upd_clause(cls, ap, stack);
    /* we reached a matching clause */
    if (!sp->pos && (sp[-1].val == 0L || cls->Tag == sp[-1].val)) {
      cls++;
    }
    if (beg == end || beg == NULL) {
      return cls-1;
    }
    beg = ClauseCodeToLogUpdClause(beg)->ClNext->ClCode;
  }
}

static ClauseDef *
install_log_upd_clauseseq(ClauseDef *cls, PredEntry *ap, istack_entry *stack, yamop **beg, yamop **end)
{
  istack_entry *sp = stack;

  if (stack[0].pos == 0) {
    while (TRUE) {
      if (beg) {
	cls->Code = cls->CurrentCode = *beg;
	cls->Tag = 0;
	cls++;
      }
      beg++;
      if (beg == end) {
	return cls-1;
      }
    }
  }
  while (TRUE) {
    if (*beg) {
      cls->Code =  cls->CurrentCode = *beg;
      sp = install_log_upd_clause(cls, ap, stack);
      /* we reached a matching clause */
      if (!sp->pos && (sp[-1].val == 0L || cls->Tag == sp[-1].val)) {
	cls++;
      }
    }
    beg++;
    if (beg == end) {
      return cls-1;
    }
  }
}

static void
reinstall_log_upd_clauses(ClauseDef *cls, ClauseDef *end, PredEntry *ap, istack_entry *stack)
{
  do {
    cls->CurrentCode = cls->Code;
    install_log_upd_clause(cls, ap, stack);
  } while (cls++ != end);
}

#if PRECOMPUTE_REGADDRESS

#define arg_from_x(I)		(((CELL *)(I))-XREGS)

#else

#define arg_from_x(I)		(I)

#endif /* ALIGN_LONGS */

static AtomSwiEntry *
lookup_c(Term t, yamop *tab, COUNT entries)
{
  AtomSwiEntry *cebase = (AtomSwiEntry *)tab;

  while (cebase->Tag != t) {
    entries--;
    cebase++;
    if (entries == 0)
      return cebase;
  }
  return cebase;
}

static FuncSwiEntry *
lookup_f(Functor f, yamop *tab, COUNT entries)
{
  FuncSwiEntry *febase = (FuncSwiEntry *)tab;
                                                
  while (febase->Tag != f) {
    entries--;
    febase++;
    if (entries == 0)
      return febase;
  }
  return febase;
}

static COUNT
count_clauses_left(yamop *cl, PredEntry *ap)
{
  if (ap->PredFlags & LogUpdatePredFlag) {
    LogUpdClause *c = ClauseCodeToLogUpdClause(cl);
    COUNT i = 0;
 
    while (c != NULL) {
      i++;
      c = c->ClNext;
    }
    return i;
  } else if (ap->PredFlags & MegaClausePredFlag) {
    MegaClause *mcl = ClauseCodeToMegaClause(ap->cs.p_code.FirstClause);
    UInt ncls = mcl->ClSize/mcl->ClItemSize;

    return (ncls-1)-((char *)cl-(char *)mcl->ClCode)/mcl->ClItemSize;
  } else {
    yamop *last = ap->cs.p_code.LastClause;
    StaticClause *c;
    COUNT i = 1;

    c = ClauseCodeToStaticClause(cl);
    while (c->ClCode != last) {
      i++;
      c = c->ClNext;
    }
    return i;
  }
}

/*
  We have jumped across indexing code. Check if we jumped within the current
  indexing block, if we moved back to a parent, or if we jumped to a child.
 */
static ClausePointer
index_jmp(ClausePointer cur, ClausePointer parent, yamop *ipc, int is_lu, yamop *e_code)
{
  if (cur.lui == NULL ||
      ipc == FAILCODE ||
      ipc == e_code ||
      ipc->opc == Yap_opcode(_expand_clauses)
      )
    return cur;
  if (is_lu) {
    LogUpdIndex *lcur = cur.lui, *ncur;
    /* check myself */
    if (ipc >= lcur->ClCode && ipc < (yamop *)((CODEADDR)lcur+lcur->ClSize))
      return cur;
    /* check if I am returning back to a parent, eg 
       switch with intermediate node */
    if (lcur->ParentIndex) {
      LogUpdIndex *pcur = lcur->ParentIndex;
      if (ipc >= pcur->ClCode && ipc < (yamop *)((CODEADDR)pcur+pcur->ClSize)) {
	cur.lui = pcur;
	return cur;
      }
    }
    /* maybe I am a new group */
    ncur = ClauseCodeToLogUpdIndex(ipc);
    if (ncur->ParentIndex != lcur) {
#ifdef DEBUG
      fprintf(stderr,"OOPS, bad parent in lu index\n");
#endif
      cur.lui = NULL;
      return cur;
    }
    cur.lui = ncur;
    return cur;    
  } else {
    StaticIndex *scur = parent.si, *ncur;
    /* check myself */
    if (!scur)
      return cur;
    if (ipc >= scur->ClCode &&
	ipc < (yamop *)((CODEADDR)scur+scur->ClSize))   
      return cur;
    ncur = ClauseCodeToStaticIndex(ipc);
    if (ncur->ClPred == scur->ClPred) {
      cur.si = ncur;
      return cur;
    }
    /*
    if (parent.si != cur.si) {
      if (parent.si) {
	StaticIndex *pcur = parent.si;
	if (ipc >= pcur->ClCode && ipc < (yamop *)((CODEADDR)pcur+pcur->ClSize))
	  return parent;
      }
    }
    cur.si = ncur;
    return cur;
    */
    cur.si = NULL;
    return cur;
  }
}

static ClausePointer
code_to_indexcl(yamop *ipc, int is_lu)
{
  ClausePointer ret;
  if (is_lu)
    ret.lui = ClauseCodeToLogUpdIndex(ipc);
  else
    ret.si = ClauseCodeToStaticIndex(ipc);
  return ret;    
}

static yamop **
expand_index(struct intermediates *cint) {
  /* first clause */
  PredEntry *ap = cint->CurrentPred;
  yamop *first, *last = NULL, *alt = NULL;
  istack_entry *stack, *sp;
  ClauseDef *cls = (ClauseDef *)H, *max;
  int NClauses;
  /* last clause to experiment with */
  yamop *ipc;
  /* labp should point at the beginning of the sequence */
  yamop **labp = NULL;
  ClausePointer parentcl;
  Term t = TermNil, *s_reg = NULL;
  int is_last_arg = TRUE;
  int argno = 1;
  int isfirstcl = TRUE;
  /* this is will be used as a new PC */
  CELL *top = (CELL *) TR;
  UInt arity = 0;
  UInt lab, fail_l, clleft, i = 0;
  int is_lu = ap->PredFlags & LogUpdatePredFlag;
  yamop *eblk = NULL;
  yamop *e_code = (yamop *)&(ap->cs.p_code.ExpandCode);

  ipc = ap->cs.p_code.TrueCodeOfPred;
  first = ap->cs.p_code.FirstClause;
  NClauses = ap->cs.p_code.NOfClauses;
  sp = stack = (istack_entry *)top;
  cint->i_labelno = 1;
  stack[0].pos = 0;
  /* try to refine the interval using the indexing code */

  parentcl = code_to_indexcl(ipc,is_lu);
  while (ipc != NULL) {
    op_numbers op;

    op = Yap_op_from_opcode(ipc->opc);
    switch(op) {
    case _try_clause:
    case _retry:
      /* this clause had no indexing */
      if (ap->PredFlags & LogUpdatePredFlag) {
	first = ClauseCodeToLogUpdClause(ipc->u.Otapl.d)->ClNext->ClCode;
      } else if (ap->PredFlags & MegaClausePredFlag) {
	MegaClause *mcl = ClauseCodeToMegaClause(ap->cs.p_code.FirstClause);
	first = (yamop *)((char *)ipc->u.Otapl.d)+mcl->ClItemSize;
      } else {
	first = ClauseCodeToStaticClause(ipc->u.Otapl.d)->ClNext->ClCode;
      }
      isfirstcl = FALSE;
      ipc = NEXTOP(ipc,Otapl);
      break;
#if TABLING
    case _table_try:
    case _table_retry:
      /* this clause had no indexing */
      first = ClauseCodeToStaticClause(PREVOP(ipc->u.Otapl.d,Otapl))->ClNext->ClCode;
      isfirstcl = FALSE;
      ipc = NEXTOP(ipc,Otapl);
      break;      
#endif /* TABLING */
    case _try_clause2:
    case _try_clause3:
    case _try_clause4:
    case _retry2:
    case _retry3:
    case _retry4:
    case _try_in:
      if (ap->PredFlags & LogUpdatePredFlag) {
	first = ClauseCodeToLogUpdClause(ipc->u.l.l)->ClNext->ClCode;
      } else if (ap->PredFlags & MegaClausePredFlag) {
	MegaClause *mcl = ClauseCodeToMegaClause(ap->cs.p_code.FirstClause);
	first = (yamop *)((char *)ipc->u.Otapl.d)+mcl->ClItemSize;
      } else {
	first = ClauseCodeToStaticClause(ipc->u.l.l)->ClNext->ClCode;
      }
      isfirstcl = FALSE;
      ipc = NEXTOP(ipc,l);
      break;
    case _retry_me:
#ifdef TABLING
    case _table_retry_me:
#endif
      isfirstcl = FALSE;
    case _try_me:
#ifdef TABLING
    case _table_try_me:
#endif
      /* ok, we found the start for an indexing block,
	 but we don't if we are going to operate here or not */
      /* if we are to commit here, alt will tell us where */
      alt = ipc->u.Otapl.d;
      ipc = NEXTOP(ipc,Otapl);
      /* start of a group, reset stack */
      sp = stack;
      stack[0].pos = 0;
      break;
    case _profiled_trust_me:
    case _trust_me:
    case _count_trust_me:
#ifdef TABLING
    case _table_trust_me:
#endif /* TABLING */
      /* we will commit to this group for sure */
      ipc = NEXTOP(ipc,Otapl);
      alt = NULL;
      /* start of a group, reset stack */
      sp = stack;
      stack[0].pos = 0;
      break;
    case _trust:
      /* we should never be here */
      Yap_Error(INTERNAL_COMPILER_ERROR, TermNil, "found trust in expand_index");
      labp =  NULL;
      ipc = NULL;
      break;
      /* should we ever be here ? I think not */
    case _try_logical:
    case _retry_logical:
    case _count_retry_logical:
    case _profiled_retry_logical:
      ipc = ipc->u.OtaLl.n;
      break;
    case _trust_logical:
    case _count_trust_logical:
    case _profiled_trust_logical:
      ipc = ipc->u.OtILl.n;
      break;
    case _enter_lu_pred:
      /* no useful info */
      ipc = ipc->u.Ills.l1;
      break;
    case _retry_profiled:
    case _count_retry:
      /* no useful info */
      ipc = NEXTOP(ipc,l);
      break;
    case _jump:
      /* just skip for now, but should worry about memory management */
      ipc = ipc->u.l.l;
      /* I don't know how up I will go */
      parentcl.si = NULL;
      break;
    case _lock_lu:
    case _procceed:
      ipc = NEXTOP(ipc,p);
      break;
    case _unlock_lu:
      ipc = NEXTOP(ipc,e);
      break;
    case _jump_if_var:
      if (IsVarTerm(Deref(ARG1))) {
	labp = &(ipc->u.l.l);
	ipc = ipc->u.l.l;
	parentcl = index_jmp(parentcl, parentcl, ipc, is_lu, e_code);
      } else {
	ipc = NEXTOP(ipc,l);
      }
      break;
    case _jump_if_nonvar:
      argno = arg_from_x(ipc->u.xll.x);
      t = Deref(XREGS[argno]);
      i = 0;
      /* expand_index expects to find the new argument */
      if (!IsVarTerm(t)) {
	argno--;
	labp = &(ipc->u.xll.l1);
	ipc = ipc->u.xll.l1;
	parentcl = index_jmp(parentcl, parentcl, ipc, is_lu, e_code);
      } else {
	ipc = NEXTOP(ipc,xll);
      }
      break;
      /* instructions type EC */
      /* instructions type e */
    case _index_dbref:
      t = AbsAppl(s_reg-1);
      sp[-1].extra = t;
      s_reg = NULL;
      ipc = NEXTOP(ipc,e);
      break;
    case _index_blob:
#if SIZEOF_DOUBLE == 2*SIZEOF_LONG_INT
      t = MkIntTerm(s_reg[0]^s_reg[1]);
#else
      t = MkIntTerm(s_reg[0]);
#endif
      sp[-1].extra = AbsAppl(s_reg-1);
      s_reg = NULL;
      ipc = NEXTOP(ipc,e);
      break;
    case _index_long:
      t = MkIntTerm((s_reg[0] & (MAX_ABS_INT-1)));
      sp[-1].extra = AbsAppl(s_reg-1);
      s_reg = NULL;
      ipc = NEXTOP(ipc,e);
      break;
    case _user_switch:
      labp = &(ipc->u.lp.l);
      ipc = ipc->u.lp.l;
      break;
      /* instructions type e */
    case _switch_on_type:
      t = Deref(ARG1);
      argno = 1;
      i = 0;
      if (IsVarTerm(t)) {
	labp = &(ipc->u.llll.l4);
	ipc = ipc->u.llll.l4;
      } else if (IsPairTerm(t)) {
	sp = push_stack(sp, 1, AbsPair(NULL), TermNil, cint);
	s_reg = RepPair(t);
	labp = &(ipc->u.llll.l1);
	ipc = ipc->u.llll.l1;	
      } else if (IsApplTerm(t)) {
	sp = push_stack(sp, 1, AbsAppl((CELL *)FunctorOfTerm(t)), TermNil, cint);
	ipc = ipc->u.llll.l3;	
      } else {
	sp = push_stack(sp, argno, t, TermNil, cint);
	ipc = ipc->u.llll.l2;	
      }
      parentcl = index_jmp(parentcl, parentcl, ipc, is_lu, e_code);
      break;
    case _switch_list_nl:
      t = Deref(ARG1);
      argno = 1;
      i = 0;
      if (IsVarTerm(t)) {	
	labp = &(ipc->u.ollll.l4);
	ipc = ipc->u.ollll.l4;
      } else if (IsPairTerm(t)) {
	s_reg = RepPair(t);
	labp = &(ipc->u.ollll.l1);
	sp = push_stack(sp, 1, AbsPair(NULL), TermNil, cint);
	ipc = ipc->u.ollll.l1;	
      } else if (t == TermNil) {
	sp = push_stack(sp, 1, t, TermNil, cint);
	ipc = ipc->u.ollll.l2;	
      } else {
	Term tn;

	if (IsApplTerm(t)) {
	  tn = AbsAppl((CELL *)FunctorOfTerm(t));
	} else {
	  tn = t;
	}
	sp = push_stack(sp, argno, tn, TermNil, cint);
	ipc = ipc->u.ollll.l3;	
      }
      parentcl = index_jmp(parentcl, parentcl, ipc, is_lu, e_code);
      break;
    case _switch_on_arg_type:
      argno = arg_from_x(ipc->u.xllll.x);
      i = 0;
      t = Deref(XREGS[argno]);
      if (IsVarTerm(t)) {
	labp = &(ipc->u.xllll.l4);
	ipc = ipc->u.xllll.l4;
      } else if (IsPairTerm(t)) {
	s_reg = RepPair(t);
	sp = push_stack(sp, argno, AbsPair(NULL), TermNil, cint);
	labp = &(ipc->u.xllll.l1);
	ipc = ipc->u.xllll.l1;	
      } else if (IsApplTerm(t)) {
	sp = push_stack(sp, argno, AbsAppl((CELL *)FunctorOfTerm(t)), TermNil, cint);
	ipc = ipc->u.xllll.l3;	
      } else {
	sp = push_stack(sp, argno, t, TermNil, cint);
	ipc = ipc->u.xllll.l2;	
      }
      parentcl = index_jmp(parentcl, parentcl, ipc, is_lu, e_code);
      break;
    case _switch_on_sub_arg_type:
      i = ipc->u.sllll.s;
      t = Deref(s_reg[i]);
      if (i != arity-1) is_last_arg = FALSE;
      t = Deref(s_reg[i]);
      if (IsVarTerm(t)) {
	labp = &(ipc->u.sllll.l4);
	ipc = ipc->u.sllll.l4;
	i++;
      } else if (IsPairTerm(t)) {
	s_reg = RepPair(t);
	sp = push_stack(sp, -i-1, AbsPair(NULL), TermNil, cint);
	labp = &(ipc->u.sllll.l1);
	ipc = ipc->u.sllll.l1;
	i = 0;
      } else if (IsApplTerm(t)) {
	sp = push_stack(sp, -i-1, AbsAppl((CELL *)FunctorOfTerm(t)), TermNil, cint);
	ipc = ipc->u.sllll.l3;
	i = 0;
      } else {
	/* We don't push stack here, instead we go over to next argument
	   sp = push_stack(sp, -i-1, t, cint);
	*/
	sp = push_stack(sp, -i-1, t, TermNil, cint);
	ipc = ipc->u.sllll.l2;	
	i++;
      }
      parentcl = index_jmp(parentcl, parentcl, ipc, is_lu, e_code);
      break;
    case _if_not_then:
      labp = NULL;
      ipc = NULL;
      break;
      /* instructions type ollll */
    case _switch_on_func:
    case _if_func:
    case _go_on_func:
      {
	FuncSwiEntry *fe;
	yamop *newpc;
	Functor f;

	s_reg = RepAppl(t);
	f = (Functor)(*s_reg++);
	if (op == _switch_on_func) {
	  fe = lookup_f_hash(f,ipc->u.sssl.l,ipc->u.sssl.s);
	} else {
	  fe = lookup_f(f,ipc->u.sssl.l,ipc->u.sssl.s);
	}
	newpc = fe->u.labp;

	labp = &(fe->u.labp);
	if (newpc == e_code) {
	  /* we found it */
	  parentcl = code_to_indexcl(ipc->u.sssl.l,is_lu);
	  ipc = NULL;
	} else {
	  ClausePointer npar = code_to_indexcl(ipc->u.sssl.l,is_lu);
	  ipc = newpc;
	  parentcl = index_jmp(npar, parentcl, ipc, is_lu, e_code);
	}
      }
      break;
    case _switch_on_cons:
    case _if_cons:
    case _go_on_cons:
      {
	AtomSwiEntry *ae;

	if (op == _switch_on_cons) {
	  ae = lookup_c_hash(t,ipc->u.sssl.l,ipc->u.sssl.s);
	} else {
	  ae = lookup_c(t,ipc->u.sssl.l,ipc->u.sssl.s);
	}

	labp = &(ae->u.labp);
	if (ae->u.labp == e_code) {
	  /* we found it */
	  parentcl = code_to_indexcl(ipc->u.sssl.l,is_lu);
	  ipc = NULL;
	} else {
	  ClausePointer npar = code_to_indexcl(ipc->u.sssl.l,is_lu);
	  ipc = ae->u.labp;
	  parentcl = index_jmp(npar, parentcl, ipc, is_lu, e_code);
	}
      }
      break;
    case _expand_index:
    case _expand_clauses:
      if (alt != NULL && ap->PredFlags & LogUpdatePredFlag) {
	op_numbers fop = Yap_op_from_opcode(alt->opc);
	if (fop == _enter_lu_pred) 
	  alt = alt->u.Ills.l1;
      }
      ipc = NULL;
      break;
    case _op_fail:
      ipc = alt;
      alt = NULL;
      break;
    default:
      if (alt == NULL) {
	Yap_Error(INTERNAL_COMPILER_ERROR,t,"unexpected instruction %d at expand_index ", op);
	labp = NULL;
	ipc = NULL;
      } else {
	/* backtrack */
	first = alt->u.Otapl.d;
	ipc = alt;
	alt = NULL;
      }
    }
  }

  /* if there was an overflow while generating the code, make sure
     S is still correct */
  if (is_lu) {
    cint->current_cl.lui = parentcl.lui;
  } else {
    cint->current_cl.si = parentcl.si;
  }
  if (s_reg != NULL)
    S = s_reg;
#ifdef TABLING
  /* handle tabling hack that insertes a failcode, 
     this really corresponds to not having any more clauses */
  if (alt == TRUSTFAILCODE)
    alt = NULL;
#endif
  if (alt == NULL) {
    /* oops, we are at last clause */
    fail_l = (UInt)FAILCODE;
    clleft = 0;
    last = ap->cs.p_code.LastClause;
  } else {
    if (ap->PredFlags & LogUpdatePredFlag) {
      op_numbers op = Yap_op_from_opcode(alt->opc);
      /* can we be here */
	if (op >= _retry2 && op <= _retry4) {
	last = alt->u.l.l;
      } else {
	last = alt->u.Otapl.d;
      }
    } else {
      op_numbers op = Yap_op_from_opcode(alt->opc);
      if (op == _retry || op == _trust) {
	last = alt->u.Otapl.d;
#ifdef TABLING
      } else if (op == _table_retry || op == _table_trust) {
	last = PREVOP(alt->u.Otapl.d,Otapl);
#endif /* TABLING */
      } else if (op >= _retry2 && op <= _retry4) {
	last = alt->u.l.l;
      }
    }
    fail_l = (UInt)alt;
    clleft = count_clauses_left(last,ap);
  }

  if (Yap_op_from_opcode((*labp)->opc) == _expand_clauses) {
    /* ok, we know how many clauses */
    yamop *ipc = *labp;
    /* check all slots, not just the ones with values */
    COUNT nclauses = ipc->u.sssllp.s1;
    yamop **clp = (yamop **)NEXTOP(ipc,sssllp);

    eblk = cint->expand_block = ipc;
    if (cls+2*nclauses > (ClauseDef *)(ASP-4096)) {
      /* tell how much space we need (worst case) */
      Yap_Error_Size += 2*NClauses*sizeof(ClauseDef);
      /* grow stack */
      save_machine_regs();
      longjmp(cint->CompilerBotch,3);
    }
    if (ap->PredFlags & LogUpdatePredFlag) {
      max = install_log_upd_clauseseq(cls, ap, stack, clp, clp+nclauses);
    } else {
      max = install_clauseseq(cls, ap, stack, clp, clp+nclauses);
    }    
  } else {
    cint->expand_block = NULL;
    if (cls+2*NClauses > (ClauseDef *)(ASP-4096)) {
      /* tell how much space we need (worst case) */
      Yap_Error_Size += 2*NClauses*sizeof(ClauseDef);
      save_machine_regs();
      longjmp(cint->CompilerBotch,3);
    }
    if (ap->PredFlags & LogUpdatePredFlag) {
      max = install_log_upd_clauses(cls, ap, stack, first, last);
    } else {
      max = install_clauses(cls, ap, stack, first, last);
    }
#if DEBUG_EXPAND
    if (ap->PredFlags & LogUpdatePredFlag) {
      fprintf(stderr,"vsc +");
    } else {
      fprintf(stderr,"vsc ");
    }
    fprintf(stderr,"  : expanding %d out of %d\n", (max-cls)+1,NClauses);
#endif
  }
  /* don't count last clause if you don't have to */
  if (alt && max->Code == last) max--;
  if (max < cls && labp != NULL) {
      *labp = FAILCODE;
    return labp;
  }
  cint->freep = (char *)(max+1);
  cint->CodeStart = cint->BlobsStart = cint->cpc = cint->icpc = NULL;
  
  if (!IsVarTerm(sp[-1].val)  && sp > stack) {
    if (IsAtomOrIntTerm(sp[-1].val)) {
      if (s_reg == NULL) { /* we have not yet looked into terms */
	lab = do_index(cls, max, cint, argno+1, fail_l, isfirstcl, clleft, top);
      } else {
	UInt arity = 0;

	if (ap->PredFlags & LogUpdatePredFlag) {
	  reinstall_log_upd_clauses(cls, max, ap, stack);
	} else {
	  reinstall_clauses(cls, max, ap, stack);
	}
	sp--;
	while (sp > stack) {
	  Term t = sp[-1].val;
	  if (IsApplTerm(t)) {
	    Functor f = (Functor)RepAppl(t);
	    if (!IsExtensionFunctor(f)) {
	      arity = ArityOfFunctor(f);
	      break;
	    } else {
	      sp--;
	    }
	  } else if (IsPairTerm(t)) {
	    arity = 2;
	    break;
	  } else {
	    sp--;
	  }
	}
	lab = do_compound_index(cls, max, s_reg, cint, i, arity, argno, fail_l, isfirstcl, is_last_arg, clleft, top, FALSE);
      }
    } else if (IsPairTerm(sp[-1].val) && sp > stack) {
      lab = do_compound_index(cls, max, s_reg, cint, i, 2, argno, fail_l, isfirstcl, is_last_arg, clleft, top, FALSE);
    } else {
      Functor f = (Functor)RepAppl(sp[-1].val);
      /* we are continuing within a compound term */
      if (IsExtensionFunctor(f)) {
	lab = do_index(cls, max, cint, argno+1, fail_l, isfirstcl, clleft, top);
      } else {
	lab = do_compound_index(cls, max, s_reg, cint, i, ArityOfFunctor(f), argno, fail_l, isfirstcl, is_last_arg, clleft, top, FALSE);
      }
    }
  } else {
    if (argno == ap->ArityOfPE) {
      lab = 
	do_var_clauses(cls, max, FALSE, cint, isfirstcl, clleft, fail_l, ap->ArityOfPE+1);
    } else {
      lab = do_index(cls, max, cint, argno+1, fail_l, isfirstcl, clleft, top);
    }
  }
  if (labp && !(lab & 1)) {
    *labp = (yamop *)lab; /* in case we have a single clause */
  }
  return labp;
}


static yamop *
ExpandIndex(PredEntry *ap, int ExtraArgs, yamop *nextop) {
  yamop *indx_out, *expand_clauses;
  yamop **labp;
  int cb;
  struct intermediates cint;

  cint.code_addr = NULL;
  if ((cb = setjmp(cint.CompilerBotch)) == 3) {
    restore_machine_regs();
    /* grow stack */
    recover_from_failed_susp_on_cls(&cint, 0);
    Yap_gcl(Yap_Error_Size, ap->ArityOfPE+ExtraArgs, ENV, nextop);
  } else if (cb == 2) {
    restore_machine_regs();
    Yap_Error_Size = recover_from_failed_susp_on_cls(&cint, Yap_Error_Size);
    if (!Yap_growheap(FALSE, Yap_Error_Size, NULL)) {
      save_machine_regs();
      if (ap->PredFlags & LogUpdatePredFlag) {
	Yap_kill_iblock((ClauseUnion *)ClauseCodeToLogUpdIndex(ap->cs.p_code.TrueCodeOfPred),NULL, ap);
      } else {
	StaticIndex *cl;

	cl = ClauseCodeToStaticIndex(ap->cs.p_code.TrueCodeOfPred);
	Yap_kill_iblock((ClauseUnion *)ClauseCodeToStaticIndex(ap->cs.p_code.TrueCodeOfPred),NULL, ap);
      }
#if defined(YAPOR) || defined(THREADS)
      if (ap->PredFlags & LogUpdatePredFlag &&
	  ap->ModuleOfPred != IDB_MODULE) {
	ap->OpcodeOfPred = LOCKPRED_OPCODE;
	ap->cs.p_code.TrueCodeOfPred = ap->CodeOfPred = (yamop *)(&(ap->OpcodeOfPred)); 
      } else {
#endif
	ap->OpcodeOfPred = INDEX_OPCODE;
	ap->CodeOfPred = ap->cs.p_code.TrueCodeOfPred = (yamop *)(&(ap->OpcodeOfPred)); 
#if defined(YAPOR) || defined(THREADS)
      }
#endif
      Yap_Error(OUT_OF_HEAP_ERROR, TermNil, Yap_ErrorMessage);
      return FAILCODE;
    }
  } else if (cb == 4) {
    restore_machine_regs();
    if (!Yap_growtrail(Yap_Error_Size, FALSE)) {
      save_machine_regs();
      if (ap->PredFlags & LogUpdatePredFlag) {
	Yap_kill_iblock((ClauseUnion *)ClauseCodeToLogUpdIndex(ap->cs.p_code.TrueCodeOfPred),NULL, ap);
      } else {
	StaticIndex *cl;

	cl = ClauseCodeToStaticIndex(ap->cs.p_code.TrueCodeOfPred);
	Yap_kill_iblock((ClauseUnion *)cl, NULL, ap);
      }
      return FAILCODE;
    }
  }
 restart_index:
  cint.CodeStart = cint.cpc = cint.BlobsStart = cint.icpc = NIL;
  cint.CurrentPred = ap;
  Yap_ErrorMessage = NULL;
  Yap_Error_Size = 0;
  if (P->opc == Yap_opcode(_expand_clauses)) {
    expand_clauses = P;
  } else {
    expand_clauses = NULL;
  }
#ifdef DEBUG
  if (Yap_Option['i' - 'a' + 1]) {
    Term tmod = ap->ModuleOfPred;
    Yap_LockStream(Yap_c_error_stream);
    if (!tmod) tmod = TermProlog;
#if THREADS
    Yap_DebugPlWrite(MkIntegerTerm(worker_id));
    Yap_DebugPutc(Yap_c_error_stream,' ');
#endif
    Yap_DebugPutc(Yap_c_error_stream,'>');
    Yap_DebugPutc(Yap_c_error_stream,'\t');
    Yap_DebugPlWrite(tmod);
    Yap_DebugPutc(Yap_c_error_stream,':');
    if (ap->ModuleOfPred == IDB_MODULE) {
      Term t = Deref(ARG1);
      if (IsAtomTerm(t)) {
	Yap_DebugPlWrite(t);
      } else if (IsIntegerTerm(t)) {
	Yap_DebugPlWrite(t);
      } else {
	Functor f = FunctorOfTerm(t);
	Atom At = NameOfFunctor(f);
	Yap_DebugPlWrite(MkAtomTerm(At));
	Yap_DebugPutc(Yap_c_error_stream,'/');
	Yap_DebugPlWrite(MkIntegerTerm(ArityOfFunctor(f)));
      }
    } else {
      if (ap->ArityOfPE == 0) {
	Atom At = (Atom)ap->FunctorOfPred;
	Yap_DebugPlWrite(MkAtomTerm(At));
      } else {
	Functor f = ap->FunctorOfPred;
	Atom At = NameOfFunctor(f);
	Yap_DebugPlWrite(MkAtomTerm(At));
	Yap_DebugPutc(Yap_c_error_stream,'/');
	Yap_DebugPlWrite(MkIntegerTerm(ArityOfFunctor(f)));
      }
      Yap_UnLockStream(Yap_c_error_stream);
    }
    Yap_DebugPutc(Yap_c_error_stream,'\n');
#if THREADS
    Yap_DebugPlWrite(MkIntegerTerm(worker_id));
    Yap_DebugPutc(Yap_c_error_stream,' ');
#endif
    Yap_UnLockStream(Yap_c_error_stream);
  }
#endif
  if ((labp = expand_index(&cint)) == NULL) {
    if (expand_clauses) {
      P = FAILCODE;
      recover_ecls_block(expand_clauses);
    }
    return FAILCODE;
  }
  if (*labp == FAILCODE) {
    if (expand_clauses) {
      P = FAILCODE;
      recover_ecls_block(expand_clauses);
    }
    return FAILCODE;
  }
#ifdef DEBUG
  if (Yap_Option['i' - 'a' + 1]) {
    Yap_LockStream(Yap_c_error_stream);
    Yap_ShowCode(&cint);
    Yap_UnLockStream(Yap_c_error_stream);
  }
#endif
  /* globals for assembler */
  IPredArity = ap->ArityOfPE;
  if (cint.CodeStart) {
    if ((indx_out = Yap_assemble(ASSEMBLING_EINDEX, TermNil, ap, FALSE, &cint)) == NULL) {
      if (!Yap_growheap(FALSE, Yap_Error_Size, NULL)) {
	Yap_Error(OUT_OF_HEAP_ERROR, TermNil, Yap_ErrorMessage);
	return FAILCODE;
      }
      goto restart_index;
    }
  } else {
    /* single case */
    if (expand_clauses) {
      P = *labp;
      recover_ecls_block(expand_clauses);
    }
    return *labp;
  }
  if (indx_out == NULL) {
    if (expand_clauses) {
      P = FAILCODE;
      recover_ecls_block(expand_clauses);
    }
    return FAILCODE;
  }
  *labp = indx_out;
  if (ap->PredFlags & LogUpdatePredFlag) {
    /* add to head of current code children */
    LogUpdIndex *ic = cint.current_cl.lui,
      *nic = ClauseCodeToLogUpdIndex(indx_out);
    if (ic == NULL)
      ic = (LogUpdIndex *)Yap_find_owner_index((yamop *)labp, ap);
    /* insert myself in the indexing code chain */ 
    nic->SiblingIndex = ic->ChildIndex;
    nic->PrevSiblingIndex = NULL;
    if (ic->ChildIndex) {
      ic->ChildIndex->PrevSiblingIndex = nic;
    }
    nic->ParentIndex = ic;
    nic->ClFlags &= ~SwitchRootMask;
    ic->ChildIndex = nic;
    ic->ClRefCount++;
  } else {
    /* add to head of current code children */
    StaticIndex *ic = cint.current_cl.si,
      *nic = ClauseCodeToStaticIndex(indx_out);
    if (ic == NULL)
      ic = (StaticIndex *)Yap_find_owner_index((yamop *)labp, ap);
    /* insert myself in the indexing code chain */ 
    nic->SiblingIndex = ic->ChildIndex;
    ic->ChildIndex = nic;
  }
  if (expand_clauses) {
    P = indx_out;
    recover_ecls_block(expand_clauses);
  }
  return indx_out;
}

yamop *
Yap_ExpandIndex(PredEntry *ap, UInt nargs) {
  return ExpandIndex(ap, nargs, CP);
}

static path_stack_entry *
push_path(path_stack_entry *sp, yamop **pipc, ClauseDef *clp, struct intermediates *cint)
{
  if (sp+1 > (path_stack_entry *)Yap_TrailTop) {
    save_machine_regs();
    longjmp(cint->CompilerBotch,4);    
  }
  sp->flag = pc_entry;
  sp->u.pce.pi_pc = pipc;
  sp->u.pce.code = clp->Code;
  sp->u.pce.current_code = clp->CurrentCode;
  sp->u.pce.work_pc = clp->u.WorkPC;
  sp->u.pce.tag = clp->Tag;
  return sp+1;
}
		 
static path_stack_entry *
fetch_new_block(path_stack_entry *sp, yamop **pipc, PredEntry *ap, struct intermediates *cint)
{
  if (sp+1 > (path_stack_entry *)Yap_TrailTop) {
    save_machine_regs();
    longjmp(cint->CompilerBotch,4);    
  }
  /* add current position */
  sp->flag = block_entry;
  sp->u.cle.entry_code = pipc;
  if (ap->PredFlags & LogUpdatePredFlag) {
    sp->u.cle.block = (ClauseUnion *)ClauseCodeToLogUpdIndex(*pipc);
  } else {
    sp->u.cle.block = (ClauseUnion *)ClauseCodeToStaticIndex(*pipc);
  }
  return sp+1;
}
		 
static path_stack_entry *
init_block_stack(path_stack_entry *sp, yamop *ipc, PredEntry *ap)
{
  /* add current position */
  
  sp->flag = block_entry;
  sp->u.cle.entry_code = NULL;
  if (ap->PredFlags & LogUpdatePredFlag) {
    sp->u.cle.block = (ClauseUnion *)ClauseCodeToLogUpdIndex(ipc);
  } else {
    sp->u.cle.block = (ClauseUnion *)ClauseCodeToStaticIndex(ipc);
  }
  return sp+1;
}

static path_stack_entry  *
cross_block(path_stack_entry *sp, yamop **pipc, PredEntry *ap, struct intermediates *cint)
{
  yamop *ipc = *pipc;
  path_stack_entry *tsp = sp;
  ClauseUnion *block;

  do {
    UInt bsize;
    while ((--tsp)->flag != block_entry);
    block = tsp->u.cle.block;
    if (block->lui.ClFlags & LogUpdMask)
      bsize = block->lui.ClSize;
    else
      bsize = block->si.ClSize;
    if (ipc > (yamop *)block &&
	ipc < (yamop *)((CODEADDR)block + bsize)) {
      path_stack_entry *nsp = tsp+1;
      for (;tsp<sp;tsp++) {
	if (tsp->flag == pc_entry) {
	  if (nsp != tsp) {
	    nsp->flag = pc_entry;
	    nsp->u.pce.pi_pc = tsp->u.pce.pi_pc;
	    nsp->u.pce.code = tsp->u.pce.code;
	    nsp->u.pce.current_code = tsp->u.pce.current_code;
	    nsp->u.pce.work_pc = tsp->u.pce.work_pc;
	    nsp->u.pce.tag = tsp->u.pce.tag;
	  }
	  nsp++;
	}
      }
      return nsp;
    }
  } while (tsp->u.cle.entry_code != NULL);
  /* moved to a new block */
  return fetch_new_block(sp, pipc, ap, cint);
}


static yamop *
pop_path(path_stack_entry **spp, ClauseDef *clp, PredEntry *ap, struct intermediates *cint)
{
  path_stack_entry *sp = *spp;
  yamop *nipc;

  while ((--sp)->flag != pc_entry);
  *spp = sp;
  clp->Code = sp->u.pce.code;
  clp->CurrentCode = sp->u.pce.current_code;
  clp->u.WorkPC = sp->u.pce.work_pc;
  clp->Tag = sp->u.pce.tag;
  if (sp->u.pce.pi_pc == NULL) {
    *spp = sp;
    return NULL;
  }
  nipc = *(sp->u.pce.pi_pc);
  *spp = cross_block(sp, sp->u.pce.pi_pc, ap, cint);
  return nipc;
}

static int
table_fe_overflow(yamop *pc, Functor f)
{
  if (pc->u.sssl.s <= MIN_HASH_ENTRIES) {
    /* we cannot expand otherwise */
    COUNT i;
    FuncSwiEntry *csw = (FuncSwiEntry *)pc->u.sssl.l;

    for (i=0; i < pc->u.sssl.s; i++,csw++) {
      if (csw->Tag == f) return FALSE;
    }
    return TRUE;
  } else {
    COUNT free = pc->u.sssl.s-pc->u.sssl.e;
    return (!free || pc->u.sssl.s/free > 4);
  }
}

static int
table_ae_overflow(yamop *pc, Term at)
{
  if (pc->u.sssl.s <= MIN_HASH_ENTRIES) {
    /* check if we are already there */
    COUNT i;
    AtomSwiEntry *csw = (AtomSwiEntry *)pc->u.sssl.l;

    for (i=0; i < pc->u.sssl.s; i++,csw++) {
      if (csw->Tag == at) return FALSE;
    }
    return TRUE;
  } else {
    COUNT free = pc->u.sssl.s-pc->u.sssl.e;
    return (!free || pc->u.sssl.s/free > 4);
  }
}

static void
replace_index_block(ClauseUnion *parent_block, yamop *cod, yamop *ncod, PredEntry *ap)
{
  if (ap->PredFlags & LogUpdatePredFlag) {
    LogUpdIndex
      *cl = ClauseCodeToLogUpdIndex(cod),
      *ncl = ClauseCodeToLogUpdIndex(ncod),
      *c = parent_block->lui.ChildIndex;
    ncl->SiblingIndex = cl->SiblingIndex;
    ncl->PrevSiblingIndex = cl->PrevSiblingIndex;
    ncl->ClRefCount = cl->ClRefCount;
    ncl->ChildIndex = cl->ChildIndex;
    ncl->ParentIndex = cl->ParentIndex;
    ncl->ClPred = cl->ClPred;
    //    INIT_LOCK(ncl->ClLock);
    if (c == cl) {
      parent_block->lui.ChildIndex = ncl;
    } else {
      if (cl->PrevSiblingIndex)
	cl->PrevSiblingIndex->SiblingIndex = ncl;
    }
    if (cl->SiblingIndex) {
      cl->SiblingIndex->PrevSiblingIndex = ncl;
    }
    c = cl->ChildIndex;
    while (c != NULL) {
      c->ParentIndex = ncl;
      c = c->SiblingIndex;
    }
    Yap_InformOfRemoval((CODEADDR)cl);
    Yap_LUIndexSpace_SW -= cl->ClSize;
    Yap_FreeCodeSpace((char *)cl);
  } else {
    StaticIndex
      *cl = ClauseCodeToStaticIndex(cod),
      *ncl = ClauseCodeToStaticIndex(ncod),
      *c = parent_block->si.ChildIndex;
    ncl->SiblingIndex = cl->SiblingIndex;
    ncl->ClPred = cl->ClPred;
    if (c == cl) {
      parent_block->si.ChildIndex = ncl;
    } else {
      while (c->SiblingIndex != cl) {
	c = c->SiblingIndex;
      }
      c->SiblingIndex = ncl;
    }
    Yap_InformOfRemoval((CODEADDR)cl);
    Yap_IndexSpace_SW -= cl->ClSize;
    Yap_FreeCodeSpace((char *)cl);
  }
}
		 
static AtomSwiEntry *
expand_ctable(yamop *pc, ClauseUnion *blk, struct intermediates *cint, Term at)
{
  PredEntry *ap = cint->CurrentPred;
  int n = pc->u.sssl.s, i, i0 = n;
  UInt fail_l = Zero;
  AtomSwiEntry *old_ae = (AtomSwiEntry *)(pc->u.sssl.l), *target;

  if (n > MIN_HASH_ENTRIES) {
    AtomSwiEntry *tmp = old_ae;
    int i;
    
    n = 1;
    for (i = 0; i < pc->u.sssl.s; i++,tmp++) {
      if (tmp->Tag != Zero) n++;
      else fail_l = tmp->u.Label;
    }
  } else {
    fail_l = old_ae[n].u.Label;
    n++;
  }
  if (n > MIN_HASH_ENTRIES) {
    int cases = MIN_HASH_ENTRIES, i, n0;
    n0 = n+1+n/4;
    while (cases < n0) cases *= 2;
    if (cases == pc->u.sssl.s) {
      return fetch_centry(old_ae, at, n-1, n);
    }
    /* initialise */
    target = (AtomSwiEntry *)emit_switch_space(cases, sizeof(AtomSwiEntry), cint, 0);
    pc->opc = Yap_opcode(_switch_on_cons);
    pc->u.sssl.s = cases;
    for (i=0; i<cases; i++) {
      target[i].Tag = Zero;
      target[i].u.Label = fail_l;
    }
  } else {
    pc->opc = Yap_opcode(_if_cons);
    pc->u.sssl.s = n;
    target = (AtomSwiEntry *)emit_switch_space(n+1, sizeof(AtomSwiEntry), cint, 0);
    target[n].Tag = Zero;
    target[n].u.Label = fail_l;
  }
  for (i = 0; i < i0; i++,old_ae++) {
    Term tag = old_ae->Tag;

    if (tag != Zero) {
      AtomSwiEntry *ics = fetch_centry(target, tag, i, n);
      ics->Tag = tag;
      ics->u.Label = old_ae->u.Label;    
    }
  }
  /* support for threads */
  if (blk)
    replace_index_block(blk, pc->u.sssl.l, (yamop *)target, ap);
  pc->u.sssl.l = (yamop *)target;
  return fetch_centry(target, at, n-1, n);
}

static FuncSwiEntry *
expand_ftable(yamop *pc, ClauseUnion *blk, struct intermediates *cint, Functor f)
{
  PredEntry *ap = cint->CurrentPred;
  int n = pc->u.sssl.s, i, i0 = n;
  UInt fail_l =  Zero;
  FuncSwiEntry *old_fe = (FuncSwiEntry *)(pc->u.sssl.l), *target;

  if (n > MIN_HASH_ENTRIES) {
    FuncSwiEntry *tmp = old_fe;
    int i;
    
    n = 1;
    for (i = 0; i < pc->u.sssl.s; i++,tmp++) {
      if (tmp->Tag != Zero) n++;
      else fail_l = tmp->u.Label;
    }
  } else {
    fail_l = old_fe[n].u.Label;
    n++;
  }
  if (n > MIN_HASH_ENTRIES) {
    int cases = MIN_HASH_ENTRIES, i, n0;
    n0 = n+1+n/4;
    while (cases < n0) cases *= 2;

    if (cases == pc->u.sssl.s) {
      return fetch_fentry(old_fe, f, n-1, n);
    }
    pc->opc = Yap_opcode(_switch_on_func);
    pc->u.sssl.s = cases;
    pc->u.sssl.e = n;
    pc->u.sssl.w = 0;
    /* initialise */
    target = (FuncSwiEntry *)emit_switch_space(cases, sizeof(FuncSwiEntry), cint, FuncSwitchMask);
    for (i=0; i<cases; i++) {
      target[i].Tag = NULL;
      target[i].u.Label = fail_l;
    }
  } else {
    pc->opc = Yap_opcode(_if_func);
    pc->u.sssl.s = n;
    pc->u.sssl.e = n;
    pc->u.sssl.w = 0;
    target = (FuncSwiEntry *)emit_switch_space(n+1, sizeof(FuncSwiEntry), cint, FuncSwitchMask);
    target[n].Tag = Zero;
    target[n].u.Label = fail_l;
  }
  for (i = 0; i < i0; i++,old_fe++) {
    Functor f = old_fe->Tag;

    if (f != NULL) {
      FuncSwiEntry *ifs = fetch_fentry(target, f, i, n);
      ifs->Tag = old_fe->Tag;
      ifs->u.Label = old_fe->u.Label;    
    }
  }
  replace_index_block(blk, pc->u.sssl.l, (yamop *)target, ap);
  pc->u.sssl.l = (yamop *)target;
  return fetch_fentry(target, f, n-1, n);
}

static void
clean_ref_to_clause(LogUpdClause *tgl)
{
  tgl->ClRefCount--;
  if ((tgl->ClFlags & ErasedMask) &&
      !(tgl->ClRefCount) &&
      !(tgl->ClFlags & InUseMask)) {
    /* last ref to the clause */
    Yap_ErLogUpdCl(tgl);
  }
}


static ClauseUnion *
current_block(path_stack_entry *sp)
{
  while ((--sp)->flag != block_entry);
  return sp->u.cle.block;
}

static path_stack_entry *
kill_block(path_stack_entry *sp, PredEntry *ap)
{
  while ((--sp)->flag != block_entry);
  if (sp->u.cle.entry_code == NULL) {
    Yap_kill_iblock(sp->u.cle.block, NULL, ap);
  } else {
    path_stack_entry *nsp = sp;
    
    while ((--nsp)->flag != block_entry);
    Yap_kill_iblock(sp->u.cle.block, nsp->u.cle.block, ap);
    *sp->u.cle.entry_code = (yamop *)&(ap->cs.p_code.ExpandCode);
  }
  return sp;
}

static LogUpdClause *
find_last_clause(yamop *start)
{
  while (start->u.OtaLl.d->ClFlags & ErasedMask) 
    start = start->u.OtaLl.n;
  /* this should be the available clause */
  return start->u.OtaLl.d;
}

static void
remove_clause_from_index(yamop *header, LogUpdClause *cl)
{
  yamop **prevp = &(header->u.Ills.l1);
  yamop *curp = header->u.Ills.l1;

  if (curp->u.OtaLl.d == cl) {
    yamop *newp = curp->u.OtaLl.n;
    newp->opc = curp->opc;
    *prevp = newp;
  } else {
    yamop *ocurp = NULL, *ocurp0 = curp;

    while (curp->u.OtaLl.d != cl) {
      ocurp = curp;
      curp = curp->u.OtaLl.n;
    }
    /* in case we were the last */
    if (curp == header->u.Ills.l2)
      header->u.Ills.l2 = ocurp;
    if (ocurp != ocurp0)
      ocurp->opc = curp->opc;
    ocurp->u.OtILl.n = curp->u.OtaLl.n;
    ocurp->u.OtILl.block = curp->u.OtILl.block;
  }
#ifdef DEBUG
  Yap_DirtyCps--;
  Yap_FreedCps++;
#endif
  clean_ref_to_clause(cl);
  Yap_LUIndexSpace_CP -= (UInt)NEXTOP((yamop*)NULL,OtILl);
  Yap_FreeCodeSpace((ADDR)curp);
}

static void
remove_dirty_clauses_from_index(yamop *header)
{
  LogUpdClause *cl;
  yamop *previouscurp;
  OPCODE endop = Yap_opcode(_trust_logical);
  yamop **prevp= &(header->u.Ills.l1), *curp = header->u.Ills.l1;
  OPCODE startopc = curp->opc;
  PredEntry *ap = curp->u.OtaLl.d->ClPred;

  if (ap->PredFlags & CountPredFlag)
    endop = Yap_opcode(_count_trust_logical);
  else if (ap->PredFlags & ProfiledPredFlag)
    endop = Yap_opcode(_profiled_trust_logical);
  while ((cl = curp->u.OtaLl.d)->ClFlags & ErasedMask) {
    yamop *ocurp = curp;

#ifdef DEBUG
    Yap_DirtyCps--;
    Yap_FreedCps++;
#endif
    clean_ref_to_clause(cl);
    curp = curp->u.OtaLl.n;
    Yap_LUIndexSpace_CP -= (UInt)NEXTOP((yamop*)NULL,OtaLl);
    Yap_FreeCodeSpace((ADDR)ocurp);
  } 
  *prevp = curp;
  curp->opc = startopc;
  if (curp->opc == endop)
    return;
  previouscurp = curp;
  curp = curp->u.OtaLl.n;
  while (TRUE) {
    if ((cl = curp->u.OtaLl.d)->ClFlags & ErasedMask) {
      yamop *ocurp = curp;

#ifdef DEBUG
      Yap_DirtyCps--;
      Yap_FreedCps++;
#endif
      clean_ref_to_clause(cl);
      if (curp->opc == endop) {
	previouscurp->opc = endop;
	previouscurp->u.OtILl.block = curp->u.OtILl.block;
	previouscurp->u.OtILl.n = NULL;
	header->u.Ills.l2 = previouscurp;
	Yap_LUIndexSpace_CP -= (UInt)NEXTOP((yamop*)NULL,OtILl);
	Yap_FreeCodeSpace((ADDR)curp);
	return;
      }
      previouscurp->u.OtaLl.n = curp->u.OtaLl.n;
      curp = curp->u.OtaLl.n;
      Yap_LUIndexSpace_CP -= (UInt)NEXTOP((yamop*)NULL,OtaLl);
      Yap_FreeCodeSpace((ADDR)ocurp);
    } else {
      previouscurp = curp;
      if (curp->opc == endop) {
	curp->u.OtILl.n = NULL;
	return;
      }
      curp = curp->u.OtaLl.n;
    }
  }
}

static path_stack_entry *
kill_clause(yamop *ipc, yamop *bg, yamop *lt, path_stack_entry *sp0, PredEntry *ap)
{
  LogUpdIndex *blk;
  yamop *start;
  op_numbers op0;
  path_stack_entry *sp = sp0;

  while ((--sp)->flag != block_entry);
  blk = (LogUpdIndex *)(sp->u.cle.block);
  start = blk->ClCode;
  op0 = Yap_op_from_opcode(start->opc);
  while (op0 == _lock_lu) {
    start = NEXTOP(start, p);
    op0 = Yap_op_from_opcode(start->opc);
  }
  while (op0 == _jump_if_nonvar) {
    start = NEXTOP(start, xll);
    op0 = Yap_op_from_opcode(start->opc);
  }
  if (op0 != _enter_lu_pred) {
    /* static code */
    return kill_block(sp+1, ap);
  }
  /* weird case ????? */
  if (!start->u.Ills.s){
    /* ERROR */
    Yap_Error(INTERNAL_ERROR, TermNil, "Ills.s == 0 %p", ipc);
    return sp;
  }
  if (start->u.Ills.s == 1) {
    /* we need to discover which clause is left and then die */
    path_stack_entry *nsp;
    LogUpdClause *tgl = find_last_clause(start->u.Ills.l1);

    nsp = sp;
    while ((--nsp)->flag != block_entry);
    /* make us point straight at clause */
    *sp->u.cle.entry_code = tgl->ClCode;
    Yap_kill_iblock(sp->u.cle.block, nsp->u.cle.block, ap);
    return sp;
  } else {
    if (
#if defined(YAPOR) || defined(THREADS)
	blk->ClRefCount == 0
#else
	!(blk->ClFlags & InUseMask)
#endif
) {
      remove_clause_from_index(start,
			       ClauseCodeToLogUpdClause(bg));
    } else {
      blk->ClFlags |= DirtyMask;
    }
    return sp;
  }
}

static path_stack_entry *
expanda_block(path_stack_entry *sp, PredEntry *ap, ClauseDef *cls, int group1, yamop *alt, struct intermediates *cint)
{
  while ((--sp)->flag != block_entry);
  Yap_kill_iblock(sp->u.cle.block, NULL, ap);
  return sp;
}

static path_stack_entry *
expandz_block(path_stack_entry *sp, PredEntry *ap, ClauseDef *cls, int group1, yamop *alt, struct intermediates *cint)
{
  while ((--sp)->flag != block_entry);
  Yap_kill_iblock(sp->u.cle.block, NULL, ap);
  return sp;
}

static LogUpdClause *
lu_clause(yamop *ipc, PredEntry *ap)
{
  if (ipc == FAILCODE)
    return NULL;
  if (ipc == (yamop *)(&(ap->OpcodeOfPred)))
    return NULL;
  return ClauseCodeToLogUpdClause(ipc);
}

static StaticClause *
find_static_clause(PredEntry *ap, yamop *ipc)
{
  StaticClause *cl = ClauseCodeToStaticClause(ap->cs.p_code.FirstClause);
  while (ipc < cl->ClCode ||
	 ipc > (yamop *)((char *)cl+ cl->ClSize)) {
    cl = cl->ClNext;
    if (!cl)
      return NULL;
  }
  return cl;
}

static StaticClause *
static_clause(yamop *ipc, PredEntry *ap, int trust)
{
  CELL *p;

  if (ipc == FAILCODE)
    return NULL;
  if (ipc == (yamop*)(&(ap->OpcodeOfPred)))
    return NULL;
  if (ap->PredFlags & MegaClausePredFlag)
    return (StaticClause *)ipc;
  if (ap->PredFlags & TabledPredFlag)
    ipc = PREVOP(ipc,Otapl); 
  p = (CELL *)ipc;
  if (trust) {
    return ClauseCodeToStaticClause(p); 
  } else {
    op_numbers op = Yap_op_from_opcode(ipc->opc);
    UInt j;

    /* unbound call, so we cannot optimise instructions */
    switch (op) {
    case _p_db_ref_x:
    case _p_float_x:
      j = Yap_regnotoreg(ipc->u.xl.x);
      break;
    case _get_list:
      j = Yap_regnotoreg(ipc->u.x.x);
      break;
    case _get_atom:
      j = Yap_regnotoreg(ipc->u.xc.x);
      break;
    case _get_float:
      j = Yap_regnotoreg(ipc->u.xd.x);
      break;
    case _get_struct:
      j = Yap_regnotoreg(ipc->u.xd.x);
      break;
    case _get_2atoms:
    case _get_3atoms:
    case _get_4atoms:
    case _get_5atoms:
    case _get_6atoms:
      return ClauseCodeToStaticClause(p);
    default:
      return find_static_clause(ap, ipc);
    }
    if (j == 1) /* must be the first instruction */
      return ClauseCodeToStaticClause(p);
    return find_static_clause(ap, ipc);
  }
  return NULL;
}

static StaticClause *
simple_static_clause(yamop *ipc, PredEntry *ap)
{
  if (ipc == (yamop*)(&(ap->OpcodeOfPred)))
    return NULL;
  if (ipc == FAILCODE)
    return NULL;
  return ClauseCodeToStaticClause(ipc);
}

/* this code should be called when we jumped to clauses */
static path_stack_entry *
kill_unsafe_block(path_stack_entry *sp, op_numbers op, PredEntry *ap, int first, int remove, ClauseDef *cls)
{
  yamop *ipc;
  while ((--sp)->flag != block_entry);
  if (sp->u.cle.entry_code == NULL)
    return sp;
  ipc = *sp->u.cle.entry_code;
  if (Yap_op_from_opcode(ipc->opc) == op) {
    /* the new block was the current clause */
    ClauseDef cld[2];

    if (remove) {
      *sp->u.cle.entry_code = FAILCODE;
      return sp;
    }
    if (ap->PredFlags & LogUpdatePredFlag) {
      struct intermediates intrs;
      LogUpdClause *lc = lu_clause(ipc, ap);

      if (first) {
	cld[0].Code = cls[0].Code;
	cld[1].Code = lc->ClCode;
      } else {
	cld[0].Code = lc->ClCode;
	cld[1].Code = cls[0].Code;
      }
      intrs.expand_block = NULL;
      *sp->u.cle.entry_code = (yamop *)suspend_indexing(cld, cld+1, ap, &intrs);
    } else {
      /* static predicate, shouldn't do much, just suspend the code here */
      *sp->u.cle.entry_code = (yamop *)&(ap->cs.p_code.ExpandCode);
      return sp;
    }
    return sp;
  }
  /* we didn't have protection, should kill now */
  return kill_block(sp+1, ap);
}

static int
compacta_expand_clauses(yamop *ipc)
{
  /* expand clauses so that you have a hole at the beginning */
  /* we know that there is at least one element here */
  yamop **start = (yamop **)(NEXTOP(ipc,sssllp));
  yamop **ptr, **end;

  ptr = end = start+ipc->u.sssllp.s1;

  while (ptr > start) {
    yamop *next = *--ptr;
    if (next) *--end = next;
  }
  if (ptr != end) {
    while (end > start) {
      *--end = NULL;
    }
    return TRUE;
  }
  return FALSE;
}

static int
compactz_expand_clauses(yamop *ipc)
{
  /* expand clauses so that you have a hole at the beginning */
  /* we know that there is at least one element here */
  yamop **start = (yamop **)(NEXTOP(ipc,sssllp));
  yamop **ptr, **end;

  end = start+ipc->u.sssllp.s1;
  ptr = start;

  while (ptr < end) {
    yamop *next = *ptr++;
    if (next) *start++ = next;
  }
  /* reset empty slots at end */
  if (start != end) {
    while (start < end) {
      *start++ = NULL;
    }
    return TRUE;
  }
  return FALSE;
}

/* this code should be called when we jumped to clauses */
static yamop *
add_to_expand_clauses(path_stack_entry **spp, yamop *ipc, ClauseDef *cls, PredEntry *ap, int first, struct intermediates *cint)
{
  path_stack_entry *sp = *spp;
  yamop **clar;

  if (first) {

    do {
      clar = (yamop **)NEXTOP(ipc,sssllp);

      if (*clar == NULL || clar[0] == cls->Code) {
	while (*clar == NULL) clar++;
	if (clar[0] != cls->Code) {
	  clar[-1] = cls->Code;
	  ipc->u.sssllp.s2++;
	}
	return pop_path(spp, cls, ap, cint);
      }
    } while (compacta_expand_clauses(ipc));
  } else {
    do {
      clar = (yamop **)NEXTOP(ipc,sssllp) + ipc->u.sssllp.s1;
      if (clar[-1] == NULL  || clar[-1] == cls->Code) {
	while (*--clar == NULL);
	if (clar[0] != cls->Code) {
	  clar[1] = cls->Code;
	  ipc->u.sssllp.s2++;
	}
	return pop_path(spp, cls, ap, cint);
      }
    } while (compactz_expand_clauses(ipc));
  }
  while ((--sp)->flag != block_entry);
  if (sp->u.cle.entry_code) {
    *sp->u.cle.entry_code = (yamop *)&(ap->cs.p_code.ExpandCode);
  }
  recover_ecls_block(ipc);
  return pop_path(spp, cls, ap, cint);
}

/* this code should be called when we jumped to clauses */
static void
nullify_expand_clause(yamop *ipc, path_stack_entry *sp, ClauseDef *cls)
{
  yamop **st = (yamop **)NEXTOP(ipc,sssllp);
  yamop **max = st+ipc->u.sssllp.s1;

  /* make sure we get rid of the reference */
  while (st < max) {
    if (*st && *st == cls->Code) {
      *st = NULL;
      ipc->u.sssllp.s2--;
      break;
    }
    st++;
  }
  /* if the block has a single element */
  if (ipc->u.sssllp.s2 == 1) {
    yamop **st = (yamop **)NEXTOP(ipc,sssllp);
    while ((--sp)->flag != block_entry);
    while (TRUE) {
      if (*st && *st != cls->Code) {
	*sp->u.cle.entry_code = *st;
	recover_ecls_block(ipc);
	return;
      }
      st++;
    }
  }
}

static yamop *
add_try(PredEntry *ap, ClauseDef *cls, yamop *next, struct intermediates *cint)
{
  yamop *newcp;
  UInt size = (UInt)NEXTOP((yamop *)NULL,OtaLl);
  LogUpdClause *lcl = ClauseCodeToLogUpdClause(cls->Code);

  if ((newcp = (yamop *)Yap_AllocCodeSpace(size)) == NULL) {
    /* OOOPS, got in trouble, must do a longjmp and recover space */
    save_machine_regs();
    longjmp(cint->CompilerBotch,2);
  }
  Yap_LUIndexSpace_CP += size;
#ifdef DEBUG
  Yap_NewCps++;
  Yap_LiveCps++;
#endif
  newcp->opc = Yap_opcode(_try_logical);
  newcp->u.OtaLl.s = ap->ArityOfPE;
  newcp->u.OtaLl.n = next;
  newcp->u.OtaLl.d = lcl;
  lcl->ClRefCount++;
  return newcp;
}

static yamop *
add_trust(LogUpdIndex *icl, ClauseDef *cls, struct intermediates *cint)
{
  yamop *newcp;
  UInt size = (UInt)NEXTOP((yamop *)NULL,OtILl);
  LogUpdClause *lcl = ClauseCodeToLogUpdClause(cls->Code);
  PredEntry *ap =  lcl->ClPred;

  if ((newcp = (yamop *)Yap_AllocCodeSpace(size)) == NULL) {
    /* OOOPS, got in trouble, must do a longjmp and recover space */
    save_machine_regs();
    longjmp(cint->CompilerBotch,2);
  }
  Yap_LUIndexSpace_CP += size;
#ifdef DEBUG
  Yap_NewCps++;
  Yap_LiveCps++;
#endif
  if (ap->PredFlags & CountPredFlag)
    newcp->opc = Yap_opcode(_count_trust_logical);
  else if (ap->PredFlags & ProfiledPredFlag)
    newcp->opc = Yap_opcode(_profiled_trust_logical);
  else
    newcp->opc = Yap_opcode(_trust_logical);
  newcp->u.OtILl.block = icl;
  newcp->u.OtILl.n = NULL;
  newcp->u.OtILl.d = lcl;
  lcl->ClRefCount++;
  return newcp;
}

static void
add_to_index(struct intermediates *cint, int first, path_stack_entry *sp, ClauseDef *cls) {
  /* last clause to experiment with */
  PredEntry *ap = cint->CurrentPred;
  yamop *ipc = ap->cs.p_code.TrueCodeOfPred;
  int group1 = TRUE;
  yamop *alt = NULL;
  UInt current_arity = 0;
  int last_arg = TRUE;
  LogUpdIndex *icl = NULL;

  sp = init_block_stack(sp, ipc, ap);
  /* try to refine the interval using the indexing code */
  while (ipc != NULL) {
    op_numbers op = Yap_op_from_opcode(ipc->opc);

    switch(op) {
    case _try_logical:
    case _retry_logical:
    case _count_retry_logical:
    case _profiled_retry_logical:
    case _trust_logical:
    case _count_trust_logical:
    case _profiled_trust_logical:
      /* ERROR */
      break;
    case _enter_lu_pred:
      ipc->u.Ills.s++;
      icl = ipc->u.Ills.I;
      if (first) {
	if (ap->PredFlags & CountPredFlag)
	  ipc->u.Ills.l1->opc = Yap_opcode(_count_retry_logical);
	else if (ap->PredFlags & ProfiledPredFlag)
	  ipc->u.Ills.l1->opc = Yap_opcode(_profiled_retry_logical);
	else
	  ipc->u.Ills.l1->opc = Yap_opcode(_retry_logical);
	ipc->u.Ills.l1 = add_try(ap, cls, ipc->u.Ills.l1, cint);
      } else {
	/* just go to next instruction */
	yamop *end = add_trust(icl, cls, cint),
	  *old = ipc->u.Ills.l2;
	
	/* we used to have two clauses */
	if (ap->PredFlags & CountPredFlag)
	  old->opc = Yap_opcode(_count_retry_logical);
	else if (ap->PredFlags & ProfiledPredFlag)
	  old->opc = Yap_opcode(_profiled_retry_logical);
	else
	  old->opc = Yap_opcode(_retry_logical);
	old->u.OtaLl.n = end;
	old->u.OtaLl.s = ap->ArityOfPE;
	ipc->u.Ills.l2 = end;
      }
      ipc = pop_path(&sp, cls, ap, cint);
      break;
    case _try_clause:
      /* I cannot expand a predicate that starts on a variable,
         have to expand the index.
      */
      if (first) {
	sp = expanda_block(sp, ap, cls, group1, alt, cint);
	ipc = pop_path(&sp, cls, ap, cint);
      } else {
	/* just go to next instruction */
	ipc = NEXTOP(ipc,Otapl);
      }
      break;
    case _try_clause2:
    case _try_clause3:
    case _try_clause4:
      /* I cannot expand a predicate that starts on a variable,
         have to expand the index.
      */
      if (first) {
	sp = expanda_block(sp, ap, cls, group1, alt, cint);
	ipc = pop_path(&sp, cls, ap, cint);
      } else {
	/* just go to next instruction */
	ipc = NEXTOP(ipc,l);
      }
      break;
    case _retry:
      /* this clause had no indexing */
      ipc = NEXTOP(ipc,Otapl);
      break;
    case _retry2:
    case _retry3:
    case _retry4:
      /* this clause had no indexing */
      ipc = NEXTOP(ipc,l);
      break;
      /* instructions type l */
    case _retry_me:
      /* should never be reached both for asserta */
      group1 = FALSE;
      ipc = ipc->u.Otapl.d;
      break;
    case _try_me:
      if (first) {
	ipc = NEXTOP(ipc,Otapl);
	alt = ipc->u.Otapl.d;
      } else {
	ipc = ipc->u.Otapl.d;
	group1 = FALSE;
      }
      break;
    case _retry_profiled:
    case _count_retry:
      ipc = NEXTOP(ipc,Otapl);
      break;
    case _profiled_trust_me:
    case _trust_me:
    case _count_trust_me:
      group1 = FALSE;
      ipc = NEXTOP(ipc,Otapl);
      break;
    case _trust:
      sp = expandz_block(sp, ap, cls, group1, alt, cint);
      ipc = pop_path(&sp, cls, ap, cint);
      break;
    case _jump:
      sp = cross_block(sp, &ipc->u.l.l, ap, cint);
      /* just skip for now, but should worry about memory management */
      ipc = ipc->u.l.l;
      break;
    case _jump_if_var:
      sp = push_path(sp, &(ipc->u.l.l), cls, cint);
      ipc = NEXTOP(ipc,l);
      break;
    case _jump_if_nonvar:
      sp = push_path(sp, &(ipc->u.xll.l2), cls, cint);
      sp = cross_block(sp, &ipc->u.xll.l1, ap, cint);
      ipc = ipc->u.xll.l1;
      break;
      /* instructions type EC */
    case _try_in:
      /* we are done */
      if (first) {
	sp = kill_block(sp, ap);
	ipc = pop_path(&sp, cls, ap, cint);
      } else {
	ipc = NEXTOP(ipc,l);
      }
      break;
    case _user_switch:
      ipc = ipc->u.lp.l;
      break;
      /* instructions type e */
    case _switch_on_type:
      sp = push_path(sp, &(ipc->u.llll.l4), cls, cint);
      if (ap->PredFlags & LogUpdatePredFlag) {
	add_head_info(cls, 1);
      } else {
	add_info(cls, 1);
      }
      if (IsPairTerm(cls->Tag)) {
	yamop *nipc = ipc->u.llll.l1;

	current_arity = 2;
	move_next(cls, 1);
	if (nipc == FAILCODE) {
	  /* jump straight to clause */
	  if (ap->PredFlags & LogUpdatePredFlag) {
	    ipc->u.llll.l1 = cls->Code;
	  } else {
	    ipc->u.llll.l1 = cls->CurrentCode;
	  }
	  ipc = pop_path(&sp, cls, ap, cint);
	} else {
	  /* go on */
	  sp = cross_block(sp, &ipc->u.llll.l1, ap, cint);
	  ipc = nipc;	
	}
      } else if (IsAtomOrIntTerm(cls->Tag)) {
	yamop *nipc = ipc->u.llll.l2;
	move_next(cls, 1);
	if (nipc == FAILCODE) {
	  /* need to expand the block */
	  sp = kill_block(sp, ap);
	  ipc = pop_path(&sp, cls, ap, cint);
	} else {
	  /* I do not have to worry about crossing a block here */
	  ipc = nipc;	
	}
      } else if (IsApplTerm(cls->Tag)) {
	yamop *nipc = ipc->u.llll.l3;
	if (nipc == FAILCODE) {
	  /* need to expand the block */
	  sp = kill_block(sp, ap);
	  ipc = pop_path(&sp, cls, ap, cint);
	} else {
	  /* I do not have to worry about crossing a block here */
	  ipc = nipc;	
	}
      } else {
	/* we can't separate into four groups,
	   need to restart.
	*/
	sp = kill_block(sp, ap);
	ipc = pop_path(&sp, cls, ap, cint);
      }
      break;
    case _switch_list_nl:
      sp = kill_block(sp, ap);
      ipc = pop_path(&sp, cls, ap, cint);
      break;
    case _switch_on_arg_type:
      sp = push_path(sp, &(ipc->u.xllll.l4), cls, cint);
      if (ap->PredFlags & LogUpdatePredFlag) {
	add_head_info(cls, Yap_regtoregno(ipc->u.xllll.x));
      } else {
	add_info(cls, Yap_regtoregno(ipc->u.xllll.x));
      }
      if (IsPairTerm(cls->Tag)) {
	yamop *nipc = ipc->u.xllll.l1;

	current_arity = 2;
	move_next(cls, Yap_regtoregno(ipc->u.xllll.x));
	if (nipc == FAILCODE) {
	  /* jump straight to clause */
	  if (ap->PredFlags & LogUpdatePredFlag) {
	    ipc->u.xllll.l1 = cls->Code;
	  } else {
	    ipc->u.xllll.l1 = cls->CurrentCode;
	  }
	  ipc = pop_path(&sp, cls, ap, cint);
	} else {
	  /* go on */
	  sp = cross_block(sp, &ipc->u.xllll.l1, ap, cint);
	  ipc = nipc;	
	}
      } else if (IsAtomOrIntTerm(cls->Tag)) {
	yamop *nipc = ipc->u.xllll.l2;
	move_next(cls, Yap_regtoregno(ipc->u.xllll.x));
	if (nipc == FAILCODE) {
	  /* need to expand the block */
	  sp = kill_block(sp, ap);
	  ipc = pop_path(&sp, cls, ap, cint);
	} else {
	  /* I do not have to worry about crossing a block here */
	  ipc = nipc;	
	}
      } else if (IsApplTerm(cls->Tag)) {
	yamop *nipc = ipc->u.xllll.l3;
	move_next(cls, Yap_regtoregno(ipc->u.xllll.x));
	if (nipc == FAILCODE) {
	  /* need to expand the block */
	  sp = kill_block(sp, ap);
	  ipc = pop_path(&sp, cls, ap, cint);
	} else {
	  /* I do not have to worry about crossing a block here */
	  ipc = nipc;	
	}
      } else {
	/* we can't separate into four groups,
	   need to restart.
	*/
	sp = kill_block(sp, ap);
	ipc = pop_path(&sp, cls, ap, cint);
      }
      break;
    case _switch_on_sub_arg_type:
      sp = push_path(sp, &(ipc->u.sllll.l4), cls, cint);
      add_arg_info(cls, ap, ipc->u.sllll.s+1);
      if (IsPairTerm(cls->Tag)) {
	yamop *nipc = ipc->u.sllll.l1;
	current_arity = 2;
	skip_to_arg(cls, ap, ipc->u.sllll.s, current_arity);
	if (current_arity != ipc->u.sllll.s+1) {
	  last_arg = FALSE;
	}
	if (nipc == FAILCODE) {
	  /* jump straight to clause */
	  if (ap->PredFlags & LogUpdatePredFlag) {
	    ipc->u.sllll.l1 = cls->Code;
	  } else {
	    ipc->u.sllll.l1 = cls->CurrentCode;
	  }
	  ipc = pop_path(&sp, cls, ap, cint);
	} else {
	  /* go on */
	  sp = cross_block(sp, &ipc->u.sllll.l1, ap, cint);
	  ipc = nipc;	
	}
      } else if (IsAtomOrIntTerm(cls->Tag)) {
	yamop *nipc = ipc->u.sllll.l2;
	skip_to_arg(cls, ap, ipc->u.sllll.s, current_arity);
	if (current_arity != ipc->u.sllll.s+1) {
	  last_arg = FALSE;
	}
	if (nipc == FAILCODE) {
	  /* need to expand the block */
	  sp = kill_block(sp, ap);
	  ipc = pop_path(&sp, cls, ap, cint);
	} else {
	  /* I do not have to worry about crossing a block here */
	  ipc = nipc;	
	}
      } else if (IsApplTerm(cls->Tag)) {
	yamop *nipc = ipc->u.sllll.l3;
	skip_to_arg(cls, ap, ipc->u.sllll.s, current_arity);
	if (current_arity != ipc->u.sllll.s+1) {
	  last_arg = FALSE;
	}
	if (nipc == FAILCODE) {
	  /* need to expand the block */
	  sp = kill_block(sp, ap);
	  ipc = pop_path(&sp, cls, ap, cint);
	} else {
	  /* I do not have to worry about crossing a block here */
	  ipc = nipc;	
	}
      } else {
	/* we can't separate into four groups,
	   need to restart.
	*/
	sp = kill_block(sp, ap);
	ipc = pop_path(&sp, cls, ap, cint);
      }
      break;
    case _if_not_then:
      ipc = pop_path(&sp, cls, ap, cint);
      break;
      /* instructions type ollll */
    case _switch_on_func:
    case _if_func:
    case _go_on_func:
      {
	FuncSwiEntry *fe;
	yamop *newpc;
	Functor f = (Functor)RepAppl(cls->Tag);
	
	if (op == _switch_on_func) {
	  fe = lookup_f_hash(f, ipc->u.sssl.l, ipc->u.sssl.s);
	} else {
	  fe = lookup_f(f, ipc->u.sssl.l, ipc->u.sssl.s);
	}
	if (!IsExtensionFunctor(f)) {
	  current_arity = ArityOfFunctor(f);
	}
	newpc = fe->u.labp;

	if (newpc == (yamop *)&(ap->cs.p_code.ExpandCode)) {
	  /* we found it */
	  ipc = pop_path(&sp, cls, ap, cint);
	} else if (newpc == FAILCODE) {
	  /* oops, nothing there */
	  if (fe->Tag != f) {
	    if (IsExtensionFunctor(f)) {
	      sp = kill_unsafe_block(sp, op, ap, first, FALSE, cls);
	      ipc = pop_path(&sp, cls, ap, cint);
	      break;
	    }
	    if (table_fe_overflow(ipc, f)) {
	      fe = expand_ftable(ipc, current_block(sp), cint, f);
	    }
	    fe->Tag = f;
	    ipc->u.sssl.e++;
	  }
	  if (ap->PredFlags & LogUpdatePredFlag) {
	     fe->u.labp = cls->Code;
	  } else {
	    fe->u.labp = cls->CurrentCode;
	  }
	  ipc = pop_path(&sp, cls, ap, cint);
	} else {
	  yamop *newpc = fe->u.labp;
	  sp = fetch_new_block(sp, &(ipc->u.sssl.l), ap, cint);
	  sp = cross_block(sp, &(fe->u.labp), ap, cint);
	  ipc = newpc;
	}
      }
      break;
    case _index_dbref:
      cls->Tag = cls->u.t_ptr;
      ipc = NEXTOP(ipc,e);
      break;
    case _index_blob:
      {
	CELL *pt = RepAppl(cls->u.t_ptr);
#if SIZEOF_DOUBLE == 2*SIZEOF_LONG_INT
	cls->Tag = MkIntTerm(pt[1]^pt[2]);
#else
	cls->Tag = MkIntTerm(pt[1]);
#endif
      }
      ipc = NEXTOP(ipc,e);
      break;
    case _index_long:
      {
	CELL *pt = RepAppl(cls->u.t_ptr);
	cls->Tag = MkIntTerm((pt[1] & (MAX_ABS_INT-1)));
      }
      ipc = NEXTOP(ipc,e);
      break;
    case _switch_on_cons:
    case _if_cons:
    case _go_on_cons:
      {
	AtomSwiEntry *ae;
	yamop *newpc;
	Term at = cls->Tag;
	
	if (op == _switch_on_cons) {
	  ae = lookup_c_hash(at,ipc->u.sssl.l,ipc->u.sssl.s);
	} else {
	  ae = lookup_c(at, ipc->u.sssl.l, ipc->u.sssl.s);
	}
	newpc = ae->u.labp;

	if (newpc == (yamop *)&(ap->cs.p_code.ExpandCode)) {
	  /* nothing more to do */
	  ipc = pop_path(&sp, cls, ap, cint);
	} else if (newpc == FAILCODE) {
	  /* oops, nothing there */
	  if (ae->Tag != at) {
	    if (table_ae_overflow(ipc, at)) {
	      ae = expand_ctable(ipc, current_block(sp), cint, at);
	    }
	    ae->Tag = at;
	    ipc->u.sssl.e++;
	  }
	  if (ap->PredFlags & LogUpdatePredFlag) {
	    ae->u.labp = cls->Code;
	  } else {
	    ae->u.labp = cls->CurrentCode;
	  }
	  ipc = pop_path(&sp, cls, ap, cint);
	} else {
	  yamop *newpc = ae->u.labp;

	  sp = fetch_new_block(sp, &(ipc->u.sssl.l), ap, cint);
	  sp = cross_block(sp, &(ae->u.labp), ap, cint);
	  ipc = newpc;
	}
      }
      break;
    case _expand_clauses:
      ipc = add_to_expand_clauses(&sp, ipc, cls, ap, first, cint);
      break;
    case _expand_index:
      ipc = pop_path(&sp, cls, ap, cint);
      break;
    case _lock_lu:
      ipc = NEXTOP(ipc,p);
      break;
    case _unlock_lu:
      ipc = NEXTOP(ipc,e);
      break;
    case _op_fail:
      while ((--sp)->flag != block_entry);
      *sp->u.cle.entry_code = cls->Code;
      ipc = pop_path(&sp, cls, ap, cint);
      break;
    default:
      sp = kill_unsafe_block(sp, op, ap, first, FALSE, cls);
      ipc = pop_path(&sp, cls, ap, cint);
    }
  }
}


void
Yap_AddClauseToIndex(PredEntry *ap, yamop *beg, int first) {
  ClauseDef cl;
  /* first clause */
  path_stack_entry *stack, *sp;
  int cb;
  struct intermediates cint;

  if (!(ap->PredFlags & LogUpdatePredFlag)) {
    if (ap->PredFlags & IndexedPredFlag)
      Yap_RemoveIndexation(ap);
    return;
  }
  cint.CurrentPred = ap;
  cint.expand_block = NULL;
  cint.CodeStart = cint.BlobsStart = cint.cpc = cint.icpc = NIL;
  if ((cb = setjmp(cint.CompilerBotch)) == 3) {
    restore_machine_regs();
    Yap_gcl(Yap_Error_Size, ap->ArityOfPE, ENV, CP);
    save_machine_regs();
  } else if (cb == 2) {
    restore_machine_regs();
    Yap_growheap(FALSE, Yap_Error_Size, NULL);
    save_machine_regs();
  } else if (cb == 4) {
    restore_machine_regs();
    Yap_growtrail(Yap_Error_Size, FALSE);
    save_machine_regs();
  }
  if (cb) {
    Yap_RemoveIndexation(ap);
    return;
  }
  Yap_Error_Size = 0;
  Yap_ErrorMessage = NULL;
#ifdef DEBUG
  if (Yap_Option['i' - 'a' + 1]) {
    Term tmod = ap->ModuleOfPred;
    Yap_LockStream(Yap_c_error_stream);
    if (!tmod) tmod = TermProlog;
    Yap_DebugPutc(Yap_c_error_stream,'+');
    Yap_DebugPutc(Yap_c_error_stream,'\t');
    Yap_DebugPlWrite(tmod);
    Yap_DebugPutc(Yap_c_error_stream,':');
    if (ap->ModuleOfPred == IDB_MODULE) {
      Term t = Deref(ARG1);
      if (IsAtomTerm(t)) {
	Yap_DebugPlWrite(t);
      } else if (IsIntegerTerm(t)) {
	Yap_DebugPlWrite(t);
      } else {
	Functor f = FunctorOfTerm(t);
	Atom At = NameOfFunctor(f);
	Yap_DebugPlWrite(MkAtomTerm(At));
	Yap_DebugPutc(Yap_c_error_stream,'/');
	Yap_DebugPlWrite(MkIntegerTerm(ArityOfFunctor(f)));
      }
    } else {
      if (ap->ArityOfPE == 0) {
	Atom At = (Atom)ap->FunctorOfPred;
	Yap_DebugPlWrite(MkAtomTerm(At));
      } else {
	Functor f = ap->FunctorOfPred;
	Atom At = NameOfFunctor(f);
	Yap_DebugPlWrite(MkAtomTerm(At));
	Yap_DebugPutc(Yap_c_error_stream,'/');
	Yap_DebugPlWrite(MkIntegerTerm(ArityOfFunctor(f)));
      }
    }
    Yap_DebugPutc(Yap_c_error_stream,'\n');
    Yap_UnLockStream(Yap_c_error_stream);
  }
#endif
  stack = (path_stack_entry *)TR;
  cl.Code =  cl.CurrentCode = beg;
  sp = push_path(stack, NULL, &cl, &cint);
  add_to_index(&cint, first, sp, &cl); 
}
		 

static void
contract_ftable(yamop *ipc, ClauseUnion *blk, PredEntry *ap, Functor f) {
  int n = ipc->u.sssl.s;
  FuncSwiEntry *fep;

  if (n > MIN_HASH_ENTRIES) {
    fep = lookup_f_hash(f, ipc->u.sssl.l, n);
  } else {
    fep = (FuncSwiEntry *)(ipc->u.sssl.l);
    while (fep->Tag != f) fep++;
  }
  fep->u.labp = FAILCODE;
}

static void
contract_ctable(yamop *ipc, ClauseUnion *blk, PredEntry *ap, Term at) {
  int n = ipc->u.sssl.s;
  AtomSwiEntry *cep;

  if (n > MIN_HASH_ENTRIES) {
    cep = lookup_c_hash(at, ipc->u.sssl.l, n);
  } else {
    cep = (AtomSwiEntry *)(ipc->u.sssl.l);
    while (cep->Tag != at) cep++;
  }
  cep->u.labp = FAILCODE;
}

static void
remove_from_index(PredEntry *ap, path_stack_entry *sp, ClauseDef *cls, yamop *bg, yamop *lt, struct intermediates *cint) {
  /* last clause to experiment with */
  yamop *ipc = ap->cs.p_code.TrueCodeOfPred;
  UInt current_arity = 0;

  if (ap->cs.p_code.NOfClauses == 1) {
    if (ap->PredFlags & IndexedPredFlag) {
      Yap_RemoveIndexation(ap);
      return;
    }
    ap->cs.p_code.TrueCodeOfPred = ap->cs.p_code.FirstClause;
    if (ap->PredFlags & (SpiedPredFlag|CountPredFlag|ProfiledPredFlag)) {
      ap->OpcodeOfPred = Yap_opcode(_spy_pred);
      ap->CodeOfPred = (yamop *)(&(ap->OpcodeOfPred)); 
#if defined(YAPOR) || defined(THREADS)
    } else if (ap->PredFlags & LogUpdatePredFlag &&
	       ap->ModuleOfPred != IDB_MODULE) {
      ap->cs.p_code.TrueCodeOfPred = FAILCODE;
      ap->OpcodeOfPred = LOCKPRED_OPCODE;
      ap->CodeOfPred = (yamop *)(&(ap->OpcodeOfPred)); 
#endif
    } else {
      ap->OpcodeOfPred = ap->cs.p_code.FirstClause->opc;
      ap->CodeOfPred = ap->cs.p_code.TrueCodeOfPred;
    }
    return;
  }
  sp = init_block_stack(sp, ipc, ap);
  /* try to refine the interval using the indexing code */
  while (ipc != NULL) {
    op_numbers op = Yap_op_from_opcode(ipc->opc);

    switch(op) {
    case _retry_profiled:
    case _count_retry:
      ipc = NEXTOP(ipc, p);
      break;
    case _try_in:
      /* I cannot expand a predicate that starts on a variable,
         have to expand the index.
      */
      if (IN_BETWEEN(bg,ipc->u.l.l,lt)) {
	sp = kill_clause(ipc, bg, lt, sp, ap);
	ipc = pop_path(&sp, cls, ap, cint);
      } else {
	/* just go to next instruction */
	ipc = NEXTOP(ipc,l);
      }
      break;
    case _try_clause:
    case _retry:
      /* I cannot expand a predicate that starts on a variable,
         have to expand the index.
      */
      if (IN_BETWEEN(bg,ipc->u.Otapl.d,lt)) {
	sp = kill_clause(ipc, bg, lt, sp, ap);
	ipc = pop_path(&sp, cls, ap, cint);
      } else {
	/* just go to next instruction */
	ipc = NEXTOP(ipc,Otapl);
      }
      break;
    case _try_clause2:
    case _try_clause3:
    case _try_clause4:
    case _retry2:
    case _retry3:
    case _retry4:
      /* I cannot expand a predicate that starts on a variable,
         have to expand the index.
      */
      if (IN_BETWEEN(bg,ipc->u.l.l,lt)) {
	sp = kill_clause(ipc, bg, lt, sp, ap);
	ipc = pop_path(&sp, cls, ap, cint);
      } else {
	/* just go to next instruction */
	ipc = NEXTOP(ipc,l);
      }
      break;
    case _trust:
      if (IN_BETWEEN(bg,ipc->u.Otapl.d,lt)) {
	sp = kill_clause(ipc, bg, lt, sp, ap);
      }
      ipc = pop_path(&sp, cls, ap, cint);
      break;
    case _enter_lu_pred:
      ipc->u.Ills.s--;
#ifdef DEBUG
      Yap_DirtyCps++;
      Yap_LiveCps--;
#endif
      sp = kill_clause(ipc, bg, lt, sp, ap);
      ipc = pop_path(&sp, cls, ap, cint);
      break;
      /* instructions type l */
    case _try_me:
    case _retry_me:
      sp = push_path(sp, &(ipc->u.Otapl.d), cls, cint);
      ipc = NEXTOP(ipc,Otapl);
      break;
    case _profiled_trust_me:
    case _trust_me:
    case _count_trust_me:
      ipc = NEXTOP(ipc,Otapl);
      break;
    case _jump:
      sp = cross_block(sp, &ipc->u.l.l, ap, cint);
      /* just skip for now, but should worry about memory management */
      ipc = ipc->u.l.l;
      break;
    case _jump_if_var:
      sp = push_path(sp, &(ipc->u.l.l), cls, cint);
      ipc = NEXTOP(ipc,l);
      break;
    case _jump_if_nonvar:
      sp = push_path(sp, &(ipc->u.xll.l2), cls, cint);
      sp = cross_block(sp, &ipc->u.xll.l1, ap, cint);
      ipc = ipc->u.xll.l1;
      break;
    case _user_switch:
      ipc = ipc->u.lp.l;
      break;
      /* instructions type e */
    case _switch_on_type:
      sp = push_path(sp, &(ipc->u.llll.l4), cls, cint);
      if (ap->PredFlags & LogUpdatePredFlag) {
	add_head_info(cls, 1);
      } else {
	add_info(cls, 1);
      }
      if (IsPairTerm(cls->Tag)) {
	yamop *nipc = ipc->u.llll.l1;
	current_arity = 2;
	if (IN_BETWEEN(bg,nipc,lt)) {
	  /* jump straight to clause */
	  ipc->u.llll.l1 = FAILCODE;
	  ipc = pop_path(&sp, cls, ap, cint);
	} else {
	  /* go on */
	  sp = cross_block(sp, &ipc->u.llll.l1, ap, cint);
	  ipc = nipc;	
	}
      } else if (IsAtomOrIntTerm(cls->Tag)) {
	yamop *nipc = ipc->u.llll.l2;
	if (IN_BETWEEN(bg,nipc,lt)) {
	  /* jump straight to clause */
	  ipc->u.llll.l2 = FAILCODE;
	  ipc = pop_path(&sp, cls, ap, cint);
	} else {
	  /* I do not have to worry about crossing a block here */
	  ipc = nipc;	
	}
      } else if (IsApplTerm(cls->Tag)) {
	yamop *nipc = ipc->u.llll.l3;
	if (IN_BETWEEN(bg,nipc,lt)) {
	  /* jump straight to clause */
	  ipc->u.llll.l3 = FAILCODE;
	  ipc = pop_path(&sp, cls, ap, cint);
	} else {
	  /* I do not have to worry about crossing a block here */
	  ipc = nipc;	
	}
      } else {
	/* we can't separate into four groups,
	   need to restart.
	*/
	sp = kill_block(sp, ap);
	ipc = pop_path(&sp, cls, ap, cint);
      }
      break;
    case _switch_list_nl:
      sp = kill_block(sp, ap);
      ipc = pop_path(&sp, cls, ap, cint);
      break;
    case _switch_on_arg_type:
      sp = push_path(sp, &(ipc->u.xllll.l4), cls, cint);
      current_arity = 2;
      if (ap->PredFlags & LogUpdatePredFlag) {
	add_head_info(cls, Yap_regtoregno(ipc->u.xllll.x));
      } else {
	add_info(cls, Yap_regtoregno(ipc->u.xllll.x));
      }
      if (IsPairTerm(cls->Tag)) {
	yamop *nipc = ipc->u.xllll.l1;
	if (IN_BETWEEN(bg,nipc,lt)) {
	  /* jump straight to clause */
	  ipc->u.xllll.l1 = FAILCODE;
	  ipc = pop_path(&sp, cls, ap, cint);
	} else {
	  /* go on */
	  sp = cross_block(sp, &ipc->u.xllll.l1, ap, cint);
	  ipc = nipc;	
	}
      } else if (IsAtomOrIntTerm(cls->Tag)) {
	yamop *nipc = ipc->u.xllll.l2;
	if (IN_BETWEEN(bg,nipc,lt)) {
	  /* jump straight to clause */
	  ipc->u.xllll.l2 = FAILCODE;
	  ipc = pop_path(&sp, cls, ap, cint);
	} else {
	  /* I do not have to worry about crossing a block here */
	  ipc = nipc;	
	}
      } else if (IsApplTerm(cls->Tag)) {
	yamop *nipc = ipc->u.xllll.l3;
	if (IN_BETWEEN(bg,nipc,lt)) {
	  /* jump straight to clause */
	  ipc->u.xllll.l3 = FAILCODE;
	  ipc = pop_path(&sp, cls, ap, cint);
	} else {
	  /* I do not have to worry about crossing a block here */
	  ipc = nipc;	
	}
      } else {
	/* we can't separate into four groups,
	   need to restart.
	*/
	sp = kill_block(sp, ap);
	ipc = pop_path(&sp, cls, ap, cint);
      }
      break;
    case _switch_on_sub_arg_type:
      sp = push_path(sp, &(ipc->u.sllll.l4), cls, cint);
      current_arity = 2;
      add_arg_info(cls, ap, ipc->u.sllll.s+1);
      if (IsPairTerm(cls->Tag)) {
	yamop *nipc = ipc->u.sllll.l1;
	if (IN_BETWEEN(bg,nipc,lt)) {
	  /* jump straight to clause */
	  ipc->u.sllll.l1 = FAILCODE;
	  ipc = pop_path(&sp, cls, ap, cint);
	} else {
	  /* go on */
	  sp = cross_block(sp, &ipc->u.sllll.l1, ap, cint);
	  ipc = nipc;	
	}
      } else if (IsAtomOrIntTerm(cls->Tag)) {
	yamop *nipc = ipc->u.sllll.l2;
	if (IN_BETWEEN(bg,nipc,lt)) {
	  /* jump straight to clause */
	  ipc->u.sllll.l2 = FAILCODE;
	  ipc = pop_path(&sp, cls, ap, cint);
	} else {
	  /* I do not have to worry about crossing a block here */
	  ipc = nipc;	
	}
      } else if (IsApplTerm(cls->Tag)) {
	yamop *nipc = ipc->u.sllll.l3;
	if (IN_BETWEEN(bg,nipc,lt)) {
	  /* jump straight to clause */
	  ipc->u.sllll.l3 = FAILCODE;
	  ipc = pop_path(&sp, cls, ap, cint);
	} else {
	  /* I do not have to worry about crossing a block here */
	  ipc = nipc;	
	}
      } else {
	/* we can't separate into four groups,
	   need to restart.
	*/
	sp = kill_block(sp, ap);
	ipc = pop_path(&sp, cls, ap, cint);
      }
      break;
    case _if_not_then:
      ipc = pop_path(&sp, cls, ap, cint);
      break;
      /* instructions type ollll */
    case _switch_on_func:
    case _if_func:
    case _go_on_func:
      {
	FuncSwiEntry *fe;
	yamop *newpc;
	Functor f = (Functor)RepAppl(cls->Tag);
	
	if (op == _switch_on_func) {
	  fe = lookup_f_hash(f, ipc->u.sssl.l, ipc->u.sssl.s);
	} else {
	  fe = lookup_f(f, ipc->u.sssl.l, ipc->u.sssl.s);
	}
	if (!IsExtensionFunctor(f)) {
	  current_arity = ArityOfFunctor(f);
	}
	newpc = fe->u.labp;

	if (newpc == (yamop *)&(ap->cs.p_code.ExpandCode)) {
	  /* we found it */
	  ipc = pop_path(&sp, cls, ap, cint);
	} else if (newpc == FAILCODE) {
	  ipc = pop_path(&sp, cls, ap, cint);
	} else if (IN_BETWEEN(bg,fe->u.Label,lt)) {
	  /* oops, nothing there */
	  contract_ftable(ipc, current_block(sp), ap, f);
	  ipc = pop_path(&sp, cls, ap, cint);
	} else {
	  yamop *newpc = fe->u.labp;
	  sp = fetch_new_block(sp, &(ipc->u.sssl.l), ap, cint);
	  sp = cross_block(sp, &(fe->u.labp), ap, cint);
	  ipc = newpc;
	}
      }
      break;
    case _index_dbref:
      cls->Tag = cls->u.t_ptr;
      ipc = NEXTOP(ipc,e);
      break;
    case _index_blob:
      {
	CELL *pt = RepAppl(cls->u.t_ptr);
#if SIZEOF_DOUBLE == 2*SIZEOF_LONG_INT
	cls->Tag = MkIntTerm(pt[1]^pt[2]);
#else
	cls->Tag = MkIntTerm(pt[1]);
#endif
      }
      ipc = NEXTOP(ipc,e);
      break;
    case _index_long:
      {
	CELL *pt = RepAppl(cls->u.t_ptr);
	cls->Tag = MkIntTerm(pt[1] & (MAX_ABS_INT-1));
      }
      ipc = NEXTOP(ipc,e);
      break;
    case _switch_on_cons:
    case _if_cons:
    case _go_on_cons:
      {
	AtomSwiEntry *ae;
	yamop *newpc;
	Term at = cls->Tag;
	
	if (op == _switch_on_cons) {
	  ae = lookup_c_hash(at,ipc->u.sssl.l,ipc->u.sssl.s);
	} else {
	  ae = lookup_c(at, ipc->u.sssl.l, ipc->u.sssl.s);
	}
	newpc = ae->u.labp;

	if (newpc == (yamop *)&(ap->cs.p_code.ExpandCode)) {
	  /* we found it */
	  ipc = pop_path(&sp, cls, ap, cint);
	} else if (newpc == FAILCODE) {
	  ipc = pop_path(&sp, cls, ap, cint);
	} else if (IN_BETWEEN(bg,ae->u.Label,lt)) {
	  /* oops, nothing there */
	  contract_ctable(ipc, current_block(sp), ap, at);
	  ipc = pop_path(&sp, cls, ap, cint);
	} else {
	  yamop *newpc = ae->u.labp;

	  sp = fetch_new_block(sp, &(ipc->u.sssl.l), ap, cint);
	  sp = cross_block(sp, &(ae->u.labp), ap, cint);
	  ipc = newpc;
	}
      }
      break;
    case _expand_index:
      ipc = pop_path(&sp, cls, ap, cint);
      break;
    case _expand_clauses:
      nullify_expand_clause(ipc, sp, cls);
      ipc = pop_path(&sp, cls, ap, cint);
      break;
    case _lock_lu:
      ipc = NEXTOP(ipc,p);
      break;
    default:
      if (IN_BETWEEN(bg,ipc,lt)) {
	sp = kill_unsafe_block(sp, op, ap, TRUE, TRUE, cls);
      }
      ipc = pop_path(&sp, cls, ap, cint);
    }
  }
}


/* clause is locked */
void
Yap_RemoveClauseFromIndex(PredEntry *ap, yamop *beg) {
  ClauseDef cl;
  /* first clause */
  path_stack_entry *stack, *sp;
  int cb;
  yamop *last;
  struct intermediates cint; 

  if (ap->PredFlags & MegaClausePredFlag) {
    return;
  }
  cint.expand_block = NULL;
  cint.CodeStart = cint.BlobsStart = cint.cpc = cint.icpc = NULL;
  if ((cb = setjmp(cint.CompilerBotch)) == 3) {
    restore_machine_regs();
    Yap_gcl(Yap_Error_Size, ap->ArityOfPE, ENV, CP);
    save_machine_regs();
  } else if (cb == 2) {
    restore_machine_regs();
    Yap_growheap(FALSE, Yap_Error_Size, NULL);
    save_machine_regs();
  } else if (cb == 4) {
    restore_machine_regs();
    Yap_growtrail(Yap_Error_Size, FALSE);
    save_machine_regs();
  }
  Yap_Error_Size = 0;
  Yap_ErrorMessage = NULL;
  if (cb) {
    /* cannot rely on the code */
    if (ap->PredFlags & LogUpdatePredFlag) {
      Yap_kill_iblock((ClauseUnion *)ClauseCodeToLogUpdIndex(ap->cs.p_code.TrueCodeOfPred),NULL, ap);
    } else {
      StaticIndex *cl;
      
      cl = ClauseCodeToStaticIndex(ap->cs.p_code.TrueCodeOfPred);
      Yap_kill_iblock((ClauseUnion *)cl, NULL, ap);
    }
    return;
  }
#ifdef DEBUG
  if (Yap_Option['i' - 'a' + 1]) {
    Term tmod = ap->ModuleOfPred;

    if (!tmod) tmod = TermProlog;
    Yap_LockStream(Yap_c_error_stream);
    Yap_DebugPutc(Yap_c_error_stream,'-');
    Yap_DebugPutc(Yap_c_error_stream,'\t');
    Yap_DebugPlWrite(tmod);
    Yap_DebugPutc(Yap_c_error_stream,':');
    if (ap->ModuleOfPred != IDB_MODULE) {
      if (ap->ArityOfPE == 0) {
	Atom At = (Atom)ap->FunctorOfPred;
	Yap_DebugPlWrite(MkAtomTerm(At));
      } else {
	Functor f = ap->FunctorOfPred;
	Atom At = NameOfFunctor(f);
	Yap_DebugPlWrite(MkAtomTerm(At));
	Yap_DebugPutc(Yap_c_error_stream,'/');
	Yap_DebugPlWrite(MkIntegerTerm(ArityOfFunctor(f)));
      }
    } else {
      if (ap->PredFlags & NumberDBPredFlag) {
	Int id = ap->src.IndxId;
	Yap_DebugPlWrite(MkIntegerTerm(id));
      } else if (ap->PredFlags & AtomDBPredFlag) {
	Atom At = (Atom)ap->FunctorOfPred;
	Yap_DebugPlWrite(MkAtomTerm(At));
      } else {
	Functor f = ap->FunctorOfPred;
	Atom At = NameOfFunctor(f);
	Yap_DebugPlWrite(MkAtomTerm(At));
	Yap_DebugPutc(Yap_c_error_stream,'/');
	Yap_DebugPlWrite(MkIntegerTerm(ArityOfFunctor(f)));
      }
    }
    Yap_DebugPutc(Yap_c_error_stream,'\n');
    Yap_UnLockStream(Yap_c_error_stream);
  }
#endif
  stack = (path_stack_entry *)TR;
  if (ap->PredFlags & LogUpdatePredFlag) {
    LogUpdClause *c = ClauseCodeToLogUpdClause(beg);
    cl.Code =  cl.CurrentCode = beg;
    last = (yamop *)((CODEADDR)c+c->ClSize);
  } else {
    StaticClause *c = ClauseCodeToStaticClause(beg);
    cl.Code =  cl.CurrentCode = beg;
    last = (yamop *)((CODEADDR)c+c->ClSize);
  }
  sp = push_path(stack, NULL, &cl, &cint);
  if (ap->cs.p_code.NOfClauses == 0) {
    /* there was no indexing code */
#if defined(YAPOR) || defined(THREADS)
    if (ap->PredFlags & LogUpdatePredFlag &&
	ap->ModuleOfPred != IDB_MODULE) {
      ap->cs.p_code.TrueCodeOfPred = FAILCODE;
      ap->CodeOfPred = (yamop *)(&(ap->OpcodeOfPred)); 
    } else {
#endif
      ap->CodeOfPred = ap->cs.p_code.TrueCodeOfPred = FAILCODE;
#if defined(YAPOR) || defined(THREADS)
    }
#endif
    ap->OpcodeOfPred = Yap_opcode(_op_fail);
  } else {
    remove_from_index(ap, sp, &cl, beg, last, &cint); 
  }
}
	     

static void
store_clause_choice_point(Term t1, Term tb, Term tr, yamop *ipc, PredEntry *pe, yamop *ap_pc, yamop *cp_pc)
{
  Term tpc = MkIntegerTerm((Int)ipc);
  Term tpe = MkIntegerTerm((Int)pe);
  CELL *tsp = ASP-5;
  choiceptr bptr = ((choiceptr)tsp)-1;

  tsp[0] = tpe;
  tsp[1] = tpc;
  tsp[2] = t1;
  tsp[3] = tb;
  tsp[4] = tr;
  bptr->cp_tr = TR;
  HB = bptr->cp_h = H;
#ifdef DEPTH_LIMIT
  bptr->cp_depth = DEPTH;
#endif
  bptr->cp_b = B;
  bptr->cp_cp = cp_pc;
  bptr->cp_ap = ap_pc;
  bptr->cp_env = ENV;
  /* now, install the new YREG */
  ASP = (CELL *)bptr;
  ASP[E_CB] = (CELL)bptr;
  B = bptr;
#ifdef YAPOR
  SCH_set_load(B);
#endif	/* YAPOR */
  SET_BB(bptr);
}

static void
update_clause_choice_point(yamop *ipc, yamop *ap_pc)
{
  Term tpc = MkIntegerTerm((Int)ipc);
  B->cp_args[1] = tpc;
  B->cp_h = H;
  B->cp_ap = ap_pc;
}

static LogUpdClause *
to_clause(yamop *ipc, PredEntry *ap)
{
  if (ap->PredFlags & LogUpdatePredFlag)
    return lu_clause(ipc, ap);
  else if (ap->PredFlags & MegaClausePredFlag)
    return (LogUpdClause *)ipc;
  else
    return (LogUpdClause *)simple_static_clause(ipc, ap);
}

LogUpdClause *
Yap_FollowIndexingCode(PredEntry *ap, yamop *ipc, Term Terms[3], yamop *ap_pc, yamop *cp_pc)
{
  CELL *s_reg = NULL;
  Term t = TermNil;
  yamop *start_pc = ipc;
  choiceptr b0 = NULL;
  yamop **jlbl = NULL;
  int lu_pred = ap->PredFlags & LogUpdatePredFlag;
  int unbounded = TRUE;

  if (ap->ModuleOfPred != IDB_MODULE) {
    if (ap->ArityOfPE) {
      CELL *tar = RepAppl(Deref(Terms[0]));
      UInt i;

      for (i = 1; i <= ap->ArityOfPE; i++) {
	XREGS[i] = tar[i];
      }
    }
  }
  /* try to refine the interval using the indexing code */
  while (ipc != NULL) {
    op_numbers op = Yap_op_from_opcode(ipc->opc);
    switch(op) {
    case _try_in:
      update_clause_choice_point(NEXTOP(ipc,l), ap_pc);
      if (lu_pred)
	return lu_clause(ipc->u.l.l, ap);
      else
	return (LogUpdClause *)static_clause(ipc->u.l.l, ap, unbounded);
      break;
    case _try_clause:
#if TABLING
    case _table_try:
#endif
      if (b0 == NULL)
	store_clause_choice_point(Terms[0], Terms[1], Terms[2], NEXTOP(ipc,Otapl), ap, ap_pc, cp_pc);
      else {
	B = b0;
	b0 = NULL;
	update_clause_choice_point(NEXTOP(ipc,Otapl), ap_pc);
      }
      if (lu_pred)
	return lu_clause(ipc->u.Otapl.d, ap);
      else
	return (LogUpdClause *)static_clause(ipc->u.Otapl.d, ap, unbounded);
    case _try_clause2:
    case _try_clause3:
    case _try_clause4:
      if (b0 == NULL)
	store_clause_choice_point(Terms[0], Terms[1], Terms[2], NEXTOP(ipc,l), ap, ap_pc, cp_pc);
      else {
	B = b0;
	b0 = NULL;
	update_clause_choice_point(NEXTOP(ipc,l), ap_pc);
      }
      if (lu_pred)
	return lu_clause(ipc->u.l.l, ap);
      else
	return (LogUpdClause *)static_clause(ipc->u.l.l, ap, unbounded);
    case _try_me:
#if TABLING
    case _table_try_me:
#endif
      if (b0 == NULL)
	store_clause_choice_point(Terms[0], Terms[1], Terms[2], ipc->u.Otapl.d, ap, ap_pc, cp_pc);
      else {
	B = b0;
	b0 = NULL;
	update_clause_choice_point(ipc->u.Otapl.d, ap_pc);
      }
      ipc = NEXTOP(ipc,Otapl);
      break;
    case _retry_profiled:
    case _count_retry:
      ipc = NEXTOP(ipc,p);
      break;
    case _retry:
#if TABLING
    case _table_retry:
#endif
      update_clause_choice_point(NEXTOP(ipc,Otapl),ap_pc);
      if (lu_pred)
	return lu_clause(ipc->u.Otapl.d, ap);
      else
	return (LogUpdClause *)static_clause(ipc->u.Otapl.d, ap, TRUE);
    case _retry2:
    case _retry3:
    case _retry4:
      update_clause_choice_point(NEXTOP(ipc,l),ap_pc);
      if (lu_pred)
	return lu_clause(ipc->u.l.l, ap);
      else
	return (LogUpdClause *)static_clause(ipc->u.l.l, ap, TRUE);
    case _retry_me:
      update_clause_choice_point(ipc->u.Otapl.d,ap_pc);
      ipc = NEXTOP(ipc,Otapl);
      break;
    case _trust:
#if TABLING
    case _table_trust:
#endif
#ifdef CUT_C
      {
	while (POP_CHOICE_POINT(B->cp_b))
	  {
	    POP_EXECUTE();
	  }
      }
#endif /* CUT_C */
#ifdef YAPOR
      {
	choiceptr cut_pt;
	cut_pt = B->cp_b;
	CUT_prune_to(cut_pt);
	B = cut_pt;
      }
#else
      B = B->cp_b;
#endif /* YAPOR */
      b0 = B;
      if (lu_pred)
	return lu_clause(ipc->u.Otapl.d, ap);
      else
	return (LogUpdClause *)static_clause(ipc->u.Otapl.d, ap, TRUE);
    case _profiled_trust_me:
    case _trust_me:
    case _count_trust_me:
#if TABLING
    case _table_trust_me:
#endif
      b0 = B;
#ifdef CUT_C
      {
	while (POP_CHOICE_POINT(B->cp_b))
	  {
	    POP_EXECUTE();
	  }
      }
#endif /* CUT_C */
#ifdef YAPOR
      {
	choiceptr cut_pt;
	cut_pt = B->cp_b;
	CUT_prune_to(cut_pt);
	B = cut_pt;
      }
#else
      B = B->cp_b;
#endif /* YAPOR */
      ipc = NEXTOP(ipc,Otapl);
      break;
    case _enter_lu_pred:
      {
	LogUpdIndex *cl = ipc->u.Ills.I;
	PredEntry *ap = cl->ClPred;

	if (ap->LastCallOfPred != LUCALL_EXEC) {
	  /*
	    only increment time stamp if we are working on current time
	    stamp
	  */
	  if (ap->TimeStampOfPred >= TIMESTAMP_RESET)
	    Yap_UpdateTimestamps(ap);
	  ap->TimeStampOfPred++;
	  /*	  fprintf(stderr,"R %x--%d--%ul\n",ap,ap->TimeStampOfPred,ap->ArityOfPE);*/
	  ap->LastCallOfPred = LUCALL_EXEC;
	}
	*--ASP = MkIntegerTerm(ap->TimeStampOfPred);
	/* indicate the indexing code is being used */
#if defined(YAPOR) || defined(THREADS)
	/* just store a reference */
	INC_CLREF_COUNT(cl);
	TRAIL_CLREF(cl);
#else
	if (!(cl->ClFlags & InUseMask)) {
	  cl->ClFlags |= InUseMask;
	  TRAIL_CLREF(cl);
	}	
#endif
      }
      ipc = ipc->u.Ills.l1;
      break;
    case _try_logical:
      if (b0 == NULL)
	store_clause_choice_point(Terms[0], Terms[1], Terms[2], ipc->u.OtaLl.n, ap, ap_pc, cp_pc);
      else {
	B = b0;
	b0 = NULL;
	update_clause_choice_point(ipc->u.OtaLl.n, ap_pc);
      }
      {
	UInt timestamp = IntegerOfTerm(((CELL *)(B+1))[5]);
   
	if (!VALID_TIMESTAMP(timestamp, ipc->u.OtaLl.d)) {
	  /* jump to next instruction */
	  ipc = ipc->u.OtaLl.n;
	  break;
	}
      }
      return ipc->u.OtaLl.d;
    case _retry_logical:
    case _profiled_retry_logical:
    case _count_retry_logical:
      {
	UInt timestamp = IntegerOfTerm(((CELL *)(B+1))[5]);
	if (!VALID_TIMESTAMP(timestamp, ipc->u.OtaLl.d)) {
	  /* jump to next instruction */
	  ipc = ipc->u.OtaLl.n;
	  break;
	}
      }
      update_clause_choice_point(ipc->u.OtILl.n,ap_pc);
      return ipc->u.OtILl.d;
#if TABLING
    case _table_try_single:
      return (LogUpdClause *)ClauseCodeToStaticClause(ipc);
#endif
    case _trust_logical:
    case _count_trust_logical:
    case _profiled_trust_logical:
      {
	UInt timestamp = ((CELL *)(B+1))[5];
	LogUpdIndex *cl = ipc->u.OtILl.block;
	LogUpdClause *newpc;

	if (!VALID_TIMESTAMP(timestamp, ipc->u.OtILl.d)) {
	  /* jump to next instruction */
	  newpc = NULL;
	} else {
	  newpc = ipc->u.OtILl.d;
	}
#if defined(YAPOR) || defined(THREADS)
	B->cp_tr--;
	TR--;
	DEC_CLREF_COUNT(cl);
	/* actually get rid of the code */
	if (cl->ClRefCount == 0 && cl->ClFlags & (ErasedMask|DirtyMask)) {
	  /* I am the last one using this clause, hence I don't need a lock
	     to dispose of it 
	  */
	  if (cl->ClFlags & ErasedMask) {
	    Yap_ErLogUpdIndex(cl);
	  } else {
	    Yap_CleanUpIndex(cl);
	  }
	}
#else
	if (TrailTerm(B->cp_tr-1) == CLREF_TO_TRENTRY(cl) &&
	    B->cp_tr != B->cp_b->cp_tr) {
	  
	  B->cp_tr--;
	  TR--;
	  cl->ClFlags &= ~InUseMask;
	  /* next, recover space for the indexing code if it was erased */
	  if (cl->ClFlags & (ErasedMask|DirtyMask)) {
	    LogUpdClause *lcl = ipc->u.OtILl.d;
	    /* make sure we don't erase the clause we are jumping to */
	    if (lcl->ClRefCount == 1 && !(lcl->ClFlags & (DirtyMask|InUseMask))) {
	      lcl->ClFlags |= InUseMask;
	      TRAIL_CLREF(lcl);
	    }
	    if (cl->ClFlags & ErasedMask) {
	      Yap_ErLogUpdIndex(cl);
	    } else {
	      Yap_CleanUpIndex(cl);
	    }
	  }
	}
#endif
#ifdef CUT_C
	{
	  while (POP_CHOICE_POINT(B->cp_b))
	    {
	      POP_EXECUTE();
	    }
	}
#endif /* CUT_C */
#ifdef YAPOR
	{
	  choiceptr cut_pt;
	  cut_pt = B->cp_b;
	  CUT_prune_to(cut_pt);
	  B = cut_pt;
	}
#else
	B = B->cp_b;
#endif /* YAPOR */
	b0 = B;
	return newpc;
      }
    case _jump:
      ipc = ipc->u.l.l;
      break;
    case _jump_if_var:
      {
	Term t = Deref(ARG1);
	if (IsVarTerm(t)) {
	  jlbl = &(ipc->u.l.l);
	  ipc = ipc->u.l.l;
	} else {
	  ipc = NEXTOP(ipc,l);
	}
      }
      break;
    case _jump_if_nonvar:
      {
	Term t = Deref(XREGS[arg_from_x(ipc->u.xll.x)]);
	if (!IsVarTerm(t)) {
	  jlbl = &(ipc->u.xll.l1);
	  ipc = ipc->u.xll.l1;
	} else {
	  ipc = NEXTOP(ipc,xll);
	}
      }
      break;
    case _user_switch:
      ipc = ipc->u.lp.l;
      break;
      /* instructions type e */
    case _switch_on_type:
      t = Deref(ARG1);
      if (IsVarTerm(t)) {
	jlbl = &(ipc->u.llll.l4);
	ipc = ipc->u.llll.l4;
      } else if (IsPairTerm(t)) {
	unbounded = FALSE;
	jlbl = &(ipc->u.llll.l1);
	ipc = ipc->u.llll.l1;
	S = s_reg = RepPair(t);
      } else if (IsAtomOrIntTerm(t)) {
	jlbl = &(ipc->u.llll.l2);
	ipc = ipc->u.llll.l2;
      } else {
	jlbl = &(ipc->u.llll.l3);
	ipc = ipc->u.llll.l3;
	S = RepAppl(t);
      }
      break;
    case _switch_list_nl:
      t = Deref(ARG1);
      if (IsVarTerm(t)) {
	jlbl = &(ipc->u.ollll.l4);
	ipc = ipc->u.ollll.l4;
      } else if (IsPairTerm(t)) {
	unbounded = FALSE;
	jlbl = &(ipc->u.ollll.l1);
	ipc = ipc->u.ollll.l1;
	S = s_reg = RepPair(t);
      } else if (t == TermNil) {
	unbounded = FALSE;
	jlbl = &(ipc->u.ollll.l2);
	ipc = ipc->u.ollll.l2;
      } else {
	jlbl = &(ipc->u.ollll.l3);
	ipc = ipc->u.ollll.l3;
	S = RepAppl(t);
      }
      break;
    case _switch_on_arg_type:
      t = Deref(XREGS[arg_from_x(ipc->u.xllll.x)]);
      if (IsVarTerm(t)) {
	jlbl = &(ipc->u.xllll.l4);
	ipc = ipc->u.xllll.l4;
      } else if (IsPairTerm(t)) {
	unbounded = FALSE;
	jlbl = &(ipc->u.xllll.l1);
	ipc = ipc->u.xllll.l1;
	S = s_reg = RepPair(t);
      } else if (IsAtomOrIntTerm(t)) {
	jlbl = &(ipc->u.xllll.l1);
	ipc = ipc->u.xllll.l2;
      } else {
	jlbl = &(ipc->u.xllll.l3);
	ipc = ipc->u.xllll.l3;
	S = RepAppl(t);
      }
      break;
    case _switch_on_sub_arg_type:
      t = Deref(s_reg[ipc->u.sllll.s]);
      if (IsVarTerm(t)) {
	jlbl = &(ipc->u.sllll.l4);
	ipc = ipc->u.sllll.l4;
      } else if (IsPairTerm(t)) {
	unbounded = FALSE;
	jlbl = &(ipc->u.sllll.l1);
	ipc = ipc->u.sllll.l1;
	S = s_reg = RepPair(t);
      } else if (IsAtomOrIntTerm(t)) {
	jlbl = &(ipc->u.sllll.l2);
	ipc = ipc->u.sllll.l2;
      } else {
	jlbl = &(ipc->u.sllll.l3);
	ipc = ipc->u.sllll.l3;
	S = RepAppl(t);
      }
      break;
    case _if_not_then:
      t = Deref(ARG1);
      if (IsVarTerm(t)) {
	jlbl = &(ipc->u.clll.l3);
	ipc = ipc->u.clll.l3;
      } else if (!IsVarTerm(t) && t != ipc->u.clll.c) {
	jlbl = &(ipc->u.clll.l1);
	ipc = ipc->u.clll.l1;
      } else {
	jlbl = &(ipc->u.clll.l2);
	ipc = ipc->u.clll.l2;
      }
      break;
      /* instructions type ollll */
    case _switch_on_func:
    case _if_func:
    case _go_on_func:
      {
	FuncSwiEntry *fe;
	Functor f;
	
	unbounded = FALSE;
	s_reg = RepAppl(t);
	f = (Functor)s_reg[0];
	s_reg++;
	S = s_reg;
	if (op == _switch_on_func) {
	  fe = lookup_f_hash(f, ipc->u.sssl.l, ipc->u.sssl.s);
	} else {
	  fe = lookup_f(f, ipc->u.sssl.l, ipc->u.sssl.s);
	}
	jlbl = &(fe->u.labp);
	ipc = fe->u.labp;
      }
      break;
    case _index_dbref:
      t = AbsAppl(s_reg-1);
      ipc = NEXTOP(ipc,e);
      break;
    case _index_blob:
#if SIZEOF_DOUBLE == 2*SIZEOF_LONG_INT
      t = MkIntTerm(s_reg[0]^s_reg[1]);
#else
      t = MkIntTerm(s_reg[0]);
#endif
      ipc = NEXTOP(ipc,e);
      break;
    case _index_long:
      t = MkIntTerm(s_reg[0] & (MAX_ABS_INT-1));
      ipc = NEXTOP(ipc,e);
      break;
    case _switch_on_cons:
    case _if_cons:
    case _go_on_cons:
      {
	AtomSwiEntry *ae;
	
	unbounded = FALSE;
	if (op == _switch_on_cons) {
	  ae = lookup_c_hash(t, ipc->u.sssl.l, ipc->u.sssl.s);
	} else {
	  ae = lookup_c(t, ipc->u.sssl.l, ipc->u.sssl.s);
	}
	jlbl = &(ae->u.labp);
	ipc = ae->u.labp;
      }
      break;
    case _expand_index:
    case _expand_clauses:
      XREGS[ap->ArityOfPE+1] = (CELL)s_reg;
      XREGS[ap->ArityOfPE+2] = (CELL)t;
      XREGS[ap->ArityOfPE+3] = Terms[0];
      XREGS[ap->ArityOfPE+4] = Terms[1];
      XREGS[ap->ArityOfPE+5] = Terms[2];
#if defined(YAPOR) || defined(THREADS)
      if (!same_lu_block(jlbl, ipc)) {
	ipc = *jlbl;
	break;
      }
#endif
      ipc = ExpandIndex(ap, 5, cp_pc);
      s_reg = (CELL *)XREGS[ap->ArityOfPE+1];
      t = XREGS[ap->ArityOfPE+2];
      Terms[0] = XREGS[ap->ArityOfPE+3];
      Terms[1] = XREGS[ap->ArityOfPE+4];
      Terms[2] = XREGS[ap->ArityOfPE+5];
      break;
    case _undef_p:
      return NULL;
    case _lock_lu:
      ipc = NEXTOP(ipc,p);
      break;
#if THREADS
    case _thread_local:
      ap = Yap_GetThreadPred(ap);
      ipc = ap->CodeOfPred;
      break;
#endif
    case _spy_pred:
    case _lock_pred:
      if ((ap->PredFlags & IndexedPredFlag) ||
	      ap->cs.p_code.NOfClauses <= 1) {
	ipc = ap->cs.p_code.TrueCodeOfPred;
	break;
      }
    case _index_pred:
      XREGS[ap->ArityOfPE+1] = (CELL)s_reg;
      XREGS[ap->ArityOfPE+2] = (CELL)t;
      XREGS[ap->ArityOfPE+3] = Terms[0];
      XREGS[ap->ArityOfPE+4] = Terms[1];
      XREGS[ap->ArityOfPE+5] = Terms[2];
      Yap_IPred(ap, 5, cp_pc);
      start_pc = ipc = ap->cs.p_code.TrueCodeOfPred;
      s_reg = (CELL *)XREGS[ap->ArityOfPE+1];
      t = XREGS[ap->ArityOfPE+2];
      Terms[0] = XREGS[ap->ArityOfPE+3];
      Terms[1] = XREGS[ap->ArityOfPE+4];
      Terms[2] = XREGS[ap->ArityOfPE+5];
      break;
    case _op_fail:
      if (ipc == FAILCODE)
	return NULL;
    default:
      if (b0) {
#ifdef CUT_C
      {
	while (POP_CHOICE_POINT(B->cp_b))
	  {
	    POP_EXECUTE();
	  }
      }
#endif /* CUT_C */
#ifdef YAPOR
	{
	  choiceptr cut_pt;
	  cut_pt = B->cp_b;
	  CUT_prune_to(cut_pt);
	  B = cut_pt;
	}
#else
	B = B->cp_b;
#endif /* YAPOR */
	/* I did a trust */
      }
      if (op == _op_fail)
	return NULL;
      if (lu_pred)
	return lu_clause(ipc, ap);
      else
	return (LogUpdClause *)static_clause(ipc, ap, unbounded);
    }
  }
  if (b0) {
    /* I did a trust */
#ifdef CUT_C
      {
	while (POP_CHOICE_POINT(B->cp_b))
	  {
	    POP_EXECUTE();
	  }
      }
#endif /* CUT_C */
#ifdef YAPOR
    {
      choiceptr cut_pt;
      cut_pt = B->cp_b;
      CUT_prune_to(cut_pt);
      B = cut_pt;
    }
#else
    B = B->cp_b;
#endif /* YAPOR */
  }
  return NULL;
}

LogUpdClause *
Yap_NthClause(PredEntry *ap, Int ncls)
{
  yamop
    *ipc = ap->cs.p_code.TrueCodeOfPred,
    *alt = NULL;
  yamop **jlbl = NULL;

  /* search every clause */
  if (ncls > ap->cs.p_code.NOfClauses)
    return NULL;
  else if (ncls == 1)
    return to_clause(ap->cs.p_code.FirstClause,ap);
  else if (ncls == ap->cs.p_code.NOfClauses)
    return to_clause(ap->cs.p_code.LastClause,ap);
  else if (ncls < 0)
    return NULL;
  
  if (ap->ModuleOfPred != IDB_MODULE) {
    if (ap->ArityOfPE) {
      UInt i;

      for (i = 1; i <= ap->ArityOfPE; i++) {
	XREGS[i] = MkVarTerm();
      }
    }
  } else {
    ARG2 = MkVarTerm();
  }
  while (TRUE) {
    op_numbers op = Yap_op_from_opcode(ipc->opc);

    switch(op) {
    case _try_in:
      if (ncls == 1)
	return to_clause(ipc->u.l.l, ap);
      ncls--;
      ipc = NEXTOP(ipc,l);
      break;
    case _retry_profiled:
    case _count_retry:
      ipc = NEXTOP(ipc,p);
    case _try_clause:
    case _retry:
      if (ncls == 1)
	return to_clause(ipc->u.Otapl.d, ap);
      else if (alt == NULL) {
	ncls --;
	/* get there in a fell swoop */
	if (ap->PredFlags & ProfiledPredFlag) {
	  if (ap->PredFlags & CountPredFlag) {
	    ipc = (yamop *)((char *)ipc+ncls*(UInt)NEXTOP(NEXTOP(NEXTOP((yamop *)NULL,Otapl),p),p));
	  } else {
	    ipc = (yamop *)((char *)ipc+ncls*(UInt)NEXTOP(NEXTOP((yamop *)NULL,Otapl),p));
	  }
	} else if (ap->PredFlags & CountPredFlag) {
	  ipc = (yamop *)((char *)ipc+ncls*(UInt)NEXTOP(NEXTOP((yamop *)NULL,Otapl),p));
	} else {
	  ipc = (yamop *)((char *)ipc+ncls*(UInt)NEXTOP((yamop *)NULL,Otapl));
	}
	return to_clause(ipc->u.Otapl.d, ap);
      } else {
	ncls--;
      }
      ipc = NEXTOP(ipc,Otapl);
      break;
    case _try_clause2:
    case _try_clause3:
    case _try_clause4:
    case _retry2:
    case _retry3:
    case _retry4:
      if (ncls == 1)
	return to_clause(ipc->u.l.l, ap);
      else if (alt == NULL) {
	ncls --;
	/* get there in a fell swoop */
	if (ap->PredFlags & ProfiledPredFlag) {
	  if (ap->PredFlags & CountPredFlag) {
	    ipc = (yamop *)((char *)ipc+ncls*(UInt)NEXTOP(NEXTOP(NEXTOP((yamop *)NULL,l),p),p));
	  } else {
	    ipc = (yamop *)((char *)ipc+ncls*(UInt)NEXTOP(NEXTOP((yamop *)NULL,l),p));
	  }
	} else if (ap->PredFlags & CountPredFlag) {
	  ipc = (yamop *)((char *)ipc+ncls*(UInt)NEXTOP(NEXTOP((yamop *)NULL,l),p));
	} else {
	  ipc = (yamop *)((char *)ipc+ncls*(UInt)NEXTOP((yamop *)NULL,l));
	}
	return to_clause(ipc->u.l.l, ap);
      } else {
	ncls--;
      }
      ipc = NEXTOP(ipc,l);
      break;
    case _trust:
      if (ncls == 1)
	return to_clause(ipc->u.l.l,ap);
      ncls--;
      ipc = alt;
      break;
    case _try_me:
    case _retry_me:
      alt = ipc->u.Otapl.d;
      ipc = NEXTOP(ipc,Otapl);
      break;
    case _profiled_trust_me:
    case _trust_me:
    case _count_trust_me:
      alt = NULL;
      ipc = NEXTOP(ipc,Otapl);
      break;
    case _try_logical:
    case _retry_logical:
    case _count_retry_logical:
    case _profiled_retry_logical:
      if (VALID_TIMESTAMP(ap->TimeStampOfPred, ipc->u.OtaLl.d)) {
	if (ncls == 1)
	  return ipc->u.OtaLl.d;
	ncls--;
      }
      ipc = ipc->u.OtaLl.n;
      break;
    case _trust_logical:
    case _count_trust_logical:
    case _profiled_trust_logical:
      if (VALID_TIMESTAMP(ap->TimeStampOfPred, ipc->u.OtILl.d)) {
	if (ncls == 1)
	  return ipc->u.OtILl.d;
      }
      return NULL;
    case _enter_lu_pred:
      ipc = ipc->u.Ills.l1;
      break;
    case _lock_lu:
      ipc = NEXTOP(ipc,p);
      break;
    case _jump:
      jlbl = &(ipc->u.l.l);
      ipc = ipc->u.l.l;
      break;
    case _jump_if_var:
      jlbl = &(ipc->u.l.l);
      ipc = ipc->u.l.l;
      break;
    case _jump_if_nonvar:
      ipc = NEXTOP(ipc,xll);
      break;
    case _user_switch:
      ipc = ipc->u.lp.l;
      break;
      /* instructions type e */
    case _switch_on_type:
      jlbl = &(ipc->u.llll.l4);
      ipc = ipc->u.llll.l4;
      break;
    case _switch_list_nl:
      jlbl = &(ipc->u.ollll.l4);
      ipc = ipc->u.ollll.l4;
      break;
    case _switch_on_arg_type:
      jlbl = &(ipc->u.xllll.l4);
      ipc = ipc->u.xllll.l4;
      break;
    case _switch_on_sub_arg_type:
      jlbl = &(ipc->u.sllll.l4);
      ipc = ipc->u.sllll.l4;
      break;
    case _if_not_then:
      jlbl = &(ipc->u.clll.l3);
      ipc = ipc->u.clll.l3;
      break;
    case _expand_index:
    case _expand_clauses:
#if defined(YAPOR) || defined(THREADS)
      if (*jlbl != (yamop *)&(ap->cs.p_code.ExpandCode)) {
	ipc = *jlbl;
	break;
      }
#endif
      ipc = ExpandIndex(ap, 0, CP);

      break;
    case _op_fail:
      ipc = alt;
      break;
    case _lock_pred:
    case _index_pred:
    case _spy_pred:
      Yap_IPred(ap, 0, CP);
      ipc = ap->cs.p_code.TrueCodeOfPred;
      break;
    case _undef_p:
    default:
      return NULL;
    }
  }
}

void
Yap_CleanUpIndex(LogUpdIndex *blk)
{
  /* just compact the code */
  yamop *start = blk->ClCode, *codep;
  op_numbers op = Yap_op_from_opcode(start->opc);

  blk->ClFlags &= ~DirtyMask;
  while (op == _lock_lu) {
    start = NEXTOP(start, p);
    op = Yap_op_from_opcode(start->opc);
  }
  while (op == _jump_if_nonvar) {
    start = NEXTOP(start, xll);
    op = Yap_op_from_opcode(start->opc);
  }
  codep = start->u.Ills.l1;
  remove_dirty_clauses_from_index(start);
}

