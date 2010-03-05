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
* File:		absmi.c							 *
* comments:	Portable abstract machine interpreter                    *
* Last rev:     $Date: 2008-08-13 01:16:26 $,$Author: vsc $						 *
* $Log: not supported by cvs2svn $
* Revision 1.246  2008/08/12 01:27:22  vsc
* MaxOS fixes
* Avoid a thread deadlock
* improvements to SWI predicates.
* make variables_in_term system builtin.
*
* Revision 1.245  2008/08/07 20:51:15  vsc
* more threadin  fixes
*
* Revision 1.244  2008/08/06 23:05:49  vsc
* fix debugging info
*
* Revision 1.243  2008/08/06 17:32:18  vsc
* more thread fixes
*
* Revision 1.242  2008/06/17 13:37:48  vsc
* fix c_interface not to crash when people try to recover slots that are
* not there.
* fix try_logical and friends to handle case where predicate has arity 0.
*
* Revision 1.241  2008/06/04 14:47:18  vsc
* make sure we do trim_trail whenever we mess with B!
*
* Revision 1.240  2008/04/04 16:11:40  vsc
* yapor had gotten broken with recent thread changes
*
* Revision 1.239  2008/04/03 13:26:37  vsc
* protect signal handling with locks for threaded version.
* fix close/1 entry in manual (obs from Nicos).
* fix -f option in chr Makefile.
*
* Revision 1.238  2008/04/03 10:50:23  vsc
* term_variables could store local variable in global.
*
* Revision 1.237  2008/03/26 14:37:07  vsc
* more icc fixes
*
* Revision 1.236  2008/03/25 16:45:52  vsc
* make or-parallelism compile again
*
* Revision 1.235  2008/02/12 17:03:50  vsc
* SWI-portability changes
*
* Revision 1.234  2008/01/27 11:01:06  vsc
* make thread code more stable
*
* Revision 1.233  2008/01/23 17:57:44  vsc
* valgrind it!
* enable atom garbage collection.
*
* Revision 1.232  2007/11/28 23:52:14  vsc
* junction tree algorithm
*
* Revision 1.231  2007/11/26 23:43:07  vsc
* fixes to support threads and assert correctly, even if inefficiently.
*
* Revision 1.230  2007/11/08 15:52:15  vsc
* fix some bugs in new dbterm code.
*
* Revision 1.229  2007/11/07 09:25:27  vsc
* speedup meta-calls
*
* Revision 1.228  2007/11/06 17:02:08  vsc
* compile ground terms away.
*
* Revision 1.227  2007/10/28 11:23:39  vsc
* fix overflow
*
* Revision 1.226  2007/10/28 00:54:09  vsc
* new version of viterbi implementation
* fix all:atvars reporting bad info
* fix bad S info in x86_64
*
* Revision 1.225  2007/10/17 09:18:26  vsc
* growtrail assumed SREG meant ASP?
*
* Revision 1.224  2007/09/24 09:02:31  vsc
* minor bug fixes
*
* Revision 1.223  2007/06/04 12:28:01  vsc
* interface speedups
* bad error message in X is foo>>2.
*
* Revision 1.222  2007/05/01 21:18:19  vsc
* fix bug in saving P at p_eq (obs from Frabrizio Riguzzi)
*
* Revision 1.221  2007/04/10 22:13:20  vsc
* fix max modules limitation
*
* Revision 1.220  2007/03/21 18:32:49  vsc
* fix memory expansion bugs.
*
* Revision 1.219  2007/01/24 09:57:25  vsc
* fix glist_void_varx
*
* Revision 1.218  2006/12/31 01:50:34  vsc
* fix some bugs in call_cleanup: the result of action should not matter,
* and !,fail would not wakeup the delayed goal.
*
* Revision 1.217  2006/12/30 03:25:44  vsc
* call_cleanup/2 and 3
*
* Revision 1.216  2006/12/29 01:57:50  vsc
* allow coroutining plus tabling, this means fixing some trouble with the
* gc and a bug in global variable handling.
*
* Revision 1.215  2006/12/27 01:32:37  vsc
* diverse fixes
*
* Revision 1.214  2006/11/28 00:46:28  vsc
* fix bug in threaded implementation
*
* Revision 1.213  2006/11/27 17:42:02  vsc
* support for UNICODE, and other bug fixes.
*
* Revision 1.212  2006/11/21 16:21:30  vsc
* fix I/O mess
* fix spy/reconsult mess
*
* Revision 1.211  2006/11/15 00:13:36  vsc
* fixes for indexing code.
*
* Revision 1.210  2006/10/25 02:31:07  vsc
* fix emulation of trust_logical
*
* Revision 1.209  2006/10/18 13:47:31  vsc
* index.c implementation of trust_logical was decrementing the wrong
* cp_tr
*
* Revision 1.208  2006/10/11 14:53:57  vsc
* fix memory leak
* fix overflow handling
* VS: ----------------------------------------------------------------------
*
* Revision 1.207  2006/10/10 20:21:42  vsc
* fix new indexing code to actually recover space
* fix predicate info to work for LUs
*
* Revision 1.206  2006/10/10 14:08:15  vsc
* small fixes on threaded implementation.
*
* Revision 1.205  2006/09/28 16:15:54  vsc
* make GMPless version compile.
*
* Revision 1.204  2006/09/20 20:03:51  vsc
* improve indexing on floats
* fix sending large lists to DB
*
* Revision 1.203  2006/08/07 18:51:44  vsc
* fix garbage collector not to try to garbage collect when we ask for large
* chunks of stack in a single go.
*
* Revision 1.202  2006/05/24 02:35:39  vsc
* make chr work and other minor fixes.
*
* Revision 1.201  2006/04/27 14:11:57  rslopes
* *** empty log message ***
*
* Revision 1.200  2006/04/12 17:14:58  rslopes
* fix needed by the EAM engine
*
* Revision 1.199  2006/04/12 15:51:23  rslopes
* small fixes
*
* Revision 1.198  2006/03/30 01:11:09  vsc
* fix nasty variable shunting bug in garbage collector :-(:wq
*
* Revision 1.197  2006/03/24 17:13:41  rslopes
* New update to BEAM engine.
* BEAM now uses YAP Indexing (JITI)
*
* Revision 1.196  2006/03/03 23:10:47  vsc
* fix MacOSX interrupt handling
* fix using Yap files as Yap scripts.
*
* Revision 1.195  2006/02/01 13:28:56  vsc
* bignum support fixes
*
* Revision 1.194  2006/01/26 19:13:24  vsc
* avoid compilation issues with lack of gmp (Remko Troncon)
*
* Revision 1.193  2006/01/18 15:34:53  vsc
* avoid sideffects from MkBigInt
*
* Revision 1.192  2006/01/17 14:10:40  vsc
* YENV may be an HW register (breaks some tabling code)
* All YAAM instructions are now brackedted, so Op introduced an { and EndOp introduces an }. This is because Ricardo assumes that.
* Fix attvars when COROUTING is undefined.
*
* Revision 1.191  2006/01/02 02:16:17  vsc
* support new interface between YAP and GMP, so that we don't rely on our own
* allocation routines.
* Several big fixes.
*
* Revision 1.190  2005/12/23 00:20:13  vsc
* updates to gprof
* support for __POWER__
* Try to saveregs before longjmp.
*
* Revision 1.189  2005/12/17 03:25:38  vsc
* major changes to support online event-based profiling
* improve error discovery and restart on scanner.
*
* Revision 1.188  2005/12/05 17:16:10  vsc
* write_depth/3
* overflow handlings and garbage collection
* Several ipdates to CLPBN
* dif/2 could be broken in the presence of attributed variables.
*
* Revision 1.187  2005/11/26 02:57:25  vsc
* improvements to debugger
* overflow fixes
* reading attvars from DB was broken.
*
* Revision 1.186  2005/11/23 03:01:32  vsc
* fix several bugs in save/restore.b
*
* Revision 1.185  2005/11/18 18:48:51  tiagosoares
* support for executing c code when a cut occurs
*
* Revision 1.184  2005/11/15 00:50:49  vsc
* fixes for stack expansion and garbage collection under tabling.
*
* Revision 1.183  2005/11/07 15:35:47  vsc
* fix bugs in garbage collection of tabling.
*
* Revision 1.182  2005/11/05 03:02:33  vsc
* get rid of unnecessary ^ in setof
* Found bug in comparisons
*
* Revision 1.181  2005/11/04 15:39:14  vsc
* absmi should PREG, never P!!
*
* Revision 1.180  2005/10/28 17:38:49  vsc
* sveral updates
*
* Revision 1.179  2005/10/18 17:04:43  vsc
* 5.1:
* - improvements to GC
*    2 generations
*    generic speedups
* - new scheme for attvars
*    - hProlog like interface also supported
* - SWI compatibility layer
*    - extra predicates
*    - global variables
*    - moved to Prolog module
* - CLP(R) by Leslie De Koninck, Tom Schrijvers, Cristian Holzbaur, Bart
* Demoen and Jan Wielemacker
* - load_files/2
*
* from 5.0.1
*
* - WIN32 missing include files (untested)
* - -L trouble (my thanks to Takeyuchi Shiramoto-san)!
* - debugging of backtrable user-C preds would core dump.
* - redeclaring a C-predicate as Prolog core dumps.
* - badly protected  YapInterface.h.
* - break/0 was failing at exit.
* - YAP_cut_fail and YAP_cut_succeed were different from manual.
* - tracing through data-bases could core dump.
* - cut could break on very large computations.
* - first pass at BigNum issues (reported by Roberto).
* - debugger could get go awol after fail port.
* - weird message on wrong debugger option.
*
* Revision 1.178  2005/10/15 17:05:23  rslopes
* enable profiling on amd64
*
* Revision 1.177  2005/09/09 17:24:37  vsc
* a new and hopefully much better implementation of atts.
*
* Revision 1.176  2005/09/08 22:06:44  rslopes
* BEAM for YAP update...
*
* Revision 1.175  2005/08/12 17:00:00  ricroc
* TABLING FIX: support for incomplete tables
*
* Revision 1.174  2005/08/05 14:55:02  vsc
* first steps to allow mavars with tabling
* fix trailing for tabling with multiple get_cons
*
* Revision 1.173  2005/08/04 15:45:49  ricroc
* TABLING NEW: support to limit the table space size
*
* Revision 1.172  2005/08/02 03:09:48  vsc
* fix debugger to do well nonsource predicates.
*
* Revision 1.171  2005/08/01 15:40:36  ricroc
* TABLING NEW: better support for incomplete tabling
*
* Revision 1.170  2005/07/06 19:33:51  ricroc
* TABLING: answers for completed calls can now be obtained by loading (new option) or executing (default) them from the trie data structure.
*
* Revision 1.169  2005/07/06 15:10:01  vsc
* improvements to compiler: merged instructions and fixes for ->
*
* Revision 1.168  2005/06/04 07:27:33  ricroc
* long int support for tabling
*
* Revision 1.167  2005/06/03 08:26:31  ricroc
* float support for tabling
*
* Revision 1.166  2005/06/01 20:25:22  vsc
* == and \= should not need a choice-point in ->
*
* Revision 1.165  2005/06/01 14:02:45  vsc
* get_rid of try_me?, retry_me? and trust_me? instructions: they are not
* significantly used nowadays.
*
* Revision 1.164  2005/05/26 18:07:32  vsc
* fix warning
*
* Revision 1.163  2005/04/10 04:01:07  vsc
* bug fixes, I hope!
*
* Revision 1.162  2005/04/07 17:48:53  ricroc
* Adding tabling support for mixed strategy evaluation (batched and local scheduling)
*   UPDATE: compilation flags -DTABLING_BATCHED_SCHEDULING and -DTABLING_LOCAL_SCHEDULING removed. To support tabling use -DTABLING in the Makefile or --enable-tabling in configure.
*   NEW: yap_flag(tabling_mode,MODE) changes the tabling execution mode of all tabled predicates to MODE (batched, local or default).
*   NEW: tabling_mode(PRED,MODE) changes the default tabling execution mode of predicate PRED to MODE (batched or local).
*
* Revision 1.161  2005/03/13 06:26:09  vsc
* fix excessive pruning in meta-calls
* fix Term->int breakage in compiler
* improve JPL (at least it does something now for amd64).
*
* Revision 1.160  2005/03/07 17:49:14  vsc
* small fixes
*
* Revision 1.159  2005/03/04 20:29:55  ricroc
* bug fixes for YapTab support
*
* Revision 1.158  2005/03/01 22:25:07  vsc
* fix pruning bug
* make DL_MALLOC less enthusiastic about walking through buckets.
*
* Revision 1.157  2005/02/08 18:04:17  vsc
* library_directory may not be deterministic (usually it isn't).
*
* Revision 1.156  2005/01/13 05:47:25  vsc
* lgamma broke arithmetic optimisation
* integer_y has type y
* pass original source to checker (and maybe even use option in parser)
* use warning mechanism for checker messages.
*
* Revision 1.155  2004/12/28 22:20:34  vsc
* some extra bug fixes for trail overflows: some cannot be recovered that easily,
* some can.
*
* Revision 1.154  2004/12/05 05:01:21  vsc
* try to reduce overheads when running with goal expansion enabled.
* CLPBN fixes
* Handle overflows when allocating big clauses properly.
*
* Revision 1.153  2004/11/19 22:08:35  vsc
* replace SYSTEM_ERROR by out OUT_OF_WHATEVER_ERROR whenever appropriate.
*
* Revision 1.152  2004/11/19 17:14:12  vsc
* a few fixes for 64 bit compiling.
*
* Revision 1.151  2004/11/04 18:22:28  vsc
* don't ever use memory that has been freed (that was done by LU).
* generic fixes for WIN32 libraries
*
* Revision 1.150  2004/10/26 20:15:36  vsc
* More bug fixes for overflow handling
*
* Revision 1.149  2004/10/14 22:14:52  vsc
* don't use a cached version of ARG1 in choice-points
*
* Revision 1.148  2004/09/30 21:37:40  vsc
* fixes for thread support
*
* Revision 1.147  2004/09/30 19:51:53  vsc
* fix overflow from within clause/2
*
* Revision 1.146  2004/09/27 20:45:02  vsc
* Mega clauses
* Fixes to sizeof(expand_clauses) which was being overestimated
* Fixes to profiling+indexing
* Fixes to reallocation of memory after restoring
* Make sure all clauses, even for C, end in _Ystop
* Don't reuse space for Streams
* Fix Stream_F on StreaNo+1
*
* Revision 1.145  2004/09/17 20:47:35  vsc
* fix some overflows recorded.
*
* Revision 1.144  2004/09/17 19:34:49  vsc
* simplify frozen/2
*
* Revision 1.143  2004/08/16 21:02:04  vsc
* more fixes for !
*
* Revision 1.142  2004/08/11 16:14:51  vsc
* whole lot of fixes:
*   - memory leak in indexing
*   - memory management in WIN32 now supports holes
*   - extend Yap interface, more support for SWI-Interface
*   - new predicate mktime in system
*   - buffer console I/O in WIN32
*
* Revision 1.141  2004/07/23 21:08:44  vsc
* windows fixes
*
* Revision 1.140  2004/07/22 21:32:20  vsc
* debugger fixes
* initial support for JPL
* bad calls to garbage collector and gc
* debugger fixes
*
* Revision 1.139  2004/07/03 03:29:24  vsc
* make it compile again on non-linux machines
*
* Revision 1.138  2004/06/29 19:04:40  vsc
* fix multithreaded version
* include new version of Ricardo's profiler
* new predicat atomic_concat
* allow multithreaded-debugging
* small fixes
*
* Revision 1.137  2004/06/23 17:24:19  vsc
* New comment-based message style
* Fix thread support (at least don't deadlock with oneself)
* small fixes for coroutining predicates
* force Yap to recover space in arrays of dbrefs
* use private predicates in debugger.
*
* Revision 1.136  2004/06/17 22:07:22  vsc
* bad bug in indexing code.
*
* Revision 1.135  2004/06/09 03:32:02  vsc
* fix bugs
*
* Revision 1.134  2004/06/05 03:36:59  vsc
* coroutining is now a part of attvars.
* some more fixes.
*
* Revision 1.133  2004/05/13 20:54:57  vsc
* debugger fixes
* make sure we always go back to current module, even during initizlization.
*
* Revision 1.132  2004/04/29 03:45:49  vsc
* fix garbage collection in execute_tail
*
* Revision 1.131  2004/04/22 20:07:02  vsc
* more fixes for USE_SYSTEM_MEMORY
*
* Revision 1.130  2004/04/22 03:24:17  vsc
* trust_logical should protect the last clause, otherwise it cannot
* jump there.
*
* Revision 1.129  2004/04/16 19:27:30  vsc
* more bug fixes
*
* Revision 1.128  2004/04/14 19:10:22  vsc
* expand_clauses: keep a list of clauses to expand
* fix new trail scheme for multi-assignment variables
*
* Revision 1.127  2004/03/31 01:03:09  vsc
* support expand group of clauses
*
* Revision 1.126  2004/03/19 11:35:42  vsc
* trim_trail for default machine
* be more aggressive about try-retry-trust chains.
*    - handle cases where block starts with a wait
*    - don't use _killed instructions, just let the thing rot by itself.
*
* Revision 1.125  2004/03/10 14:59:54  vsc
* optimise -> for type tests
*
* Revision 1.124  2004/03/08 19:31:01  vsc
* move to 4.5.3
*									 *
*									 *
*************************************************************************/


#define IN_ABSMI_C 1

#include "absmi.h"
#include "heapgc.h"

#ifdef CUT_C
#include "cut_c.h"
#endif

#ifdef PUSH_X
#else

/* keep X as a global variable */

Term Yap_XREGS[MaxTemps];	/* 29                                     */

#endif

#include "arith2.h"

/*
  I can creep if I am not a prolog builtin that has been called
  by a prolog builtin,
  exception: meta-calls
*/
static PredEntry *
creep_allowed(PredEntry *p, PredEntry *p0)
{
  if (!p0)
    return NULL;
  if (p0 == PredMetaCall)
    return p0;
  if (!p0->ModuleOfPred && 
      (!p ||
       !p->ModuleOfPred ||
       p->PredFlags & StandardPredFlag))
    return NULL;
  return p;
}

#ifdef COROUTINING
/*
  Imagine we are interrupting the execution, say, because we have a spy
   point or because we have goals to wake up. This routine saves the current
   live temporary registers into a structure pointed to by register ARG1.
   The registers are then recovered by a nasty builtin
   called 
*/
static Term
push_live_regs(yamop *pco)
{
  CELL *lab = (CELL *)(pco->u.l.l);
  CELL max = lab[0];
  CELL curr = lab[1];
  CELL *start = H;
  Int tot = 0;

  if (max) {
    CELL i;

    lab += 2;
    H++;
    for (i=0; i <= max; i++) {
      if (i == 8*CellSize) {
	curr = lab[0];
	lab++;
      }
      if (curr & 1) {
	CELL d1;

	tot+=2;
	H[0] = MkIntTerm(i);
	d1 = XREGS[i];
	deref_head(d1, wake_up_unk);
      wake_up_nonvar:
	/* just copy it to the heap */
	H[1] = d1;
	H += 2;
	continue;

	{
	  CELL *pt0;
	  deref_body(d1, pt0, wake_up_unk, wake_up_nonvar);
	  /* bind it, in case it is a local variable */
	  if (pt0 <= H) {
	    /* variable is safe */
	    H[1] = (CELL)pt0;
	  } else {
	    d1 = Unsigned(H+1);
	    RESET_VARIABLE(H+1);
	    Bind_Local(pt0, d1);
	  }
	}
	H += 2;
      }
      curr >>= 1;
    }
    start[0] = (CELL)Yap_MkFunctor(AtomTrue, tot);
    return(AbsAppl(start));
  } else {
    return(TermNil);
  }
}
#endif

#if defined(ANALYST) || defined(DEBUG)

char *Yap_op_names[_std_top + 1] =
{
#define OPCODE(OP,TYPE) #OP
#include "YapOpcodes.h"
#undef  OPCODE
};

#endif

Int 
Yap_absmi(int inp)
{
#if BP_FREE
  /* some function might be using bp for an internal variable, it is the
     callee's responsability to save it */
  yamop* PCBACKUP = P1REG;
#endif

#ifdef LONG_LIVED_REGISTERS
  register CELL d0, d1;
  register CELL *pt0, *pt1;

#endif /* LONG_LIVED_REGISTERS */
  
#ifdef SHADOW_P 
  register yamop *PREG = P;
#endif /* SHADOW_P */

#ifdef SHADOW_CP
  register yamop *CPREG = CP;
#endif /* SHADOW_CP */

#ifdef SHADOW_HB
  register CELL *HBREG = HB;
#endif /* SHADOW_HB */

#ifdef SHADOW_Y
  register CELL *YREG = Yap_REGS.YENV_;
#endif /* SHADOW_Y */

#ifdef SHADOW_S
  register CELL *SREG = Yap_REGS.S_;
#else
#define SREG S
#endif /* SHADOW_S */

  /* The indexing register so that we will not destroy ARG1 without
   * reason */
#define I_R (XREGS[0])

#if USE_THREADED_CODE
/************************************************************************/
/*     Abstract Machine Instruction Address Table                       */
/*  This must be declared inside the function. We use the asm directive */
/* to make it available outside this function                           */
/************************************************************************/
  static void *OpAddress[] =
  {
#define OPCODE(OP,TYPE) && OP
#include "YapOpcodes.h"
#undef  OPCODE
  };

#endif /* USE_THREADED_CODE */

#ifdef SHADOW_REGS

  /* work with a local pointer to the registers */
  register REGSTORE *regp = &Yap_REGS;

#endif /* SHADOW_REGS */

#if PUSH_REGS

/* useful on a X86 with -fomit-frame-pointer optimisation */
  /* The idea is to push REGS onto the X86 stack frame */

  /* first allocate local space */
  REGSTORE absmi_regs;
  REGSTORE *old_regs = Yap_regp;

#endif /* PUSH_REGS */

#ifdef BEAM
  CELL OLD_B=B;
  extern PredEntry *bpEntry;
  if (inp==-9000) {
#if PUSH_REGS
    old_regs = &Yap_REGS;
    init_absmi_regs(&absmi_regs);
    Yap_regp = &absmi_regs;
#endif
    CACHE_A1();
    PREG=bpEntry->CodeOfPred;
    JMPNext();			/* go execute instruction at PREG */
  }

#endif


#if USE_THREADED_CODE
  /* absmadr */
  if (inp > 0) {
    Yap_ABSMI_OPCODES = OpAddress;
#if BP_FREE
    P1REG = PCBACKUP;
#endif
    return(0);
  }
#endif /* USE_THREADED_CODE */

#if PUSH_REGS
  old_regs = &Yap_REGS;

  /* done, let us now initialise this space */
  init_absmi_regs(&absmi_regs);

  /* the registers are all set up, let's swap */
#ifdef THREADS
  pthread_setspecific(Yap_yaamregs_key, (const void *)&absmi_regs);  
  ThreadHandle[worker_id].current_yaam_regs = &absmi_regs;
#else
  Yap_regp = &absmi_regs;
#endif
#undef Yap_REGS
#define Yap_REGS absmi_regs

#endif /* PUSH_REGS */

#ifdef SHADOW_REGS

  /* use regp as a copy of REGS */
  regp = &Yap_REGS;

#ifdef REGS
#undef REGS
#endif
#define REGS (*regp)

#endif /* SHADOW_REGS */

  setregs();

  CACHE_A1();

 reset_absmi:
  
  SP = SP0;

#if USE_THREADED_CODE
  JMPNext();			/* go execute instruction at P          */

#else
  /* when we start we are not in write mode */

  {
    op_numbers opcode = _Ystop;
    op_numbers old_op;
#ifdef DEBUG_XX
    unsigned long ops_done;
#endif

    goto nextop;

  nextop_write:

    old_op = opcode;
    opcode = PREG->u.o.opcw;
    goto op_switch;

  nextop:

    old_op = opcode;
    opcode = PREG->opc;

  op_switch:

#ifdef ANALYST
    Yap_opcount[opcode]++;
    Yap_2opcount[old_op][opcode]++;
#ifdef DEBUG_XX
    ops_done++;
    /*    if (B->cp_b > 0x103fff90)
      fprintf(stderr,"(%ld) doing %s, done %s, B is %p, HB is %p, H is %p\n",
      ops_done,Yap_op_names[opcode],Yap_op_names[old_op],B,B->cp_h,H);*/
#endif
#endif /* ANALYST */

    switch (opcode) {
#endif /* USE_THREADED_CODE */

    noheapleft:
      {
	CELL cut_b = LCL0-(CELL *)(SREG[E_CB]);

#ifdef SHADOW_S
	S = SREG;
#endif
	saveregs();
	/* do a garbage collection first to check if we can recover memory */
	if (!Yap_growheap(FALSE, 0, NULL)) {
	  Yap_Error(OUT_OF_HEAP_ERROR, TermNil, "YAP failed to grow heap: %s", Yap_ErrorMessage);
	  setregs();
	  FAIL();
	}
	setregs();
	CACHE_A1();
#ifdef SHADOW_S
	SREG = S;
#endif
	if (SREG == ASP) {
	  SREG[E_CB] = (CELL)(LCL0-cut_b);
	}
      }
      goto reset_absmi;

#if !OS_HANDLES_TR_OVERFLOW
    notrailleft:
      /* if we are within indexing code, the system may have to
       * update a S */
      {
	CELL cut_b;

#ifdef SHADOW_S
	S = SREG;
#endif
	/* YREG was pointing to where we were going to build the
	 * next choice-point. The stack shifter will need to know this
	 * to move the local stack */
	if (YREG > (CELL *) PROTECT_FROZEN_B(B)) {
	  ASP = (CELL *) PROTECT_FROZEN_B(B);
	} else {
	  ASP = YREG+E_CB;
	}
	cut_b = LCL0-(CELL *)(ASP[E_CB]);
	saveregs();
	if(!Yap_growtrail (0, FALSE)) {
	  Yap_Error(OUT_OF_TRAIL_ERROR,TermNil,"YAP failed to reserve %ld bytes in growtrail",sizeof(CELL) * 16 * 1024L);
	  setregs();
	  FAIL();
	}
	setregs();
#ifdef SHADOW_S
	SREG = S;
#endif
	if (SREG == ASP) {
	  SREG[E_CB] = (CELL)(LCL0-cut_b);
	}
      }
      goto reset_absmi;

#endif /* OS_HANDLES_TR_OVERFLOW */

      BOp(Ystop, l);
      if (YREG > (CELL *) PROTECT_FROZEN_B(B)) {
	ASP = (CELL *) PROTECT_FROZEN_B(B);
      }
      else {
	ASP = YREG+E_CB;
      }
      saveregs();
#if PUSH_REGS
      restore_absmi_regs(old_regs);
#endif
#if BP_FREE
      P1REG = PCBACKUP;
#endif
      return 1;
      ENDBOp();

      BOp(Nstop, e);
      if (YREG > (CELL *) PROTECT_FROZEN_B(B)) {
	ASP = (CELL *) PROTECT_FROZEN_B(B);
      }
      else {
	ASP = YREG+E_CB;
      }
      saveregs();
#if PUSH_REGS
      restore_absmi_regs(old_regs);
#endif
#if BP_FREE
      P1REG = PCBACKUP;
#endif
      return 0;
      ENDBOp();

/*****************************************************************
*        Plain try - retry - trust instructions                  *
*****************************************************************/
      /* try_me    Label,NArgs */
      Op(try_me, Otapl);
      /* check if enough space between trail and codespace */
      check_trail(TR);
      /* I use YREG =to go through the choicepoint. Usually YREG =is in a
       * register, but sometimes (X86) not. In this case, have a
       * new register to point at YREG =*/
      CACHE_Y(YREG);
      /* store arguments for procedure */
      store_at_least_one_arg(PREG->u.Otapl.s);
      /* store abstract machine registers */
      store_yaam_regs(PREG->u.Otapl.d, 0);
      /* On a try_me, set cut to point at previous choicepoint,
       * that is, to the B before the cut.
       */
      set_cut(S_YREG, B);
      /* now, install the new YREG =*/
      B = B_YREG;
#ifdef YAPOR
      SCH_set_load(B_YREG);
#endif	/* YAPOR */
      SET_BB(B_YREG);
      ENDCACHE_Y();
      PREG = NEXTOP(PREG, Otapl);
      GONext();
      ENDOp();

      /* retry_me    Label,NArgs */
      Op(retry_me, Otapl);
      CACHE_Y(B);
      /* After retry, cut should be pointing at the parent
       * choicepoint for the current B */
      restore_yaam_regs(PREG->u.Otapl.d);
      restore_at_least_one_arg(PREG->u.Otapl.s);
#ifdef FROZEN_STACKS
      S_YREG = (CELL *) PROTECT_FROZEN_B(B_YREG);
      set_cut(S_YREG, B->cp_b);
#else
      set_cut(S_YREG, B_YREG->cp_b);
#endif /* FROZEN_STACKS */
      SET_BB(B_YREG);
      ENDCACHE_Y();
      PREG = NEXTOP(PREG, Otapl);
      GONext();
      ENDOp();

      /* trust_me    UnusedLabel,NArgs */
      Op(trust_me, Otapl);
      CACHE_Y(B);
#ifdef YAPOR
      if (SCH_top_shared_cp(B)) {
	SCH_last_alternative(PREG, B_YREG);
	restore_at_least_one_arg(PREG->u.Otapl.s);
#ifdef FROZEN_STACKS
	S_YREG = (CELL *) PROTECT_FROZEN_B(B_YREG);
#endif /* FROZEN_STACKS */
	set_cut(S_YREG, B->cp_b);
      }
      else
#endif	/* YAPOR */
      {
	pop_yaam_regs();
	pop_at_least_one_arg(PREG->u.Otapl.s);
	/* After trust, cut should be pointing at the new top
	 * choicepoint */
#ifdef FROZEN_STACKS
	S_YREG = (CELL *) PROTECT_FROZEN_B(B_YREG);
#endif /* FROZEN_STACKS */
	set_cut(S_YREG, B);
      }
      PREG = NEXTOP(PREG, Otapl);
      SET_BB(B_YREG);
      ENDCACHE_Y();
      GONext();
      ENDOp();

/*****************************************************************
*        Profiled try - retry - trust instructions               *
*****************************************************************/

      /* profiled_enter_me    Pred */
      Op(enter_profiling, p);
      LOCK(PREG->u.p.p->StatisticsForPred.lock);
      PREG->u.p.p->StatisticsForPred.NOfEntries++;
      UNLOCK(PREG->u.p.p->StatisticsForPred.lock);
      PREG = NEXTOP(PREG, p);
      GONext();
      ENDOp();

     /* profiled_retry    Label,NArgs */
      Op(retry_profiled, p);
      LOCK(PREG->u.p.p->StatisticsForPred.lock);
      PREG->u.p.p->StatisticsForPred.NOfRetries++;
      UNLOCK(PREG->u.p.p->StatisticsForPred.lock);
      PREG = NEXTOP(PREG, p);
      GONext();
      ENDOp();

      /* profiled_retry_me    Label,NArgs */
      Op(profiled_retry_me, Otapl);
      CACHE_Y(B);
      /* After retry, cut should be pointing at the parent
       * choicepoint for the current B */
      LOCK(PREG->u.Otapl.p->StatisticsForPred.lock);
      PREG->u.Otapl.p->StatisticsForPred.NOfRetries++;
      UNLOCK(PREG->u.Otapl.p->StatisticsForPred.lock);
      restore_yaam_regs(PREG->u.Otapl.d);
      restore_args(PREG->u.Otapl.s);
#ifdef FROZEN_STACKS
      S_YREG = (CELL *) PROTECT_FROZEN_B(B_YREG);
      set_cut(S_YREG, B->cp_b);
#else
      set_cut(S_YREG, B_YREG->cp_b);
#endif /* FROZEN_STACKS */
      SET_BB(B_YREG);
      ENDCACHE_Y();
      PREG = NEXTOP(PREG, Otapl);
      GONext();
      ENDOp();

      /* profiled_trust_me    UnusedLabel,NArgs */
      Op(profiled_trust_me, Otapl);
      CACHE_Y(B);
#ifdef YAPOR
      if (SCH_top_shared_cp(B)) {
	SCH_last_alternative(PREG, B_YREG);
	restore_args(PREG->u.Otapl.s);
#ifdef FROZEN_STACKS
        S_YREG = (CELL *) PROTECT_FROZEN_B(B_YREG);
#endif /* FROZEN_STACKS */
	set_cut(S_YREG, B->cp_b);
      }
      else
#endif	/* YAPOR */
      {
	pop_yaam_regs();
	pop_args(PREG->u.Otapl.s);
	/* After trust, cut should be pointing at the new top
	 * choicepoint */
#ifdef FROZEN_STACKS
	S_YREG = (CELL *) PROTECT_FROZEN_B(B_YREG);
#endif /* FROZEN_STACKS */
	set_cut(S_YREG, B);
      }
      SET_BB(B_YREG);
      ENDCACHE_Y();
      LOCK(PREG->u.Otapl.p->StatisticsForPred.lock);
      PREG->u.Otapl.p->StatisticsForPred.NOfRetries++;
      UNLOCK(PREG->u.Otapl.p->StatisticsForPred.lock);
      PREG = NEXTOP(PREG, Otapl);
      GONext();
      ENDOp();

      BOp(profiled_retry_logical, OtaLl);
      check_trail(TR);
      {
	UInt timestamp;
	CACHE_Y(B);
      
	timestamp = IntegerOfTerm(((CELL *)(B_YREG+1))[PREG->u.OtaLl.s]);
	if (!VALID_TIMESTAMP(timestamp, PREG->u.OtaLl.d)) {
	  /* jump to next instruction */
	  PREG=PREG->u.OtaLl.n;
	  JMPNext();
	}
	restore_yaam_regs(PREG->u.OtaLl.n);
	restore_args(PREG->u.OtaLl.s);
	LOCK(PREG->u.OtaLl.d->ClPred->StatisticsForPred.lock);
	PREG->u.OtaLl.d->ClPred->StatisticsForPred.NOfRetries++;
	UNLOCK(PREG->u.OtaLl.d->ClPred->StatisticsForPred.lock);
#ifdef THREADS
	PP = PREG->u.OtaLl.d->ClPred;
#endif
	PREG = PREG->u.OtaLl.d->ClCode;
#ifdef FROZEN_STACKS
	S_YREG = (CELL *) PROTECT_FROZEN_B(B_YREG);
	set_cut(S_YREG, B->cp_b);
#else
	set_cut(S_YREG, B_YREG->cp_b);
#endif /* FROZEN_STACKS */
	SET_BB(B_YREG);
	ENDCACHE_Y();
      }
      JMPNext();
      ENDBOp();

      BOp(profiled_trust_logical, OtILl);
      CACHE_Y(B);
      {
	LogUpdIndex *cl = PREG->u.OtILl.block;
	PredEntry *ap = cl->ClPred;
	LogUpdClause *lcl = PREG->u.OtILl.d;
	UInt timestamp = IntegerOfTerm(((CELL *)(B_YREG+1))[ap->ArityOfPE]);

	if (!VALID_TIMESTAMP(timestamp, lcl)) {
	  /* jump to next alternative */
	  PREG = FAILCODE;
	} else {
	  LOCK(ap->StatisticsForPred.lock);
	  ap->StatisticsForPred.NOfRetries++;
	  UNLOCK(ap->StatisticsForPred.lock);
	  PREG = lcl->ClCode;
	}
	/* HEY, leave indexing block alone!! */
	/* check if we are the ones using this code */
#if defined(YAPOR) || defined(THREADS)
	LOCK(ap->PELock);
	PP = ap;
	DEC_CLREF_COUNT(cl);
	/* clear the entry from the trail */
	B->cp_tr--;
	TR = B->cp_tr;
	/* actually get rid of the code */
	if (cl->ClRefCount == 0 && (cl->ClFlags & (ErasedMask|DirtyMask))) {
	  if (PREG != FAILCODE) {
	    /* I am the last one using this clause, hence I don't need a lock
	       to dispose of it 
	    */
	    if (lcl->ClRefCount == 1) {
	      /* make sure the clause isn't destroyed */
	      /* always add an extra reference */
	      INC_CLREF_COUNT(lcl);
	      TRAIL_CLREF(lcl);
	    }
	  }
	  if (cl->ClFlags & ErasedMask) {
	    saveregs();
	    Yap_ErLogUpdIndex(cl);
	    setregs();
	  } else {
	    saveregs();
	    Yap_CleanUpIndex(cl);
	    setregs();
	  }
	  save_pc();
	}
#else
	if (TrailTerm(B->cp_tr-1) == CLREF_TO_TRENTRY(cl) &&
	    B->cp_tr != B->cp_b->cp_tr) {
	  cl->ClFlags &= ~InUseMask;
	  TR = --B->cp_tr;
	  /* next, recover space for the indexing code if it was erased */
	  if (cl->ClFlags & (ErasedMask|DirtyMask)) {
	    if (PREG != FAILCODE) {
	      /* make sure we don't erase the clause we are jumping too */
	      if (lcl->ClRefCount == 1 && !(lcl->ClFlags & InUseMask)) {
		lcl->ClFlags |= InUseMask;
		TRAIL_CLREF(lcl);
	      }
	    }
	    if (cl->ClFlags & ErasedMask) {
	      saveregs();
	      Yap_ErLogUpdIndex(cl);
	      setregs();
	    } else {
	      saveregs();
	      Yap_CleanUpIndex(cl);
	      setregs();
	    }
	    save_pc();
	  }
	}
#endif
#ifdef YAPOR
	if (SCH_top_shared_cp(B)) {
	  SCH_last_alternative(PREG, B_YREG);
	  restore_args(ap->ArityOfPE);
#ifdef FROZEN_STACKS
	  S_YREG = (CELL *) PROTECT_FROZEN_B(B_YREG);
#else
	  S_YREG++;
#endif /* FROZEN_STACKS */
	  set_cut(S_YREG, B->cp_b);
	} else
#endif	/* YAPOR */
	  {
	    pop_yaam_regs();
	    pop_args(ap->ArityOfPE);
	    S_YREG--;
#ifdef FROZEN_STACKS
	    S_YREG = (CELL *) PROTECT_FROZEN_B(B_YREG);
#endif /* FROZEN_STACKS */
	    set_cut(S_YREG, B);
	  }
	SET_BB(B_YREG);
	ENDCACHE_Y();
	JMPNext();
      }
      ENDBOp();

/*****************************************************************
*        Call count instructions                                 *
*****************************************************************/

      /* count_enter_me    Label,NArgs */
      Op(count_call, p);
      LOCK(PREG->u.p.p->StatisticsForPred.lock);
      PREG->u.p.p->StatisticsForPred.NOfEntries++;
      UNLOCK(PREG->u.p.p->StatisticsForPred.lock);
      ReductionsCounter--;
      if (ReductionsCounter == 0 && ReductionsCounterOn) {
	saveregs();
	Yap_Error(CALL_COUNTER_UNDERFLOW,TermNil,"");
	setregs();
	JMPNext();
      } 
      PredEntriesCounter--;
      if (PredEntriesCounter == 0 && PredEntriesCounterOn) {
	saveregs();
	Yap_Error(PRED_ENTRY_COUNTER_UNDERFLOW,TermNil,"");
	setregs();
	JMPNext();
      } 
      PREG = NEXTOP(PREG, p);
      GONext();
      ENDOp();

      /* count_retry    Label,NArgs */
      Op(count_retry, p);
      LOCK(PREG->u.p.p->StatisticsForPred.lock);
      PREG->u.p.p->StatisticsForPred.NOfRetries++;
      UNLOCK(PREG->u.p.p->StatisticsForPred.lock);
      RetriesCounter--;
      if (RetriesCounter == 0 && RetriesCounterOn) {
	/* act as if we had backtracked */
	ENV = B->cp_env;
	saveregs();
	Yap_Error(RETRY_COUNTER_UNDERFLOW,TermNil,"");
	setregs();
	JMPNext();
      } 
      PredEntriesCounter--;
      if (PredEntriesCounter == 0 && PredEntriesCounterOn) {
	ENV = B->cp_env;
	saveregs();
	Yap_Error(PRED_ENTRY_COUNTER_UNDERFLOW,TermNil,"");
	setregs();
	JMPNext();
      } 
      PREG = NEXTOP(PREG, p);
      GONext();
      ENDOp();

      /* count_retry_me    Label,NArgs */
      Op(count_retry_me, Otapl);
      CACHE_Y(B);
      restore_yaam_regs(PREG->u.Otapl.d);
      restore_args(PREG->u.Otapl.s);
      /* After retry, cut should be pointing at the parent
       * choicepoint for the current B */
#ifdef FROZEN_STACKS
      S_YREG = (CELL *) PROTECT_FROZEN_B(B_YREG);
      set_cut(S_YREG, B->cp_b);
#else
      set_cut(S_YREG, B_YREG->cp_b);
#endif /* FROZEN_STACKS */
      SET_BB(B_YREG);
      ENDCACHE_Y();
      LOCK(((PredEntry *)(PREG->u.Otapl.p))->StatisticsForPred.lock);
      ((PredEntry *)(PREG->u.Otapl.p))->StatisticsForPred.NOfRetries++;
      UNLOCK(((PredEntry *)(PREG->u.Otapl.p))->StatisticsForPred.lock);
      RetriesCounter--;
      if (RetriesCounter == 0 && RetriesCounterOn) {
	saveregs();
	Yap_Error(RETRY_COUNTER_UNDERFLOW,TermNil,"");
	setregs();
	JMPNext();
      } 
      PredEntriesCounter--;
      if (PredEntriesCounter == 0 && PredEntriesCounterOn) {
	saveregs();
	Yap_Error(PRED_ENTRY_COUNTER_UNDERFLOW,TermNil,"");
	setregs();
	JMPNext();
      } 
      PREG = NEXTOP(PREG, Otapl);
      GONext();
      ENDOp();

      /* count_trust_me    UnusedLabel,NArgs */
      Op(count_trust_me, Otapl);
      CACHE_Y(B);
#ifdef YAPOR
      if (SCH_top_shared_cp(B)) {
	SCH_last_alternative(PREG, B_YREG);
	restore_args(PREG->u.Otapl.s);
#ifdef FROZEN_STACKS
        S_YREG = (CELL *) PROTECT_FROZEN_B(B_YREG);
#endif /* FROZEN_STACKS */
	set_cut(S_YREG, B->cp_b);
      }
      else
#endif	/* YAPOR */
      {
	pop_yaam_regs();
	pop_args(PREG->u.Otapl.s);
	/* After trust, cut should be pointing at the new top
	 * choicepoint */
#ifdef FROZEN_STACKS
	S_YREG = (CELL *) PROTECT_FROZEN_B(B_YREG);
#endif /* FROZEN_STACKS */
	set_cut(S_YREG, B);
      }
      SET_BB(B_YREG);
      ENDCACHE_Y();
      RetriesCounter--;
      if (RetriesCounter == 0) {
	saveregs();
	Yap_Error(RETRY_COUNTER_UNDERFLOW,TermNil,"");
	setregs();
	JMPNext();
      } 
      PredEntriesCounter--;
      if (PredEntriesCounter == 0) {
	saveregs();
	Yap_Error(PRED_ENTRY_COUNTER_UNDERFLOW,TermNil,"");
	setregs();
	JMPNext();
      } 
      LOCK(((PredEntry *)(PREG->u.Otapl.p))->StatisticsForPred.lock);
      ((PredEntry *)(PREG->u.Otapl.p))->StatisticsForPred.NOfRetries++;
      UNLOCK(((PredEntry *)(PREG->u.Otapl.p))->StatisticsForPred.lock);
      PREG = NEXTOP(PREG, Otapl);
      GONext();
      ENDOp();

      BOp(count_retry_logical, OtaLl);
      check_trail(TR);
      {
	UInt timestamp;
	CACHE_Y(B);
      
	timestamp = IntegerOfTerm(((CELL *)(B_YREG+1))[PREG->u.OtaLl.s]);
	if (!VALID_TIMESTAMP(timestamp, PREG->u.OtaLl.d)) {
	  /* jump to next instruction */
	  PREG=PREG->u.OtaLl.n;
	  JMPNext();
	}
	restore_yaam_regs(PREG->u.OtaLl.n);
	restore_args(PREG->u.OtaLl.s);
	RetriesCounter--;
	if (RetriesCounter == 0) {
	  saveregs();
	  Yap_Error(RETRY_COUNTER_UNDERFLOW,TermNil,"");
	  setregs();
	  JMPNext();
	} 
	PredEntriesCounter--;
	if (PredEntriesCounter == 0) {
	  saveregs();
	  Yap_Error(PRED_ENTRY_COUNTER_UNDERFLOW,TermNil,"");
	  setregs();
	  JMPNext();
	} 
	LOCK(PREG->u.OtaLl.d->ClPred->StatisticsForPred.lock);
	PREG->u.OtaLl.d->ClPred->StatisticsForPred.NOfRetries++;
	UNLOCK(PREG->u.OtaLl.d->ClPred->StatisticsForPred.lock);
#ifdef THREADS
	PP = PREG->u.OtaLl.d->ClPred;
#endif
	PREG = PREG->u.OtaLl.d->ClCode;
#ifdef FROZEN_STACKS
	S_YREG = (CELL *) PROTECT_FROZEN_B(B_YREG);
	set_cut(S_YREG, B->cp_b);
#else
	set_cut(S_YREG, B_YREG->cp_b);
#endif /* FROZEN_STACKS */
	SET_BB(B_YREG);
	ENDCACHE_Y();
      }
      JMPNext();
      ENDBOp();

      BOp(count_trust_logical, OtILl);
      CACHE_Y(B);
      {
	LogUpdIndex *cl = PREG->u.OtILl.block;
	PredEntry *ap = cl->ClPred;
	LogUpdClause *lcl = PREG->u.OtILl.d;
	UInt timestamp = IntegerOfTerm(((CELL *)(B_YREG+1))[ap->ArityOfPE]);

	if (!VALID_TIMESTAMP(timestamp, lcl)) {
	  /* jump to next alternative */
	  PREG = FAILCODE;
	} else {
	  RetriesCounter--;
	  if (RetriesCounter == 0) {
	    saveregs();
	    Yap_Error(RETRY_COUNTER_UNDERFLOW,TermNil,"");
	    setregs();
	    JMPNext();
	  } 
	  PredEntriesCounter--;
	  if (PredEntriesCounter == 0) {
	    saveregs();
	    Yap_Error(PRED_ENTRY_COUNTER_UNDERFLOW,TermNil,"");
	    setregs();
	    JMPNext();
	  } 
	  LOCK(ap->StatisticsForPred.lock);
	  ap->StatisticsForPred.NOfRetries++;
	  UNLOCK(ap->StatisticsForPred.lock);
	  PREG = lcl->ClCode;
	}
	/* HEY, leave indexing block alone!! */
	/* check if we are the ones using this code */
#if defined(YAPOR) || defined(THREADS)
	LOCK(ap->PELock);
	PP = ap;
	DEC_CLREF_COUNT(cl);
	/* clear the entry from the trail */
	TR = --B->cp_tr;
	/* actually get rid of the code */
	if (cl->ClRefCount == 0 && (cl->ClFlags & (ErasedMask|DirtyMask))) {
	  if (PREG != FAILCODE) {
	    /* I am the last one using this clause, hence I don't need a lock
	       to dispose of it 
	    */
	    if (lcl->ClRefCount == 1) {
	      /* make sure the clause isn't destroyed */
	      /* always add an extra reference */
	      INC_CLREF_COUNT(lcl);
	      TRAIL_CLREF(lcl);
	    }
	  }
	  if (cl->ClFlags & ErasedMask) {
	    saveregs();
	    Yap_ErLogUpdIndex(cl);
	    setregs();
	  } else {
	    saveregs();
	    Yap_CleanUpIndex(cl);
	    setregs();
	  }
	  save_pc();
	}
#else
	if (TrailTerm(B->cp_tr-1) == CLREF_TO_TRENTRY(cl) &&
	    B->cp_tr != B->cp_b->cp_tr) {
	  cl->ClFlags &= ~InUseMask;
	  TR = --B->cp_tr;
	  /* next, recover space for the indexing code if it was erased */
	  if (cl->ClFlags & (ErasedMask|DirtyMask)) {
	    if (PREG != FAILCODE) {
	      /* make sure we don't erase the clause we are jumping too */
	      if (lcl->ClRefCount == 1 && !(lcl->ClFlags & InUseMask)) {
		lcl->ClFlags |= InUseMask;
		TRAIL_CLREF(lcl);
	      }
	    }
	    if (cl->ClFlags & ErasedMask) {
	      saveregs();
	      Yap_ErLogUpdIndex(cl);
	      setregs();
	    } else {
	      saveregs();
	      Yap_CleanUpIndex(cl);
	      setregs();
	    }
	    save_pc();
	  }
	}
#endif
#ifdef YAPOR
	if (SCH_top_shared_cp(B)) {
	  SCH_last_alternative(PREG, B_YREG);
	  restore_args(ap->ArityOfPE);
#ifdef FROZEN_STACKS
	  S_YREG = (CELL *) PROTECT_FROZEN_B(B_YREG);
#else
	  S_YREG++;
#endif /* FROZEN_STACKS */
	  set_cut(S_YREG, B->cp_b);
	} else
#endif	/* YAPOR */
	  {
	    pop_yaam_regs();
	    pop_args(ap->ArityOfPE);
	    S_YREG--;
#ifdef FROZEN_STACKS
	    S_YREG = (CELL *) PROTECT_FROZEN_B(B_YREG);
#endif /* FROZEN_STACKS */
	    set_cut(S_YREG, B);
	  }
	SET_BB(B_YREG);
	ENDCACHE_Y();
	JMPNext();
      }
      ENDBOp();



/*****************************************************************
*        enter a logical semantics dynamic predicate             *
*****************************************************************/

      /* only meaningful with THREADS on! */
      /* lock logical updates predicate.  */
      Op(lock_lu, p);
#if defined(YAPOR) || defined(THREADS)
      if (PP) {
	GONext();
      }
      PP = PREG->u.p.p;
      LOCK(PP->PELock);
#endif
      PREG = NEXTOP(PREG, p);
      GONext();
      ENDOp();
 
      /* only meaningful with THREADS on! */
      /* lock logical updates predicate.  */
      Op(unlock_lu, e);
#if defined(YAPOR) || defined(THREADS)
      UNLOCK(PP->PELock);
      PP = NULL;
#endif
      PREG = NEXTOP(PREG, e);
      GONext();
      ENDOp();
 

      /* enter logical pred               */
      BOp(alloc_for_logical_pred, L);
      check_trail(TR); 
      /* say that an environment is using this clause */
      /* we have our own copy for the clause */
#if defined(YAPOR) || defined(THREADS)
      {
	LogUpdClause *cl = PREG->u.L.ClBase;
	PredEntry *ap = cl->ClPred;

	/* always add an extra reference */
	INC_CLREF_COUNT(cl);
	TRAIL_CLREF(cl);
	UNLOCK(ap->PELock);
	PP = NULL;
      }
#else
      {
	LogUpdClause *cl = (LogUpdClause *)PREG->u.L.ClBase;
	if (!(cl->ClFlags & InUseMask)) {
	  cl->ClFlags |= InUseMask;
	  TRAIL_CLREF(cl);
	}
      }
#endif
      PREG = NEXTOP(PREG, L);
      GONext();
      ENDBOp();

      /* copy database term               */
      BOp(copy_idb_term, e);
      {
	LogUpdClause *cl = ClauseCodeToLogUpdClause(PREG);
	Term t;

	ASP = YREG+E_CB;
	saveregs();
	while ((t = Yap_FetchTermFromDB(cl->ClSource)) == 0L) {
	  if (Yap_Error_TYPE == OUT_OF_ATTVARS_ERROR) {
	    Yap_Error_TYPE = YAP_NO_ERROR;
	    if (!Yap_growglobal(NULL)) {
	      UNLOCK(PP->PELock);
#if defined(YAPOR) || defined(THREADS)
	      PP = NULL;
#endif
	      Yap_Error(OUT_OF_ATTVARS_ERROR, TermNil, Yap_ErrorMessage);
	      FAIL();
	    }
	  } else {
	    Yap_Error_TYPE = YAP_NO_ERROR;
	    if (!Yap_gc(3, ENV, CP)) {
	      UNLOCK(PP->PELock);
#if defined(YAPOR) || defined(THREADS)
	      PP = NULL;
#endif
	      Yap_Error(OUT_OF_STACK_ERROR, TermNil, Yap_ErrorMessage);
	      FAIL();
	    }
	  }
	}
	if (!Yap_IUnify(ARG2, t)) {
	  setregs();
	  UNLOCK(PP->PELock);
#if defined(YAPOR) || defined(THREADS)
	  PP = NULL;
#endif
	  FAIL();
	}
	if (!Yap_IUnify(ARG3, MkDBRefTerm((DBRef)cl))) {
	  setregs();
	  UNLOCK(PP->PELock);
#if defined(YAPOR) || defined(THREADS)
	  PP = NULL;
#endif
	  FAIL();
	}
	setregs();

#if defined(YAPOR) || defined(THREADS)
	/* always add an extra reference */
	INC_CLREF_COUNT(cl);
	TRAIL_CLREF(cl);
	UNLOCK(PP->PELock);
	PP = NULL;
#else
	if (!(cl->ClFlags & InUseMask)) {
	  /* Clause *cl = (Clause *)PREG->u.EC.ClBase;

	  PREG->u.EC.ClTrail = TR-(tr_fr_ptr)Yap_TrailBase;
	  PREG->u.EC.ClENV = LCL0-YREG;*/
	  cl->ClFlags |= InUseMask;
	  TRAIL_CLREF(cl);
	}
#endif
      }
      PREG = CPREG;
      YREG = ENV;
#ifdef DEPTH_LIMIT
      DEPTH = YREG[E_DEPTH];
#endif
      GONext();
      ENDBOp();


      /* unify with database term               */
      BOp(unify_idb_term, e);
      {
	LogUpdClause *cl = ClauseCodeToLogUpdClause(PREG);

	saveregs();
	if (!Yap_IUnify(ARG2, cl->ClSource->Entry)) {
	  setregs();
	  UNLOCK(PP->PELock);
#if defined(YAPOR) || defined(THREADS)
	  PP = NULL;
#endif
	  FAIL();
	}
	if (!Yap_IUnify(ARG3, MkDBRefTerm((DBRef)cl))) {
	  setregs();
	  UNLOCK(PP->PELock);
#if defined(YAPOR) || defined(THREADS)
	  PP = NULL;
#endif
	  FAIL();
	}
	setregs();

	/* say that an environment is using this clause */
	/* we have our own copy for the clause */
#if defined(YAPOR) || defined(THREADS)
	/* always add an extra reference */
	INC_CLREF_COUNT(cl);
	TRAIL_CLREF(cl);
	UNLOCK(PP->PELock);
	PP = NULL;
#else
	if (!(cl->ClFlags & InUseMask)) {
	  /* Clause *cl = (Clause *)PREG->u.EC.ClBase;

	  PREG->u.EC.ClTrail = TR-(tr_fr_ptr)Yap_TrailBase;
	  PREG->u.EC.ClENV = LCL0-YREG;*/
	  cl->ClFlags |= InUseMask;
	  TRAIL_CLREF(cl);
	}
#endif
      }
      PREG = CPREG;
      YREG = ENV;
#ifdef DEPTH_LIMIT
      DEPTH = YREG[E_DEPTH];
#endif
      GONext();
      ENDBOp();


/*****************************************************************
*        try and retry of dynamic predicates                     *
*****************************************************************/

      /* spy_or_trymark                   */
      BOp(spy_or_trymark, Otapl);
      LOCK(((PredEntry *)(PREG->u.Otapl.p))->PELock);
      PREG = (yamop *)(&(((PredEntry *)(PREG->u.Otapl.p))->OpcodeOfPred));
      UNLOCK(((PredEntry *)(PREG->u.Otapl.p))->PELock);
      goto dospy;
      ENDBOp();

      /* try_and_mark   Label,NArgs       */
      BOp(try_and_mark, Otapl);
      check_trail(TR);
#if defined(YAPOR) || defined(THREADS)
#ifdef YAPOR
      /* The flags I check here should never change during execution */
      CUT_wait_leftmost();
#endif /* YAPOR */
      if (PREG->u.Otapl.p->PredFlags & LogUpdatePredFlag) {
	LOCK(PREG->u.Otapl.p->PELock);
	PP = PREG->u.Otapl.p;
      }
      if (PREG->u.Otapl.p->CodeOfPred != PREG) {
	/* oops, someone changed the procedure under our feet,
	   fortunately this is no big deal because we haven't done
	   anything yet */
	PP = NULL;
	PREG = PREG->u.Otapl.p->CodeOfPred;
	UNLOCK(PREG->u.Otapl.p->PELock);
	/* for profiler */
	save_pc();
	JMPNext();
      }
#endif
      CACHE_Y(YREG);
      PREG = PREG->u.Otapl.d;
      /*
	I've got a read lock on the DB, so I don't need to care...
	 niaaahh.... niahhhh...
      */
      LOCK(DynamicLock(PREG));
      /* one can now mess around with the predicate */
      UNLOCK(((PredEntry *)(PREG->u.Otapl.p))->PELock);
      BEGD(d1);
      d1 = PREG->u.Otapl.s;
      store_args(d1);
      store_yaam_regs(PREG, 0);
      ENDD(d1);
      set_cut(S_YREG, B);
      B = B_YREG;
#ifdef YAPOR
      SCH_set_load(B_YREG);
#endif	/* YAPOR */
      SET_BB(B_YREG);
      ENDCACHE_Y();
#if defined(YAPOR) || defined(THREADS)
      INC_CLREF_COUNT(ClauseCodeToDynamicClause(PREG));
      UNLOCK(DynamicLock(PREG));
      TRAIL_CLREF(ClauseCodeToDynamicClause(PREG));
#else
      if (FlagOff(InUseMask, DynamicFlags(PREG))) {

	SetFlag(InUseMask, DynamicFlags(PREG));
	TRAIL_CLREF(ClauseCodeToDynamicClause(PREG));
      }
#endif
      PREG = NEXTOP(PREG,Otapl);
      JMPNext();

      ENDBOp();

      BOp(count_retry_and_mark, Otapl);
      RetriesCounter--;
      if (RetriesCounter == 0) {
	saveregs();
	Yap_Error(RETRY_COUNTER_UNDERFLOW,TermNil,"");
	setregs();
	JMPNext();
      } 
      PredEntriesCounter--;
      if (PredEntriesCounter == 0) {
	saveregs();
	Yap_Error(PRED_ENTRY_COUNTER_UNDERFLOW,TermNil,"");
	setregs();
	JMPNext();
      } 
      /* enter a retry dynamic */
      ENDBOp();

      BOp(profiled_retry_and_mark, Otapl);
      LOCK(((PredEntry *)(PREG->u.Otapl.p))->StatisticsForPred.lock);
      ((PredEntry *)(PREG->u.Otapl.p))->StatisticsForPred.NOfRetries++;
      UNLOCK(((PredEntry *)(PREG->u.Otapl.p))->StatisticsForPred.lock);
      /* enter a retry dynamic */
      ENDBOp();

      /* retry_and_mark   Label,NArgs     */
      BOp(retry_and_mark, Otapl);
#ifdef YAPOR
      CUT_wait_leftmost();
#endif /* YAPOR */
      /* need to make the DB stable until I get the new clause */
      LOCK(PREG->u.Otapl.p->PELock);
      CACHE_Y(B);
      PREG = PREG->u.Otapl.d;
      LOCK(DynamicLock(PREG));
      UNLOCK(PREG->u.Otapl.p->PELock);
      restore_yaam_regs(PREG);
      restore_args(PREG->u.Otapl.s);
#ifdef FROZEN_STACKS
      S_YREG = (CELL *) PROTECT_FROZEN_B(B_YREG);
      set_cut(S_YREG, B->cp_b);
#else
      set_cut(S_YREG, B_YREG->cp_b);
#endif /* FROZEN_STACKS */
      SET_BB(B_YREG);
      ENDCACHE_Y();
#if defined(YAPOR) || defined(THREADS)
      INC_CLREF_COUNT(ClauseCodeToDynamicClause(PREG));
      TRAIL_CLREF(ClauseCodeToDynamicClause(PREG));
      UNLOCK(DynamicLock(PREG));
#else
      if (FlagOff(InUseMask, DynamicFlags(PREG))) {

	SetFlag(InUseMask, DynamicFlags(PREG));
	TRAIL_CLREF(ClauseCodeToDynamicClause(PREG));
      }
#endif
      PREG = NEXTOP(PREG, Otapl);
      JMPNext();

      ENDBOp();

/*****************************************************************
*        Failure                                                 *
*****************************************************************/

      /* trust_fail                       */
      BOp(trust_fail, e);
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
#endif	/* YAPOR */
      goto fail;
      ENDBOp();

#ifdef YAPOR
    shared_fail:
      B = Get_LOCAL_top_cp();
      SET_BB(PROTECT_FROZEN_B(B));
      goto fail;
#endif	/* YAPOR */

      /* fail                             */
      PBOp(op_fail, e);

      if (PP) {
	UNLOCK(PP->PELock);
	PP = NULL;
      }      
#ifdef COROUTINING
      CACHE_Y_AS_ENV(YREG);
      check_stack(NoStackFail, H);
      ENDCACHE_Y_AS_ENV();
#endif

    fail:
      {
	register tr_fr_ptr pt0 = TR;
#if defined(YAPOR) || defined(THREADS)
	if (PP) {
	  UNLOCK(PP->PELock);
	  PP = NULL;
	}
#endif
	PREG = B->cp_ap;
	save_pc();
	CACHE_TR(B->cp_tr);
	PREFETCH_OP(PREG);
      failloop:
	if (pt0 == S_TR) {
	  SP = SP0;
#ifdef LOW_LEVEL_TRACER
	  if (Yap_do_low_level_trace) {
	    int go_on = TRUE;
	    yamop *ipc = PREG;

	    while (go_on) {
	      op_numbers opnum = Yap_op_from_opcode(ipc->opc);

	      go_on = FALSE;
	      switch (opnum) {
#ifdef TABLING
	      case _table_load_answer:
		low_level_trace(retry_table_loader, LOAD_CP(B)->cp_pred_entry, NULL);
		break;
	      case _table_try_answer:
	      case _table_retry_me:
	      case _table_trust_me:
	      case _table_retry:
	      case _table_trust:
	      case _table_completion:
#ifdef DETERMINISTIC_TABLING
		if (IS_DET_GEN_CP(B))
		  low_level_trace(retry_table_generator, DET_GEN_CP(B)->cp_pred_entry, NULL);
		else
#endif /* DETERMINISTIC_TABLING */
		  low_level_trace(retry_table_generator, GEN_CP(B)->cp_pred_entry, (CELL *)(GEN_CP(B) + 1));
		break;
	      case _table_answer_resolution:
		low_level_trace(retry_table_consumer, CONS_CP(B)->cp_pred_entry, NULL);
		break;
	      case _trie_trust_null:
	      case _trie_retry_null:
	      case _trie_trust_null_in_new_pair:
	      case _trie_retry_null_in_new_pair:
	      case _trie_trust_var:
	      case _trie_retry_var:
	      case _trie_trust_var_in_new_pair:
	      case _trie_retry_var_in_new_pair:
	      case _trie_trust_val:
	      case _trie_retry_val:
	      case _trie_trust_val_in_new_pair:
	      case _trie_retry_val_in_new_pair:
	      case _trie_trust_atom:
	      case _trie_retry_atom:
	      case _trie_trust_atom_in_new_pair:
	      case _trie_retry_atom_in_new_pair:
	      case _trie_trust_pair:
	      case _trie_retry_pair:
	      case _trie_trust_struct:
	      case _trie_retry_struct:
	      case _trie_trust_struct_in_new_pair:
	      case _trie_retry_struct_in_new_pair:
	      case _trie_trust_extension:
	      case _trie_retry_extension:
	      case _trie_trust_float:
	      case _trie_retry_float:
	      case _trie_trust_long:
	      case _trie_retry_long:
        case _trie_do_hash:
        case _trie_retry_hash:
		low_level_trace(retry_table_loader, UndefCode, NULL);
		break;
#endif /* TABLING */
	      case _or_else:
	      case _or_last:
		low_level_trace(retry_or, (PredEntry *)ipc, &(B->cp_a1));
		break;
	      case _retry2:
	      case _retry3:
	      case _retry4:
		ipc = NEXTOP(ipc,l);
		go_on = TRUE;
		break;
	      case _jump:
		ipc = ipc->u.l.l;
		go_on = TRUE;
		break;
	      case _retry_c:
	      case _retry_userc:
		low_level_trace(retry_pred, ipc->u.OtapFs.p, B->cp_args);
		break;
	      case _retry_profiled:
	      case _count_retry:
		ipc = NEXTOP(ipc,p);
		go_on = TRUE;
		break;
	      case _retry_me:
	      case _trust_me:
	      case _count_retry_me:
	      case _count_trust_me:
	      case _profiled_retry_me:
	      case _profiled_trust_me:
	      case _retry_and_mark:
	      case _profiled_retry_and_mark:
	      case _retry:
	      case _trust:
		low_level_trace(retry_pred, ipc->u.Otapl.p, B->cp_args);
		break;
	      case _try_logical:
	      case _retry_logical:
	      case _profiled_retry_logical:
	      case _count_retry_logical:
	      case _trust_logical:
	      case _profiled_trust_logical:
	      case _count_trust_logical:
		low_level_trace(retry_pred, ipc->u.OtILl.d->ClPred, B->cp_args);
		break;
	      case _Nstop:
	      case _Ystop:
		low_level_trace(retry_pred, NULL, B->cp_args);
		break;
	      default:
		break;
	      }
	    }
	  }
#endif	/* LOW_LEVEL_TRACER */
#ifdef FROZEN_STACKS
#ifdef SBA
	  if (pt0 < TR_FZ || pt0 > (tr_fr_ptr)Yap_TrailTop)
#else
	  if (pt0 < TR_FZ)
#endif /* SBA */
	    {
	      TR = TR_FZ;
	      TRAIL_LINK(pt0);
	    } else
#endif /* FROZEN_STACKS */
	  RESTORE_TR();
	  GONext();
	}
	BEGD(d1);
	d1 = TrailTerm(pt0-1);
	pt0--;
	if (IsVarTerm(d1)) {
#if defined(SBA) && defined(YAPOR)
	  /* clean up the trail when we backtrack */
	  if (Unsigned((Int)(d1)-(Int)(H_FZ)) >
	      Unsigned((Int)(B_FZ)-(Int)(H_FZ))) {
	    RESET_VARIABLE(STACK_TO_SBA(d1));
	  } else
#endif
	    /* normal variable */
	    RESET_VARIABLE(d1);
	  goto failloop;
	}
	/* pointer to code space */
	/* or updatable variable */
#if defined(TERM_EXTENSIONS) || defined(FROZEN_STACKS) || defined(MULTI_ASSIGNMENT_VARIABLES)
	if (IsPairTerm(d1))
#endif /* TERM_EXTENSIONS || FROZEN_STACKS || MULTI_ASSIGNMENT_VARIABLES */
	  {
	    register CELL flags;
	    CELL *pt1 = RepPair(d1);
#ifdef LIMIT_TABLING
	    if ((ADDR) pt1 == Yap_TrailBase) {
	      sg_fr_ptr sg_fr = (sg_fr_ptr) TrailVal(pt0);
	      TrailTerm(pt0) = AbsPair((CELL *)(pt0 - 1));
	      SgFr_state(sg_fr)--;  /* complete_in_use --> complete : compiled_in_use --> compiled */
	      insert_into_global_sg_fr_list(sg_fr);
	      goto failloop;
	    }
#endif /* LIMIT_TABLING */
#ifdef FROZEN_STACKS  /* TRAIL */
            /* avoid frozen segments */
	    if (
#ifdef SBA
		(ADDR) pt1 >= HeapTop
#else
		IN_BETWEEN(Yap_TrailBase, pt1, Yap_TrailTop)
#endif /* SBA */
		)
            {
	      pt0 = (tr_fr_ptr) pt1;
	      goto failloop;
	    } else
#endif /* FROZEN_STACKS */
	      if (IN_BETWEEN(Yap_GlobalBase, pt1, H0))
		goto failloop;		       	    
	    flags = *pt1;
#if defined(YAPOR) || defined(THREADS)
	    if (FlagOn(DBClMask, flags)) {
	      DBRef dbr = DBStructFlagsToDBStruct(pt1);
	      int erase;

	      LOCK(dbr->lock);
	      DEC_DBREF_COUNT(dbr);
	      erase = (dbr->Flags & ErasedMask) && (dbr->ref_count == 0);
	      UNLOCK(dbr->lock);
	      if (erase) {
		saveregs();
		Yap_ErDBE(dbr);
		setregs();
	      }
	    } else {
	      if (flags & LogUpdMask) {
		if (flags & IndexMask) {
		  LogUpdIndex *cl = ClauseFlagsToLogUpdIndex(pt1);
		  int erase;
		  PredEntry *ap = cl->ClPred;

		  LOCK(ap->PELock);
		  DEC_CLREF_COUNT(cl);
		  erase = (cl->ClFlags & ErasedMask) && !(cl->ClRefCount);
		  if (erase) {
		    saveregs();
		    /* at this point, 
		       we are the only ones accessing the clause,
		       hence we don't need to have a lock it */
		    Yap_ErLogUpdIndex(cl);
		    setregs();
		  } else if (cl->ClFlags & DirtyMask) {
		    saveregs();
		    /* at this point, 
		       we are the only ones accessing the clause,
		       hence we don't need to have a lock it */
		    Yap_CleanUpIndex(cl);
		    setregs();
		  }
		  UNLOCK(ap->PELock);
		} else {
		  LogUpdClause *cl = ClauseFlagsToLogUpdClause(pt1);
		  int erase;
		  PredEntry *ap = cl->ClPred;

		  LOCK(ap->PELock);
		  DEC_CLREF_COUNT(cl);
		  erase = (cl->ClFlags & ErasedMask) && !(cl->ClRefCount);
		  if (erase) {
		    saveregs();
		    /* at this point, 
		       we are the only ones accessing the clause,
		       hence we don't need to have a lock it */
		    Yap_ErLogUpdCl(cl);
		    setregs();
		  }
		  UNLOCK(ap->PELock);
		}
	      } else {
		DynamicClause *cl = ClauseFlagsToDynamicClause(pt1);
		int erase;
		  
		LOCK(cl->ClLock);
		DEC_CLREF_COUNT(cl);
		erase = (cl->ClFlags & ErasedMask) && !(cl->ClRefCount);
		UNLOCK(cl->ClLock);
		if (erase) {
		  saveregs();
		  /* at this point, 
		     we are the only ones accessing the clause,
		     hence we don't need to have a lock it */
		  Yap_ErCl(cl);
		  setregs();
		}
	      }
	    }
#else
	    ResetFlag(InUseMask, flags);
	    *pt1 = flags;
	    if (FlagOn((ErasedMask|DirtyMask), flags)) {
	      if (FlagOn(DBClMask, flags)) {
		saveregs();
		Yap_ErDBE(DBStructFlagsToDBStruct(pt1));
		setregs();
	      } else {
		saveregs();
		if (flags & LogUpdMask) {
		  if (flags & IndexMask) {
		    if (FlagOn(ErasedMask, flags)) {
		      Yap_ErLogUpdIndex(ClauseFlagsToLogUpdIndex(pt1));
		    } else {
		      Yap_CleanUpIndex(ClauseFlagsToLogUpdIndex(pt1));
		    }
		  } else {
		    Yap_ErLogUpdCl(ClauseFlagsToLogUpdClause(pt1));
		  }
		} else {
		  Yap_ErCl(ClauseFlagsToDynamicClause(pt1));
		}
		setregs();
	      }
	    }
#endif
	    goto failloop;
	  }
#ifdef MULTI_ASSIGNMENT_VARIABLES
	else /* if (IsApplTerm(d1)) */ {
	  CELL *pt = RepAppl(d1);
	  /* AbsAppl means */
	  /* multi-assignment variable */
	  /* so the next cell is the old value */ 
#ifdef FROZEN_STACKS
	  --pt0;
	  pt[0] = TrailVal(pt0);
#else
	  pt[0] = TrailTerm(pt0-1);
	  pt0 -= 2;
#endif /* FROZEN_STACKS */
	  goto failloop;
	}
#endif
	ENDD(d1);
	ENDCACHE_TR();
      }
      ENDPBOp();



/************************************************************************\
*	Cut & Commit Instructions					*
\************************************************************************/

      /* cut                              */
      Op(cut, e);
      PREG = NEXTOP(PREG, e);
      {
	choiceptr d0;
	/* assume cut is always in stack */
	d0 = (choiceptr)YREG[E_CB];
#ifdef CUT_C
	{
	  if (SHOULD_CUT_UP_TO(B,d0))
	  {
	    while (POP_CHOICE_POINT(d0))
	      {
		POP_EXECUTE();
	      }
	  }
	}
#endif /* CUT_C */
#ifdef YAPOR
	CUT_prune_to(d0);
#endif /* YAPOR */
	if (SHOULD_CUT_UP_TO(B,d0)) {
	  /* cut ! */
	  while (B->cp_b < d0) {
	    B = B->cp_b;
	  }
#ifdef TABLING
	  abolish_incomplete_subgoals(B);
#endif /* TABLING */
	trim_trail:
	  HBREG = PROTECT_FROZEN_H(B->cp_b);
#include "trim_trail.h"
	  B = B->cp_b;
	  SET_BB(PROTECT_FROZEN_B(B));
	}
      }
      GONext();
      ENDOp();

      /* cut_t                            */
      /* cut_t does the same as cut */
      Op(cut_t, e);
      PREG = NEXTOP(PREG, e);
      {
	choiceptr d0;

	/* assume cut is always in stack */
	d0 = (choiceptr)YREG[E_CB];
#ifdef CUT_C
	{
	  if (SHOULD_CUT_UP_TO(B,d0))
	    {
	      while (POP_CHOICE_POINT(d0))
		{
		  POP_EXECUTE();
		}
	    }
	}
#endif /* CUT_C */
#ifdef YAPOR
	CUT_prune_to(d0);
#endif	/* YAPOR */
	if (SHOULD_CUT_UP_TO(B,d0)) {
	  /* cut ! */
	  while (B->cp_b < d0) {
	    B = B->cp_b;
	  }
#ifdef TABLING
	  abolish_incomplete_subgoals(B);
#endif /* TABLING */
#ifdef FROZEN_STACKS
	  { 
	    choiceptr top_b = PROTECT_FROZEN_B(B->cp_b);
#ifdef SBA
	    if (ENV > (CELL *) top_b || ENV < H) YREG = (CELL *) top_b;
#else
	    if (ENV > (CELL *) top_b) YREG = (CELL *) top_b;
#endif /* SBA */
	    else YREG = (CELL *)((CELL)ENV + ENV_Size(CPREG));
	  }
#else
	  if (ENV > (CELL *)B->cp_b) {
	    YREG = (CELL *)B->cp_b;
	  }
	  else {
	    YREG = (CELL *) ((CELL) ENV + ENV_Size(CPREG));
	  }
#endif /* FROZEN_STACKS */
	  YREG[E_CB] = (CELL)d0;
	  goto trim_trail;
	}
      }
      GONext();
      ENDOp();

      /* cut_e                            */
      Op(cut_e, e);
      PREG = NEXTOP(PREG, e);
      {
	choiceptr d0;
	/* we assume dealloc leaves in S the previous env             */
	d0 = (choiceptr)SREG[E_CB];
#ifdef CUT_C
	{
	  if (SHOULD_CUT_UP_TO(B,d0))
	    {
	      while (POP_CHOICE_POINT(d0))
		{
		  POP_EXECUTE();
		}
	    }
	}
#endif /* CUT_C */
#ifdef YAPOR
	CUT_prune_to(d0);
#endif	/* YAPOR */
	if (SHOULD_CUT_UP_TO(B,d0)) {
	/* cut ! */
	  while (B->cp_b < d0) {
	    B = B->cp_b;
	  }
#ifdef TABLING
	  abolish_incomplete_subgoals(B);
#endif /* TABLING */
	  goto trim_trail;
	}
      }
      GONext();
      ENDOp();

      /* save_b_x      Xi                 */
      Op(save_b_x, x);
      BEGD(d0);
      d0 = PREG->u.x.x;
#if defined(SBA) && defined(FROZEN_STACKS)
      XREG(d0) = MkIntegerTerm((Int)B);
#else
      XREG(d0) = MkIntegerTerm(LCL0-(CELL *) (B));
#endif /* SBA && FROZEN_STACKS */
      PREG = NEXTOP(PREG, x);
      ENDD(d0);
      GONext();
      ENDOp();

      /* save_b_y      Yi                 */
      Op(save_b_y, y);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(YREG+PREG->u.y.y,MkIntegerTerm((Int)B));
#else
      YREG[PREG->u.y.y] = MkIntegerTerm(LCL0-(CELL *) (B));
#endif /* SBA && FROZEN_STACKS */
      PREG = NEXTOP(PREG, y);
      GONext();
      ENDOp();

      /* commit_b_x    Xi                 */
      Op(commit_b_x, xp);
      BEGD(d0);
      d0 = XREG(PREG->u.xp.x);
#ifdef COROUTINING
      CACHE_Y_AS_ENV(YREG);
      check_stack(NoStackCommitX, H);
      ENDCACHE_Y_AS_ENV();
    do_commit_b_x:
#endif
      /* skip a void call and a label */
      PREG = NEXTOP(NEXTOP(NEXTOP(PREG, xp),Osbpp),l);
      {
	choiceptr pt0;
#if defined(SBA) && defined(FROZEN_STACKS)
	pt0 = (choiceptr)IntegerOfTerm(d0);
#else
	pt0 = (choiceptr)(LCL0-IntegerOfTerm(d0));
#endif /* SBA && FROZEN_STACKS */
#ifdef CUT_C
      {
	if (SHOULD_CUT_UP_TO(B,(choiceptr) pt0))
	  {
	    while (POP_CHOICE_POINT(pt0))
	      {
		POP_EXECUTE();
	      }
	  }
      }
#endif /* CUT_C */
#ifdef YAPOR
	CUT_prune_to(pt0);
#endif	/* YAPOR */
	if (SHOULD_CUT_UP_TO(B,pt0)) {
	  while (B->cp_b < pt0) {
	    B = B->cp_b;
	  }
#ifdef TABLING
	  abolish_incomplete_subgoals(B);
#endif /* TABLING */
	  goto trim_trail;
	}
      }
      ENDD(d0);
      GONext();
      ENDOp();

      /* commit_b_y    Yi                 */
      Op(commit_b_y, yp);
      BEGD(d0);
      d0 = YREG[PREG->u.yp.y];
#ifdef COROUTINING
      CACHE_Y_AS_ENV(YREG);
      check_stack(NoStackCommitY, H);
      ENDCACHE_Y_AS_ENV();
    do_commit_b_y:
#endif
      PREG = NEXTOP(NEXTOP(NEXTOP(PREG, yp),Osbpp),l);
      {
	choiceptr pt0;
#if defined(SBA) && defined(FROZEN_STACKS)
	pt0 = (choiceptr)IntegerOfTerm(d0);
#else
	pt0 = (choiceptr)(LCL0-IntegerOfTerm(d0));
#endif /* SBA && FROZEN_STACKS */
#ifdef CUT_C
	{
	  if (SHOULD_CUT_UP_TO(B,(choiceptr) pt0))
	    {
	      while (POP_CHOICE_POINT(pt0))
		{
		  POP_EXECUTE();
		}
	    }
	}
#endif /* CUT_C */
#ifdef YAPOR
	CUT_prune_to(pt0);
#endif	/* YAPOR */
	if (SHOULD_CUT_UP_TO(B,pt0)) {
	  while (B->cp_b < pt0) {
	    B = B->cp_b;
	  }
#ifdef TABLING
	  abolish_incomplete_subgoals(B);
#endif /* TABLING */
	  goto trim_trail;
	}
      }
      ENDD(d0);
      GONext();
      ENDOp();

/*************************************************************************
* 	Call / Proceed instructions                                      *
*************************************************************************/

/* Macros for stack trimming                                            */

      /* execute     Label               */
      BOp(execute, pp);
      {
	PredEntry *pt0;
	CACHE_Y_AS_ENV(YREG);
	pt0 = PREG->u.pp.p;
#ifdef LOW_LEVEL_TRACER
	if (Yap_do_low_level_trace) {
	  low_level_trace(enter_pred,pt0,XREGS+1);
	}
#endif	/* LOW_LEVEL_TRACE */
	CACHE_A1();
	ALWAYS_LOOKAHEAD(pt0->OpcodeOfPred);
	BEGD(d0);
	d0 = (CELL)B;
#ifndef NO_CHECKING
	check_stack(NoStackExecute, H);
#endif
	PREG = pt0->CodeOfPred;
	/* for profiler */
	save_pc();
	ENV_YREG[E_CB] = d0;
	ENDD(d0);
#ifdef DEPTH_LIMIT
	if (DEPTH <= MkIntTerm(1)) {/* I assume Module==0 is prolog */
	  if (pt0->ModuleOfPred) {
	    if (DEPTH == MkIntTerm(0))
	      FAIL();
	    else DEPTH = RESET_DEPTH();
	  }
	} else if (pt0->ModuleOfPred)
	  DEPTH -= MkIntConstant(2);
#endif	/* DEPTH_LIMIT */
      /* this is the equivalent to setting up the stack */
	ALWAYS_GONext();
	ALWAYS_END_PREFETCH();
	ENDCACHE_Y_AS_ENV();
      }
      ENDBOp();

    NoStackExecute:
      SREG = (CELL *) PREG->u.pp.p;
      PP = PREG->u.pp.p0;
      if (ActiveSignals & YAP_CDOVF_SIGNAL) {
	ASP = YREG+E_CB;
	if (ASP > (CELL *)PROTECT_FROZEN_B(B))
	  ASP = (CELL *)PROTECT_FROZEN_B(B);
	goto noheapleft;
      }
      if (ActiveSignals)
	goto creep;
      else
	goto NoStackExec;

      /* dexecute    Label               */
      /* joint deallocate and execute */
      BOp(dexecute, pp);
#ifdef LOW_LEVEL_TRACER
      if (Yap_do_low_level_trace)
	low_level_trace(enter_pred,PREG->u.pp.p,XREGS+1);
#endif	/* LOW_LEVEL_TRACER */ 
     CACHE_Y_AS_ENV(YREG);
      {
	PredEntry *pt0;

	CACHE_A1();
	pt0 = PREG->u.pp.p;
#ifndef NO_CHECKING
	/* check stacks */
	check_stack(NoStackDExecute, H);
#endif
#ifdef DEPTH_LIMIT
	if (DEPTH <= MkIntTerm(1)) {/* I assume Module==0 is primitives */
	  if (pt0->ModuleOfPred) {
	    if (DEPTH == MkIntTerm(0))
	      FAIL();
	    else DEPTH = RESET_DEPTH();
	  }
	} else if (pt0->ModuleOfPred)
	  DEPTH -= MkIntConstant(2);
#endif	/* DEPTH_LIMIT */
	PREG = pt0->CodeOfPred;
	/* for profiler */
	save_pc();
	ALWAYS_LOOKAHEAD(pt0->OpcodeOfPred);
	/* do deallocate */
	CPREG = (yamop *) ENV_YREG[E_CP];
	ENV_YREG = ENV = (CELL *) ENV_YREG[E_E];
#ifdef FROZEN_STACKS
	{ 
	  choiceptr top_b = PROTECT_FROZEN_B(B);
#ifdef SBA
	  if (ENV_YREG > (CELL *) top_b || ENV_YREG < H) ENV_YREG = (CELL *) top_b;
#else
	  if (ENV_YREG > (CELL *) top_b) ENV_YREG = (CELL *) top_b;
#endif /* SBA */
	  else ENV_YREG = (CELL *)((CELL)ENV_YREG + ENV_Size(CPREG));
	}
#else
	if (ENV_YREG > (CELL *)B) {
	  ENV_YREG = (CELL *)B;
	}
	else {
	  ENV_YREG = (CELL *) ((CELL) ENV_YREG + ENV_Size(CPREG));
	}
#endif /* FROZEN_STACKS */
	WRITEBACK_Y_AS_ENV();
	/* setup GB */
	ENV_YREG[E_CB] = (CELL) B;
	ALWAYS_GONext();
	ALWAYS_END_PREFETCH();
      }
      ENDCACHE_Y_AS_ENV();
      ENDBOp();

      BOp(fcall, Osbpp);
      CACHE_Y_AS_ENV(YREG);
      ENV_YREG[E_CP] = (CELL) CPREG;
      ENV_YREG[E_E] = (CELL) ENV;
#ifdef DEPTH_LIMIT
      ENV_YREG[E_DEPTH] = DEPTH;
#endif	/* DEPTH_LIMIT */
      ENDCACHE_Y_AS_ENV();
      ENDBOp();

      BOp(call, Osbpp);
#ifdef LOW_LEVEL_TRACER
      if (Yap_do_low_level_trace) {
	low_level_trace(enter_pred,PREG->u.Osbpp.p,XREGS+1);
      }
#endif	/* LOW_LEVEL_TRACER */
      CACHE_Y_AS_ENV(YREG);
      {
	PredEntry *pt;
	pt = PREG->u.Osbpp.p;
	CACHE_A1();
#ifndef NO_CHECKING
	check_stack(NoStackCall, H);
#endif
	ENV = ENV_YREG;
	/* Try to preserve the environment */
	ENV_YREG = (CELL *) (((char *) ENV_YREG) + PREG->u.Osbpp.s);
	CPREG = NEXTOP(PREG, Osbpp);
	ALWAYS_LOOKAHEAD(pt->OpcodeOfPred);
	PREG = pt->CodeOfPred;
	/* for profiler */
	save_pc();
#ifdef DEPTH_LIMIT
	if (DEPTH <= MkIntTerm(1)) {/* I assume Module==0 is primitives */
	  if (pt->ModuleOfPred) {
	    if (DEPTH == MkIntTerm(0))
	      FAIL();
	    else DEPTH = RESET_DEPTH();
	  }
	} else if (pt->ModuleOfPred)
	  DEPTH -= MkIntConstant(2);
#endif	/* DEPTH_LIMIT */
#ifdef FROZEN_STACKS
	{ 
	  choiceptr top_b = PROTECT_FROZEN_B(B);
#ifdef SBA
	  if (ENV_YREG > (CELL *) top_b || ENV_YREG < H) ENV_YREG = (CELL *) top_b;
#else
	  if (ENV_YREG > (CELL *) top_b) ENV_YREG = (CELL *) top_b;
#endif /* SBA */
	}
#else
	if (ENV_YREG > (CELL *) B) {
	  ENV_YREG = (CELL *) B;
	}
#endif /* FROZEN_STACKS */
	WRITEBACK_Y_AS_ENV();
	/* setup GB */
	ENV_YREG[E_CB] = (CELL) B;
#ifdef YAPOR
	SCH_check_requests();
#endif	/* YAPOR */
	ALWAYS_GONext();
	ALWAYS_END_PREFETCH();
      }
      ENDCACHE_Y_AS_ENV();
      ENDBOp();

    NoStackCall:
      PP = PREG->u.Osbpp.p0;
      /* on X86 machines S will not actually be holding the pointer to pred */
      if (ActiveSignals & YAP_CREEP_SIGNAL) {
	PredEntry *ap = PREG->u.Osbpp.p;
	SREG = (CELL *) ap;
	goto creepc;
      }
      SREG = (CELL *) PREG->u.Osbpp.p;
      if (ActiveSignals & YAP_CDOVF_SIGNAL) {
	ASP = (CELL *) (((char *) YREG) + PREG->u.Osbpp.s);
	if (ASP > (CELL *)PROTECT_FROZEN_B(B))
	  ASP = (CELL *)PROTECT_FROZEN_B(B);
	goto noheapleft;
      }
      if (ActiveSignals) {
	goto creepc;
      }
      ASP = (CELL *) (((char *) YREG) + PREG->u.Osbpp.s);
      if (ASP > (CELL *)PROTECT_FROZEN_B(B))
	ASP = (CELL *)PROTECT_FROZEN_B(B);
      saveregs();
      if (!Yap_gc(((PredEntry *)SREG)->ArityOfPE, YREG, NEXTOP(PREG, Osbpp))) {
	Yap_Error(OUT_OF_STACK_ERROR,TermNil,Yap_ErrorMessage);
      }
      setregs();

      JMPNext();


      /* don't forget I cannot creep at deallocate (where to?) */
      /* also, this is unusual in that I have already done deallocate,
	 so I don't need to redo it.
       */ 
    NoStackDeallocate:
      {
	CELL cut_b = LCL0-(CELL *)(SREG[E_CB]);

	/* 
	   don't do a creep here; also, if our instruction is followed by
	   a execute_c, just wait a bit more */
	if (ActiveSignals & YAP_CREEP_SIGNAL ||
	    (PREG->opc != Yap_opcode(_procceed) &&
	     PREG->opc != Yap_opcode(_cut_e))) {
	  GONext();
	}
	PP = PREG->u.p.p;
	ASP = YREG+E_CB;
	/* cut_e */
	if (SREG <= ASP) {
	  ASP = SREG-EnvSizeInCells;
	}
	if (ActiveSignals & YAP_CDOVF_SIGNAL) {
	  goto noheapleft;
	}
	if (ActiveSignals) {
	  if (Yap_op_from_opcode(PREG->opc) == _cut_e) {
	    /* followed by a cut */
	    ARG1 = MkIntegerTerm(LCL0-(CELL *)SREG[E_CB]);
	    SREG = (CELL *)RepPredProp(Yap_GetPredPropByFunc(FunctorCutBy,1));
	  } else {
	    SREG = (CELL *)RepPredProp(Yap_GetPredPropByAtom(AtomTrue,0));
	  }
	  goto creep;
	}
	saveregs();
	if (!Yap_gc(0, ENV, CPREG)) {
	  Yap_Error(OUT_OF_STACK_ERROR,TermNil,Yap_ErrorMessage);
	}
	setregs();
	SREG = ASP;
	SREG[E_CB] = (CELL)(LCL0-cut_b);
      }
      JMPNext();

#ifdef COROUTINING

     /* This is easier: I know there is an environment so I cannot do allocate */
    NoStackCommitY:
      PP = PREG->u.yp.p0;
      /* find something to fool S */
      if (!ActiveSignals || ActiveSignals & YAP_CDOVF_SIGNAL) {
	goto do_commit_b_y;
      }
      if (!(ActiveSignals & YAP_CREEP_SIGNAL)) {
	SREG = (CELL *)RepPredProp(Yap_GetPredPropByFunc(FunctorRestoreRegs,0));
	XREGS[0] = YREG[PREG->u.yp.y];
	PREG = NEXTOP(PREG,yp);
	goto creep_either;
      }
      /* don't do debugging and friends here */
      goto do_commit_b_y;

      /* Problem: have I got an environment or not? */
    NoStackCommitX:
      PP = PREG->u.xp.p0;
      /* find something to fool S */
      if (!ActiveSignals || ActiveSignals & YAP_CDOVF_SIGNAL) {
	goto do_commit_b_x;
      }
      if (!(ActiveSignals & YAP_CREEP_SIGNAL)) {
	SREG = (CELL *)RepPredProp(Yap_GetPredPropByFunc(FunctorRestoreRegs,0));
#if USE_THREADED_CODE
	if (PREG->opc == (OPCODE)OpAddress[_fcall])
#else
	  if (PREG->opc == _fcall)
#endif
	    {
	      /* fill it up */
	      CACHE_Y_AS_ENV(YREG);
	      ENV_YREG[E_CP] = (CELL) CPREG;
	      ENV_YREG[E_E] = (CELL) ENV;
#ifdef DEPTH_LIMIT
	      ENV_YREG[E_DEPTH] = DEPTH;
#endif	/* DEPTH_LIMIT */
	      ENDCACHE_Y_AS_ENV();
	    }
	XREGS[0] = XREG(PREG->u.xp.x);
	PREG = NEXTOP(PREG,xp);
	goto creep_either;
      }
      /* don't do debugging and friends here */
      goto do_commit_b_x;

      /* Problem: have I got an environment or not? */
    NoStackFail:
      /* find something to fool S */
      if (!ActiveSignals || ActiveSignals & YAP_CDOVF_SIGNAL) {
	goto fail;
      }
      if (!(ActiveSignals & YAP_CREEP_SIGNAL)) {
	SREG = (CELL *)RepPredProp(Yap_GetPredPropByAtom(AtomFail,0));
	/* make sure we have the correct environment for continuation */
	ENV = B->cp_env;
	YREG  = (CELL *)B;
	goto creep;
      }
      /* don't do debugging and friends here */
      goto fail;

      /* don't forget I cannot creep at ; */
    NoStackEither:
      PP = PREG->u.Osblp.p0;
      if (ActiveSignals & YAP_CREEP_SIGNAL) {
	goto either_notest;
      }
      /* find something to fool S */
      SREG = (CELL *)RepPredProp(Yap_GetPredPropByFunc(FunctorRestoreRegs1,0));
      if (ActiveSignals & YAP_CDOVF_SIGNAL) {
	ASP = (CELL *) (((char *) YREG) + PREG->u.Osbpp.s);
	if (ASP > (CELL *)PROTECT_FROZEN_B(B))
	  ASP = (CELL *)PROTECT_FROZEN_B(B);
	goto noheapleft;
      }
      if (ActiveSignals) {
	goto creep_either;
      }
      ASP = (CELL *) (((char *) YREG) + PREG->u.Osbpp.s);
      if (ASP > (CELL *)PROTECT_FROZEN_B(B))
	ASP = (CELL *)PROTECT_FROZEN_B(B);
      saveregs();
      if (!Yap_gc(0, YREG, NEXTOP(PREG, Osbpp))) {
	Yap_Error(OUT_OF_STACK_ERROR,TermNil,Yap_ErrorMessage);
      }
      setregs();
      JMPNext();

    creep_either:			/* do creep in either      */
      ENV = YREG;
      CPREG = NEXTOP(PREG, Osbpp);
      YREG = (CELL *) (((char *) YREG) + PREG->u.Osbpp.s);
#ifdef FROZEN_STACKS
      { 
	choiceptr top_b = PROTECT_FROZEN_B(B);
#ifdef SBA
	if (YREG > (CELL *) top_b || YREG < H) YREG = (CELL *) top_b;
#else
	if (YREG > (CELL *) top_b) YREG = (CELL *) top_b;
#endif /* SBA */
	else YREG = YREG + ENV_Size(CPREG);
      }
#else
      if (YREG > (CELL *) B)
	YREG = (CELL *) B;
#endif /* FROZEN_STACKS */
      /* setup GB */
      ARG1 = push_live_regs(CPREG);
      /* ARG0 has an extra argument for suspended cuts */
      ARG2 = XREGS[0];
      YREG[E_CB] = (CELL) B;
      goto creep;
#endif

    creepc:			/* do creep in call                                     */
      ENV = YREG;
      CPREG = NEXTOP(PREG, Osbpp);
      YREG = (CELL *) (((char *) YREG) + PREG->u.Osbpp.s);
#ifdef FROZEN_STACKS
      { 
	choiceptr top_b = PROTECT_FROZEN_B(B);
#ifdef SBA
	if (YREG > (CELL *) top_b || YREG < H) YREG = (CELL *) top_b;
#else
	if (YREG > (CELL *) top_b) YREG = (CELL *) top_b;
#endif /* SBA */
	else YREG = YREG + ENV_Size(CPREG);
      }
#else
      if (YREG > (CELL *) B)
	YREG = (CELL *) B;
      else
	/* I am not sure about this */
	YREG = YREG + ENV_Size(CPREG);
#endif /* FROZEN_STACKS */
      /* setup GB */
      YREG[E_CB] = (CELL) B;
      goto creep;

    NoStackDExecute:
      PP = PREG->u.pp.p0;
      if (ActiveSignals & YAP_CREEP_SIGNAL) {
	PredEntry *ap = PREG->u.pp.p;

	if (ap->PredFlags & HiddenPredFlag) {
	  CACHE_Y_AS_ENV(YREG);
	  CACHE_A1();
	  check_depth(DEPTH, ap);
	  PREG = ap->CodeOfPred;
	  /* for profiler */
	  save_pc();
	  ALWAYS_LOOKAHEAD(ap->OpcodeOfPred);
	  /* do deallocate */
	  CPREG = (yamop *) ENV_YREG[E_CP];
	  ENV_YREG = ENV = (CELL *) ENV_YREG[E_E];
#ifdef FROZEN_STACKS
	  { 
	    choiceptr top_b = PROTECT_FROZEN_B(B);

#ifdef SBA
	    if (ENV_YREG > (CELL *) top_b || ENV_YREG < H) ENV_YREG = (CELL *) top_b;
#else
	    if (ENV_YREG > (CELL *) top_b) ENV_YREG = (CELL *) top_b;
#endif
	    else ENV_YREG = (CELL *)((CELL)ENV_YREG + ENV_Size(CPREG));
	  }
#else
	  if (ENV_YREG > (CELL *)B) {
	    ENV_YREG = (CELL *)B;
	  } else {
	    ENV_YREG = (CELL *) ((CELL) ENV_YREG + ENV_Size(CPREG));
	  }
#endif /* FROZEN_STACKS */
	  WRITEBACK_Y_AS_ENV();
	  /* setup GB */
	  ENV_YREG[E_CB] = (CELL) B;
	  ALWAYS_GONext();
	  ALWAYS_END_PREFETCH();
	  ENDCACHE_Y_AS_ENV();
	} else {
	  SREG = (CELL *) ap;
	  goto creepde;
	}
      }
      /* set SREG for next instructions */
      SREG = (CELL *) PREG->u.p.p;
      if (ActiveSignals & YAP_CDOVF_SIGNAL) {
	ASP = YREG+E_CB;
	if (ASP > (CELL *)PROTECT_FROZEN_B(B))
	  ASP = (CELL *)PROTECT_FROZEN_B(B);
	goto noheapleft;
      }
      if (ActiveSignals)
	goto creepde;
      /* try performing garbage collection */

      ASP = YREG+E_CB;
      if (ASP > (CELL *)PROTECT_FROZEN_B(B))
	ASP = (CELL *)PROTECT_FROZEN_B(B);
      saveregs();
      if (!Yap_gc(((PredEntry *)(SREG))->ArityOfPE, (CELL *)YREG[E_E], (yamop *)YREG[E_CP])) {
	Yap_Error(OUT_OF_STACK_ERROR,TermNil,Yap_ErrorMessage);
      }
      setregs();
      /* hopefully, gc will succeeded, and we will retry
       * the instruction */
      JMPNext();

    NoStackExec:

      /* try performing garbage collection */

      ASP = YREG+E_CB;
      if (ASP > (CELL *)PROTECT_FROZEN_B(B))
	ASP = (CELL *)PROTECT_FROZEN_B(B);
      saveregs();
      if (!Yap_gc(((PredEntry *)(SREG))->ArityOfPE, ENV, CPREG)) {
	Yap_Error(OUT_OF_STACK_ERROR,TermNil,Yap_ErrorMessage);
      }
      setregs();
      /* hopefully, gc will succeeded, and we will retry
       * the instruction */
      JMPNext();

    creepde:
      /* first, deallocate */
      CPREG = (yamop *) YREG[E_CP];
      ENV = YREG = (CELL *) YREG[E_E];
#ifdef DEPTH_LIMIT
      YREG[E_DEPTH] = DEPTH;
#endif	/* DEPTH_LIMIT */
#ifdef FROZEN_STACKS
      { 
	choiceptr top_b = PROTECT_FROZEN_B(B);
#ifdef SBA
	if (YREG > (CELL *) top_b || YREG < H) YREG = (CELL *) top_b;
#else
	if (YREG > (CELL *) top_b) YREG = (CELL *) top_b;
#endif /* SBA */
	else YREG = (CELL *) ((CELL)YREG + ENV_Size(CPREG));
      }
#else
      if (YREG > (CELL *) B) {
	YREG = (CELL *) B;
      }
      else {
	YREG = (CELL *) ((CELL) YREG + ENV_Size(CPREG));
      }
#endif /* FROZEN_STACKS */
      /* setup GB */
      YREG[E_CB] = (CELL) B;

      /* and now CREEP */

    creep:
#if  defined(_MSC_VER) || defined(__MINGW32__)
	/* I need this for Windows and other systems where SIGINT
	   is not proceesed by same thread as absmi */
      LOCK(SignalLock);
      if (Yap_PrologMode & (AbortMode|InterruptMode)) {
	CreepFlag = CalculateStackGap();
	UNLOCK(SignalLock);
	/* same instruction */
	if (Yap_PrologMode & InterruptMode) {
	  Yap_PrologMode &= ~InterruptMode;
	  ASP = YREG+E_CB;
	  if (ASP > (CELL *)PROTECT_FROZEN_B(B))
	    ASP = (CELL *)PROTECT_FROZEN_B(B);
	  saveregs();
	  Yap_ProcessSIGINT();
	  setregs();
	} 
	JMPNext();
      }
      UNLOCK(SignalLock);
#endif
#ifdef SHADOW_S
      S = SREG;
#endif
      /* tell whether we can creep or not, this is hard because we will
	 lose the info RSN
      */
      PP = creep_allowed((PredEntry*)SREG,PP);
      BEGD(d0);
      d0 = ((PredEntry *)(SREG))->ArityOfPE;
      if (d0 == 0) {
	H[1] = MkAtomTerm((Atom) ((PredEntry *)(SREG))->FunctorOfPred);
      }
      else {
	H[d0 + 2] = AbsAppl(H);
	*H = (CELL) ((PredEntry *)(SREG))->FunctorOfPred;
	H++;
	BEGP(pt1);
	pt1 = XREGS + 1;
	for (; d0 > 0; --d0) {
	  BEGD(d1);
	  BEGP(pt0);
	  pt0 = pt1;
	  d1 = *pt0;
	  deref_head(d1, creep_unk);
	creep_nonvar:
	  /* just copy it to the heap */
	  pt1++;
	  *H++ = d1;
	  continue;

	  derefa_body(d1, pt0, creep_unk, creep_nonvar);
	  if (pt0 <= H) {
	    /* variable is safe */
	    *H++ = (CELL)pt0;
	    pt1++;
	  } else {
	    /* bind it, in case it is a local variable */
	    d1 = Unsigned(H);
	    RESET_VARIABLE(H);
	    pt1++;
	    H += 1;
	    Bind_Local(pt0, d1);
	  }
	  ENDP(pt0);
	  ENDD(d1);
	}
	ENDP(pt1);
      }
      ENDD(d0);
      H[0] = Yap_Module_Name((PredEntry *)SREG);
      ARG1 = (Term) AbsPair(H);

      H += 2;
      LOCK(SignalLock);
#ifdef COROUTINING
      if (ActiveSignals & YAP_WAKEUP_SIGNAL) {
	CreepFlag = CalculateStackGap();
	ActiveSignals &= ~YAP_WAKEUP_SIGNAL;
	UNLOCK(SignalLock);
	ARG2 = Yap_ListOfWokenGoals();
	SREG = (CELL *) (WakeUpCode);
	/* no more goals to wake up */
	Yap_UpdateTimedVar(WokenGoals, TermNil);
      } else	
#endif
	{
	  CreepFlag = CalculateStackGap();
	  SREG = (CELL *) CreepCode;
	  UNLOCK(SignalLock);
	}
      PREG = ((PredEntry *)SREG)->CodeOfPred;
#ifdef LOW_LEVEL_TRACER
      if (Yap_do_low_level_trace)
	low_level_trace(enter_pred,(PredEntry *)(SREG),XREGS+1);
#endif	/* LOW_LEVEL_TRACE */
      /* for profiler */
      save_pc();
      CACHE_A1();
      JMPNext();

      BOp(procceed, p);
      CACHE_Y_AS_ENV(YREG);
      ALWAYS_LOOKAHEAD(CPREG->opc);
      PREG = CPREG;
      /* for profiler */
      save_pc();
      ENV_YREG = ENV;
#ifdef DEPTH_LIMIT
      DEPTH = ENV_YREG[E_DEPTH];
#endif
      WRITEBACK_Y_AS_ENV();
      ALWAYS_GONext();
      ALWAYS_END_PREFETCH();
      ENDCACHE_Y_AS_ENV();
      ENDBOp();

      Op(allocate, e);
      CACHE_Y_AS_ENV(YREG);
      PREG = NEXTOP(PREG, e);
      ENV_YREG[E_CP] = (CELL) CPREG;
      ENV_YREG[E_E] = (CELL) ENV;
#ifdef DEPTH_LIMIT
      ENV_YREG[E_DEPTH] = DEPTH;
#endif	/* DEPTH_LIMIT */
      ENV = ENV_YREG;
      ENDCACHE_Y_AS_ENV();
      GONext();
      ENDOp();

      Op(deallocate, p);
      CACHE_Y_AS_ENV(YREG);
      PREG = NEXTOP(PREG, p);
      /* other instructions do depend on S being set by deallocate
	 :-( */
      SREG = YREG;
      CPREG = (yamop *) ENV_YREG[E_CP];
      ENV = ENV_YREG = (CELL *) ENV_YREG[E_E];
#ifdef DEPTH_LIMIT
      DEPTH = ENV_YREG[E_DEPTH];
#endif	/* DEPTH_LIMIT */
#ifdef FROZEN_STACKS
      { 
	choiceptr top_b = PROTECT_FROZEN_B(B);
#ifdef SBA
	if (ENV_YREG > (CELL *) top_b || ENV_YREG < H) ENV_YREG = (CELL *) top_b;
#else
	if (ENV_YREG > (CELL *) top_b) ENV_YREG = (CELL *) top_b;
#endif /* SBA */
	else ENV_YREG = (CELL *)((CELL) ENV_YREG + ENV_Size(CPREG));
      }
#else
      if (ENV_YREG > (CELL *) B)
	ENV_YREG = (CELL *) B;
      else
	ENV_YREG = (CELL *) ((CELL) ENV_YREG + ENV_Size(CPREG));
#endif /* FROZEN_STACKS */
      WRITEBACK_Y_AS_ENV();
#ifndef NO_CHECKING
      /* check stacks */
      check_stack(NoStackDeallocate, H);
#endif
      ENDCACHE_Y_AS_ENV();
      GONext();
      ENDOp();

/**********************************************
*        OPTYap instructions                  *
**********************************************/

#ifdef YAPOR
#include "or.insts.i"
#endif /* YAPOR */
#ifdef TABLING
#include "tab.insts.i"
#include "tab.tries.insts.i"
#endif /* TABLING */



#ifdef BEAM
 extern int eam_am(PredEntry *);

     Op(retry_eam, e);
       printf("Aqui estou eu..................\n");
       if (!eam_am(2)) {
	 abort_eam("Falhei\n");
	   FAIL();
       }

       goto procceed;
       PREG = NEXTOP(PREG, e);
       GONext();
     ENDOp();

     Op(run_eam, os);
       if (inp==-9000) { /* usar a indexa��o para saber quais as alternativas validas */
 	  extern CELL *beam_ALTERNATIVES;
          *beam_ALTERNATIVES= (CELL *) PREG->u.os.opcw;
	  beam_ALTERNATIVES++;
	  if (OLD_B!=B) goto fail;
#if PUSH_REGS
	  Yap_regp=old_regs;
#endif
	  return(0);
        } 

        saveregs();
        if (!eam_am((PredEntry *) PREG->u.os.s)) FAIL();
	setregs();

	/* cut */
	BACKUP_B();
#ifdef CUT_C
	while (POP_CHOICE_POINT(B->cp_b)) {
	    POP_EXECUTE();
	}
#endif /* CUT_C */
        B = B->cp_b;  /* cut_fail */
        HB = B->cp_h; /* cut_fail */
        RECOVER_B();

        if (0) { register choiceptr ccp;                                          
	  /* initialize ccp */
#define NORM_CP(CP)            ((choiceptr)(CP))

          YREG = (CELL *) (NORM_CP(YREG) - 1);
          ccp = NORM_CP(YREG);
          store_yaam_reg_cpdepth(ccp);
          ccp->cp_tr = TR;
          ccp->cp_ap = BEAM_RETRY_CODE;
          ccp->cp_h  = H;
          ccp->cp_b  = B;
          ccp->cp_env= ENV;
          ccp->cp_cp = CPREG;
          B = ccp;
          SET_BB(B);
        }
        goto procceed;
        PREG = NEXTOP(PREG, os);
        GONext();
      ENDOp();
#endif


/************************************************************************\
*    Get Instructions							*
\************************************************************************/

      Op(get_x_var, xx);
      BEGD(d0);
      d0 = XREG(PREG->u.xx.xr);
      XREG(PREG->u.xx.xl) = d0;
      PREG = NEXTOP(PREG, xx);
      ENDD(d0);
      GONext();
      ENDOp();

      Op(get_y_var, yx);
      BEGD(d0);
      BEGP(pt0);
      pt0 = YREG + PREG->u.yx.y;
      d0 = XREG(PREG->u.yx.x);
      PREG = NEXTOP(PREG, yx);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(pt0,d0);
#else
      *pt0 = d0;
#endif /* SBA && FROZEN_STACKS */
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(get_yy_var, yyxx);
      CACHE_Y(YREG);
      BEGD(d0);
      BEGP(pt0);
      pt0 = S_YREG + PREG->u.yyxx.y1;
      d0 = XREG(PREG->u.yyxx.x1);
      BEGD(d1);
      BEGP(pt1);
      pt1 = S_YREG + PREG->u.yyx.y2;
      d1 = XREG(PREG->u.yyxx.x2);
      PREG = NEXTOP(PREG, yyxx);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(pt0,d0);
#else
      *pt0 = d0;
#endif /* SBA && FROZEN_STACKS */
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(pt1,d1);
#else
      *pt1 = d1;
#endif /* SBA && FROZEN_STACKS */
      ENDP(pt1);
      ENDD(d1);
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDCACHE_Y();
      ENDOp();

      /* The code for get_x_val is hard to follow because I use a
       * lot of jumps. The convention is that in the label
       * gval_X_YREG X refers to the state of the first argument, and
       * YREG to the state of the second argument */
      Op(get_x_val, xx);
      BEGD(d0);
      d0 = XREG(PREG->u.xx.xl);
      deref_head(d0, gvalx_unk);

      /* d0 will keep the first argument */
    gvalx_nonvar:
      /* first argument is bound */
      BEGD(d1);
      d1 = XREG(PREG->u.xx.xr);
      deref_head(d1, gvalx_nonvar_unk);

    gvalx_nonvar_nonvar:
      /* both arguments are bound */
      /* we may have to bind structures */
      PREG = NEXTOP(PREG, xx);
      UnifyBound(d0, d1);

      BEGP(pt0);
      /* deref second argument */
      deref_body(d1, pt0, gvalx_nonvar_unk, gvalx_nonvar_nonvar);
      /* first argument bound, second unbound */
      PREG = NEXTOP(PREG, xx);
      BIND_AND_JUMP(pt0, d0);
#ifdef COROUTINING
      DO_TRAIL(pt0, d0);
      if (pt0 < H0) Yap_WakeUp(pt0);
#endif
      GONext();

      ENDP(pt0);
      ENDD(d1);

      BEGP(pt0);
      /* first argument may be unbound */
      deref_body(d0, pt0, gvalx_unk, gvalx_nonvar);
      /* first argument is unbound and in pt0 and in d0 */
      BEGD(d1);
      d1 = XREG(PREG->u.xx.xr);
      deref_head(d1, gvalx_var_unk);

    gvalx_var_nonvar:
      /* first unbound, second bound */
      PREG = NEXTOP(PREG, xx);
      BIND(pt0, d1, bind_gvalx_var_nonvar);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    bind_gvalx_var_nonvar:
#endif
      GONext();

      BEGP(pt1);
      deref_body(d1, pt1, gvalx_var_unk, gvalx_var_nonvar);
      /* both arguments are unbound */
      PREG = NEXTOP(PREG, xx);
      UnifyCells(pt0, pt1, uc1, uc2);
#ifdef COROUTINING
      DO_TRAIL(pt0, (CELL)pt1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    uc1:
#endif
      GONext();
#ifdef COROUTINING
    uc2:
      DO_TRAIL(pt1, (CELL)pt0);
      if (pt1 < H0) Yap_WakeUp(pt1);
      GONext();
#endif
      ENDP(pt1);
      ENDD(d1);
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      /* The code for get_y_val mostly uses the code for get_x_val
       */

      Op(get_y_val, yx);
      BEGD(d0);
      BEGD(d1);
      BEGP(pt0);
      pt0 = YREG + PREG->u.yx.y;
      d0 = *pt0;

      /* From now on, it's just a copy of the code for get_x_val */

      deref_head(d0, gvaly_unk);
    gvaly_nonvar:

      /* first argument is bound */
      d1 = XREG(PREG->u.yx.x);
      deref_head(d1, gvaly_nonvar_unk);
    gvaly_nonvar_nonvar:

      /* both arguments are bound */
      /* we may have to bind structures */
      PREG = NEXTOP(PREG, yx);
      UnifyBound(d0, d1);

      BEGP(pt1);
      /* deref second argument */
      deref_body(d1, pt1, gvaly_nonvar_unk, gvaly_nonvar_nonvar);
      /* first argument bound, second unbound */
      PREG = NEXTOP(PREG, yx);
      BIND(pt1, d0, bind_gvaly_nonvar_var);
#ifdef COROUTINING
      DO_TRAIL(pt1, d0);
      if (pt1 < H0) Yap_WakeUp(pt1);
    bind_gvaly_nonvar_var:
#endif
      GONext();
      ENDP(pt1);


      /* first argument may be unbound */
      derefa_body(d0, pt0, gvaly_unk, gvaly_nonvar);
      /* first argument is unbound */
      d1 = XREG(PREG->u.yx.x);
      deref_head(d1, gvaly_var_unk);

    gvaly_var_nonvar:
      /* first unbound, second bound */
      PREG = NEXTOP(PREG, yx);
      BIND(pt0, d1, bind_gvaly_var_nonvar);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    bind_gvaly_var_nonvar:
#endif
      GONext();

      BEGP(pt1);
      deref_body(d1, pt1, gvaly_var_unk, gvaly_var_nonvar);
      /* both arguments are unbound */
      PREG = NEXTOP(PREG, yx);
      UnifyCells(pt0, pt1, uc3, uc4);
#ifdef COROUTINING
      DO_TRAIL(pt0, (CELL)pt1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    uc3:
#endif
      GONext();
#ifdef COROUTINING
    uc4:
      DO_TRAIL(pt1, (CELL)pt0);
      if (pt1 < H0) Yap_WakeUp(pt1);
      GONext();
#endif
      ENDP(pt1);
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(get_atom, xc);
      BEGD(d0);
      BEGD(d1);
      /* fetch arguments */
      d0 = XREG(PREG->u.xc.x);
      d1 = PREG->u.xc.c;

      BEGP(pt0);
      deref_head(d0, gatom_unk);
      /* argument is nonvar */
    gatom_nonvar:
      if (d0 == d1) {
	PREG = NEXTOP(PREG, xc);
	GONext();
      }
      else {
	FAIL();
      }

      deref_body(d0, pt0, gatom_unk, gatom_nonvar);
      /* argument is a variable */
      PREG = NEXTOP(PREG, xc);
      BIND(pt0, d1, bind_gatom);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    bind_gatom:
#endif
      GONext();
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(get_2atoms, cc);
      BEGD(d0);
      BEGD(d1);
      /* fetch arguments */
      d0 = ARG1;

      BEGP(pt0);
      deref_head(d0, gatom_2unk);
      /* argument is nonvar */
    gatom_2nonvar:
      if (d0 == PREG->u.cc.c1) {
	goto gatom_2b;
      }
      else {
	FAIL();
      }

      deref_body(d0, pt0, gatom_2unk, gatom_2nonvar);
      /* argument is a variable */
      BIND(pt0, PREG->u.cc.c1, gatom_2b);
#ifdef COROUTINING
      DO_TRAIL(pt0, PREG->u.cc.c1);
      if (pt0 < H0) Yap_WakeUp(pt0);
#endif
      ENDP(pt0);
    gatom_2b:
      /* fetch arguments */
      d0 = ARG2;
      d1 = PREG->u.cc.c2;

      BEGP(pt0);
      deref_head(d0, gatom_2bunk);
      /* argument is nonvar */
    gatom_2bnonvar:
      if (d0 == d1) {
	PREG = NEXTOP(PREG, cc);
	GONext();
      }
      else {
	FAIL();
      }

      deref_body(d0, pt0, gatom_2bunk, gatom_2bnonvar);
      /* argument is a variable */
      PREG = NEXTOP(PREG, cc);
      BIND(pt0, d1, gatom_2c);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    gatom_2c:
#endif
      GONext();
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(get_3atoms, ccc);
      BEGD(d0);
      BEGD(d1);
      /* fetch arguments */
      d0 = ARG1;

      BEGP(pt0);
      deref_head(d0, gatom_3unk);
      /* argument is nonvar */
    gatom_3nonvar:
      if (d0 == PREG->u.ccc.c1) {
	goto gatom_3b;
      }
      else {
	FAIL();
      }

      deref_body(d0, pt0, gatom_3unk, gatom_3nonvar);
      /* argument is a variable */
      BIND(pt0, PREG->u.ccc.c1, gatom_3b);
#ifdef COROUTINING
      DO_TRAIL(pt0, PREG->u.ccc.c1);
      if (pt0 < H0) Yap_WakeUp(pt0);
#endif
      ENDP(pt0);
    gatom_3b:
      /* fetch arguments */
      d0 = ARG2;

      BEGP(pt0);
      deref_head(d0, gatom_3bunk);
      /* argument is nonvar */
    gatom_3bnonvar:
      if (d0 == PREG->u.ccc.c2) {
	goto gatom_3c;
      }
      else {
	FAIL();
      }

      deref_body(d0, pt0, gatom_3bunk, gatom_3bnonvar);
      /* argument is a variable */
      BIND(pt0, PREG->u.ccc.c2, gatom_3c);
#ifdef COROUTINING
      DO_TRAIL(pt0, PREG->u.ccc.c2);
      if (pt0 < H0) Yap_WakeUp(pt0);
#endif
      ENDP(pt0);
    gatom_3c:
      /* fetch arguments */
      d0 = ARG3;
      d1 = PREG->u.ccc.c3;

      BEGP(pt0);
      deref_head(d0, gatom_3cunk);
      /* argument is nonvar */
    gatom_3cnonvar:
      if (d0 == d1) {
	PREG = NEXTOP(PREG, ccc);
	GONext();
      }
      else {
	FAIL();
      }

      deref_body(d0, pt0, gatom_3cunk, gatom_3cnonvar);
      /* argument is a variable */
      PREG = NEXTOP(PREG, ccc);
      BIND(pt0, d1, gatom_3d);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    gatom_3d:
#endif
      GONext();
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(get_4atoms, cccc);
      BEGD(d0);
      BEGD(d1);
      /* fetch arguments */
      d0 = ARG1;

      BEGP(pt0);
      deref_head(d0, gatom_4unk);
      /* argument is nonvar */
    gatom_4nonvar:
      if (d0 == PREG->u.cccc.c1) {
	goto gatom_4b;
      }
      else {
	FAIL();
      }

      deref_body(d0, pt0, gatom_4unk, gatom_4nonvar);
      /* argument is a variable */
      BIND(pt0, PREG->u.cccc.c1, gatom_4b);
#ifdef COROUTINING
      DO_TRAIL(pt0, PREG->u.cccc.c1);
      if (pt0 < H0) Yap_WakeUp(pt0);
#endif
      ENDP(pt0);
    gatom_4b:
      /* fetch arguments */
      d0 = ARG2;

      BEGP(pt0);
      deref_head(d0, gatom_4bunk);
      /* argument is nonvar */
    gatom_4bnonvar:
      if (d0 == PREG->u.cccc.c2) {
	goto gatom_4c;
      }
      else {
	FAIL();
      }

      deref_body(d0, pt0, gatom_4bunk, gatom_4bnonvar);
      /* argument is a variable */
      BIND(pt0, PREG->u.cccc.c2, gatom_4c);
#ifdef COROUTINING
      DO_TRAIL(pt0, PREG->u.cccc.c2);
      if (pt0 < H0) Yap_WakeUp(pt0);
#endif
      ENDP(pt0);
    gatom_4c:
      /* fetch arguments */
      d0 = ARG3;

      BEGP(pt0);
      deref_head(d0, gatom_4cunk);
      /* argument is nonvar */
    gatom_4cnonvar:
      if (d0 == PREG->u.cccc.c3) {
	goto gatom_4d;
      }
      else {
	FAIL();
      }

      deref_body(d0, pt0, gatom_4cunk, gatom_4cnonvar);
      /* argument is a variable */
      BIND(pt0, PREG->u.cccc.c3, gatom_4d);
#ifdef COROUTINING
      DO_TRAIL(pt0, PREG->u.cccc.c3);
      if (pt0 < H0) Yap_WakeUp(pt0);
#endif
      ENDP(pt0);
   gatom_4d:
      /* fetch arguments */
      d0 = ARG4;
      d1 = PREG->u.cccc.c4;

      BEGP(pt0);
      deref_head(d0, gatom_4dunk);
      /* argument is nonvar */
    gatom_4dnonvar:
      if (d0 == d1) {
	PREG = NEXTOP(PREG, cccc);
	GONext();
      }
      else {
	FAIL();
      }

      deref_body(d0, pt0, gatom_4dunk, gatom_4dnonvar);
      /* argument is a variable */
      PREG = NEXTOP(PREG, cccc);
      BIND(pt0, d1, gatom_4e);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    gatom_4e:
#endif
      GONext();
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(get_5atoms, ccccc);
      BEGD(d0);
      BEGD(d1);
      /* fetch arguments */
      d0 = ARG1;

      BEGP(pt0);
      deref_head(d0, gatom_5unk);
      /* argument is nonvar */
    gatom_5nonvar:
      if (d0 == PREG->u.ccccc.c1) {
	goto gatom_5b;
      }
      else {
	FAIL();
      }

      deref_body(d0, pt0, gatom_5unk, gatom_5nonvar);
      /* argument is a variable */
      BIND(pt0, PREG->u.ccccc.c1, gatom_5b);
#ifdef COROUTINING
      DO_TRAIL(pt0, PREG->u.ccccc.c1);
      if (pt0 < H0) Yap_WakeUp(pt0);
#endif
      ENDP(pt0);
    gatom_5b:
      /* fetch arguments */
      d0 = ARG2;

      BEGP(pt0);
      deref_head(d0, gatom_5bunk);
      /* argument is nonvar */
    gatom_5bnonvar:
      if (d0 == PREG->u.ccccc.c2) {
	goto gatom_5c;
      }
      else {
	FAIL();
      }

      deref_body(d0, pt0, gatom_5bunk, gatom_5bnonvar);
      /* argument is a variable */
      BIND(pt0, PREG->u.ccccc.c2, gatom_5c);
#ifdef COROUTINING
      DO_TRAIL(pt0, PREG->u.ccccc.c2);
      if (pt0 < H0) Yap_WakeUp(pt0);
#endif
      ENDP(pt0);
    gatom_5c:
      /* fetch arguments */
      d0 = ARG3;

      BEGP(pt0);
      deref_head(d0, gatom_5cunk);
      /* argument is nonvar */
    gatom_5cnonvar:
      if (d0 == PREG->u.ccccc.c3) {
	goto gatom_5d;
      }
      else {
	FAIL();
      }

      deref_body(d0, pt0, gatom_5cunk, gatom_5cnonvar);
      /* argument is a variable */
      BIND(pt0, PREG->u.ccccc.c3, gatom_5d);
#ifdef COROUTINING
      DO_TRAIL(pt0, PREG->u.ccccc.c3);
      if (pt0 < H0) Yap_WakeUp(pt0);
#endif
      ENDP(pt0);
   gatom_5d:
      /* fetch arguments */
      d0 = ARG4;

      BEGP(pt0);
      deref_head(d0, gatom_5dunk);
      /* argument is nonvar */
    gatom_5dnonvar:
      if (d0 == PREG->u.ccccc.c4) {
	goto gatom_5e;
      }
      else {
	FAIL();
      }

      deref_body(d0, pt0, gatom_5dunk, gatom_5dnonvar);
      /* argument is a variable */
      BIND(pt0, PREG->u.ccccc.c4, gatom_5e);
#ifdef COROUTINING
      DO_TRAIL(pt0, PREG->u.ccccc.c4);
      if (pt0 < H0) Yap_WakeUp(pt0);
#endif
      ENDP(pt0);
   gatom_5e:
      /* fetch arguments */
      d0 = ARG5;
      d1 = PREG->u.ccccc.c5;

      BEGP(pt0);
      deref_head(d0, gatom_5eunk);
      /* argument is nonvar */
    gatom_5enonvar:
      if (d0 == d1) {
	PREG = NEXTOP(PREG, ccccc);
	GONext();
      }
      else {
	FAIL();
      }

      deref_body(d0, pt0, gatom_5eunk, gatom_5enonvar);
      /* argument is a variable */
      PREG = NEXTOP(PREG, ccccc);
      BIND(pt0, d1, gatom_5f);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    gatom_5f:
#endif
      GONext();
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(get_6atoms, cccccc);
      BEGD(d0);
      BEGD(d1);
      /* fetch arguments */
      d0 = ARG1;

      BEGP(pt0);
      deref_head(d0, gatom_6unk);
      /* argument is nonvar */
    gatom_6nonvar:
      if (d0 == PREG->u.cccccc.c1) {
	goto gatom_6b;
      }
      else {
	FAIL();
      }

      deref_body(d0, pt0, gatom_6unk, gatom_6nonvar);
      /* argument is a variable */
      BIND(pt0, PREG->u.cccccc.c1, gatom_6b);
#ifdef COROUTINING
      DO_TRAIL(pt0, PREG->u.cccccc.c1);
      if (pt0 < H0) Yap_WakeUp(pt0);
#endif
      ENDP(pt0);
    gatom_6b:
      /* fetch arguments */
      d0 = ARG2;

      BEGP(pt0);
      deref_head(d0, gatom_6bunk);
      /* argument is nonvar */
    gatom_6bnonvar:
      if (d0 == PREG->u.cccccc.c2) {
	goto gatom_6c;
      }
      else {
	FAIL();
      }

      deref_body(d0, pt0, gatom_6bunk, gatom_6bnonvar);
      /* argument is a variable */
      BIND(pt0, PREG->u.cccccc.c2, gatom_6c);
#ifdef COROUTINING
      DO_TRAIL(pt0, PREG->u.cccccc.c2);
      if (pt0 < H0) Yap_WakeUp(pt0);
#endif
      ENDP(pt0);
    gatom_6c:
      /* fetch arguments */
      d0 = ARG3;

      BEGP(pt0);
      deref_head(d0, gatom_6cunk);
      /* argument is nonvar */
    gatom_6cnonvar:
      if (d0 == PREG->u.cccccc.c3) {
	goto gatom_6d;
      }
      else {
	FAIL();
      }

      deref_body(d0, pt0, gatom_6cunk, gatom_6cnonvar);
      /* argument is a variable */
      BIND(pt0, PREG->u.cccccc.c3, gatom_6d);
#ifdef COROUTINING
      DO_TRAIL(pt0, PREG->u.cccccc.c3);
      if (pt0 < H0) Yap_WakeUp(pt0);
#endif
      ENDP(pt0);
   gatom_6d:
      /* fetch arguments */
      d0 = ARG4;

      BEGP(pt0);
      deref_head(d0, gatom_6dunk);
      /* argument is nonvar */
    gatom_6dnonvar:
      if (d0 == PREG->u.cccccc.c4) {
	goto gatom_6e;
      }
      else {
	FAIL();
      }

      deref_body(d0, pt0, gatom_6dunk, gatom_6dnonvar);
      /* argument is a variable */
      BIND(pt0, PREG->u.cccccc.c4, gatom_6e);
#ifdef COROUTINING
      DO_TRAIL(pt0, PREG->u.cccccc.c4);
      if (pt0 < H0) Yap_WakeUp(pt0);
#endif
      ENDP(pt0);
   gatom_6e:
      /* fetch arguments */
      d0 = ARG5;

      BEGP(pt0);
      deref_head(d0, gatom_6eunk);
      /* argument is nonvar */
    gatom_6enonvar:
      if (d0 == PREG->u.cccccc.c5) {
	goto gatom_6f;
      }
      else {
	FAIL();
      }

      deref_body(d0, pt0, gatom_6eunk, gatom_6enonvar);
      /* argument is a variable */
      BIND(pt0, PREG->u.cccccc.c5, gatom_6f);
#ifdef COROUTINING
      DO_TRAIL(pt0, PREG->u.cccccc.c5);
      if (pt0 < H0) Yap_WakeUp(pt0);
#endif
      ENDP(pt0);
    gatom_6f:
      /* fetch arguments */
      d0 = ARG6;
      d1 = PREG->u.cccccc.c6;

      BEGP(pt0);
      deref_head(d0, gatom_6funk);
      /* argument is nonvar */
    gatom_6fnonvar:
      if (d0 == d1) {
	PREG = NEXTOP(PREG, cccccc);
	GONext();
      }
      else {
	FAIL();
      }

      deref_body(d0, pt0, gatom_6funk, gatom_6fnonvar);
      /* argument is a variable */
      PREG = NEXTOP(PREG, cccccc);
      BIND(pt0, d1, gatom_6g);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    gatom_6g:
#endif
      GONext();
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      /* The next instructions can lead to either the READ stream
       * or the write stream */

      OpRW(get_list, x);
      BEGD(d0);
      d0 = XREG(PREG->u.x.x);
      deref_head(d0, glist_unk);

    glist_nonvar:
      /* did we find a list? */
      if (!IsPairTerm(d0)) {
	FAIL();
      }
      START_PREFETCH(x);
      PREG = NEXTOP(PREG, x);
      /* enter read mode */
      SREG = RepPair(d0);
      GONext();
      END_PREFETCH();

      BEGP(pt0);
      deref_body(d0, pt0, glist_unk, glist_nonvar);
      /* glist var */
      /* enter write mode */
      CACHE_S();
      S_SREG = H;
      START_PREFETCH_W(x);
      PREG = NEXTOP(PREG, x);
      BEGD(d0);
      d0 = AbsPair(S_SREG);
      BIND(pt0, d0, bind_glist);
#ifdef COROUTINING
      DO_TRAIL(pt0, d0);
      if (pt0 < H0) {
	Yap_WakeUp(pt0);
	S_SREG = H;
      }
    bind_glist:
#endif
      /* don't put an ENDD just after a label */
      H = S_SREG + 2;
      ENDD(d0);
      WRITEBACK_S(S_SREG);
      GONextW();


      END_PREFETCH_W();
      ENDCACHE_S();
      ENDP(pt0);

      ENDD(d0);
      ENDOpRW();

      OpRW(get_struct, xfa);
      BEGD(d0);
      d0 = XREG(PREG->u.xfa.x);
      deref_head(d0, gstruct_unk);

    gstruct_nonvar:
      if (!IsApplTerm(d0))
	FAIL();
      /* we have met a compound term */
      START_PREFETCH(xfa);
      CACHE_S();
      S_SREG = RepAppl(d0);
      /* check functor */
      d0 = (CELL) (PREG->u.xfa.f);
      if (*S_SREG != d0) {
	FAIL();
      }
      WRITEBACK_S(S_SREG+1);
      ENDCACHE_S();
      PREG = NEXTOP(PREG, xfa);
      /* enter read mode */
      GONext();
      END_PREFETCH();

      BEGP(pt0);
      deref_body(d0, pt0, gstruct_unk, gstruct_nonvar);
      /* Enter Write mode */
      /* set d1 to be the new structure we are going to create */
      START_PREFETCH_W(xfa);
      BEGD(d1);
      d1 = AbsAppl(H);
      BIND(pt0, d1, bind_gstruct);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) {
	Yap_WakeUp(pt0);
      }
    bind_gstruct:
#endif
      /* now, set pt0 to point to the heap where we are going to
       * build our term */
      pt0 = H;
      ENDD(d1);
      /* first, put the functor */
      d0 = (CELL) (PREG->u.xfa.f);
      *pt0++ = d0;
      H = pt0 + PREG->u.xfa.a;
      PREG = NEXTOP(PREG, xfa);
      /* set SREG */
      SREG = pt0;
      /* update H */
      GONextW();
      END_PREFETCH_W();
      ENDP(pt0);

      ENDD(d0);
      ENDOpRW();

      Op(get_float, xd);
      BEGD(d0);
      d0 = XREG(PREG->u.xd.x);
      deref_head(d0, gfloat_unk);

    gfloat_nonvar:
      if (!IsApplTerm(d0))
	FAIL();
      /* we have met a preexisting float */
      START_PREFETCH(xd);
      BEGP(pt0);
      pt0 = RepAppl(d0);
      /* check functor */
      if (*pt0 != (CELL)FunctorDouble) {
	FAIL();
      }
      BEGP(pt1);
      pt1 = PREG->u.xd.d;
      PREG = NEXTOP(PREG, xd);
      if (
	  pt1[1] != pt0[1]
#if SIZEOF_DOUBLE == 2*SIZEOF_INT_P
	  || pt1[2] != pt0[2]
#endif
	  ) FAIL();
      ENDP(pt1);
      ENDP(pt0);
      /* enter read mode */
      GONext();
      END_PREFETCH();

      BEGP(pt0);
      deref_body(d0, pt0, gfloat_unk, gfloat_nonvar);
      /* Enter Write mode */
      /* set d1 to be the new structure we are going to create */
      START_PREFETCH(xc);
      BEGD(d1);
      d1 = AbsAppl(PREG->u.xd.d);
      PREG = NEXTOP(PREG, xd);
      BIND(pt0, d1, bind_gfloat);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    bind_gfloat:
#endif
      GONext();
      ENDD(d1);
      END_PREFETCH();
      ENDP(pt0);

      ENDD(d0);
      ENDOp();

      Op(get_longint, xi);
      BEGD(d0);
      d0 = XREG(PREG->u.xi.x);
      deref_head(d0, glongint_unk);

    glongint_nonvar:
      if (!IsApplTerm(d0))
	FAIL();
      /* we have met a preexisting longint */
      START_PREFETCH(xi);
      BEGP(pt0);
      pt0 = RepAppl(d0);
      /* check functor */
      if (*pt0 != (CELL)FunctorLongInt) {
	FAIL();
      }
      if (PREG->u.xi.i[1] != (CELL)pt0[1]) FAIL();
      ENDP(pt0);
      PREG = NEXTOP(PREG, xi);
      /* enter read mode */
      GONext();
      END_PREFETCH();

      BEGP(pt0);
      deref_body(d0, pt0, glongint_unk, glongint_nonvar);
      /* Enter Write mode */
      /* set d1 to be the new structure we are going to create */
      START_PREFETCH(xi);
      BEGD(d1);
      d1 = AbsAppl(PREG->u.xi.i);
      PREG = NEXTOP(PREG, xi);
      BIND(pt0, d1, bind_glongint);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    bind_glongint:
#endif
      GONext();
      ENDD(d1);
      END_PREFETCH();
      ENDP(pt0);

      ENDD(d0);
      ENDOp();

      Op(get_bigint, xc);
#ifdef USE_GMP
      BEGD(d0);
      d0 = XREG(PREG->u.xc.x);
      deref_head(d0, gbigint_unk);

    gbigint_nonvar:
      if (!IsApplTerm(d0))
	FAIL();
      /* we have met a preexisting bigint */
      START_PREFETCH(xc);
      BEGP(pt0);
      pt0 = RepAppl(d0);
      /* check functor */
      if (*pt0 != (CELL)FunctorBigInt)
	{
	  FAIL();
	}
      if (mpz_cmp(Yap_BigIntOfTerm(d0),Yap_BigIntOfTerm(PREG->u.xc.c)))
	FAIL();
      PREG = NEXTOP(PREG, xc);      
      ENDP(pt0);
      /* enter read mode */
      GONext();
      END_PREFETCH();

      BEGP(pt0);
      deref_body(d0, pt0, gbigint_unk, gbigint_nonvar);
      /* Enter Write mode */
      /* set d1 to be the new structure we are going to create */
      START_PREFETCH(xc);
      BEGD(d1);
      d1 = PREG->u.xc.c;
      PREG = NEXTOP(PREG, xc);
      BIND(pt0, d1, bind_gbigint);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    bind_gbigint:
#endif
      GONext();
      ENDD(d1);
      END_PREFETCH();
      ENDP(pt0);

      ENDD(d0);
#else
      FAIL();
#endif
      ENDOp();


      Op(get_dbterm, xc);
      BEGD(d0);
      d0 = XREG(PREG->u.xc.x);
      deref_head(d0, gdbterm_unk);

    gdbterm_nonvar:
      BEGD(d1);
      /* we have met a preexisting dbterm */
      d1 = PREG->u.xc.c;
      PREG = NEXTOP(PREG, xc);      
      UnifyBound(d0,d1);
      ENDD(d1);

      BEGP(pt0);
      deref_body(d0, pt0, gdbterm_unk, gdbterm_nonvar);
      /* Enter Write mode */
      /* set d1 to be the new structure we are going to create */
      START_PREFETCH(xc);
      BEGD(d1);
      d1 = PREG->u.xc.c;
      PREG = NEXTOP(PREG, xc);
      BIND(pt0, d1, bind_gdbterm);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    bind_gdbterm:
#endif
      GONext();
      ENDD(d1);
      END_PREFETCH();
      ENDP(pt0);

      ENDD(d0);
      ENDOp();

/************************************************************************\
*    Optimised Get List Instructions					*
\************************************************************************/
      OpRW(glist_valx, xx);
      BEGD(d0);
      d0 = XREG(PREG->u.xx.xl);
      deref_head(d0, glist_valx_write);
    glist_valx_read:
      BEGP(pt0);
      /* did we find a list? */
      if (!IsPairTerm(d0))
	FAIL();
      /* enter read mode */
      START_PREFETCH(xx);
      pt0 = RepPair(d0);
      SREG = pt0 + 1;
      /* start unification with first argument */
      d0 = *pt0;
      deref_head(d0, glist_valx_unk);

      /* first argument is in d0 */
    glist_valx_nonvar:
      /* first argument is bound */
      BEGD(d1);
      d1 = XREG(PREG->u.xx.xr);
      deref_head(d1, glist_valx_nonvar_unk);

    glist_valx_nonvar_nonvar:
      /* both arguments are bound */
      /* we may have to bind structures */
      PREG = NEXTOP(PREG, xx);
      UnifyBound(d0, d1);

      BEGP(pt1);
      /* deref second argument */
      deref_body(d1, pt1, glist_valx_nonvar_unk, glist_valx_nonvar_nonvar);
      /* head bound, argument unbound */
      PREG = NEXTOP(PREG, xx);
      BIND(pt1, d0, bind_glist_valx_nonvar_var);
#ifdef COROUTINING
      DO_TRAIL(pt1, d0);
      if (pt1 < H0) Yap_WakeUp(pt1);
    bind_glist_valx_nonvar_var:
#endif
      GONext();
      ENDP(pt1);


      ENDD(d1);

      /* head may be unbound */
      derefa_body(d0, pt0, glist_valx_unk, glist_valx_nonvar);
      /* head is unbound, pt0 has the value */
      d0 = XREG(PREG->u.xx.xr);
      deref_head(d0, glist_valx_var_unk);

    glist_valx_var_nonvar:
      /* head is unbound, second arg bound */
      PREG = NEXTOP(PREG, xx);
      BIND_GLOBAL(pt0, d0, bind_glist_valx_var_nonvar);
#ifdef COROUTINING
      DO_TRAIL(pt0, d0);
      if (pt0 < H0) Yap_WakeUp(pt0);
    bind_glist_valx_var_nonvar:
#endif
      GONext();

      BEGP(pt1);
      deref_body(d0, pt1, glist_valx_var_unk, glist_valx_var_nonvar);
      /* head and second argument are unbound */
      PREG = NEXTOP(PREG, xx);
      UnifyGlobalRegCells(pt0, pt1, uc5, uc6);
#ifdef COROUTINING
      DO_TRAIL(pt0, (CELL)pt1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    uc5:
#endif
      GONext();
#ifdef COROUTINING
    uc6:
      DO_TRAIL(pt1, (CELL)pt0);
      if (pt1 < H0) Yap_WakeUp(pt1);
      GONext();
#endif
      ENDP(pt1);
      ENDP(pt0);
      END_PREFETCH();

      BEGP(pt0);
      deref_body(d0, pt0, glist_valx_write, glist_valx_read);
      CACHE_S();
      /* enter write mode */
      S_SREG = H;
      BEGD(d1);
      d1 = XREG(PREG->u.xx.xr);
      d0 = AbsPair(S_SREG);
      S_SREG[0] = d1;
      ENDD(d1);
      ALWAYS_START_PREFETCH_W(xx);
#ifdef COROUTINING
      PREG = NEXTOP(PREG, xx);
      H = S_SREG + 2;
      WRITEBACK_S(S_SREG+1);
#endif
      DBIND(pt0, d0, dbind);
#ifndef COROUTINING
      /* include XREG on it */
      PREG = NEXTOP(PREG, xx);
      H = S_SREG + 2;
      WRITEBACK_S(S_SREG+1);
#endif

#ifdef COROUTINING
      DO_TRAIL(pt0, d0);
      if (pt0 < H0) Yap_WakeUp(pt0);
    dbind:
#endif
      ALWAYS_GONextW();
      ALWAYS_END_PREFETCH_W();
      ENDCACHE_S();
      ENDP(pt0);

      ENDD(d0);
      ENDOpRW();

      OpRW(glist_valy, yx);
      BEGD(d0);
      d0 = XREG(PREG->u.yx.x);
      deref_head(d0, glist_valy_write);
    glist_valy_read:
      BEGP(pt0);
      /* did we find a list? */
      if (!IsPairTerm(d0))
	FAIL();
      START_PREFETCH(yx);
      /* enter read mode */
      pt0 = RepPair(d0);
      SREG = pt0 + 1;
      /* start unification with first argument */
      d0 = *pt0;
      deref_head(d0, glist_valy_unk);

    glist_valy_nonvar:
      /* first argument is bound */
      BEGD(d1);
      BEGP(pt1);
      pt1 = YREG + PREG->u.yx.y;
      d1 = *pt1;
      PREG = NEXTOP(PREG, yx);
      deref_head(d1, glist_valy_nonvar_unk);

    glist_valy_nonvar_nonvar:
      /* both arguments are bound */
      /* we may have to bind structures */
      SREG = pt0 + 1;
      UnifyBound(d0, d1);

      /* deref second argument */
      derefa_body(d1, pt1, glist_valy_nonvar_unk, glist_valy_nonvar_nonvar);
      /* first argument bound, second unbound */
      BIND(pt1, d0, bind_glist_valy_nonvar_var);
#ifdef COROUTINING
      DO_TRAIL(pt1, d0);
      if (pt1 < H0) Yap_WakeUp(pt1);
    bind_glist_valy_nonvar_var:
#endif
      GONext();


      ENDP(pt1);

      /* first argument may be unbound */
      derefa_body(d0, pt0, glist_valy_unk, glist_valy_nonvar);
      /* first argument is unbound */
      BEGP(pt1);
      pt1 = YREG+PREG->u.yx.y;
      d1 = *pt1;
      deref_head(d1, glist_valy_var_unk);
    glist_valy_var_nonvar:
      /* first unbound, second bound */
      PREG = NEXTOP(PREG, yx);
      BIND_GLOBAL(pt0, d1, bind_glist_valy_var_nonvar);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    bind_glist_valy_var_nonvar:
#endif
      GONext();

      derefa_body(d1, pt1, glist_valy_var_unk, glist_valy_var_nonvar);
      /* both arguments are unbound */
      PREG = NEXTOP(PREG, yx);
      UnifyGlobalRegCells(pt0, pt1, uc7, uc8);
#ifdef COROUTINING
      DO_TRAIL(pt0, (CELL)pt1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    uc7:
#endif
      GONext();
#ifdef COROUTINING
    uc8:
      DO_TRAIL(pt1, (CELL)pt0);
      if (pt1 < H0) Yap_WakeUp(pt1);
      GONext();
#endif
      ENDP(pt1);
      ENDD(d1);

      END_PREFETCH();
      ENDP(pt0);

      BEGP(pt0);
      deref_body(d0, pt0, glist_valy_write, glist_valy_read);
      /* enter write mode */
      START_PREFETCH_W(yx);
      BEGP(pt1);
      pt1 = H;
      d0 = AbsPair(pt1);
      BIND(pt0, d0, bind_glist_valy_write);
#ifdef COROUTINING
      DO_TRAIL(pt0, d0);
      if (pt0 < H0) {
	Yap_WakeUp(pt0);
	pt1 = H;
      }
    bind_glist_valy_write:
#endif
      BEGD(d0);
      /* include XREG on it */
      d0 = YREG[PREG->u.yx.y];
      pt1[0] = d0;
      ENDD(d0);
      H = pt1 + 2;
      SREG = pt1 + 1;
      ENDP(pt1);
      PREG = NEXTOP(PREG, yx);
      GONextW();
      END_PREFETCH_W();
      ENDP(pt0);

      ENDD(d0);
      ENDOpRW();

      Op(gl_void_varx, xx);
      BEGD(d0);
      d0 = XREG(PREG->u.xx.xl);
      deref_head(d0, glist_void_varx_write);
    glist_void_varx_read:
      /* did we find a list? */
      if (!IsPairTerm(d0))
	FAIL();
      ALWAYS_START_PREFETCH(xx);
      /* enter read mode */
      BEGP(pt0);
      pt0 = RepPair(d0);
      d0 = pt0[1];
      XREG(PREG->u.xx.xr) = d0;
      PREG = NEXTOP(PREG, xx);
      ALWAYS_GONext();
      ENDP(pt0);
      ALWAYS_END_PREFETCH();

      BEGP(pt0);
      deref_body(d0, pt0, glist_void_varx_write, glist_void_varx_read);
      /* enter write mode */
      BEGP(pt1);
      pt1 = H;
      /* include XREG on it */
      XREG(PREG->u.xx.xr) =
	Unsigned(pt1 + 1);
      RESET_VARIABLE(pt1);
      RESET_VARIABLE(pt1+1);
      H = pt1 + 2;
      BEGD(d0);
      d0 = AbsPair(pt1);
      BIND(pt0, d0, bind_glist_varx_write);
#ifdef COROUTINING
      DO_TRAIL(pt0, d0);
      if (pt0 < H0) Yap_WakeUp(pt0);
    bind_glist_varx_write:
#endif
      PREG = NEXTOP(PREG, xx);
      ENDD(d0);
      ENDP(pt1);
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(gl_void_vary, yx);
      BEGD(d0);
      d0 = XREG(PREG->u.yx.x);
      deref_head(d0, glist_void_vary_write);
    glist_void_vary_read:
      /* did we find a list? */
      if (!IsPairTerm(d0))
	FAIL();
      /* enter read mode */
      BEGP(pt0);
      pt0 = RepPair(d0);
      d0 = pt0[1];
      ENDP(pt0);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(YREG+PREG->u.yx.y,d0);
#else
      YREG[PREG->u.yx.y] = d0;
#endif /* SBA && FROZEN_STACKS */
      PREG = NEXTOP(PREG, yx);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, glist_void_vary_write, glist_void_vary_read);
      /* enter write mode */
      BEGP(pt1);
      pt1 = H;
      /* include XREG on it */
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(YREG+PREG->u.yx.y,Unsigned(pt1 + 1));
#else
      YREG[PREG->u.yx.y] = Unsigned(pt1 + 1);
#endif /* SBA && FROZEN_STACKS */
      PREG = NEXTOP(PREG, yx);
      RESET_VARIABLE(pt1);
      RESET_VARIABLE(pt1+1);
      d0 = AbsPair(pt1);
      H = pt1 + 2;
      BIND(pt0, d0, bind_glist_void_vary_write);
#ifdef COROUTINING
      DO_TRAIL(pt0, d0);
      if (pt0 < H0) Yap_WakeUp(pt0);
    bind_glist_void_vary_write:
#endif
      GONext();
      ENDP(pt1);
      ENDP(pt0);

      ENDD(d0);
      ENDOp();

      Op(gl_void_valx, xx);
      BEGD(d0);
      d0 = XREG(PREG->u.xx.xl);
      deref_head(d0, glist_void_valx_write);
    glist_void_valx_read:
      BEGP(pt0);
      /* did we find a list? */
      if (!IsPairTerm(d0))
	FAIL();
      /* enter read mode */
      pt0 = RepPair(d0)+1;
      /* start unification with first argument */
      d0 = *pt0;
      deref_head(d0, glist_void_valx_unk);

    glist_void_valx_nonvar:
      /* first argument is bound */
      BEGD(d1);
      d1 = XREG(PREG->u.xx.xr);
      deref_head(d1, glist_void_valx_nonvar_unk);

    glist_void_valx_nonvar_nonvar:
      /* both arguments are bound */
      /* we may have to bind structures */
      PREG = NEXTOP(PREG, xx);
      UnifyBound(d0, d1);

      /* deref second argument */
      BEGP(pt1);
      deref_body(d1, pt1, glist_void_valx_nonvar_unk, glist_void_valx_nonvar_nonvar);
      /* first argument bound, second unbound */
      PREG = NEXTOP(PREG, xx);
      BIND(pt1, d0, bind_glist_void_valx_nonvar_var);
#ifdef COROUTINING
      DO_TRAIL(pt1, d0);
      if (pt1 < H0) Yap_WakeUp(pt1);
  bind_glist_void_valx_nonvar_var:
#endif
      GONext();
      ENDP(pt1);
      ENDD(d1);

      /* first argument may be unbound */
      derefa_body(d0, pt0, glist_void_valx_unk, glist_void_valx_nonvar);
      /* first argument is unbound */
      BEGD(d1);
      d1 = XREG(PREG->u.xx.xr);
      deref_head(d1, glist_void_valx_var_unk);

    glist_void_valx_var_nonvar:
      /* first unbound, second bound */
      PREG = NEXTOP(PREG, xx);
      BIND_GLOBAL(pt0, d1, bind_glist_void_valx_var_nonvar);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
  bind_glist_void_valx_var_nonvar:
#endif
      GONext();

      BEGP(pt1);
      deref_body(d1, pt1, glist_void_valx_var_unk, glist_void_valx_var_nonvar);
      /* both arguments are unbound */
      PREG = NEXTOP(PREG, xx);
      UnifyGlobalRegCells(pt0, pt1, uc9, uc10);
#ifdef COROUTINING
      DO_TRAIL(pt0, (CELL)pt1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    uc9:
#endif
      GONext();
#ifdef COROUTINING
    uc10:
      DO_TRAIL(pt1, (CELL)pt0);
      if (pt1 < H0) Yap_WakeUp(pt1);
      GONext();
#endif
      ENDP(pt1);
      ENDD(d1);
      ENDP(pt0);

      BEGP(pt0);
      deref_body(d0, pt0, glist_void_valx_write, glist_void_valx_read);
      /* enter write mode */
      BEGP(pt1);
      pt1 = H;
      d0 = AbsPair(pt1);
      BIND(pt0, d0, bind_glist_void_valx_write);
#ifdef COROUTINING
      DO_TRAIL(pt0, d0);
      if (pt0 < H0) {
	Yap_WakeUp(pt0);
	pt1 = H;
      }
    bind_glist_void_valx_write:
#endif
      BEGD(d0);
      /* include XREG on it */
      d0 = XREG(PREG->u.xx.xr);
      RESET_VARIABLE(pt1);
      pt1[1] = d0;
      H = pt1 + 2;
      ENDD(d0);
      ENDP(pt1);
      PREG = NEXTOP(PREG, xx);
      GONext();
      ENDP(pt0);

      ENDD(d0);
      ENDOp();

      Op(gl_void_valy, yx);
      BEGD(d0);
      d0 = XREG(PREG->u.yx.x);
      deref_head(d0, glist_void_valy_write);
    glist_void_valy_read:
      BEGP(pt0);
      /* did we find a list? */
      if (!IsPairTerm(d0))
	FAIL();
      /* enter read mode */
      pt0 = RepPair(d0)+1;
      /* start unification with first argument */
      d0 = *pt0;
      deref_head(d0, glist_void_valy_unk);

    glist_void_valy_nonvar:
      /* first argument is bound */
      BEGD(d1);
      BEGP(pt1);
      pt1 = YREG+PREG->u.yx.y;
      d1 = *pt1;
      deref_head(d1, glist_void_valy_nonvar_unk);

    glist_void_valy_nonvar_nonvar:
      /* both arguments are bound */
      /* we may have to bind structures */
      PREG = NEXTOP(PREG, yx);
      UnifyBound(d0, d1);

      /* deref second argument */
      derefa_body(d1, pt1, glist_void_valy_nonvar_unk, glist_void_valy_nonvar_nonvar);
      /* first argument bound, second unbound */
      PREG = NEXTOP(PREG, yx);
      BIND(pt1, d0, bind_glist_void_valy_nonvar_var);
#ifdef COROUTINING
      DO_TRAIL(pt1, d0);
      if (pt1 < H0) Yap_WakeUp(pt1);
  bind_glist_void_valy_nonvar_var:
#endif
      GONext();

      ENDP(pt1);

      /* first argument may be unbound */
      derefa_body(d0, pt0, glist_void_valy_unk, glist_void_valy_nonvar);
      /* first argument is unbound */
      BEGP(pt1);
      pt1 = YREG+PREG->u.yx.y;
      d1 = *pt1;
      deref_head(d1, glist_void_valy_var_unk);

    glist_void_valy_var_nonvar:
      /* first unbound, second bound */
      PREG = NEXTOP(PREG, yx);
      BIND_GLOBAL(pt0, d1, bind_glist_void_valy_var_nonvar);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
  bind_glist_void_valy_var_nonvar:
#endif
      GONext();

      deref_body(d1, pt1, glist_void_valy_var_unk, glist_void_valy_var_nonvar);
      /* both arguments are unbound */
      PREG = NEXTOP(PREG, yx);
      UnifyGlobalRegCells(pt0, pt1, uc11, uc12);
#ifdef COROUTINING
      DO_TRAIL(pt0, (CELL)pt1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    uc11:
#endif
      GONext();
#ifdef COROUTINING
    uc12:
      DO_TRAIL(pt1, (CELL)pt0);
      if (pt1 < H0) Yap_WakeUp(pt1);
      GONext();
#endif
      ENDP(pt1);
      ENDD(d1);
      ENDP(pt0);

      BEGP(pt0);
      deref_body(d0, pt0, glist_void_valy_write, glist_void_valy_read);
      /* enter write mode */
      CACHE_S();
      S_SREG = H;
      d0 = AbsPair(S_SREG);
      BIND(pt0, d0, bind_glist_void_valy_write);
#ifdef COROUTINING
      DO_TRAIL(pt0, d0);
      if (pt0 < H0) {
	Yap_WakeUp(pt0);
	S_SREG = H;
      }
    bind_glist_void_valy_write:
#endif
      /* include XREG on it */
      BEGD(d1);
      d1 = YREG[PREG->u.yx.y];
      RESET_VARIABLE(S_SREG);
      S_SREG[1] = d1;
      ENDD(d1);
      PREG = NEXTOP(PREG, yx);
      H = S_SREG + 2;
      ENDCACHE_S();
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();



/************************************************************************\
* 	Unify instructions						*
\************************************************************************/

      Op(unify_x_var, ox);
      CACHE_S();
      READ_IN_S();
      BEGD(d0);
      d0 = *S_SREG;
#ifdef SBA
      if (d0 == 0)
	d0 = (CELL)S_SREG;
#endif
      WRITEBACK_S(S_SREG+1);
      ALWAYS_START_PREFETCH(ox);
      XREG(PREG->u.ox.x) = d0;
      PREG = NEXTOP(PREG, ox);
      ALWAYS_GONext();
      ALWAYS_END_PREFETCH();
      ENDD(d0);
      ENDCACHE_S();
      ENDOp();

      OpW(unify_x_var_write, ox);
      CACHE_S();
      READ_IN_S();
      BEGP(pt0);
      pt0 = &XREG(PREG->u.ox.x);
      PREG = NEXTOP(PREG, ox);
      RESET_VARIABLE(S_SREG);
      *pt0 = (CELL) S_SREG;
      WRITEBACK_S(S_SREG+1);
      ENDP(pt0);
      ENDCACHE_S();
      GONextW();
      ENDOpW();

      BOp(unify_l_x_var, ox);
      ALWAYS_START_PREFETCH(ox);
      BEGP(pt0);
      BEGD(d0);
      d0 = SREG[0];
      pt0 = &XREG(PREG->u.ox.x);
      PREG = NEXTOP(PREG, ox);
#ifdef SBA
      if (d0 == 0)
	d0 = (CELL)SREG;
#endif
      *pt0 = d0;
      ALWAYS_GONext();
      ENDD(d0);
      ENDP(pt0);
      ALWAYS_END_PREFETCH();
      ENDBOp();

      BOp(unify_l_x_var_write, ox);
      ALWAYS_START_PREFETCH(ox);
      CACHE_S();
      READ_IN_S();
      BEGP(pt0);
      pt0 = &XREG(PREG->u.ox.x);
      PREG = NEXTOP(PREG, ox);
      RESET_VARIABLE(S_SREG);
      *pt0 = (CELL)S_SREG;
      ENDP(pt0);
      ENDCACHE_S();
      ALWAYS_GONext();
      ENDBOp();
      ALWAYS_END_PREFETCH();

      BOp(unify_x_var2, oxx);
      CACHE_S();
      ALWAYS_START_PREFETCH(oxx);
      READ_IN_S();
      BEGP(pt0);
      pt0 = &XREG(PREG->u.oxx.xr);
      BEGD(d0);
      d0 = S_SREG[0];
      BEGD(d1);
      d1 = S_SREG[1];
#ifdef SBA
      if (d0 == 0)
	d0 = (CELL)S_SREG;
      if (d1 == 0)
	d1 = (CELL)(S_SREG+1);
#endif
      WRITEBACK_S(S_SREG+2);
      XREG(PREG->u.oxx.xl) = d0;
      PREG = NEXTOP(PREG, oxx);
      *pt0 = d1;
      ENDD(d0);
      ENDD(d1);
      ENDP(pt0);
      ALWAYS_GONext();
      ENDBOp();
      ALWAYS_END_PREFETCH();
      ENDCACHE_S();

      OpW(unify_x_var2_write, oxx);
      CACHE_S();
      READ_IN_S();
      BEGP(pt0);
      pt0 = &XREG(PREG->u.oxx.xr);
      RESET_VARIABLE(S_SREG);
      XREG(PREG->u.oxx.xl) = (CELL) S_SREG;
      S_SREG++;
      PREG = NEXTOP(PREG, oxx);
      RESET_VARIABLE(S_SREG);
      *pt0 = (CELL) S_SREG;
      ENDP(pt0);
      WRITEBACK_S(S_SREG+1);
      ENDCACHE_S();
      GONextW();
      ENDOpW();

      BOp(unify_l_x_var2, oxx);
      ALWAYS_START_PREFETCH(oxx);
      CACHE_S();
      READ_IN_S();
      BEGP(pt0);
      pt0 = &XREG(PREG->u.oxx.xr);
      BEGD(d0);
      d0 = S_SREG[0];
      BEGD(d1);
      d1 = S_SREG[1];
#ifdef SBA
      if (d0 == 0)
	XREG(PREG->u.oxx.xl) = (CELL)S_SREG;
      else
#endif
	XREG(PREG->u.oxx.xl) = d0;
      PREG = NEXTOP(PREG, oxx);
#ifdef SBA
      if (d1 == 0)
	*pt0 = (CELL)(S_SREG+1);
      else
#endif
	*pt0 = d1;
      ENDD(d0);
      ENDD(d1);
      ENDP(pt0);
      ENDCACHE_S();
      ALWAYS_GONext();
      ENDBOp();
      ALWAYS_END_PREFETCH();

      Op(unify_l_x_var2_write, oxx);
      CACHE_S();
      READ_IN_S();
      BEGP(pt0);
      pt0 = &XREG(PREG->u.oxx.xr);
      XREG(PREG->u.oxx.xl) = (CELL) S_SREG;
      RESET_VARIABLE(S_SREG);
      S_SREG++;
      *pt0 = (CELL) S_SREG;
      PREG = NEXTOP(PREG, oxx);
      RESET_VARIABLE(S_SREG);
      ENDP(pt0);
      ENDCACHE_S();
      GONext();
      ENDOp();

      Op(unify_y_var, oy);
      BEGD(d0);
      d0 = *SREG++;
#if defined(SBA)
#ifdef FROZEN_STACKS
      if (d0 == 0) {
	Bind_Local(YREG+PREG->u.oy.y,(CELL)(SREG-1));
      } else {
	Bind_Local(YREG+PREG->u.oy.y,d0);
      }
#else
      if (d0 == 0) {
	YREG[PREG->u.oy.y] = (CELL)(SREG-1);
      } else
	YREG[PREG->u.oy.y] = d0;
#endif /* FROZEN_STACKS */
#else
      YREG[PREG->u.oy.y] = d0;
#endif /* SBA */
      PREG = NEXTOP(PREG, oy);
      GONext();
      ENDD(d0);
      ENDOp();

      OpW(unify_y_var_write, oy);
      CACHE_S();
      READ_IN_S();
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(YREG+PREG->u.oy.y,(CELL) S_SREG);
#else
      YREG[PREG->u.oy.y] = (CELL) S_SREG;
#endif /* SBA && FROZEN_STACKS */
      PREG = NEXTOP(PREG, oy);
      RESET_VARIABLE(S_SREG);
      WRITEBACK_S(S_SREG+1);
      ENDCACHE_S();
      GONextW();
      ENDOpW();

      Op(unify_l_y_var, oy);
      BEGD(d0);
      d0 = SREG[0];
#ifdef SBA
#ifdef FROZEN_STACKS
      if (d0 == 0) {
	Bind_Local(YREG+PREG->u.oy.y,(CELL)SREG);
      } else {
	Bind_Local(YREG+PREG->u.oy.y,d0);
      }
#else
      if (d0 == 0) {
	YREG[PREG->u.oy.y] = (CELL)SREG;
      } else {
	YREG[PREG->u.oy.y] = d0;
      }
#endif /* FROZEN_STACKS */
#else
      YREG[PREG->u.oy.y] = d0;
#endif /* SBA */
      PREG = NEXTOP(PREG, oy);
      GONext();
      ENDD(d0);
      ENDOp();

      Op(unify_l_y_var_write, oy);
      CACHE_S();
      READ_IN_S();
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(YREG+PREG->u.oy.y,(CELL) S_SREG);
#else
      YREG[PREG->u.oy.y] = (CELL) S_SREG;
#endif /* SBA && FROZEN_STACKS */
      PREG = NEXTOP(PREG, oy);
      RESET_VARIABLE(S_SREG);
      ENDCACHE_S();
      GONext();
      ENDOp();

      /* We assume the value in X is pointing to an object in the
       * global stack */
      Op(unify_x_val, ox);
      BEGD(d0);
      BEGD(d1);
      BEGP(pt0);
      pt0 = SREG;
      d0 = *pt0;
      deref_head(d0, uvalx_unk);

    uvalx_nonvar:
      /* first argument is bound */
      d1 = XREG(PREG->u.ox.x);
      deref_head(d1, uvalx_nonvar_unk);

    uvalx_nonvar_nonvar:
      /* both arguments are bound */
      /* we may have to bind structures */
      PREG = NEXTOP(PREG, ox);
      SREG++;
      UnifyBound(d0, d1);

      /* deref second argument */
      /* pt0 is in the structure and pt1 the register */
      BEGP(pt1);
      deref_body(d1, pt1, uvalx_nonvar_unk, uvalx_nonvar_nonvar);
      /* first argument bound, second unbound */
      PREG = NEXTOP(PREG, ox);
      SREG++;
      BIND(pt1, d0, bind_uvalx_nonvar_var);
#ifdef COROUTINING
      DO_TRAIL(pt1, d0);
      if (pt1 < H0) Yap_WakeUp(pt1);
  bind_uvalx_nonvar_var:
#endif
      GONext();
     ENDP(pt1);

      /* first argument may be unbound */
      derefa_body(d0, pt0, uvalx_unk, uvalx_nonvar);
      /* first argument is unbound */
      d1 = XREG(PREG->u.ox.x);
      deref_head(d1, uvalx_var_unk);

    uvalx_var_nonvar:
      /* first unbound, second bound */
      PREG = NEXTOP(PREG, ox);
      SREG++;
      BIND_GLOBAL(pt0, d1, bind_uvalx_var_nonvar);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
  bind_uvalx_var_nonvar:
#endif
      GONext();

      BEGP(pt1);
      deref_body(d1, pt1, uvalx_var_unk, uvalx_var_nonvar);
      /* both arguments are unbound */
      PREG = NEXTOP(PREG, ox);
      SREG++;
      UnifyGlobalRegCells(pt0, pt1, uc13, uc14);
#ifdef COROUTINING
      DO_TRAIL(pt0, (CELL)pt1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    uc13:
#endif
      GONext();
#ifdef COROUTINING
    uc14:
      DO_TRAIL(pt1, (CELL)pt0);
      if (pt1 < H0) Yap_WakeUp(pt1);
      GONext();
#endif
      ENDP(pt1);
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      OpW(unify_x_val_write, ox);
      /* we are in write mode */
      *SREG++ = XREG(PREG->u.ox.x);
      PREG = NEXTOP(PREG, ox);
      GONextW();
      ENDOpW();

      /* We assume the value in X is pointing to an object in the
       * global stack */
      Op(unify_l_x_val, ox);
      BEGD(d0);
      BEGD(d1);
      BEGP(pt0);
      pt0 = SREG;
      d0 = *pt0;
      deref_head(d0, ulvalx_unk);

    ulvalx_nonvar:
      /* first argument is bound */
      d1 = XREG(PREG->u.ox.x);
      deref_head(d1, ulvalx_nonvar_unk);

    ulvalx_nonvar_nonvar:
      /* both arguments are bound */
      /* we may have to bind structures */
      PREG = NEXTOP(PREG, ox);
      UnifyBound(d0, d1);

      BEGP(pt1);
      /* deref second argument */
      deref_body(d1, pt1, ulvalx_nonvar_unk, ulvalx_nonvar_nonvar);
      /* first argument bound, second unbound */
      PREG = NEXTOP(PREG, ox);
      BIND(pt1, d0, bind_ulvalx_nonvar_var);
#ifdef COROUTINING
      DO_TRAIL(pt1, d0);
      if (pt1 < H0) Yap_WakeUp(pt1);
  bind_ulvalx_nonvar_var:
#endif
      GONext();
      ENDP(pt1);

      /* first argument may be unbound */
      derefa_body(d0, pt0, ulvalx_unk, ulvalx_nonvar);
      /* first argument is unbound */
      d1 = XREG(PREG->u.ox.x);
      deref_head(d1, ulvalx_var_unk);

    ulvalx_var_nonvar:
      /* first unbound, second bound */
      PREG = NEXTOP(PREG, ox);
      BIND_GLOBAL(pt0, d1, bind_ulvalx_var_nonvar);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
  bind_ulvalx_var_nonvar:
#endif
      GONext();

      BEGP(pt1);
      deref_body(d1, pt1, ulvalx_var_unk, ulvalx_var_nonvar);
      /* both arguments are unbound */
      PREG = NEXTOP(PREG, ox);
      UnifyGlobalRegCells(pt0, pt1, uc15, uc16);
#ifdef COROUTINING
      DO_TRAIL(pt0, (CELL)pt1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    uc15:
#endif
      GONext();
#ifdef COROUTINING
    uc16:
      DO_TRAIL(pt1, (CELL)pt0);
      if (pt1 < H0) Yap_WakeUp(pt1);
      GONext();
#endif
      ENDP(pt1);
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(unify_l_x_val_write, ox);
      /* we are in write mode */
      SREG[0] = XREG(PREG->u.ox.x);
      PREG = NEXTOP(PREG, ox);
      GONext();
      ENDOp();

      /* We assume the value in X is pointing to an object in the
       * global stack */
      Op(unify_y_val, oy);
      BEGD(d0);
      BEGD(d1);
      BEGP(pt0);
      pt0 = SREG;
      d0 = *pt0;
      deref_head(d0, uvaly_unk);

    uvaly_nonvar:
      /* first argument is bound */
      BEGP(pt1);
      pt1 = YREG+PREG->u.oy.y; 
      d1 = *pt1;
      deref_head(d1, uvaly_nonvar_unk);

    uvaly_nonvar_nonvar:
      /* both arguments are bound */
      /* we may have to bind structures */
      PREG = NEXTOP(PREG, oy);
      SREG++;
      UnifyBound(d0, d1);

      /* deref second argument */
      derefa_body(d1, pt1, uvaly_nonvar_unk, uvaly_nonvar_nonvar);
      /* first argument bound, second unbound */
      PREG = NEXTOP(PREG, oy);
      SREG++;
      BIND(pt1, d0, bind_uvaly_nonvar_var);
#ifdef COROUTINING
      DO_TRAIL(pt1, d0);
      if (pt1 < H0) Yap_WakeUp(pt1);
  bind_uvaly_nonvar_var:
#endif
      GONext();
      ENDP(pt1);

      /* first argument may be unbound */
      derefa_body(d0, pt0, uvaly_unk, uvaly_nonvar);
      /* first argument is unbound */
      BEGP(pt1);
      pt1 = YREG+PREG->u.oy.y;
      d1 = *pt1;
      deref_head(d1, uvaly_var_unk);

    uvaly_var_nonvar:
      /* first unbound, second bound */
      PREG = NEXTOP(PREG, oy);
      SREG++;
      BIND_GLOBAL(pt0, d1, bind_uvaly_var_nonvar);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
  bind_uvaly_var_nonvar:
#endif
      GONext();

      derefa_body(d1, pt1, uvaly_var_unk, uvaly_var_nonvar);
      /* both arguments are unbound */
      PREG = NEXTOP(PREG, oy);
      SREG++;
      UnifyGlobalRegCells(pt0, pt1, uc17, uc18);
#ifdef COROUTINING
      DO_TRAIL(pt0, (CELL)pt1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    uc17:
#endif
      GONext();
#ifdef COROUTINING
    uc18:
      DO_TRAIL(pt1, (CELL)pt1);
      if (pt1 < H0) Yap_WakeUp(pt1);
      GONext();
#endif
      ENDP(pt1);
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      OpW(unify_y_val_write, oy);
      /* we are in write mode */
      BEGD(d0);
      d0 = YREG[PREG->u.oy.y];
#ifdef SBA
      if (d0 == 0) /* free variable */
	*SREG++ = (CELL)(YREG+PREG->u.oy.y);
      else
#endif
	*SREG++ = d0;
      ENDD(d0);
      PREG = NEXTOP(PREG, oy);
      GONextW();
      ENDOpW();

      /* We assume the value in X is pointing to an object in the
       * global stack */
      Op(unify_l_y_val, oy);
      BEGD(d0);
      BEGD(d1);
      BEGP(pt0);
      pt0 = SREG;
      d0 = *pt0;
      deref_head(d0, ulvaly_unk);

    ulvaly_nonvar:
      /* first argument is bound */
      BEGP(pt1);
      pt1 = YREG+PREG->u.oy.y;
      d1 = *pt1;
      deref_head(d1, ulvaly_nonvar_unk);

    ulvaly_nonvar_nonvar:
      /* both arguments are bound */
      /* we may have to bind structures */
      PREG = NEXTOP(PREG, oy);
      UnifyBound(d0, d1);

      /* deref second argument */
      derefa_body(d1, pt1, ulvaly_nonvar_unk, ulvaly_nonvar_nonvar);
      /* first argument bound, second unbound */
      PREG = NEXTOP(PREG, oy);
      BIND(pt1, d0, bind_ulvaly_nonvar_var);
#ifdef COROUTINING
      DO_TRAIL(pt1, d0);
      if (pt1 < H0) Yap_WakeUp(pt1);
  bind_ulvaly_nonvar_var:
#endif
      GONext();
      ENDP(pt1);

      /* first argument may be unbound */
      derefa_body(d0, pt0, ulvaly_unk, ulvaly_nonvar);
      /* first argument is unbound */
      BEGP(pt1);
      pt1 = YREG+PREG->u.oy.y;
      d1 = *pt1;
      deref_head(d1, ulvaly_var_unk);

    ulvaly_var_nonvar:
      /* first unbound, second bound */
      PREG = NEXTOP(PREG, oy);
      BIND_GLOBAL(pt0, d1, bind_ulvaly_var_nonvar);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
  bind_ulvaly_var_nonvar:
#endif
      GONext();

      /* Here we are in trouble: we have a clash between pt1 and
       * SREG. We address this by storing SREG in d0 for the duration. */
      derefa_body(d1, pt1, ulvaly_var_unk, ulvaly_var_nonvar);
      /* both arguments are unbound */
      PREG = NEXTOP(PREG, oy);
      UnifyGlobalRegCells(pt0, pt1, uc19, uc20);
#ifdef COROUTINING
      DO_TRAIL(pt0, (CELL)pt1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    uc19:
#endif
      GONext();
#ifdef COROUTINING
    uc20:
      DO_TRAIL(pt1, (CELL)pt0);
      if (pt1 < H0) Yap_WakeUp(pt1);
      GONext();
#endif
      ENDP(pt1);
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(unify_l_y_val_write, oy);
      /* we are in write mode */
      BEGD(d0);
      d0 = YREG[PREG->u.oy.y];
#ifdef SBA
      if (d0 == 0) /* new variable */
	SREG[0] = (CELL)(YREG+PREG->u.oy.y);
      else
#endif
	SREG[0] = d0;
      ENDD(d0);
      PREG = NEXTOP(PREG, oy);
      GONext();
      ENDOp();

      /* In the next instructions, we do not know anything about
       * what is in X */
      Op(unify_x_loc, ox);
      BEGD(d0);
      BEGD(d1);
      BEGP(pt0);
      pt0 = SREG;
      d0 = *pt0;
      deref_head(d0, uvalx_loc_unk);

    uvalx_loc_nonvar:
      /* first argument is bound */
      d1 = XREG(PREG->u.ox.x);
      deref_head(d1, uvalx_loc_nonvar_unk);

    uvalx_loc_nonvar_nonvar:
      /* both arguments are bound */
      /* we may have to bind structures */
      PREG = NEXTOP(PREG, ox);
      SREG++;
      UnifyBound(d0, d1);

      BEGP(pt1);
      /* deref second argument */
      deref_body(d1, pt1, uvalx_loc_nonvar_unk, uvalx_loc_nonvar_nonvar);
      /* first argument bound, second unbound */
      PREG = NEXTOP(PREG, ox);
      SREG++;
      BIND(pt1, d0, bind_uvalx_loc_nonvar_var);
#ifdef COROUTINING
      DO_TRAIL(pt1, d0);
      if (pt1 < H0) Yap_WakeUp(pt1);
  bind_uvalx_loc_nonvar_var:
#endif
      GONext();
      ENDP(pt1);


      /* first argument may be unbound */
      derefa_body(d0, pt0, uvalx_loc_unk, uvalx_loc_nonvar);
      /* first argument is unbound */
      d1 = XREG(PREG->u.ox.x);
      deref_head(d1, uvalx_loc_var_unk);

    uvalx_loc_var_nonvar:
      /* first unbound, second bound */
      PREG = NEXTOP(PREG, ox);
      SREG++;
      BIND_GLOBAL(pt0, d1, bind_uvalx_loc_var_nonvar);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
  bind_uvalx_loc_var_nonvar:
#endif
      GONext();

      /* Here we are in trouble: we have a clash between pt1 and
       * SREG. We address this by storing SREG in d0 for the duration. */
      BEGP(pt1);
      deref_body(d1, pt1, uvalx_loc_var_unk, uvalx_loc_var_nonvar);
      /* both arguments are unbound */
      PREG = NEXTOP(PREG, ox);
      SREG++;
      UnifyGlobalRegCells(pt0, pt1, uc21, uc22);
#ifdef COROUTINING
      DO_TRAIL(pt0, (CELL)pt1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    uc21:
#endif
      GONext();
#ifdef COROUTINING
    uc22:
      DO_TRAIL(pt1, (CELL)pt0);
      if (pt1 < H0) Yap_WakeUp(pt1);
      GONext();
#endif
      ENDP(pt1);
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      OpW(unify_x_loc_write, ox);
      /* we are in write mode */
      BEGD(d0);
      d0 = XREG(PREG->u.ox.x);
      deref_head(d0, unify_x_loc_unk);
    unify_x_loc_nonvar:
      *SREG++ = d0;
      PREG = NEXTOP(PREG, ox);
      GONextW();

      BEGP(pt0);
      deref_body(d0, pt0, unify_x_loc_unk, unify_x_loc_nonvar);
      /* move ahead in the instructions */
      PREG = NEXTOP(PREG, ox);
      /* d0 is a variable, check whether we need to globalise it */
      if (pt0 < H) {
	/* variable is global */
	*SREG++ = Unsigned(pt0);
	GONextW();
      }
      else {
	/* bind our variable to the structure */
	CACHE_S();
	READ_IN_S();
	Bind_Local(pt0, Unsigned(S_SREG));
	RESET_VARIABLE(S_SREG);
	WRITEBACK_S(S_SREG+1);
	ENDCACHE_S();
	GONextW();
      }
      ENDP(pt0);
      ENDD(d0);
      ENDOpW();

      /* In the next instructions, we do not know anything about
       * what is in X */
      Op(unify_l_x_loc, ox);
      BEGD(d0);
      BEGD(d1);
      BEGP(pt0);
      pt0 = SREG;
      d0 = *pt0;
      deref_head(d0, ulvalx_loc_unk);

    ulvalx_loc_nonvar:
      /* first argument is bound */
      d1 = XREG(PREG->u.ox.x);
      deref_head(d1, ulvalx_loc_nonvar_unk);

    ulvalx_loc_nonvar_nonvar:
      /* both arguments are bound */
      /* we may have to bind structures */
      PREG = NEXTOP(PREG, ox);
      UnifyBound(d0, d1);

      /* deref second argument */
      deref_body(d1, pt0, ulvalx_loc_nonvar_unk, ulvalx_loc_nonvar_nonvar);
      /* first argument bound, second unbound */
      PREG = NEXTOP(PREG, ox);
      BIND(pt0, d0, bind_ulvalx_loc_nonvar_var);
#ifdef COROUTINING
      DO_TRAIL(pt0, d0);
      if (pt0 < H0) Yap_WakeUp(pt0);
  bind_ulvalx_loc_nonvar_var:
#endif
      GONext();

      /* first argument may be unbound */
      derefa_body(d0, pt0, ulvalx_loc_unk, ulvalx_loc_nonvar);
      /* first argument is unbound */
      d1 = XREG(PREG->u.ox.x);
      deref_head(d1, ulvalx_loc_var_unk);

    ulvalx_loc_var_nonvar:
      /* first unbound, second bound */
      PREG = NEXTOP(PREG, ox);
      BIND_GLOBAL(pt0, d1, bind_ulvalx_loc_var_nonvar);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
  bind_ulvalx_loc_var_nonvar:
#endif
      GONext();

      BEGP(pt1);
      deref_body(d1, pt1, ulvalx_loc_var_unk, ulvalx_loc_var_nonvar);
      /* both arguments are unbound */
      PREG = NEXTOP(PREG, ox);
      UnifyGlobalRegCells(pt0, pt1, uc23, uc24);
#ifdef COROUTINING
      DO_TRAIL(pt0, (CELL)pt1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    uc23:
#endif
      GONext();
#ifdef COROUTINING
    uc24:
      DO_TRAIL(pt1, (CELL)pt0);
      if (pt1 < H0) Yap_WakeUp(pt1);
      GONext();
#endif
      ENDP(pt1);
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(unify_l_x_loc_write, ox);
      /* we are in write mode */
      BEGD(d0);
      d0 = XREG(PREG->u.ox.x);
      deref_head(d0, ulnify_x_loc_unk);
    ulnify_x_loc_nonvar:
      SREG[0] = d0;
      PREG = NEXTOP(PREG, ox);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, ulnify_x_loc_unk, ulnify_x_loc_nonvar);
      /* d0 is a variable, check whether we need to globalise it */
      PREG = NEXTOP(PREG, ox);
      if (pt0 < H) {
	/* variable is global */
	SREG[0] = Unsigned(pt0);
	GONext();
      }
      else {
	/* create a new Heap variable and bind our variable to it */
	Bind_Local(pt0, Unsigned(SREG));
	RESET_VARIABLE(SREG);
	GONext();
      }
      ENDP(pt0);
      ENDD(d0);
      ENDOpW();

      Op(unify_y_loc, oy);
      /* we are in read mode */
      BEGD(d0);
      BEGD(d1);
      BEGP(pt0);
      pt0 = SREG;
      d0 = *pt0;
      deref_head(d0, uvaly_loc_unk);

    uvaly_loc_nonvar:
      /* structure is bound */
      BEGP(pt1);
      pt1 =  YREG+PREG->u.oy.y;
      d1 = *pt1;
      deref_head(d1, uvaly_loc_nonvar_unk);

    uvaly_loc_nonvar_nonvar:
      /* both arguments are bound */
      /* we may have to bind structures */
      PREG = NEXTOP(PREG, oy);
      SREG++;
      UnifyBound(d0, d1);

      /* deref second argument */
      derefa_body(d1, pt1, uvaly_loc_nonvar_unk, uvaly_loc_nonvar_nonvar);
      /* first argument bound, second unbound */
      PREG = NEXTOP(PREG, oy);
      SREG++;
      BIND(pt1, d0, bind_uvaly_loc_nonvar_var);
#ifdef COROUTINING
      DO_TRAIL(pt1, d0);
      if (pt1 < H0) Yap_WakeUp(pt1);
  bind_uvaly_loc_nonvar_var:
#endif
      GONext();
      ENDP(pt1);

      /* first argument may be unbound */
      derefa_body(d0, pt0, uvaly_loc_unk, uvaly_loc_nonvar);
      /* first argument is unbound */
      BEGP(pt1);
      pt1 = YREG+PREG->u.oy.y; 
      d1 = *pt1;
      deref_head(d1, uvaly_loc_var_unk);

    uvaly_loc_var_nonvar:
      /* first unbound, second bound */
      PREG = NEXTOP(PREG, oy);
      SREG++;
      BIND_GLOBAL(pt0, d1, bind_uvaly_loc_var_nonvar);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
  bind_uvaly_loc_var_nonvar:
#endif
      GONext();

      /* Here we are in trouble: we have a clash between pt1 and
       * SREG. We address this by storing SREG in d0 for the duration. */
      derefa_body(d1, pt1, uvaly_loc_var_unk, uvaly_loc_var_nonvar);
      /* both arguments are unbound */
      PREG = NEXTOP(PREG, oy);
      SREG++;
      UnifyGlobalRegCells(pt0, pt1, uc25, uc26);
#ifdef COROUTINING
      DO_TRAIL(pt0, (CELL)pt1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    uc25:
#endif
      GONext();
#ifdef COROUTINING
    uc26:
      DO_TRAIL(pt1, (CELL)pt0);
      if (pt1 < H0) Yap_WakeUp(pt1);
      GONext();
#endif
      ENDP(pt1);
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      OpW(unify_y_loc_write, oy);
      /* we are in write mode */
      BEGD(d0);
      BEGP(pt0);
      pt0 = YREG+PREG->u.oy.y;
      d0 = *pt0;
      deref_head(d0, unify_y_loc_unk);
    unify_y_loc_nonvar:
      *SREG++ = d0;
      PREG = NEXTOP(PREG, oy);
      GONextW();

      derefa_body(d0, pt0, unify_y_loc_unk, unify_y_loc_nonvar);
      /* d0 is a variable, check whether we need to globalise it */
      PREG = NEXTOP(PREG, oy);
      if (pt0 < H) {
	/* variable is global */
	*SREG++ = Unsigned(pt0);
	GONextW();
      }
      else {
	/* create a new Heap variable and bind our variable to it */
	CACHE_S();
	READ_IN_S();
	Bind_Local(pt0, Unsigned(S_SREG));
	RESET_VARIABLE(S_SREG);
	WRITEBACK_S(S_SREG+1);
	ENDCACHE_S();
	GONextW();
      }
      ENDP(pt0);
      ENDD(d0);
      ENDOpW();

      Op(unify_l_y_loc, oy);
      /* else we are in read mode */
      BEGD(d0);
      BEGD(d1);
      BEGP(pt0);
      pt0 = SREG;
      d0 = *pt0;
      deref_head(d0, ulvaly_loc_unk);

    ulvaly_loc_nonvar:
      /* structure is bound */
      BEGP(pt1);
      pt1 = YREG+PREG->u.oy.y;
      d1 = *pt1;
      deref_head(d1, ulvaly_loc_nonvar_unk);

    ulvaly_loc_nonvar_nonvar:
      /* both arguments are bound */
      /* we may have to bind structures */
      PREG = NEXTOP(PREG, oy);
      UnifyBound(d0, d1);

      /* deref second argument */
      derefa_body(d1, pt1, ulvaly_loc_nonvar_unk, ulvaly_loc_nonvar_nonvar);
      /* first argument bound, second unbound */
      PREG = NEXTOP(PREG, oy);
      BIND(pt1, d0, bind_ulvaly_loc_nonvar_var);
#ifdef COROUTINING
      DO_TRAIL(pt1, d0);
      if (pt1 < H0) Yap_WakeUp(pt1);
  bind_ulvaly_loc_nonvar_var:
#endif
      GONext();
      ENDP(pt1);

      /* first argument may be unbound */
      derefa_body(d0, pt0, ulvaly_loc_unk, ulvaly_loc_nonvar);
      /* first argument is unbound */
      BEGP(pt1);
      pt1 = YREG+PREG->u.oy.y;
      d1 = *pt1;
      deref_head(d1, ulvaly_loc_var_unk);

    ulvaly_loc_var_nonvar:
      /* first unbound, second bound */
      PREG = NEXTOP(PREG, oy);
      BIND_GLOBAL(pt0, d1, bind_ulvaly_loc_var_nonvar);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
  bind_ulvaly_loc_var_nonvar:
#endif
      GONext();

      /* Here we are in trouble: we have a clash between pt1 and
       * SREG. We address this by storing SREG in d0 for the duration. */
      derefa_body(d1, pt1, ulvaly_loc_var_unk, ulvaly_loc_var_nonvar);
      /* both arguments are unbound */
      PREG = NEXTOP(PREG, oy);
      UnifyGlobalRegCells(pt0, pt1, uc27, uc28);
#ifdef COROUTINING
      DO_TRAIL(pt0, (CELL)pt1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    uc27:
#endif
      GONext();
#ifdef COROUTINING
    uc28:
      DO_TRAIL(pt1, (CELL)pt0);
      if (pt1 < H0) Yap_WakeUp(pt1);
      GONext();
#endif
      ENDP(pt1);
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(unify_l_y_loc_write, oy);
      /* we are in write mode */
      BEGD(d0);
      BEGP(pt0);
      pt0 = YREG+PREG->u.oy.y;
      d0 = *pt0;
      deref_head(d0, ulunify_y_loc_unk);
    ulunify_y_loc_nonvar:
      SREG[0] = d0;
      PREG = NEXTOP(PREG, oy);
      GONext();

      derefa_body(d0, pt0, ulunify_y_loc_unk, ulunify_y_loc_nonvar);
      /* d0 is a variable, check whether we need to globalise it */
      PREG = NEXTOP(PREG, oy);
      if (pt0 < H) {
	/* variable is global */
	SREG[0] = Unsigned(pt0);
	GONext();
      }
      else {
	/* create a new Heap variable and bind our variable to it */
	CACHE_S();
	READ_IN_S();
	Bind_Local(pt0, Unsigned(S_SREG));
	RESET_VARIABLE(S_SREG);
	ENDCACHE_S();
	GONext();
      }
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(unify_void, o);
      SREG++;
      PREG = NEXTOP(PREG, o);
      GONext();
      ENDOp();

      OpW(unify_void_write, o);
      CACHE_S();
      READ_IN_S();
      PREG = NEXTOP(PREG, o);
      RESET_VARIABLE(S_SREG);
      WRITEBACK_S(S_SREG+1);
      ENDCACHE_S();
      GONextW();
      ENDOpW();

      Op(unify_l_void, o);
      PREG = NEXTOP(PREG, o);
      GONext();
      ENDOp();

      Op(unify_l_void_write, o);
      PREG = NEXTOP(PREG, o);
      RESET_VARIABLE(SREG);
      GONext();
      ENDOp();

      Op(unify_n_voids, os);
      SREG += PREG->u.os.s;
      PREG = NEXTOP(PREG, os);
      GONext();
      ENDOp();

      OpW(unify_n_voids_write, os);
      BEGD(d0);
      CACHE_S();
      d0 = PREG->u.os.s;
      READ_IN_S();
      PREG = NEXTOP(PREG, os);
      for (; d0 > 0; d0--) {
	RESET_VARIABLE(S_SREG);
	S_SREG++;
      }
      WRITEBACK_S(S_SREG);
      ENDCACHE_S();
      ENDD(d0);
      GONextW();
      ENDOpW();

      Op(unify_l_n_voids, os);
      PREG = NEXTOP(PREG, os);
      GONext();
      ENDOp();

      Op(unify_l_n_voids_write, os);
      BEGD(d0);
      d0 = PREG->u.os.s;
      PREG = NEXTOP(PREG, os);
      CACHE_S();
      READ_IN_S();
      for (; d0 > 0; d0--) {
	RESET_VARIABLE(S_SREG);
	S_SREG++;
      }
      ENDCACHE_S();
      ENDD(d0);
      GONext();
      ENDOp();

      Op(unify_atom, oc);
      BEGD(d0);
      BEGP(pt0);
      pt0 = SREG++;
      d0 = *pt0;
      deref_head(d0, uatom_unk);
    uatom_nonvar:
      if (d0 != PREG->u.oc.c) {
	FAIL();
      }
      PREG = NEXTOP(PREG, oc);
      GONext();

      derefa_body(d0, pt0, uatom_unk, uatom_nonvar);
      d0 = PREG->u.oc.c;
      PREG = NEXTOP(PREG, oc);
      BIND_GLOBAL(pt0, d0, bind_uatom);
#ifdef COROUTINING
      DO_TRAIL(pt0, d0);
      if (pt0 < H0) Yap_WakeUp(pt0);
  bind_uatom:
#endif
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      OpW(unify_atom_write, oc);
      * SREG++ = PREG->u.oc.c;
      PREG = NEXTOP(PREG, oc);
      GONextW();
      ENDOpW();

      Op(unify_l_atom, oc);
      BEGD(d0);
      BEGP(pt0);
      pt0 = SREG;
      d0 = *SREG;
      deref_head(d0, ulatom_unk);
    ulatom_nonvar:
      if (d0 != PREG->u.oc.c) {
	FAIL();
      }
      PREG = NEXTOP(PREG, oc);
      GONext();

      derefa_body(d0, pt0, ulatom_unk, ulatom_nonvar);
      d0 = PREG->u.oc.c;
      PREG = NEXTOP(PREG, oc);
      BIND_GLOBAL(pt0, d0, bind_ulatom);
#ifdef COROUTINING
      DO_TRAIL(pt0, d0);
      if (pt0 < H0) Yap_WakeUp(pt0);
  bind_ulatom:
#endif
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(unify_l_atom_write, oc);
      SREG[0] = PREG->u.oc.c;
      PREG = NEXTOP(PREG, oc);
      GONext();
      ENDOp();

      Op(unify_n_atoms, osc);
      {
	register Int i = PREG->u.osc.s;		/* not enough registers */

	BEGD(d1);
	d1 = PREG->u.osc.c;
	for (; i > 0; i--) {
	  BEGD(d0);
	  BEGP(pt0);
	  pt0 = SREG++;
	  d0 = *pt0;
	  deref_head(d0, uatom_n_var);
	uatom_n_nonvar:
	  if (d0 != d1) {
	    FAIL();
	  }
	  continue;

	  derefa_body(d0, pt0, uatom_n_var, uatom_n_nonvar);
	  BIND_GLOBAL(pt0, d1, bind_unlatom);
#ifdef COROUTINING
	  DO_TRAIL(pt0, d1);
	  if (pt0 < H0) Yap_WakeUp(pt0);
	bind_unlatom:
	  continue;
#endif
	  ENDP(pt0);
	  ENDD(d0);
	}
	ENDD(d1);
      }
      PREG = NEXTOP(PREG, osc);
      GONext();
      ENDOp();

      OpW(unify_n_atoms_write, osc);
      BEGD(d0);
      BEGD(d1);
      d0 = PREG->u.osc.s;
      d1 = PREG->u.osc.c;
      /* write N atoms */
      CACHE_S();
      READ_IN_S();
      PREG = NEXTOP(PREG, osc);
      for (; d0 > 0; d0--)
	*S_SREG++ = d1;
      WRITEBACK_S(S_SREG);
      ENDCACHE_S();
      ENDD(d1);
      ENDD(d0);
      GONextW();
      ENDOpW();

      Op(unify_float, od);
      BEGD(d0);
      BEGP(pt0);
      pt0 = SREG++;
      d0 = *pt0;
      deref_head(d0, ufloat_unk);
    ufloat_nonvar:
      if (!IsApplTerm(d0)) {
	FAIL();	
      }
      /* look inside term */
      BEGP(pt0);
      pt0 = RepAppl(d0);
      BEGD(d0);
      d0 = *pt0;
      if (d0 != (CELL)FunctorDouble) {
	FAIL();
      }
      ENDD(d0);
      BEGP(pt1);
      pt1 = PREG->u.od.d;
      PREG = NEXTOP(PREG, od);
      if (
	  pt1[1] != pt0[1]
#if SIZEOF_DOUBLE == 2*SIZEOF_INT_P
	  || pt1[2] != pt0[2]
#endif
	  ) FAIL();
      ENDP(pt1);
      ENDP(pt0);
      GONext();

      derefa_body(d0, pt0, ufloat_unk, ufloat_nonvar);
      BEGD(d1);
      d1 = AbsAppl(PREG->u.od.d);
      PREG = NEXTOP(PREG, od);
      BIND_GLOBAL(pt0, d1, bind_ufloat);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
  bind_ufloat:
#endif
      GONext();
      ENDD(d1);
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      OpW(unify_float_write, od);
      * SREG++ = AbsAppl(PREG->u.od.d);
      PREG = NEXTOP(PREG, od);
      GONextW();
      ENDOpW();

      Op(unify_l_float, od);
      BEGD(d0);
      CACHE_S();
      READ_IN_S();
      d0 = *S_SREG;
      deref_head(d0, ulfloat_unk);
    ulfloat_nonvar:
      if (!IsApplTerm(d0)) {
	FAIL();	
      }
      BEGP(pt0);
      pt0 = RepAppl(d0);
      BEGD(d0);
      d0 = *pt0;
      if (d0 != (CELL)FunctorDouble) {
	FAIL();
      }
      ENDD(d0);
      BEGP(pt1);
      pt1 = PREG->u.od.d;
      PREG = NEXTOP(PREG, od);
      if (
	  pt1[1] != pt0[1]
#if SIZEOF_DOUBLE == 2*SIZEOF_INT_P
	  || pt1[2] != pt0[2]
#endif
	  ) FAIL();
      ENDP(pt1);
      ENDP(pt0);
      GONext();

      derefa_body(d0, S_SREG, ulfloat_unk, ulfloat_nonvar);
      BEGD(d1);
      d1 = AbsAppl(PREG->u.od.d);
      PREG = NEXTOP(PREG, od);
      BIND_GLOBAL(S_SREG, d1, bind_ulfloat);
#ifdef COROUTINING
      DO_TRAIL(S_SREG, d1);
      if (S_SREG < H0) Yap_WakeUp(S_SREG);
  bind_ulfloat:
#endif
      GONext();
      ENDD(d1);
      ENDCACHE_S();
      ENDD(d0);
      ENDOp();

      Op(unify_l_float_write, od);
      SREG[0] = AbsAppl(PREG->u.od.d);
      PREG = NEXTOP(PREG, od);
      GONext();
      ENDOp();

      Op(unify_longint, oi);
      BEGD(d0);
      BEGP(pt0);
      pt0 = SREG++;
      d0 = *pt0;
      deref_head(d0, ulongint_unk);
    ulongint_nonvar:
      /* look inside term */
      if (!IsApplTerm(d0)) {
	FAIL();	
      }
      BEGP(pt0);
      pt0 = RepAppl(d0);
      BEGD(d0);
      d0 = *pt0;
      if (d0 != (CELL)FunctorLongInt) {
	FAIL();
      }
      ENDD(d0);
      BEGP(pt1);
      pt1 = PREG->u.oi.i;
      PREG = NEXTOP(PREG, oi);
      if (pt1[1] != pt0[1]) FAIL();
      ENDP(pt1);
      ENDP(pt0);
      GONext();

      derefa_body(d0, pt0, ulongint_unk, ulongint_nonvar);
      BEGD(d1);
      d1 = AbsAppl(PREG->u.oi.i);
      PREG = NEXTOP(PREG, oi);
      BIND_GLOBAL(pt0, d1, bind_ulongint);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
  bind_ulongint:
#endif
      GONext();
      ENDD(d1);
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      OpW(unify_longint_write, oi);
      * SREG++ = AbsAppl(PREG->u.oi.i);
      PREG = NEXTOP(PREG, oi);
      GONextW();
      ENDOpW();

      Op(unify_l_longint, oi);
      BEGD(d0);
      CACHE_S();
      READ_IN_S();
      d0 = *S_SREG;
      deref_head(d0, ullongint_unk);
    ullongint_nonvar:
      if (!IsApplTerm(d0)) {
	FAIL();	
      }
      BEGP(pt0);
      pt0 = RepAppl(d0);
      BEGD(d0);
      d0 = *pt0;
      if (d0 != (CELL)FunctorLongInt) {
	FAIL();
      }
      ENDD(d0);
      BEGP(pt1);
      pt1 = PREG->u.oi.i;
      PREG = NEXTOP(PREG, oi);
      if (pt1[1] != pt0[1]) FAIL();
      ENDP(pt1);
      ENDP(pt0);
      GONext();

      derefa_body(d0, S_SREG, ullongint_unk, ullongint_nonvar);
      BEGD(d1);
      d1 = AbsAppl(PREG->u.oi.i);
      PREG = NEXTOP(PREG, oi);
      BIND_GLOBAL(S_SREG, d1, bind_ullongint);
#ifdef COROUTINING
      DO_TRAIL(S_SREG, d1);
      if (S_SREG < H0) Yap_WakeUp(S_SREG);
  bind_ullongint:
#endif
      GONext();
      ENDD(d1);
      ENDCACHE_S();
      ENDD(d0);
      ENDOp();

      Op(unify_l_longint_write, oi);
      SREG[0] = AbsAppl(PREG->u.oi.i);
      PREG = NEXTOP(PREG, oi);
      GONext();
      ENDOp();

      Op(unify_bigint, oc);
#ifdef USE_GMP
      BEGD(d0);
      BEGP(pt0);
      pt0 = SREG++;
      d0 = *pt0;
      deref_head(d0, ubigint_unk);
    ubigint_nonvar:
      /* look inside term */
      if (!IsApplTerm(d0)) {
	FAIL();	
      }
      BEGP(pt0);
      pt0 = RepAppl(d0);
      BEGD(d1);
      d1 = *pt0;
      if (d1 != (CELL)FunctorBigInt)
      {
	FAIL();
      }
      ENDD(d1);
      if (mpz_cmp(Yap_BigIntOfTerm(d0),Yap_BigIntOfTerm(PREG->u.oc.c)))
	FAIL();
      PREG = NEXTOP(PREG, oc);
      ENDP(pt0);
      GONext();

      derefa_body(d0, pt0, ubigint_unk, ubigint_nonvar);
      BEGD(d1);
      d1 = PREG->u.oc.c;
      PREG = NEXTOP(PREG, oi);
      BIND_GLOBAL(pt0, d1, bind_ubigint);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
  bind_ubigint:
#endif
      GONext();
      ENDD(d1);
      ENDP(pt0);
      ENDD(d0);
#else
      FAIL();
#endif
      ENDOp();

      Op(unify_l_bigint, oc);
#ifdef USE_GMP
      BEGD(d0);
      CACHE_S();
      READ_IN_S();
      d0 = *S_SREG;
      deref_head(d0, ulbigint_unk);
    ulbigint_nonvar:
      if (!IsApplTerm(d0)) {
	FAIL();	
      }
      BEGP(pt0);
      pt0 = RepAppl(d0);
      BEGD(d0);
      d0 = *pt0;
      if (d0 != (CELL)FunctorBigInt)
      {
	FAIL();
      }
      ENDD(d0);
      if (mpz_cmp(Yap_BigIntOfTerm(d0),Yap_BigIntOfTerm(PREG->u.oc.c)))
	FAIL();
      PREG = NEXTOP(PREG, oc);
      ENDP(pt0);
      GONext();

      derefa_body(d0, S_SREG, ulbigint_unk, ulbigint_nonvar);
      BEGD(d1);
      d1 = PREG->u.oc.c;
      PREG = NEXTOP(PREG, oc);
      BIND_GLOBAL(S_SREG, d1, bind_ulbigint);
#ifdef COROUTINING
      DO_TRAIL(S_SREG, d1);
      if (S_SREG < H0) Yap_WakeUp(S_SREG);
  bind_ulbigint:
#endif
      GONext();
      ENDD(d1);
      ENDCACHE_S();
      ENDD(d0);
#else
      FAIL();
#endif
      ENDOp();

      Op(unify_dbterm, oc);
      BEGD(d0);
      BEGP(pt0);
      pt0 = SREG++;
      d0 = *pt0;
      deref_head(d0, udbterm_unk);
    udbterm_nonvar:
      BEGD(d1);
      /* we have met a preexisting dbterm */
      d1 = PREG->u.oc.c;
      PREG = NEXTOP(PREG, oc);
      UnifyBound(d0,d1);
      ENDD(d1);

      derefa_body(d0, pt0, udbterm_unk, udbterm_nonvar);
      BEGD(d1);
      d1 = AbsAppl(PREG->u.oi.i);
      PREG = NEXTOP(PREG, oi);
      BIND_GLOBAL(pt0, d1, bind_udbterm);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
  bind_udbterm:
#endif
      GONext();
      ENDD(d1);
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(unify_l_dbterm, oc);
      BEGD(d0);
      CACHE_S();
      READ_IN_S();
      d0 = *S_SREG;
      deref_head(d0, uldbterm_unk);
    uldbterm_nonvar:
      BEGD(d1);
      /* we have met a preexisting dbterm */
      d1 = PREG->u.oc.c;
      PREG = NEXTOP(PREG, oc);
      UnifyBound(d0,d1);
      ENDD(d1);

      derefa_body(d0, S_SREG, uldbterm_unk, uldbterm_nonvar);
      BEGD(d1);
      d1 = PREG->u.oc.c;
      PREG = NEXTOP(PREG, oc);
      BIND_GLOBAL(S_SREG, d1, bind_uldbterm);
#ifdef COROUTINING
      DO_TRAIL(S_SREG, d1);
      if (S_SREG < H0) Yap_WakeUp(S_SREG);
  bind_uldbterm:
#endif
      GONext();
      ENDD(d1);
      ENDCACHE_S();
      ENDD(d0);
      ENDOp();

      OpRW(unify_list, o);
      *--SP = Unsigned(SREG + 1);
      *--SP = READ_MODE;
      BEGD(d0);
      BEGP(pt0);
      pt0 = SREG;
      d0 = *pt0;
      deref_head(d0, ulist_unk);
    ulist_nonvar:
      if (!IsPairTerm(d0)) {
	FAIL();
      }
      /* we continue in read mode */
      START_PREFETCH(o);
      SREG = RepPair(d0);
      PREG = NEXTOP(PREG, o);
      GONext();
      END_PREFETCH();

      derefa_body(d0, pt0, ulist_unk, ulist_nonvar);
      /* we enter write mode */
      START_PREFETCH_W(o);
      CACHE_S();
      READ_IN_S();
      S_SREG = H;
      PREG = NEXTOP(PREG, o);
      H = S_SREG + 2;
      d0 = AbsPair(S_SREG);
      WRITEBACK_S(S_SREG);
      ENDCACHE_S();
      BIND_GLOBAL(pt0, d0, bind_ulist_var);
#ifdef COROUTINING
      DO_TRAIL(pt0, d0);
      if (pt0 < H0) Yap_WakeUp(pt0);
  bind_ulist_var:
#endif
      GONextW();
      END_PREFETCH_W();

      ENDP(pt0);

      ENDD(d0);
      ENDOpRW();

      OpW(unify_list_write, o);
      PREG = NEXTOP(PREG, o);
      BEGD(d0);
      d0 = AbsPair(H);
      CACHE_S();
      READ_IN_S();
      SP -= 2;
      SP[0] = WRITE_MODE;
      SP[1] = Unsigned(S_SREG + 1);
      S_SREG[0] = d0;
      S_SREG = H;
      H = S_SREG + 2;
      WRITEBACK_S(S_SREG);
      ENDCACHE_S();
      GONextW();
      ENDD(d0);
      ENDOpW();

      OpRW(unify_l_list, o);
      BEGD(d0);
      BEGP(pt0);
      pt0 = SREG;
      d0 = *pt0;
      deref_head(d0, ullist_unk);
    ullist_nonvar:
      START_PREFETCH(o);
      if (!IsPairTerm(d0)) {
	FAIL();
      }
      /* we continue in read mode */
      PREG = NEXTOP(PREG, o);
      SREG = RepPair(d0);
      GONext();
      END_PREFETCH();

      derefa_body(d0, pt0, ullist_unk, ullist_nonvar);
      /* we enter write mode */
      START_PREFETCH_W(o);
      PREG = NEXTOP(PREG, o);
      CACHE_S();
      READ_IN_S();
      S_SREG = H;
      H = S_SREG + 2;
      d0 = AbsPair(S_SREG);
      WRITEBACK_S(S_SREG);
      ENDCACHE_S();
      BIND_GLOBAL(pt0, d0, bind_ullist_var);
#ifdef COROUTINING
      DO_TRAIL(pt0, d0);
      if (pt0 < H0) Yap_WakeUp(pt0);
  bind_ullist_var:
#endif
      GONextW();
      END_PREFETCH_W();

      ENDP(pt0);
      ENDD(d0);
      ENDOpRW();

      OpW(unify_l_list_write, o);
      /* we continue in write mode */
      BEGD(d0);
      d0 = AbsPair(H);
      PREG = NEXTOP(PREG, o);
      CACHE_S();
      READ_IN_S();
      S_SREG[0] = d0;
      S_SREG = H;
      H = S_SREG + 2;
      WRITEBACK_S(S_SREG);
      ENDCACHE_S();
      GONextW();
      ENDD(d0);
      ENDOpW();

      OpRW(unify_struct, ofa);
      *--SP = Unsigned(SREG + 1);
      *--SP = READ_MODE;
      BEGD(d0);
      BEGP(pt0);
      pt0 = SREG;
      d0 = *pt0;
      START_PREFETCH(ofa);
      deref_head(d0, ustruct_unk);
    ustruct_nonvar:
      /* we are in read mode */
      if (!IsApplTerm(d0)) {
	FAIL();
      }
      CACHE_S();
      READ_IN_S();
      /* we continue in read mode */
      S_SREG = RepAppl(d0);
      /* just check functor */
      d0 = (CELL) (PREG->u.ofa.f);
      if (*S_SREG != d0) {
	FAIL();
      }
      PREG = NEXTOP(PREG, ofa);
      WRITEBACK_S(S_SREG+1);
      ENDCACHE_S();
      GONext();
      END_PREFETCH();

      derefa_body(d0, pt0, ustruct_unk, ustruct_nonvar);
      /* Enter Write mode */
      START_PREFETCH_W(ofa);
      /* set d1 to be the new structure we are going to create */
      BEGD(d1);
      d1 = AbsAppl(H);
      /* we know the variable must be in the heap */
      BIND_GLOBAL(pt0, d1, bind_ustruct);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    bind_ustruct:
#endif
      /* now, set pt0 to point to the heap where we are going to
       * build our term */
      pt0 = H;
      ENDD(d1);
      /* first, put the functor */
      d0 = (CELL) (PREG->u.ofa.f);
      *pt0++ = d0;
      H = pt0 + PREG->u.ofa.a;
      PREG = NEXTOP(PREG, ofa);
      /* set SREG */
      SREG = pt0;
      /* update H */
      GONextW();
      END_PREFETCH_W();

      ENDP(pt0);

      ENDD(d0);
      ENDOpRW();

      OpW(unify_struct_write, ofa);
      CACHE_S();
      READ_IN_S();
      *--SP = Unsigned(S_SREG + 1);
      *--SP = WRITE_MODE;
      /* we continue in write mode */
      BEGD(d0);
      d0 = AbsAppl(H);
      S_SREG[0] = d0;
      S_SREG = H;
      d0 = (CELL) (PREG->u.ofa.f);
      *S_SREG++ = d0;
      H = S_SREG + PREG->u.ofa.a;
      PREG = NEXTOP(PREG, ofa);
      WRITEBACK_S(S_SREG);
      ENDCACHE_S();
      ENDD(d0);
      GONextW();
      ENDOpW();

      OpRW(unify_l_struc, ofa);
      BEGD(d0);
      BEGP(pt0);
      pt0 = SREG;
      d0 = *pt0;
      deref_head(d0, ulstruct_unk);
    ulstruct_nonvar:
      /* we are in read mode */
      START_PREFETCH(ofa);
      if (!IsApplTerm(d0)) {
	FAIL();
      }
      /* we continue in read mode */
      SREG = RepAppl(d0);
      /* just check functor */
      d0 = (CELL) (PREG->u.ofa.f);
      if (*SREG++ != d0) {
	FAIL();
      }
      PREG = NEXTOP(PREG, ofa);
      GONext();
      END_PREFETCH();

      derefa_body(d0, pt0, ulstruct_unk, ulstruct_nonvar);
      /* Enter Write mode */
      /* set d1 to be the new structure we are going to create */
      START_PREFETCH_W(ofa);
      BEGD(d1);
      d1 = AbsAppl(H);
      /* we know the variable must be in the heap */
      BIND_GLOBAL(pt0, d1, bind_ulstruct);
#ifdef COROUTINING
      DO_TRAIL(pt0, d1);
      if (pt0 < H0) Yap_WakeUp(pt0);
    bind_ulstruct:
#endif
      /* now, set pt0 to point to the heap where we are going to
       * build our term */
      pt0 = H;
      ENDD(d1);
      /* first, put the functor */
      d0 = (CELL) (PREG->u.ofa.f);
      *pt0++ = d0;
      H = pt0 + PREG->u.ofa.a;
      PREG = NEXTOP(PREG, ofa);
      /* set SREG */
      SREG = pt0;
      /* update H */
      GONextW();
      END_PREFETCH_W();
      ENDP(pt0);

      ENDD(d0);
      ENDOpRW();

      OpW(unify_l_struc_write, ofa);
      BEGD(d0);
      d0 = AbsAppl(H);
      CACHE_S();
      READ_IN_S();
      S_SREG[0] = d0;
      S_SREG = H;
      d0 = (CELL) (PREG->u.ofa.f);
      *S_SREG++ = d0;
      H = S_SREG + PREG->u.ofa.a;
      PREG = NEXTOP(PREG, ofa);
      WRITEBACK_S(S_SREG);
      ENDCACHE_S();
      ENDD(d0);
      GONextW();
      ENDOpW();


/************************************************************************\
* Put Instructions							 *
\************************************************************************/

      Op(put_x_var, xx);
      BEGP(pt0);
      pt0 = H;
      XREG(PREG->u.xx.xl) = Unsigned(pt0);
      H = pt0 + 1;
      XREG(PREG->u.xx.xr) = Unsigned(pt0);
      PREG = NEXTOP(PREG, xx);
      RESET_VARIABLE(pt0);
      ENDP(pt0);
      GONext();
      ENDOp();

      Op(put_y_var, yx);
      BEGP(pt0);
      pt0 = YREG + PREG->u.yx.y;
      XREG(PREG->u.yx.x) = (CELL) pt0;
      PREG = NEXTOP(PREG, yx);
#if defined(SBA) && defined(FROZEN_STACKS)
      /* We must initialise a shared variable to point to the SBA */
      if (Unsigned((Int)(pt0)-(Int)(H_FZ)) >
	  Unsigned((Int)(B_FZ)-(Int)(H_FZ))) {
	*pt0 =  (CELL)STACK_TO_SBA(pt0);
      } else
#endif /* SBA && FROZEN_STACKS */
	RESET_VARIABLE(pt0);
      ENDP(pt0);
      GONext();
      ENDOp();

      Op(put_x_val, xx);
      BEGD(d0);
      d0 = XREG(PREG->u.xx.xl);
      XREG(PREG->u.xx.xr) = d0;
      ENDD(d0);
      PREG = NEXTOP(PREG, xx);
      GONext();
      ENDOp();

      Op(put_xx_val, xxxx);
      BEGD(d0);
      BEGD(d1);
      d0 = XREG(PREG->u.xxxx.xl1);
      d1 = XREG(PREG->u.xxxx.xl2);
      XREG(PREG->u.xxxx.xr1) = d0;
      XREG(PREG->u.xxxx.xr2) = d1;
      ENDD(d1);
      ENDD(d0);
      PREG = NEXTOP(PREG, xxxx);
      GONext();
      ENDOp();

      Op(put_y_val, yx);
      BEGD(d0);
      d0 = YREG[PREG->u.yx.y];
#ifdef SBA
      if (d0 == 0) /* new variable */
	XREG(PREG->u.yx.x) = (CELL)(YREG+PREG->u.yx.y);
      else
#endif
	XREG(PREG->u.yx.x) = d0;
      ENDD(d0);
      PREG = NEXTOP(PREG, yx);
      GONext();
      ENDOp();

      Op(put_y_vals, yyxx);
      ALWAYS_START_PREFETCH(yyxx);
      BEGD(d0);
      d0 = YREG[PREG->u.yyxx.y1];
#ifdef SBA
      if (d0 == 0) /* new variable */
	XREG(PREG->u.yyxx.x1) = (CELL)(YREG+PREG->u.yyxx.y1);
      else
#endif
	XREG(PREG->u.yyxx.x1) = d0;
      ENDD(d0);
      /* allow for some prefetching */
      PREG = NEXTOP(PREG, yyxx);
      BEGD(d1);
      d1 = YREG[PREVOP(PREG,yyxx)->u.yyxx.y2];
#ifdef SBA
      if (d1 == 0) /* new variable */
	XREG(PREVOP(PREG->u.yyxx,yyxx).x2) = (CELL)(YREG+PREG->u.yyxx.y2);
      else
#endif
	XREG(PREVOP(PREG,yyxx)->u.yyxx.x2) = d1;
      ENDD(d1);
      ALWAYS_END_PREFETCH();
      GONext();
      ENDOp();

      Op(put_unsafe, yx);
      BEGD(d0);
      BEGP(pt0);
      pt0 = YREG+PREG->u.yx.y;
      d0 = *pt0;
      deref_head(d0, punsafe_unk);
    punsafe_nonvar:
      XREG(PREG->u.yx.x) = d0;
      PREG = NEXTOP(PREG, yx);
      GONext();

      derefa_body(d0, pt0, punsafe_unk, punsafe_nonvar);
      /* d0 is a variable, check whether we need to globalise it */
      if (pt0 <= H || pt0 >= YREG) {
	/* variable is safe */
	XREG(PREG->u.yx.x) = Unsigned(pt0);
	PREG = NEXTOP(PREG, yx);
	GONext();
      }
      else {
	/* create a new Heap variable and bind our variable to it */
	Bind_Local(pt0, Unsigned(H));
	XREG(PREG->u.yx.x) = (CELL) H;
	RESET_VARIABLE(H);
	H++;
	PREG = NEXTOP(PREG, yx);
	GONext();
      }
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(put_atom, xc);
      BEGD(d0);
      d0 = PREG->u.xc.c;
      XREG(PREG->u.xc.x) = d0;
      PREG = NEXTOP(PREG, xc);
      GONext();
      ENDD(d0);
      ENDOp();

      Op(put_float, xd);
      BEGD(d0);
      d0 = AbsAppl(PREG->u.xd.d);
      XREG(PREG->u.xd.x) = d0;
      PREG = NEXTOP(PREG, xd);
      GONext();
      ENDD(d0);
      ENDOp();

      Op(put_longint, xi);
      BEGD(d0);
      d0 = AbsAppl(PREG->u.xi.i);
      XREG(PREG->u.xi.x) = d0;
      PREG = NEXTOP(PREG, xi);
      GONext();
      ENDD(d0);
      ENDOp();

      Op(put_list, x);
      CACHE_S();
      READ_IN_S();
      S_SREG = H;
      H += 2;
      BEGD(d0);
      d0 = AbsPair(S_SREG);
      XREG(PREG->u.x.x) = d0;
      PREG = NEXTOP(PREG, x);
      ENDD(d0);
      WRITEBACK_S(S_SREG);
      ENDCACHE_S();
      GONext();
      ENDOp();

      Op(put_struct, xfa);
      BEGD(d0);
      d0 = AbsAppl(H);
      XREG(PREG->u.xfa.x) = d0;
      d0 = (CELL) (PREG->u.xfa.f);
      *H++ = d0;
      SREG = H;
      H += PREG->u.xfa.a;
      ENDD(d0);
      PREG = NEXTOP(PREG, xfa);
      GONext();
      ENDOp();

/************************************************************************\
* 	Write Instructions						*
\************************************************************************/

      Op(write_x_var, x);
      XREG(PREG->u.x.x) = Unsigned(SREG);
      PREG = NEXTOP(PREG, x);
      RESET_VARIABLE(SREG);
      SREG++;
      GONext();
      ENDOp();

      Op(write_void, e);
      PREG = NEXTOP(PREG, e);
      RESET_VARIABLE(SREG);
      SREG++;
      GONext();
      ENDOp();

      Op(write_n_voids, s);
      BEGD(d0);
      d0 = PREG->u.s.s;
      PREG = NEXTOP(PREG, s);
      for (; d0 > 0; d0--) {
	RESET_VARIABLE(SREG);
	SREG++;
      }
      ENDD(d0);
      GONext();
      ENDOp();

      Op(write_y_var, y);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(YREG+PREG->u.y.y,Unsigned(SREG));
#else
      YREG[PREG->u.y.y] = Unsigned(SREG);
#endif /* SBA && FROZEN_STACKS */
      PREG = NEXTOP(PREG, y);
      RESET_VARIABLE(SREG);
      SREG++;
      GONext();
      ENDOp();

      Op(write_x_val, x);
      BEGD(d0);
      d0 = XREG(PREG->u.x.x);
      *SREG++ = d0;
      ENDD(d0);
      PREG = NEXTOP(PREG, x);
      GONext();
      ENDOp();

      Op(write_x_loc, x);
      BEGD(d0);
      d0 = XREG(PREG->u.x.x);
      PREG = NEXTOP(PREG, x);
      deref_head(d0, w_x_unk);
    w_x_bound:
      *SREG++ = d0;
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, w_x_unk, w_x_bound);
#if defined(SBA) && defined(FROZEN_STACKS)
      if (pt0 > H && pt0<(CELL *)B_FZ) {
#else
      if (pt0 > H) {
#endif /* SBA && FROZEN_STACKS */
	/* local variable: let us bind it to the list */
#ifdef FROZEN_STACKS  /* TRAIL */
	Bind_Local(pt0, Unsigned(SREG));
#else
	TRAIL_LOCAL(pt0, Unsigned(SREG));
	*pt0 = Unsigned(SREG);
#endif /* FROZEN_STACKS */
	RESET_VARIABLE(SREG);
	SREG++;
	GONext();
      }
      else {
	*SREG++ = Unsigned(pt0);
	GONext();
      }
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(write_y_val, y);
      BEGD(d0);
      d0 = YREG[PREG->u.y.y];
#ifdef SBA
      if (d0 == 0) /* new variable */
	*SREG++ = (CELL)(YREG+PREG->u.y.y);
      else
#endif
	*SREG++ = d0;
      ENDD(d0);
      PREG = NEXTOP(PREG, y);
      GONext();
      ENDOp();

      Op(write_y_loc, y);
      BEGD(d0);
      BEGP(pt0);
      pt0 = YREG+PREG->u.y.y;
      d0 = *pt0;
      deref_head(d0, w_y_unk);
    w_y_bound:
      PREG = NEXTOP(PREG, y);
      *SREG++ = d0;
      GONext();

      derefa_body(d0, pt0, w_y_unk, w_y_bound);
      if (pt0 > H
#if defined(SBA) && defined(FROZEN_STACKS)
	  && pt0<(CELL *)B_FZ
#endif /* SBA && FROZEN_STACKS */
	  ) {
	PREG = NEXTOP(PREG, y);
	/* local variable: let us bind it to the list */
#ifdef FROZEN_STACKS
	Bind_Local(pt0, Unsigned(SREG));
#else
	*pt0 = Unsigned(SREG);
	TRAIL_LOCAL(pt0, Unsigned(SREG));
#endif /* FROZEN_STACKS */
	RESET_VARIABLE(SREG);
	SREG++;
	GONext();
      } else {
	PREG = NEXTOP(PREG, y);
	*SREG++ = Unsigned(pt0);
	GONext();
      }
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(write_atom, c);
      BEGD(d0);
      d0 = PREG->u.c.c;
      *SREG++ = d0;
      ENDD(d0);
      PREG = NEXTOP(PREG, c);
      GONext();
      ENDOp();

      Op(write_float, d);
      BEGD(d0);
      d0 = AbsAppl(PREG->u.d.d);
      *SREG++ = d0;
      ENDD(d0);
      PREG = NEXTOP(PREG, d);
      GONext();
      ENDOp();

      Op(write_longint, i);
      BEGD(d0);
      d0 = AbsAppl(PREG->u.i.i);
      *SREG++ = d0;
      ENDD(d0);
      PREG = NEXTOP(PREG, i);
      GONext();
      ENDOp();

      Op(write_n_atoms, sc);
      BEGD(d0);
      BEGD(d1);
      d0 = PREG->u.sc.s;
      d1 = PREG->u.sc.c;
      for (; d0 > 0; d0--)
	*SREG++ = d1;
      ENDD(d1);
      ENDD(d0);
      PREG = NEXTOP(PREG, sc);
      GONext();
      ENDOp();

      Op(write_list, e);
      BEGD(d0);
      d0 = AbsPair(H);
      *SREG++ = d0;
      /* I will not actually store the mode in the stack */
      SP[-1] = Unsigned(SREG);
      SP[-2] = 1;		/* Put instructions follow the main stream */
      SP -= 2;
      SREG = H;
      H += 2;
      ENDD(d0);
      PREG = NEXTOP(PREG, e);
      GONext();
      ENDOp();

      Op(write_l_list, e);
      ALWAYS_START_PREFETCH(e);
      PREG = NEXTOP(PREG, e);
      BEGD(d0);
      CACHE_S();
      READ_IN_S();
      d0 = AbsPair(H);
      *S_SREG = d0;
      WRITEBACK_S(H);
      H += 2;
      ENDCACHE_S();
      ENDD(d0);
      ALWAYS_END_PREFETCH();
      GONext();
      ENDOp();

      Op(write_struct, fa);
      BEGD(d0);
      d0 = AbsAppl(H);
      *SREG++ = d0;
      SP[-1] = Unsigned(SREG);
      SP[-2] = 1;		/* Put instructions follow the main stream */
      SP -= 2;
      d0 = (CELL) (PREG->u.fa.f);
      *H++ = d0;
      ENDD(d0);
      BEGD(d0);
      d0 = PREG->u.fa.a;
      PREG = NEXTOP(PREG, fa);
      SREG = H;
      H += d0;
      ENDD(d0);
      GONext();
      ENDOp();

      Op(write_l_struc, fa);
      BEGD(d0);
      d0 = AbsAppl(H);
      *SREG = d0;
      d0 = (CELL) (PREG->u.fa.f);
      *H++ = d0;
      SREG = H;
      ENDD(d0);
      BEGD(d0);
      d0 = PREG->u.fa.a;
      PREG = NEXTOP(PREG, fa);
      H += d0;
      ENDD(d0);
      GONext();
      ENDOp();

/************************************************************************\
*   Save last unified struct or list					*
\************************************************************************/

/* vitor: I think I should kill these two instructions, by expanding the
 * othe instructions.
 */

      Op(save_pair_x, ox);
      XREG(PREG->u.ox.x) = AbsPair(SREG);
      PREG = NEXTOP(PREG, ox);
      GONext();
      ENDOp();

      OpW(save_pair_x_write, ox);
      XREG(PREG->u.ox.x) = AbsPair(SREG);
      PREG = NEXTOP(PREG, ox);
      GONextW();
      ENDOpW();

      Op(save_pair_y, oy);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(YREG+PREG->u.oy.y,AbsPair(SREG));
#else
      YREG[PREG->u.oy.y] = AbsPair(SREG);
#endif /* SBA && FROZEN_STACKS */
      PREG = NEXTOP(PREG, oy);
      GONext();
      ENDOp();

      OpW(save_pair_y_write, oy);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(YREG+PREG->u.oy.y,AbsPair(SREG));
#else
      YREG[PREG->u.oy.y] = AbsPair(SREG);
#endif /* SBA && FROZEN_STACKS */
      PREG = NEXTOP(PREG, oy);
      GONextW();
      ENDOpW();

      Op(save_appl_x, ox);
      XREG(PREG->u.ox.x) = AbsAppl(SREG - 1);
      PREG = NEXTOP(PREG, ox);
      GONext();
      ENDOp();

      OpW(save_appl_x_write, ox);
      XREG(PREG->u.ox.x) = AbsAppl(SREG - 1);
      PREG = NEXTOP(PREG, ox);
      GONextW();
      ENDOpW();

      Op(save_appl_y, oy);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(YREG+PREG->u.oy.y,AbsAppl(SREG-1));
#else
      YREG[PREG->u.oy.y] = AbsAppl(SREG - 1);
#endif /* SBA && FROZEN_STACKS */
      PREG = NEXTOP(PREG, oy);
      GONext();
      ENDOp();

      OpW(save_appl_y_write, oy);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(YREG+PREG->u.oy.y,AbsAppl(SREG-1));
#else
      YREG[PREG->u.oy.y] = AbsAppl(SREG - 1);
#endif /* SBA && FROZEN_STACKS */
      PREG = NEXTOP(PREG, oy);
      GONextW();
      ENDOpW();


/************************************************************************\
*   Instructions for implemeting 'or;'					 *
\************************************************************************/

      BOp(jump, l);
      PREG = PREG->u.l.l;
      JMPNext();
      ENDBOp();

      /* This instruction is called when the previous goal
	 was interrupted when waking up goals
      */	 
      BOp(move_back, l);
      PREG = (yamop *)(((char *)PREG)-(Int)(NEXTOP((yamop *)NULL,Osbpp)));
      JMPNext();
      ENDBOp();

      /* This instruction is called when the previous goal
	 was interrupted when waking up goals
      */	 
      BOp(skip, l);
      PREG = NEXTOP(PREG,l);
      JMPNext();
      ENDBOp();

      Op(either, Osblp);
#ifdef LOW_LEVEL_TRACER
      if (Yap_do_low_level_trace) {
	low_level_trace(try_or, (PredEntry *)PREG, NULL);
      }
#endif
#ifdef COROUTINING
      CACHE_Y_AS_ENV(YREG);
      check_stack(NoStackEither, H);
      ENDCACHE_Y_AS_ENV();
      either_notest:
#endif
      BEGD(d0);
      /* Try to preserve the environment */
      d0 = PREG->u.Osblp.s;
      BEGCHO(pt1);
      pt1 = (choiceptr) ((char *) YREG + (yslot) d0);
#ifdef FROZEN_STACKS
      { 
	choiceptr top_b = PROTECT_FROZEN_B(B);
#ifdef SBA
	if (pt1 > top_b || pt1 < (choiceptr)H) pt1 = top_b;
#else
	if (pt1 > top_b) pt1 = top_b;
#endif /* SBA */
      }
#else
      if (pt1 > B) {
	pt1 = B;
      }
#endif /* FROZEN_STACKS */
      pt1 = (choiceptr)(((CELL *) pt1)-1);
      *(CELL **) pt1 = YREG;
      store_yaam_regs_for_either(PREG->u.Osblp.l, PREG);
      SREG = (CELL *) (B = pt1);
#ifdef YAPOR
      SCH_set_load(pt1);
#endif	/* YAPOR */
      SET_BB(pt1);
      ENDCHO(pt1);
      /* skip the current instruction plus the next one */
      PREG = NEXTOP(NEXTOP(PREG, Osblp),l);
      GONext();
      ENDD(d0);
      ENDOp();

      Op(or_else, Osblp);
      H = HBREG = PROTECT_FROZEN_H(B);
      ENV = B->cp_env;
      B->cp_cp = PREG;
#ifdef DEPTH_LIMIT
      DEPTH = B->cp_depth;
#endif	/* DEPTH_LIMIT */
      SET_BB(PROTECT_FROZEN_B(B));
#ifdef YAPOR
      if (SCH_top_shared_cp(B)) {
	SCH_new_alternative(PREG, PREG->u.Osblp.l);
      } else
#endif	/* YAPOR */
      B->cp_ap = PREG->u.Osblp.l;
      PREG = NEXTOP(PREG, Osblp);
      YREG = (CELL *) B->cp_a1;
      GONext();
      ENDOp();

#ifdef YAPOR
      Op(or_last, Osblp);
#else
      Op(or_last, p);
#endif	/* YAPOR */
      BEGCHO(pt0);
      pt0 = B;
#ifdef YAPOR
      if (SCH_top_shared_cp(B)) {
	H = HBREG = PROTECT_FROZEN_H(pt0);
	YREG = (CELL *) pt0->cp_a1;
	ENV = pt0->cp_env;
#ifdef DEPTH_LIMIT
	DEPTH = pt0->cp_depth;
#endif	/* DEPTH_LIMIT */
        SCH_new_alternative(PREG, NULL);
      }
      else
#endif	/* YAPOR */
      {
	B = pt0->cp_b;
	H = PROTECT_FROZEN_H(pt0);
	YREG = (CELL *) pt0->cp_a1;
	ENV = pt0->cp_env;
#ifdef DEPTH_LIMIT
	DEPTH = pt0->cp_depth;
#endif	/* DEPTH_LIMIT */
	HBREG = PROTECT_FROZEN_H(B);
      }
#ifdef YAPOR
      PREG = NEXTOP(PREG, Osblp);
#else
      PREG = NEXTOP(PREG, p);
#endif	/* YAPOR */
      SET_BB(PROTECT_FROZEN_B(B));
      GONext();
      ENDCHO(pt0);
      ENDOp();

/************************************************************************\
*	Pop operations							 *
\************************************************************************/

      OpRW(pop_n, s);
      /* write mode might have been called from read mode */
      BEGD(d0);
      d0 = PREG->u.os.s;
      SP = (CELL *) (((char *) SP) + d0);
      ENDD(d0);
      BEGD(d0);
      d0 = SP[0];
      if (d0) {
	START_PREFETCH(s);
	SREG = (CELL *) (SP[1]);
	SP += 2;
	PREG = NEXTOP(PREG, s);
	GONext();
	END_PREFETCH();
      }
      else {
	START_PREFETCH_W(s);
	SREG = (CELL *) (SP[1]);
	SP += 2;
	PREG = NEXTOP(PREG, s);
	GONextW();
	END_PREFETCH_W();
      }
      ENDD(d0);
      ENDOpRW();

      OpRW(pop, e);
      BEGD(d0);
      d0 = SP[0];
      SREG = (CELL *) (SP[1]);
      SP += 2;
      if (d0) {
	START_PREFETCH(e);
	PREG = NEXTOP(PREG, e);
	GONext();
	END_PREFETCH();
      }
      else {
	START_PREFETCH_W(e);
	PREG = NEXTOP(PREG, e);
	GONextW();
	END_PREFETCH_W();
      }
      ENDD(d0);
      ENDOpRW();

/************************************************************************\
*	Call C predicates instructions					 *
\************************************************************************/

      BOp(call_cpred, Osbpp);
      check_trail(TR);
      if (!(PREG->u.Osbpp.p->PredFlags & (SafePredFlag|HiddenPredFlag))) {
	CACHE_Y_AS_ENV(YREG);
	check_stack(NoStackCall, H);
	ENDCACHE_Y_AS_ENV();
      }
#ifdef FROZEN_STACKS
      { 
	choiceptr top_b = PROTECT_FROZEN_B(B);

#ifdef SBA
	if (YREG > (CELL *) top_b || YREG < H) ASP = (CELL *)top_b;
#else
	if (YREG > (CELL *) top_b) ASP = (CELL *)top_b;
#endif /* SBA */
	else ASP = (CELL *)(((char *)YREG) +  PREG->u.Osbpp.s);
      }
#else
      if (YREG > (CELL *) B) {
	ASP = (CELL *) B;
      } else {
	ASP = (CELL *) (((char *) YREG) + PREG->u.Osbpp.s);
      }
      /* for slots to work */
#endif /* FROZEN_STACKS */
#ifdef LOW_LEVEL_TRACER
      if (Yap_do_low_level_trace)
	low_level_trace(enter_pred,PREG->u.Osbpp.p,XREGS+1);
#endif	/* LOW_LEVEL_TRACE */
      BEGD(d0);
      CPredicate f = PREG->u.Osbpp.p->cs.f_code;
      PREG = NEXTOP(PREG, Osbpp);
      saveregs();
      d0 = (f)();
      setregs();
#ifdef SHADOW_S
      SREG = Yap_REGS.S_;
#endif
      if (!d0) {
	FAIL();
      }
      CACHE_A1();
      ENDD(d0);
      JMPNext();
      ENDBOp();
      
      /* execute     Label               */
      BOp(execute_cpred, pp);
      check_trail(TR);
      {
	PredEntry *pt0;

	BEGD(d0);
	CACHE_Y_AS_ENV(YREG);
#ifdef FROZEN_STACKS
	{ 
	  choiceptr top_b = PROTECT_FROZEN_B(B);

#ifdef SBA
	  if (YREG > (CELL *) top_b || YREG < H) ASP = (CELL *)top_b;
#else
	  if (YREG > (CELL *) top_b) ASP = (CELL *)top_b;
#endif /* SBA */
	  else ASP = YREG+E_CB;
	}
#else
	if (YREG > (CELL *) B) {
	  ASP = (CELL *) B;
	} else {
	  ASP = YREG+E_CB;
	}
	/* for slots to work */
#endif /* FROZEN_STACKS */
	pt0 = PREG->u.pp.p;
#ifdef LOW_LEVEL_TRACER
	if (Yap_do_low_level_trace) {
	  low_level_trace(enter_pred,pt0,XREGS+1);
	}
#endif	/* LOW_LEVEL_TRACE */
	CACHE_A1();
	BEGD(d0);
	d0 = (CELL)B;
#ifndef NO_CHECKING
	check_stack(NoStackExecute, H);
#endif
	/* for profiler */
	save_pc();
	ENV_YREG[E_CB] = d0;
	ENDD(d0);
#ifdef DEPTH_LIMIT
	if (DEPTH <= MkIntTerm(1)) {/* I assume Module==0 is prolog */
	  if (pt0->ModuleOfPred) {
	    if (DEPTH == MkIntTerm(0))
	      FAIL();
	    else DEPTH = RESET_DEPTH();
	  }
	} else if (pt0->ModuleOfPred) {
	  DEPTH -= MkIntConstant(2);
	}
#endif	/* DEPTH_LIMIT */
	/* now call C-Code */
	{
	  CPredicate f = PREG->u.pp.p->cs.f_code;
	  yamop *oldPREG = PREG;
	  saveregs();
	  d0 = (f)();
	  setregs();
#ifdef SHADOW_S
	  SREG = Yap_REGS.S_;
#endif
	  if (!d0) {
	    FAIL();
	  }
	  if (oldPREG == PREG) {
	    /* we did not update PREG */
	    /* we can proceed */
	    PREG = CPREG;
	    ENV_YREG = ENV;
#ifdef DEPTH_LIMIT
	    DEPTH = ENV_YREG[E_DEPTH];
#endif
	    WRITEBACK_Y_AS_ENV();
	  } else {
	    /* call the new code  */
	    CACHE_A1();
	  }
	}
	JMPNext();
	ENDCACHE_Y_AS_ENV();
	ENDD(d0);
      }
      ENDBOp();

      /* Like previous, the only difference is that we do not */
      /* trust the C-function we are calling and hence we must */
      /* guarantee that *all* machine registers are saved and */
      /* restored */
      BOp(call_usercpred, Osbpp);
      CACHE_Y_AS_ENV(YREG);
      check_stack(NoStackCall, H);
      ENDCACHE_Y_AS_ENV();
#ifdef LOW_LEVEL_TRACER
	if (Yap_do_low_level_trace) {
	  low_level_trace(enter_pred,PREG->u.Osbpp.p0,XREGS+1);
	}
#endif	/* LOW_LEVEL_TRACE */
#ifdef FROZEN_STACKS
      { 
	choiceptr top_b = PROTECT_FROZEN_B(B);
#ifdef SBA
	if (YREG > (CELL *) top_b || YREG < H) ASP = (CELL *) top_b;
#else
	if (YREG > (CELL *) top_b) ASP = (CELL *) top_b;
#endif /* SBA */
	else ASP = (CELL *)(((char *)YREG) +  PREG->u.Osbpp.s);
      }
#else
      if (YREG > (CELL *) B)
	ASP = (CELL *) B;
      else {
	ASP = (CELL *) (((char *) YREG) + PREG->u.Osbpp.s);
      }
      /* for slots to work */
#endif /* FROZEN_STACKS */
      Yap_StartSlots();
      Yap_PrologMode = UserCCallMode;
      {
	PredEntry *p = PREG->u.Osbpp.p;
#ifdef LOW_LEVEL_TRACER
	if (Yap_do_low_level_trace)
	  low_level_trace(enter_pred,p,XREGS+1);
#endif	/* LOW_LEVEL_TRACE */
	PREG = NEXTOP(PREG, Osbpp);
	saveregs();
	save_machine_regs();

	SREG = (CELL *) YAP_Execute(p, p->cs.f_code);
	EX = 0L;
      }

      restore_machine_regs();
      setregs();
      Yap_PrologMode = UserMode;
      if (!SREG) {
	FAIL();
      }
      /* in case we call Execute */
      YREG = ENV;
      JMPNext();
      ENDBOp();

      BOp(call_c_wfail, slp);
#ifdef LOW_LEVEL_TRACER
	if (Yap_do_low_level_trace) {
	  low_level_trace(enter_pred,PREG->u.slp.p,XREGS+1);
	}
#endif	/* LOW_LEVEL_TRACE */
#ifdef FROZEN_STACKS
      { 
	choiceptr top_b = PROTECT_FROZEN_B(B);
#ifdef SBA
	if (YREG > (CELL *) top_b || YREG < H) ASP = (CELL *) top_b;
#else
	if (YREG > (CELL *) top_b) ASP = (CELL *) top_b;
#endif /* SBA */
	else {
	  BEGD(d0);
	  d0 = PREG->u.slp.s;
	  ASP = ((CELL *)YREG) + d0;
	  ENDD(d0);
	}
      }
#else
      if (YREG > (CELL *) B)
	ASP = (CELL *) B;
      else {
	BEGD(d0);
	d0 = PREG->u.slp.s;
	ASP = ((CELL *) YREG) + d0;
	ENDD(d0);
      }
#endif /* FROZEN_STACKS */
      {
	CPredicate f = PREG->u.slp.p->cs.f_code;
	saveregs();
	SREG = (CELL *)((f)());
	setregs();
      }
      if (!SREG) {
	/* be careful about error handling */
	if (PREG != FAILCODE)
	  PREG = PREG->u.slp.l;
      } else {
	PREG = NEXTOP(PREG, slp);
      }
      CACHE_A1();
      JMPNext();
      ENDBOp();

      BOp(try_c, OtapFs);
#ifdef YAPOR
      CUT_wait_leftmost();
#endif /* YAPOR */
      CACHE_Y(YREG);
#ifdef CUT_C
      /* Alocate space for the cut_c structure*/
      CUT_C_PUSH(NEXTOP(NEXTOP(PREG,OtapFs),OtapFs),S_YREG);
#endif  
      S_YREG = S_YREG - PREG->u.OtapFs.extra;
      store_args(PREG->u.OtapFs.s);
      store_yaam_regs(NEXTOP(PREG, OtapFs), 0);
      B = B_YREG;
#ifdef YAPOR
      SCH_set_load(B_YREG);
#endif	/* YAPOR */
      SET_BB(B_YREG);
      ENDCACHE_Y();

    TRYCC:
      ASP = (CELL *)B;
      {
	CPredicate f = (CPredicate)(PREG->u.OtapFs.f);
	saveregs();
	SREG = (CELL *) ((f) ());
      	/* This last instruction changes B B*/
#ifdef CUT_C
	while (POP_CHOICE_POINT(B)){ 
	  cut_c_pop();
	}
#endif 
	setregs();
      }
      if (!SREG) {
#ifdef CUT_C
	/* Removes the cut functions from the stack
	 without executing them because we have fail 
	 and not cuted the predicate*/
	while(POP_CHOICE_POINT(B))
	  cut_c_pop();
#endif 
	FAIL();
      }
      if ((CELL *) B == YREG && ASP != (CELL *) B) {
	/* as Luis says, the predicate that did the try C might
	 * have left some data on the stack. We should preserve
	 * it, unless the builtin also did cut */
	YREG = ASP;
	HBREG = PROTECT_FROZEN_H(B);
	SET_BB(B);
      }
      PREG = CPREG;
      YREG = ENV;
      JMPNext();
      ENDBOp();

      BOp(retry_c, OtapFs);
#ifdef YAPOR
      CUT_wait_leftmost();
#endif /* YAPOR */
      CACHE_Y(B);
      CPREG = B_YREG->cp_cp;
      ENV = B_YREG->cp_env;
      H = PROTECT_FROZEN_H(B);
#ifdef DEPTH_LIMIT
      DEPTH =B->cp_depth;
#endif
      HBREG = H;
      restore_args(PREG->u.OtapFs.s);
      ENDCACHE_Y();
      goto TRYCC;
      ENDBOp();

#ifdef CUT_C
      BOp(cut_c, OtapFs);
      /*This is a phantom instruction. This is not executed by the WAM*/
#ifdef DEBUG
      /*If WAM executes this instruction, probably there's an error
       when we put this instruction, cut_c, after retry_c*/
      printf ("ERROR: Should not print this message FILE: absmi.c %d\n",__LINE__);
#endif /*DEBUG*/
      ENDBOp();
#endif

      BOp(try_userc, OtapFs);
#ifdef YAPOR
      CUT_wait_leftmost();
#endif /* YAPOR */
      CACHE_Y(YREG);
#ifdef CUT_C
      /* Alocate space for the cut_c structure*/
      CUT_C_PUSH(NEXTOP(NEXTOP(PREG,OtapFs),OtapFs),S_YREG);
#endif
      S_YREG = S_YREG - PREG->u.OtapFs.extra;
      store_args(PREG->u.OtapFs.s);
      store_yaam_regs(NEXTOP(PREG, OtapFs), 0);
      B = B_YREG;
#ifdef YAPOR
      SCH_set_load(B_YREG);
#endif
      SET_BB(B_YREG);
      ENDCACHE_Y();
      Yap_PrologMode = UserCCallMode;
      ASP = YREG;
      /* for slots to work */
      Yap_StartSlots();
      saveregs();
      save_machine_regs();
      SREG = (CELL *) YAP_ExecuteFirst(PREG->u.OtapFs.p, (CPredicate)(PREG->u.OtapFs.f));
      EX = 0L;
      restore_machine_regs();
      setregs();
      Yap_PrologMode = UserMode;
      if (!SREG) {
	FAIL();
      }
      if ((CELL *) B == YREG && ASP != (CELL *) B) {
	/* as Luis says, the predicate that did the try C might
	 * have left some data on the stack. We should preserve
	 * it, unless the builtin also did cut */
	YREG = ASP;
	HBREG = PROTECT_FROZEN_H(B);
      }
      PREG = CPREG;
      YREG = ENV;
      CACHE_A1();
      JMPNext();
      ENDBOp();

      BOp(retry_userc, OtapFs);
#ifdef YAPOR
      CUT_wait_leftmost();
#endif /* YAPOR */
      CACHE_Y(B);
      CPREG = B_YREG->cp_cp;
      ENV = B_YREG->cp_env;
      H = PROTECT_FROZEN_H(B);
#ifdef DEPTH_LIMIT
      DEPTH =B->cp_depth;
#endif
      HBREG = H;
      restore_args(PREG->u.OtapFs.s);
      ENDCACHE_Y();

      Yap_PrologMode = UserCCallMode;
      ASP = YREG;
      /* for slots to work */
      Yap_StartSlots();
      saveregs();
      save_machine_regs();
      SREG = (CELL *) YAP_ExecuteNext(PREG->u.OtapFs.p, (CPredicate)(PREG->u.OtapFs.f));
      EX = 0L;
      restore_machine_regs();
      setregs();
      Yap_PrologMode = UserMode;
      if (!SREG) {
#ifdef CUT_C
	/* Removes the cut functions from the stack
	 without executing them because we have fail 
	 and not cuted the predicate*/
	while(POP_CHOICE_POINT(B))
	  cut_c_pop();
#endif 
	FAIL();
      }
      if ((CELL *) B == YREG && ASP != (CELL *) B) {
	/* as Luis says, the predicate that did the try C might
	 * have left some data on the stack. We should preserve
	 * it, unless the builtin also did cut */
	YREG = ASP;
	HBREG = PROTECT_FROZEN_H(B);
      }
      PREG = CPREG;
      YREG = ENV;
      CACHE_A1();
      JMPNext();
      ENDBOp();

#ifdef CUT_C
      BOp(cut_userc, OtapFs);
      /*This is a phantom instruction. This is not executed by the WAM*/
#ifdef DEBUG
      /*If WAM executes this instruction, probably there's an error
       when we put this instruction, cut_userc, after retry_userc*/
      printf ("ERROR: Should not print this message FILE: absmi.c %d\n",__LINE__);
#endif /*DEBUG*/
      ENDBOp();
#endif


/************************************************************************\
*	support instructions             				 *
\************************************************************************/

      BOp(lock_pred, e);
#if defined(YAPOR) || defined(THREADS)
      {
	PredEntry *ap = PredFromDefCode(PREG);
 	LOCK(ap->PELock);
	PP = ap;
	if (!ap->cs.p_code.NOfClauses) {
	  FAIL();
	}
	/*
	  we do not lock access to the predicate,
	  we must take extra care here
	*/
	if (ap->cs.p_code.NOfClauses > 1 &&
	    !(ap->PredFlags & IndexedPredFlag)) {
	  /* update ASP before calling IPred */
	  ASP = YREG+E_CB;
	  if (ASP > (CELL *) PROTECT_FROZEN_B(B)) {
	    ASP = (CELL *) PROTECT_FROZEN_B(B);
	  }
	  saveregs();
	  Yap_IPred(ap, 0, CP);
	  /* IPred can generate errors, it thus must get rid of the lock itself */
	  setregs();
	  CACHE_A1();
	  /* for profiler */
	  save_pc();
	}
	PREG = ap->cs.p_code.TrueCodeOfPred;
      }
#endif
      JMPNext();
      ENDBOp();

      BOp(index_pred, e);
      {
	PredEntry *ap = PredFromDefCode(PREG);
#if defined(YAPOR) || defined(THREADS)
      /*
	we do not lock access to the predicate,
	we must take extra care here
      */
	if (!PP) {
	  LOCK(ap->PELock);
	}
	if (ap->OpcodeOfPred != INDEX_OPCODE) {
	  /* someone was here before we were */
	  PREG = ap->CodeOfPred;
	  /* for profiler */
	  save_pc();
	  JMPNext();
	}
#endif
      /* update ASP before calling IPred */
	ASP = YREG+E_CB;
	if (ASP > (CELL *) PROTECT_FROZEN_B(B)) {
	  ASP = (CELL *) PROTECT_FROZEN_B(B);
	}
	saveregs();
	Yap_IPred(ap, 0, CP);
      /* IPred can generate errors, it thus must get rid of the lock itself */
	setregs();
	CACHE_A1();
	PREG = ap->CodeOfPred;
	/* for profiler */
	save_pc();
#if defined(YAPOR) || defined(THREADS)
	if (!PP)
#endif
	  UNLOCK(ap->PELock);

      }
      JMPNext();
      ENDBOp();

#if THREADS
      BOp(thread_local, e);
      {
	PredEntry *ap = PredFromDefCode(PREG);
	ap = Yap_GetThreadPred(ap);
	PREG = ap->CodeOfPred;
	/* for profiler */
	save_pc();
      }
      JMPNext();
      ENDBOp();
#endif

      BOp(expand_index, e);
      {
	PredEntry *pe = PredFromExpandCode(PREG);
	yamop *pt0;

	/* update ASP before calling IPred */
	ASP = YREG+E_CB;
	if (ASP > (CELL *) PROTECT_FROZEN_B(B)) {
	  ASP = (CELL *) PROTECT_FROZEN_B(B);
	}
#if defined(YAPOR) || defined(THREADS)
	if (!PP) {
	  LOCK(pe->PELock);
	}
	if (!same_lu_block(PREG_ADDR, PREG)) {
	  PREG = *PREG_ADDR;
	  if (!PP)
	    UNLOCK(pe->PELock);
	  JMPNext();
	}
#endif
#ifdef SHADOW_S
	S = SREG;
#endif /* SHADOW_S */
 	saveregs();
	pt0 = Yap_ExpandIndex(pe, 0);
	/* restart index */
	setregs();
#ifdef SHADOW_S
	SREG = S;
#endif /* SHADOW_S */
 	PREG = pt0;
#if defined(YAPOR) || defined(THREADS)
	if (!PP)
#endif
	  UNLOCK(pe->PELock);
	JMPNext();
      }
      ENDBOp();

      BOp(expand_clauses, sssllp);
      {
	PredEntry *pe = PREG->u.sssllp.p;
	yamop *pt0;

	/* update ASP before calling IPred */
	ASP = YREG+E_CB;
	if (ASP > (CELL *) PROTECT_FROZEN_B(B)) {
	  ASP = (CELL *) PROTECT_FROZEN_B(B);
	}
#if defined(YAPOR) || defined(THREADS)
	if (PP == NULL) {
	  LOCK(pe->PELock);
	}
	if (!same_lu_block(PREG_ADDR, PREG)) {
	  PREG = *PREG_ADDR;
	  if (!PP) {
	    UNLOCK(pe->PELock);	    
	  }
	  JMPNext();
	}
#endif
 	saveregs();
	pt0 = Yap_ExpandIndex(pe, 0);
	/* restart index */
	setregs();
	UNLOCK(pe->PELock);
 	PREG = pt0;
#if defined(YAPOR) || defined(THREADS)
	if (!PP) {
	  UNLOCK(pe->PELock);
	}
#endif
	JMPNext();
      }
      ENDBOp();

      BOp(undef_p, e);
      /* save S for module name */
      { 
	PredEntry *pe = PredFromDefCode(PREG);
	BEGD(d0);
	/* avoid trouble with undefined dynamic procedures */
	if ((pe->PredFlags & (DynamicPredFlag|LogUpdatePredFlag|MultiFileFlag)) ||
	    (UndefCode->OpcodeOfPred == UNDEF_OPCODE)) {
#if defined(YAPOR) || defined(THREADS)
	  PP = NULL;
#endif
	  UNLOCK(pe->PELock);
	  FAIL();
	}
	d0 = pe->ArityOfPE;
	UNLOCK(pe->PELock);
	if (d0 == 0) {
	  H[1] = MkAtomTerm((Atom)(pe->FunctorOfPred));
	}
	else {
	  H[d0 + 2] = AbsAppl(H);
	  *H = (CELL) pe->FunctorOfPred;
	  H++;
	  BEGP(pt1);
	  pt1 = XREGS + 1;
	  for (; d0 > 0; --d0) {
	    BEGD(d1);
	    BEGP(pt0);
	    pt0 = pt1++;
	    d1 = *pt0;
	    deref_head(d1, undef_unk);
	  undef_nonvar:
	    /* just copy it to the heap */
	    *H++ = d1;
	    continue;

	    derefa_body(d1, pt0, undef_unk, undef_nonvar);
	    if (pt0 <= H) {
	      /* variable is safe */
	      *H++ = (CELL)pt0;
	    } else {
	      /* bind it, in case it is a local variable */
	      d1 = Unsigned(H);
	      RESET_VARIABLE(H);
	      H += 1;
	      Bind_Local(pt0, d1);
	    }
	    ENDP(pt0);
	    ENDD(d1);
	  }
	  ENDP(pt1);
	}
	ENDD(d0);
	H[0] = Yap_Module_Name(pe);
	ARG1 = (Term) AbsPair(H);
	H += 2;
#ifdef LOW_LEVEL_TRACER
      if (Yap_do_low_level_trace)
	low_level_trace(enter_pred,UndefCode,XREGS+1);
#endif	/* LOW_LEVEL_TRACE */
      }

      PREG = UndefCode->CodeOfPred;
      /* for profiler */
      save_pc();
      CACHE_A1();
      JMPNext();
      ENDBOp();

      BOp(spy_pred, e);
    dospy:
      {
	PredEntry *pe = PredFromDefCode(PREG);
	BEGD(d0);
 	LOCK(pe->PELock);
	if (!(pe->PredFlags & IndexedPredFlag) &&
	      pe->cs.p_code.NOfClauses > 1) {
	  /* update ASP before calling IPred */
	  ASP = YREG+E_CB;
	  if (ASP > (CELL *) PROTECT_FROZEN_B(B)) {
	    ASP = (CELL *) PROTECT_FROZEN_B(B);
	  }
	  saveregs();
	  Yap_IPred(pe, 0, CP);
	  /* IPred can generate errors, it thus must get rid of the lock itself */
	  setregs();
	}
	/* first check if we need to increase the counter */
	if ((pe->PredFlags & CountPredFlag)) {
	  LOCK(pe->StatisticsForPred.lock);
	  pe->StatisticsForPred.NOfEntries++;
	  UNLOCK(pe->StatisticsForPred.lock);
	  ReductionsCounter--;
	  if (ReductionsCounter == 0 && ReductionsCounterOn) {
	    UNLOCK(pe->PELock);
	    saveregs();
	    Yap_Error(CALL_COUNTER_UNDERFLOW,TermNil,"");
	    setregs();
	    JMPNext();
	  } 
	  PredEntriesCounter--;
	  if (PredEntriesCounter == 0 && PredEntriesCounterOn) {
	    UNLOCK(pe->PELock);
	    saveregs();
	    Yap_Error(PRED_ENTRY_COUNTER_UNDERFLOW,TermNil,"");
	    setregs();
	    JMPNext();
	  } 
	  if ((pe->PredFlags & (CountPredFlag|ProfiledPredFlag|SpiedPredFlag)) == 
	    CountPredFlag) {
	    PREG = pe->cs.p_code.TrueCodeOfPred;
	    UNLOCK(pe->PELock);
	    JMPNext();
	  }
	}
	/* standard profiler */
	if ((pe->PredFlags & ProfiledPredFlag)) {
	  LOCK(pe->StatisticsForPred.lock);
	  pe->StatisticsForPred.NOfEntries++;
	  UNLOCK(pe->StatisticsForPred.lock);
	  if (!(pe->PredFlags & SpiedPredFlag)) {
	    PREG = pe->cs.p_code.TrueCodeOfPred;
	    UNLOCK(pe->PELock);
	    JMPNext();
	  }
	}
	if (!DebugOn) {
	  PREG = pe->cs.p_code.TrueCodeOfPred;
	  UNLOCK(pe->PELock);
	  JMPNext();
	}
	UNLOCK(pe->PELock);
	
	d0 = pe->ArityOfPE;
	/* save S for ModuleName */
	if (d0 == 0) {
	  H[1] = MkAtomTerm((Atom)(pe->FunctorOfPred));
	} else {
	  *H = (CELL) pe->FunctorOfPred;
	  H[d0 + 2] = AbsAppl(H);
	  H++;
	  BEGP(pt1);
	  pt1 = XREGS + 1;
	  for (; d0 > 0; --d0) {
	    BEGD(d1);
	    BEGP(pt0);
	    pt0 = pt1++;
	    d1 = *pt0;
	    deref_head(d1, dospy_unk);
	  dospy_nonvar:
	    /* just copy it to the heap */
	    *H++ = d1;
	    continue;
	    
	    derefa_body(d1, pt0, dospy_unk, dospy_nonvar);
	    if (pt0 <= H) {
	      /* variable is safe */
	      *H++ = (CELL)pt0;
	    } else {
	      /* bind it, in case it is a local variable */
	      d1 = Unsigned(H);
	      RESET_VARIABLE(H);
	      H += 1;
	      Bind_Local(pt0, d1);
	    }
	    ENDP(pt0);
	    ENDD(d1);
	  }
	  ENDP(pt1);
	}
	ENDD(d0);
	H[0] = Yap_Module_Name(pe);
      }
      ARG1 = (Term) AbsPair(H);
      H += 2;
      {
	PredEntry *pt0;
#ifdef THREADS
	LOCK(ThreadHandlesLock);
#endif
	pt0 = SpyCode;
	P_before_spy = PREG;
	PREG = pt0->CodeOfPred;
	/* for profiler */
#ifdef THREADS
	UNLOCK(ThreadHandlesLock);
#endif
	save_pc();
	CACHE_A1();
#ifdef LOW_LEVEL_TRACER
	if (Yap_do_low_level_trace)
	  low_level_trace(enter_pred,pt0,XREGS+1);
#endif	/* LOW_LEVEL_TRACE */
      }
      JMPNext();
      ENDBOp();


/************************************************************************\
* 	Try / Retry / Trust for main indexing blocks			*
\************************************************************************/

      BOp(try_clause, Otapl);
      check_trail(TR);
      CACHE_Y(YREG);
      /* Point AP to the code that follows this instruction */
      store_at_least_one_arg(PREG->u.Otapl.s);
      store_yaam_regs(NEXTOP(PREG, Otapl), 0);
      PREG = PREG->u.Otapl.d;
      set_cut(S_YREG, B);
      B = B_YREG;
#ifdef YAPOR
      SCH_set_load(B_YREG);
#endif	/* YAPOR */
      SET_BB(B_YREG);
      ENDCACHE_Y();
      JMPNext();
      ENDBOp();

      BOp(try_clause2, l);
      check_trail(TR);
      CACHE_Y(YREG);
      /* Point AP to the code that follows this instruction */
      {
	register CELL x2 = ARG2;
	register CELL x1 = ARG1;

	store_yaam_regs(NEXTOP(PREG, l), 2);
	B_YREG->cp_a1 = x1;
	B_YREG->cp_a2 = x2;
      }
      PREG = PREG->u.l.l;
      set_cut(S_YREG, B);
      B = B_YREG;
#ifdef YAPOR
      SCH_set_load(B_YREG);
#endif	/* YAPOR */
      SET_BB(B_YREG);
      ENDCACHE_Y();
      JMPNext();
      ENDBOp();

      BOp(try_clause3, l);
      check_trail(TR);
      CACHE_Y(YREG);
      /* Point AP to the code that follows this instruction */
      {
	store_yaam_regs(NEXTOP(PREG, l), 3);
	B_YREG->cp_a1 = ARG1;
	B_YREG->cp_a2 = ARG2;
	B_YREG->cp_a3 = ARG3;
      }
      PREG = PREG->u.l.l;
      set_cut(S_YREG, B);
      B = B_YREG;
#ifdef YAPOR
      SCH_set_load(B_YREG);
#endif	/* YAPOR */
      SET_BB(B_YREG);
      ENDCACHE_Y();
      JMPNext();
      ENDBOp();

      BOp(try_clause4, l);
      check_trail(TR);
      CACHE_Y(YREG);
      /* Point AP to the code that follows this instruction */
      {
	store_yaam_regs(NEXTOP(PREG, l), 4);
	B_YREG->cp_a1 = ARG1;
	B_YREG->cp_a2 = ARG2;
	B_YREG->cp_a3 = ARG3;
	B_YREG->cp_a4 = ARG4;
      }
      PREG = PREG->u.l.l;
      set_cut(S_YREG, B);
      B = B_YREG;
#ifdef YAPOR
      SCH_set_load(B_YREG);
#endif	/* YAPOR */
      SET_BB(B_YREG);
      ENDCACHE_Y();
      JMPNext();
      ENDBOp();

      BOp(retry, Otapl);
      CACHE_Y(B);
      restore_yaam_regs(NEXTOP(PREG, Otapl));
      restore_at_least_one_arg(PREG->u.Otapl.s);
#ifdef FROZEN_STACKS
      S_YREG = (CELL *) PROTECT_FROZEN_B(B_YREG);
      set_cut(S_YREG, B->cp_b);
#else
      set_cut(S_YREG, B_YREG->cp_b);
#endif /* FROZEN_STACKS */
      SET_BB(B_YREG);
      ENDCACHE_Y();
      PREG = PREG->u.Otapl.d;
      JMPNext();
      ENDBOp();

      BOp(retry2, l);
      CACHE_Y(B);
      restore_yaam_regs(NEXTOP(PREG, l));
      PREG = PREG->u.l.l;
      ARG1 = B_YREG->cp_a1;
      ARG2 = B_YREG->cp_a2;
#ifdef FROZEN_STACKS
      S_YREG = (CELL *) PROTECT_FROZEN_B(B_YREG);
      set_cut(S_YREG, B->cp_b);
#else
      set_cut(S_YREG, B_YREG->cp_b);
#endif /* FROZEN_STACKS */
      SET_BB(B_YREG);
      ENDCACHE_Y();
      JMPNext();
      ENDBOp();

      BOp(retry3, l);
      CACHE_Y(B);
      restore_yaam_regs(NEXTOP(PREG, l));
      PREG = PREG->u.l.l;
      ARG1 = B_YREG->cp_a1;
      ARG2 = B_YREG->cp_a2;
      ARG3 = B_YREG->cp_a3;
#ifdef FROZEN_STACKS
      S_YREG = (CELL *) PROTECT_FROZEN_B(B_YREG);
      set_cut(S_YREG, B->cp_b);
#else
      set_cut(S_YREG, B_YREG->cp_b);
#endif /* FROZEN_STACKS */
      SET_BB(B_YREG);
      ENDCACHE_Y();
      JMPNext();
      ENDBOp();

      BOp(retry4, l);
      CACHE_Y(B);
      restore_yaam_regs(NEXTOP(PREG, l));
      PREG = PREG->u.l.l;
      ARG1 = B_YREG->cp_a1;
      ARG2 = B_YREG->cp_a2;
      ARG3 = B_YREG->cp_a3;
      ARG4 = B_YREG->cp_a4;
#ifdef FROZEN_STACKS
      S_YREG = (CELL *) PROTECT_FROZEN_B(B_YREG);
      set_cut(S_YREG, B->cp_b);
#else
      set_cut(S_YREG, B_YREG->cp_b);
#endif /* FROZEN_STACKS */
      SET_BB(B_YREG);
      ENDCACHE_Y();
      JMPNext();
      ENDBOp();

      BOp(trust, Otapl);
      CACHE_Y(B);
#ifdef YAPOR
      if (SCH_top_shared_cp(B)) {
	SCH_last_alternative(PREG, B_YREG);
	restore_at_least_one_arg(PREG->u.Otapl.s);
#ifdef FROZEN_STACKS
        S_YREG = (CELL *) PROTECT_FROZEN_B(B_YREG);
#endif /* FROZEN_STACKS */
	set_cut(S_YREG, B->cp_b);
      }
      else
#endif	/* YAPOR */
      {
	pop_yaam_regs();
	pop_at_least_one_arg(PREG->u.Otapl.s);
#ifdef FROZEN_STACKS
        S_YREG = (CELL *) PROTECT_FROZEN_B(B_YREG);
#endif /* FROZEN_STACKS */
	set_cut(S_YREG, B);
      }
      SET_BB(B_YREG);
      ENDCACHE_Y();
      PREG = PREG->u.Otapl.d;
      JMPNext();
      ENDBOp();

      BOp(try_in, l);
      B->cp_ap = NEXTOP(PREG, l);
      PREG = PREG->u.l.l;
      JMPNext();
      ENDBOp();



/************************************************************************\
* 	Logical Updates							*
\************************************************************************/

      /* enter logical pred               */
      BOp(enter_lu_pred, Ills);
      check_trail(TR);
      /* mark the indexing code */
      {
	LogUpdIndex *cl = PREG->u.Ills.I;
	PredEntry *ap = cl->ClPred;

	if (ap->LastCallOfPred != LUCALL_EXEC) {
	  /*
	    only increment time stamp if we are working on current time
	    stamp
	  */
	  if (ap->TimeStampOfPred >= TIMESTAMP_RESET)
	    Yap_UpdateTimestamps(ap);
	  ap->TimeStampOfPred++;
	  ap->LastCallOfPred = LUCALL_EXEC;
	  /*	  fprintf(stderr,"R %x--%d--%ul\n",ap,ap->TimeStampOfPred,ap->ArityOfPE);*/
	}
	*--YREG = MkIntegerTerm(ap->TimeStampOfPred);
	/* fprintf(stderr,"> %p/%p %d %d\n",cl,ap,ap->TimeStampOfPred,PREG->u.Ills.s);*/
	PREG = PREG->u.Ills.l1;
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
      GONext();
      ENDBOp();

      BOp(try_logical, OtaLl);
      check_trail(TR);
      {
	UInt timestamp;
	
	CACHE_Y(YREG);
	timestamp = IntegerOfTerm(S_YREG[0]);
	/* fprintf(stderr,"+ %p/%p %d %d %d--%u\n",PREG,PREG->u.OtaLl.d->ClPred,timestamp,PREG->u.OtaLl.d->ClPred->TimeStampOfPred,PREG->u.OtaLl.d->ClTimeStart,PREG->u.OtaLl.d->ClTimeEnd);*/
	/* Point AP to the code that follows this instruction */
	/* always do this, even if we are not going to use it */
	store_args(PREG->u.OtaLl.s);
	store_yaam_regs(PREG->u.OtaLl.n, 0);
	set_cut(S_YREG, B);
	B = B_YREG;
#ifdef YAPOR
	SCH_set_load(B_YREG);
#endif	/* YAPOR */
#ifdef YAPOR
	PP = PREG->u.OtaLl.d->ClPred;
#endif	/* YAPOR */
	if (!VALID_TIMESTAMP(timestamp, PREG->u.OtaLl.d)) {
	  /* jump to next alternative */
	  PREG=PREG->u.OtaLl.n;
	} else {
	  PREG = PREG->u.OtaLl.d->ClCode;
	}
	SET_BB(B_YREG);
	ENDCACHE_Y();
      }
      JMPNext();
      ENDBOp();

      BOp(retry_logical, OtaLl);
      check_trail(TR);
      {
	UInt timestamp;
	CACHE_Y(B);
      
#if defined(YAPOR) || defined(THREADS)
	if (!PP) {
	  PP = PREG->u.OtaLl.d->ClPred;
	  LOCK(PP->PELock);
	}
#endif
	timestamp = IntegerOfTerm(((CELL *)(B_YREG+1))[PREG->u.OtaLl.s]);
	/* fprintf(stderr,"^ %p/%p %d %d %d--%u\n",PREG,PREG->u.OtaLl.d->ClPred,timestamp,PREG->u.OtaLl.d->ClPred->TimeStampOfPred,PREG->u.OtaLl.d->ClTimeStart,PREG->u.OtaLl.d->ClTimeEnd);*/
	if (!VALID_TIMESTAMP(timestamp, PREG->u.OtaLl.d)) {
	  /* jump to next instruction */
	  PREG=PREG->u.OtaLl.n;
	  JMPNext();
	}
	restore_yaam_regs(PREG->u.OtaLl.n);
	restore_at_least_one_arg(PREG->u.OtaLl.s);
#ifdef THREADS
	PP = PREG->u.OtaLl.d->ClPred;
#endif
	PREG = PREG->u.OtaLl.d->ClCode;
#ifdef FROZEN_STACKS
	S_YREG = (CELL *) PROTECT_FROZEN_B(B_YREG);
	set_cut(S_YREG, B->cp_b);
#else
	set_cut(S_YREG, B_YREG->cp_b);
#endif /* FROZEN_STACKS */
	SET_BB(B_YREG);
	ENDCACHE_Y();
      }
      JMPNext();
      ENDBOp();

      BOp(trust_logical, OtILl);
      CACHE_Y(B);
      {
	LogUpdIndex *cl = PREG->u.OtILl.block;
	PredEntry *ap = cl->ClPred;
	LogUpdClause *lcl = PREG->u.OtILl.d;
	UInt timestamp = IntegerOfTerm(((CELL *)(B_YREG+1))[ap->ArityOfPE]);

	/* fprintf(stderr,"- %p/%p %d %d %p\n",PREG,ap,timestamp,ap->TimeStampOfPred,PREG->u.OtILl.d->ClCode);*/
#if defined(YAPOR) || defined(THREADS)
	if (!PP) {
	  LOCK(ap->PELock);
	  PP = ap;
	}
#endif
	if (!VALID_TIMESTAMP(timestamp, lcl)) {
	  /* jump to next alternative */
	  PREG = FAILCODE;
	} else {
	  PREG = lcl->ClCode;
	}
	/* HEY, leave indexing block alone!! */
	/* check if we are the ones using this code */
#if defined(YAPOR) || defined(THREADS)
	DEC_CLREF_COUNT(cl);
	/* clear the entry from the trail */
	B->cp_tr--;
	TR = B->cp_tr;
	/* actually get rid of the code */
	if (cl->ClRefCount == 0 && (cl->ClFlags & (ErasedMask|DirtyMask))) {
	  if (PREG != FAILCODE) {
	    if (lcl->ClRefCount == 1) {
	      /* make sure the clause isn't destroyed */
	      /* always add an extra reference */
	      INC_CLREF_COUNT(lcl);
	      TRAIL_CLREF(lcl);
	      B->cp_tr = TR;
	    }
	  }
	  if (cl->ClFlags & ErasedMask) {
	    saveregs();
	    Yap_ErLogUpdIndex(cl);
	    setregs();
	  } else {
	    saveregs();
	    Yap_CleanUpIndex(cl);
	    setregs();
	  }
	  save_pc();
	}
#else
	if (TrailTerm(B->cp_tr-1) == CLREF_TO_TRENTRY(cl) &&
	    B->cp_tr != B->cp_b->cp_tr) {
	  cl->ClFlags &= ~InUseMask;
	  B->cp_tr--;
	  TR = B->cp_tr;
	  /* next, recover space for the indexing code if it was erased */
	  if (cl->ClFlags & (ErasedMask|DirtyMask)) {
	    if (PREG != FAILCODE) {
	      /* make sure we don't erase the clause we are jumping too */
	      if (lcl->ClRefCount == 1 && !(lcl->ClFlags & InUseMask)) {
		lcl->ClFlags |= InUseMask;
		TRAIL_CLREF(lcl);
		B->cp_tr = TR;
	      }
	    }
	    if (cl->ClFlags & ErasedMask) {
	      saveregs();
	      Yap_ErLogUpdIndex(cl);
	      setregs();
	    } else {
	      saveregs();
	      Yap_CleanUpIndex(cl);
	      setregs();
	    }
	  }
	}
#endif
#ifdef YAPOR
	if (SCH_top_shared_cp(B)) {
	  SCH_last_alternative(PREG, B_YREG);
	  restore_args(ap->ArityOfPE);
#ifdef FROZEN_STACKS
	  S_YREG = (CELL *) PROTECT_FROZEN_B(B_YREG);
#else
	  S_YREG++;
#endif /* FROZEN_STACKS */
	  set_cut(S_YREG, B->cp_b);
	} else
#endif	/* YAPOR */
	  {
	    pop_yaam_regs();
	    pop_args(ap->ArityOfPE);
	    S_YREG--;
#ifdef FROZEN_STACKS
	    S_YREG = (CELL *) PROTECT_FROZEN_B(B_YREG);
#endif /* FROZEN_STACKS */
	    set_cut(S_YREG, B);
	  }
	SET_BB(B_YREG);
	ENDCACHE_Y();
#if defined(YAPOR) || defined(THREADS)
	if (PREG == FAILCODE) {
	  UNLOCK(PP->PELock);
	  PP = NULL;
	}
#endif
	JMPNext();
      }
      ENDBOp();


/************************************************************************\
* 	Indexing in ARG1						*
\************************************************************************/

      BOp(user_switch, lp);
      {
	yamop *new = Yap_udi_search(PREG->u.lp.p);
	if (!new) {
	  PREG = PREG->u.lp.l;
	  JMPNext();
	}
	PREG = new;
	JMPNext();
      }
      ENDBOp();

      BOp(switch_on_type, llll);
      BEGD(d0);
      d0 = CACHED_A1();
      deref_head(d0, swt_unk);
      /* nonvar */
    swt_nvar:
      if (IsPairTerm(d0)) {
	/* pair */
	SREG = RepPair(d0);
	copy_jmp_address(PREG->u.llll.l1);
	PREG = PREG->u.llll.l1;
	JMPNext();
      }
      else if (!IsApplTerm(d0)) {
	/* constant */
	copy_jmp_address(PREG->u.llll.l2);
	PREG = PREG->u.llll.l2;
	I_R = d0;
	JMPNext();
      }
      else {
	/* appl */
	copy_jmp_address(PREG->u.llll.l3);
	PREG = PREG->u.llll.l3;
	SREG = RepAppl(d0);
	JMPNext();
      }

      BEGP(pt0);
      deref_body(d0, pt0, swt_unk, swt_nvar);
      /* variable */
      copy_jmp_address(PREG->u.llll.l4);
      PREG = PREG->u.llll.l4;
      JMPNext();
      ENDP(pt0);
      ENDD(d0);
      ENDBOp();

      /* specialised case where the arguments may be:
       * a list;
       * the empty list;
       * some other atom;
       * a variable;
       * 
       */
      BOp(switch_list_nl, ollll);
      ALWAYS_LOOKAHEAD(PREG->u.ollll.pop);
      BEGD(d0);
      d0 = CACHED_A1();
#if UNIQUE_TAG_FOR_PAIRS
      deref_list_head(d0, swlnl_unk_p);
      swlnl_list_p:
      {
#else
	deref_head(d0, swlnl_unk_p);
	/* non variable */
      swlnl_nvar_p:
	if (IsPairTerm(d0)) {
	  /* pair */
#endif
	  copy_jmp_address(PREG->u.ollll.l1);
	  PREG = PREG->u.ollll.l1;
	  SREG = RepPair(d0);
	  ALWAYS_GONext();
	}
#if UNIQUE_TAG_FOR_PAIRS
      swlnl_nlist_p:
#endif
	if (d0 == TermNil) {
	  /* empty list */
	  PREG = PREG->u.ollll.l2;
	  JMPNext();
	}
	else {
	  /* appl or constant */
	  if (IsApplTerm(d0)) {
	    copy_jmp_address(PREG->u.ollll.l3);
	    PREG = PREG->u.ollll.l3;
	    SREG = RepAppl(d0);
	    JMPNext();
	  } else {
	    copy_jmp_address(PREG->u.ollll.l3);
	    PREG = PREG->u.ollll.l3;
	    I_R = d0;
	    JMPNext();
	  }
	}

	BEGP(pt0);
#if UNIQUE_TAG_FOR_PAIRS
      swlnl_unk_p:
	deref_list_body(d0, pt0, swlnl_list_p, swlnl_nlist_p);
#else
	deref_body(d0, pt0, swlnl_unk_p, swlnl_nvar_p);
#endif
	ENDP(pt0);
	/* variable */
	copy_jmp_address(PREG->u.ollll.l4);
	PREG = PREG->u.ollll.l4;
	JMPNext();
	ENDD(d0);
      }
      ENDBOp();

      BOp(switch_on_arg_type, xllll);
      BEGD(d0);
      d0 = XREG(PREG->u.xllll.x);
      deref_head(d0, arg_swt_unk);
      /* nonvar */
    arg_swt_nvar:
      if (IsPairTerm(d0)) {
	/* pair */
	copy_jmp_address(PREG->u.xllll.l1);
	PREG = PREG->u.xllll.l1;
	SREG = RepPair(d0);
	JMPNext();
      }
      else if (!IsApplTerm(d0)) {
	/* constant */
	copy_jmp_address(PREG->u.xllll.l2);
	PREG = PREG->u.xllll.l2;
	I_R = d0;
	JMPNext();
      }
      else {
	/* appl */
	copy_jmp_address(PREG->u.xllll.l3);
	PREG = PREG->u.xllll.l3;
	SREG = RepAppl(d0);
	JMPNext();
      }

      BEGP(pt0);
      deref_body(d0, pt0, arg_swt_unk, arg_swt_nvar);
      /* variable */
      copy_jmp_address(PREG->u.xllll.l4);
      PREG = PREG->u.xllll.l4;
      JMPNext();
      ENDP(pt0);
      ENDD(d0);
      ENDBOp();

      BOp(switch_on_sub_arg_type, sllll);
      BEGD(d0);
      d0 = SREG[PREG->u.sllll.s];
      deref_head(d0, sub_arg_swt_unk);
      /* nonvar */
    sub_arg_swt_nvar:
      if (IsPairTerm(d0)) {
	/* pair */
	copy_jmp_address(PREG->u.sllll.l1);
	PREG = PREG->u.sllll.l1;
	SREG = RepPair(d0);
	JMPNext();
      }
      else if (!IsApplTerm(d0)) {
	/* constant */
	copy_jmp_address(PREG->u.sllll.l2);
	PREG = PREG->u.sllll.l2;
	I_R = d0;
	JMPNext();
      }
      else {
	/* appl */
	copy_jmp_address(PREG->u.sllll.l3);
	PREG = PREG->u.sllll.l3;
	SREG = RepAppl(d0);
	JMPNext();
      }

      BEGP(pt0);
      deref_body(d0, pt0, sub_arg_swt_unk, sub_arg_swt_nvar);
      /* variable */
      copy_jmp_address(PREG->u.sllll.l4);
      PREG = PREG->u.sllll.l4;
      JMPNext();
      ENDP(pt0);
      ENDD(d0);
      ENDBOp();

      BOp(jump_if_var, l);
      BEGD(d0);
      d0 = CACHED_A1();
      deref_head(d0, jump_if_unk);
      /* non var */
    jump0_if_nonvar:
      PREG = NEXTOP(PREG, l);
      JMPNext();

      BEGP(pt0);
      deref_body(d0, pt0, jump_if_unk, jump0_if_nonvar);
      /* variable */
      copy_jmp_address(PREG->u.l.l);
      PREG = PREG->u.l.l;
      ENDP(pt0);
      JMPNext();
      ENDD(d0);
      ENDBOp();

      BOp(jump_if_nonvar, xll);
      BEGD(d0);
      d0 = XREG(PREG->u.xll.x);
      deref_head(d0, jump2_if_unk);
      /* non var */
    jump2_if_nonvar:
      copy_jmp_address(PREG->u.xll.l1);
      PREG = PREG->u.xll.l1;
      JMPNext();

      BEGP(pt0);
      deref_body(d0, pt0, jump2_if_unk, jump2_if_nonvar);
      /* variable */
      PREG = NEXTOP(PREG, xll);
      ENDP(pt0);
      JMPNext();
      ENDD(d0);
      ENDBOp();

      BOp(if_not_then, clll);
      BEGD(d0);
      d0 = CACHED_A1();
      deref_head(d0, if_n_unk);
    if_n_nvar:
      /* not variable */
      if (d0 == PREG->u.clll.c) {
	/* equal to test value */
	copy_jmp_address(PREG->u.clll.l2);
	PREG = PREG->u.clll.l2;
	JMPNext();
      }
      else {
	/* different from test value */
	/* the case to optimise */
	copy_jmp_address(PREG->u.clll.l1);
	PREG = PREG->u.clll.l1;
	JMPNext();
      }

      BEGP(pt0);
      deref_body(d0, pt0, if_n_unk, if_n_nvar);
      ENDP(pt0);
      /* variable */
      copy_jmp_address(PREG->u.clll.l3);
      PREG = PREG->u.clll.l3;
      JMPNext();
      ENDD(d0);
      ENDBOp();

/************************************************************************\
* 	Indexing on ARG1							*
\************************************************************************/

#define HASH_SHIFT 6

      BOp(switch_on_func, sssl);
      BEGD(d1);
      d1 = *SREG++;
      /* we use a very simple hash function to find elements in a
       * switch table */
      {
	CELL
	/* first, calculate the mask */
	  Mask = (PREG->u.sssl.s - 1) << 1,	/* next, calculate the hash function */
	  hash = d1 >> (HASH_SHIFT - 1) & Mask;
	CELL *base;

	base = (CELL *)PREG->u.sssl.l;
	/* PREG now points at the beginning of the hash table */
	BEGP(pt0);
	/* pt0 will always point at the item */
	pt0 = base + hash;
	BEGD(d0);
	d0 = pt0[0];
	/* a match happens either if we found the value, or if we
	 * found an empty slot */
	if (d0 == d1 || d0 == 0) {
	  copy_jmp_addressa(pt0+1);
	  PREG = (yamop *) (pt0[1]);
	  JMPNext();
	}
	else {
	  /* ooops, collision, look for other items   */
	  register CELL d = ((d1 | 1) << 1) & Mask;

	  while (1) {
	    hash = (hash + d) & Mask;
	    pt0 = base + hash;
	    d0 = pt0[0];
	    if (d0 == d1 || d0 == 0) {
	      copy_jmp_addressa(pt0+1);
	      PREG = (yamop *) pt0[1];
	      JMPNext();
	    }
	  }
	}
	ENDD(d0);
	ENDP(pt0);
      }
      ENDD(d1);
      ENDBOp();

      BOp(switch_on_cons, sssl);
      BEGD(d1);
      d1 = I_R;
      /* we use a very simple hash function to find elements in a
       * switch table */
      {
	CELL
	/* first, calculate the mask */
	  Mask = (PREG->u.sssl.s - 1) << 1,	/* next, calculate the hash function */
	  hash = d1 >> (HASH_SHIFT - 1) & Mask;
	CELL *base;

	base = (CELL *)PREG->u.sssl.l;
	/* PREG now points at the beginning of the hash table */
	BEGP(pt0);
	/* pt0 will always point at the item */
	pt0 = base + hash;
	BEGD(d0);
	d0 = pt0[0];
	/* a match happens either if we found the value, or if we
	 * found an empty slot */
	if (d0 == d1 || d0 == 0) {
	  copy_jmp_addressa(pt0+1);
	  PREG = (yamop *) (pt0[1]);
	  JMPNext();
	}
	else {
	  /* ooops, collision, look for other items   */
	  register CELL d = ((d1 | 1) << 1) & Mask;

	  while (1) {
	    hash = (hash + d) & Mask;
	    pt0 = base + hash;
	    d0 = pt0[0];
	    if (d0 == d1 || d0 == 0) {
	      copy_jmp_addressa(pt0+1);
	      PREG = (yamop *) pt0[1];
	      JMPNext();
	    }
	  }
	}
	ENDD(d0);
	ENDP(pt0);
      }
      ENDD(d1);
      ENDBOp();

      BOp(go_on_func, sssl);
      BEGD(d0);
      {
	CELL *pt = (CELL *)(PREG->u.sssl.l);

	d0 = *SREG++;
	if (d0 == pt[0]) {
	  copy_jmp_addressa(pt+1);
	  PREG = (yamop *) pt[1];
	  JMPNext();
	} else {
	  copy_jmp_addressa(pt+3);
	  PREG = (yamop *) pt[3];
	  JMPNext();
	}
      }
      ENDD(d0);
      ENDBOp();

      BOp(go_on_cons, sssl);
      BEGD(d0);
      {
	CELL *pt = (CELL *)(PREG->u.sssl.l);

	d0 = I_R;
	if (d0 == pt[0]) {
	  copy_jmp_addressa(pt+1);
	  PREG = (yamop *) pt[1];
	  JMPNext();
	} else {
	  copy_jmp_addressa(pt+3);
	  PREG = (yamop *) pt[3];
	  JMPNext();
	}
      }
      ENDD(d0);
      ENDBOp();

      BOp(if_func, sssl);
      BEGD(d1);
      BEGP(pt0);
      pt0 = (CELL *) PREG->u.sssl.l;
      d1 = *SREG++;
      while (pt0[0] != d1 && pt0[0] != (CELL)NULL ) {
	pt0 += 2;
      }
      copy_jmp_addressa(pt0+1);
      PREG = (yamop *) (pt0[1]);
      JMPNext();
      ENDP(pt0);
      ENDD(d1);
      ENDBOp();

      BOp(if_cons, sssl);
      BEGD(d1);
      BEGP(pt0);
      pt0 = (CELL *) PREG->u.sssl.l;
      d1 = I_R;
      while (pt0[0] != d1 && pt0[0] != 0L ) {
	pt0 += 2;
      }
      copy_jmp_addressa(pt0+1);
      PREG = (yamop *) (pt0[1]);
      JMPNext();
      ENDP(pt0);
      ENDD(d1);
      ENDBOp();

      Op(index_dbref, e);
      PREG = NEXTOP(PREG, e);
      I_R = AbsAppl(SREG-1);
      GONext();
      ENDOp();
      
      Op(index_blob, e);
      PREG = NEXTOP(PREG, e);
#if SIZEOF_DOUBLE == 2*SIZEOF_LONG_INT
      I_R = MkIntTerm(SREG[0]^SREG[1]);
#else
      I_R = MkIntTerm(SREG[0]);
#endif
      GONext();
      ENDOp();
      
      Op(index_long, e);
      PREG = NEXTOP(PREG, e);
      I_R = MkIntTerm(SREG[0] & (MAX_ABS_INT-1));
      GONext();
      ENDOp();
      


/************************************************************************\
*	Native Code Execution						 *
\************************************************************************/
 
      /* native_me  */
      BOp(native_me, aFlp); 

	if (PREG->u.aFlp.n) 
	  EXEC_NATIVE(PREG->u.aFlp.n);
	else {
	  PREG->u.aFlp.n++;
	  if (PREG->u.aFlp.n == MAX_INVOCATION)
	    PREG->u.aFlp.n = Yapc_Compile(PREG->u.aFlp.p);
        }

      PREG = NEXTOP(PREG, aFlp);
      GONext();
      
      ENDBOp();


 
/************************************************************************\
*	Basic Primitive Predicates					 *
\************************************************************************/

      Op(p_atom_x, xl);
      BEGD(d0);
      d0 = XREG(PREG->u.xl.x);
      deref_head(d0, atom_x_unk);
    atom_x_nvar:
      if (IsAtomTerm(d0)) {
	PREG = NEXTOP(PREG, xl);
	GONext();
      }
      else {
	PREG = PREG->u.xl.F;
	GONext();
      }

      BEGP(pt0);
      deref_body(d0, pt0, atom_x_unk, atom_x_nvar);
      PREG = PREG->u.xl.F;
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_atom_y, yl);
      BEGD(d0);
      BEGP(pt0);
      pt0 = YREG + PREG->u.yl.y;
      d0 = *pt0;
      deref_head(d0, atom_y_unk);
    atom_y_nvar:
      if (IsAtomTerm(d0)) {
	PREG = NEXTOP(PREG, yl);
	GONext();
      }
      else {
	PREG = PREG->u.yl.F;
	GONext();
      }

      derefa_body(d0, pt0, atom_y_unk, atom_y_nvar);
      PREG = PREG->u.yl.F;
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_atomic_x, xl);
      BEGD(d0);
      d0 = XREG(PREG->u.xl.x);
      deref_head(d0, atomic_x_unk);
    atomic_x_nvar:
      /* non variable */
      if (IsAtomicTerm(d0)) {
	PREG = NEXTOP(PREG, xl);
	GONext();
      }
      else {
	PREG = PREG->u.xl.F;
	GONext();
      }

      BEGP(pt0);
      deref_body(d0, pt0, atomic_x_unk, atomic_x_nvar);
      PREG = PREG->u.xl.F;
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_atomic_y, yl);
      BEGD(d0);
      BEGP(pt0);
      pt0 = YREG + PREG->u.yl.y;
      d0 = *pt0;
      deref_head(d0, atomic_y_unk);
    atomic_y_nvar:
      /* non variable */
      if (IsAtomicTerm(d0)) {
	PREG = NEXTOP(PREG, yl);
	GONext();
      }
      else {
	PREG = PREG->u.yl.F;
	GONext();
      }

      derefa_body(d0, pt0, atomic_y_unk, atomic_y_nvar);
      PREG = PREG->u.yl.F;
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_integer_x, xl);
      BEGD(d0);
      d0 = XREG(PREG->u.xl.x);
      deref_head(d0, integer_x_unk);
    integer_x_nvar:
      /* non variable */
      if (IsIntTerm(d0)) {
	PREG = NEXTOP(PREG, xl);
	GONext();
      }
      if (IsApplTerm(d0)) {
	Functor f0 = FunctorOfTerm(d0);
	if (IsExtensionFunctor(f0)) {
	  switch ((CELL)f0) {
	  case (CELL)FunctorLongInt:
	  case (CELL)FunctorBigInt:
	    PREG = NEXTOP(PREG, xl);
	    GONext();
	  default:
	    PREG = PREG->u.xl.F;
	    GONext();
	  }
	}
      }
      PREG = PREG->u.xl.F;
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, integer_x_unk, integer_x_nvar);
      PREG = PREG->u.xl.F;
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_integer_y, yl);
      BEGD(d0);
      BEGP(pt0);
      pt0 = YREG + PREG->u.yl.y;
      d0 = *pt0;
      deref_head(d0, integer_y_unk);
    integer_y_nvar:
      /* non variable */
      if (IsIntTerm(d0)) {
	PREG = NEXTOP(PREG, yl);
	GONext();
      }
      if (IsApplTerm(d0)) {
	Functor f0 = FunctorOfTerm(d0);
	if (IsExtensionFunctor(f0)) {
	  switch ((CELL)f0) {
	  case (CELL)FunctorLongInt:
#ifdef USE_GMP
	  case (CELL)FunctorBigInt:
#endif
	    PREG = NEXTOP(PREG, yl);
	    GONext();
	  default:
	    PREG = PREG->u.yl.F;
	    GONext();
	  }
	}
      }
      PREG = PREG->u.yl.F;
      GONext();

      derefa_body(d0, pt0, integer_y_unk, integer_y_nvar);
      PREG = PREG->u.yl.F;
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_nonvar_x, xl);
      BEGD(d0);
      d0 = XREG(PREG->u.xl.x);
      deref_head(d0, nonvar_x_unk);
    nonvar_x_nvar:
      PREG = NEXTOP(PREG, xl);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, nonvar_x_unk, nonvar_x_nvar);
      PREG = PREG->u.xl.F;
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_nonvar_y, yl);
      BEGD(d0);
      BEGP(pt0);
      pt0 = YREG + PREG->u.yl.y;
      d0 = *pt0;
      deref_head(d0, nonvar_y_unk);
    nonvar_y_nvar:
      PREG = NEXTOP(PREG, yl);
      GONext();

      derefa_body(d0, pt0, nonvar_y_unk, nonvar_y_nvar);
      PREG = PREG->u.yl.F;
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_number_x, xl);
      BEGD(d0);
      d0 = XREG(PREG->u.xl.x);
      deref_head(d0, number_x_unk);
    number_x_nvar:
      /* non variable */
      if (IsIntTerm(d0)) {
	PREG = NEXTOP(PREG, xl);
	GONext();
      }
      if (IsApplTerm(d0)) {
	Functor f0 = FunctorOfTerm(d0);
	if (IsExtensionFunctor(f0)) {
	  switch ((CELL)f0) {
	  case (CELL)FunctorLongInt:
	  case (CELL)FunctorDouble:
#ifdef USE_GMP
	  case (CELL)FunctorBigInt:
#endif
	    PREG = NEXTOP(PREG, xl);
	    GONext();
	  default:
	    PREG = PREG->u.xl.F;
	    GONext();
	  } 
	} 
      }
      PREG = PREG->u.xl.F;
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, number_x_unk, number_x_nvar);
      PREG = PREG->u.xl.F;
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_number_y, yl);
      BEGD(d0);
      BEGP(pt0);
      pt0 = YREG + PREG->u.yl.y;
      d0 = *pt0;
      deref_head(d0, number_y_unk);
    number_y_nvar:
      /* non variable */
      /* non variable */
      if (IsIntTerm(d0)) {
	PREG = NEXTOP(PREG, xl);
	GONext();
      }
      if (IsApplTerm(d0)) {
	Functor f0 = FunctorOfTerm(d0);
	if (IsExtensionFunctor(f0)) {
	  switch ((CELL)f0) {
	  case (CELL)FunctorLongInt:
	  case (CELL)FunctorDouble:
#ifdef USE_GMP
	  case (CELL)FunctorBigInt:
#endif
	    PREG = NEXTOP(PREG, yl);
	    GONext();
	  default:
	    PREG = PREG->u.yl.F;
	    GONext();
	  } 
	}
      }
      PREG = PREG->u.xl.F;
      GONext();

      derefa_body(d0, pt0, number_y_unk, number_y_nvar);
      PREG = PREG->u.yl.F;
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_var_x, xl);
      BEGD(d0);
      d0 = XREG(PREG->u.xl.x);
      deref_head(d0, var_x_unk);
    var_x_nvar:
      /* non variable */
      PREG = PREG->u.xl.F;
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, var_x_unk, var_x_nvar);
      PREG = NEXTOP(PREG, xl);
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_var_y, yl);
      BEGD(d0);
      BEGP(pt0);
      pt0 = YREG + PREG->u.yl.y;
      d0 = *pt0;
      deref_head(d0, var_y_unk);
    var_y_nvar:
      /* non variable */
      PREG = PREG->u.yl.F;
      GONext();

      derefa_body(d0, pt0, var_y_unk, var_y_nvar);
      PREG = NEXTOP(PREG, yl);
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_db_ref_x, xl);
      BEGD(d0);
      d0 = XREG(PREG->u.xl.x);
      deref_head(d0, dbref_x_unk);
    dbref_x_nvar:
      /* non variable */
      if (IsDBRefTerm(d0)) {
	/* only allow references to the database, not general references
	 * to go through. */
	PREG = NEXTOP(PREG, xl);
	GONext();
      }
      else {
	PREG = PREG->u.xl.F;
	GONext();
      }

      BEGP(pt0);
      deref_body(d0, pt0, dbref_x_unk, dbref_x_nvar);
      PREG = PREG->u.xl.F;
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_db_ref_y, yl);
      BEGD(d0);
      BEGP(pt0);
      pt0 = YREG + PREG->u.yl.y;
      d0 = *pt0;
      deref_head(d0, dbref_y_unk);
    dbref_y_nvar:
      /* non variable */
      if (IsDBRefTerm(d0)) {
	/* only allow references to the database, not general references
	 * to go through. */
	PREG = NEXTOP(PREG, yl);
	GONext();
      }
      else {
	PREG = PREG->u.yl.F;
	GONext();
      }

      derefa_body(d0, pt0, dbref_y_unk, dbref_y_nvar);
      PREG = PREG->u.yl.F;
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_primitive_x, xl);
      BEGD(d0);
      d0 = XREG(PREG->u.xl.x);
      deref_head(d0, primi_x_unk);
    primi_x_nvar:
      /* non variable */
      if (IsPrimitiveTerm(d0)) {
	PREG = NEXTOP(PREG, xl);
	GONext();
      }
      else {
	PREG = PREG->u.xl.F;
	GONext();
      }

      BEGP(pt0);
      deref_body(d0, pt0, primi_x_unk, primi_x_nvar);
      PREG = PREG->u.xl.F;
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_primitive_y, yl);
      BEGD(d0);
      BEGP(pt0);
      pt0 = YREG + PREG->u.yl.y;
      d0 = *pt0;
      deref_head(d0, primi_y_unk);
    primi_y_nvar:
      /* non variable */
      if (IsPrimitiveTerm(d0)) {
	PREG = NEXTOP(PREG, yl);
	GONext();
      }
      else {
	PREG = PREG->u.yl.F;
	GONext();
      }

      derefa_body(d0, pt0, primi_y_unk, primi_y_nvar);
      PREG = PREG->u.yl.F;
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_compound_x, xl);
      BEGD(d0);
      d0 = XREG(PREG->u.xl.x);
      deref_head(d0, compound_x_unk);
    compound_x_nvar:
      /* non variable */
      if (IsPairTerm(d0)) {
	PREG = NEXTOP(PREG, xl);
	GONext();
      }
      else if (IsApplTerm(d0)) {
	if (IsExtensionFunctor(FunctorOfTerm(d0))) {
	  PREG = PREG->u.xl.F;
	  GONext();
	}
	PREG = NEXTOP(PREG, xl);
	GONext();
      }
      else {
	PREG = PREG->u.xl.F;
	GONext();
      }

      BEGP(pt0);
      deref_body(d0, pt0, compound_x_unk, compound_x_nvar);
      PREG = PREG->u.xl.F;
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_compound_y, yl);
      BEGD(d0);
      BEGP(pt0);
      pt0 = YREG + PREG->u.yl.y;
      d0 = *pt0;
      deref_head(d0, compound_y_unk);
    compound_y_nvar:
      /* non variable */
      if (IsPairTerm(d0)) {
	PREG = NEXTOP(PREG, yl);
	GONext();
      }
      else if (IsApplTerm(d0)) {
	if (IsExtensionFunctor(FunctorOfTerm(d0))) {
	  PREG = PREG->u.yl.F;
	  GONext();
	}
	PREG = NEXTOP(PREG, yl);
	GONext();
      }
      else {
	PREG = PREG->u.yl.F;
	GONext();
      }

      derefa_body(d0, pt0, compound_y_unk, compound_y_nvar);
      PREG = PREG->u.yl.F;
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_float_x, xl);
      BEGD(d0);
      d0 = XREG(PREG->u.xl.x);
      deref_head(d0, float_x_unk);
    float_x_nvar:
      /* non variable */
      if (IsFloatTerm(d0)) {
	PREG = NEXTOP(PREG, xl);
	GONext();
      }
      PREG = PREG->u.xl.F;
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, float_x_unk, float_x_nvar);
      PREG = PREG->u.xl.F;
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_float_y, yl);
      BEGD(d0);
      BEGP(pt0);
      pt0 = YREG + PREG->u.yl.y;
      d0 = *pt0;
      deref_head(d0, float_y_unk);
    float_y_nvar:
      /* non variable */
      if (IsFloatTerm(d0)) {
	PREG = NEXTOP(PREG, yl);
	GONext();
      }
      PREG = PREG->u.yl.F;
      GONext();

      derefa_body(d0, pt0, float_y_unk, float_y_nvar);
      PREG = PREG->u.yl.F;
      GONext();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_cut_by_x, xl);
      BEGD(d0);
      d0 = XREG(PREG->u.xl.x);
      deref_head(d0, cutby_x_unk);
    cutby_x_nvar:
#if defined(SBA) && defined(FROZEN_STACKS)
      if (!IsIntegerTerm(d0))
#else
      if (!IsIntTerm(d0))
#endif /* SBA && FROZEN_STACKS */
	{
	  PREG = NEXTOP(PREG, xl);
	  GONext();
	}
      BEGCHO(pt0);
#if defined(SBA) && defined(FROZEN_STACKS)
      pt0 = (choiceptr)IntegerOfTerm(d0);
#else
      pt0 = (choiceptr)(LCL0-IntOfTerm(d0));
#endif /* SBA && FROZEN_STACKS */
#ifdef CUT_C
      {
	if (SHOULD_CUT_UP_TO(B, pt0))
	  {
	    while (POP_CHOICE_POINT(pt0))
	      {
		POP_EXECUTE();
	      }
	  }
      }
#endif /* CUT_C */
#ifdef YAPOR
      CUT_prune_to(pt0);
#endif /* YAPOR */
      /* find where to cut to */
      if (SHOULD_CUT_UP_TO(B,pt0)) {
	/* Wow, we're gonna cut!!! */
	while (B->cp_b < pt0) {
	  B = B->cp_b;
	}
#ifdef TABLING
	abolish_incomplete_subgoals(B);
#endif /* TABLING */
	PREG = NEXTOP(PREG, xl);
	goto trim_trail;
      }
      PREG = NEXTOP(PREG, xl);
      ENDCHO(pt0);
      GONext();

      BEGP(pt1);
      deref_body(d0, pt1, cutby_x_unk, cutby_x_nvar);
      ENDP(pt1);
      /* never cut to a variable */
      /* Abort */
      FAIL();
      ENDD(d0);
      ENDOp();

      Op(p_cut_by_y, yl);
      BEGD(d0);
      BEGP(pt0);
      pt0 = YREG + PREG->u.yl.y;
      d0 = *pt0;
      deref_head(d0, cutby_y_unk);
    cutby_y_nvar:
#if defined(SBA) && defined(FROZEN_STACKS)
      if (!IsIntegerTerm(d0))
#else
      if (!IsIntTerm(d0))
#endif
	{
	  FAIL();
	}
      /* find where to cut to */
      BEGCHO(pt1);
#if defined(SBA) && defined(FROZEN_STACKS)
      pt1 = (choiceptr)IntegerOfTerm(d0);
#else
      pt1 = (choiceptr)(LCL0-IntOfTerm(d0));
#endif /* SBA && FROZEN_STACKS */
#ifdef CUT_C
      {
	if (SHOULD_CUT_UP_TO(B,(choiceptr) pt1))
	  {
	    while (POP_CHOICE_POINT(pt1))
	      {
		POP_EXECUTE();
	      }
	  }
      }
#endif /* CUT_C */
#ifdef YAPOR
      CUT_prune_to(pt1);
#endif /* YAPOR */
      if (SHOULD_CUT_UP_TO(B,pt1)) {
	/* Wow, we're gonna cut!!! */
	while (B->cp_b < pt1) {
	  B = B->cp_b;
	}
#ifdef TABLING
	abolish_incomplete_subgoals(B);
#endif /* TABLING */
	PREG = NEXTOP(PREG, xl);
	goto trim_trail;
      }
      PREG = NEXTOP(PREG, yl);
      GONext();
      ENDCHO(pt1);

      derefa_body(d0, pt0, cutby_y_unk, cutby_y_nvar);
      /* never cut to a variable */
      /* Abort */
      FAIL();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_plus_vv, xxx);
      BEGD(d0);
      BEGD(d1);
      d0 = XREG(PREG->u.xxx.x1);
      /* first check pt1 */
      deref_head(d0, plus_vv_unk);
    plus_vv_nvar:
      d1 = XREG(PREG->u.xxx.x2);
      /* next check A2 */
      deref_head(d1, plus_vv_nvar_unk);
    plus_vv_nvar_nvar:
      /* d0 and d1 are where I want them */
      if (IsIntTerm(d0) && IsIntTerm(d1)) {
	d0 = MkIntegerTerm(IntOfTerm(d0) + IntOfTerm(d1));
      }
      else {
	saveregs();
	d0 = p_plus(Yap_Eval(d0), Yap_Eval(d1));
	setregs();
	if (d0 == 0L) {
	  saveregs();
	  Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	  setregs();
	  FAIL();
	}
      }
      XREG(PREG->u.xxx.x) = d0;
      PREG = NEXTOP(PREG, xxx);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, plus_vv_unk, plus_vv_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is _+B");
      setregs();
      FAIL();
      ENDP(pt0);

      BEGP(pt0);
      deref_body(d1, pt0, plus_vv_nvar_unk, plus_vv_nvar_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A+B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(p_plus_vc, xxn);
      BEGD(d0);
      d0 = XREG(PREG->u.xxn.xi);
      /* first check pt1 */
      deref_head(d0, plus_vc_unk);
    plus_vc_nvar:
      {
	Int d1 = PREG->u.xxn.c;
	if (IsIntTerm(d0)) {
	  d0 = MkIntegerTerm(IntOfTerm(d0) + d1);
	}
	else {
	  saveregs();
	  d0 = p_plus(Yap_Eval(d0), MkIntegerTerm(d1));
	  setregs();
	  if (d0 == 0L) {
	    saveregs();
	    Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	    setregs();
	    FAIL();
	  }
	}
      }
      XREG(PREG->u.xxn.x) = d0;
      PREG = NEXTOP(PREG, xxn);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, plus_vc_unk, plus_vc_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A+B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_plus_y_vv, yxx);
      BEGD(d0);
      BEGD(d1);
      d0 = XREG(PREG->u.yxx.x1);
      /* first check pt1 */
      deref_head(d0, plus_y_vv_unk);
    plus_y_vv_nvar:
      d1 = XREG(PREG->u.xxx.x2);
      /* next check A2 */
      deref_head(d1, plus_y_vv_nvar_unk);
    plus_y_vv_nvar_nvar:
      /* d0 and d1 are where I want them */
      if (IsIntTerm(d0) && IsIntTerm(d1)) {
	d0 = MkIntegerTerm(IntOfTerm(d0) + IntOfTerm(d1));
      }
      else {
	saveregs();
	d0 = p_plus(Yap_Eval(d0), Yap_Eval(d1));
	setregs();
	if (d0 == 0L) {
	  saveregs();
	  Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	  setregs();
	  FAIL();
	}
      }
      BEGP(pt0);
      pt0 = YREG + PREG->u.yxx.y;
      PREG = NEXTOP(PREG, yxx);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(pt0,d0);
#else
      *pt0 = d0;
#endif /* SBA && FROZEN_STACKS */
      ENDP(pt0);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, plus_y_vv_unk, plus_y_vv_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A+B");
      setregs();
      FAIL();
      ENDP(pt0);

      BEGP(pt0);
      deref_body(d1, pt0, plus_y_vv_nvar_unk, plus_y_vv_nvar_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A+B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(p_plus_y_vc, yxn);
      BEGD(d0);
      d0 = XREG(PREG->u.yxn.xi);
      /* first check pt1 */
      deref_head(d0, plus_y_vc_unk);
    plus_y_vc_nvar:
      {
	Int d1 = PREG->u.yxn.c;
	if (IsIntTerm(d0)) {
	  d0 = MkIntegerTerm(IntOfTerm(d0) + d1);
	}
	else {
	  saveregs();
	  d0 = p_plus(Yap_Eval(d0), MkIntegerTerm(d1));
	  setregs();
	  if (d0 == 0L) {
	    saveregs();
	    Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	    setregs();
	    FAIL();
	  }
	}
      }
      BEGP(pt0);
      pt0 = YREG + PREG->u.yxn.y;
      PREG = NEXTOP(PREG, yxn);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(pt0,d0);
#else
      *pt0 = d0;
#endif /* SBA && FROZEN_STACKS */
      ENDP(pt0);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, plus_y_vc_unk, plus_y_vc_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A+B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_minus_vv, xxx);
      BEGD(d0);
      BEGD(d1);
      d0 = XREG(PREG->u.xxx.x1);
      /* first check pt1 */
      deref_head(d0, minus_vv_unk);
    minus_vv_nvar:
      d1 = XREG(PREG->u.xxx.x2);
      /* next check A2 */
      deref_head(d1, minus_vv_nvar_unk);
    minus_vv_nvar_nvar:
      /* d0 and d1 are where I want them */
      if (IsIntTerm(d0) && IsIntTerm(d1)) {
	d0 = MkIntegerTerm(IntOfTerm(d0) - IntOfTerm(d1));
      }
      else {
	saveregs();
	d0 = p_minus(Yap_Eval(d0), Yap_Eval(d1));
	setregs();
	if (d0 == 0L) {
	  saveregs();
	  Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	  setregs();
	  FAIL();
	}
      }
      XREG(PREG->u.xxx.x) = d0;
      PREG = NEXTOP(PREG, xxx);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, minus_vv_unk, minus_vv_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A-B");
      setregs();
      FAIL();
      ENDP(pt0);

      BEGP(pt0);
      deref_body(d1, pt0, minus_vv_nvar_unk, minus_vv_nvar_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A-B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(p_minus_cv, xxn);
      BEGD(d0);
      d0 = XREG(PREG->u.xxn.xi);
      /* first check pt1 */
      deref_head(d0, minus_cv_unk);
    minus_cv_nvar:
      {
	Int d1 = PREG->u.xxn.c;
	if (IsIntTerm(d0)) {
	  d0 = MkIntegerTerm(d1 - IntOfTerm(d0));
	}
	else {
	  saveregs();
	  d0 = p_minus(MkIntegerTerm(d1),Yap_Eval(d0));
	  setregs();
	  if (d0 == 0L) {
	    saveregs();
	    Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	    setregs();
	    FAIL();
	  }
	}
      }
      XREG(PREG->u.xxn.x) = d0;
      PREG = NEXTOP(PREG, xxn);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, minus_cv_unk, minus_cv_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A-B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_minus_y_vv, yxx);
      BEGD(d0);
      BEGD(d1);
      d0 = XREG(PREG->u.yxx.x1);
      /* first check pt1 */
      deref_head(d0, minus_y_vv_unk);
    minus_y_vv_nvar:
      d1 = XREG(PREG->u.xxx.x2);
      /* next check A2 */
      deref_head(d1, minus_y_vv_nvar_unk);
    minus_y_vv_nvar_nvar:
      /* d0 and d1 are where I want them */
      if (IsIntTerm(d0) && IsIntTerm(d1)) {
	d0 = MkIntegerTerm(IntOfTerm(d0) - IntOfTerm(d1));
      }
      else {
	saveregs();
	d0 = p_minus(Yap_Eval(d0), Yap_Eval(d1));
	setregs();
	if (d0 == 0L) {
	  saveregs();
	  Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	  setregs();
	  FAIL();
	}
      }
      BEGP(pt0);
      pt0 = YREG + PREG->u.yxx.y;
      PREG = NEXTOP(PREG, yxx);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(pt0,d0);
#else
      *pt0 = d0;
#endif /* SBA && FROZEN_STACKS */
      ENDP(pt0);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, minus_y_vv_unk, minus_y_vv_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A-B");
      setregs();
      FAIL();
      ENDP(pt0);

      BEGP(pt0);
      deref_body(d1, pt0, minus_y_vv_nvar_unk, minus_y_vv_nvar_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A-B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(p_minus_y_cv, yxn);
      BEGD(d0);
      d0 = XREG(PREG->u.yxn.xi);
      /* first check pt1 */
      deref_head(d0, minus_y_cv_unk);
    minus_y_cv_nvar:
      {
	Int d1 = PREG->u.yxn.c;
	if (IsIntTerm(d0)) {
	  d0 = MkIntegerTerm(d1 - IntOfTerm(d0));
	}
	else {
	  saveregs();
	  d0 = p_minus(MkIntegerTerm(d1), Yap_Eval(d0));
	  setregs();
	  if (d0 == 0L) {
	    saveregs();
	    Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	    setregs();
	    FAIL();
	  }
	}
      }
      BEGP(pt0);
      pt0 = YREG + PREG->u.yxn.y;
      PREG = NEXTOP(PREG, yxn);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(pt0,d0);
#else
      *pt0 = d0;
#endif /* SBA && FROZEN_STACKS */
      ENDP(pt0);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, minus_y_cv_unk, minus_y_cv_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A-B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_times_vv, xxx);
      BEGD(d0);
      BEGD(d1);
      d0 = XREG(PREG->u.xxx.x1);
      /* first check pt1 */
      deref_head(d0, times_vv_unk);
    times_vv_nvar:
      d1 = XREG(PREG->u.xxx.x2);
      /* next check A2 */
      deref_head(d1, times_vv_nvar_unk);
    times_vv_nvar_nvar:
      /* d0 and d1 are where I want them */
      if (IsIntTerm(d0) && IsIntTerm(d1)) {
	d0 = times_int(IntOfTerm(d0), IntOfTerm(d1));
      }
      else {
	saveregs();
	d0 = p_times(Yap_Eval(d0), Yap_Eval(d1));
	setregs();
	if (d0 == 0L) {
	  saveregs();
	  Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	  setregs();
	  FAIL();
	}
      }
      XREG(PREG->u.xxx.x) = d0;
      PREG = NEXTOP(PREG, xxx);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, times_vv_unk, times_vv_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A*B");
      setregs();
      FAIL();
      ENDP(pt0);

      BEGP(pt0);
      deref_body(d1, pt0, times_vv_nvar_unk, times_vv_nvar_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A*B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(p_times_vc, xxn);
      BEGD(d0);
      d0 = XREG(PREG->u.xxn.xi);
      /* first check pt1 */
      deref_head(d0, times_vc_unk);
    times_vc_nvar:
      {
	Int d1 = PREG->u.xxn.c;
	if (IsIntTerm(d0)) {
	  d0 = times_int(IntOfTerm(d0), d1);
	}
	else {
	  saveregs();
	  d0 = p_times(Yap_Eval(d0), MkIntegerTerm(d1));
	  setregs();
	  if (d0 == 0L) {
	    saveregs();
	    Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	    setregs();
	    FAIL();
	  }
	}
      }
      XREG(PREG->u.xxn.x) = d0;
      PREG = NEXTOP(PREG, xxn);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, times_vc_unk, times_vc_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A*B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_times_y_vv, yxx);
      BEGD(d0);
      BEGD(d1);
      d0 = XREG(PREG->u.yxx.x1);
      /* first check pt1 */
      deref_head(d0, times_y_vv_unk);
    times_y_vv_nvar:
      d1 = XREG(PREG->u.xxx.x2);
      /* next check A2 */
      deref_head(d1, times_y_vv_nvar_unk);
    times_y_vv_nvar_nvar:
      /* d0 and d1 are where I want them */
      if (IsIntTerm(d0) && IsIntTerm(d1)) {
	d0 = times_int(IntOfTerm(d0), IntOfTerm(d1));
      }
      else {
	saveregs();
	d0 = p_times(Yap_Eval(d0), Yap_Eval(d1));
	setregs();
	if (d0 == 0L) {
	  saveregs();
	  Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	  setregs();
	  FAIL();
	}
      }
      BEGP(pt0);
      pt0 = YREG + PREG->u.yxx.y;
      PREG = NEXTOP(PREG, yxx);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(pt0,d0);
#else
      *pt0 = d0;
#endif /* SBA && FROZEN_STACKS */
      ENDP(pt0);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, times_y_vv_unk, times_y_vv_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A*B");
      setregs();
      FAIL();
      ENDP(pt0);

      BEGP(pt0);
      deref_body(d1, pt0, times_y_vv_nvar_unk, times_y_vv_nvar_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A*B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(p_times_y_vc, yxn);
      BEGD(d0);
      d0 = XREG(PREG->u.yxn.xi);
      /* first check pt1 */
      deref_head(d0, times_y_vc_unk);
    times_y_vc_nvar:
      {
	Int d1 = PREG->u.yxn.c;
	if (IsIntTerm(d0)) {
	  d0 = times_int(IntOfTerm(d0), d1);
	}
	else {
	  saveregs();
	  d0 = p_times(Yap_Eval(d0), MkIntegerTerm(d1));
	  setregs();
	  if (d0 == 0L) {
	    saveregs();
	    Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	    setregs();
	    FAIL();
	  }
	}
      }
      BEGP(pt0);
      pt0 = YREG + PREG->u.yxn.y;
      PREG = NEXTOP(PREG, yxn);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(pt0,d0);
#else
      *pt0 = d0;
#endif /* SBA && FROZEN_STACKS */
      ENDP(pt0);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, times_y_vc_unk, times_y_vc_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A*B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_div_vv, xxx);
      BEGD(d0);
      BEGD(d1);
      d0 = XREG(PREG->u.xxx.x1);
      /* first check pt1 */
      deref_head(d0, div_vv_unk);
    div_vv_nvar:
      d1 = XREG(PREG->u.xxx.x2);
      /* next check A2 */
      deref_head(d1, div_vv_nvar_unk);
    div_vv_nvar_nvar:
      /* d0 and d1 are where I want them */
      if (IsIntTerm(d0) && IsIntTerm(d1)) {
	Int div = IntOfTerm(d1);
	if (div == 0) {
	  saveregs();
	  Yap_Error(EVALUATION_ERROR_ZERO_DIVISOR,TermNil,"// /2");
	  setregs();
	  FAIL();
	}
	d0 = MkIntTerm(IntOfTerm(d0) / div);
      }
      else {
	saveregs();
	d0 = p_div(Yap_Eval(d0), Yap_Eval(d1));
	setregs();
	if (d0 == 0L) {
	  saveregs();
	  Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	  setregs();
	  FAIL();
	}
      }
      XREG(PREG->u.xxx.x) = d0;
      PREG = NEXTOP(PREG, xxx);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, div_vv_unk, div_vv_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A//B");
      setregs();
      FAIL();
      ENDP(pt0);

      BEGP(pt0);
      deref_body(d1, pt0, div_vv_nvar_unk, div_vv_nvar_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A//B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(p_div_vc, xxn);
      BEGD(d0);
      d0 = XREG(PREG->u.xxn.xi);
      /* first check pt1 */
      deref_head(d0, div_vc_unk);
    div_vc_nvar:
      {
	Int d1 = PREG->u.xxn.c;
	if (IsIntTerm(d0)) {
	  d0 = MkIntTerm(IntOfTerm(d0) / d1);
	}
	else {
	  saveregs();
	  d0 = p_div(Yap_Eval(d0),MkIntegerTerm(d1));
	  setregs();
	  if (d0 == 0L) {
	    saveregs();
	    Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	    setregs();
	    FAIL();
	  }
	}
      }
      XREG(PREG->u.xxn.x) = d0;
      PREG = NEXTOP(PREG, xxn);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, div_vc_unk, div_vc_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A//B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_div_cv, xxn);
      BEGD(d0);
      d0 = XREG(PREG->u.xxn.xi);
      /* first check pt1 */
      deref_head(d0, div_cv_unk);
    div_cv_nvar:
      {
	Int d1 = PREG->u.xxn.c;
	if (IsIntTerm(d0)) {
	  Int div = IntOfTerm(d0);
	  if (div == 0){
	    saveregs();
	    Yap_Error(EVALUATION_ERROR_ZERO_DIVISOR,TermNil,"// /2");
	    setregs();
	    FAIL();
	  }
	  d0 = MkIntegerTerm(d1 / div);
	}
	else {
	  saveregs();
	  d0 = p_div(MkIntegerTerm(d1),Yap_Eval(d0));
	  if (d0 == 0L) {
	    saveregs();
	    Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	    setregs();
	    FAIL();
	  }
	}
      }
      XREG(PREG->u.xxn.x) = d0;
      PREG = NEXTOP(PREG, xxn);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, div_cv_unk, div_cv_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A//B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_div_y_vv, yxx);
      BEGD(d0);
      BEGD(d1);
      d0 = XREG(PREG->u.yxx.x1);
      /* first check pt1 */
      deref_head(d0, div_y_vv_unk);
    div_y_vv_nvar:
      d1 = XREG(PREG->u.xxx.x2);
      /* next check A2 */
      deref_head(d1, div_y_vv_nvar_unk);
    div_y_vv_nvar_nvar:
      /* d0 and d1 are where I want them */
      if (IsIntTerm(d0) && IsIntTerm(d1)) {
	Int div = IntOfTerm(d1);
	if (div == 0) {
	  saveregs();
	  Yap_Error(EVALUATION_ERROR_ZERO_DIVISOR,TermNil,"// /2");
	  setregs();
	  FAIL();
	}
	d0 = MkIntTerm(IntOfTerm(d0) / div);
      }
      else {
	saveregs();
	d0 = p_div(Yap_Eval(d0), Yap_Eval(d1));
	setregs();
	if (d0 == 0L) {
	  saveregs();
	  Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	  setregs();
	  FAIL();
	}
      }
      BEGP(pt0);
      pt0 = YREG + PREG->u.yxx.y;
      PREG = NEXTOP(PREG, yxx);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(pt0,d0);
#else
      *pt0 = d0;
#endif /* SBA && FROZEN_STACKS */
      ENDP(pt0);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, div_y_vv_unk, div_y_vv_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A//B");
      setregs();
      FAIL();
      ENDP(pt0);

      BEGP(pt0);
      deref_body(d1, pt0, div_y_vv_nvar_unk, div_y_vv_nvar_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A//B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(p_div_y_vc, yxn);
      BEGD(d0);
      d0 = XREG(PREG->u.yxn.xi);
      /* first check pt1 */
      deref_head(d0, div_y_vc_unk);
    div_y_vc_nvar:
      {
	Int d1 = PREG->u.yxn.c;
	if (IsIntTerm(d0)) {
	  d0 = MkIntTerm(IntOfTerm(d0)/d1);
	}
	else {
	  saveregs();
	  d0 = p_div(Yap_Eval(d0),MkIntegerTerm(d1));
	  setregs();
	  if (d0 == 0L) {
	    saveregs();
	    Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	    setregs();
	    FAIL();
	  }
	}
      }
      BEGP(pt0);
      pt0 = YREG + PREG->u.yxn.y;
      PREG = NEXTOP(PREG, yxn);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(pt0,d0);
#else
      *pt0 = d0;
#endif /* SBA && FROZEN_STACKS */
      ENDP(pt0);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, div_y_vc_unk, div_y_vc_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A//B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_div_y_cv, yxn);
      BEGD(d0);
      d0 = XREG(PREG->u.yxn.xi);
      /* first check pt1 */
      deref_head(d0, div_y_cv_unk);
    div_y_cv_nvar:
      {
	Int d1 = PREG->u.yxn.c;
	if (IsIntTerm(d0)) {
	  Int div = IntOfTerm(d0);
	  if (div == 0) {
	    saveregs();
	    Yap_Error(EVALUATION_ERROR_ZERO_DIVISOR,TermNil,"// /2");
	    setregs();
	    FAIL();
	  }
	  d0 = MkIntegerTerm(d1 / div);
	}
	else {
	  saveregs();
	  d0 = p_div(MkIntegerTerm(d1), Yap_Eval(d0));
	  setregs();
	  if (d0 == 0L) {
	    saveregs();
	    Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	    setregs();
	    FAIL();
	  }
	}
      }
      BEGP(pt0);
      pt0 = YREG + PREG->u.yxn.y;
      PREG = NEXTOP(PREG, yxn);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(pt0,d0);
#else
      *pt0 = d0;
#endif /* SBA && FROZEN_STACKS */
      ENDP(pt0);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, div_y_cv_unk, div_y_cv_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A//B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();


      Op(p_and_vv, xxx);
      BEGD(d0);
      BEGD(d1);
      d0 = XREG(PREG->u.xxx.x1);
      /* first check pt1 */
      deref_head(d0, and_vv_unk);
    and_vv_nvar:
      d1 = XREG(PREG->u.xxx.x2);
      /* next check A2 */
      deref_head(d1, and_vv_nvar_unk);
    and_vv_nvar_nvar:
      /* d0 and d1 are where I want them */
      if (IsIntTerm(d0) && IsIntTerm(d1)) {
	d0 = MkIntegerTerm(IntOfTerm(d0) & IntOfTerm(d1));
      }
      else {
	saveregs();
	d0 = p_and(Yap_Eval(d0), Yap_Eval(d1));
	setregs();
	if (d0 == 0L) {
	  saveregs();
	  Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	  setregs();
	  FAIL();
	}
      }
      XREG(PREG->u.xxx.x) = d0;
      PREG = NEXTOP(PREG, xxx);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, and_vv_unk, and_vv_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A/\\B");
      setregs();
      FAIL();
      ENDP(pt0);

      BEGP(pt0);
      deref_body(d1, pt0, and_vv_nvar_unk, and_vv_nvar_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A/\\B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(p_and_vc, xxn);
      BEGD(d0);
      d0 = XREG(PREG->u.xxn.xi);
      /* first check pt1 */
      deref_head(d0, and_vc_unk);
    and_vc_nvar:
      {
	Int d1 = PREG->u.xxn.c;
	if (IsIntTerm(d0)) {
	  d0 = MkIntegerTerm(IntOfTerm(d0) & d1);
	}
	else {
	  saveregs();
	  d0 = p_and(Yap_Eval(d0), MkIntegerTerm(d1));
	  setregs();
	  if (d0 == 0L) {
	    saveregs();
	    Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	    setregs();
	    FAIL();
	  }
	}
      }
      XREG(PREG->u.xxn.x) = d0;
      PREG = NEXTOP(PREG, xxn);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, and_vc_unk, and_vc_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A/\\B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_and_y_vv, yxx);
      BEGD(d0);
      BEGD(d1);
      d0 = XREG(PREG->u.yxx.x1);
      /* first check pt1 */
      deref_head(d0, and_y_vv_unk);
    and_y_vv_nvar:
      d1 = XREG(PREG->u.xxx.x2);
      /* next check A2 */
      deref_head(d1, and_y_vv_nvar_unk);
    and_y_vv_nvar_nvar:
      /* d0 and d1 are where I want them */
      if (IsIntTerm(d0) && IsIntTerm(d1)) {
	d0 = MkIntegerTerm(IntOfTerm(d0) & IntOfTerm(d1));
      }
      else {
	saveregs();
	d0 = p_and(Yap_Eval(d0), Yap_Eval(d1));
	setregs();
	if (d0 == 0L) {
	  saveregs();
	  Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	  setregs();
	  FAIL();
	}
      }
      BEGP(pt0);
      pt0 = YREG + PREG->u.yxx.y;
      PREG = NEXTOP(PREG, yxx);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(pt0,d0);
#else
      *pt0 = d0;
#endif /* SBA && FROZEN_STACKS */
      ENDP(pt0);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, and_y_vv_unk, and_y_vv_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A/\\B");
      setregs();
      FAIL();
      ENDP(pt0);

      BEGP(pt0);
      deref_body(d1, pt0, and_y_vv_nvar_unk, and_y_vv_nvar_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A/\\B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(p_and_y_vc, yxn);
      BEGD(d0);
      d0 = XREG(PREG->u.yxn.xi);
      /* first check pt1 */
      deref_head(d0, and_y_vc_unk);
    and_y_vc_nvar:
      {
	Int d1 = PREG->u.yxn.c;
	if (IsIntTerm(d0)) {
	  d0 = MkIntegerTerm(IntOfTerm(d0) & d1);
	}
	else {
	  saveregs();
	  d0 = p_and(Yap_Eval(d0), MkIntegerTerm(d1));
	  setregs();
	  if (d0 == 0L) {
	    saveregs();
	    Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	    setregs();
	    FAIL();
	  }
	}
      }
      BEGP(pt0);
      pt0 = YREG + PREG->u.yxn.y;
      PREG = NEXTOP(PREG, yxn);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(pt0,d0);
#else
      *pt0 = d0;
#endif /* SBA && FROZEN_STACKS */
      ENDP(pt0);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, and_y_vc_unk, and_y_vc_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A/\\B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();


      Op(p_or_vv, xxx);
      BEGD(d0);
      BEGD(d1);
      d0 = XREG(PREG->u.xxx.x1);
      /* first check pt1 */
      deref_head(d0, or_vv_unk);
    or_vv_nvar:
      d1 = XREG(PREG->u.xxx.x2);
      /* next check A2 */
      deref_head(d1, or_vv_nvar_unk);
    or_vv_nvar_nvar:
      /* d0 and d1 are where I want them */
      if (IsIntTerm(d0) && IsIntTerm(d1)) {
	d0 = MkIntegerTerm(IntOfTerm(d0) | IntOfTerm(d1));
      }
      else {
	saveregs();
	d0 = p_or(Yap_Eval(d0), Yap_Eval(d1));
	setregs();
	if (d0 == 0L) {
	  saveregs();
	  Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	  setregs();
	  FAIL();
	}
      }
      XREG(PREG->u.xxx.x) = d0;
      PREG = NEXTOP(PREG, xxx);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, or_vv_unk, or_vv_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A\\/B");
      setregs();
      FAIL();
      ENDP(pt0);

      BEGP(pt0);
      deref_body(d1, pt0, or_vv_nvar_unk, or_vv_nvar_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A\\/B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(p_or_vc, xxn);
      BEGD(d0);
      d0 = XREG(PREG->u.xxn.xi);
      /* first check pt1 */
      deref_head(d0, or_vc_unk);
    or_vc_nvar:
      {
	Int d1 = PREG->u.xxn.c;
	if (IsIntTerm(d0)) {
	  d0 = MkIntegerTerm(IntOfTerm(d0) | d1);
	}
	else {
	  saveregs();
	  d0 = p_or(Yap_Eval(d0), MkIntegerTerm(d1));
	  if (d0 == 0L) {
	    saveregs();
	    Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	    setregs();
	    FAIL();
	  }
	}
      }
      XREG(PREG->u.xxn.x) = d0;
      PREG = NEXTOP(PREG, xxn);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, or_vc_unk, or_vc_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A\\/B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_or_y_vv, yxx);
      BEGD(d0);
      BEGD(d1);
      d0 = XREG(PREG->u.yxx.x1);
      /* first check pt1 */
      deref_head(d0, or_y_vv_unk);
    or_y_vv_nvar:
      d1 = XREG(PREG->u.xxx.x2);
      /* next check A2 */
      deref_head(d1, or_y_vv_nvar_unk);
    or_y_vv_nvar_nvar:
      /* d0 and d1 are where I want them */
      if (IsIntTerm(d0) && IsIntTerm(d1)) {
	d0 = MkIntegerTerm(IntOfTerm(d0) | IntOfTerm(d1));
      }
      else {
	saveregs();
	d0 = p_or(Yap_Eval(d0), Yap_Eval(d1));
	setregs();
	if (d0 == 0L) {
	  saveregs();
	  Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	  setregs();
	  FAIL();
	}
      }
      BEGP(pt0);
      pt0 = YREG + PREG->u.yxx.y;
      PREG = NEXTOP(PREG, yxx);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(pt0,d0);
#else
      *pt0 = d0;
#endif /* SBA && FROZEN_STACKS */
      ENDP(pt0);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, or_y_vv_unk, or_y_vv_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A\\/B");
      setregs();
      FAIL();
      ENDP(pt0);

      BEGP(pt0);
      deref_body(d1, pt0, or_y_vv_nvar_unk, or_y_vv_nvar_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A\\/B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(p_or_y_vc, yxn);
      BEGD(d0);
      d0 = XREG(PREG->u.yxn.xi);
      /* first check pt1 */
      deref_head(d0, or_y_vc_unk);
    or_y_vc_nvar:
      {
	Int d1 = PREG->u.yxn.c;
	if (IsIntTerm(d0)) {
	  d0 = MkIntegerTerm(IntOfTerm(d0) | d1);
	}
	else {
	  saveregs();
	  d0 = p_or(Yap_Eval(d0), MkIntegerTerm(d1));
	  setregs();
	  if (d0 == 0L) {
	    saveregs();
	    Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	    setregs();
	    FAIL();
	  }
	}
      }
      BEGP(pt0);
      pt0 = YREG + PREG->u.yxn.y;
      PREG = NEXTOP(PREG, yxn);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(pt0,d0);
#else
      *pt0 = d0;
#endif /* SBA && FROZEN_STACKS */
      ENDP(pt0);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, or_y_vc_unk, or_y_vc_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A\\/B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_sll_vv, xxx);
      BEGD(d0);
      BEGD(d1);
      d0 = XREG(PREG->u.xxx.x1);
      /* first check pt1 */
      deref_head(d0, sll_vv_unk);
    sll_vv_nvar:
      d1 = XREG(PREG->u.xxx.x2);
      /* next check A2 */
      deref_head(d1, sll_vv_nvar_unk);
    sll_vv_nvar_nvar:
      /* d0 and d1 are where I want them */
      if (IsIntTerm(d0) && IsIntTerm(d1)) {
	Int i2 = IntOfTerm(d1);
	if (i2 < 0)
	  d0 = MkIntegerTerm(IntOfTerm(d0) >> -i2);
	else
	  d0 = do_sll(IntOfTerm(d0),i2);
      }
      else {
	saveregs();
	d0 = p_sll(Yap_Eval(d0), Yap_Eval(d1));
	setregs();
      }
      if (d0 == 0L) {
	saveregs();
	Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	setregs();
	FAIL();
      }
      XREG(PREG->u.xxx.x) = d0;
      PREG = NEXTOP(PREG, xxx);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, sll_vv_unk, sll_vv_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A<<B");
      setregs();
      FAIL();
      ENDP(pt0);

      BEGP(pt0);
      deref_body(d1, pt0, sll_vv_nvar_unk, sll_vv_nvar_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A<<B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(p_sll_vc, xxn);
      BEGD(d0);
      d0 = XREG(PREG->u.xxn.xi);
      /* first check pt1 */
      deref_head(d0, sll_vc_unk);
    sll_vc_nvar:
      {
	Int d1 = PREG->u.xxn.c;
	if (IsIntTerm(d0)) {
	  d0 = do_sll(IntOfTerm(d0), (Int)d1);
	}
	else {
	  saveregs();
	  d0 = p_sll(Yap_Eval(d0), MkIntegerTerm(d1));
	  setregs();
	}
      }
      if (d0 == 0L) {
	saveregs();
	Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	setregs();
	FAIL();
      }
      XREG(PREG->u.xxn.x) = d0;
      PREG = NEXTOP(PREG, xxn);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, sll_vc_unk, sll_vc_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A<<B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_sll_cv, xxn);
      BEGD(d0);
      d0 = XREG(PREG->u.xxn.xi);
      /* first check pt1 */
      deref_head(d0, sll_cv_unk);
    sll_cv_nvar:
      {
	Int d1 = PREG->u.xxn.c;
	if (IsIntTerm(d0)) {
	  Int i2 = IntOfTerm(d0);
	  if (i2 < 0)
	    d0 = MkIntegerTerm(d1 >> -i2);
	  else
	    d0 = do_sll(d1,i2);
	}
	else {
	  saveregs();
	  d0 = p_sll(MkIntegerTerm(d1), Yap_Eval(d0));
	  setregs();
	}
      }
      if (d0 == 0L) {
	saveregs();
	Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	setregs();
	FAIL();
      }
      XREG(PREG->u.xxn.x) = d0;
      PREG = NEXTOP(PREG, xxn);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, sll_cv_unk, sll_cv_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A<<B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_sll_y_vv, yxx);
      BEGD(d0);
      BEGD(d1);
      d0 = XREG(PREG->u.yxx.x1);
      /* first check pt1 */
      deref_head(d0, sll_y_vv_unk);
    sll_y_vv_nvar:
      d1 = XREG(PREG->u.xxx.x2);
      /* next check A2 */
      deref_head(d1, sll_y_vv_nvar_unk);
    sll_y_vv_nvar_nvar:
      /* d0 and d1 are where I want them */
      if (IsIntTerm(d0) && IsIntTerm(d1)) {
	Int i2 = IntOfTerm(d1);
	if (i2 < 0)
	  d0 = MkIntegerTerm(IntOfTerm(d0) >> -i2);
	else
	  d0 = do_sll(IntOfTerm(d0),i2);
      }
      else {
	saveregs();
	d0 = p_sll(Yap_Eval(d0), Yap_Eval(d1));
	setregs();
      }
      if (d0 == 0L) {
	saveregs();
	Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	setregs();
	FAIL();
      }
      BEGP(pt0);
      pt0 = YREG + PREG->u.yxx.y;
      PREG = NEXTOP(PREG, yxx);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(pt0,d0);
#else
      *pt0 = d0;
#endif /* SBA && FROZEN_STACKS */
      ENDP(pt0);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, sll_y_vv_unk, sll_y_vv_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A<<B");
      setregs();
      FAIL();
      ENDP(pt0);

      BEGP(pt0);
      deref_body(d1, pt0, sll_y_vv_nvar_unk, sll_y_vv_nvar_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A<<B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(p_sll_y_vc, yxn);
      BEGD(d0);
      d0 = XREG(PREG->u.yxn.xi);
      /* first check pt1 */
      deref_head(d0, sll_y_vc_unk);
    sll_y_vc_nvar:
      {
	Int d1 = PREG->u.yxn.c;
	if (IsIntTerm(d0)) {
	  d0 = do_sll(IntOfTerm(d0), Yap_Eval(d1));
	}
	else {
	  saveregs();
	  d0 = p_sll(Yap_Eval(d0), MkIntegerTerm(d1));
	  setregs();
	}
      }
      if (d0 == 0L) {
	saveregs();
	Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	setregs();
	FAIL();
      }
      BEGP(pt0);
      pt0 = YREG + PREG->u.yxn.y;
      PREG = NEXTOP(PREG, yxn);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(pt0,d0);
#else
      *pt0 = d0;
#endif /* SBA && FROZEN_STACKS */
      ENDP(pt0);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, sll_y_vc_unk, sll_y_vc_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A<<B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();


      Op(p_sll_y_cv, yxn);
      BEGD(d0);
      d0 = XREG(PREG->u.yxn.xi);
      /* first check pt1 */
      deref_head(d0, sll_y_cv_unk);
    sll_y_cv_nvar:
      {
	Int d1 = PREG->u.yxn.c;
	if (IsIntTerm(d0)) {
	  Int i2 = IntOfTerm(d0);
	  if (i2 < 0)
	    d0 = MkIntegerTerm(d1 >> -i2);
	  else
	    d0 = do_sll(d1,i2);
	}
	else {
	  saveregs();
	  d0 = p_sll(MkIntegerTerm(d1), Yap_Eval(0));
	  setregs();
	}
      }
      if (d0 == 0L) {
	saveregs();
	Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	setregs();
	FAIL();
      }
      BEGP(pt0);
      pt0 = YREG + PREG->u.yxn.y;
      PREG = NEXTOP(PREG, yxn);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(pt0,d0);
#else
      *pt0 = d0;
#endif /* SBA && FROZEN_STACKS */
      ENDP(pt0);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, sll_y_cv_unk, sll_y_cv_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A<<B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_slr_vv, xxx);
      BEGD(d0);
      BEGD(d1);
      d0 = XREG(PREG->u.xxx.x1);
      /* first check pt1 */
      deref_head(d0, slr_vv_unk);
    slr_vv_nvar:
      d1 = XREG(PREG->u.xxx.x2);
      /* next check A2 */
      deref_head(d1, slr_vv_nvar_unk);
    slr_vv_nvar_nvar:
      /* d0 and d1 are where I want them */
      if (IsIntTerm(d0) && IsIntTerm(d1)) {
	Int i2 = IntOfTerm(d1);
	if (i2 < 0)
	  d0 = do_sll(IntOfTerm(d0), -i2);
	else
	  d0 = MkIntTerm(IntOfTerm(d0) >> i2);
      }
      else {
	saveregs();
	d0 = p_slr(Yap_Eval(d0), Yap_Eval(d1));
	setregs();
      }
      if (d0 == 0L) {
	saveregs();
	Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	setregs();
	FAIL();
      }
      XREG(PREG->u.xxx.x) = d0;
      PREG = NEXTOP(PREG, xxx);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, slr_vv_unk, slr_vv_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A>>B");
      setregs();
      FAIL();
      ENDP(pt0);

      BEGP(pt0);
      deref_body(d1, pt0, slr_vv_nvar_unk, slr_vv_nvar_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A>>B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(p_slr_vc, xxn);
      BEGD(d0);
      d0 = XREG(PREG->u.xxn.xi);
      /* first check pt1 */
      deref_head(d0, slr_vc_unk);
    slr_vc_nvar:
      {
	Int d1 = PREG->u.xxn.c;
	if (IsIntTerm(d0)) {
	  d0 = MkIntTerm(IntOfTerm(d0) >> d1);
	}
	else {
	  saveregs();
	  d0 = p_slr(Yap_Eval(d0), MkIntegerTerm(d1));
	  setregs();
	  if (d0 == 0L) {
	    saveregs();
	    Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	    setregs();
	    FAIL();
	  }
	}
      }
      XREG(PREG->u.xxn.x) = d0;
      PREG = NEXTOP(PREG, xxn);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, slr_vc_unk, slr_vc_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A>>B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_slr_cv, xxn);
      BEGD(d0);
      d0 = XREG(PREG->u.xxn.xi);
      /* first check pt1 */
      deref_head(d0, slr_cv_unk);
    slr_cv_nvar:
      {
	Int d1 = PREG->u.xxn.c;
	if (IsIntTerm(d0)) {
	 Int i2 = IntOfTerm(d0);
	 if (i2 < 0)
	   d0 = do_sll(d1, -i2);
	 else
	   d0 = MkIntegerTerm(d1 >> i2);
	}
	else {
	  saveregs();
	  d0 = p_slr(MkIntegerTerm(d1), Yap_Eval(d0));
	  setregs();
	}
      }
      if (d0 == 0L) {
	saveregs();
	Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	setregs();
	FAIL();
      }
      XREG(PREG->u.xxn.x) = d0;
      PREG = NEXTOP(PREG, xxn);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, slr_cv_unk, slr_cv_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A>>B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_slr_y_vv, yxx);
      BEGD(d0);
      BEGD(d1);
      d0 = XREG(PREG->u.yxx.x1);
      /* first check pt1 */
      deref_head(d0, slr_y_vv_unk);
    slr_y_vv_nvar:
      d1 = XREG(PREG->u.xxx.x2);
      /* next check A2 */
      deref_head(d1, slr_y_vv_nvar_unk);
    slr_y_vv_nvar_nvar:
      /* d0 and d1 are where I want them */
      if (IsIntTerm(d0) && IsIntTerm(d1)) {
	 Int i2 = IntOfTerm(d1);
	 if (i2 < 0)
	   d0 = do_sll(IntOfTerm(d0), -i2);
	 else
	   d0 = MkIntTerm(IntOfTerm(d0) >> i2);
      }
      else {
	saveregs();
	d0 = p_slr(Yap_Eval(d0), Yap_Eval(d1));
	setregs();
      }
      BEGP(pt0);
      if (d0 == 0L) {
	saveregs();
	Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	setregs();
	FAIL();
      }
      pt0 = YREG + PREG->u.yxx.y;
      PREG = NEXTOP(PREG, yxx);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(pt0,d0);
#else
      *pt0 = d0;
#endif /* SBA && FROZEN_STACKS */
      ENDP(pt0);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, slr_y_vv_unk, slr_y_vv_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A>>B");
      setregs();
      FAIL();
      ENDP(pt0);

      BEGP(pt0);
      deref_body(d1, pt0, slr_y_vv_nvar_unk, slr_y_vv_nvar_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A>>B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(p_slr_y_vc, yxn);
      BEGD(d0);
      d0 = XREG(PREG->u.yxn.xi);
      /* first check pt1 */
      deref_head(d0, slr_y_vc_unk);
    slr_y_vc_nvar:
      {
	Int d1 = PREG->u.yxn.c;
	if (IsIntTerm(d0)) {
	  d0 = MkIntTerm(IntOfTerm(d0) >> d1);
	}
	else {
	  saveregs();
	  d0 = p_slr(Yap_Eval(d0), MkIntegerTerm(d1));
	  setregs();
	  if (d0 == 0L) {
	    saveregs();
	    Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	    setregs();
	    FAIL();
	  }
	}
      }
      BEGP(pt0);
      pt0 = YREG + PREG->u.yxn.y;
      PREG = NEXTOP(PREG, yxn);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(pt0,d0);
#else
      *pt0 = d0;
#endif /* SBA && FROZEN_STACKS */
      ENDP(pt0);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, slr_y_vc_unk, slr_y_vc_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A>>B");
      setregs();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      Op(p_slr_y_cv, yxn);
      BEGD(d0);
      d0 = XREG(PREG->u.yxn.xi);
      /* first check pt1 */
      deref_head(d0, slr_y_cv_unk);
    slr_y_cv_nvar:
      {
	Int d1 = PREG->u.yxn.c;
	if (IsIntTerm(d0)) {
	 Int i2 = IntOfTerm(d0);
	 if (i2 < 0)
	   d0 = do_sll(d1, -i2);
	 else
	   d0 = MkIntegerTerm(d1 >> i2);
	}
	else {
	  saveregs();
	  d0 = p_slr(MkIntegerTerm(d1), Yap_Eval(d0));
	  setregs();
	}
      }
      if (d0 == 0L) {
	saveregs();
	Yap_Error(Yap_Error_TYPE, Yap_Error_Term, Yap_ErrorMessage);
	setregs();
	FAIL();
      }
      BEGP(pt0);
      pt0 = YREG + PREG->u.yxn.y;
      PREG = NEXTOP(PREG, yxn);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(pt0,d0);
#else
      *pt0 = d0;
#endif /* SBA && FROZEN_STACKS */
      ENDP(pt0);
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, slr_y_cv_unk, slr_y_cv_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, TermNil, "X is A>>B");
      setregs();
      FAIL();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      BOp(call_bfunc_xx, plxxs);
      BEGD(d0);
      BEGD(d1);
      d0 = XREG(PREG->u.plxxs.x1);
    call_bfunc_xx_nvar:
      d1 = XREG(PREG->u.plxxs.x2);
    call_bfunc_xx2_nvar:
      deref_head(d0, call_bfunc_xx_unk);
      deref_head(d1, call_bfunc_xx2_unk);
      if (IsIntTerm(d0) && IsIntTerm(d1)) {
	COUNT flags;

	Int v = IntOfTerm(d0) - IntOfTerm(d1);
	flags = PREG->u.plxxs.flags;
	if (v > 0) {
	  if (flags & GT_OK_IN_CMP) {
	    yamop *nextp = NEXTOP(PREG, plxxs);
	    ALWAYS_LOOKAHEAD(nextp->opc);
	    PREG = nextp;
	    ALWAYS_GONext();
	    ALWAYS_END_PREFETCH();
	  } else {
	    yamop *nextp = PREG->u.plxxs.f;
	    ALWAYS_LOOKAHEAD(nextp->opc);
	    PREG = nextp;
	    ALWAYS_GONext();
	    ALWAYS_END_PREFETCH();
	  }    
	} else if (v < 0) {
	  if (flags & LT_OK_IN_CMP) {
	    yamop *nextp = NEXTOP(PREG, plxxs);
	    ALWAYS_LOOKAHEAD(nextp->opc);
	    PREG = nextp;
	    ALWAYS_GONext();
	    ALWAYS_END_PREFETCH();
	  } else {
	    yamop *nextp = PREG->u.plxxs.f;
	    ALWAYS_LOOKAHEAD(nextp->opc);
	    PREG = nextp;
	    ALWAYS_GONext();
	    ALWAYS_END_PREFETCH();
	  }
	} else /* if (v == 0) */ {
	  if (flags & EQ_OK_IN_CMP) {
	    yamop *nextp = NEXTOP(PREG, plxxs);
	    ALWAYS_LOOKAHEAD(nextp->opc);
	    PREG = nextp;
	    ALWAYS_GONext();
	    ALWAYS_END_PREFETCH();
	  } else {
	    yamop *nextp = PREG->u.plxxs.f;
	    ALWAYS_LOOKAHEAD(nextp->opc);
	    PREG = nextp;
	    ALWAYS_GONext();
	    ALWAYS_END_PREFETCH();
	  }
	}
      } 
    exec_bin_cmp_xx:
      {
	 CmpPredicate f = PREG->u.plxxs.p->cs.d_code;
	 saveregs();
	 d0 = (CELL) (f) (d0,d1);
	 setregs();
      }
      if (PREG == FAILCODE) {
	JMPNext();
      }
      if (!d0) {
	  PREG = PREG->u.plxxs.f;
	JMPNext();
      }
      PREG = NEXTOP(PREG, plxxs);
      JMPNext();

      BEGP(pt0);
      deref_body(d0, pt0, call_bfunc_xx_unk, call_bfunc_xx_nvar);
      d1 = Deref(d1);
      goto exec_bin_cmp_xx;
      ENDP(pt0);

      BEGP(pt0);
      deref_body(d1, pt0, call_bfunc_xx2_unk, call_bfunc_xx2_nvar);
      goto exec_bin_cmp_xx;
      ENDP(pt0);

      ENDD(d1);
      ENDD(d0);
      ENDBOp();

      BOp(call_bfunc_yx, plxys);
      BEGD(d0);
      BEGD(d1);
      BEGP(pt0);
      pt0 = YREG + PREG->u.plxys.y;
      d1 = XREG(PREG->u.plxys.x);
      d0 = *pt0;
      ENDP(pt0);
      deref_head(d0, call_bfunc_yx_unk);
    call_bfunc_yx_nvar:
      deref_head(d1, call_bfunc_yx2_unk);
    call_bfunc_yx2_nvar:
      if (IsIntTerm(d0) && IsIntTerm(d1)) {
	int flags;

	Int v = IntOfTerm(d0) - IntOfTerm(d1);
	flags = PREG->u.plxys.flags;
	if (v > 0) {
	  if (flags & GT_OK_IN_CMP) {
	    PREG = NEXTOP(PREG, plxys);
	    JMPNext();
	  } else {
	    PREG = PREG->u.plxys.f;
	    JMPNext();
	  }
	} else if (v < 0) {
	  if (flags & LT_OK_IN_CMP) {
	    PREG = NEXTOP(PREG, plxys);
	    JMPNext();
	  } else {
	    PREG = PREG->u.plxys.f;
	    JMPNext();
	  }
	} else /* if (v == 0) */ {
	  if (flags & EQ_OK_IN_CMP) {
	    PREG = NEXTOP(PREG, plxys);
	    JMPNext();
	  } else {
	    PREG = PREG->u.plxys.f;
	    JMPNext();
	  }
	}
      } 
    exec_bin_cmp_yx:
      {
	CmpPredicate f = PREG->u.plxys.p->cs.d_code;
	saveregs();
	d0 = (CELL) (f) (d0,d1);
	setregs();
      }
      if (!d0 || PREG == FAILCODE) {
	if (PREG != FAILCODE)
	  PREG = PREG->u.plxys.f;
	JMPNext();
      }
      PREG = NEXTOP(PREG, plxys);
      JMPNext();

      BEGP(pt0);
      deref_body(d0, pt0, call_bfunc_yx_unk, call_bfunc_yx_nvar);
      d1 = Deref(d1);
      goto exec_bin_cmp_yx;
      ENDP(pt0);

      BEGP(pt0);
      deref_body(d1, pt0, call_bfunc_yx2_unk, call_bfunc_yx2_nvar);
      goto exec_bin_cmp_yx;
      ENDP(pt0);

      ENDD(d1);
      ENDD(d0);
      ENDBOp();

      BOp(call_bfunc_xy, plxys);
      BEGD(d0);
      BEGD(d1);
      BEGP(pt0);
      pt0 = YREG + PREG->u.plxys.y;
      d0 = XREG(PREG->u.plxys.x);
      d1 = *pt0;
      ENDP(pt0);
      deref_head(d0, call_bfunc_xy_unk);
    call_bfunc_xy_nvar:
      deref_head(d1, call_bfunc_xy2_unk);
    call_bfunc_xy2_nvar:
      if (IsIntTerm(d0) && IsIntTerm(d1)) {
	int flags;

	Int v = IntOfTerm(d0) - IntOfTerm(d1);
	flags = PREG->u.plxys.flags;
	if (v > 0) {
	  if (flags & GT_OK_IN_CMP) {
	    PREG = NEXTOP(PREG, plxys);
	    JMPNext();
	  } else {
	    PREG = PREG->u.plxys.f;
	    JMPNext();
	  }
	} else if (v < 0) {
	  if (flags & LT_OK_IN_CMP) {
	    PREG = NEXTOP(PREG, plxys);
	    JMPNext();
	  } else {
	    PREG = PREG->u.plxys.f;
	    JMPNext();
	  }
	} else /* if (v == 0) */ {
	  if (flags & EQ_OK_IN_CMP) {
	    PREG = NEXTOP(PREG, plxys);
	    JMPNext();
	  } else {
	    PREG = PREG->u.plxys.f;
	    JMPNext();
	  }
	}
      } 
    exec_bin_cmp_xy:
      {
	CmpPredicate f = PREG->u.plxys.p->cs.d_code;
	saveregs();
	d0 = (CELL) (f) (d0,d1);
	setregs();
      }
      if (!d0 || PREG == FAILCODE) {
	if (PREG != FAILCODE)
	  PREG = PREG->u.plxys.f;
	JMPNext();
      }
      PREG = NEXTOP(PREG, plxys);
      JMPNext();

      BEGP(pt0);
      deref_body(d0, pt0, call_bfunc_xy_unk, call_bfunc_xy_nvar);
      d1 = Deref(d1);
      goto exec_bin_cmp_xy;
      ENDP(pt0);

      BEGP(pt0);
      deref_body(d1, pt0, call_bfunc_xy2_unk, call_bfunc_xy2_nvar);
      goto exec_bin_cmp_xy;
      ENDP(pt0);

      ENDD(d1);
      ENDD(d0);
      ENDBOp();

      BOp(call_bfunc_yy, plyys);
      BEGD(d0);
      BEGD(d1);
      BEGP(pt0);
      pt0 = YREG + PREG->u.plyys.y1;
      BEGP(pt1);
      pt1 = YREG + PREG->u.plyys.y2;
      d0 = *pt0;
      d1 = *pt1;
      ENDP(pt1);
      ENDP(pt0);
      deref_head(d0, call_bfunc_yy_unk);
    call_bfunc_yy_nvar:
      deref_head(d1, call_bfunc_yy2_unk);
    call_bfunc_yy2_nvar:
      if (IsIntTerm(d0) && IsIntTerm(d1)) {
	int flags;

	Int v = IntOfTerm(d0) - IntOfTerm(d1);
	flags = PREG->u.plyys.flags;
	if (v > 0) {
	  if (flags & GT_OK_IN_CMP) {
	    PREG = NEXTOP(PREG, plyys);
	    JMPNext();
	  } else {
	    PREG = PREG->u.plyys.f;
	    JMPNext();
	  }
	} else if (v < 0) {
	  if (flags & LT_OK_IN_CMP) {
	    PREG = NEXTOP(PREG, plyys);
	    JMPNext();
	  } else {
	    PREG = PREG->u.plyys.f;
	    JMPNext();
	  }
	} else /* if (v == 0) */ {
	  if (flags & EQ_OK_IN_CMP) {
	    PREG = NEXTOP(PREG, plyys);
	    JMPNext();
	  } else {
	    PREG = PREG->u.plyys.f;
	    JMPNext();
	  }
	}
      } 
    exec_bin_cmp_yy:
      {
	CmpPredicate f = PREG->u.plyys.p->cs.d_code;
	saveregs();
	d0 = (CELL) (f) (d0,d1);
	setregs();
      }
      if (!d0 || PREG == FAILCODE) {
	if (PREG != FAILCODE)
	  PREG = PREG->u.plyys.f;
	JMPNext();
      }
      PREG = NEXTOP(PREG, plyys);
      JMPNext();

      BEGP(pt0);
      deref_body(d0, pt0, call_bfunc_yy_unk, call_bfunc_yy_nvar);
      d1 = Deref(d1);
      goto exec_bin_cmp_yy;
      ENDP(pt0);

      BEGP(pt0);
      deref_body(d1, pt0, call_bfunc_yy2_unk, call_bfunc_yy2_nvar);
      goto exec_bin_cmp_yy;
      ENDP(pt0);

      ENDD(d1);
      ENDD(d0);
      ENDBOp();

      Op(p_equal, e);
      save_hb();
      if (Yap_IUnify(ARG1, ARG2) == FALSE) {
	FAIL();
      }
      PREG = NEXTOP(PREG, e);
      GONext();
      ENDOp();

      Op(p_dif, l);
#ifdef LOW_LEVEL_TRACER
      if (Yap_do_low_level_trace)
	low_level_trace(enter_pred,RepPredProp(Yap_GetPredPropByFunc(FunctorDiff,0)),XREGS+1);
#endif	/* LOW_LEVEL_TRACE */
      BEGD(d0);
      BEGD(d1);
      d0 = ARG1;
      deref_head(d0, dif_unk1);
    dif_nvar1:
      /* first argument is bound */
      d1 = ARG2;
      deref_head(d1, dif_nvar1_unk2);
    dif_nvar1_nvar2:
      /* both arguments are bound */
      if (d0 == d1) {
	PREG = PREG->u.l.l;
	GONext();
      }
      if (IsAtomOrIntTerm(d0) || IsAtomOrIntTerm(d1)) {
	PREG = NEXTOP(PREG, l);
	GONext();
      }
      {
	Int opresult;
#ifdef COROUTINING
	/*
	 * We may wake up goals during our attempt to unify the
	 * two terms. If we are adding to the tail of a list of
	 * woken goals that should be ok, but otherwise we need
	 * to restore WokenGoals to its previous value.
	 */
	CELL OldWokenGoals = Yap_ReadTimedVar(WokenGoals);

#endif
	/* We will have to look inside compound terms */
	register tr_fr_ptr pt0;
	/* store the old value of TR for clearing bindings */
	pt0 = TR;
	BEGCHO(pt1);
	pt1 = B;
	/* make B and HB point to H to guarantee all bindings will
	 * be trailed
	 */
	HBREG = H;
	B = (choiceptr) H;
	B->cp_h = H;
	SET_BB(B);
	save_hb();
	opresult = Yap_IUnify(d0, d1);
#ifdef COROUTINING
	/* now restore Woken Goals to its old value */
	Yap_UpdateTimedVar(WokenGoals, OldWokenGoals);
	if (OldWokenGoals == TermNil) {
	  Yap_undo_signal(YAP_WAKEUP_SIGNAL);
	}
#endif
	/* restore B */
	B = pt1;
	SET_BB(PROTECT_FROZEN_B(pt1));
#ifdef COROUTINING
	H = HBREG;
#endif
	HBREG = B->cp_h;
	/* untrail all bindings made by Yap_IUnify */
	while (TR != pt0) {
	  BEGD(d1);
	  d1 = TrailTerm(--TR);
	  if (IsVarTerm(d1)) {
#if defined(SBA) && defined(YAPOR)
	    /* clean up the trail when we backtrack */
	    if (Unsigned((Int)(d1)-(Int)(H_FZ)) >
		Unsigned((Int)(B_FZ)-(Int)(H_FZ))) {
	      RESET_VARIABLE(STACK_TO_SBA(d1));
	    } else
#endif
	      /* normal variable */
	      RESET_VARIABLE(d1);
#ifdef MULTI_ASSIGNMENT_VARIABLES
	  } else /* if (IsApplTerm(d1)) */ {
	    CELL *pt = RepAppl(d1);
	    /* AbsAppl means */
	    /* multi-assignment variable */
	    /* so the next cell is the old value */ 
#ifdef FROZEN_STACKS
	    pt[0] = TrailVal(--TR);
#else
	    pt[0] = TrailTerm(--TR);
	    TR--;
#endif /* FROZEN_STACKS */
#endif /* MULTI_ASSIGNMENT_VARIABLES */
	  }
	  ENDD(d1);
	}
	if (opresult) {
	  /* restore B, no need to restore HB */
	  PREG = PREG->u.l.l;
	  GONext();
	}
	/* restore B, and later HB */
	PREG = NEXTOP(PREG, l);
	ENDCHO(pt1);
      }
      GONext();

      BEGP(pt0);
      deref_body(d0, pt0, dif_unk1, dif_nvar1);
      ENDP(pt0);
      /* first argument is unbound */
      PREG = PREG->u.l.l;
      GONext();

      BEGP(pt0);
      deref_body(d1, pt0, dif_nvar1_unk2, dif_nvar1_nvar2);
      ENDP(pt0);
      /* second argument is unbound */
      PREG = PREG->u.l.l;
      GONext();
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(p_eq, l);
#ifdef LOW_LEVEL_TRACER
      if (Yap_do_low_level_trace)
	low_level_trace(enter_pred,RepPredProp(Yap_GetPredPropByFunc(FunctorSame,0)),XREGS+1);
#endif	/* LOW_LEVEL_TRACE */
      BEGD(d0);
      BEGD(d1);
      d0 = ARG1;
      deref_head(d0, p_eq_unk1);
    p_eq_nvar1:
      /* first argument is bound */
      d1 = ARG2;
      deref_head(d1, p_eq_nvar1_unk2);
    p_eq_nvar1_nvar2:
      /* both arguments are bound */
      if (d0 == d1) {
	PREG = NEXTOP(PREG, l);
	GONext();
      }
      if (IsPairTerm(d0)) {
	if (!IsPairTerm(d1)) {
	  PREG = PREG->u.l.l;
	  GONext();
	}
	BEGD(d2);
	always_save_pc();
	d2 = iequ_complex(RepPair(d0)-1, RepPair(d0)+1,RepPair(d1)-1);
	if (d2 == FALSE) {
	  PREG = PREG->u.l.l;
	  GONext();
	}
	ENDD(d2);
	always_set_pc();
	PREG = NEXTOP(PREG, l);
	GONext();
      }
      if (IsApplTerm(d0)) {
	Functor f0 = FunctorOfTerm(d0);
	Functor f1;

	/* f1 must be a compound term, even if it is a suspension */
	if (!IsApplTerm(d1)) {
	  PREG = PREG->u.l.l;
	  GONext();
	}
	f1 = FunctorOfTerm(d1);

	/* we now know f1 is true */
	/* deref if a compound term */
	if (IsExtensionFunctor(f0)) {
	  switch ((CELL)f0) {
	  case (CELL)FunctorDBRef:
	    if (d0 == d1) {
	      PREG = NEXTOP(PREG, l);
	      GONext();
	    }
	    PREG = PREG->u.l.l;
	    GONext();
	  case (CELL)FunctorLongInt:
	    if (f1 != FunctorLongInt) {
	      PREG = PREG->u.l.l;
	      GONext();
	    }
	    if (LongIntOfTerm(d0) == LongIntOfTerm(d1)) {
	      PREG = NEXTOP(PREG, l);
	      GONext();
	    }
	    PREG = PREG->u.l.l;
	    GONext();
#ifdef USE_GMP
	  case (CELL)FunctorBigInt:
	    if (f1 != FunctorBigInt) {
	      PREG = PREG->u.l.l;
	      GONext();
	    }
	    if (mpz_cmp(Yap_BigIntOfTerm(d0), Yap_BigIntOfTerm(d1)) == 0) {
	      PREG = NEXTOP(PREG, l);
	      GONext();
	    }
	    PREG = PREG->u.l.l;
	    GONext();
#endif
	  case (CELL)FunctorDouble:
	    if (f1 != FunctorDouble) {
	      PREG = PREG->u.l.l;
	      GONext();
	    }
	    if (FloatOfTerm(d0) == FloatOfTerm(d1)) {
	      PREG = NEXTOP(PREG, l);
	      GONext();
	    }
	  default:
	    PREG = PREG->u.l.l;
	    GONext();
	  }
	}
	if (f0 != f1) {
	  PREG = PREG->u.l.l;
	  GONext();
	}
	always_save_pc();
	BEGD(d2);
	d2 = iequ_complex(RepAppl(d0), RepAppl(d0)+ArityOfFunctor(f0), RepAppl(d1));
	if (d2 == FALSE) {
	  PREG = PREG->u.l.l;
	  GONext();
	}
	ENDD(d2);
	always_set_pc();
	PREG = NEXTOP(PREG, l);
	GONext();
      }
      PREG = PREG->u.l.l;
      GONext();

      BEGP(pt0);
      deref_body(d1, pt0, p_eq_nvar1_unk2, p_eq_nvar1_nvar2);
      ENDP(pt0);
      /* first argument is bound */
      /* second argument is unbound */
      /* I don't need to worry about co-routining because an
	 unbound variable may never be == to a constrained variable!! */
      PREG = PREG->u.l.l;
      GONext();
      ENDD(d1);

      BEGP(pt0);
      deref_body(d0, pt0, p_eq_unk1, p_eq_nvar1);
      BEGD(d1);
      d1 = ARG2;
      deref_head(d1, p_eq_var1_unk2);
    p_eq_var1_nvar2:
      /* I don't need to worry about co-routining because an
	 unbound variable may never be == to a constrained variable!! */
      PREG = PREG->u.l.l;
      GONext();

      BEGP(pt1);
      deref_body(d1, pt1, p_eq_var1_unk2, p_eq_var1_nvar2);
      /* first argument is unbound */
      /* second argument is unbound */
      if (pt1 != pt0) {
	PREG = PREG->u.l.l;
	GONext();
      }
      PREG = NEXTOP(PREG, l);
      GONext();      
      ENDP(pt1);
      ENDD(d1);
      ENDP(pt0);

      ENDD(d0);
      ENDOp();

      Op(p_arg_vv, xxx);
#ifdef LOW_LEVEL_TRACER
      if (Yap_do_low_level_trace) {
	H[0] = XREG(PREG->u.xxx.x1);
	H[1] = XREG(PREG->u.xxx.x2);
	RESET_VARIABLE(H+2);
	low_level_trace(enter_pred,RepPredProp(Yap_GetPredPropByFunc(FunctorArg,0)),H);
      }
#endif	/* LOW_LEVEL_TRACE */
      BEGD(d0);
      d0 = XREG(PREG->u.xxx.x1);
      deref_head(d0, arg_arg1_unk);
    arg_arg1_nvar:
      /* ARG1 is ok! */
      if (IsIntTerm(d0))
	d0 = IntOfTerm(d0);
      else if (IsLongIntTerm(d0)) {
	d0 = LongIntOfTerm(d0);
      } else {
	saveregs();
	Yap_Error(TYPE_ERROR_INTEGER,d0,"arg 1 of arg/3");
	setregs();
	FAIL();
      }

      /* d0 now got the argument we want */
      BEGD(d1);
      d1 = XREG(PREG->u.xxx.x2);
      deref_head(d1, arg_arg2_unk);
    arg_arg2_nvar:
      /* d1 now got the structure we want to fetch the argument
       * from */
      if (IsApplTerm(d1)) {
	BEGP(pt0);
	pt0 = RepAppl(d1);
	d1 = *pt0;
	if (IsExtensionFunctor((Functor) d1)) {
	  FAIL();
	}
	if ((Int)d0 <= 0 ||
	    (Int)d0 > ArityOfFunctor((Functor) d1)) {
	  /* don't complain here for Prolog compatibility 
	  if ((Int)d0 <= 0) {
	    saveregs();
	    Yap_Error(DOMAIN_ERROR_NOT_LESS_THAN_ZERO,
		  MkIntegerTerm(d0),"arg 1 of arg/3");	    
	    setregs();
	  }
	  */
	  FAIL();
	}
	XREG(PREG->u.xxx.x) = pt0[d0];
	PREG = NEXTOP(PREG, xxx);
	GONext();
	ENDP(pt0);
      }
      else if (IsPairTerm(d1)) {
	BEGP(pt0);
	pt0 = RepPair(d1);
	if (d0 != 1 && d0 != 2) {
	  if ((Int)d0 < 0) {
	    saveregs();
	    Yap_Error(DOMAIN_ERROR_NOT_LESS_THAN_ZERO,
		  MkIntegerTerm(d0),"arg 1 of arg/3");
	    setregs();
	  }
	  FAIL();
	}
	XREG(PREG->u.xxx.x) = pt0[d0-1];
	PREG = NEXTOP(PREG, xxx);
	GONext();
	ENDP(pt0);
      }
      else {
	saveregs();
	Yap_Error(TYPE_ERROR_COMPOUND, d1, "arg 2 of arg/3");
	setregs();
	FAIL();
      }

      BEGP(pt0);
      deref_body(d1, pt0, arg_arg2_unk, arg_arg2_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, d1,"arg 2 of arg/3");;
      setregs();
      ENDP(pt0);
      FAIL();
      ENDD(d1);

      BEGP(pt0);
      deref_body(d0, pt0, arg_arg1_unk, arg_arg1_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, d0, "arg 1 of arg/3");;
      setregs();
      ENDP(pt0);
      FAIL();
      ENDD(d0);
      ENDOp();

      Op(p_arg_cv, xxn);
#ifdef LOW_LEVEL_TRACER
      if (Yap_do_low_level_trace) {
	CELL *Ho = H;
	Term t = MkIntegerTerm(PREG->u.xxn.c); 
	H[0] =  t;
	H[1] = XREG(PREG->u.xxn.xi);
	RESET_VARIABLE(H+2);
	low_level_trace(enter_pred,RepPredProp(Yap_GetPredPropByFunc(FunctorArg,0)),H);
	H = Ho;
      }
#endif	/* LOW_LEVEL_TRACE */
      BEGD(d0);
      d0 = PREG->u.xxn.c;
      /* d0 now got the argument we want */
      BEGD(d1);
      d1 = XREG(PREG->u.xxn.xi);
      deref_head(d1, arg_arg2_vc_unk);
    arg_arg2_vc_nvar:
      /* d1 now got the structure we want to fetch the argument
       * from */
      if (IsApplTerm(d1)) {
	BEGP(pt0);
	pt0 = RepAppl(d1);
	d1 = *pt0;
	if (IsExtensionFunctor((Functor) d1)) {
	  FAIL();
	}
	if ((Int)d0 <= 0 ||
	    (Int)d0 > ArityOfFunctor((Functor) d1)) {
	  /* don't complain here for Prolog compatibility 
	  if ((Int)d0 <= 0) {
	    saveregs();
	    Yap_Error(DOMAIN_ERROR_NOT_LESS_THAN_ZERO,
		  MkIntegerTerm(d0),"arg 1 of arg/3");	    
	    setregs();
	  }
	  */
	  FAIL();
	}
	XREG(PREG->u.xxn.x) = pt0[d0];
	PREG = NEXTOP(PREG, xxn);
	GONext();
	ENDP(pt0);
      }
      else if (IsPairTerm(d1)) {
	BEGP(pt0);
	pt0 = RepPair(d1);
	if (d0 != 1 && d0 != 2) {
	  if ((Int)d0 < 0) {
	    saveregs();
	    Yap_Error(DOMAIN_ERROR_NOT_LESS_THAN_ZERO,
		  MkIntegerTerm(d0),"arg 1 of arg/3");
	    setregs();
	  }
	  FAIL();
	}
	XREG(PREG->u.xxn.x) = pt0[d0-1];
	PREG = NEXTOP(PREG, xxn);
	GONext();
	ENDP(pt0);
      }
      else {
	saveregs();
	Yap_Error(TYPE_ERROR_COMPOUND, d1, "arg 2 of arg/3");
	setregs();
	FAIL();
      }

      BEGP(pt0);
      deref_body(d1, pt0, arg_arg2_vc_unk, arg_arg2_vc_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, d1,"arg 2 of arg/3");;
      setregs();
      ENDP(pt0);
      FAIL();
      ENDD(d1);

      ENDD(d0);
      ENDOp();

      Op(p_arg_y_vv, yxx);
#ifdef LOW_LEVEL_TRACER
      if (Yap_do_low_level_trace) {
	H[0] = XREG(PREG->u.yxx.x1);
	H[1] = XREG(PREG->u.yxx.x2);
	H[2] = YREG[PREG->u.yxx.y];
	RESET_VARIABLE(H+2);
	low_level_trace(enter_pred,RepPredProp(Yap_GetPredPropByFunc(FunctorArg,0)),H);
      }
#endif	/* LOW_LEVEL_TRACE */
      BEGD(d0);
      d0 = XREG(PREG->u.yxx.x1);
      deref_head(d0, arg_y_arg1_unk);
    arg_y_arg1_nvar:
      /* ARG1 is ok! */
      if (IsIntTerm(d0))
	d0 = IntOfTerm(d0);
      else if (IsLongIntTerm(d0)) {
	d0 = LongIntOfTerm(d0);
      } else {
	saveregs();
	Yap_Error(TYPE_ERROR_INTEGER,d0,"arg 1 of arg/3");
	setregs();
	FAIL();
      }

      /* d0 now got the argument we want */
      BEGD(d1);
      d1 = XREG(PREG->u.yxx.x2);
      deref_head(d1, arg_y_arg2_unk);
    arg_y_arg2_nvar:
      /* d1 now got the structure we want to fetch the argument
       * from */
      if (IsApplTerm(d1)) {
	BEGP(pt0);
	pt0 = RepAppl(d1);
	d1 = *pt0;
	if (IsExtensionFunctor((Functor) d1)) {
	  FAIL();
	}
	if ((Int)d0 <= 0 ||
	    (Int)d0 > ArityOfFunctor((Functor) d1)) {
	  /* don't complain here for Prolog compatibility 
	  if ((Int)d0 <= 0) {
	    saveregs();
	    Yap_Error(DOMAIN_ERROR_NOT_LESS_THAN_ZERO,
		  MkIntegerTerm(d0),"arg 1 of arg/3");	    
	    saveregs();
	  }
	  */
	  FAIL();
	}
	BEGP(pt1);
	pt1 = YREG + PREG->u.yxx.y;
	PREG = NEXTOP(PREG, yxx);
#if defined(SBA) && defined(FROZEN_STACKS)
	Bind_Local(pt1,pt0[d0]);
#else
	*pt1 = pt0[d0];
#endif /* SBA && FROZEN_STACKS */
	ENDP(pt1);
	GONext();
	ENDP(pt0);
      }
      else if (IsPairTerm(d1)) {
	BEGP(pt0);
	pt0 = RepPair(d1);
	if (d0 != 1 && d0 != 2) {
	  if ((Int)d0 < 0) {
	    saveregs();
	    Yap_Error(DOMAIN_ERROR_NOT_LESS_THAN_ZERO,
		  MkIntegerTerm(d0),"arg 1 of arg/3");
	    setregs();
	  }
	  FAIL();
	}
	BEGP(pt1);
	pt1 = YREG + PREG->u.yxx.y;
	PREG = NEXTOP(PREG, yxx);
#if defined(SBA) && defined(FROZEN_STACKS)
	Bind_Local(pt1,pt0[d0-1]);
#else
	*pt1 = pt0[d0-1];
#endif /* SBA && FROZEN_STACKS */
	GONext();
	ENDP(pt1);
	ENDP(pt0);
      }
      else {
	saveregs();
	Yap_Error(TYPE_ERROR_COMPOUND, d1, "arg 2 of arg/3");
	setregs();
	FAIL();
      }

      BEGP(pt0);
      deref_body(d1, pt0, arg_y_arg2_unk, arg_y_arg2_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, d1,"arg 2 of arg/3");;
      setregs();
      ENDP(pt0);
      FAIL();
      ENDD(d1);

      BEGP(pt0);
      deref_body(d0, pt0, arg_y_arg1_unk, arg_y_arg1_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, d0, "arg 1 of arg/3");;
      setregs();
      ENDP(pt0);
      FAIL();
      ENDD(d0);
      ENDOp();

      Op(p_arg_y_cv, yxn);
#ifdef LOW_LEVEL_TRACER
      if (Yap_do_low_level_trace) {
	CELL *Ho = H;
	Term t = MkIntegerTerm(PREG->u.yxn.c); 
	H[0] =  t;
	H[1] = XREG(PREG->u.yxn.xi);
	H[2] = YREG[PREG->u.yxn.y];
	RESET_VARIABLE(H+2);
	low_level_trace(enter_pred,RepPredProp(Yap_GetPredPropByFunc(FunctorArg,0)),H);
	H = Ho;
      }
#endif	/* LOW_LEVEL_TRACE */
      BEGD(d0);
      d0 = PREG->u.yxn.c;
      /* d0 now got the argument we want */
      BEGD(d1);
      d1 = XREG(PREG->u.yxn.xi);
      deref_head(d1, arg_y_arg2_vc_unk);
    arg_y_arg2_vc_nvar:
      /* d1 now got the structure we want to fetch the argument
       * from */
      if (IsApplTerm(d1)) {
	BEGP(pt0);
	pt0 = RepAppl(d1);
	d1 = *pt0;
	if (IsExtensionFunctor((Functor) d1)) {
	  FAIL();
	}
	if ((Int)d0 <= 0 ||
	    (Int)d0 > ArityOfFunctor((Functor) d1)) {
	  /* don't complain here for Prolog compatibility 
	  if ((Int)d0 <= 0) {
	    saveregs();
	    Yap_Error(DOMAIN_ERROR_NOT_LESS_THAN_ZERO,
		  MkIntegerTerm(d0),"arg 1 of arg/3");	    
	    setregs();
	  }
	  */
	  FAIL();
	}
	BEGP(pt1);
	pt1 = YREG + PREG->u.yxn.y;
	PREG = NEXTOP(PREG, yxn);
#if defined(SBA) && defined(FROZEN_STACKS)
	Bind_Local(pt1,pt0[d0]);
#else
	*pt1 = pt0[d0];
#endif /* SBA && FROZEN_STACKS */
	ENDP(pt1);
	GONext();
	ENDP(pt0);
      }
      else if (IsPairTerm(d1)) {
	BEGP(pt0);
	pt0 = RepPair(d1);
	if (d0 != 1 && d0 != 2) {
	  if ((Int)d0 < 0) {
	    saveregs();
	    Yap_Error(DOMAIN_ERROR_NOT_LESS_THAN_ZERO,
		  MkIntegerTerm(d0),"arg 1 of arg/3");
	    setregs();
	  }
	  FAIL();
	}
	BEGP(pt1);
	pt1 = YREG + PREG->u.yxn.y;
	PREG = NEXTOP(PREG, yxn);
#if defined(SBA) && defined(FROZEN_STACKS)
	Bind_Local(pt1,pt0[d0-1]);
#else
	*pt1 = pt0[d0-1];
#endif /* SBA && FROZEN_STACKS */
	ENDP(pt1);
	GONext();
	ENDP(pt0);
      }
      else {
	saveregs();
	Yap_Error(TYPE_ERROR_COMPOUND, d1, "arg 2 of arg/3");
	setregs();
	FAIL();
      }

      BEGP(pt0);
      deref_body(d1, pt0, arg_y_arg2_vc_unk, arg_y_arg2_vc_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, d1,"arg 2 of arg/3");;
      setregs();
      ENDP(pt0);
      FAIL();
      ENDD(d1);

      ENDD(d0);
      ENDOp();

      Op(p_func2s_vv, xxx);
      /* A1 is a variable */
    restart_func2s:
#ifdef LOW_LEVEL_TRACER
      if (Yap_do_low_level_trace) {
	RESET_VARIABLE(H);
	H[1] = XREG(PREG->u.xxx.x1);
	H[2] = XREG(PREG->u.xxx.x2);
	low_level_trace(enter_pred,RepPredProp(Yap_GetPredPropByFunc(FunctorFunctor,0)),H);
      }
#endif	/* LOW_LEVEL_TRACE */
      /* We have to build the structure */
      BEGD(d0);
      d0 = XREG(PREG->u.xxx.x1);
      deref_head(d0, func2s_unk);
    func2s_nvar:
      /* we do, let's get the third argument */
      BEGD(d1);
      d1 = XREG(PREG->u.xxx.x2);
      deref_head(d1, func2s_unk2);
    func2s_nvar2:
      /* Uuuff, the second and third argument are bound */
      if (IsIntegerTerm(d1))
	d1 = IntegerOfTerm(d1);
      else {
	saveregs();
	if (IsBigIntTerm(d1)) {
	  Yap_Error(RESOURCE_ERROR_STACK, d1, "functor/3");
	} else {
	  Yap_Error(TYPE_ERROR_INTEGER, d1, "functor/3");
	}
	setregs();
	FAIL();
      }
      if (!IsAtomicTerm(d0)) {
	saveregs();
	Yap_Error(TYPE_ERROR_ATOM,d0,"functor/3");
	setregs();
	FAIL();
      }
      /* We made it!!!!! we got in d0 the name, in d1 the arity and
       * in pt0 the variable to bind it to. */
      if (d0 == TermDot && d1 == 2) {
	RESET_VARIABLE(H);
	RESET_VARIABLE(H+1);
	d0 = AbsPair(H);
	H += 2;
	/* else if arity is 0 just pass d0 through */
	/* Ding, ding, we made it */
	XREG(PREG->u.xxx.x) = d0;
	PREG = NEXTOP(NEXTOP(NEXTOP(PREG, xxx),Osbpp),l);
	GONext();
      }
      else if ((Int)d1 > 0) {
	/* now let's build a compound term */
	if (!IsAtomTerm(d0)) {
	  saveregs();
	  Yap_Error(TYPE_ERROR_ATOM,d0,"functor/3");
	  setregs();
	  FAIL();
	}
	BEGP(pt1);
	if (!IsAtomTerm(d0)) {
	  FAIL();
	}
	else
	  d0 = (CELL) Yap_MkFunctor(AtomOfTerm(d0), (Int) d1);
	pt1 = H;
	*pt1++ = d0;
	d0 = AbsAppl(H);
	if (pt1+d1 > ENV || pt1+d1 > (CELL *)B) {
	  /* make sure we have something to show for our trouble */
	  saveregs();
	  if (!Yap_gcl((1+d1)*sizeof(CELL), 0, YREG, NEXTOP(NEXTOP(PREG,xxx),Osbpp))) {
	    Yap_Error(OUT_OF_STACK_ERROR,TermNil,Yap_ErrorMessage);
	    setregs();
	    JMPNext();
	  } else {
	    setregs();
	  }
	  goto restart_func2s;
	}
	while ((Int)d1--) {
	  RESET_VARIABLE(pt1);
	  pt1++;
	}
	H = pt1;
	/* done building the term */
	ENDP(pt1);
	/* else if arity is 0 just pass d0 through */
	/* Ding, ding, we made it */
	XREG(PREG->u.xxx.x) = d0;
	PREG = NEXTOP(NEXTOP(NEXTOP(PREG, xxx),Osbpp),l);
	GONext();
      }	else if ((Int)d1  == 0) {
	XREG(PREG->u.xxx.x) = d0;
	PREG = NEXTOP(NEXTOP(NEXTOP(PREG, xxx),Osbpp),l);
	GONext();
      }	else {
	saveregs();
	Yap_Error(DOMAIN_ERROR_NOT_LESS_THAN_ZERO,MkIntegerTerm(d1),"functor/3");
	setregs();
	FAIL();
      }

      BEGP(pt1);
      deref_body(d1, pt1, func2s_unk2, func2s_nvar2);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, d1, "functor/3");
      setregs();
      ENDP(pt1);
      /* Oops, third argument was unbound */
      FAIL();
      ENDD(d1);

      BEGP(pt1);
      deref_body(d0, pt1, func2s_unk, func2s_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, d0, "functor/3");
      setregs();
      ENDP(pt1);
      /* Oops, second argument was unbound too */
      FAIL();
      ENDD(d0);
      ENDOp();

      Op(p_func2s_cv, xxc);
      /* A1 is a variable */
    restart_func2s_cv:
#ifdef LOW_LEVEL_TRACER
      if (Yap_do_low_level_trace) {
	RESET_VARIABLE(H);
	H[1] = PREG->u.xxc.c;
	H[2] = XREG(PREG->u.xxc.xi);
	low_level_trace(enter_pred,RepPredProp(Yap_GetPredPropByFunc(FunctorFunctor,0)),H);
      }
#endif	/* LOW_LEVEL_TRACE */
      BEGD(d0);
      /* We have to build the structure */
      d0 = PREG->u.xxc.c;
      /* we do, let's get the third argument */
      BEGD(d1);
      d1 = XREG(PREG->u.xxc.xi);
      deref_head(d1, func2s_unk2_cv);
    func2s_nvar2_cv:
      /* Uuuff, the second and third argument are bound */
      if (IsIntegerTerm(d1))
	d1 = IntegerOfTerm(d1);
      else {
	saveregs();
	if (IsBigIntTerm(d1)) {
	  Yap_Error(RESOURCE_ERROR_STACK, d1, "functor/3");
	} else {
	  Yap_Error(TYPE_ERROR_INTEGER,d1,"functor/3");
	}
	setregs();
	FAIL();
      }
      /* We made it!!!!! we got in d0 the name, in d1 the arity and
       * in pt0 the variable to bind it to. */
      if (d0 == TermDot && d1 == 2) {
	RESET_VARIABLE(H);
	RESET_VARIABLE(H+1);
	d0 = AbsPair(H);
	H += 2;
	/* else if arity is 0 just pass d0 through */
	/* Ding, ding, we made it */
	XREG(PREG->u.xxc.x) = d0;
	PREG = NEXTOP(NEXTOP(NEXTOP(PREG, xxc),Osbpp),l);
	GONext();
      } else if ((Int)d1 > 0) {
	/* now let's build a compound term */
	if (!IsAtomTerm(d0)) {
	  saveregs();
	  Yap_Error(TYPE_ERROR_ATOM,d0,"functor/3");
	  setregs();
	  FAIL();
	}
	BEGP(pt1);
	if (!IsAtomTerm(d0)) {
	  FAIL();
	}
	else
	  d0 = (CELL) Yap_MkFunctor(AtomOfTerm(d0), (Int) d1);
	pt1 = H;
	*pt1++ = d0;
	d0 = AbsAppl(H);
	if (pt1+d1 > ENV || pt1+d1 > (CELL *)B) {
	  /* make sure we have something to show for our trouble */
	  saveregs();
	  if (!Yap_gcl((1+d1)*sizeof(CELL), 0, YREG, NEXTOP(NEXTOP(PREG,xxc),Osbpp))) {
	    Yap_Error(OUT_OF_STACK_ERROR,TermNil,Yap_ErrorMessage);
	    setregs();
	    JMPNext();
	  } else {
	    setregs();
	  }
	  goto restart_func2s_cv;
	}
	while ((Int)d1--) {
	  RESET_VARIABLE(pt1);
	  pt1++;
	}
	/* done building the term */
	H = pt1;
	ENDP(pt1);
	/* else if arity is 0 just pass d0 through */
	/* Ding, ding, we made it */
	XREG(PREG->u.xxc.x) = d0;
	PREG = NEXTOP(NEXTOP(NEXTOP(PREG, xxc),Osbpp),l);
	GONext();
      }	else if (d1  == 0) {
	XREG(PREG->u.xxc.x) = d0;
	PREG = NEXTOP(NEXTOP(NEXTOP(PREG, xxc),Osbpp),l);
	GONext();
      }	else {
	saveregs();
	Yap_Error(DOMAIN_ERROR_NOT_LESS_THAN_ZERO,MkIntegerTerm(d1),"functor/3");
	setregs();
	FAIL();
      }

      BEGP(pt1);
      deref_body(d1, pt1, func2s_unk2_cv, func2s_nvar2_cv);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, d1, "functor/3");
      setregs();
      ENDP(pt1);
      /* Oops, third argument was unbound */
      FAIL();
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(p_func2s_vc, xxn);
      /* A1 is a variable */
    restart_func2s_vc:
#ifdef LOW_LEVEL_TRACER
      if (Yap_do_low_level_trace) {
	Term ti;
	CELL *hi = H;

	ti = MkIntegerTerm(PREG->u.xxn.c);
	RESET_VARIABLE(H);
	H[1] = XREG(PREG->u.xxn.xi);
	H[2] = ti;
	low_level_trace(enter_pred,RepPredProp(Yap_GetPredPropByFunc(FunctorFunctor,0)),H);
	H = hi;
      }
#endif	/* LOW_LEVEL_TRACE */
      /* We have to build the structure */
      BEGD(d0);
      d0 = XREG(PREG->u.xxn.xi);
      deref_head(d0, func2s_unk_vc);
    func2s_nvar_vc:
      BEGD(d1);
      d1 = PREG->u.xxn.c;
      if (!IsAtomicTerm(d0)) {
	saveregs();
	Yap_Error(TYPE_ERROR_ATOM,d0,"functor/3");
	setregs();
	FAIL();
      }
      /* We made it!!!!! we got in d0 the name, in d1 the arity and
       * in pt0 the variable to bind it to. */
      if (d0 == TermDot && d1 == 2) {
	RESET_VARIABLE(H);
	RESET_VARIABLE(H+1);
	d0 = AbsPair(H);
	H += 2;
	/* else if arity is 0 just pass d0 through */
	/* Ding, ding, we made it */
	XREG(PREG->u.xxn.x) = d0;
	PREG = NEXTOP(NEXTOP(NEXTOP(PREG, xxn),Osbpp),l);
	GONext();
      }
      /* now let's build a compound term */
      if (d1 == 0) {
	XREG(PREG->u.xxn.x) = d0;
	PREG = NEXTOP(NEXTOP(NEXTOP(PREG, xxn),Osbpp),l);
	GONext();
      }
      if (!IsAtomTerm(d0)) {
	saveregs();
	Yap_Error(TYPE_ERROR_ATOM,d0,"functor/3");
	setregs();
	FAIL();
      }
      BEGP(pt1);
      if (!IsAtomTerm(d0)) {
	FAIL();
      }
      else
	d0 = (CELL) Yap_MkFunctor(AtomOfTerm(d0), (Int) d1);
      pt1 = H;
      *pt1++ = d0;
      d0 = AbsAppl(H);
      if (pt1+d1 > ENV || pt1+d1 > (CELL *)B) {
	/* make sure we have something to show for our trouble */
	saveregs();
	if (!Yap_gc(0, YREG, NEXTOP(NEXTOP(PREG,xxn),Osbpp))) {
	  Yap_Error(OUT_OF_STACK_ERROR,TermNil,Yap_ErrorMessage);
	  setregs();
	  JMPNext();
	} else {
	  setregs();
	}
	goto restart_func2s_vc;
      }
      while ((Int)d1--) {
	RESET_VARIABLE(pt1);
	pt1++;
      }
      /* done building the term */
      H = pt1;
      ENDP(pt1);
      ENDD(d1);
      /* else if arity is 0 just pass d0 through */
      /* Ding, ding, we made it */
      XREG(PREG->u.xxn.x) = d0;
      PREG = NEXTOP(NEXTOP(NEXTOP(PREG, xxn),Osbpp),l);
      GONext();

      BEGP(pt1);
      deref_body(d0, pt1, func2s_unk_vc, func2s_nvar_vc);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, d0, "functor/3");
      setregs();
      ENDP(pt1);
      /* Oops, second argument was unbound too */
      FAIL();
      ENDD(d0);
      ENDOp();

      Op(p_func2s_y_vv, yxx);
      /* A1 is a variable */
    restart_func2s_y:
#ifdef LOW_LEVEL_TRACER
      if (Yap_do_low_level_trace) {
	RESET_VARIABLE(H);
	H[1] = XREG(PREG->u.yxx.x1);
	H[2] = XREG(PREG->u.yxx.x2);
	low_level_trace(enter_pred,RepPredProp(Yap_GetPredPropByFunc(FunctorFunctor,0)),H);
      }
#endif	/* LOW_LEVEL_TRACE */
      /* We have to build the structure */
      BEGD(d0);
      d0 = XREG(PREG->u.yxx.x1);
      deref_head(d0, func2s_y_unk);
    func2s_y_nvar:
      /* we do, let's get the third argument */
      BEGD(d1);
      d1 = XREG(PREG->u.yxx.x2);
      deref_head(d1, func2s_y_unk2);
    func2s_y_nvar2:
      /* Uuuff, the second and third argument are bound */
      if (IsIntegerTerm(d1))
	d1 = IntegerOfTerm(d1);
      else {
	saveregs();
	if (IsBigIntTerm(d1)) {
	  Yap_Error(RESOURCE_ERROR_STACK, d1, "functor/3");
	} else {
	  Yap_Error(TYPE_ERROR_INTEGER,d1,"functor/3");
	}
	setregs();
	FAIL();
      }
      if (!IsAtomicTerm(d0)) {
	saveregs();
	Yap_Error(TYPE_ERROR_ATOM,d0,"functor/3");
	setregs();
	FAIL();
      }
      /* We made it!!!!! we got in d0 the name, in d1 the arity and
       * in pt0 the variable to bind it to. */
      if (d0 == TermDot && d1 == 2) {
	RESET_VARIABLE(H);
	RESET_VARIABLE(H+1);
	d0 = AbsPair(H);
	H += 2;
	BEGP(pt1);
	pt1 = YREG + PREG->u.yxx.y;
	PREG = NEXTOP(NEXTOP(NEXTOP(PREG, yxx),Osbpp),l);
#if defined(SBA) && defined(FROZEN_STACKS)
	Bind_Local(pt1,d0);
#else
	*pt1 = d0;
#endif /* SBA && FROZEN_STACKS */
	ENDP(pt1);
	GONext();
      } else if ((Int)d1 > 0) {
	/* now let's build a compound term */
	if (!IsAtomTerm(d0)) {
	  saveregs();
	  Yap_Error(TYPE_ERROR_ATOM,d0,"functor/3");
	  setregs();
	  FAIL();
	}
	BEGP(pt1);
	if (!IsAtomTerm(d0)) {
	  FAIL();
	}
	else
	  d0 = (CELL) Yap_MkFunctor(AtomOfTerm(d0), (Int) d1);
	pt1 = H;
	*pt1++ = d0;
	d0 = AbsAppl(H);
	if (pt1+d1 > ENV || pt1+d1 > (CELL *)B) {
	  /* make sure we have something to show for our trouble */
	  saveregs();
	  if (!Yap_gcl((1+d1)*sizeof(CELL), 0, YREG, NEXTOP(NEXTOP(PREG,yxx),Osbpp))) {
	    Yap_Error(OUT_OF_STACK_ERROR,TermNil,Yap_ErrorMessage);
	    setregs();
	    JMPNext();
	  } else {
	    setregs();
	  }
	  goto restart_func2s_y;
	}
	while ((Int)d1--) {
	  RESET_VARIABLE(pt1);
	  pt1++;
	}
	/* done building the term */
	H = pt1;
	ENDP(pt1);
	/* else if arity is 0 just pass d0 through */
	/* Ding, ding, we made it */
	BEGP(pt1);
	pt1 = YREG + PREG->u.yxx.y;
	PREG = NEXTOP(NEXTOP(NEXTOP(PREG, yxx),Osbpp),l);
#if defined(SBA) && defined(FROZEN_STACKS)
	Bind_Local(pt1,d0);
#else
	*pt1 = d0;
#endif /* SBA && FROZEN_STACKS */
	ENDP(pt1);
	GONext();
      }	else if (d1  == 0) {
	BEGP(pt1);
	pt1 = YREG + PREG->u.yxx.y;
	PREG = NEXTOP(NEXTOP(NEXTOP(PREG, yxx),Osbpp),l);
#if defined(SBA) && defined(FROZEN_STACKS)
	Bind_Local(pt1,d0);
#else
	*pt1 = d0;
#endif /* SBA && FROZEN_STACKS */
	ENDP(pt1);
	GONext();
      }	else {
	saveregs();
	Yap_Error(DOMAIN_ERROR_NOT_LESS_THAN_ZERO,MkIntegerTerm(d1),"functor/3");
	setregs();
	FAIL();
      }

      BEGP(pt1);
      deref_body(d1, pt1, func2s_y_unk2, func2s_y_nvar2);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, d1, "functor/3");
      setregs();
      ENDP(pt1);
      /* Oops, third argument was unbound */
      FAIL();
      ENDD(d1);

      BEGP(pt1);
      deref_body(d0, pt1, func2s_y_unk, func2s_y_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, d0, "functor/3");
      setregs();
      ENDP(pt1);
      /* Oops, second argument was unbound too */
      FAIL();
      ENDD(d0);
      ENDOp();

      Op(p_func2s_y_cv, yxn);
      /* A1 is a variable */
    restart_func2s_y_cv:
#ifdef LOW_LEVEL_TRACER
      if (Yap_do_low_level_trace) {
	RESET_VARIABLE(H);
	H[1] = PREG->u.yxn.c;
	H[2] = XREG(PREG->u.yxn.xi);
	low_level_trace(enter_pred,RepPredProp(Yap_GetPredPropByFunc(FunctorFunctor,0)),H);
      }
#endif	/* LOW_LEVEL_TRACE */
      /* We have to build the structure */
      BEGD(d0);
      d0 = PREG->u.yxn.c;
      /* we do, let's get the third argument */
      BEGD(d1);
      d1 = XREG(PREG->u.yxn.xi);
      deref_head(d1, func2s_y_unk_cv);
    func2s_y_nvar_cv:
      /* Uuuff, the second and third argument are bound */
      if (IsIntegerTerm(d1)) {
	d1 = IntegerOfTerm(d1);
      } else {
	saveregs();
	if (IsBigIntTerm(d1)) {
	  Yap_Error(RESOURCE_ERROR_STACK, d1, "functor/3");
	} else {
	  Yap_Error(TYPE_ERROR_INTEGER,d1,"functor/3");
	}
	setregs();
	FAIL();
      }
      /* We made it!!!!! we got in d0 the name, in d1 the arity and
       * in pt0 the variable to bind it to. */
      if (d0 == TermDot && d1 == 2) {
	RESET_VARIABLE(H);
	RESET_VARIABLE(H+1);
	d0 = AbsPair(H);
	H += 2;
	/* else if arity is 0 just pass d0 through */
	/* Ding, ding, we made it */
	BEGP(pt1);
	pt1 = YREG + PREG->u.yxn.y;
	PREG = NEXTOP(NEXTOP(NEXTOP(PREG, yxn),Osbpp),l);
#if defined(SBA) && defined(FROZEN_STACKS)
	Bind_Local(pt1,d0);
#else
	*pt1 = d0;
#endif /* SBA && FROZEN_STACKS */
	ENDP(pt1);
	GONext();
      }
      else if ((Int)d1 > 0) {
	/* now let's build a compound term */
	if (!IsAtomTerm(d0)) {
	  saveregs();
	  Yap_Error(TYPE_ERROR_ATOM,d0,"functor/3");
	  setregs();
	  FAIL();
	}
	if (!IsAtomTerm(d0)) {
	  FAIL();
	}
	else
	  d0 = (CELL) Yap_MkFunctor(AtomOfTerm(d0), (Int) d1);
	BEGP(pt1);
	pt1 = H;
	*pt1++ = d0;
	d0 = AbsAppl(H);
	if (pt1+d1 > ENV || pt1+d1 > (CELL *)B) {
	  /* make sure we have something to show for our trouble */
	  saveregs();
	  if (!Yap_gcl((1+d1)*sizeof(CELL), 0, YREG, NEXTOP(NEXTOP(PREG,yxn),Osbpp))) {
	    Yap_Error(OUT_OF_STACK_ERROR,TermNil,Yap_ErrorMessage);
	    setregs();
	    JMPNext();
	  } else {
	    setregs();
	  }
	  goto restart_func2s_y_cv;
	}
	while ((Int)d1--) {
	  RESET_VARIABLE(pt1);
	  pt1++;
	}
	/* done building the term */
	H = pt1;
	ENDP(pt1);
	/* else if arity is 0 just pass d0 through */
	/* Ding, ding, we made it */
	BEGP(pt1);
	pt1 = YREG + PREG->u.yxn.y;
	PREG = NEXTOP(NEXTOP(NEXTOP(PREG, yxn),Osbpp),l);
#if defined(SBA) && defined(FROZEN_STACKS)
	Bind_Local(pt1,d0);
#else
	*pt1 = d0;
#endif /* SBA && FROZEN_STACKS */
	ENDP(pt1);
	GONext();
      }	else if (d1  == 0) {
	BEGP(pt1);
	pt1 = YREG + PREG->u.yxn.y;
	PREG = NEXTOP(NEXTOP(NEXTOP(PREG, yxn),Osbpp),l);
#if defined(SBA) && defined(FROZEN_STACKS)
	Bind_Local(pt1,d0);
#else
	*pt1 = d0;
#endif /* SBA && FROZEN_STACKS */
	ENDP(pt1);
	GONext();
      }	else {
	saveregs();
	Yap_Error(DOMAIN_ERROR_NOT_LESS_THAN_ZERO,MkIntegerTerm(d1),"functor/3");
	setregs();
	FAIL();
      }

      BEGP(pt1);
      deref_body(d1, pt1, func2s_y_unk_cv, func2s_y_nvar_cv);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, d1, "functor/3");
      setregs();
      ENDP(pt1);
      /* Oops, third argument was unbound */
      FAIL();
      ENDD(d1);
      ENDD(d0);
      ENDOp();

      Op(p_func2s_y_vc, yxn);
      /* A1 is a variable */
    restart_func2s_y_vc:
#ifdef LOW_LEVEL_TRACER
      if (Yap_do_low_level_trace) {
	Term ti;
	CELL *hi = H;

	ti = MkIntegerTerm((Int)(PREG->u.yxn.c));
	RESET_VARIABLE(H);
	H[1] = XREG(PREG->u.yxn.xi);
	H[2] = ti;
	low_level_trace(enter_pred,RepPredProp(Yap_GetPredPropByFunc(FunctorFunctor,0)),H);
	H = hi;
      }
#endif	/* LOW_LEVEL_TRACE */
      /* We have to build the structure */
      BEGD(d0);
      d0 = XREG(PREG->u.yxn.xi);
      deref_head(d0, func2s_y_unk_vc);
    func2s_y_nvar_vc:
      BEGD(d1);
      d1 = PREG->u.yxn.c;
      if (!IsAtomicTerm(d0)) {
	saveregs();
	Yap_Error(TYPE_ERROR_ATOM,d0,"functor/3");
	setregs();
	FAIL();
      }
      /* We made it!!!!! we got in d0 the name, in d1 the arity and
       * in pt0 the variable to bind it to. */
      if (d0 == TermDot && d1 == 2) {
	RESET_VARIABLE(H);
	RESET_VARIABLE(H+1);
	d0 = AbsPair(H);
	H += 2;
	/* else if arity is 0 just pass d0 through */
	/* Ding, ding, we made it */
	BEGP(pt1);
	pt1 = YREG + PREG->u.yxn.y;
	PREG = NEXTOP(NEXTOP(NEXTOP(PREG, yxn),Osbpp),l);
#if defined(SBA) && defined(FROZEN_STACKS)
	Bind_Local(pt1,d0);
#else
	*pt1 = d0;
#endif /* SBA && FROZEN_STACKS */
	ENDP(pt1);
	GONext();
      }
      if (d1 == 0) {
	BEGP(pt1);
	pt1 = YREG + PREG->u.yxn.y;
	PREG = NEXTOP(NEXTOP(NEXTOP(PREG, yxn),Osbpp),l);
#if defined(SBA) && defined(FROZEN_STACKS)
	Bind_Local(pt1,d0);
#else
	*pt1 = d0;
#endif /* SBA && FROZEN_STACKS */
	ENDP(pt1);
	GONext();
      }
      if (!IsAtomTerm(d0)) {
	saveregs();
	Yap_Error(TYPE_ERROR_ATOM,d0,"functor/3");
	setregs();
	FAIL();
      }
      /* now let's build a compound term */
      if (!IsAtomTerm(d0)) {
	saveregs();
	Yap_Error(TYPE_ERROR_ATOM,d0,"functor/3");
	setregs();
	FAIL();
      }
      BEGP(pt1);
      if (!IsAtomTerm(d0)) {
	FAIL();
      }
      else 
	d0 = (CELL) Yap_MkFunctor(AtomOfTerm(d0), (Int) d1);
      pt1 = H;
      *pt1++ = d0;
      d0 = AbsAppl(H);
      if (pt1+d1 > ENV || pt1+d1 > (CELL *)B) {
	/* make sure we have something to show for our trouble */
	saveregs();
	if (!Yap_gcl((1+d1)*sizeof(CELL), 0, YREG, NEXTOP(NEXTOP(PREG,yxn),Osbpp))) {
	  Yap_Error(OUT_OF_STACK_ERROR,TermNil,Yap_ErrorMessage);
	  setregs();
	  JMPNext();
	} else {
	  setregs();
	}
	goto restart_func2s_y_vc;
      }
      while ((Int)d1--) {
	RESET_VARIABLE(pt1);
	pt1++;
      }
      /* done building the term */
      H = pt1;
      ENDP(pt1);
      /* else if arity is 0 just pass d0 through */
      /* Ding, ding, we made it */
      BEGP(pt1);
      pt1 = YREG + PREG->u.yxn.y;
      PREG = NEXTOP(NEXTOP(NEXTOP(PREG, yxn),Osbpp),l);
#if defined(SBA) && defined(FROZEN_STACKS)
      Bind_Local(pt1,d0);
#else
      *pt1 = d0;
#endif /* SBA && FROZEN_STACKS */
      ENDP(pt1);
      ENDD(d1);
      GONext();

      BEGP(pt1);
      deref_body(d0, pt1, func2s_y_unk_vc, func2s_y_nvar_vc);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, d0, "functor/3");
      setregs();
      ENDP(pt1);
      /* Oops, second argument was unbound too */
      FAIL();
      ENDD(d0);
      ENDOp();

      Op(p_func2f_xx, xxx);
#ifdef LOW_LEVEL_TRACER
      if (Yap_do_low_level_trace) {
	H[0] = XREG(PREG->u.xxx.x);
	RESET_VARIABLE(H+1);
	RESET_VARIABLE(H+2);
	low_level_trace(enter_pred,RepPredProp(Yap_GetPredPropByFunc(FunctorFunctor,0)),H);
      }
#endif	/* LOW_LEVEL_TRACE */
      BEGD(d0);
      d0 = XREG(PREG->u.xxx.x);
      deref_head(d0, func2f_xx_unk);
    func2f_xx_nvar:
      if (IsApplTerm(d0)) {
	Functor d1 = FunctorOfTerm(d0);
	if (IsExtensionFunctor(d1)) {
	  XREG(PREG->u.xxx.x1) = d0;
	  XREG(PREG->u.xxx.x2) = MkIntTerm(0);
	  PREG = NEXTOP(PREG, xxx);
	  GONext();
	}
	XREG(PREG->u.xxx.x1) = MkAtomTerm(NameOfFunctor(d1));
	XREG(PREG->u.xxx.x2) = MkIntegerTerm(ArityOfFunctor(d1));
	PREG = NEXTOP(PREG, xxx);
	GONext();
      } else if (IsPairTerm(d0)) {
	XREG(PREG->u.xxx.x1) = TermDot;
	XREG(PREG->u.xxx.x2) = MkIntTerm(2);
	PREG = NEXTOP(PREG, xxx);
	GONext();
      } else {
	XREG(PREG->u.xxx.x1) = d0;
	XREG(PREG->u.xxx.x2) = MkIntTerm(0);
	PREG = NEXTOP(PREG, xxx);
	GONext();
      }

      BEGP(pt1);
      deref_body(d0, pt1, func2f_xx_unk, func2f_xx_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, d0, "functor/3");
      setregs();
      ENDP(pt1);
      /* Oops, second argument was unbound too */
      FAIL();
      ENDD(d0);
      ENDOp();

      Op(p_func2f_xy, xxy);
#ifdef LOW_LEVEL_TRACER
      if (Yap_do_low_level_trace) {
	H[0] = XREG(PREG->u.xxy.x);
	RESET_VARIABLE(H+1);
	RESET_VARIABLE(H+2);
	low_level_trace(enter_pred,RepPredProp(Yap_GetPredPropByFunc(FunctorFunctor,0)),H);
      }
#endif	/* LOW_LEVEL_TRACE */
      BEGD(d0);
      d0 = XREG(PREG->u.xxy.x);
      deref_head(d0, func2f_xy_unk);
    func2f_xy_nvar:
      if (IsApplTerm(d0)) {
	Functor d1 = FunctorOfTerm(d0);
	CELL *pt0 = YREG+PREG->u.xxy.y2;
	if (IsExtensionFunctor(d1)) {
	  XREG(PREG->u.xxy.x1) = d0;
	  PREG = NEXTOP(PREG, xxy);
	  *pt0 = MkIntTerm(0);
	  GONext();
	}
	XREG(PREG->u.xxy.x1) = MkAtomTerm(NameOfFunctor(d1));
	PREG = NEXTOP(PREG, xxy);
	*pt0 = MkIntegerTerm(ArityOfFunctor(d1));
	GONext();
      } else if (IsPairTerm(d0)) {
	CELL *pt0 = YREG+PREG->u.xxy.y2;
	XREG(PREG->u.xxy.x1) = TermDot;
	PREG = NEXTOP(PREG, xxy);
	*pt0 = MkIntTerm(2);
	GONext();
      } else {
	CELL *pt0 = YREG+PREG->u.xxy.y2;
	XREG(PREG->u.xxy.x1) = d0;
	PREG = NEXTOP(PREG, xxy);
	*pt0 = MkIntTerm(0);
	GONext();
      }

      BEGP(pt1);
      deref_body(d0, pt1, func2f_xy_unk, func2f_xy_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, d0, "functor/3");
      setregs();
      ENDP(pt1);
      /* Oops, second argument was unbound too */
      FAIL();
      ENDD(d0);
      ENDOp();

      Op(p_func2f_yx, yxx);
#ifdef LOW_LEVEL_TRACER
      if (Yap_do_low_level_trace) {
	H[0] = XREG(PREG->u.yxx.x2);
	RESET_VARIABLE(H+1);
	RESET_VARIABLE(H+2);
	low_level_trace(enter_pred,RepPredProp(Yap_GetPredPropByFunc(FunctorFunctor,0)),H);
      }
#endif	/* LOW_LEVEL_TRACE */
      BEGD(d0);
      d0 = XREG(PREG->u.yxx.x2);
      deref_head(d0, func2f_yx_unk);
    func2f_yx_nvar:
      if (IsApplTerm(d0)) {
	Functor d1 = FunctorOfTerm(d0);
	CELL *pt0 = YREG+PREG->u.yxx.y;
	if (IsExtensionFunctor(d1)) {
	  XREG(PREG->u.yxx.x1) = MkIntTerm(0);
	  PREG = NEXTOP(PREG, yxx);
	  *pt0 = d0;
	  GONext();
	}
	XREG(PREG->u.yxx.x1) = MkIntegerTerm(ArityOfFunctor(d1));
	PREG = NEXTOP(PREG, yxx);
	*pt0 = MkAtomTerm(NameOfFunctor(d1));
	GONext();
      } else if (IsPairTerm(d0)) {
	CELL *pt0 = YREG+PREG->u.yxx.y;
	XREG(PREG->u.yxx.x1) = MkIntTerm(2);
	PREG = NEXTOP(PREG, yxx);
	*pt0 = TermDot;
	GONext();
      } else {
	CELL *pt0 = YREG+PREG->u.yxx.y;
	XREG(PREG->u.yxx.x1) = MkIntTerm(0);
	PREG = NEXTOP(PREG, yxx);
	*pt0 = d0;
	GONext();
      }

      BEGP(pt1);
      deref_body(d0, pt1, func2f_yx_unk, func2f_yx_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, d0, "functor/3");
      setregs();
      ENDP(pt1);
      /* Oops, second argument was unbound too */
      FAIL();
      ENDD(d0);
      ENDOp();

      Op(p_func2f_yy, yyx);
#ifdef LOW_LEVEL_TRACER
      if (Yap_do_low_level_trace) {
	H[0] = XREG(PREG->u.yyx.x);
	RESET_VARIABLE(H+1);
	RESET_VARIABLE(H+2);
	low_level_trace(enter_pred,RepPredProp(Yap_GetPredPropByFunc(FunctorFunctor,0)),H);
      }
#endif	/* LOW_LEVEL_TRACE */
      BEGD(d0);
      d0 = XREG(PREG->u.yyx.x);
      deref_head(d0, func2f_yy_unk);
    func2f_yy_nvar:
      if (IsApplTerm(d0)) {
	Functor d1 = FunctorOfTerm(d0);
	CELL *pt0 = YREG+PREG->u.yyx.y1;
	CELL *pt1 = YREG+PREG->u.yyx.y2;
	if (IsExtensionFunctor(d1)) {
	  PREG = NEXTOP(PREG, yyx);
	  *pt0 =  d0;
	  *pt1 = MkIntTerm(0);
	  GONext();
	}
	PREG = NEXTOP(PREG, yyx);
	*pt0 = MkAtomTerm(NameOfFunctor(d1));
	*pt1 = MkIntegerTerm(ArityOfFunctor(d1));
	GONext();
      } else if (IsPairTerm(d0)) {
	CELL *pt0 = YREG+PREG->u.yyx.y1;
	CELL *pt1 = YREG+PREG->u.yyx.y2;
	PREG = NEXTOP(PREG, yyx);
	*pt0 = TermDot;
	*pt1 = MkIntTerm(2);
	GONext();
      } else {
	CELL *pt0 = YREG+PREG->u.yyx.y1;
	CELL *pt1 = YREG+PREG->u.yyx.y2;
	PREG = NEXTOP(PREG, yyx);
	*pt0 = d0;
	*pt1 = MkIntTerm(0);
	GONext();
      }

      BEGP(pt1);
      deref_body(d0, pt1, func2f_yy_unk, func2f_yy_nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, d0, "functor/3");
      setregs();
      ENDP(pt1);
      /* Oops, second argument was unbound too */
      FAIL();
      ENDD(d0);
      ENDOp();

      Op(p_functor, e);
#ifdef LOW_LEVEL_TRACER
      if (Yap_do_low_level_trace)
	low_level_trace(enter_pred,RepPredProp(Yap_GetPredPropByFunc(FunctorFunctor,0)),XREGS+1);
#endif	/* LOW_LEVEL_TRACE */
      restart_functor:
      BEGD(d0);
      d0 = ARG1;
      deref_head(d0, func_unk);
    func_nvar:
      /* A1 is bound */
      BEGD(d1);
      if (IsApplTerm(d0)) {
	d1 = *RepAppl(d0);
	if (IsExtensionFunctor((Functor) d1)) {
	  if (d1 <= (CELL)FunctorDouble && d1 >= (CELL)FunctorLongInt ) {
	    d1 = MkIntTerm(0);
	  } else
	    FAIL();
	} else {
	    d0 = MkAtomTerm(NameOfFunctor((Functor) d1));
	    d1 = MkIntTerm(ArityOfFunctor((Functor) d1));
	}
      }
      else if (IsPairTerm(d0)) {
	d0 = TermDot;
	d1 = MkIntTerm(2);
      }
      else {
	d1 = MkIntTerm(0);
      }
      /* d1 and d0 now have the two arguments */
      /* let's go and bind them */
      {
	register CELL arity = d1;

	d1 = ARG2;
	deref_head(d1, func_nvar_unk);
      func_nvar_nvar:
	/* A2 was bound */
	if (d0 != d1) {
	  FAIL();
	}
	/* I have to this here so that I don't have a jump to a closing bracket */
	d0 = arity;
	goto func_bind_x3;

	BEGP(pt0);
	deref_body(d1, pt0, func_nvar_unk, func_nvar_nvar);
	/* A2 is a variable, go and bind it */
	BIND(pt0, d0, bind_func_nvar_var);
#ifdef COROUTINING
	DO_TRAIL(pt0, d0);
	if (pt0 < H0) Yap_WakeUp(pt0);
      bind_func_nvar_var:
#endif
	/* I have to this here so that I don't have a jump to a closing bracket */
	d0 = arity;
	ENDP(pt0);
      func_bind_x3:
	/* now let's process A3 */
	d1 = ARG3;
	deref_head(d1, func_nvar3_unk);
      func_nvar3_nvar:
	/* A3 was bound */
	if (d0 != d1) {
	  FAIL();
	}
	/* Done */
	PREG = NEXTOP(NEXTOP(NEXTOP(PREG, e),Osbmp),l);
	GONext();

	BEGP(pt0);
	deref_body(d1, pt0, func_nvar3_unk, func_nvar3_nvar);
	/* A3 is a variable, go and bind it */
	PREG = NEXTOP(NEXTOP(NEXTOP(PREG, e),Osbmp),l);
	BIND(pt0, d0, bind_func_nvar3_var);
	/* Done */
#ifdef COROUTINING
	DO_TRAIL(pt0, d0);
	if (pt0 < H0) Yap_WakeUp(pt0);
      bind_func_nvar3_var:
#endif
	GONext();


	ENDP(pt0);

      }
      ENDD(d1);

      BEGP(pt0);
      deref_body(d0, pt0, func_unk, func_nvar);
      /* A1 is a variable */
      /* We have to build the structure */
      d0 = ARG2;
      deref_head(d0, func_var_2unk);
    func_var_2nvar:
      /* we do, let's get the third argument */
      BEGD(d1);
      d1 = ARG3;
      deref_head(d1, func_var_3unk);
    func_var_3nvar:
      /* Uuuff, the second and third argument are bound */
      if (IsIntTerm(d1))
	d1 = IntOfTerm(d1);
      else {
	saveregs();
	Yap_Error(TYPE_ERROR_INTEGER,ARG3,"functor/3");
	setregs();
	FAIL();
      }
      if (!IsAtomicTerm(d0)) {
	saveregs();
	Yap_Error(TYPE_ERROR_ATOM,d0,"functor/3");
	setregs();
	FAIL();
      }      /* We made it!!!!! we got in d0 the name, in d1 the arity and
       * in pt0 the variable to bind it to. */
      if (d0 == TermDot && d1 == 2) {
	RESET_VARIABLE(H);
	RESET_VARIABLE(H+1);
	d0 = AbsPair(H);
	H += 2;
      }
      else if ((Int)d1 > 0) {
	/* now let's build a compound term */
	if (!IsAtomTerm(d0)) {
	  saveregs();
	  Yap_Error(TYPE_ERROR_ATOM,d0,"functor/3");
	  setregs();
	  FAIL();
	}
	BEGP(pt1);
	if (!IsAtomTerm(d0)) {
	  FAIL();
	}
	else
	  d0 = (CELL) Yap_MkFunctor(AtomOfTerm(d0), (Int) d1);
	pt1 = H;
	*pt1++ = d0;
	d0 = AbsAppl(H);
	if (pt1+d1 > ENV || pt1+d1 > (CELL *)B) {
	  /* make sure we have something to show for our trouble */
	  saveregs();
	  if (!Yap_gcl((1+d1)*sizeof(CELL), 3, YREG, NEXTOP(NEXTOP(PREG,e),Osbmp))) {
	    Yap_Error(OUT_OF_STACK_ERROR,TermNil,Yap_ErrorMessage);
	    setregs();
	    JMPNext();
	  } else {
	    setregs();
	  }
	  goto restart_functor;	/*  */
	}
	while ((Int)d1--) {
	  RESET_VARIABLE(pt1);
	  pt1++;
	}
	/* done building the term */
	H = pt1;
	ENDP(pt1);
      }	else if ((Int)d1  < 0) {
	saveregs();
	Yap_Error(DOMAIN_ERROR_NOT_LESS_THAN_ZERO,MkIntegerTerm(d1),"functor/3");
	setregs();
	FAIL();
      }
      /* else if arity is 0 just pass d0 through */
      /* Ding, ding, we made it */
      PREG = NEXTOP(NEXTOP(NEXTOP(PREG, e),Osbpp),l);
      BIND(pt0, d0, bind_func_var_3nvar);
#ifdef COROUTINING
      DO_TRAIL(pt0, d0);
      if (pt0 < H0) Yap_WakeUp(pt0);
    bind_func_var_3nvar:
#endif
      GONext();


      BEGP(pt1);
      deref_body(d1, pt1, func_var_3unk, func_var_3nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, d1, "functor/3");
      setregs();
      ENDP(pt1);
      /* Oops, third argument was unbound */
      FAIL();
      ENDD(d1);

      BEGP(pt1);
      deref_body(d0, pt1, func_var_2unk, func_var_2nvar);
      saveregs();
      Yap_Error(INSTANTIATION_ERROR, d0, "functor/3");
      setregs();
      ENDP(pt1);
      /* Oops, second argument was unbound too */
      FAIL();
      ENDP(pt0);
      ENDD(d0);
      ENDOp();

      BOp(p_execute2, Osbpp);
      { 
	PredEntry *pen;
	Term mod = ARG2;

	deref_head(mod, execute2_unk0);
    execute2_nvar0:
	if (!IsAtomTerm(mod)) {
	  saveregs();
	  Yap_Error(TYPE_ERROR_ATOM, mod, "call/2");
	  setregs();
	}
	CACHE_Y_AS_ENV(YREG);
	/* Try to preserve the environment */
	ENV_YREG = (CELL *) (((char *) YREG) + PREG->u.Osbpp.s);
#ifdef FROZEN_STACKS
	{ 
	  choiceptr top_b = PROTECT_FROZEN_B(B);
#ifdef SBA
	  if (ENV_YREG > (CELL *) top_b || ENV_YREG < H) ENV_YREG = (CELL *) top_b;
#else
	  if (ENV_YREG > (CELL *) top_b) ENV_YREG = (CELL *) top_b;
#endif /* SBA */
	}
#else
	if (ENV_YREG > (CELL *) B) {
	  ENV_YREG = (CELL *) B;
	}
#endif /* FROZEN_STACKS */
	BEGD(d0);
	d0 = ARG1;
      restart_execute2:
	deref_head(d0, execute2_unk);
      execute2_nvar:
	if (IsApplTerm(d0)) {
	  Functor f = FunctorOfTerm(d0);
	  if (IsExtensionFunctor(f)) {
	    goto execute2_metacall;
	  }
	  pen = RepPredProp(PredPropByFunc(f, mod));
	  if (pen->PredFlags & (MetaPredFlag|GoalExPredFlag)) {
	    if (f == FunctorModule) {
	      Term tmod = ArgOfTerm(1,d0);
	      if (!IsVarTerm(tmod) && IsAtomTerm(tmod)) {
		d0 = ArgOfTerm(2,d0);
		mod = tmod;
		goto execute2_nvar;
	      }
	    } else if (f == FunctorComma) {
	      SREG = RepAppl(d0);
	      BEGD(d1);
	      d1 = SREG[2];
	      /* create an to execute2 the call */
	      deref_head(d1, execute2_comma_unk);
	    execute2_comma_nvar:
	      if (IsAtomTerm(d1)) {
		ENV_YREG[-EnvSizeInCells-2]  = MkIntegerTerm((Int)PredPropByAtom(AtomOfTerm(d1),mod));
		ENV_YREG[-EnvSizeInCells-3]  = mod;
	      } else if (IsApplTerm(d1)) {
		Functor f = FunctorOfTerm(d1);
		if (IsExtensionFunctor(f)) {
		  goto execute2_metacall;
		} else {
		  if (f == FunctorModule) goto execute2_metacall;
		  ENV_YREG[-EnvSizeInCells-2]  = MkIntegerTerm((Int)PredPropByFunc(f,mod));
		  ENV_YREG[-EnvSizeInCells-3]  = mod;
		}
	      } else {
		goto execute2_metacall;
	      }
	      ENV_YREG[E_CP] = (CELL)NEXTOP(PREG,Osbpp);
	      ENV_YREG[E_CB] = (CELL)B;
	      ENV_YREG[E_E]  = (CELL)ENV;
#ifdef DEPTH_LIMIT
	      ENV_YREG[E_DEPTH] = DEPTH;
#endif	/* DEPTH_LIMIT */
	      ENV_YREG[-EnvSizeInCells-1]  = d1;
	      ENV = ENV_YREG;
	      ENV_YREG -= EnvSizeInCells+3;
	      PREG = COMMA_CODE;
	      /* for profiler */
	      save_pc();
	      d0 = SREG[1];
	      goto restart_execute2;

	      BEGP(pt1);
	      deref_body(d1, pt1, execute2_comma_unk, execute2_comma_nvar);
	      goto execute2_metacall;
	      ENDP(pt1);
	      ENDD(d1);
	    } else if (mod != CurrentModule) {
		goto execute2_metacall;
	    }
	  }
	  if (PRED_GOAL_EXPANSION_ALL) {
	    goto execute2_metacall;
	  }

	  BEGP(pt1);
	  pt1 = RepAppl(d0);
	  BEGD(d2);
	  for (d2 = ArityOfFunctor(f); d2; d2--) {
#ifdef SBA
	    BEGD(d1);
	    d1 = pt1[d2];
	    if (d1 == 0) {
	      XREGS[d2] = (CELL)(pt1+d2);
	    } else {
	      XREGS[d2] = d1;
	    }
#else
	    XREGS[d2] = pt1[d2];
#endif
	  }
	  ENDD(d2);
	  ENDP(pt1);
	  CACHE_A1();
	} else if (IsAtomTerm(d0)) {
	  if (PRED_GOAL_EXPANSION_ALL) {
	    goto execute2_metacall;
	  } else {
	    pen = RepPredProp(PredPropByAtom(AtomOfTerm(d0), mod));
	  }
	} else {
	  goto execute2_metacall;
	}

      execute2_end:
	/* code copied from call */
#ifndef NO_CHECKING
	check_stack(NoStackPExecute2, H);
#endif
	CPREG = NEXTOP(PREG, Osbpp);
	ALWAYS_LOOKAHEAD(pen->OpcodeOfPred);
	PREG = pen->CodeOfPred;
	/* for profiler */
	save_pc();
#ifdef DEPTH_LIMIT
	if (DEPTH <= MkIntTerm(1)) {/* I assume Module==0 is primitives */
	  if (pen->ModuleOfPred) {
	    if (DEPTH == MkIntTerm(0))
	      FAIL();
	    else DEPTH = RESET_DEPTH();
	  }
	} else if (pen->ModuleOfPred)
	  DEPTH -= MkIntConstant(2);
#endif	/* DEPTH_LIMIT */
#ifdef LOW_LEVEL_TRACER
	if (Yap_do_low_level_trace)
	  low_level_trace(enter_pred,pen,XREGS+1);
#endif	/* LOW_LEVEL_TRACER */
	WRITEBACK_Y_AS_ENV();
	/* setup GB */
	ENV_YREG[E_CB] = (CELL) B;
#ifdef YAPOR
	SCH_check_requests();
#endif	/* YAPOR */
	CACHE_A1();
	ALWAYS_GONext();
	ALWAYS_END_PREFETCH();

	BEGP(pt1);
	deref_body(d0, pt1, execute2_unk, execute2_nvar);
       execute2_metacall:
	ARG1 = ARG3 = d0;
	pen = PredMetaCall;
	ARG2 = Yap_cp_as_integer(B);
	if (mod)
	  ARG4 = mod;
	else
	  ARG4 = TermProlog;
	goto execute2_end;
	ENDP(pt1);

	ENDD(d0);
      NoStackPExecute2:
	PP = PredMetaCall;
	SREG = (CELL *) pen;
	ASP = ENV_YREG;
	/* setup GB */
	WRITEBACK_Y_AS_ENV();
	YREG[E_CB] = (CELL) B;
	if (ActiveSignals) {
	  goto creep_pe;
	}
	saveregs_and_ycache();
	if (!Yap_gc(((PredEntry *)SREG)->ArityOfPE, ENV, NEXTOP(PREG, Osbpp))) {
	  Yap_Error(OUT_OF_STACK_ERROR,TermNil,Yap_ErrorMessage);
	}
	setregs_and_ycache();
	goto execute2_end;
	ENDCACHE_Y_AS_ENV();

	BEGP(pt1);
	deref_body(mod, pt1, execute2_unk0, execute2_nvar0);
	saveregs();
	Yap_Error(INSTANTIATION_ERROR, mod, "call/2");
	setregs();
	ENDP(pt1);
	/* Oops, second argument was unbound too */
	FAIL();
      }
      ENDBOp();

      BOp(p_execute, Osbmp);
      { 
	PredEntry *pen;
	Term mod = PREG->u.Osbmp.mod;

	CACHE_Y_AS_ENV(YREG);
	/* Try to preserve the environment */
	ENV_YREG = (CELL *) (((char *) YREG) + PREG->u.Osbmp.s);
#ifdef FROZEN_STACKS
	{ 
	  choiceptr top_b = PROTECT_FROZEN_B(B);
#ifdef SBA
	  if (ENV_YREG > (CELL *) top_b || ENV_YREG < H) ENV_YREG = (CELL *) top_b;
#else
	  if (ENV_YREG > (CELL *) top_b) ENV_YREG = (CELL *) top_b;
#endif /* SBA */
	}
#else
	if (ENV_YREG > (CELL *) B) {
	  ENV_YREG = (CELL *) B;
	}
#endif /* FROZEN_STACKS */
	BEGD(d0);
	d0 = ARG1;
      restart_execute:
	deref_head(d0, execute_unk);
      execute_nvar:
	if (IsApplTerm(d0)) {
	  Functor f = FunctorOfTerm(d0);
	  if (IsExtensionFunctor(f)) {
	    goto execute_metacall;
	  }
	  pen = RepPredProp(PredPropByFunc(f, mod));
	  if (pen->PredFlags & (MetaPredFlag|GoalExPredFlag)) {
	    if (f == FunctorModule) {
	      Term tmod = ArgOfTerm(1,d0);
	      if (!IsVarTerm(tmod) && IsAtomTerm(tmod)) {
		d0 = ArgOfTerm(2,d0);
		mod = tmod;
		goto execute_nvar;
	      }
	    } else if (f == FunctorComma) {
	      SREG = RepAppl(d0);
	      BEGD(d1);
	      d1 = SREG[2];
	      /* create an to execute the call */
	      deref_head(d1, execute_comma_unk);
	    execute_comma_nvar:
	      if (IsAtomTerm(d1)) {
		ENV_YREG[-EnvSizeInCells-2]  = MkIntegerTerm((Int)PredPropByAtom(AtomOfTerm(d1),mod));
		ENV_YREG[-EnvSizeInCells-3]  = mod;
	      } else if (IsApplTerm(d1)) {
		f = FunctorOfTerm(d1);
		if (IsExtensionFunctor(f)) {
		  goto execute_metacall;
		} else {
		  if (f == FunctorModule) goto execute_metacall;
		  ENV_YREG[-EnvSizeInCells-2]  = MkIntegerTerm((Int)PredPropByFunc(f,mod));
		  ENV_YREG[-EnvSizeInCells-3]  = mod;
		}
	      } else {
		goto execute_metacall;
	      }
	      ENV_YREG[E_CP] = (CELL)NEXTOP(PREG,Osbmp);
	      ENV_YREG[E_CB] = (CELL)B;
	      ENV_YREG[E_E]  = (CELL)ENV;
#ifdef DEPTH_LIMIT
	      ENV_YREG[E_DEPTH] = DEPTH;
#endif	/* DEPTH_LIMIT */
	      ENV_YREG[-EnvSizeInCells-1]  = d1;
	      ENV = ENV_YREG;
	      ENV_YREG -= EnvSizeInCells+3;
	      PREG = COMMA_CODE;
	      /* for profiler */
	      save_pc();
	      d0 = SREG[1];
	      goto restart_execute;

	      BEGP(pt1);
	      deref_body(d1, pt1, execute_comma_unk, execute_comma_nvar);
	      goto execute_metacall;
	      ENDP(pt1);
	      ENDD(d1);
	    } else if (mod != CurrentModule) {
		goto execute_metacall;
	    }
	  }
	  if (PRED_GOAL_EXPANSION_ALL) {
	    goto execute_metacall;
	  }

	  BEGP(pt1);
	  pt1 = RepAppl(d0);
	  BEGD(d2);
	  for (d2 = ArityOfFunctor(f); d2; d2--) {
#ifdef SBA
	    BEGD(d1);
	    d1 = pt1[d2];
	    if (d1 == 0) {
	      XREGS[d2] = (CELL)(pt1+d2);
	    } else {
	      XREGS[d2] = d1;
	    }
#else
	    XREGS[d2] = pt1[d2];
#endif
	  }
	  ENDD(d2);
	  ENDP(pt1);
	  CACHE_A1();
	} else if (IsAtomTerm(d0)) {
	  if (PRED_GOAL_EXPANSION_ALL) {
	    goto execute_metacall;
	  } else {
	    pen = RepPredProp(PredPropByAtom(AtomOfTerm(d0), mod));
	  }
	} else {
	  goto execute_metacall;
	}

      execute_end:
	/* code copied from call */
#ifndef NO_CHECKING
	check_stack(NoStackPExecute, H);
#endif
	CPREG = NEXTOP(PREG, Osbmp);
	ALWAYS_LOOKAHEAD(pen->OpcodeOfPred);
	PREG = pen->CodeOfPred;
	/* for profiler */
	save_pc();
#ifdef DEPTH_LIMIT
	if (DEPTH <= MkIntTerm(1)) {/* I assume Module==0 is primitives */
	  if (pen->ModuleOfPred) {
	    if (DEPTH == MkIntTerm(0))
	      FAIL();
	    else DEPTH = RESET_DEPTH();
	  }
	} else if (pen->ModuleOfPred)
	  DEPTH -= MkIntConstant(2);
#endif	/* DEPTH_LIMIT */
#ifdef LOW_LEVEL_TRACER
	if (Yap_do_low_level_trace)
	  low_level_trace(enter_pred,pen,XREGS+1);
#endif	/* LOW_LEVEL_TRACER */
	WRITEBACK_Y_AS_ENV();
	/* setup GB */
	ENV_YREG[E_CB] = (CELL) B;
#ifdef YAPOR
	SCH_check_requests();
#endif	/* YAPOR */
	CACHE_A1();
	ALWAYS_GONext();
	ALWAYS_END_PREFETCH();

	BEGP(pt1);
	deref_body(d0, pt1, execute_unk, execute_nvar);
       execute_metacall:
	ARG1 = ARG3 = d0;
	pen = PredMetaCall;
	ARG2 = Yap_cp_as_integer(B);
	if (mod)
	  ARG4 = mod;
	else
	  ARG4 = TermProlog;
	goto execute_end;
	ENDP(pt1);

	ENDD(d0);
      NoStackPExecute:
	PP = PredMetaCall;
	SREG = (CELL *) pen;
	ASP = ENV_YREG;
	/* setup GB */
	WRITEBACK_Y_AS_ENV();
	YREG[E_CB] = (CELL) B;
	if (ActiveSignals) {
	  goto creep_pe;
	}
	saveregs_and_ycache();
	if (!Yap_gc(((PredEntry *)SREG)->ArityOfPE, ENV, NEXTOP(PREG, Osbmp))) {
	  Yap_Error(OUT_OF_STACK_ERROR,TermNil,Yap_ErrorMessage);
	}
	setregs_and_ycache();
	goto execute_end;
	ENDCACHE_Y_AS_ENV();
      }
      ENDBOp();

    creep_pe:			/* do creep in call                                     */
      CPREG = NEXTOP(PREG, Osbmp);
      goto creep;

      BOp(p_execute_tail, Osbpp);
      {
	PredEntry *pen;
	Term mod;
	UInt arity;

	CACHE_Y_AS_ENV(YREG);
	BEGP(pt0);
	BEGD(d0);
	d0 = ENV_YREG[-EnvSizeInCells-1];
	pen = RepPredProp((Prop)IntegerOfTerm(ENV_YREG[-EnvSizeInCells-2]));
	CPREG = (yamop *) ENV_YREG[E_CP];
	pt0 = ENV_YREG;
	ENV_YREG = ENV = (CELL *) ENV_YREG[E_E];
#ifdef FROZEN_STACKS
	{ 
	  choiceptr top_b = PROTECT_FROZEN_B(B);

#ifdef SBA
	  if (ENV_YREG > (CELL *) top_b || ENV_YREG < H) ENV_YREG = (CELL *) top_b;
#else
	  if (ENV_YREG > (CELL *) top_b) ENV_YREG = (CELL *) top_b;
#endif /* SBA */
	  else ENV_YREG = (CELL *)((CELL)ENV_YREG + ENV_Size(CPREG));
	}
#else
	if (ENV_YREG > (CELL *)B) {
	  ENV_YREG = (CELL *)B;
	} else {
	  ENV_YREG = (CELL *) ((CELL) ENV_YREG+ ENV_Size(CPREG));
	}
#endif /* FROZEN_STACKS */
	arity = pen->ArityOfPE;
	if (pen->PredFlags & (MetaPredFlag|GoalExPredFlag)) {
	  mod = pt0[-EnvSizeInCells-3];
	  if (pen->FunctorOfPred == FunctorComma) {
	    SREG = RepAppl(d0);
	    BEGD(d1);
	    d1 = SREG[2];
	  execute_comma_comma:
	    /* create an to execute the call */
	    deref_head(d1, execute_comma_comma_unk);
	  execute_comma_comma_nvar:
	    ENV_YREG[E_CB] = pt0[E_CB];
	    if (IsAtomTerm(d1)) {
	      ENV_YREG[-EnvSizeInCells-2]  = MkIntegerTerm((Int)PredPropByAtom(AtomOfTerm(d1),mod));
	    } else if (IsApplTerm(d1)) {
	      Functor f = FunctorOfTerm(d1);
	      if (IsExtensionFunctor(f)) {
		goto execute_metacall_after_comma;
	      } else if (f == FunctorModule) {
		Term tmod = ArgOfTerm(1, d1);
		if (IsVarTerm(tmod) || !IsAtomTerm(tmod))
		  goto execute_metacall_after_comma;
		mod = tmod;
		d1 = RepAppl(d1)[2];
		goto execute_comma_comma;
	      } else {
		ENV_YREG[-EnvSizeInCells-2]  = MkIntegerTerm((Int)PredPropByFunc(f,mod));
	      }
	    } else {
	      goto execute_metacall_after_comma;
	    }
	    ENV_YREG[E_CP] = (CELL)CPREG;
	    ENV_YREG[E_E]  = (CELL)ENV;
#ifdef DEPTH_LIMIT
	    ENV_YREG[E_DEPTH] = DEPTH;
#endif	/* DEPTH_LIMIT */
	    ENV_YREG[-EnvSizeInCells-1]  = d1;
	    ENV_YREG[-EnvSizeInCells-3]  = mod;
	    ENV = ENV_YREG;
	    ENV_YREG -= EnvSizeInCells+3;
	    d0 = SREG[1];
	    CPREG = NEXTOP(COMMA_CODE,Osbpp);
	  execute_comma_comma2:
	    /* create an to execute the call */
	    deref_head(d0, execute_comma_comma2_unk);
	  execute_comma_comma2_nvar:
	    if (IsAtomTerm(d0)) {
	      Atom at = AtomOfTerm(d0);
	      arity = 0;
	      if (at == AtomCut) {
		choiceptr cut_pt = (choiceptr)pt0[E_CB];
#ifdef CUT_C
		{
		  if (SHOULD_CUT_UP_TO(B,(choiceptr) cut_pt))
		    {
		      while (POP_CHOICE_POINT(cut_pt))
			{
			  POP_EXECUTE();
			}
		    }
		}
#endif /* CUT_C */
#ifdef YAPOR
		CUT_prune_to(cut_pt);
#endif /* YAPOR */
		/* find where to cut to */
		if (SHOULD_CUT_UP_TO(B,cut_pt)) {
		  /* Wow, we're gonna cut!!! */
#ifdef TABLING
		  while (B->cp_b < cut_pt) {
		    B = B->cp_b;
		  }
		  abolish_incomplete_subgoals(B);
#endif /* TABLING */
		  B = cut_pt;
		  HB = PROTECT_FROZEN_H(B);
		}
	      }
	      pen = RepPredProp(PredPropByAtom(at, mod));
	      goto execute_comma;
	    } else if (IsApplTerm(d0)) {
	      Functor f = FunctorOfTerm(d0);
	      if (IsExtensionFunctor(f) || f == FunctorModule) {
		Term tmod = ArgOfTerm(1, d0);
		if (IsVarTerm(tmod) || !IsAtomTerm(tmod))
		  goto execute_metacall_after_comma;
		mod = tmod;
		d0 = RepAppl(d0)[2];
		goto execute_comma_comma2;
	      } else {
		pen = RepPredProp(PredPropByFunc(f,mod));
		if (pen->PredFlags & (MetaPredFlag|GoalExPredFlag)) {
		  goto execute_metacall_after_comma;
		}
		arity = pen->ArityOfPE;
		goto execute_comma;
	      }
	    } else {
	      if (mod != CurrentModule)
		goto execute_metacall_after_comma;
	      else {
		arity = pen->ArityOfPE;
		goto execute_comma;
	      }
	    }

	    BEGP(pt1);
	    deref_body(d0, pt1, execute_comma_comma2_unk, execute_comma_comma2_nvar);
	    goto execute_metacall_after_comma;
	    ENDP(pt1);

	    BEGP(pt1);
	    deref_body(d1, pt1, execute_comma_comma_unk, execute_comma_comma_nvar);
	    goto execute_metacall_after_comma;
	    ENDP(pt1);
	    ENDD(d1);
	  } else {
	    if (mod != CurrentModule) {
	      execute_metacall_after_comma:
	      ARG1 = ARG3 = d0;
	      pen = PredMetaCall;
	      ARG2 = Yap_cp_as_integer((choiceptr)pt0[E_CB]);
	      if (mod)
		ARG4 = mod;
	      else
		ARG4 = TermProlog;
	      CACHE_A1();
	      goto execute_after_comma;
	    }
	  }
	}
      execute_comma:
	if (arity) {
	  BEGP(pt1);
	  pt1 = RepAppl(d0);
	  BEGD(d2);
	  for (d2 = arity; d2; d2--) {
#ifdef SBA
	    BEGD(d1);
	    d1 = pt1[d2];
	    if (d1 == 0)
	      XREGS[d2] = (CELL)(pt1+d2);
	    else
	      XREGS[d2] = d1;
#else
	    XREGS[d2] = pt1[d2];
#endif
	  }
	  ENDD(d2);
	  ENDP(pt1);
	  CACHE_A1();
	} else if ((Atom)(pen->FunctorOfPred) == AtomCut) {
	  choiceptr cut_pt = (choiceptr)pt0[E_CB];
#ifdef CUT_C
	  {
	    if (SHOULD_CUT_UP_TO(B,(choiceptr) cut_pt))
	      {
		while (POP_CHOICE_POINT(cut_pt))
		  {
		    POP_EXECUTE();
		  }
	      }
	  }
#endif /* CUT_C */
#ifdef YAPOR
	  CUT_prune_to(cut_pt);
#endif /* YAPOR */
	  /* find where to cut to */
	  if (SHOULD_CUT_UP_TO(B,cut_pt)) {
	    /* Wow, we're gonna cut!!! */
#ifdef TABLING
	    while (B->cp_b < cut_pt) {
	      B = B->cp_b;
	    }
	    abolish_incomplete_subgoals(B);
#endif /* TABLING */
	    B = cut_pt;
	    HB = PROTECT_FROZEN_H(B);
	  }
	}

      execute_after_comma:
#ifndef NO_CHECKING
	check_stack(NoStackPTExecute, H);
#endif
	PREG = pen->CodeOfPred;
	/* for profiler */
	save_pc();
	ALWAYS_LOOKAHEAD(pen->OpcodeOfPred);
	ENV_YREG[E_CB] = (CELL)B;
#ifdef LOW_LEVEL_TRACER
	if (Yap_do_low_level_trace)
	  low_level_trace(enter_pred,pen,XREGS+1);
#endif	/* LOW_LEVEL_TRACER */
#ifdef DEPTH_LIMIT
	if (DEPTH <= MkIntTerm(1)) {/* I assume Module==0 is primitives */
	  if (pen->ModuleOfPred) {
	    if (DEPTH == MkIntTerm(0))
	      FAIL();
	    else DEPTH = RESET_DEPTH();
	  }
	} else if (pen->ModuleOfPred) {
	  DEPTH -= MkIntConstant(2);
	}
#endif	/* DEPTH_LIMIT */
	/* do deallocate */
	WRITEBACK_Y_AS_ENV();
	ALWAYS_GONext();
	ALWAYS_END_PREFETCH();

	ENDD(d0);
	ENDP(pt0);
      NoStackPTExecute:
	PP = NULL;
	WRITEBACK_Y_AS_ENV();
	SREG = (CELL *) pen;
	ASP = ENV_YREG;
	if (ASP > (CELL *)PROTECT_FROZEN_B(B))
	  ASP = (CELL *)PROTECT_FROZEN_B(B);
	LOCK(SignalLock);
	if (ActiveSignals & YAP_CDOVF_SIGNAL) {
	  UNLOCK(SignalLock);
	  saveregs_and_ycache();
	  if (!Yap_growheap(FALSE, 0, NULL)) {
	    Yap_Error(OUT_OF_HEAP_ERROR, TermNil, "YAP failed to grow heap: %s", Yap_ErrorMessage);
	    setregs_and_ycache();
	    FAIL();
	  }
	  setregs_and_ycache();
	  LOCK(SignalLock);
	  ActiveSignals &= ~YAP_CDOVF_SIGNAL;
	  CreepFlag = CalculateStackGap();
	  if (!ActiveSignals) {
	    UNLOCK(SignalLock);
	    goto execute_after_comma;
	  }
	}
	if (ActiveSignals & YAP_TROVF_SIGNAL) {
	  UNLOCK(SignalLock);
#ifdef SHADOW_S
	  S = SREG;
#endif
	  saveregs_and_ycache();
	  if(!Yap_growtrail (0, FALSE)) {
	    Yap_Error(OUT_OF_TRAIL_ERROR,TermNil,"YAP failed to reserve %ld bytes in growtrail",sizeof(CELL) * 16 * 1024L);
	    setregs_and_ycache();
	    FAIL();
	  }
	  setregs_and_ycache();
	  ActiveSignals &= ~YAP_TROVF_SIGNAL;
	  CreepFlag = CalculateStackGap();
	  if (!ActiveSignals) {
	    UNLOCK(SignalLock);
	    goto execute_after_comma;
	  }
	}
	if (ActiveSignals) {
	  if (ActiveSignals & YAP_CDOVF_SIGNAL) {
	    UNLOCK(SignalLock);
	    goto noheapleft;
	  }
	  UNLOCK(SignalLock);
	  goto creep;
	}
	UNLOCK(SignalLock);
	saveregs_and_ycache();
	if (!Yap_gc(((PredEntry *)SREG)->ArityOfPE, ENV, NEXTOP(PREG, Osbpp))) {
	  Yap_Error(OUT_OF_STACK_ERROR,TermNil,Yap_ErrorMessage);
	}
	setregs_and_ycache();
	goto execute_after_comma;
	ENDCACHE_Y_AS_ENV();

      }
      ENDBOp();

#if !USE_THREADED_CODE
    default:
      saveregs();
      Yap_Error(SYSTEM_ERROR, MkIntegerTerm(opcode), "trying to execute invalid YAAM instruction %d", opcode);
      setregs();
      FAIL();
    }
  }
#endif

#if USE_THREADED_CODE
#if PUSH_REGS
  restore_absmi_regs(old_regs);
#endif
#if BP_FREE
  P1REG = PCBACKUP;
#endif
  return (0);
#endif

}

/* dummy function that is needed for profiler */
int Yap_absmiEND(void)
{
  return 1;
}
