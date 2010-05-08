/*************************************************************************
*									 *
*	 YAP Prolog 							 *
*									 *
*	Yap Prolog was developed at NCCUP - Universidade do Porto	 *
*									 *
* Copyright L.Damas, V. Santos Costa and Universidade do Porto 1985--	 *
*									 *
**************************************************************************
*									 *
* File:		stdpreds.c						 *
* comments:	General-purpose C implemented system predicates		 *
*									 *
* Last rev:     $Date: 2008-07-24 16:02:00 $,$Author: vsc $						 *
* $Log: not supported by cvs2svn $
* Revision 1.131  2008/06/12 10:55:52  vsc
* fix syntax error messages
*
* Revision 1.130  2008/04/06 11:53:02  vsc
*  fix some restore bugs
*
* Revision 1.129  2008/03/15 12:19:33  vsc
* fix flags
*
* Revision 1.128  2008/02/15 12:41:33  vsc
* more fixes to modules
*
* Revision 1.127  2008/02/13 10:15:35  vsc
* fix some bugs from yesterday plus improve support for modules in
* operators.
*
* Revision 1.126  2008/02/07 23:09:13  vsc
* don't break ISO standard in current_predicate/1.
* Include Nicos flag.
*
* Revision 1.125  2008/01/23 17:57:53  vsc
* valgrind it!
* enable atom garbage collection.
*
* Revision 1.124  2007/11/26 23:43:08  vsc
* fixes to support threads and assert correctly, even if inefficiently.
*
* Revision 1.123  2007/11/06 17:02:12  vsc
* compile ground terms away.
*
* Revision 1.122  2007/10/18 08:24:16  vsc
* fix global variables
*
* Revision 1.121  2007/10/10 09:44:24  vsc
* some more fixes to make YAP swi compatible
* fix absolute_file_name (again)
* fix setarg
*
* Revision 1.120  2007/10/08 23:02:15  vsc
* minor fixes
*
* Revision 1.119  2007/04/18 23:01:16  vsc
* fix deadlock when trying to create a module with the same name as a
* predicate (for now, just don't lock modules). obs Paulo Moura.
*
* Revision 1.118  2007/02/26 10:41:40  vsc
* fix prolog_flags for chr.
*
* Revision 1.117  2007/01/28 14:26:37  vsc
* WIN32 support
*
* Revision 1.116  2006/12/13 16:10:23  vsc
* several debugger and CLP(BN) improvements.
*
* Revision 1.115  2006/11/28 13:46:41  vsc
* fix wide_char support for name/2.
*
* Revision 1.114  2006/11/27 17:42:03  vsc
* support for UNICODE, and other bug fixes.
*
* Revision 1.113  2006/11/16 14:26:00  vsc
* fix handling of infinity in name/2 and friends.
*
* Revision 1.112  2006/11/08 01:56:47  vsc
* fix argument order in db statistics.
*
* Revision 1.111  2006/11/06 18:35:04  vsc
* 1estranha
*
* Revision 1.110  2006/10/10 14:08:17  vsc
* small fixes on threaded implementation.
*
* Revision 1.109  2006/09/15 19:32:47  vsc
* ichanges for QSAR
*
* Revision 1.108  2006/09/01 20:14:42  vsc
* more fixes for global data-structures.
* statistics on atom space.
*
* Revision 1.107  2006/08/22 16:12:46  vsc
* global variables
*
* Revision 1.106  2006/08/07 18:51:44  vsc
* fix garbage collector not to try to garbage collect when we ask for large
* chunks of stack in a single go.
*
* Revision 1.105  2006/06/05 19:36:00  vsc
* hacks
*
* Revision 1.104  2006/05/19 14:31:32  vsc
* get rid of IntArrays and FloatArray code.
* include holes when calculating memory usage.
*
* Revision 1.103  2006/05/18 16:33:05  vsc
* fix info reported by memory manager under DL_MALLOC and SYSTEM_MALLOC
*
* Revision 1.102  2006/04/28 17:53:44  vsc
* fix the expand_consult patch
*
* Revision 1.101  2006/04/28 13:23:23  vsc
* fix number of overflow bugs affecting threaded version
* make current_op faster.
*
* Revision 1.100  2006/02/05 02:26:35  tiagosoares
* MYDDAS: Top Level Functionality
*
* Revision 1.99  2006/02/05 02:17:54  tiagosoares
* MYDDAS: Top Level Functionality
*
* Revision 1.98  2005/12/17 03:25:39  vsc
* major changes to support online event-based profiling
* improve error discovery and restart on scanner.
*
* Revision 1.97  2005/11/22 11:25:59  tiagosoares
* support for the MyDDAS interface library
*
* Revision 1.96  2005/10/28 17:38:49  vsc
* sveral updates
*
* Revision 1.95  2005/10/21 16:09:02  vsc
* SWI compatible module only operators
*
* Revision 1.94  2005/09/08 22:06:45  rslopes
* BEAM for YAP update...
*
* Revision 1.93  2005/08/04 15:45:53  ricroc
* TABLING NEW: support to limit the table space size
*
* Revision 1.92  2005/07/20 13:54:27  rslopes
* solved warning: cast from pointer to integer of different size
*
* Revision 1.91  2005/07/06 19:33:54  ricroc
* TABLING: answers for completed calls can now be obtained by loading (new option) or executing (default) them from the trie data structure.
*
* Revision 1.90  2005/07/06 15:10:14  vsc
* improvements to compiler: merged instructions and fixes for ->
*
* Revision 1.89  2005/05/26 18:01:11  rslopes
* *** empty log message ***
*
* Revision 1.88  2005/04/27 20:09:25  vsc
* indexing code could get confused with suspension points
* some further improvements on oveflow handling
* fix paths in Java makefile
* changs to support gibbs sampling in CLP(BN)
*
* Revision 1.87  2005/04/07 17:48:55  ricroc
* Adding tabling support for mixed strategy evaluation (batched and local scheduling)
*   UPDATE: compilation flags -DTABLING_BATCHED_SCHEDULING and -DTABLING_LOCAL_SCHEDULING removed. To support tabling use -DTABLING in the Makefile or --enable-tabling in configure.
*   NEW: yap_flag(tabling_mode,MODE) changes the tabling execution mode of all tabled predicates to MODE (batched, local or default).
*   NEW: tabling_mode(PRED,MODE) changes the default tabling execution mode of predicate PRED to MODE (batched or local).
*
* Revision 1.86  2005/03/13 06:26:11  vsc
* fix excessive pruning in meta-calls
* fix Term->int breakage in compiler
* improve JPL (at least it does something now for amd64).
*
* Revision 1.85  2005/03/02 19:48:02  vsc
* Fix some possible errors in name/2 and friends, and cleanup code a bit
* YAP_Error changed.
*
* Revision 1.84  2005/03/02 18:35:46  vsc
* try to make initialisation process more robust
* try to make name more robust (in case Lookup new atom fails)
*
* Revision 1.83  2005/03/01 22:25:09  vsc
* fix pruning bug
* make DL_MALLOC less enthusiastic about walking through buckets.
*
* Revision 1.82  2005/02/21 16:50:04  vsc
* amd64 fixes
* library fixes
*
* Revision 1.81  2005/02/08 04:05:35  vsc
* fix mess with add clause
* improves on sigsegv handling
*
* Revision 1.80  2005/01/05 05:32:37  vsc
* Ricardo's latest version of profiler.
*
* Revision 1.79  2004/12/28 22:20:36  vsc
* some extra bug fixes for trail overflows: some cannot be recovered that easily,
* some can.
*
* Revision 1.78  2004/12/08 04:45:03  vsc
* polish changes to undefp
* get rid of a few warnings
*
* Revision 1.77  2004/12/05 05:07:26  vsc
* name/2 should accept [] as a valid list (string)
*
* Revision 1.76  2004/12/05 05:01:25  vsc
* try to reduce overheads when running with goal expansion enabled.
* CLPBN fixes
* Handle overflows when allocating big clauses properly.
*
* Revision 1.75  2004/12/02 06:06:46  vsc
* fix threads so that they at least start
* allow error handling to work with threads
* replace heap_base by Yap_heap_base, according to Yap's convention for globals.
*
* Revision 1.74  2004/11/19 22:08:43  vsc
* replace SYSTEM_ERROR by out OUT_OF_WHATEVER_ERROR whenever appropriate.
*
* Revision 1.73  2004/11/19 17:14:14  vsc
* a few fixes for 64 bit compiling.
*
* Revision 1.72  2004/11/18 22:32:37  vsc
* fix situation where we might assume nonextsing double initialisation of C predicates (use
* Hidden Pred Flag).
* $host_type was double initialised.
*
* Revision 1.71  2004/07/23 21:08:44  vsc
* windows fixes
*
* Revision 1.70  2004/06/29 19:04:42  vsc
* fix multithreaded version
* include new version of Ricardo's profiler
* new predicat atomic_concat
* allow multithreaded-debugging
* small fixes
*
* Revision 1.69  2004/06/16 14:12:53  vsc
* miscellaneous fixes
*
* Revision 1.68  2004/05/14 17:11:30  vsc
* support BigNums in interface
*
* Revision 1.67  2004/05/14 16:33:45  vsc
* add Yap_ReadBuffer
*
* Revision 1.66  2004/05/13 20:54:58  vsc
* debugger fixes
* make sure we always go back to current module, even during initizlization.
*
* Revision 1.65  2004/04/27 15:14:36  vsc
* fix halt/0 and halt/1
*									 *
*									 *
*************************************************************************/
#ifdef SCCS
static char     SccsId[] = "%W% %G%";
#endif


/*
 * This file includes the definition of a miscellania of standard predicates
 * for yap refering to: Consulting, Executing a C predicate from call,
 * Comparisons (both general and numeric), Structure manipulation, Direct
 * access to atoms and predicates, Basic support for the debugger 
 *
 * It also includes a table where all C-predicates are initializated 
 *
 */

#include "Yap.h"
#include "Yatom.h"
#include "YapHeap.h"
#include "eval.h"
#include "yapio.h"
#include <stdio.h>
#if HAVE_STRING_H
#include <string.h>
#endif
#if HAVE_MALLOC_H
#include <malloc.h>
#endif
#include <wchar.h>

STD_PROTO(static Int p_setval, (void));
STD_PROTO(static Int p_value, (void));
STD_PROTO(static Int p_values, (void));
#ifdef undefined
STD_PROTO(static CODEADDR *FindAtom, (CODEADDR, int *));
#endif /* undefined */
STD_PROTO(static Int p_opdec, (void));
STD_PROTO(static Term get_num, (char *));
STD_PROTO(static Int p_name, (void));
STD_PROTO(static Int p_atom_chars, (void));
STD_PROTO(static Int p_atom_codes, (void));
STD_PROTO(static Int p_atom_length, (void));
STD_PROTO(static Int p_atom_split, (void));
STD_PROTO(static Int p_number_chars, (void));
STD_PROTO(static Int p_number_codes, (void));
STD_PROTO(static Int p_univ, (void));
STD_PROTO(static Int p_abort, (void));
#ifdef BEAM
STD_PROTO(Int p_halt, (void));
#else
STD_PROTO(static Int p_halt, (void));
#endif
STD_PROTO(static Int init_current_atom, (void));
STD_PROTO(static Int cont_current_atom, (void));
STD_PROTO(static Int init_current_predicate, (void));
STD_PROTO(static Int cont_current_predicate, (void));
STD_PROTO(static Int init_current_predicate_for_atom, (void));
STD_PROTO(static Int cont_current_predicate_for_atom, (void));
STD_PROTO(static OpEntry *NextOp, (OpEntry *));
STD_PROTO(static Int init_current_op, (void));
STD_PROTO(static Int cont_current_op, (void));
STD_PROTO(static Int init_current_atom_op, (void));
STD_PROTO(static Int cont_current_atom_op, (void));
#ifdef DEBUG
STD_PROTO(static Int p_debug, (void));
#endif
STD_PROTO(static Int p_flags, (void));
STD_PROTO(static int AlreadyHidden, (char *));
STD_PROTO(static Int p_hide, (void));
STD_PROTO(static Int p_hidden, (void));
STD_PROTO(static Int p_unhide, (void));
STD_PROTO(static Int TrailMax, (void));
STD_PROTO(static Int GlobalMax, (void));
STD_PROTO(static Int LocalMax, (void));
STD_PROTO(static Int p_statistics_heap_max, (void));
STD_PROTO(static Int p_statistics_global_max, (void));
STD_PROTO(static Int p_statistics_local_max, (void));
STD_PROTO(static Int p_statistics_heap_info, (void));
STD_PROTO(static Int p_statistics_stacks_info, (void));
STD_PROTO(static Int p_statistics_trail_info, (void));
STD_PROTO(static Term mk_argc_list, (void));
STD_PROTO(static Int p_argv, (void));
STD_PROTO(static Int p_cputime, (void));
STD_PROTO(static Int p_systime, (void));
STD_PROTO(static Int p_runtime, (void));
STD_PROTO(static Int p_walltime, (void));
STD_PROTO(static Int p_access_yap_flags, (void));
STD_PROTO(static Int p_set_yap_flags, (void));

#ifdef BEAM
STD_PROTO(Int use_eam, (void));
STD_PROTO(Int eager_split, (void));
STD_PROTO(Int force_wait, (void));
STD_PROTO(Int commit, (void));
STD_PROTO(Int skip_while_var, (void));
STD_PROTO(Int wait_while_var, (void));
STD_PROTO(Int show_time, (void));
STD_PROTO(Int start_eam, (void));
STD_PROTO(Int cont_eam, (void));

extern int EAM;
extern int eam_am(PredEntry*);
extern int showTime(void); 

Int start_eam(void) {
  if (eam_am((PredEntry *) 0x1)) return (TRUE); 
  else { cut_fail(); return (FALSE); }
}

Int cont_eam(void) {
  if (eam_am((PredEntry *) 0x2)) return (TRUE); 
  else { cut_fail(); return (FALSE); }
}

Int use_eam(void) { 
  if (EAM)  EAM=0;
    else { Yap_PutValue(AtomCArith,0); EAM=1; }
  return(TRUE);
}

Int commit(void) { 
  if (EAM) {
  printf("Nao deveria ter sido chamado commit do stdpreds\n");
  exit(1);
  }
  return(TRUE);
}

Int skip_while_var(void) { 
  if (EAM) {
  printf("Nao deveria ter sido chamado skip_while_var do stdpreds\n");
  exit(1);
  }
  return(TRUE);
}

Int wait_while_var(void) { 
  if (EAM) {
  printf("Nao deveria ter sido chamado wait_while_var do stdpreds\n");
  exit(1);
  }
  return(TRUE);
}

Int force_wait(void) {
  if (EAM) {
  printf("Nao deveria ter sido chamado force_wait do stdpreds\n");
  exit(1);
  }
  return(TRUE);
}

Int eager_split(void) {
  if (EAM) {
  printf("Nao deveria ter sido chamado eager_split do stdpreds\n");
  exit(1);
  }
  return(TRUE);
}

Int show_time(void)  /* MORE PRECISION */
{
  return (showTime());
}

#endif /* BEAM */ 

static Int 
p_setval(void)
{				/* '$set_value'(+Atom,+Atomic) */
	Term            t1 = Deref(ARG1), t2 = Deref(ARG2);
	if (!IsVarTerm(t1) && IsAtomTerm(t1) &&
	    (!IsVarTerm(t2) && (IsAtomTerm(t2) || IsNumTerm(t2)))) {
		Yap_PutValue(AtomOfTerm(t1), t2);
		return (TRUE);
	}
	return (FALSE);
}

static Int 
p_value(void)
{				/* '$get_value'(+Atom,?Val) */
  Term t1 = Deref(ARG1);
  if (IsVarTerm(t1)) {
    Yap_Error(INSTANTIATION_ERROR,t1,"get_value/2");
    return (FALSE);
  }
  if (!IsAtomTerm(t1)) {
    Yap_Error(TYPE_ERROR_ATOM,t1,"get_value/2");
    return (FALSE);
  }
  return (Yap_unify_constant(ARG2, Yap_GetValue(AtomOfTerm(t1))));
}


static Int 
p_values(void)
{				/* '$values'(Atom,Old,New) */
  Term            t1 = Deref(ARG1), t3 = Deref(ARG3);

  if (IsVarTerm(t1)) {
    Yap_Error(INSTANTIATION_ERROR,t1,"set_value/2");
    return (FALSE);
  }
  if (!IsAtomTerm(t1)) {
    Yap_Error(TYPE_ERROR_ATOM,t1,"set_value/2");
    return (FALSE);
  }
  if (!Yap_unify_constant(ARG2, Yap_GetValue(AtomOfTerm(t1))))
    return (FALSE);
  if (!IsVarTerm(t3)) {
    if (IsAtomTerm(t3) || IsNumTerm(t3)) {
      Yap_PutValue(AtomOfTerm(t1), t3);
    } else
      return (FALSE);
  }
  return (TRUE);
}

inline static void
do_signal(yap_signals sig)
{
  LOCK(SignalLock);
  if (!Yap_InterruptsDisabled)
    CreepFlag = Unsigned(LCL0);
  ActiveSignals |= sig;
  UNLOCK(SignalLock);
}

inline static void
undo_signal(yap_signals sig)
{
  LOCK(SignalLock);
  if (ActiveSignals == sig) {
    CreepFlag = CalculateStackGap();
  }
  ActiveSignals &= ~sig;
  UNLOCK(SignalLock);
}


static Int 
p_creep(void)
{
  Atom            at;
  PredEntry      *pred;

  at = AtomCreep;
  pred = RepPredProp(PredPropByFunc(Yap_MkFunctor(at, 1),0));
  CreepCode = pred;
  do_signal(YAP_CREEP_SIGNAL);
  return TRUE;
}

static Int 
p_signal_creep(void)
{
  Atom            at;
  PredEntry      *pred;

  at = AtomCreep;
  pred = RepPredProp(PredPropByFunc(Yap_MkFunctor(at, 1),0));
  CreepCode = pred;
  LOCK(SignalLock);
  ActiveSignals |= YAP_CREEP_SIGNAL;
  UNLOCK(SignalLock);
  return TRUE;
}

static Int 
p_disable_creep(void)
{
  LOCK(SignalLock);
  if (ActiveSignals & YAP_CREEP_SIGNAL) {
    ActiveSignals &= ~YAP_CREEP_SIGNAL;    
    if (!ActiveSignals)
      CreepFlag = CalculateStackGap();
    UNLOCK(SignalLock);
    return TRUE;
  }
  UNLOCK(SignalLock);
  return FALSE;
}

/* never fails */
static Int 
p_disable_docreep(void)
{
  LOCK(SignalLock);
  if (ActiveSignals & YAP_CREEP_SIGNAL) {
    ActiveSignals &= ~YAP_CREEP_SIGNAL;    
    if (!ActiveSignals)
      CreepFlag = CalculateStackGap();
    UNLOCK(SignalLock);
  } else {
    UNLOCK(SignalLock);
  }
  return TRUE;
}

static Int 
p_stop_creep(void)
{
  LOCK(SignalLock);
  ActiveSignals &= ~YAP_CREEP_SIGNAL;
  if (!ActiveSignals) {
    CreepFlag = CalculateStackGap();
  }
  UNLOCK(SignalLock);
  return TRUE;
}

void 
Yap_signal(yap_signals sig)
{
  do_signal(sig);
}

void 
Yap_undo_signal(yap_signals sig)
{
  undo_signal(sig);
}

#ifdef undefined

/*
 * Returns where some particular piece of code is, it may take its time but
 * then you only need it while creeping, so why bother ? 
 */
static CODEADDR *
FindAtom(codeToFind, arity)
     CODEADDR        codeToFind;
     unsigned int   *arityp;
{
  Atom            a;
  int             i;

  for (i = 0; i < AtomHashTableSize; ++i) {
    READ_LOCK(HashChain[i].AeRWLock);
    a = HashChain[i].Entry;
    READ_UNLOCK(HashChain[i].AeRWLock);
    while (a != NIL) {
      register PredEntry *pp;
      AtomEntry *ae = RepAtom(a);
      READ_LOCK(ae->ARWLock);
      pp = RepPredProp(RepAtom(a)->PropsOfAE);
      while (!EndOfPAEntr(pp) && ((pp->KindOfPE & 0x8000)
				  || (pp->CodeOfPred != codeToFind)))
	pp = RepPredProp(pp->NextOfPE);
      if (pp != NIL) {
	CODEADDR *out;
	LOCK(pp->PELock);
	out = &(pp->CodeOfPred)
	*arityp = pp->ArityOfPE;
	UNLOCK(pp->PELock);
	READ_UNLOCK(ae->ARWLock);
	return (out);
      }
      a = RepAtom(a)->NextOfAE;
      READ_UNLOCK(ae->ARWLock);
    }
  }
  for (i = 0; i < WideAtomHashTableSize; ++i) {
    READ_LOCK(HashChain[i].AeRWLock);
    a = HashChain[i].Entry;
    READ_UNLOCK(HashChain[i].AeRWLock);
    while (a != NIL) {
      register PredEntry *pp;
      AtomEntry *ae = RepAtom(a);
      READ_LOCK(ae->ARWLock);
      pp = RepPredProp(RepAtom(a)->PropsOfAE);
      while (!EndOfPAEntr(pp) && ((pp->KindOfPE & 0x8000)
				  || (pp->CodeOfPred != codeToFind)))
	pp = RepPredProp(pp->NextOfPE);
      if (pp != NIL) {
	CODEADDR *out;
	LOCK(pp->PELock);
	out = &(pp->CodeOfPred)
	*arityp = pp->ArityOfPE;
	UNLOCK(pp->PELock);
	READ_UNLOCK(ae->ARWLock);
	return (out);
      }
      a = RepAtom(a)->NextOfAE;
      READ_UNLOCK(ae->ARWLock);
    }
  }
  *arityp = 0;
  return (0);
}

/*
 * This is called when you want to creep a C-predicate or a predicate written
 * in assembly 
 */
CELL 
FindWhatCreep(toCreep)
     CELL            toCreep;
{
  unsigned int    arity;
  Atom            at;
  CODEADDR       *place;

  if (toCreep > 64) {	/* written in C */
    int             i;
    place = FindAtom((CODEADDR) toCreep, &arity);
    *--ASP = Unsigned(P);
    *--ASP = N = arity;
    for (i = 1; i <= arity; ++i)
      *--ASP = X[i];
    /* P = CellPtr(CCREEPCODE);		 */
    return (Unsigned(place));
  }
}

#endif				/* undefined */

static Int 
p_opdec(void)
{				/* '$opdec'(p,type,atom)		 */
  /* we know the arguments are integer, atom, atom */
  Term            p = Deref(ARG1), t = Deref(ARG2), at = Deref(ARG3);
  Term tmod = Deref(ARG4);
  if (tmod == TermProlog) {
    tmod = PROLOG_MODULE;
  }
  return Yap_OpDec((int) IntOfTerm(p), RepAtom(AtomOfTerm(t))->StrOfAE,
		   AtomOfTerm(at), tmod);
}


#ifdef NO_STRTOD

#if HAVE_CTYPE_H
#include <ctype.h>
#endif

double 
strtod(s, pe)
	char           *s, **pe;
{
	double          r = atof(s);
	*pe = s;
	while (*s == ' ')
		++s;
	if (*s == '+' || *s == '-')
		++s;
	if (!isdigit(*s))
		return (r);
	while (isdigit(*s))
		++s;
	if (*s == '.')
		++s;
	while (isdigit(*s))
		++s;
	if (*s == 'e' || *s == 'E')
		++s;
	if (*s == '+' || *s == '-')
		++s;
	while (isdigit(*s))
		++s;
	*pe = s;
	return (r);
}

#else

#include <stdlib.h>

#endif

static char *cur_char_ptr;

static int
get_char_from_string(int s)
{
  if (cur_char_ptr[0] == '\0')
    return -1;
  cur_char_ptr++;
  return cur_char_ptr[-1];
}

    
#ifndef INFINITY
#define INFINITY (1.0/0.0)
#endif

#ifndef NAN
#define NAN      (0.0/0.0)
#endif

static Term 
get_num(char *t)
{
  Term out;

  cur_char_ptr = t;
  out = Yap_scan_num(get_char_from_string);
  /* not ever iso */
  if (out == TermNil && yap_flags[LANGUAGE_MODE_FLAG] != 1) {
    int sign = 1;
    if (t[0] == '+') {
      t++;
    }
    if (t[0] == '-') {
      t++;
      sign = -1;
    }
    if(strcmp(t,"inf") == 0) {
      if (sign > 0) {
	return MkFloatTerm(INFINITY);
      } else {
	return MkFloatTerm(-INFINITY);
      }
    }
    if(strcmp(t,"nan") == 0) {
      if (sign > 0) {
	return MkFloatTerm(NAN);
      } else {
	return MkFloatTerm(-NAN);
      }
    }
  }
  if (cur_char_ptr[0] == '\0')
    return(out);
  else
    return(TermNil);
}

static UInt 
runtime(void)
{
  return(Yap_cputime()-Yap_total_gc_time()-Yap_total_stack_shift_time());
}

/* $runtime(-SinceInterval,-SinceStart)	 */
static Int 
p_runtime(void)
{
  Int now, interval,
    gc_time,
    ss_time;
  Term tnow, tinterval;

  Yap_cputime_interval(&now, &interval);
  gc_time = Yap_total_gc_time();
  now -= gc_time;
  ss_time = Yap_total_stack_shift_time();
  now -= ss_time;
  interval -= (gc_time-LastGcTime)+(ss_time-LastSSTime);
  LastGcTime = gc_time;
  LastSSTime = ss_time;
  tnow = MkIntegerTerm(now);
  tinterval = MkIntegerTerm(interval);
  return( Yap_unify_constant(ARG1, tnow) && 
	 Yap_unify_constant(ARG2, tinterval) );
}

/* $cputime(-SinceInterval,-SinceStart)	 */
static Int 
p_cputime(void)
{
  Int now, interval;
  Yap_cputime_interval(&now, &interval);
  return( Yap_unify_constant(ARG1, MkIntegerTerm(now)) && 
	 Yap_unify_constant(ARG2, MkIntegerTerm(interval)) );
}

static Int 
p_systime(void)
{
  Int now, interval;
  Yap_systime_interval(&now, &interval);
  return( Yap_unify_constant(ARG1, MkIntegerTerm(now)) && 
	 Yap_unify_constant(ARG2, MkIntegerTerm(interval)) );
}

static Int 
p_walltime(void)
{
  Int now, interval;
  Yap_walltime_interval(&now, &interval);
  return( Yap_unify_constant(ARG1, MkIntegerTerm(now)) && 
	 Yap_unify_constant(ARG2, MkIntegerTerm(interval)) );
}

static Int 
p_char_code(void)
{
  Int t0 = Deref(ARG1);
  if (IsVarTerm(t0)) {
    Term t1 = Deref(ARG2);
    if (IsVarTerm(t1)) {
      Yap_Error(INSTANTIATION_ERROR,t0,"char_code/2");
      return(FALSE);
    } else if (!IsIntegerTerm(t1)) {
      fprintf(stderr,"hello\n"),
      Yap_Error(TYPE_ERROR_INTEGER,t1,"char_code/2");
      return(FALSE);
    } else {
      Int code = IntegerOfTerm(t1);
      Term tout;

      if (code < 0) {
	Yap_Error(REPRESENTATION_ERROR_CHARACTER_CODE,t1,"char_code/2");
	return(FALSE);
      }
      if (code > MAX_ISO_LATIN1) {
	wchar_t wcodes[2];

	wcodes[0] = code;
	wcodes[1] = '\0';
	tout = MkAtomTerm(Yap_LookupWideAtom(wcodes));
      } else {
	char codes[2];

	codes[0] = code;
	codes[1] = '\0';
	tout = MkAtomTerm(Yap_LookupAtom(codes));
      }
      return Yap_unify(ARG1,tout);
    }
  } else if (!IsAtomTerm(t0)) {
    Yap_Error(TYPE_ERROR_CHARACTER,t0,"char_code/2");
    return(FALSE);
  } else {
    Atom at = AtomOfTerm(t0);
    Term tf;

    if (IsWideAtom(at)) {
      wchar_t *c = RepAtom(at)->WStrOfAE;
      
      if (c[1] != '\0') {
	Yap_Error(TYPE_ERROR_CHARACTER,t0,"char_code/2");
	return FALSE;
      }
      tf = MkIntegerTerm(c[0]);
    } else {
      char *c = RepAtom(at)->StrOfAE;
      
      if (c[1] != '\0') {
	Yap_Error(TYPE_ERROR_CHARACTER,t0,"char_code/2");
	return FALSE;
      }
      tf = MkIntTerm((unsigned char)(c[0]));
    }
    return Yap_unify(ARG2,tf);
  }
}

static wchar_t *
ch_to_wide(char *base, char *charp)
{
  int n = charp-base, i;
  wchar_t *nb = (wchar_t *)base;

  if ((nb+n) + 1024 > (wchar_t *)AuxSp) {
    Yap_Error_TYPE = OUT_OF_AUXSPACE_ERROR;	  
    Yap_ErrorMessage = "Heap Overflow While Scanning: please increase code space (-h)";
    return NULL;
  }
  for (i=n; i > 0; i--) {
    nb[i-1] = base[i-1];
  }
  return nb+n;
}

static Int 
p_name(void)
{				/* name(?Atomic,?String)		 */
  char            *String, *s; /* alloc temp space on trail */
  Term            t = Deref(ARG2), NewT, AtomNameT = Deref(ARG1);
  wchar_t *ws = NULL;

 restart_aux:
  if (!IsVarTerm(AtomNameT)) {
    if (!IsVarTerm(t) && !IsPairTerm(t) && t != TermNil) {
      Yap_Error(TYPE_ERROR_LIST,ARG2,
		"name/2");
      return FALSE;
    }
    if (IsAtomTerm(AtomNameT)) {
      Atom at = AtomOfTerm(AtomNameT);
      if (IsWideAtom(at)) {
	NewT = Yap_WideStringToList((wchar_t *)(RepAtom(at)->StrOfAE));
	return Yap_unify(NewT, ARG2);
      } else
	String = RepAtom(at)->StrOfAE;
    } else if (IsIntTerm(AtomNameT)) {
      String = Yap_PreAllocCodeSpace();
      if (String + 1024 > (char *)AuxSp) 
	goto expand_auxsp;
#if SHORT_INTS
      sprintf(String, "%ld", IntOfTerm(AtomNameT));
#else
      sprintf(String, "%d", IntOfTerm(AtomNameT));
#endif
    } else if (IsFloatTerm(AtomNameT)) {
      String = Yap_PreAllocCodeSpace();
      if (String + 1024 > (char *)AuxSp) 
	goto expand_auxsp;

      sprintf(String, "%f", FloatOfTerm(AtomNameT));
    } else if (IsLongIntTerm(AtomNameT)) {
      String = Yap_PreAllocCodeSpace();
      if (String + 1024 > (char *)AuxSp) 
	goto expand_auxsp;

#if SHORT_INTS
      sprintf(String, "%ld", LongIntOfTerm(AtomNameT));
#else
      sprintf(String, "%d", LongIntOfTerm(AtomNameT));
#endif
#if USE_GMP
    } else if (IsBigIntTerm(AtomNameT)) {
      String = Yap_PreAllocCodeSpace();
      if (String + 1024 > (char *)AuxSp) 
	goto expand_auxsp;
      mpz_get_str(String, 10, Yap_BigIntOfTerm(AtomNameT));
#endif
    } else {
      Yap_Error(TYPE_ERROR_ATOMIC,AtomNameT,"name/2");
      return FALSE;
    }
    NewT = Yap_StringToList(String);
    return Yap_unify(NewT, ARG2);
  }
  s = String = ((AtomEntry *)Yap_PreAllocCodeSpace())->StrOfAE;
  if (String == ((AtomEntry *)NULL)->StrOfAE ||
      String + 1024 > (char *)AuxSp) 
    goto expand_auxsp;
  if (!IsVarTerm(t) && t == MkAtomTerm(AtomNil)) {
    return Yap_unify_constant(ARG1, MkAtomTerm(AtomEmptyAtom));
  }
  while (!IsVarTerm(t) && IsPairTerm(t)) {
    Term            Head;
    Int             i;

    Head = HeadOfTerm(t);
    if (IsVarTerm(Head)) {
      Yap_Error(INSTANTIATION_ERROR,Head,"name/2");
      return FALSE;
    }
    if (!IsIntegerTerm(Head)) {
      Yap_Error(TYPE_ERROR_INTEGER,Head,"name/2");
      return FALSE;
    }
    i = IntegerOfTerm(Head);
    if (i < 0) {
      Yap_Error(DOMAIN_ERROR_NOT_LESS_THAN_ZERO,Head,"name/2");
      return FALSE;
    }
    if (ws) {
      if (ws > (wchar_t *)AuxSp-1024) {
	goto expand_auxsp;
      }
      *ws++ = i;      
    } else {
      if (i > MAX_ISO_LATIN1) {
	ws = ch_to_wide(String, s);
	*ws++ = i;
      } else {
	if (s > (char *)AuxSp-1024) {
	  goto expand_auxsp;
	}
	*s++ = i;
      }
    }
    t = TailOfTerm(t);
  }
  if (ws) {
    Atom at;

    *ws = '\0';
    while ((at = Yap_LookupWideAtom((wchar_t *)String)) == NIL) {
      if (!Yap_growheap(FALSE, 0, NULL)) {
	Yap_Error(OUT_OF_HEAP_ERROR, ARG2, "generating atom from string in name/2");
	return FALSE;
      }
      /* safest to restart, we don't know what happened to String */
      t = Deref(ARG2);
      AtomNameT = Deref(ARG1);
      goto restart_aux;
    }
    NewT = MkAtomTerm(at);
    return Yap_unify_constant(ARG1, NewT);
  }
  *s = '\0';
  if (IsVarTerm(t)) {
    Yap_Error(INSTANTIATION_ERROR,t,"name/2");
    return(FALSE);
  }
  if (IsAtomTerm(t) && AtomOfTerm(t) == AtomNil) {
    if ((NewT = get_num(String)) == TermNil) {
      Atom at;
      while ((at = Yap_LookupAtom(String)) == NIL) {
	if (!Yap_growheap(FALSE, 0, NULL)) {
	  Yap_Error(OUT_OF_HEAP_ERROR, ARG2, "generating atom from string in name/2");
	  return FALSE;
	}
	/* safest to restart, we don't know what happened to String */
	t = Deref(ARG2);
	AtomNameT = Deref(ARG1);
	goto restart_aux;
      }
      NewT = MkAtomTerm(at);
    }
    return Yap_unify_constant(ARG1, NewT);
  } else {
    Yap_Error(TYPE_ERROR_LIST,ARG2,"name/2");
    return FALSE;
  }

  /* error handling */
 expand_auxsp:
  String = Yap_ExpandPreAllocCodeSpace(0,NULL, TRUE);
  if (String + 1024 > (char *)AuxSp) {
    /* crash in flames */
    Yap_Error(OUT_OF_AUXSPACE_ERROR, ARG1, "allocating temp space in name/2");
    return FALSE;
  }
  AtomNameT = Deref(ARG1);
  t = Deref(ARG2);
  goto restart_aux;

}

static Int 
p_atom_chars(void)
{
  Term t1 = Deref(ARG1);
  char           *String;

 restart_aux:
  if (!IsVarTerm(t1)) {
    Term            NewT;
    Atom at;

    if (!IsAtomTerm(t1)) {
      Yap_Error(TYPE_ERROR_ATOM, t1, "atom_chars/2");
      return(FALSE);
    }
    at = AtomOfTerm(t1);
    if (IsWideAtom(at)) {
      if (yap_flags[YAP_TO_CHARS_FLAG] == QUINTUS_TO_CHARS) {
	NewT = Yap_WideStringToList((wchar_t *)RepAtom(at)->StrOfAE);
      } else {
	NewT = Yap_WideStringToListOfAtoms((wchar_t *)RepAtom(AtomOfTerm(t1))->StrOfAE);
      }
    } else {
      if (yap_flags[YAP_TO_CHARS_FLAG] == QUINTUS_TO_CHARS) {
	NewT = Yap_StringToList(RepAtom(at)->StrOfAE);
      } else {
	NewT = Yap_StringToListOfAtoms(RepAtom(AtomOfTerm(t1))->StrOfAE);
      }
    }
    return Yap_unify(NewT, ARG2);
  } else {
    /* ARG1 unbound */
    Term   t = Deref(ARG2);
    char  *s;
    wchar_t *ws = NULL;
    Atom at;

    String = ((AtomEntry *)Yap_PreAllocCodeSpace())->StrOfAE;
    if (String + 1024 > (char *)AuxSp) 
      goto expand_auxsp;
    s = String;
    if (IsVarTerm(t)) {
      Yap_Error(INSTANTIATION_ERROR, t1, "atom_chars/2");
      return(FALSE);		
    }
    if (t == TermNil) {
      return (Yap_unify_constant(t1, MkAtomTerm(AtomEmptyAtom)));
    }
    if (!IsPairTerm(t)) {
      Yap_Error(TYPE_ERROR_LIST, t, "atom_chars/2");
      return(FALSE);		
    }
    if (yap_flags[YAP_TO_CHARS_FLAG] == QUINTUS_TO_CHARS) {
      while (t != TermNil) {
	register Term   Head;
	register Int    i;
	Head = HeadOfTerm(t);
	if (IsVarTerm(Head)) {
	  Yap_Error(INSTANTIATION_ERROR,Head,"atom_chars/2");
	  return(FALSE);
	} else if (!IsIntegerTerm(Head)) {
	  Yap_Error(REPRESENTATION_ERROR_CHARACTER_CODE,Head,"atom_chars/2");
	  return(FALSE);		
	}
	i = IntegerOfTerm(Head);
	if (i < 0) {
	  Yap_Error(REPRESENTATION_ERROR_CHARACTER_CODE,Head,"atom_chars/2");
	  return(FALSE);		
	}
	if (i > MAX_ISO_LATIN1 && !ws) {
	  ws = ch_to_wide(String, s);
	}
	if (ws) {
	  if (ws > (wchar_t *)AuxSp-1024) {
	    goto expand_auxsp;
	  }
	  *ws++ = i;      
	} else {
	  if (s+1024 > (char *)AuxSp) {
	    goto expand_auxsp;
	  }
	  *s++ = i;
	}
	t = TailOfTerm(t);
	if (IsVarTerm(t)) {
	  Yap_Error(INSTANTIATION_ERROR,t,"atom_chars/2");
	  return(FALSE);
	} else if (!IsPairTerm(t) && t != TermNil) {
	  Yap_Error(TYPE_ERROR_LIST, t, "atom_chars/2");
	  return(FALSE);
	}
      }
    } else {
      /* ISO Prolog Mode */
      while (t != TermNil) {
	register Term   Head;
	register char   *is;

	Head = HeadOfTerm(t);
	if (IsVarTerm(Head)) {
	  Yap_Error(INSTANTIATION_ERROR,Head,"atom_chars/2");
	  return(FALSE);
	} else if (!IsAtomTerm(Head)) {
	  Yap_Error(TYPE_ERROR_CHARACTER,Head,"atom_chars/2");
	  return(FALSE);		
	}
	at = AtomOfTerm(Head);
	if (IsWideAtom(at)) {
	  wchar_t *wis = (wchar_t *)RepAtom(at)->StrOfAE;
	  if (wis[1] != '\0') {
	    Yap_Error(TYPE_ERROR_CHARACTER,Head,"atom_chars/2");
	    return(FALSE);		
	  }
	  if (!ws) {
	    ws = ch_to_wide(String, s);	    
	  }
	  if (ws+1024 == (wchar_t *)AuxSp) {
	    goto expand_auxsp;
	  }
	  *ws++ = wis[0];
	} else {
	  is = RepAtom(at)->StrOfAE;
	  if (is[1] != '\0') {
	    Yap_Error(TYPE_ERROR_CHARACTER,Head,"atom_chars/2");
	    return(FALSE);		
	  }
	  if (ws) {
	    if (ws+1024 == (wchar_t *)AuxSp) {
	      goto expand_auxsp;
	    }
	    *ws++ = is[0];
	  } else {
	    if (s+1024 == (char *)AuxSp) {
	      goto expand_auxsp;
	    }
	    *s++ = is[0];
	  }
	}
	t = TailOfTerm(t);
	if (IsVarTerm(t)) {
	  Yap_Error(INSTANTIATION_ERROR,t,"atom_chars/2");
	  return(FALSE);
	} else if (!IsPairTerm(t) && t != TermNil) {
	  Yap_Error(TYPE_ERROR_LIST, t, "atom_chars/2");
	  return(FALSE);
	}
      }
    }
    if (ws) {
      *ws++ = '\0';
      while ((at = Yap_LookupWideAtom((wchar_t *)String)) == NIL) {
	if (!Yap_growheap(FALSE, 0, NULL)) {
	  Yap_Error(OUT_OF_HEAP_ERROR, TermNil, Yap_ErrorMessage);
	  return FALSE;
	}
      }
    } else {
      *s++ = '\0';
      while ((at = Yap_LookupAtom(String)) == NIL) {
	if (!Yap_growheap(FALSE, 0, NULL)) {
	  Yap_Error(OUT_OF_HEAP_ERROR, TermNil, Yap_ErrorMessage);
	  return FALSE;
	}
      }
    }
    return Yap_unify_constant(ARG1, MkAtomTerm(at));
  }
  /* error handling */
 expand_auxsp:
  String = Yap_ExpandPreAllocCodeSpace(0,NULL, TRUE);
  if (String + 1024 > (char *)AuxSp) {
    /* crash in flames */
    Yap_Error(OUT_OF_AUXSPACE_ERROR, ARG1, "allocating temp space in atom_chars/2");
    return FALSE;
  }
  t1 = Deref(ARG1);
  goto restart_aux;
}

static Int 
p_atom_concat(void)
{
  Term t1;
  int wide_mode = FALSE;
  UInt sz;

 restart:
  t1 = Deref(ARG1);
  /* we need to have a list */
  if (IsVarTerm(t1)) {
    Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
    Yap_Error(INSTANTIATION_ERROR, ARG1, "atom_concat/2");
    return FALSE;
  }
  if (wide_mode) {
    wchar_t *cptr = (wchar_t *)(((AtomEntry *)Yap_PreAllocCodeSpace())->StrOfAE), *cpt0;
    wchar_t *top = (wchar_t *)AuxSp;
    char *atom_str;
    Atom ahead;

    cpt0 = cptr;
    while (IsPairTerm(t1)) {
      Term thead = HeadOfTerm(t1);
      if (IsVarTerm(thead)) {
	Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
	Yap_Error(INSTANTIATION_ERROR, ARG1, "atom_concat/2");
	return(FALSE);
      }
      if (!IsAtomTerm(thead)) {
	Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
	Yap_Error(TYPE_ERROR_ATOM, ARG1, "atom_concat/2");
	return(FALSE);
      }
      ahead = AtomOfTerm(thead);
      atom_str = RepAtom(ahead)->StrOfAE;
      if (IsWideAtom(ahead)) {
	/* check for overflows */
	sz = wcslen((wchar_t *)atom_str);
      } else {
	sz = strlen(atom_str);
      }
      if (cptr+sz >= top-1024) {
	Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
	if (!Yap_growheap(FALSE, sz+1024, NULL)) {
	  Yap_Error(OUT_OF_HEAP_ERROR, TermNil, Yap_ErrorMessage);
	  return FALSE;
	}
	goto restart;
      }
      if (IsWideAtom(ahead)) {
	memcpy((void *)cptr, (void *)atom_str, sz*sizeof(wchar_t));
	cptr += sz;
      } else {
	int i;
	for (i=0; i < sz; i++) {
	  *cptr++ = *atom_str++;
	}
      }
      t1 = TailOfTerm(t1);
      if (IsVarTerm(t1)) {
	Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
	Yap_Error(INSTANTIATION_ERROR, ARG1, "atom_concat/2");
	return FALSE;
      }
    }
    if (t1 == TermNil) {
      Atom at;
      
      cptr[0] = '\0';
      while ((at = Yap_LookupWideAtom(cpt0)) == NIL) {
	if (!Yap_growheap(FALSE, 0, NULL)) {
	  Yap_Error(OUT_OF_HEAP_ERROR, TermNil, Yap_ErrorMessage);
	  return FALSE;
	}
      }
      Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
      return Yap_unify(ARG2, MkAtomTerm(at));
    }
  } else {
    char *cptr = ((AtomEntry *)Yap_PreAllocCodeSpace())->StrOfAE, *cpt0;
    char *top = (char *)AuxSp;
    char *atom_str;

    cpt0 = cptr;
    while (IsPairTerm(t1)) {
      Term thead = HeadOfTerm(t1);
      if (IsVarTerm(thead)) {
	Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
	Yap_Error(INSTANTIATION_ERROR, ARG1, "atom_concat/2");
	return(FALSE);
      }
      if (!IsAtomTerm(thead)) {
	Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
	Yap_Error(TYPE_ERROR_ATOM, ARG1, "atom_concat/2");
	return(FALSE);
      }
      if (IsWideAtom(AtomOfTerm(thead)) && !wide_mode) {
	wide_mode = TRUE;
	Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
	goto restart;
      }
      atom_str = RepAtom(AtomOfTerm(thead))->StrOfAE;
      /* check for overflows */
      sz = strlen(atom_str);
      if (cptr+sz >= top-1024) {
	Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
	if (!Yap_growheap(FALSE, sz+1024, NULL)) {
	  Yap_Error(OUT_OF_HEAP_ERROR, TermNil, Yap_ErrorMessage);
	  return FALSE;
	}
	goto restart;
      }
      memcpy((void *)cptr, (void *)atom_str, sz);
      cptr += sz;
      t1 = TailOfTerm(t1);
      if (IsVarTerm(t1)) {
	Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
	Yap_Error(INSTANTIATION_ERROR, ARG1, "atom_concat/2");
	return FALSE;
      }
    }
    if (t1 == TermNil) {
      Atom at;
      
      cptr[0] = '\0';
      while ((at = Yap_LookupAtom(cpt0)) == NIL) {
	if (!Yap_growheap(FALSE, 0, NULL)) {
	  Yap_Error(OUT_OF_HEAP_ERROR, TermNil, Yap_ErrorMessage);
	  return FALSE;
	}
      }
      Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
      return Yap_unify(ARG2, MkAtomTerm(at));
    }
  }
  Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
  Yap_Error(TYPE_ERROR_LIST, ARG1, "atom_concat/2");
  return FALSE;
}

static Int 
p_atomic_concat(void)
{
  Term t1;
  int wide_mode = FALSE;
  char *base;

 restart:
  base = ((AtomEntry *)Yap_PreAllocCodeSpace())->StrOfAE;
  while (base+1024 > (char *)AuxSp) {
    base = Yap_ExpandPreAllocCodeSpace(0,NULL, TRUE);
    if (base + 1024 > (char *)AuxSp) {
      /* crash in flames */
      Yap_Error(OUT_OF_AUXSPACE_ERROR, ARG1, "allocating temp space in atomic_concat/2");
      return FALSE;
    }
  }
  t1 = Deref(ARG1);
  /* we need to have a list */
  if (IsVarTerm(t1)) {
    Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
    Yap_Error(INSTANTIATION_ERROR, ARG1, "atom_concat/2");
    return FALSE;
  }
  if (wide_mode) {
    wchar_t *wcptr = (wchar_t *)base, *wcpt0;
    wchar_t *wtop = (wchar_t *)AuxSp;
    
    wcpt0 = wcptr;
    while (IsPairTerm(t1)) {
      Term thead = HeadOfTerm(t1);
      if (IsVarTerm(thead)) {
	Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
	Yap_Error(INSTANTIATION_ERROR, ARG1, "atom_concat/2");
	return FALSE;
      }
      if (!IsAtomicTerm(thead)) {
	Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
	Yap_Error(TYPE_ERROR_ATOMIC, ARG1, "atom_concat/2");
	return FALSE;
      }
      if (IsAtomTerm(thead)) {
	Atom at = AtomOfTerm(thead);

	if (IsWideAtom(at)) {
	  wchar_t *watom_str = (wchar_t *)RepAtom(AtomOfTerm(thead))->StrOfAE;
	  UInt sz = wcslen(watom_str);

	  if (wcptr+sz >= wtop-1024) {
	    Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
	    if (!Yap_growheap(FALSE, sz+1024, NULL)) {
	      Yap_Error(OUT_OF_HEAP_ERROR, TermNil, Yap_ErrorMessage);
	      return FALSE;
	    }
	    goto restart;
	  }
	  memcpy((void *)wcptr, (void *)watom_str, sz*sizeof(wchar_t));
	  wcptr += sz;
	} else {
	  char *atom_str = RepAtom(AtomOfTerm(thead))->StrOfAE;
	  /* check for overflows */
	  UInt sz = strlen(atom_str);
	  if (wcptr+sz >= wtop-1024) {
	    Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
	    if (!Yap_growheap(FALSE, sz+1024, NULL)) {
	      Yap_Error(OUT_OF_HEAP_ERROR, TermNil, Yap_ErrorMessage);
	      return FALSE;
	    }
	    goto restart;
	  }
	  while ((*wcptr++ = *atom_str++));
	  wcptr--;
	} 
      } else if (IsIntegerTerm(thead)) {
	UInt sz, i;
	char *cptr = (char *)wcptr;

#if HAVE_SNPRINTF
	sz = snprintf(cptr, (wtop-wcptr)-1024,"%ld", (long int)IntegerOfTerm(thead));
#else
	sz = sprintf(cptr,"%ld", (long int)IntegerOfTerm(thead));
#endif
	for (i=sz; i>0; i--) {
	  wcptr[i-1] = cptr[i-1];
	}
	wcptr += sz;
      } else if (IsFloatTerm(thead)) {
	char *cptr = (char *)wcptr;
	UInt i, sz;

#if HAVE_SNPRINTF
	sz = snprintf(cptr,(wtop-wcptr)-1024,"%g", FloatOfTerm(thead));
#else
	sz = sprintf(cptr,"%g", FloatOfTerm(thead));
#endif
	for (i=sz; i>0; i--) {
	  wcptr[i-1] = cptr[i-1];
	}
	wcptr += sz;
#if USE_GMP
      } else if (IsBigIntTerm(thead)) {
	MP_INT *n = Yap_BigIntOfTerm(thead);
	int sz, i;
	char *tmp = (char *)wcptr;

	if ((sz = mpz_sizeinbase (n, 10)) > (wtop-wcptr)-1024) {
	  Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
	  if (!Yap_growheap(FALSE, sz+1024, NULL)) {
	    Yap_Error(OUT_OF_HEAP_ERROR, TermNil, Yap_ErrorMessage);
	    return(FALSE);
	  }
	  goto restart;
	}
	mpz_get_str(tmp, 10, n);
	for (i=sz; i>0; i--) {
	  wcptr[i-1] = tmp[i-1];
	}
	wcptr += sz;
#endif
      }
      t1 = TailOfTerm(t1);
      if (IsVarTerm(t1)) {
	Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
	Yap_Error(INSTANTIATION_ERROR, ARG1, "atom_concat/2");
	return(FALSE);
      }
    }
    if (t1 == TermNil) {
      Atom at;

      wcptr[0] = '\0';
      while ((at = Yap_LookupWideAtom(wcpt0)) == NIL) {
	if (!Yap_growheap(FALSE, 0, NULL)) {
	  Yap_Error(OUT_OF_HEAP_ERROR, TermNil, Yap_ErrorMessage);
	  return FALSE;
	}
      }
      Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
      return Yap_unify(ARG2, MkAtomTerm(at));
    }
  } else {
    char *top = (char *)AuxSp;
    char *cpt0 = base;
    char *cptr = base;

    while (IsPairTerm(t1)) {
      Term thead = HeadOfTerm(t1);
      if (IsVarTerm(thead)) {
	Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
	Yap_Error(INSTANTIATION_ERROR, ARG1, "atom_concat/2");
	return(FALSE);
      }
      if (!IsAtomicTerm(thead)) {
	Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
	Yap_Error(TYPE_ERROR_ATOMIC, ARG1, "atom_concat/2");
	return(FALSE);
      }
      if (IsAtomTerm(thead)) {
	char *atom_str;
	UInt sz;

	if (IsWideAtom(AtomOfTerm(thead))) {
	  Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
	  wide_mode = TRUE;
	  goto restart;
	}
	atom_str = RepAtom(AtomOfTerm(thead))->StrOfAE;
	/* check for overflows */
	sz = strlen(atom_str);
	if (cptr+sz >= top-1024) {
	  Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
	  if (!Yap_growheap(FALSE, sz+1024, NULL)) {
	    Yap_Error(OUT_OF_HEAP_ERROR, TermNil, Yap_ErrorMessage);
	    return(FALSE);
	  }
	  goto restart;
	} 
	memcpy((void *)cptr, (void *)atom_str, sz);
	cptr += sz;
      } else if (IsIntegerTerm(thead)) {
#if HAVE_SNPRINTF
	snprintf(cptr, (top-cptr)-1024,"%ld", (long int)IntegerOfTerm(thead));
#else
	sprintf(cptr,"%ld", (long int)IntegerOfTerm(thead));
#endif
	while (*cptr && cptr < top-1024) cptr++;
      } else if (IsFloatTerm(thead)) {
#if HAVE_SNPRINTF
	snprintf(cptr,(top-cptr)-1024,"%g", FloatOfTerm(thead));
#else
	sprintf(cptr,"%g", FloatOfTerm(thead));
#endif
	while (*cptr && cptr < top-1024) cptr++;
#if USE_GMP
      } else if (IsBigIntTerm(thead)) {
	MP_INT *n = Yap_BigIntOfTerm(thead);
	int sz;

	if ((sz = mpz_sizeinbase (n, 10)) > (top-cptr)-1024) {
	  Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
	  if (!Yap_growheap(FALSE, sz+1024, NULL)) {
	    Yap_Error(OUT_OF_HEAP_ERROR, TermNil, Yap_ErrorMessage);
	    return(FALSE);
	  }
	  goto restart;
	}
	mpz_get_str(cptr, 10, n);
	while (*cptr) cptr++;
#endif
      }
      t1 = TailOfTerm(t1);
      if (IsVarTerm(t1)) {
	Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
	Yap_Error(INSTANTIATION_ERROR, ARG1, "atom_concat/2");
	return(FALSE);
      }
    }
    if (t1 == TermNil) {
      Atom at;

      cptr[0] = '\0';
      while ((at = Yap_LookupAtom(cpt0)) == NIL) {
	if (!Yap_growheap(FALSE, 0, NULL)) {
	  Yap_Error(OUT_OF_HEAP_ERROR, TermNil, Yap_ErrorMessage);
	  return FALSE;
	}
      }
      Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
      return Yap_unify(ARG2, MkAtomTerm(at));
    }
  }
  Yap_ReleasePreAllocCodeSpace((ADDR)cpt0);
  Yap_Error(TYPE_ERROR_LIST, ARG1, "atom_concat/2");
  return(FALSE);
}

static Int 
p_atom_codes(void)
{
  Term t1 = Deref(ARG1);
  char *String;

 restart_pred:
  if (!IsVarTerm(t1)) {
    Term            NewT;
    Atom at;

    if (!IsAtomTerm(t1)) {
      Yap_Error(TYPE_ERROR_ATOM, t1, "atom_codes/2");
      return(FALSE);
    }
    at = AtomOfTerm(t1);
    if (IsWideAtom(at)) {
      NewT = Yap_WideStringToList((wchar_t *)RepAtom(at)->StrOfAE);
    } else {
      NewT = Yap_StringToList(RepAtom(at)->StrOfAE);
    }
    return (Yap_unify(NewT, ARG2));
  } else {
    /* ARG1 unbound */
    Term   t = Deref(ARG2);
    char  *s;
    wchar_t *ws = NULL;

    String = ((AtomEntry *)Yap_PreAllocCodeSpace())->StrOfAE;
    if (String + 1024 > (char *)AuxSp) { 
      goto expand_auxsp;
    }
    s = String;
    if (IsVarTerm(t)) {
      Yap_Error(INSTANTIATION_ERROR, t1, "atom_codes/2");
      return(FALSE);		
    }
    if (t == TermNil) {
      return (Yap_unify_constant(t1, MkAtomTerm(AtomEmptyAtom)));
    }
    if (!IsPairTerm(t)) {
      Yap_Error(TYPE_ERROR_LIST, t, "atom_codes/2");
      return(FALSE);		
    }
    while (t != TermNil) {
      register Term   Head;
      register Int    i;
      Head = HeadOfTerm(t);
      if (IsVarTerm(Head)) {
	Yap_Error(INSTANTIATION_ERROR,Head,"atom_codes/2");
	return(FALSE);
      } else if (!IsIntTerm(Head)) {
	Yap_Error(REPRESENTATION_ERROR_CHARACTER_CODE,Head,"atom_codes/2");
	return(FALSE);		
      }
      i = IntOfTerm(Head);
      if (i < 0) {
	Yap_Error(REPRESENTATION_ERROR_CHARACTER_CODE,Head,"atom_codes/2");
	return(FALSE);		
      }
      if (i > MAX_ISO_LATIN1 && !ws) {
	ws = ch_to_wide(String, s);
      }
      if (ws) {
	if (ws+1024 > (wchar_t *)AuxSp) {
	  goto expand_auxsp;
	}
	*ws++ = i;
      } else {
	if (s+1024 > (char *)AuxSp) {
	  goto expand_auxsp;
	}
	*s++ = i;
      }
      t = TailOfTerm(t);
      if (IsVarTerm(t)) {
	Yap_Error(INSTANTIATION_ERROR,t,"atom_codes/2");
	return(FALSE);
      } else if (!IsPairTerm(t) && t != TermNil) {
	Yap_Error(TYPE_ERROR_LIST, t, "atom_codes/2");
	return(FALSE);
      }
    }
    if (ws) {
      *ws++ = '\0';
      return Yap_unify_constant(ARG1, MkAtomTerm(Yap_LookupWideAtom((wchar_t *)String)));
    } else {
      *s++ = '\0';
      return Yap_unify_constant(ARG1, MkAtomTerm(Yap_LookupAtom(String)));
    }
  }
  /* error handling */
 expand_auxsp:
  if (String + 1024 > (char *)AuxSp) { 
    String = Yap_ExpandPreAllocCodeSpace(0,NULL, TRUE);
  
    if (String + 1024 > (char *)AuxSp) {
      /* crash in flames */
      Yap_Error(OUT_OF_AUXSPACE_ERROR, ARG1, "allocating temp space in atom_codes/2");
      return FALSE;
    }
    t1 = Deref(ARG1);
  }
  goto restart_pred;
}

static Int 
p_atom_length(void)
{
  Term t1 = Deref(ARG1);
  Term t2 = Deref(ARG2);
  Atom at;

  if (IsVarTerm(t1)) {
    Yap_Error(INSTANTIATION_ERROR, t1, "atom_length/2");
    return(FALSE);		
  }
  if (!IsAtomTerm(t1)) {
    Yap_Error(TYPE_ERROR_ATOM, t1, "atom_length/2");
    return(FALSE);
  }
  at = AtomOfTerm(t1);
  if (!IsVarTerm(t2)) {
    size_t len;
    if (!IsIntegerTerm(t2)) {
      Yap_Error(TYPE_ERROR_INTEGER, t2, "atom_length/2");
      return(FALSE);
    }
    if ((len = IntegerOfTerm(t2)) < 0) {
      Yap_Error(DOMAIN_ERROR_NOT_LESS_THAN_ZERO, t2, "atom_length/2");
      return(FALSE);
    }
    if (IsWideAtom(at)) {
      return wcslen((wchar_t *)RepAtom(at)->StrOfAE) == len;
    } else {
      return(strlen(RepAtom(at)->StrOfAE) == len);
    }
  } else {
    Term tj;
    size_t len;

    if (IsWideAtom(at)) {
      len = wcslen((wchar_t *)RepAtom(at)->StrOfAE);
    } else {
      len = strlen(RepAtom(at)->StrOfAE);
    }
    tj = MkIntegerTerm(len);
    return Yap_unify_constant(t2,tj);
  }
}


static int
is_wide(wchar_t *s)
{
  wchar_t ch;

  while ((ch = *s++)) {
    if (ch > MAX_ISO_LATIN1)
      return TRUE;
  }
  return FALSE;
}

/* split an atom into two sub-atoms */
static Int 
p_atom_split(void)
{
  Term t1 = Deref(ARG1);
  Term t2 = Deref(ARG2);
  size_t len;
  int i;
  Term to1, to2;
  Atom at;

  if (IsVarTerm(t1)) {
    Yap_Error(INSTANTIATION_ERROR, t1, "$atom_split/4");
    return(FALSE);		
  }
  if (!IsAtomTerm(t1)) {
    Yap_Error(TYPE_ERROR_ATOM, t1, "$atom_split/4");
    return(FALSE);
  }
  if (IsVarTerm(t2)) {
    Yap_Error(INSTANTIATION_ERROR, t2, "$atom_split/4");
    return(FALSE);		
  }
  if (!IsIntTerm(t2)) {
    Yap_Error(TYPE_ERROR_INTEGER, t2, "$atom_split/4");
    return(FALSE);
  }
  if ((len = IntOfTerm(t2)) < 0) {
    Yap_Error(DOMAIN_ERROR_NOT_LESS_THAN_ZERO, t2, "$atom_split/4");
    return(FALSE);
  }
  at  = AtomOfTerm(t1);
  if (IsWideAtom(at)) {
    wchar_t *ws, *ws1 = (wchar_t *)H;
    char *s1 = (char *)H;
    size_t wlen;

    ws = (wchar_t *)RepAtom(at)->StrOfAE;
    wlen = wcslen(ws);
    if (len > wlen) return FALSE;
    if (s1+len > (char *)LCL0-1024)
      Yap_Error(OUT_OF_STACK_ERROR,t1,"$atom_split/4");
    for (i = 0; i< len; i++) {
      if (ws[i] > MAX_ISO_LATIN1) {
	break;
      }
      s1[i] = ws[i];
    }
    if (ws1[i] > MAX_ISO_LATIN1) {
      /* first sequence is wide */
      if (ws1+len > (wchar_t *)ASP-1024)
	Yap_Error(OUT_OF_STACK_ERROR,t1,"$atom_split/4");
      ws = (wchar_t *)RepAtom(at)->StrOfAE;
      for (i = 0; i< len; i++) {
	ws1[i] = ws[i];
      }
      ws1[len] = '\0';
      to1 = MkAtomTerm(Yap_LookupWideAtom(ws1));
      /* we don't know if the rest of the string is wide or not */
      if (is_wide(ws+len)) {
	to2 = MkAtomTerm(Yap_LookupWideAtom(ws+len)); 
      } else {
	char *s2 = (char *)H;
	if (s2+(wlen-len) > (char *)ASP-1024)
	  Yap_Error(OUT_OF_STACK_ERROR,t1,"$atom_split/4");
	ws += len;
	while ((*s2++ = *ws++));
	to2 = MkAtomTerm(Yap_LookupAtom((char *)H)); 	
      }
    } else {
      s1[len] = '\0';
      to1 = MkAtomTerm(Yap_LookupAtom(s1));
      /* second atom must be wide, if first wasn't */
      to2 = MkAtomTerm(Yap_LookupWideAtom(ws+len)); 
    }
  } else {
    char *s, *s1 = (char *)H;

    s = RepAtom(at)->StrOfAE;
    if (len > (Int)strlen(s)) return(FALSE);
    if (s1+len > (char *)ASP-1024)
      Yap_Error(OUT_OF_STACK_ERROR,t1,"$atom_split/4");
    for (i = 0; i< len; i++) {
      s1[i] = s[i];
    }
    s1[len] = '\0';
    to1 = MkAtomTerm(Yap_LookupAtom(s1));
    to2 = MkAtomTerm(Yap_LookupAtom(s+len)); 
  }
  return(Yap_unify_constant(ARG3,to1) && Yap_unify_constant(ARG4,to2));
}

static Term
gen_syntax_error(Atom InpAtom, char *s)
{
  Term ts[7], ti[2];
  ti[0] = ARG1;
  ti[1] = ARG2;
  ts[0] = Yap_MkApplTerm(Yap_MkFunctor(Yap_LookupAtom(s),2),2,ti);
  ts[1] = ts[4] = ts[5] = MkIntTerm(0);
  ts[2] = MkAtomTerm(AtomExpectedNumber);
  ts[3] = TermNil;
  ts[6] = MkAtomTerm(InpAtom);
  return(Yap_MkApplTerm(FunctorSyntaxError,7,ts));
}

static Term
gen_syntax_type_error(void)
{
  Term ts[7], ti[2];
  ti[0] = ARG1;
  ti[1] = ARG2;
  ts[0] = Yap_MkApplTerm(Yap_MkFunctor(Yap_LookupAtom("number_chars"),2),2,ti);
  ts[1] = ts[4] = ts[5] = MkIntTerm(0);
  ts[2] = MkAtomTerm(AtomExpectedNumber);
  ts[3] = TermNil;
  ts[6] = ARG2;
  return(Yap_MkApplTerm(FunctorSyntaxError,7,ts));
}

static Int 
p_number_chars(void)
{
  char   *String; /* alloc temp space on Trail */
  register Term   t = Deref(ARG2), t1 = Deref(ARG1);
  Term NewT;
  register char  *s;

 restart_aux:
  String = Yap_PreAllocCodeSpace();
  if (String+1024 > (char *)AuxSp) {
    String = Yap_ExpandPreAllocCodeSpace(0, NULL, TRUE);
    if (String + 1024 > (char *)AuxSp) {
      /* crash in flames */
      Yap_Error(OUT_OF_AUXSPACE_ERROR, ARG1, "allocating temp space in number_chars/2");
      return FALSE;
    }
  }
  if (IsNonVarTerm(t1) && IsVarTerm(t)) {
    Term            NewT;
    if (!IsNumTerm(t1)) {
      Yap_Error(TYPE_ERROR_NUMBER, t1, "number_chars/2");
      return(FALSE);
    } else if (IsIntTerm(t1)) {
#if SHORT_INTS
      sprintf(String, "%ld", IntOfTerm(t1));
#else
      sprintf(String, "%d", IntOfTerm(t1));
#endif
    } else if (IsFloatTerm(t1)) {
      sprintf(String, "%f", FloatOfTerm(t1));
    } else if (IsLongIntTerm(t1)) {
#if SHORT_INTS
      sprintf(String, "%ld", LongIntOfTerm(t1));
#else
      sprintf(String, "%d", LongIntOfTerm(t1));
#endif
#if USE_GMP
    } else if (IsBigIntTerm(t1)) {
      mpz_get_str(String, 10, Yap_BigIntOfTerm(t1));
#endif
    }
    if (yap_flags[YAP_TO_CHARS_FLAG] == QUINTUS_TO_CHARS) {
      NewT = Yap_StringToList(String);
    } else {
      NewT = Yap_StringToListOfAtoms(String);
    }
    return Yap_unify(NewT, ARG2);
  }
  if (IsVarTerm(t)) {
    Yap_Error(INSTANTIATION_ERROR, t1, "number_chars/2");
    return(FALSE);		
  }
  if (!IsPairTerm(t) && t != TermNil) {
    Yap_Error(TYPE_ERROR_LIST, t, "number_chars/2");
    return(FALSE);		
  }
  s = String;
  if (yap_flags[YAP_TO_CHARS_FLAG] == QUINTUS_TO_CHARS) {
    while (t != TermNil) {
      register Term   Head;
      register Int    i;
      Head = HeadOfTerm(t);
      if (IsVarTerm(Head)) {
	Yap_Error(SYNTAX_ERROR,gen_syntax_type_error(),"number_chars/2");
	return(FALSE);
      } else if (!IsIntTerm(Head)) {
	Yap_Error(SYNTAX_ERROR,gen_syntax_type_error(),"number_chars/2");
	return(FALSE);		
      }
      i = IntOfTerm(Head);
      if (i < 0 || i > 255) {
	Yap_Error(SYNTAX_ERROR,gen_syntax_type_error(),"number_chars/2");
	return(FALSE);		
      }
      if (s+1024 > (char *)AuxSp) {
	int offs = (s-String);
	String = Yap_ExpandPreAllocCodeSpace(0, NULL, TRUE);
	if (String + (offs+1024) > (char *)AuxSp) {
	  /* crash in flames */
	  Yap_Error(OUT_OF_AUXSPACE_ERROR, ARG1, "allocating temp space in number_chars/2");
	  return FALSE;
	}
	goto restart_aux;
      }
      *s++ = i;
      t = TailOfTerm(t);
      if (IsVarTerm(t)) {
	Yap_Error(SYNTAX_ERROR,gen_syntax_type_error(),"number_chars/2");
	return(FALSE);
      } else if (!IsPairTerm(t) && t != TermNil) {
	Yap_Error(SYNTAX_ERROR,gen_syntax_type_error(),"number_chars/2");
	return(FALSE);
      }
    }
  } else {
    /* ISO code */
    while (t != TermNil) {
      register Term   Head;
      register char   *is;
      
      Head = HeadOfTerm(t);
      if (IsVarTerm(Head)) {
	Yap_Error(SYNTAX_ERROR,gen_syntax_type_error(),"number_chars/2");
	return(FALSE);
      } else if (!IsAtomTerm(Head)) {
	Yap_Error(SYNTAX_ERROR,gen_syntax_type_error(),"number_chars/2");
	return(FALSE);		
      }
      is = RepAtom(AtomOfTerm(Head))->StrOfAE;
      if (is[1] != '\0') {
	Yap_Error(SYNTAX_ERROR,gen_syntax_type_error(),"number_chars/2");
	return(FALSE);		
      }
      if (s+1 == (char *)AuxSp) {
	char *nString;

	*H++ = t;
	nString = Yap_ExpandPreAllocCodeSpace(0, NULL, TRUE);
	t = *--H;
	s = nString+(s-String);
	String = nString;
      }
      *s++ = is[0];
      t = TailOfTerm(t);
      if (IsVarTerm(t)) {
	Yap_Error(SYNTAX_ERROR,gen_syntax_type_error(),"number_chars/2");
	return(FALSE);
      } else if (!IsPairTerm(t) && t != TermNil) {
	Yap_Error(SYNTAX_ERROR,gen_syntax_type_error(),"number_chars/2");
	return(FALSE);
      }
    }
  }
  *s++ = '\0';
  if ((NewT = get_num(String)) == TermNil) {
    Yap_Error(SYNTAX_ERROR, gen_syntax_error(Yap_LookupAtom(String), "number_chars"), "while scanning %s", String);
    return (FALSE);
  }
  return (Yap_unify(ARG1, NewT));
}

static Int 
p_number_atom(void)
{
  char   *String; /* alloc temp space on Trail */
  register Term   t = Deref(ARG2), t1 = Deref(ARG1);
  Term NewT;
  char  *s;

  s = String = ((AtomEntry *)Yap_PreAllocCodeSpace())->StrOfAE;
  if (String+1024 > (char *)AuxSp) {
    s = String = Yap_ExpandPreAllocCodeSpace(0, NULL, TRUE);
    if (String + 1024 > (char *)AuxSp) {
      /* crash in flames */
      Yap_Error(OUT_OF_AUXSPACE_ERROR, ARG1, "allocating temp space in number_atom/2");
      return FALSE;
    }
  }
  if (IsNonVarTerm(t1)) {
    Atom at;

    if (IsIntTerm(t1)) {

#if SHORT_INTS
      sprintf(String, "%ld", IntOfTerm(t1));
#else
      sprintf(String, "%d", IntOfTerm(t1));
#endif
    } else if (IsFloatTerm(t1)) {
      sprintf(String, "%f", FloatOfTerm(t1));
    } else if (IsLongIntTerm(t1)) {

#if SHORT_INTS
      sprintf(String, "%ld", LongIntOfTerm(t1));
#else
      sprintf(String, "%d", LongIntOfTerm(t1));
#endif

#if USE_GMP
    } else if (IsBigIntTerm(t1)) {
      mpz_get_str(String, 10, Yap_BigIntOfTerm(t1));
#endif
    } else {
      Yap_Error(TYPE_ERROR_NUMBER, t1, "number_atom/2");
      return FALSE;
    }
    while ((at = Yap_LookupAtom(String)) == NIL) {
      if (!Yap_growheap(FALSE, 0, NULL)) {
	Yap_Error(OUT_OF_HEAP_ERROR, TermNil, Yap_ErrorMessage);
	return FALSE;
      }
    }
    return Yap_unify(MkAtomTerm(at), ARG2);
  }
  if (IsVarTerm(t)) {
      Yap_Error(INSTANTIATION_ERROR, t, "number_chars/2");
      return(FALSE);		
  }
  if (!IsAtomTerm(t)) {
    Yap_Error(TYPE_ERROR_LIST, t, "number_atom/2");
    return(FALSE);		
  }
  s = RepAtom(AtomOfTerm(t))->StrOfAE;
  if ((NewT = get_num(s)) == TermNil) {
    Yap_Error(SYNTAX_ERROR, gen_syntax_error(Yap_LookupAtom(String), "number_atom"), "while scanning %s", s);
    return (FALSE);
  }
  return (Yap_unify(ARG1, NewT));
}

static Int 
p_number_codes(void)
{
  char   *String; /* alloc temp space on Trail */
  register Term   t = Deref(ARG2), t1 = Deref(ARG1);
  Term NewT;
  register char  *s;

  String = Yap_PreAllocCodeSpace();
  if (String+1024 > (char *)AuxSp) {
    s = String = Yap_ExpandPreAllocCodeSpace(0, NULL, TRUE);
    if (String + 1024 > (char *)AuxSp) {
      /* crash in flames */
      Yap_Error(OUT_OF_AUXSPACE_ERROR, ARG1, "allocating temp space in number_codes/2");
      return FALSE;
    }
  }
  if (IsNonVarTerm(t1) && IsVarTerm(t)) {
    if (IsIntTerm(t1)) {
#if SHORT_INTS
      sprintf(String, "%ld", IntOfTerm(t1));
#else
      sprintf(String, "%d", IntOfTerm(t1));
#endif
    } else if (IsFloatTerm(t1)) {
      sprintf(String, "%f", FloatOfTerm(t1));
    } else if (IsLongIntTerm(t1)) {
#if SHORT_INTS
      sprintf(String, "%ld", LongIntOfTerm(t1));
#else
      sprintf(String, "%d", LongIntOfTerm(t1));
#endif
#if USE_GMP
    } else if (IsBigIntTerm(t1)) {
      mpz_get_str(String, 10, Yap_BigIntOfTerm(t1));
#endif
    } else {
      Yap_Error(TYPE_ERROR_NUMBER, t1, "number_codes/2");
      return FALSE;
    }
    NewT = Yap_StringToList(String);
    return Yap_unify(NewT, ARG2);
  }
  if (IsVarTerm(t)) {
    Yap_Error(INSTANTIATION_ERROR, t, "number_codes/2");
  }
  if (!IsPairTerm(t) && t != TermNil) {
    Yap_Error(TYPE_ERROR_LIST, t, "number_codes/2");
    return(FALSE);		
  }
  s = String; /* alloc temp space on Trail */
  while (t != TermNil) {
    register Term   Head;
    register Int    i;

    Head = HeadOfTerm(t);
    if (IsVarTerm(Head)) {
      Yap_Error(INSTANTIATION_ERROR,Head,"number_codes/2");
      return(FALSE);
    } else if (!IsIntTerm(Head)) {
      Yap_Error(REPRESENTATION_ERROR_CHARACTER_CODE,Head,"number_codes/2");
      return(FALSE);		
    }
    i = IntOfTerm(Head);
    if (i < 0 || i > 255) {
      Yap_Error(REPRESENTATION_ERROR_CHARACTER_CODE,Head,"number_codes/2");
      return(FALSE);		
    }
    if (s+1 == (char *)AuxSp) {
      char *nString;

      *H++ = t;
      nString = Yap_ExpandPreAllocCodeSpace(0, NULL, TRUE);
      t = *--H;
      s = nString+(s-String);
      String = nString;
    }
    *s++ = i;
    t = TailOfTerm(t);
    if (IsVarTerm(t)) {
      Yap_Error(INSTANTIATION_ERROR,t,"number_codes/2");
      return(FALSE);
    } else if (!IsPairTerm(t) && t != TermNil) {
      Yap_Error(TYPE_ERROR_LIST, t, "number_codes/2");
      return(FALSE);
    }
  }
  *s++ = '\0';
  if ((NewT = get_num(String)) == TermNil) {
    Yap_Error(SYNTAX_ERROR, gen_syntax_error(Yap_LookupAtom(String), "number_codes"), "while scanning %s", String);
    return (FALSE);
  }
  return (Yap_unify(ARG1, NewT));
}

static Int 
p_atom_number(void)
{
  Term   t = Deref(ARG1), t2 = Deref(ARG2);
  Term NewT;

  if (IsVarTerm(t)) {
    char   *String; /* alloc temp space on Trail */
    char  *s;
    if (IsVarTerm(t2)) {
      Yap_Error(INSTANTIATION_ERROR, t2, "atom_number/2");
      return FALSE;
    }
    String = Yap_PreAllocCodeSpace();
    if (String+1024 > (char *)AuxSp) {
      s = String = Yap_ExpandPreAllocCodeSpace(0, NULL, TRUE);
      if (String + 1024 > (char *)AuxSp) {
	/* crash in flames */
	Yap_Error(OUT_OF_AUXSPACE_ERROR, ARG1, "allocating temp space in number_codes/2");
	return FALSE;
      }
    }
    if (IsIntTerm(t2)) {
#if SHORT_INTS
      sprintf(String, "%ld", IntOfTerm(t2));
#else
      sprintf(String, "%d", IntOfTerm(t2));
#endif
    } else if (IsFloatTerm(t2)) {
      sprintf(String, "%g", FloatOfTerm(t2));
    } else if (IsLongIntTerm(t2)) {
#if SHORT_INTS
      sprintf(String, "%ld", LongIntOfTerm(t2));
#else
      sprintf(String, "%d", LongIntOfTerm(t2));
#endif
#if USE_GMP
    } else if (IsBigIntTerm(t2)) {
      mpz_get_str(String, 10, Yap_BigIntOfTerm(t2));
#endif
    } else {
      Yap_Error(TYPE_ERROR_NUMBER, t2, "atom_number/2");
      return FALSE;
    }
    NewT = MkAtomTerm(Yap_LookupAtom(String));
    return Yap_unify(NewT, ARG1);
  } else {
    Atom at;
    char *s;
    
    if (!IsAtomTerm(t)) {
      Yap_Error(TYPE_ERROR_ATOM, t, "atom_number/2");
      return FALSE;
    }
    at = AtomOfTerm(t);
    if (IsWideAtom(at)) {
      Yap_Error(SYNTAX_ERROR, gen_syntax_error(at, "number_codes"), "while scanning %S", RepAtom(at)->WStrOfAE);
      return FALSE;
    }
    s = RepAtom(at)->StrOfAE; /* alloc temp space on Trail */
    if ((NewT = get_num(s)) == TermNil) {
      Yap_Error(SYNTAX_ERROR, gen_syntax_error(at, "atom_number"), "while scanning %s", s);
      return FALSE;
    }
    return Yap_unify(ARG2, NewT);
  }
}

static Int 
p_univ(void)
{				/* A =.. L			 */
  unsigned int    arity;
  register Term   tin;
  Term            twork, t2;
  Atom            at;

  tin = Deref(ARG1);
  t2 = Deref(ARG2);
  if (IsVarTerm(tin)) {
    /* we need to have a list */
    Term           *Ar;
    if (IsVarTerm(t2)) {
      Yap_Error(INSTANTIATION_ERROR, t2, "(=..)/2");
      return(FALSE);
    }
    if (!IsPairTerm(t2)) {
      if (t2 == TermNil)
	Yap_Error(DOMAIN_ERROR_NON_EMPTY_LIST, t2, "(=..)/2");
      else
	Yap_Error(TYPE_ERROR_LIST, ARG2, "(=..)/2");
      return (FALSE);
    }
    twork = HeadOfTerm(t2);
    if (IsVarTerm(twork)) {
      Yap_Error(INSTANTIATION_ERROR, twork, "(=..)/2");
      return(FALSE);
    }
    if (IsNumTerm(twork)) {
      Term tt = TailOfTerm(t2);
      if (IsVarTerm(tt) || tt != MkAtomTerm(AtomNil)) {
	Yap_Error(TYPE_ERROR_ATOM, twork, "(=..)/2");
	return (FALSE);
      }
      return (Yap_unify_constant(ARG1, twork));
    }
    if (!IsAtomTerm(twork)) {
      Yap_Error(TYPE_ERROR_ATOM, twork, "(=..)/2");
      return (FALSE);
    }      
    at = AtomOfTerm(twork);
    twork = TailOfTerm(t2);
    if (IsVarTerm(twork)) {
      Yap_Error(INSTANTIATION_ERROR, twork, "(=..)/2");
      return(FALSE);
    } else if (!IsPairTerm(twork)) {
      if (twork != TermNil) {
	Yap_Error(TYPE_ERROR_LIST, ARG2, "(=..)/2");
	return(FALSE);
      }
      return (Yap_unify_constant(ARG1, MkAtomTerm(at)));
    }
  build_compound:
    /* build the term directly on the heap */
    Ar = H;
    H++;
    
    while (!IsVarTerm(twork) && IsPairTerm(twork)) {
      *H++ = HeadOfTerm(twork);
      if (H > ASP - 1024) {
	/* restore space */
	H = Ar;
	if (!Yap_gcl((ASP-H)*sizeof(CELL), 2, ENV, gc_P(P,CP))) {
	  Yap_Error(OUT_OF_STACK_ERROR, TermNil, Yap_ErrorMessage);
	  return FALSE;
	}
	twork = TailOfTerm(Deref(ARG2));
	goto build_compound;
      }
      twork = TailOfTerm(twork);
    }
    if (IsVarTerm(twork)) {
      Yap_Error(INSTANTIATION_ERROR, twork, "(=..)/2");
      return(FALSE);
    }
    if (twork != TermNil) {
      Yap_Error(TYPE_ERROR_LIST, ARG2, "(=..)/2");
      return (FALSE);
    }
#ifdef SFUNC
    DOES_NOT_WORK();
    {
      SFEntry        *pe = (SFEntry *) Yap_GetAProp(at, SFProperty);
      if (pe)
	twork = MkSFTerm(Yap_MkFunctor(at, SFArity),
			 arity, CellPtr(TR), pe->NilValue);
      else
	twork = Yap_MkApplTerm(Yap_MkFunctor(at, arity),
			   arity, CellPtr(TR));
    }
#else
    arity = H-Ar-1;
    if (at == AtomDot && arity == 2) {
      Ar[0] = Ar[1];
      Ar[1] = Ar[2];
      H --;
      twork = AbsPair(Ar);
    } else {      
      *Ar = (CELL)(Yap_MkFunctor(at, arity));
      twork = AbsAppl(Ar);
    }
#endif
    return (Yap_unify(ARG1, twork));
  }
  if (IsAtomicTerm(tin)) {
    twork = MkPairTerm(tin, MkAtomTerm(AtomNil));
    return (Yap_unify(twork, ARG2));
  }
  if (IsRefTerm(tin))
    return (FALSE);
  if (IsApplTerm(tin)) {
    Functor         fun = FunctorOfTerm(tin);
    arity = ArityOfFunctor(fun);
    at = NameOfFunctor(fun);
#ifdef SFUNC
    if (arity == SFArity) {
      CELL           *p = CellPtr(TR);
      CELL           *q = ArgsOfSFTerm(tin);
      int             argno = 1;
      while (*q) {
	while (*q > argno++)
	  *p++ = MkVarTerm();
	++q;
	*p++ = Deref(*q++);
      }
      twork = Yap_ArrayToList(CellPtr(TR), argno - 1);
      while (IsIntTerm(twork)) {
	if (!Yap_gc(2, ENV, gc_P(P,CP))) {
	  Yap_Error(OUT_OF_STACK_ERROR, TermNil, Yap_ErrorMessage);
	  return(FALSE);
	}    
	twork = Yap_ArrayToList(CellPtr(TR), argno - 1);
      }
    } else
#endif
      {
	while (H+arity*2 > ASP-1024) {
	  if (!Yap_gcl((arity*2)*sizeof(CELL), 2, ENV, gc_P(P,CP))) {
	    Yap_Error(OUT_OF_STACK_ERROR, TermNil, Yap_ErrorMessage);
	    return(FALSE);
	  }
	  tin = Deref(ARG1);
	}
	twork = Yap_ArrayToList(RepAppl(tin) + 1, arity);
      }
  } else {
    /* We found a list */
    at = AtomDot;
    twork = Yap_ArrayToList(RepPair(tin), 2);
  }
  twork = MkPairTerm(MkAtomTerm(at), twork);
  return (Yap_unify(ARG2, twork));
}

/* $sub_atom_extract(A,Bef,Size,After,SubAt).*/
static Int
p_sub_atom_extract(void)
{
  Atom at = AtomOfTerm(Deref(ARG1)), nat;
  Int start = IntegerOfTerm(Deref(ARG2));
  Int len = IntegerOfTerm(Deref(ARG3));
  Int leftover;

  if (start < 0)
    Yap_Error(DOMAIN_ERROR_NOT_LESS_THAN_ZERO,ARG2,"sub_atom/5");
  if (len < 0)
    Yap_Error(DOMAIN_ERROR_NOT_LESS_THAN_ZERO,ARG3,"sub_atom/5");
 start:
  if (IsWideAtom(at)) {
    wchar_t *s = RepAtom(at)->WStrOfAE;
    int max = wcslen(s);
    Int i;

    leftover = max-(start+len);
    if (leftover < 0)
      return FALSE;
    for (i=0;i<len;i++) {
      if ((s+start)[i] > 255) break;
    }
    if (i == len) {
      char *String = Yap_PreAllocCodeSpace();
      if (String + (len+1024) >= (char *)AuxSp) 
	goto expand_auxsp;
      for (i=0;i<len;i++) {
	String[i] = (s+start)[i];
      }
      String[len] = '\0';
      nat = Yap_LookupAtom(String);    
    } else {
      wchar_t *String = (wchar_t *)Yap_PreAllocCodeSpace();
      if (String + (len+1024) >= (wchar_t *)AuxSp) 
	goto expand_auxsp;
      wcsncpy(String, s+start, len);
      String[len] = '\0';
      nat = Yap_LookupWideAtom(String);    
    }
  } else {
    char *s = RepAtom(at)->StrOfAE, *String;
    int max = strlen(s);

    leftover = max-(start+len);
    if (leftover < 0)
      return FALSE;
    String = Yap_PreAllocCodeSpace();
    if (String + (len+1024) >= (char *)AuxSp) 
      goto expand_auxsp;
    strncpy(String, s+start, len);
    String[len] = '\0';
    nat = Yap_LookupAtom(String);
  }
  return Yap_unify(ARG5,MkAtomTerm(nat)) &&
    Yap_unify(ARG4,MkIntegerTerm(leftover));

 expand_auxsp:
  {
    char *String = Yap_ExpandPreAllocCodeSpace(len, NULL, TRUE);
    if (String + 1024 > (char *)AuxSp) {
      /* crash in flames */
      Yap_Error(OUT_OF_AUXSPACE_ERROR, ARG1, "allocating temp space in sub_atom/5");
      return FALSE;
    }
  }
  goto start;
}

/* $sub_atom_extract(A,Bef,Size,After,SubAt).*/
static Int
cont_sub_atom_fetch(void)
{
  Atom at = AtomOfTerm(EXTRA_CBACK_ARG(5,1));
  Atom subatom = AtomOfTerm(EXTRA_CBACK_ARG(5,2));
  Int offset = IntegerOfTerm(EXTRA_CBACK_ARG(5,3));
  Int sb = IntegerOfTerm(EXTRA_CBACK_ARG(5,4));
  Int sz = IntegerOfTerm(EXTRA_CBACK_ARG(5,5));

  if (IsWideAtom(at)) {
    wchar_t *s = RepAtom(at)->WStrOfAE;
    wchar_t *ins, *where;
    Int start, after;
    Int res;

    
    if (!IsWideAtom(subatom)) {
      /* first convert to wchar_t */
      char *inschars = RepAtom(subatom)->StrOfAE;
      Int i;

      if (offset+sz > sb)
	cut_fail();
      ins = (wchar_t *)Yap_PreAllocCodeSpace();
      while ((ins = (wchar_t *)Yap_PreAllocCodeSpace()) + (sz+1) > (wchar_t *)AuxSp) {
	if (!Yap_ExpandPreAllocCodeSpace(sizeof(wchar_t)*(sz+1), NULL, TRUE)) {
	  Yap_Error(OUT_OF_AUXSPACE_ERROR, ARG5, "allocating temp space in sub_atom/2");
	  return FALSE;
	}
      }
      for (i=0;i<=sz;i++)
	ins[i] = inschars[i];
    } else {
      ins = RepAtom(subatom)->WStrOfAE;
    }
    if (!(where = wcsstr(s+offset, ins))) {
      cut_fail();
    }
    if (!Yap_unify(EXTRA_CBACK_ARG(5,5), ARG3)) {
      cut_fail();
    }
    start = where-s;
    after = sb-(start+sz);
    res = (Yap_unify(MkIntegerTerm(start), ARG2) &&
	   Yap_unify(MkIntegerTerm(after), ARG4));
    if (!res) {
      if (!after) {
	cut_fail();
      } else {
	EXTRA_CBACK_ARG(5,3) = MkIntegerTerm(start+1);
	return FALSE;
      }
    }
    if (!after) {
      cut_succeed();
    } else {
      EXTRA_CBACK_ARG(5,3) = MkIntegerTerm(start+1);
      return TRUE;
    }
  } else {
    char *s = RepAtom(at)->StrOfAE;
    char *ins, *where;
    Int start, after;
    Int res;

    ins = subatom->StrOfAE;
    if (offset+sz > sb) {
      cut_fail();
    }
    if (!(where = strstr(s+offset, ins))) {
      cut_fail();
    }
    if (!Yap_unify(EXTRA_CBACK_ARG(5,5), ARG3)) {
      cut_fail();
    }
    start = where-s;
    after = sb-(start+sz);
    res = (Yap_unify(MkIntegerTerm(start), ARG2) &&
	   Yap_unify(MkIntegerTerm(after), ARG4));
    if (!res) {
      if (!after) {
	cut_fail();
      } else {
	EXTRA_CBACK_ARG(5,3) = MkIntegerTerm(start+1);
	return FALSE;
      }
    }
    if (!after) {
      cut_succeed();
    } else {
      EXTRA_CBACK_ARG(5,3) = MkIntegerTerm(start+1);
      return TRUE;
    }
  }
}

/* $sub_atom_extract(A,Bef,Size,After,SubAt).*/
static Int
init_sub_atom_fetch(void)
{
  Term tat1, tat2;
  Atom at1, at2;

  EXTRA_CBACK_ARG(5,1) = tat1 = Deref(ARG1);  
  EXTRA_CBACK_ARG(5,2) = tat2 = Deref(ARG5);  
  EXTRA_CBACK_ARG(5,3) = MkIntegerTerm(0);
  at1 = AtomOfTerm(tat1);
  at2 = AtomOfTerm(tat2);
  if (IsWideAtom(at1)) {
    EXTRA_CBACK_ARG(5,4) = MkIntegerTerm(wcslen(at1->WStrOfAE));
    if (IsWideAtom(at2)) {
      EXTRA_CBACK_ARG(5,5) = MkIntegerTerm(wcslen(at2->WStrOfAE));
    } else {
      EXTRA_CBACK_ARG(5,5) = MkIntegerTerm(strlen(at2->StrOfAE));
    }
  } else {
    EXTRA_CBACK_ARG(5,4) = MkIntegerTerm(strlen(at1->StrOfAE));
    if (IsWideAtom(at2)) {
      cut_fail();
    } else {
      EXTRA_CBACK_ARG(5,5) = MkIntegerTerm(strlen(at2->StrOfAE));
    }
  }
  return cont_sub_atom_fetch();
}


static Int 
p_abort(void)
{				/* abort			 */
  /* make sure we won't go creeping around */
  Yap_Error(PURE_ABORT, TermNil, "");
  return(FALSE);
}

#ifdef BEAM
extern void exit_eam(char *s); 

Int 
#else
static Int 
#endif
p_halt(void)
{				/* halt				 */
  Term t = Deref(ARG1);
  Int out;

#ifdef BEAM
  if (EAM) exit_eam("\n\n[ Prolog execution halted ]\n");
#endif

  if (IsVarTerm(t)) {
    Yap_Error(INSTANTIATION_ERROR,t,"halt/1");
    return(FALSE);
  }
  if (!IsIntegerTerm(t)) {
    Yap_Error(TYPE_ERROR_INTEGER,t,"halt/1");
    return(FALSE);
  }
  out = IntegerOfTerm(t);
  Yap_exit(out);
  return TRUE;
}


static Int 
cont_current_atom(void)
{
  Atom            catom;
  Int             i = IntOfTerm(EXTRA_CBACK_ARG(1,2));
  AtomEntry       *ap; /* nasty hack for gcc on hpux */

  /* protect current hash table line */
  if (IsAtomTerm(EXTRA_CBACK_ARG(1,1)))
    catom = AtomOfTerm(EXTRA_CBACK_ARG(1,1));
  else
    catom = NIL;
  if (catom == NIL){
    i++;
    /* move away from current hash table line */
    while (i < AtomHashTableSize) {
      READ_LOCK(HashChain[i].AERWLock);
      catom = HashChain[i].Entry;
      READ_UNLOCK(HashChain[i].AERWLock);
      if (catom != NIL) {
	break;
      }
      i++;
    }
    if (i == AtomHashTableSize) {
      cut_fail();
    }
  }
  ap = RepAtom(catom);
  if (Yap_unify_constant(ARG1, MkAtomTerm(catom))) {
    READ_LOCK(ap->ARWLock);
    if (ap->NextOfAE == NIL) {
      READ_UNLOCK(ap->ARWLock);
      i++;
      while (i < AtomHashTableSize) {
	READ_LOCK(HashChain[i].AERWLock);
	catom = HashChain[i].Entry;
	READ_UNLOCK(HashChain[i].AERWLock);
	if (catom != NIL) {
	  break;
	}
	i++;
      }
      if (i == AtomHashTableSize) {
	cut_fail();
      } else {
	EXTRA_CBACK_ARG(1,1) = MkAtomTerm(catom);
      }
    } else {
      EXTRA_CBACK_ARG(1,1) = MkAtomTerm(ap->NextOfAE);
      READ_UNLOCK(ap->ARWLock);
    }
    EXTRA_CBACK_ARG(1,2) = MkIntTerm(i);
    return TRUE;
  } else {
    return FALSE;
  }
}

static Int 
init_current_atom(void)
{				/* current_atom(?Atom)		 */
  Term t1 = Deref(ARG1);
  if (!IsVarTerm(t1)) {
    if (IsAtomTerm(t1))
      cut_succeed();
    else
      cut_fail();
  }
  READ_LOCK(HashChain[0].AERWLock);
  if (HashChain[0].Entry != NIL) {
    EXTRA_CBACK_ARG(1,1) = MkAtomTerm(HashChain[0].Entry);
  } else {
    EXTRA_CBACK_ARG(1,1) = MkIntTerm(0);
  }
  READ_UNLOCK(HashChain[0].AERWLock);
  EXTRA_CBACK_ARG(1,2) = MkIntTerm(0);
  return (cont_current_atom());
}


static Int 
cont_current_wide_atom(void)
{
  Atom            catom;
  Int             i = IntOfTerm(EXTRA_CBACK_ARG(1,2));
  AtomEntry       *ap; /* nasty hack for gcc on hpux */

  /* protect current hash table line */
  if (IsAtomTerm(EXTRA_CBACK_ARG(1,1)))
    catom = AtomOfTerm(EXTRA_CBACK_ARG(1,1));
  else
    catom = NIL;
  if (catom == NIL){
    i++;
    /* move away from current hash table line */
    while (i < WideAtomHashTableSize) {
      READ_LOCK(WideHashChain[i].AERWLock);
      catom = WideHashChain[i].Entry;
      READ_UNLOCK(WideHashChain[i].AERWLock);
      if (catom != NIL) {
	break;
      }
      i++;
    }
    if (i == WideAtomHashTableSize) {
      cut_fail();
    }
  }
  ap = RepAtom(catom);
  if (Yap_unify_constant(ARG1, MkAtomTerm(catom))) {
    READ_LOCK(ap->ARWLock);
    if (ap->NextOfAE == NIL) {
      READ_UNLOCK(ap->ARWLock);
      i++;
      while (i < WideAtomHashTableSize) {
	READ_LOCK(WideHashChain[i].AERWLock);
	catom = WideHashChain[i].Entry;
	READ_UNLOCK(WideHashChain[i].AERWLock);
	if (catom != NIL) {
	  break;
	}
	i++;
      }
      if (i == WideAtomHashTableSize) {
	cut_fail();
      } else {
	EXTRA_CBACK_ARG(1,1) = MkAtomTerm(catom);
      }
    } else {
      EXTRA_CBACK_ARG(1,1) = MkAtomTerm(ap->NextOfAE);
      READ_UNLOCK(ap->ARWLock);
    }
    EXTRA_CBACK_ARG(1,2) = MkIntTerm(i);
    return TRUE;
  } else {
    return FALSE;
  }
}

static Int 
init_current_wide_atom(void)
{				/* current_atom(?Atom)		 */
  Term t1 = Deref(ARG1);
  if (!IsVarTerm(t1)) {
    if (IsAtomTerm(t1))
      cut_succeed();
    else
      cut_fail();
  }
  READ_LOCK(WideHashChain[0].AERWLock);
  if (WideHashChain[0].Entry != NIL) {
    EXTRA_CBACK_ARG(1,1) = MkAtomTerm(WideHashChain[0].Entry);
  } else {
    EXTRA_CBACK_ARG(1,1) = MkIntTerm(0);
  }
  READ_UNLOCK(WideHashChain[0].AERWLock);
  EXTRA_CBACK_ARG(1,2) = MkIntTerm(0);
  return (cont_current_wide_atom());
}

static Int 
cont_current_predicate(void)
{
  PredEntry      *pp = (PredEntry *)IntegerOfTerm(EXTRA_CBACK_ARG(3,1));
  UInt Arity;
  Term name;

  while (pp != NULL) {
    if (pp->PredFlags & HiddenPredFlag) {
      pp = pp->NextPredOfModule;
    } else
      break;
  }
  if (pp == NULL)
    cut_fail();
  EXTRA_CBACK_ARG(3,1) = (CELL)MkIntegerTerm((Int)(pp->NextPredOfModule));
  if (pp->FunctorOfPred == FunctorModule)
    return FALSE;
  if (pp->ModuleOfPred != IDB_MODULE) {
    Arity = pp->ArityOfPE;
    if (Arity)
      name = MkAtomTerm(NameOfFunctor(pp->FunctorOfPred));
    else
      name = MkAtomTerm((Atom)pp->FunctorOfPred);
  } else {
    if (pp->PredFlags & NumberDBPredFlag) {
      name = MkIntegerTerm(pp->src.IndxId);
      Arity = 0;
    } else if (pp->PredFlags & AtomDBPredFlag) {
      name = MkAtomTerm((Atom)pp->FunctorOfPred);
      Arity = 0;
    } else {
      Functor f = pp->FunctorOfPred;
      name = MkAtomTerm(NameOfFunctor(f));
      Arity = ArityOfFunctor(f);
    }
  }
  return
    Yap_unify(ARG2,name) &&
    Yap_unify(ARG3, MkIntegerTerm((Int)Arity));
}

static Int 
init_current_predicate(void)
{
  Term t1 = Deref(ARG1);

  if (IsVarTerm(t1) || !IsAtomTerm(t1)) cut_fail();
  EXTRA_CBACK_ARG(3,1) = MkIntegerTerm((Int)Yap_ModulePred(t1));
  return cont_current_predicate();
}

static Int 
cont_current_predicate_for_atom(void)
{
  Prop pf = (Prop)IntegerOfTerm(EXTRA_CBACK_ARG(3,1));
  Term mod = Deref(ARG2);

  while (pf != NIL) {
    FunctorEntry *pp = RepFunctorProp(pf);
    if (IsFunctorProperty(pp->KindOfPE)) {
      Prop p0 = pp->PropsOfFE;
      while (p0) {
	PredEntry *p = RepPredProp(p0);
	if (p->ModuleOfPred == mod ||
	    p->ModuleOfPred == 0) {
	  /* we found the predicate */
	  EXTRA_CBACK_ARG(3,1) = MkIntegerTerm((Int)(pp->NextOfPE));
	  return 
	    Yap_unify(ARG3,Yap_MkNewApplTerm(p->FunctorOfPred,p->ArityOfPE));
	}
	p0 = p->NextOfPE;
      }
    } else if (pp->KindOfPE == PEProp) {
      PredEntry *pe = RepPredProp(pf);
      if (pe->ModuleOfPred == mod ||
	  pe->ModuleOfPred == 0) {
	/* we found the predicate */
	EXTRA_CBACK_ARG(3,1) = MkIntegerTerm((Int)(pp->NextOfPE));
	return Yap_unify(ARG3,MkAtomTerm((Atom)(pe->FunctorOfPred)));
      }
    }
    pf = pp->NextOfPE;
  }
  cut_fail();
}

static Int 
init_current_predicate_for_atom(void)
{
  Term t1 = Deref(ARG1);

  if (IsVarTerm(t1) || !IsAtomTerm(t1)) cut_fail();
  EXTRA_CBACK_ARG(3,1) = MkIntegerTerm((Int)RepAtom(AtomOfTerm(t1))->PropsOfAE);
  return (cont_current_predicate_for_atom());
}

static OpEntry *
NextOp(OpEntry *pp)
{
  while (!EndOfPAEntr(pp) && pp->KindOfPE != OpProperty &&
	 (pp->OpModule != PROLOG_MODULE || pp->OpModule != CurrentModule))
    pp = RepOpProp(pp->NextOfPE);
  return (pp);
}

int
Yap_IsOp(Atom at)
{
  OpEntry *op = NextOp(RepOpProp((Prop)(RepAtom(at)->PropsOfAE)));
  return (!EndOfPAEntr(op));
}

int
Yap_IsOpMaxPrio(Atom at)
{
  OpEntry *op = NextOp(RepOpProp((Prop)(RepAtom(at)->PropsOfAE)));
  int max;

  if (EndOfPAEntr(op))
    return 0;
  max = (op->Prefix & 0xfff);
  if ((op->Infix & 0xfff) > max)
    max = op->Infix & 0xfff;
  if ((op->Posfix & 0xfff) > max)
    max = op->Posfix & 0xfff;
  return max;
}

static Int
unify_op(OpEntry         *op)
{
  Term tmod = op->OpModule;

  if (tmod == PROLOG_MODULE)
    tmod = TermProlog;
  return 
    Yap_unify_constant(ARG2,tmod) &&
    Yap_unify_constant(ARG3,MkIntegerTerm(op->Prefix)) &&
    Yap_unify_constant(ARG4,MkIntegerTerm(op->Infix)) &&
    Yap_unify_constant(ARG5,MkIntegerTerm(op->Posfix));
}

static Int 
cont_current_op(void)
{
  OpEntry         *op = (OpEntry *)IntegerOfTerm(EXTRA_CBACK_ARG(5,1)), *next;
  
  READ_LOCK(op->OpRWLock);
  next = op->OpNext;
  if (Yap_unify_constant(ARG1,MkAtomTerm(op->OpName)) &&
      unify_op(op)) {
    READ_UNLOCK(op->OpRWLock);
    if (next) {
      EXTRA_CBACK_ARG(5,1) = (CELL) MkIntegerTerm((CELL)next);
      return TRUE;
    } else {
      cut_succeed();
    }
  } else {
    READ_UNLOCK(op->OpRWLock);
    if (next) {
      EXTRA_CBACK_ARG(5,1) = (CELL) MkIntegerTerm((CELL)next);
      return FALSE;
    } else {
      cut_fail();
    }
  }
}

static Int 
init_current_op(void)
{				/* current_op(-Precedence,-Type,-Atom)		 */
  EXTRA_CBACK_ARG(5,1) = (CELL) MkIntegerTerm((CELL)OpList);
  return cont_current_op();
}

static Int 
cont_current_atom_op(void)
{
  OpEntry         *op = (OpEntry *)IntegerOfTerm(EXTRA_CBACK_ARG(5,1)), *next;
  
  READ_LOCK(op->OpRWLock);
  next = NextOp(RepOpProp(op->NextOfPE));
  if (unify_op(op)) {
    READ_UNLOCK(op->OpRWLock);
    if (next) {
      EXTRA_CBACK_ARG(5,1) = (CELL) MkIntegerTerm((CELL)next);
      return TRUE;
    } else {
      cut_succeed();
    }
  } else {
    READ_UNLOCK(op->OpRWLock);
    if (next) {
      EXTRA_CBACK_ARG(5,1) = (CELL) MkIntegerTerm((CELL)next);
      return FALSE;
    } else {
      cut_fail();
    }
  }
}

static Int 
init_current_atom_op(void)
{				/* current_op(-Precedence,-Type,-Atom)		 */
  Term t = Deref(ARG1);
  AtomEntry *ae;
  OpEntry *ope;

  if (IsVarTerm(t) || !IsAtomTerm(t)) {
    Yap_Error(TYPE_ERROR_ATOM,t,"current_op/3");
    cut_fail();
  }
  ae = RepAtom(AtomOfTerm(t));
  if (EndOfPAEntr((ope = NextOp(RepOpProp(ae->PropsOfAE))))) {
    cut_fail();
  }
  EXTRA_CBACK_ARG(5,1) = (CELL) MkIntegerTerm((Int)ope);
  return cont_current_atom_op();
}

#ifdef DEBUG
static Int 
p_debug()
{				/* $debug(+Flag) */
  int             i = IntOfTerm(Deref(ARG1));

  if (i >= 'a' && i <= 'z')
    Yap_Option[i - 96] = !Yap_Option[i - 96];
  return (1);
}
#endif

static Int 
p_flags(void)
{				/* $flags(+Functor,+Mod,?OldFlags,?NewFlags) */
  PredEntry      *pe;
  Int             newFl;
  Term t1 = Deref(ARG1);
  Term mod = Deref(ARG2);

  if (IsVarTerm(mod) || !IsAtomTerm(mod)) {
    return(FALSE);
  }
  if (IsVarTerm(t1))
    return (FALSE);
  if (IsAtomTerm(t1)) {
    while ((pe = RepPredProp(PredPropByAtom(AtomOfTerm(t1), mod)))== NULL) {
      if (!Yap_growheap(FALSE, 0, NULL)) {
	Yap_Error(OUT_OF_HEAP_ERROR, ARG1, "while generating new predicate");
	return FALSE;
      }
      t1 = Deref(ARG1);
      mod = Deref(ARG2);
    }
  } else if (IsApplTerm(t1)) {
    Functor         funt = FunctorOfTerm(t1);
    while ((pe = RepPredProp(PredPropByFunc(funt, mod)))== NULL) {
      if (!Yap_growheap(FALSE, 0, NULL)) {
	Yap_Error(OUT_OF_HEAP_ERROR, ARG1, "while generating new predicate");
	return FALSE;
      }
      t1 = Deref(ARG1);
      mod = Deref(ARG2);
    }
  } else
    return (FALSE);
  if (EndOfPAEntr(pe))
    return (FALSE);
  LOCK(pe->PELock);
  if (!Yap_unify_constant(ARG3, MkIntegerTerm(pe->PredFlags))) {
    UNLOCK(pe->PELock);
    return(FALSE);
  }
  ARG4 = Deref(ARG4);
  if (IsVarTerm(ARG4)) {
    UNLOCK(pe->PELock);
    return (TRUE);
  } else if (!IsIntegerTerm(ARG4)) {
    Term te = Yap_Eval(ARG4);

    if (IsIntegerTerm(te)) {
      newFl = IntegerOfTerm(te);
    } else {
      UNLOCK(pe->PELock);
      Yap_Error(TYPE_ERROR_INTEGER, ARG4, "flags");
      return(FALSE);
    }
  } else
    newFl = IntegerOfTerm(ARG4);
  pe->PredFlags = (CELL)newFl;
  UNLOCK(pe->PELock);
  return TRUE;
}

static int 
AlreadyHidden(char *name)
{
  AtomEntry      *chain;

  READ_LOCK(INVISIBLECHAIN.AERWLock);
  chain = RepAtom(INVISIBLECHAIN.Entry);
  READ_UNLOCK(INVISIBLECHAIN.AERWLock);
  while (!EndOfPAEntr(chain) && strcmp(chain->StrOfAE, name) != 0)
    chain = RepAtom(chain->NextOfAE);
  if (EndOfPAEntr(chain))
    return (FALSE);
  return (TRUE);
}

static Int 
p_hide(void)
{				/* hide(+Atom)		 */
  Atom            atomToInclude;
  Term t1 = Deref(ARG1);

  if (IsVarTerm(t1)) {
    Yap_Error(INSTANTIATION_ERROR,t1,"hide/1");
    return(FALSE);
  }
  if (!IsAtomTerm(t1)) {
    Yap_Error(TYPE_ERROR_ATOM,t1,"hide/1");
    return(FALSE);
  }
  atomToInclude = AtomOfTerm(t1);
  if (AlreadyHidden(RepAtom(atomToInclude)->StrOfAE)) {
    Yap_Error(SYSTEM_ERROR,t1,"an atom of name %s was already hidden",
	  RepAtom(atomToInclude)->StrOfAE);
    return(FALSE);
  }
  Yap_ReleaseAtom(atomToInclude);
  WRITE_LOCK(INVISIBLECHAIN.AERWLock);
  WRITE_LOCK(RepAtom(atomToInclude)->ARWLock);
  RepAtom(atomToInclude)->NextOfAE = INVISIBLECHAIN.Entry;
  WRITE_UNLOCK(RepAtom(atomToInclude)->ARWLock);
  INVISIBLECHAIN.Entry = atomToInclude;
  WRITE_UNLOCK(INVISIBLECHAIN.AERWLock);
  return (TRUE);
}

static Int 
p_hidden(void)
{				/* '$hidden'(+F)		 */
  Atom            at;
  AtomEntry      *chain;
  Term t1 = Deref(ARG1);

  if (IsVarTerm(t1))
    return (FALSE);
  if (IsAtomTerm(t1))
    at = AtomOfTerm(t1);
  else if (IsApplTerm(t1))
    at = NameOfFunctor(FunctorOfTerm(t1));
  else
    return (FALSE);
  READ_LOCK(INVISIBLECHAIN.AERWLock);
  chain = RepAtom(INVISIBLECHAIN.Entry);
  while (!EndOfPAEntr(chain) && AbsAtom(chain) != at)
    chain = RepAtom(chain->NextOfAE);
  READ_UNLOCK(INVISIBLECHAIN.AERWLock);
  if (EndOfPAEntr(chain))
    return (FALSE);
  return (TRUE);
}


static Int 
p_unhide(void)
{				/* unhide(+Atom)		 */
  AtomEntry      *atom, *old, *chain;
  Term t1 = Deref(ARG1);

  if (IsVarTerm(t1)) {
    Yap_Error(INSTANTIATION_ERROR,t1,"unhide/1");
    return(FALSE);
  }
  if (!IsAtomTerm(t1)) {
    Yap_Error(TYPE_ERROR_ATOM,t1,"unhide/1");
    return(FALSE);
  }
  atom = RepAtom(AtomOfTerm(t1));
  WRITE_LOCK(atom->ARWLock);
  if (atom->PropsOfAE != NIL) {
    Yap_Error(SYSTEM_ERROR,t1,"cannot unhide an atom in use");
    return(FALSE);
  }
  WRITE_LOCK(INVISIBLECHAIN.AERWLock);
  chain = RepAtom(INVISIBLECHAIN.Entry);
  old = NIL;
  while (!EndOfPAEntr(chain) && strcmp(chain->StrOfAE, atom->StrOfAE) != 0) {
    old = chain;
    chain = RepAtom(chain->NextOfAE);
  }
  if (EndOfPAEntr(chain))
    return (FALSE);
  atom->PropsOfAE = chain->PropsOfAE;
  if (old == NIL)
    INVISIBLECHAIN.Entry = chain->NextOfAE;
  else
    old->NextOfAE = chain->NextOfAE;
  WRITE_UNLOCK(INVISIBLECHAIN.AERWLock);
  WRITE_UNLOCK(atom->ARWLock);
  return (TRUE);
}

void
Yap_show_statistics(void)
{
  unsigned long int heap_space_taken;
  double frag;

#if USE_SYSTEM_MALLOC && HAVE_MALLINFO
  struct mallinfo mi = mallinfo();

  heap_space_taken = (mi.arena+mi.hblkhd)-Yap_HoleSize;
#else
  heap_space_taken = 
    (unsigned long int)(Unsigned(HeapTop)-Unsigned(Yap_HeapBase))-Yap_HoleSize;
#endif
  frag  = (100.0*(heap_space_taken-HeapUsed))/heap_space_taken;

  fprintf(Yap_stderr, "Code Space:  %ld (%ld bytes needed, %ld bytes used, fragmentation %.3f%%).\n", 
	     (unsigned long int)(Unsigned (H0) - Unsigned (Yap_HeapBase)),
	     (unsigned long int)(Unsigned(HeapTop)-Unsigned(Yap_HeapBase)),
	     (unsigned long int)(HeapUsed),
	     frag);
  fprintf(Yap_stderr, "Stack Space: %ld (%ld for Global, %ld for local).\n", 
	     (unsigned long int)(sizeof(CELL)*(LCL0-H0)),
	     (unsigned long int)(sizeof(CELL)*(H-H0)),
	     (unsigned long int)(sizeof(CELL)*(LCL0-ASP)));
  fprintf(Yap_stderr, "Trail Space: %ld (%ld used).\n", 
	     (unsigned long int)(sizeof(tr_fr_ptr)*(Unsigned(Yap_TrailTop)-Unsigned(Yap_TrailBase))),
	     (unsigned long int)(sizeof(tr_fr_ptr)*(Unsigned(TR)-Unsigned(Yap_TrailBase))));
  fprintf(Yap_stderr, "Runtime: %lds.\n", (unsigned long int)(runtime ()));
  fprintf(Yap_stderr, "Cputime: %lds.\n", (unsigned long int)(Yap_cputime ()));
  fprintf(Yap_stderr, "Walltime: %lds.\n", (unsigned long int)(Yap_walltime ()));
}

static Int
p_statistics_heap_max(void)
{
  Term tmax = MkIntegerTerm(HeapMax);

  return(Yap_unify(tmax, ARG1));
}

/* The results of the next routines are not to be trusted too */
/* much. Basically, any stack shifting will seriously confuse the */
/* results */

static Int    TrailTide = -1, LocalTide = -1, GlobalTide = -1;

/* maximum Trail usage */
static Int
TrailMax(void)
{
  Int i;
  Int TrWidth = Unsigned(Yap_TrailTop) - Unsigned(Yap_TrailBase);
  CELL *pt;

  if (TrailTide != TrWidth) {
    pt = (CELL *)TR;
    while (pt+2 < (CELL *)Yap_TrailTop) {
      if (pt[0] == 0 &&
	  pt[1] == 0 &&
	  pt[2] == 0)
	break;
      else
	pt++;
    }
    if (pt+2 < (CELL *)Yap_TrailTop)
      i = Unsigned(pt) - Unsigned(Yap_TrailBase);
    else
      i = TrWidth;
  } else
    return(TrWidth);
  if (TrailTide > i)
    i = TrailTide;
  else
    TrailTide = i;
  return(i);
}

static Int
p_statistics_trail_max(void)
{
  Term tmax = MkIntegerTerm(TrailMax());

  return(Yap_unify(tmax, ARG1));
  
}

/* maximum Global usage */
static Int
GlobalMax(void)
{
  Int i;
  Int StkWidth = Unsigned(LCL0) - Unsigned(H0);
  CELL *pt;

  if (GlobalTide != StkWidth) {
    pt = H;
    while (pt+2 < ASP) {
      if (pt[0] == 0 &&
	  pt[1] == 0 &&
	  pt[2] == 0)
	break;
      else
	pt++;
    }
    if (pt+2 < ASP)
      i = Unsigned(pt) - Unsigned(H0);
    else
      /* so that both Local and Global have reached maximum width */
      GlobalTide = LocalTide = i = StkWidth;
  } else
    return(StkWidth);
  if (GlobalTide > i)
    i = GlobalTide;
  else
    GlobalTide = i;
  return(i);
}

static Int 
p_statistics_global_max(void)
{
  Term tmax = MkIntegerTerm(GlobalMax());

  return(Yap_unify(tmax, ARG1));
  
}


static Int
LocalMax(void)
{
  Int i;
  Int StkWidth = Unsigned(LCL0) - Unsigned(H0);
  CELL *pt;

  if (LocalTide != StkWidth) {
    pt = LCL0;
    while (pt-3 > H) {
      if (pt[-1] == 0 &&
	  pt[-2] == 0 &&
	  pt[-3] == 0)
	break;
      else
	--pt;
    }
    if (pt-3 > H)
      i = Unsigned(LCL0) - Unsigned(pt);
    else
      /* so that both Local and Global have reached maximum width */
      GlobalTide = LocalTide = i = StkWidth;
  } else
    return(StkWidth);
  if (LocalTide > i)
    i = LocalTide;
  else
    LocalTide = i;
  return(i);
}

static Int 
p_statistics_local_max(void)
{
  Term tmax = MkIntegerTerm(LocalMax());

  return(Yap_unify(tmax, ARG1));
  
}



static Int 
p_statistics_heap_info(void)
{
  Term tusage = MkIntegerTerm(HeapUsed);

#if USE_SYSTEM_MALLOC && HAVE_MALLINFO
  struct mallinfo mi = mallinfo();

  Term tmax = MkIntegerTerm((mi.arena+mi.hblkhd)-Yap_HoleSize);
#else
  Term tmax = MkIntegerTerm((Yap_GlobalBase - Yap_HeapBase)-Yap_HoleSize);
#endif

  return(Yap_unify(tmax, ARG1) && Yap_unify(tusage,ARG2));
  
}


static Int 
p_statistics_stacks_info(void)
{
  Term tmax = MkIntegerTerm(Unsigned(LCL0) - Unsigned(H0));
  Term tgusage = MkIntegerTerm(Unsigned(H) - Unsigned(H0));
  Term tlusage = MkIntegerTerm(Unsigned(LCL0) - Unsigned(ASP));

  return(Yap_unify(tmax, ARG1) && Yap_unify(tgusage,ARG2) && Yap_unify(tlusage,ARG3));
  
}



static Int 
p_statistics_trail_info(void)
{
  Term tmax = MkIntegerTerm(Unsigned(Yap_TrailTop) - Unsigned(Yap_TrailBase));
  Term tusage = MkIntegerTerm(Unsigned(TR) - Unsigned(Yap_TrailBase));

  return(Yap_unify(tmax, ARG1) && Yap_unify(tusage,ARG2));
  
}

static Int 
p_statistics_atom_info(void)
{
  UInt count = 0, spaceused = 0, i;

  for (i =0; i < AtomHashTableSize; i++) {
    Atom catom;

    READ_LOCK(HashChain[i].AERWLock);
    catom = HashChain[i].Entry;
    if (catom != NIL) {
      READ_LOCK(RepAtom(catom)->ARWLock);
    }
    READ_UNLOCK(HashChain[i].AERWLock);
    while (catom != NIL) {
      Atom ncatom;
      count++;
      spaceused += sizeof(AtomEntry)+strlen(RepAtom(catom)->StrOfAE);
      ncatom = RepAtom(catom)->NextOfAE;
      if (ncatom != NIL) {
	READ_LOCK(RepAtom(ncatom)->ARWLock);
      }
      READ_UNLOCK(RepAtom(ncatom)->ARWLock);
      catom = ncatom;
    }
  }
  for (i =0; i < WideAtomHashTableSize; i++) {
    Atom catom;

    READ_LOCK(WideHashChain[i].AERWLock);
    catom = WideHashChain[i].Entry;
    if (catom != NIL) {
      READ_LOCK(RepAtom(catom)->ARWLock);
    }
    READ_UNLOCK(WideHashChain[i].AERWLock);
    while (catom != NIL) {
      Atom ncatom;
      count++;
      spaceused += sizeof(AtomEntry)+wcslen((wchar_t *)( RepAtom(catom)->StrOfAE));
      ncatom = RepAtom(catom)->NextOfAE;
      if (ncatom != NIL) {
	READ_LOCK(RepAtom(ncatom)->ARWLock);
      }
      READ_UNLOCK(RepAtom(ncatom)->ARWLock);
      catom = ncatom;
    }
  }
  return Yap_unify(ARG1, MkIntegerTerm(count)) &&
    Yap_unify(ARG2, MkIntegerTerm(spaceused));
}


static Int 
p_statistics_db_size(void)
{
  Term t = MkIntegerTerm(Yap_ClauseSpace);
  Term tit = MkIntegerTerm(Yap_IndexSpace_Tree);
  Term tis = MkIntegerTerm(Yap_IndexSpace_SW);
  Term tie = MkIntegerTerm(Yap_IndexSpace_EXT);

  return
    Yap_unify(t, ARG1) &&
    Yap_unify(tit, ARG2) &&
    Yap_unify(tis, ARG3) &&
    Yap_unify(tie, ARG4);
  
}

static Int 
p_statistics_lu_db_size(void)
{
  Term t = MkIntegerTerm(Yap_LUClauseSpace);
  Term tit = MkIntegerTerm(Yap_LUIndexSpace_Tree);
  Term tic = MkIntegerTerm(Yap_LUIndexSpace_CP);
  Term tix = MkIntegerTerm(Yap_LUIndexSpace_EXT);
  Term tis = MkIntegerTerm(Yap_LUIndexSpace_SW);

  return
    Yap_unify(t, ARG1) &&
    Yap_unify(tit, ARG2) &&
    Yap_unify(tic, ARG3) &&
    Yap_unify(tis, ARG4) &&
    Yap_unify(tix, ARG5);
}



static Term
mk_argc_list(void)
{
  int i =0;
  Term t = TermNil;
  while (i < Yap_argc) {
    char *arg = Yap_argv[i];
    /* check for -L -- */
    if (arg[0] == '-' && arg[1] == 'L') {
      arg += 2;
      while (*arg != '\0' && (*arg == ' ' || *arg == '\t'))
	arg++;
      if (*arg == '-' && arg[1] == '-' && arg[2] == '\0') {
	/* we found the separator */
	int j;
	for (j = Yap_argc-1; j > i+1; --j) {
	  t = MkPairTerm(MkAtomTerm(Yap_LookupAtom(Yap_argv[j])),t);
	}
	return t;
      } else if (Yap_argv[i+1] && Yap_argv[i+1][0] == '-' && Yap_argv[i+1][1] == '-'  && Yap_argv[i+1][2] == '\0') {
	/* we found the separator */
	int j;
	for (j = Yap_argc-1; j > i+2; --j) {
	  t = MkPairTerm(MkAtomTerm(Yap_LookupAtom(Yap_argv[j])),t);
	}
	return t;
      }
    }
    if (arg[0] == '-' && arg[1] == '-' && arg[2] == '\0') {
      /* we found the separator */
      int j;
      for (j = Yap_argc-1; j > i; --j) {
	t = MkPairTerm(MkAtomTerm(Yap_LookupAtom(Yap_argv[j])),t);
      }
      return(t);
    }
    i++;
  } 
  return(t);
}

static Int 
p_argv(void)
{
  Term t = mk_argc_list();
  return Yap_unify(t, ARG1);
}

static Int 
p_executable(void)
{
  
  Yap_TrueFileName (Yap_argv[0], Yap_FileNameBuf, FALSE);
  return Yap_unify(MkAtomTerm(Yap_LookupAtom(Yap_FileNameBuf)),ARG1);
}

static Int
p_access_yap_flags(void)
{
  Term tflag = Deref(ARG1);
  Int flag;
  Term tout = 0;

  if (IsVarTerm(tflag)) {
    Yap_Error(INSTANTIATION_ERROR, tflag, "access_yap_flags/2");
    return(FALSE);		
  }
  if (!IsIntTerm(tflag)) {
    Yap_Error(TYPE_ERROR_INTEGER, tflag, "access_yap_flags/2");
    return(FALSE);		
  }
  flag = IntOfTerm(tflag);
  if (flag < 0 || flag > NUMBER_OF_YAP_FLAGS) {
    return(FALSE);
  }
#ifdef TABLING
  if (flag == TABLING_MODE_FLAG) {
    int n = 0;
    if (IsMode_CompletedOn(yap_flags[flag])) {
      if (IsMode_LoadAnswers(yap_flags[flag]))
	tout = MkAtomTerm(AtomLoadAnswers);
      else
	tout = MkAtomTerm(AtomExecAnswers);
      n++;
    }
    if (IsMode_SchedulingOn(yap_flags[flag])) {
      Term taux = tout;
      if (IsMode_Local(yap_flags[flag]))
	tout = MkAtomTerm(AtomLocalA);
      else
	tout = MkAtomTerm(AtomBatched);
      if (n) {
	taux = MkPairTerm(taux, MkAtomTerm(AtomNil));
	tout = MkPairTerm(tout, taux);
      }
      n++;
    }
    if (n == 0)
      tout = MkAtomTerm(AtomDefault);
  } else
#endif /* TABLING */
  tout = MkIntegerTerm(yap_flags[flag]);
  return(Yap_unify(ARG2, tout));
}

static Int 
p_has_yap_or(void)
{
#ifdef YAPOR
  return(TRUE);
#else
  return(FALSE);
#endif
}

static Int 
p_has_eam(void)
{
#ifdef BEAM
  return(TRUE);
#else
  return(FALSE);
#endif
}


static Int
p_set_yap_flags(void)
{
  Term tflag = Deref(ARG1);
  Term tvalue = Deref(ARG2);
  Int flag, value;

  if (IsVarTerm(tflag)) {
    Yap_Error(INSTANTIATION_ERROR, tflag, "set_yap_flags/2");
    return(FALSE);		
  }
  if (!IsIntTerm(tflag)) {
    Yap_Error(TYPE_ERROR_INTEGER, tflag, "set_yap_flags/2");
    return(FALSE);		
  }
  flag = IntOfTerm(tflag);
  if (IsVarTerm(tvalue)) {
    Yap_Error(INSTANTIATION_ERROR, tvalue, "set_yap_flags/2");
    return(FALSE);		
  }
  if (!IsIntTerm(tvalue)) {
    Yap_Error(TYPE_ERROR_INTEGER, tvalue, "set_yap_flags/2");
    return(FALSE);		
  }
  value = IntOfTerm(tvalue);
  /* checking should have been performed */
  switch(flag) {
  case CHAR_CONVERSION_FLAG:
    if (value != 0 && value != 1)
      return(FALSE);
    yap_flags[CHAR_CONVERSION_FLAG] = value;
    break;
  case YAP_DOUBLE_QUOTES_FLAG:
    if (value < 0 || value > 2)
      return(FALSE);
    yap_flags[YAP_DOUBLE_QUOTES_FLAG] = value;
    break;
  case YAP_TO_CHARS_FLAG:
    if (value != 0 && value != 1)
      return(FALSE);
    yap_flags[YAP_TO_CHARS_FLAG] = value;
    break;
  case LANGUAGE_MODE_FLAG:
    if (value < 0 || value > 2)
      return(FALSE);
    if (value == 1) {
      Yap_heap_regs->pred_meta_call = RepPredProp(PredPropByFunc(FunctorMetaCall,0));
    } else {
      Yap_heap_regs->pred_meta_call = RepPredProp(PredPropByFunc(FunctorMetaCall,0));
    }
    yap_flags[LANGUAGE_MODE_FLAG] = value;
    break;
  case STRICT_ISO_FLAG:
    if (value != 0 && value !=  1)
      return(FALSE);
    yap_flags[STRICT_ISO_FLAG] = value;
    break;
  case SOURCE_MODE_FLAG:
    if (value != 0 && value !=  1)
      return(FALSE);
    yap_flags[SOURCE_MODE_FLAG] = value;
    break;
  case CHARACTER_ESCAPE_FLAG:
    if (value != ISO_CHARACTER_ESCAPES
	&& value != CPROLOG_CHARACTER_ESCAPES
	&& value != SICSTUS_CHARACTER_ESCAPES)
      return(FALSE);
    yap_flags[CHARACTER_ESCAPE_FLAG] = value;
    break;
  case WRITE_QUOTED_STRING_FLAG:
    if (value != 0 && value !=  1)
      return(FALSE);
    yap_flags[WRITE_QUOTED_STRING_FLAG] = value;
    break;
  case ALLOW_ASSERTING_STATIC_FLAG:
    if (value != 0 && value !=  1)
      return(FALSE);
    yap_flags[ALLOW_ASSERTING_STATIC_FLAG] = value;
    break;
  case STACK_DUMP_ON_ERROR_FLAG:
    if (value != 0 && value !=  1)
      return(FALSE);
    yap_flags[STACK_DUMP_ON_ERROR_FLAG] = value;
    break;
  case GENERATE_DEBUG_INFO_FLAG:
    if (value != 0 && value != 1)
      return(FALSE);
    yap_flags[GENERATE_DEBUG_INFO_FLAG] = value;
    break;
  case INDEXING_MODE_FLAG:
    if (value < INDEX_MODE_OFF || value >  INDEX_MODE_MAX)
      return(FALSE);
    yap_flags[INDEXING_MODE_FLAG] = value;
    break;
#ifdef TABLING
  case TABLING_MODE_FLAG:
    if (value == 0) {  /* default */
      tab_ent_ptr tab_ent = GLOBAL_root_tab_ent;
      while(tab_ent) {
	      /* local / batched */
	      if (IsDefaultMode_Local(TabEnt_mode(tab_ent)))
	        SetMode_Local(TabEnt_mode(tab_ent));
	      else
	        SetMode_Batched(TabEnt_mode(tab_ent));
	      
	      /* load answers / exec answers */
	      if (IsDefaultMode_LoadAnswers(TabEnt_mode(tab_ent)))
          TabEnt_set_load(tab_ent)
	      else
          TabEnt_set_exec(tab_ent)
	        
#ifdef TABLING_CALL_SUBSUMPTION
	      /* subsumptive / variant */
	      if (IsDefaultMode_Subsumptive(TabEnt_mode(tab_ent)))
          TabEnt_set_subsumptive(tab_ent)
        else if(IsDefaultMode_Grounded(TabEnt_mode(tab_ent)))
          TabEnt_set_retroactive(tab_ent)
        else
          TabEnt_set_variant(tab_ent)
#endif /* TABLING_CALL_SUBSUMPTION */
	      tab_ent = TabEnt_next(tab_ent);
      }
      yap_flags[TABLING_MODE_FLAG] = 0;
    } else if (value == 1) {  /* batched */
      tab_ent_ptr tab_ent = GLOBAL_root_tab_ent;
      while(tab_ent) {
	      SetMode_Batched(TabEnt_mode(tab_ent));
	      tab_ent = TabEnt_next(tab_ent);
      }
      SetMode_Batched(yap_flags[TABLING_MODE_FLAG]);
      SetMode_SchedulingOn(yap_flags[TABLING_MODE_FLAG]);
    } else if (value == 2) {  /* local */
      tab_ent_ptr tab_ent = GLOBAL_root_tab_ent;
      while(tab_ent) {
	      SetMode_Local(TabEnt_mode(tab_ent));
	      tab_ent = TabEnt_next(tab_ent);
      }
      SetMode_Local(yap_flags[TABLING_MODE_FLAG]);
      SetMode_SchedulingOn(yap_flags[TABLING_MODE_FLAG]);
    } else if (value == 3) {  /* exec_answers */
      tab_ent_ptr tab_ent = GLOBAL_root_tab_ent;
      while(tab_ent) {
        TabEnt_set_exec(tab_ent);
	      tab_ent = TabEnt_next(tab_ent);
      }
      SetMode_ExecAnswers(yap_flags[TABLING_MODE_FLAG]);
      SetMode_CompletedOn(yap_flags[TABLING_MODE_FLAG]);
    } else if (value == 4) {  /* load_answers */
      tab_ent_ptr tab_ent = GLOBAL_root_tab_ent;
      while(tab_ent) {
        TabEnt_set_load(tab_ent);
	      tab_ent = TabEnt_next(tab_ent);
      }
      SetMode_LoadAnswers(yap_flags[TABLING_MODE_FLAG]);
      SetMode_CompletedOn(yap_flags[TABLING_MODE_FLAG]);
#ifdef TABLING_CALL_SUBSUMPTION
    } else if(value == 5) { /* variant */
      tab_ent_ptr tab_ent = GLOBAL_root_tab_ent;
      while(tab_ent) {
        TabEnt_set_variant(tab_ent);
        tab_ent = TabEnt_next(tab_ent);
      }
      SetMode_Variant(yap_flags[TABLING_MODE_FLAG]);
      SetMode_ChecksOn(yap_flags[TABLING_MODE_FLAG]);
    } else if(value == 6) { /* subsumptive */
      tab_ent_ptr tab_ent = GLOBAL_root_tab_ent;
      while(tab_ent) {
        TabEnt_set_subsumptive(tab_ent);
        tab_ent = TabEnt_next(tab_ent);
      }
      SetMode_Subsumptive(yap_flags[TABLING_MODE_FLAG]);
      SetMode_ChecksOn(yap_flags[TABLING_MODE_FLAG]);
    } else if(value == 7) { /* grounded */
      tab_ent_ptr tab_ent = GLOBAL_root_tab_ent;
      while(tab_ent) {
        TabEnt_set_retroactive(tab_ent);
        tab_ent = TabEnt_next(tab_ent);
      }
      SetMode_Grounded(yap_flags[TABLING_MODE_FLAG]);
      SetMode_ChecksOn(yap_flags[TABLING_MODE_FLAG]);
#endif /* TABLING_CALL_SUBSUMPTION */
    }
    break;
#endif /* TABLING */
  case VARS_CAN_HAVE_QUOTE_FLAG:
    if (value != 0  && value != 1)
      return(FALSE);
    yap_flags[VARS_CAN_HAVE_QUOTE_FLAG] = value;
    break;
  case QUIET_MODE_FLAG:
    if (value != 0  && value != 1)
      return(FALSE);
    yap_flags[VARS_CAN_HAVE_QUOTE_FLAG] = value;
    break;
  default:
    return(FALSE);
  }
  return(TRUE);
}

static Int
p_system_mode(void)
{
  Int i = IntegerOfTerm(Deref(ARG1));
  if (i == 0) 
    Yap_PrologMode &= ~SystemMode;
  else
    Yap_PrologMode |= SystemMode;
  return TRUE;
}

static Int
p_lock_system(void)
{
  LOCK(BGL);
  return TRUE;
}

static Int
p_unlock_system(void)
{
  UNLOCK(BGL);
  return TRUE;
}

static Int
p_enterundefp(void)
{
  if (DoingUndefp) {
    return FALSE;
  }
  DoingUndefp = TRUE;
  return TRUE;
}

static Int
p_exitundefp(void)
{
  if (DoingUndefp) {
    DoingUndefp = FALSE;
    return TRUE;
  }
  return FALSE;
}

#ifndef YAPOR
static Int
p_default_sequential(void) {
  return(TRUE);
}
#endif

#ifdef DEBUG
extern void DumpActiveGoals(void);

static Int
p_dump_active_goals(void) {
  DumpActiveGoals();
  return(TRUE);
}
#endif

#ifdef INES
static Int
p_euc_dist(void) {
  Term t1 = Deref(ARG1);
  Term t2 = Deref(ARG2);
  double d1 = (double)(IntegerOfTerm(ArgOfTerm(1,t1))-IntegerOfTerm(ArgOfTerm(1,t2)));
  double d2 = (double)(IntegerOfTerm(ArgOfTerm(2,t1))-IntegerOfTerm(ArgOfTerm(2,t2)));
  double d3 = (double)(IntegerOfTerm(ArgOfTerm(3,t1))-IntegerOfTerm(ArgOfTerm(3,t2)));
  Int result = (Int)sqrt(d1*d1+d2*d2+d3*d3);
  return(Yap_unify(ARG3,MkIntegerTerm(result)));
}

volatile int loop_counter = 0;

static Int
p_loop(void) {
  while (loop_counter == 0);
  return(TRUE);
}
#endif

#if QSAR
static Int
p_in_range(void) {
  Term t;
  double i,j;
  double d1;
  double d2;
  double d3;

  t = Deref(ARG1);
  if (IsFloatTerm(t)) i = FloatOfTerm(t); else i = IntegerOfTerm(t); 
  t = Deref(ARG4);
  if (IsFloatTerm(t)) j = FloatOfTerm(t); else j = IntegerOfTerm(t);
  d1 = i-j;
  t = Deref(ARG2);
  if (IsFloatTerm(t)) i = FloatOfTerm(t); else i = IntegerOfTerm(t); 
  t = Deref(ARG5);
  if (IsFloatTerm(t)) j = FloatOfTerm(t); else j = IntegerOfTerm(t);
  d2 = i-j;
  t = Deref(ARG3);
  if (IsFloatTerm(t)) i = FloatOfTerm(t); else i = IntegerOfTerm(t); 
  t = Deref(ARG6);
  if (IsFloatTerm(t)) j = FloatOfTerm(t); else j = IntegerOfTerm(t);
  d3 = i-j;
  t = Deref(ARG7);
  if (IsFloatTerm(t)) i = FloatOfTerm(t); else i = IntegerOfTerm(t); 
  t = Deref(ARG8);
  if (IsFloatTerm(t)) j = FloatOfTerm(t); else j = IntegerOfTerm(t);
  
  return fabs(sqrt(d1*d1 + d2*d2 + d3*d3)-i) <= j; 

}

static Int
p_in_range2(void) {
  CELL *p1, *p2;
  Term t;
  double i,j;
  double d1;
  double d2;
  double d3;
  UInt arity;
  p1 = RepAppl(Deref(ARG1));
  arity = ArityOfFunctor((Functor)*p1);
  p1 += arity-2;
  p2 = RepAppl(Deref(ARG2))+(arity-2);;

  t = Deref(p1[0]);
  if (IsFloatTerm(t)) i = FloatOfTerm(t); else i = IntegerOfTerm(t); 
  t = Deref(p2[0]);
  if (IsFloatTerm(t)) j = FloatOfTerm(t); else j = IntegerOfTerm(t);
  d1 = i-j;
  t = Deref(p1[1]);
  if (IsFloatTerm(t)) i = FloatOfTerm(t); else i = IntegerOfTerm(t); 
  t = Deref(p2[1]);
  if (IsFloatTerm(t)) j = FloatOfTerm(t); else j = IntegerOfTerm(t);
  d2 = i-j;
  t = Deref(p1[2]);
  if (IsFloatTerm(t)) i = FloatOfTerm(t); else i = IntegerOfTerm(t); 
  t = Deref(p2[2]);
  if (IsFloatTerm(t)) j = FloatOfTerm(t); else j = IntegerOfTerm(t);
  d3 = i-j;
  t = Deref(ARG3);
  if (IsFloatTerm(t)) i = FloatOfTerm(t); else i = IntegerOfTerm(t); 
  t = Deref(ARG4);
  if (IsFloatTerm(t)) j = FloatOfTerm(t); else j = IntegerOfTerm(t);
  
  return fabs(sqrt(d1*d1 + d2*d2 + d3*d3)-i) <= j; 

}
#endif

static Int
p_max_tagged_integer(void) {
  return Yap_unify(ARG1, MkIntTerm(MAX_ABS_INT-1L));
}

static Int
p_min_tagged_integer(void) {
  return Yap_unify(ARG1, MkIntTerm(-MAX_ABS_INT));
}

void 
Yap_InitBackCPreds(void)
{
  Yap_InitCPredBack("$current_atom", 1, 2, init_current_atom, cont_current_atom,
		SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPredBack("$current_wide_atom", 1, 2, init_current_wide_atom,
		    cont_current_wide_atom,
		    SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPredBack("$current_predicate", 3, 1, init_current_predicate, cont_current_predicate,
		SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPredBack("$current_predicate_for_atom", 3, 1, init_current_predicate_for_atom, cont_current_predicate_for_atom,
		SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPredBack("$current_op", 5, 1, init_current_op, cont_current_op,
		SafePredFlag|SyncPredFlag);
  Yap_InitCPredBack("$current_atom_op", 5, 1, init_current_atom_op, cont_current_atom_op,
		SafePredFlag|SyncPredFlag);
  Yap_InitCPredBack("$sub_atom_fetch", 5, 5, init_sub_atom_fetch, cont_sub_atom_fetch, HiddenPredFlag);
#ifdef BEAM
  Yap_InitCPredBack("eam", 1, 0, start_eam, cont_eam,
		SafePredFlag);
#endif

  Yap_InitBackIO();
  Yap_InitBackDB();
  Yap_InitUserBacks();
#if defined MYDDAS_MYSQL && defined CUT_C
  Yap_InitBackMYDDAS_MySQLPreds();
#endif
#if defined MYDDAS_ODBC && defined CUT_C
  Yap_InitBackMYDDAS_ODBCPreds();
#endif
#if defined CUT_C && (defined MYDDAS_ODBC || defined MYDDAS_MYSQL)
  Yap_InitBackMYDDAS_SharedPreds();
#endif
}

typedef void (*Proc)(void);

Proc E_Modules[]= {/* init_fc,*/ (Proc) 0 };

#ifndef YAPOR
static
Int p_yapor_threads(void) {
  return FALSE;
}
#endif


void 
Yap_InitCPreds(void)
{
  /* numerical comparison */
  Yap_InitCPred("set_value", 2, p_setval, SafePredFlag|SyncPredFlag);
  Yap_InitCPred("get_value", 2, p_value, TestPredFlag|SafePredFlag|SyncPredFlag);
  Yap_InitCPred("$values", 3, p_values, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  /* general purpose */
  Yap_InitCPred("$opdec", 4, p_opdec, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("name", 2, p_name, 0);
  Yap_InitCPred("char_code", 2, p_char_code, SafePredFlag);
  Yap_InitCPred("atom_chars", 2, p_atom_chars, 0);
  Yap_InitCPred("atom_codes", 2, p_atom_codes, 0);
  Yap_InitCPred("atom_length", 2, p_atom_length, SafePredFlag);
  Yap_InitCPred("$atom_split", 4, p_atom_split, SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$sub_atom_extract", 5, p_sub_atom_extract, HiddenPredFlag);
  Yap_InitCPred("number_chars", 2, p_number_chars, 0);
  Yap_InitCPred("number_atom", 2, p_number_atom, 0);
  Yap_InitCPred("number_codes", 2, p_number_codes, 0);
  Yap_InitCPred("atom_number", 2, p_atom_number, 0);
  Yap_InitCPred("atom_concat", 2, p_atom_concat, 0);
  Yap_InitCPred("atomic_concat", 2, p_atomic_concat, 0);
  Yap_InitCPred("=..", 2, p_univ, 0);
  Yap_InitCPred("$statistics_trail_max", 1, p_statistics_trail_max, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$statistics_heap_max", 1, p_statistics_heap_max, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$statistics_global_max", 1, p_statistics_global_max, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$statistics_local_max", 1, p_statistics_local_max, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$statistics_heap_info", 2, p_statistics_heap_info, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$statistics_stacks_info", 3, p_statistics_stacks_info, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$statistics_trail_info", 2, p_statistics_trail_info, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$statistics_atom_info", 2, p_statistics_atom_info, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$statistics_db_size", 4, p_statistics_db_size, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$statistics_lu_db_size", 5, p_statistics_lu_db_size, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$argv", 1, p_argv, SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$executable", 1, p_executable, SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$runtime", 2, p_runtime, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$cputime", 2, p_cputime, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$systime", 2, p_systime, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$walltime", 2, p_walltime, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$access_yap_flags", 2, p_access_yap_flags, SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$set_yap_flags", 2, p_set_yap_flags, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$p_system_mode", 1, p_system_mode, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("abort", 0, p_abort, SyncPredFlag);
  Yap_InitCPred("$max_tagged_integer", 1, p_max_tagged_integer, SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$min_tagged_integer", 1, p_min_tagged_integer, SafePredFlag|HiddenPredFlag);
#ifdef BEAM
  Yap_InitCPred("@", 0, eager_split, SafePredFlag);
  Yap_InitCPred(":", 0, force_wait, SafePredFlag);
  Yap_InitCPred("/", 0, commit, SafePredFlag);
  Yap_InitCPred("skip_while_var",1,skip_while_var,SafePredFlag);
  Yap_InitCPred("wait_while_var",1,wait_while_var,SafePredFlag);
  Yap_InitCPred("eamtime", 0, show_time, SafePredFlag);
  Yap_InitCPred("eam", 0, use_eam, SafePredFlag);
#endif
  Yap_InitCPred("$halt", 1, p_halt, SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$lock_system", 0, p_lock_system, SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$unlock_system", 0, p_unlock_system, SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$enter_undefp", 0, p_enterundefp, SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$exit_undefp", 0, p_exitundefp, SafePredFlag|HiddenPredFlag);
  /* basic predicates for the prolog machine tracer */
  /* they are defined in analyst.c */
  /* Basic predicates for the debugger */
  Yap_InitCPred("$creep", 0, p_creep, SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$signal_creep", 0, p_signal_creep, SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$disable_creep", 0, p_disable_creep, SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$disable_docreep", 0, p_disable_docreep, SafePredFlag|HiddenPredFlag);
  Yap_InitCPred("$do_not_creep", 0, p_stop_creep, SafePredFlag|SyncPredFlag|HiddenPredFlag);
#ifdef DEBUG
  Yap_InitCPred("$debug", 1, p_debug, SafePredFlag|SyncPredFlag|HiddenPredFlag);
#endif
  /* Accessing and changing the flags for a predicate */
  Yap_InitCPred("$flags", 4, p_flags, SyncPredFlag|HiddenPredFlag);
  /* hiding and unhiding some predicates */
  Yap_InitCPred("hide", 1, p_hide, SafePredFlag|SyncPredFlag);
  Yap_InitCPred("unhide", 1, p_unhide, SafePredFlag|SyncPredFlag);
  Yap_InitCPred("$hidden", 1, p_hidden, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$has_yap_or", 0, p_has_yap_or, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$has_eam", 0, p_has_eam, SafePredFlag|SyncPredFlag|HiddenPredFlag);
#ifndef YAPOR
  Yap_InitCPred("$default_sequential", 1, p_default_sequential, SafePredFlag|SyncPredFlag|HiddenPredFlag);
  Yap_InitCPred("$yapor_threads", 1, p_yapor_threads, SafePredFlag|SyncPredFlag|HiddenPredFlag);
#endif
#ifdef INES
  Yap_InitCPred("euc_dist", 3, p_euc_dist, SafePredFlag);
  Yap_InitCPred("loop", 0, p_loop, SafePredFlag);
#endif
#if QSAR
  Yap_InitCPred("in_range", 8, p_in_range, TestPredFlag|SafePredFlag);
  Yap_InitCPred("in_range", 4, p_in_range2, TestPredFlag|SafePredFlag);
#endif
#ifdef DEBUG
  Yap_InitCPred("dump_active_goals", 0, p_dump_active_goals, SafePredFlag|SyncPredFlag);
#endif
#ifndef YAPOR
  Yap_InitCPred("$yapor_threads", 1, p_yapor_threads, SafePredFlag|SyncPredFlag);
#endif

  Yap_InitArrayPreds();
  Yap_InitBBPreds();
  Yap_InitBigNums();
  Yap_InitCdMgr();
  Yap_InitCmpPreds();
  Yap_InitCoroutPreds();
  Yap_InitDBPreds();
  Yap_InitExecFs();
  Yap_InitGlobals();
  Yap_InitInlines();
  Yap_InitIOPreds();
  Yap_InitLoadForeign();
  Yap_InitModulesC();
  Yap_InitSavePreds();
  Yap_InitSysPreds();
  Yap_InitUnify();
#if defined CUT_C && defined MYDDAS_MYSQL 
  Yap_InitMYDDAS_MySQLPreds();
#endif
#if defined CUT_C && defined MYDDAS_ODBC 
  Yap_InitMYDDAS_ODBCPreds();
#endif
#if defined CUT_C && (defined MYDDAS_ODBC || defined MYDDAS_MYSQL)
  Yap_InitMYDDAS_SharedPreds();
#endif
#if defined MYDDAS_TOP_LEVEL && defined MYDDAS_MYSQL // && defined HAVE_LIBREADLINE
  Yap_InitMYDDAS_TopLevelPreds();
#endif
  Yap_udi_init();

  Yap_InitUserCPreds();
  Yap_InitUtilCPreds();
  Yap_InitSortPreds();
  Yap_InitMaVarCPreds();
#ifdef DEPTH_LIMIT
  Yap_InitItDeepenPreds();
#endif
#ifdef ANALYST
  Yap_InitAnalystPreds();
#endif
#ifdef LOW_LEVEL_TRACER
  Yap_InitLowLevelTrace();
#endif
  Yap_InitEval();
  Yap_InitGrowPreds();
  Yap_InitLowProf();
#if defined(YAPOR) || defined(TABLING)
  Yap_init_optyap_preds();
#endif /* YAPOR || TABLING */
  Yap_InitThreadPreds();
  {
    void            (*(*(p))) (void) = E_Modules;
    while (*p)
      (*(*p++)) ();
  }
#if CAMACHO
  {
    extern void InitForeignPreds(void);
  
    Yap_InitForeignPreds();
  }
#endif
#if APRIL
  {
    extern void init_ol(void), init_time(void);
  
    init_ol();
    init_time();
  }
#endif
#if SUPPORT_CONDOR
  init_sys();
  init_random();
  //  init_tries();
  init_regexp();
#endif
  {
    Term cm = CurrentModule;
    CurrentModule = SWI_MODULE;
    Yap_swi_install();
    CurrentModule = cm;
  }
}


