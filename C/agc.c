/*************************************************************************
*									 *
*	 YAP Prolog 							 *
*									 *
*	Yap Prolog was developed at NCCUP - Universidade do Porto	 *
*									 *
* Copyright L.Damas, V.S.Costa and Universidade do Porto 1985-1997	 *
*									 *
**************************************************************************
*									 *
* File:		agc.c							 *
* Last rev:								 *
* mods:									 *
* comments:	reclaim unused atoms and functors			 *
*									 *
*************************************************************************/
#ifdef SCCS
static char     SccsId[] = "@(#)agc.c	1.3 3/15/90";
#endif


#include "absmi.h"
#include "Foreign.h"
#include "alloc.h"
#include "yapio.h"
#include "iopreds.h"
#include "attvar.h"

#ifdef DEBUG
/* #define DEBUG_RESTORE1 1 */
/* #define DEBUG_RESTORE2 1 */
/* #define DEBUG_RESTORE3 1 */
#define errout Yap_stderr
#endif

STATIC_PROTO(void  RestoreEntries, (PropEntry *, int));
STATIC_PROTO(void  CleanCode, (PredEntry *));

static int agc_calls;

static YAP_ULONG_LONG agc_collected;

static Int tot_agc_time = 0; /* total time spent in GC */

static Int tot_agc_recovered = 0; /* number of heap objects in all garbage collections */

#define AtomMarkedBit 1

static inline void
MarkAtomEntry(AtomEntry *ae)
{
  CELL c = (CELL)(ae->NextOfAE);
  c |= AtomMarkedBit;
  ae->NextOfAE = (Atom)c;
}

static inline int
AtomResetMark(AtomEntry *ae)
{
  CELL c = (CELL)(ae->NextOfAE);
  if (c & AtomMarkedBit) {
    c &= ~AtomMarkedBit;
    ae->NextOfAE = (Atom)c;
    return TRUE;
  }
  return FALSE;
}

static inline Atom
CleanAtomMarkedBit(Atom a)
{
  CELL c = (CELL)a;
  c &= ~AtomMarkedBit;
  return (Atom)c;
}

static inline Functor
FuncAdjust(Functor f)
{
  if (!IsExtensionFunctor(f)) {  
    AtomEntry *ae = RepAtom(NameOfFunctor(f));
    MarkAtomEntry(ae);
  }
  return(f);
}


static inline Term
AtomTermAdjust(Term t)
{
  AtomEntry *ae = RepAtom(AtomOfTerm(t));
  MarkAtomEntry(ae);
  return(t);  
}

static inline Atom
AtomAdjust(Atom a)
{
  AtomEntry *ae;
  if (a == NIL) return(a);
  ae = RepAtom(a);
  MarkAtomEntry(ae);
  return(a);
}

#define IsOldCode(P) FALSE
#define IsOldCodeCellPtr(P) FALSE
#define IsOldDelay(P) FALSE
#define IsOldDelayPtr(P) FALSE
#define IsOldLocalInTR(P) FALSE
#define IsOldLocalInTRPtr(P) FALSE
#define IsOldGlobal(P) FALSE
#define IsOldGlobalPtr(P) FALSE
#define IsOldTrail(P) FALSE
#define IsOldTrailPtr(P) FALSE

#define CharP(X) ((char *)(X))

#define REINIT_LOCK(P) 
#define REINIT_RWLOCK(P) 
#define OrArgAdjust(P) 
#define TabEntryAdjust(P) 
#define IntegerAdjust(D)  (D)
#define AddrAdjust(P) (P)
#define MFileAdjust(P) (P)
#define CodeVarAdjust(P) (P)
#define ConstantAdjust(P) (P)
#define ArityAdjust(P) (P)
#define DoubleInCodeAdjust(P) 
#define IntegerInCodeAdjust(P) 
#define OpcodeAdjust(P) (P)
#define ModuleAdjust(P) (P)
#define ExternalFunctionAdjust(P) (P)
#define PredEntryAdjust(P) (P)
#define ModEntryPtrAdjust(P) (P)
#define AtomEntryAdjust(P) (P)
#define GlobalEntryAdjust(P) (P)
#define BlobTermAdjust(P) (P)
#define CodeComposedTermAdjust(P) (P)
#define CellPtoHeapAdjust(P) (P)
#define PtoAtomHashEntryAdjust(P) (P)
#define CellPtoHeapCellAdjust(P) (P)
#define CellPtoTRAdjust(P) (P)
#define CodeAddrAdjust(P) (P)
#define ConsultObjAdjust(P) (P)
#define DelayAddrAdjust(P) (P)
#define DelayAdjust(P) (P)
#define GlobalAdjust(P) (P)
#define DBRefAdjust(P) (P)
#define DBRefPAdjust(P) (P)
#define DBTermAdjust(P) (P)
#define LUIndexAdjust(P) (P)
#define SIndexAdjust(P) (P)
#define LocalAddrAdjust(P) (P)
#define GlobalAddrAdjust(P) (P)
#define OpListAdjust(P) (P)
#define PtoLUCAdjust(P) (P)
#define PtoStCAdjust(P) (P)
#define PtoArrayEAdjust(P) (P)
#define PtoArraySAdjust(P) (P)
#define PtoGlobalEAdjust(P) (P)
#define PtoDelayAdjust(P) (P)
#define PtoGloAdjust(P) (P)
#define PtoLocAdjust(P) (P)
#define PtoHeapCellAdjust(P) (P)
#define PtoOpAdjust(P) (P)
#define PtoLUClauseAdjust(P) (P)
#define PtoLUIndexAdjust(P) (P)
#define PtoDBTLAdjust(P) (P)
#define PtoPredAdjust(P) (P)
#define PtoPtoPredAdjust(P) (P)
#define OpRTableAdjust(P) (P)
#define OpEntryAdjust(P) (P)
#define PropAdjust(P) (P)
#define TrailAddrAdjust(P) (P)
#define XAdjust(P) (P)
#define YAdjust(P) (P)
#define HoldEntryAdjust(P) (P)
#define CodeCharPAdjust(P) (P)
#define CodeVoidPAdjust(P) (P)

#define recompute_mask(dbr)

#define rehash(oldcode, NOfE, KindOfEntries)

#include "rheap.h"

static void init_reg_copies(void)
{
  OldASP = ASP;
  OldLCL0 = LCL0;
  OldTR = TR;
  OldGlobalBase = (CELL *)Yap_GlobalBase;
  OldH = H;
  OldH0 = H0;
  OldTrailBase = Yap_TrailBase;
  OldTrailTop = Yap_TrailTop;
  OldHeapBase = Yap_HeapBase;
  OldHeapTop = HeapTop;
}


static void
RestoreAtomList(Atom atm)
{
  AtomEntry      *at;

  at = RepAtom(atm);
  if (EndOfPAEntr(at))
    return;
  do {
    RestoreAtom(atm);
    atm = CleanAtomMarkedBit(at->NextOfAE);
    at = RepAtom(atm);
  } while (!EndOfPAEntr(at));
}



static void
mark_trail(void)
{
  register tr_fr_ptr pt;

  pt = TR;
  /* moving the trail is simple */
  while (pt != (tr_fr_ptr)Yap_TrailBase) {
    CELL reg = TrailTerm(pt-1);

    if (!IsVarTerm(reg)) {
      if (IsAtomTerm(reg)) {
	MarkAtomEntry(RepAtom(AtomOfTerm(reg)));
      }
    }

    pt--;
  }
}

static void
mark_registers(void)
{
  CELL *pt;

  pt = XREGS;
  /* moving the trail is simple */
  while (pt != XREGS+MaxTemps) {
    CELL reg = *pt++;

    if (!IsVarTerm(reg)) {
      if (IsAtomTerm(reg)) {
	MarkAtomEntry(RepAtom(AtomOfTerm(reg)));
      }
    }
  }
}

static void
mark_local(void)
{
  CELL   *pt;

  /* Adjusting the local */
  pt = LCL0;
  /* moving the trail is simple */
  while (pt > ASP) {
    CELL reg = *--pt;

    if (!IsVarTerm(reg)) {
      if (IsAtomTerm(reg)
#ifdef TABLING
	  /* assume we cannot have atoms on first page,
	     so this must be an arity
	  */
	  && reg > Yap_page_size
#endif
	  ) {
	MarkAtomEntry(RepAtom(AtomOfTerm(reg)));
      }
    }
  }
}

static CELL *
mark_global_cell(CELL *pt)
{   
  CELL reg = *pt;

  if (IsVarTerm(reg)) {
    /* skip bitmaps */
    switch(reg) {
    case (CELL)FunctorDouble:
#if SIZEOF_DOUBLE == 2*SIZEOF_LONG_INT
      return pt + 4;
#else
      return pt + 3;
#endif
    case (CELL)FunctorBigInt:
      {
	Int sz = 3 +
	  (sizeof(MP_INT)+
	   (((MP_INT *)(pt+2))->_mp_alloc*sizeof(mp_limb_t)))/sizeof(CELL);
	return pt + sz;
      }
    case (CELL)FunctorLongInt:
      return pt + 3;
      break;
    }
  } else if (IsAtomTerm(reg)) {
    MarkAtomEntry(RepAtom(AtomOfTerm(reg)));
    return pt+1;
  }
  return pt+1;
}

static void
mark_global(void)
{
  CELL *pt;

  /*
   * to clean the global now that functors are just variables pointing to
   * the code 
   */
  pt = H0;
  while (pt < H) {
    pt = mark_global_cell(pt);
  }
}

static void
mark_stacks(void)
{
  mark_registers();
  mark_trail();
  mark_local();
  mark_global();
}

static void
clean_atom_list(AtomHashEntry *HashPtr)
{
  Atom atm = HashPtr->Entry;
  Atom *patm = &(HashPtr->Entry);
  while (atm != NIL) {
    AtomEntry *at =  RepAtom(atm);
    if (AtomResetMark(at) ||
	at->PropsOfAE != NIL ||
	(AGCHook != NULL && !AGCHook(atm))) {
      patm = &(at->NextOfAE);
      atm = at->NextOfAE;
    } else {
      NOfAtoms--;
      if (IsWideAtom(atm)) {
#ifdef DEBUG_RESTORE3
	fprintf(stderr, "Purged %p:%S\n", at, at->WStrOfAE);
#endif
	agc_collected += sizeof(AtomEntry)+wcslen(at->WStrOfAE);
      } else {
#ifdef DEBUG_RESTORE3
	fprintf(stderr, "Purged %p:%s patm=%p %p\n", at, at->StrOfAE, patm, at->NextOfAE);
#endif
	agc_collected += sizeof(AtomEntry)+strlen(at->StrOfAE);
      }
      *patm = atm = at->NextOfAE;
      Yap_FreeCodeSpace((char *)at);
    }
  }
}

/*
 * This is the really tough part, to restore the whole of the heap 
 */
static void 
clean_atoms(void)
{
  AtomHashEntry *HashPtr = HashChain;
  register int    i;

  AtomResetMark(AtomFoundVar);
  AtomResetMark(AtomFreeTerm);
  for (i = 0; i < AtomHashTableSize; ++i) {
    clean_atom_list(HashPtr);
    HashPtr++;
  }
  HashPtr = WideHashChain;
  for (i = 0; i < WideAtomHashTableSize; ++i) {
    clean_atom_list(HashPtr);
    HashPtr++;
  }
  clean_atom_list(&INVISIBLECHAIN);
}

static void
atom_gc(void)
{
  int		gc_verbose = Yap_is_gc_verbose();
  int           gc_trace = 0;
  

  UInt		time_start, agc_time;
#if  defined(YAPOR) || defined(THREADS)
  return;
#endif
  if (Yap_GetValue(AtomGcTrace) != TermNil)
    gc_trace = 1;

  agc_calls++;
  agc_collected = 0;
  
  if (gc_trace) {
    fprintf(Yap_stderr, "%% agc:\n");
  } else if (gc_verbose) {
    fprintf(Yap_stderr, "%%   Start of atom garbage collection %d:\n", agc_calls);
  }
  time_start = Yap_cputime();
  /* get the number of active registers */
  YAPEnterCriticalSection();
  init_reg_copies();
  mark_stacks();
  restore_codes();
  clean_atoms();
  AGcLastCall = NOfAtoms;
  YAPLeaveCriticalSection();
  agc_time = Yap_cputime()-time_start;
  tot_agc_time += agc_time;
  tot_agc_recovered += agc_collected;
  if (gc_verbose) {
#ifdef _WIN32
    fprintf(Yap_stderr, "%%   Collected %I64d bytes.\n", agc_collected);
#else
    fprintf(Yap_stderr, "%%   Collected %lld bytes.\n", agc_collected);
#endif
    fprintf(Yap_stderr, "%%   GC %d took %g sec, total of %g sec doing GC so far.\n", agc_calls, (double)agc_time/1000, (double)tot_agc_time/1000);
  }
}

void
Yap_atom_gc(void)
{
  atom_gc();
}

static Int
p_atom_gc(void)
{
#ifndef FIXED_STACKS
  atom_gc();
#endif  /* FIXED_STACKS */
  return TRUE;
}

static Int
p_inform_agc(void)
{
  Term tn = MkIntegerTerm(tot_agc_time);
  Term tt = MkIntegerTerm(agc_calls);
  Term ts = MkIntegerTerm(tot_agc_recovered);

  return(Yap_unify(tn, ARG2) && Yap_unify(tt, ARG1) && Yap_unify(ts, ARG3));

}

static Int
p_agc_threshold(void)
{
  Term t = Deref(ARG1);
  if (IsVarTerm(t)) {
    return Yap_unify(ARG1, MkIntegerTerm(AGcThreshold));
  } else if (!IsIntegerTerm(t)) {
    Yap_Error(TYPE_ERROR_INTEGER,t,"prolog_flag/2 agc_margin");
    return FALSE;
  } else {
    Int i = IntegerOfTerm(t);
    if (i<0) {
      Yap_Error(DOMAIN_ERROR_NOT_LESS_THAN_ZERO,t,"prolog_flag/2 agc_margin");
      return FALSE;
    } else {
      AGcThreshold = i;
      return TRUE;
    }
  }
}

void 
Yap_init_agc(void)
{
  Yap_InitCPred("$atom_gc", 0, p_atom_gc, HiddenPredFlag);
  Yap_InitCPred("$inform_agc", 3, p_inform_agc, HiddenPredFlag);
  Yap_InitCPred("$agc_threshold", 1, p_agc_threshold, HiddenPredFlag|SafePredFlag);
}
