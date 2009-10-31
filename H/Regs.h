/*************************************************************************
*									 *
*	 YAP Prolog 	%W% %G% 					 *
*	Yap Prolog was developed at NCCUP - Universidade do Porto	 *
*									 *
* Copyright L.Damas, V.S.Costa and Universidade do Porto 1985-1997	 *
*									 *
**************************************************************************
*									 *
* File:		Regs.h							 *
* mods:									 *
* comments:	YAP abstract machine registers				 *
* version:      $Id: Regs.h,v 1.42 2008-08-12 01:27:22 vsc Exp $	 *
*************************************************************************/


/*********  abstract machine registers **********************************/
#ifdef CUT_C
#include "cut_c.h"
#endif

#if defined MYDDAS_ODBC || defined MYDDAS_MYSQL
#include "myddas.h"
#endif

#define MaxTemps	512
#define MaxArithms	32

#ifdef i386
#define PUSH_REGS 1
#undef  PUSH_X
#endif

#ifdef __x86_64__
#undef  PUSH_REGS
#undef  PUSH_X
#endif

#if defined(sparc) || defined(__sparc)
#undef  PUSH_REGS
#undef  PUSH_X
#endif

#ifdef __alpha
#undef  PUSH_REGS
#undef  PUSH_X
#endif

#if defined(_POWER) || defined(__POWERPC__)
#undef  PUSH_REGS
#undef  PUSH_X
#endif

#ifdef hppa
#undef  PUSH_REGS
#undef  PUSH_X
#endif

#ifdef mips
#undef  PUSH_REGS
#undef  PUSH_X
#endif

/* force a cache of WAM regs for multi-threaded architectures! */
#ifdef THREADS
#ifndef PUSH_REGS
#define PUSH_REGS 1
#endif
#ifndef PUSH_X
#define PUSH_X 1
#endif
#endif

EXTERN void restore_machine_regs(void);
EXTERN void save_machine_regs(void);
EXTERN void restore_H(void);
EXTERN void save_H(void);
EXTERN void restore_B(void);
EXTERN void save_B(void);

typedef struct
  {
    CELL    CreepFlag_;		/* 13                                         */
    CELL   *HB_;		/* 4 heap (global) stack top at latest c.p.   */
#if defined(SBA) || defined(TABLING)
    choiceptr BB_;		/* 4 local stack top at latest c.p.   */
#endif /* SBA || TABLING */
    CELL  *H0_;			/* 2 base of heap (global) stack              */
    tr_fr_ptr TR_;		/* 24 top of trail                            */
    CELL   *H_;			/* 25 top of heap (global)  stack             */
    choiceptr B_;		/* 26 latest choice point                     */
#ifdef CUT_C
    cut_c_str_ptr CUT_C_TOP;
#endif
#if defined CUT_C && (defined MYDDAS_ODBC || defined MYDDAS_MYSQL)
    MYDDAS_GLOBAL MYDDAS_GLOBAL_POINTER;
#endif
#ifdef  DEPTH_LIMIT
    CELL   DEPTH_;		/* 27                                         */
#endif  /* DEPTH_LIMIT */
    yamop *CP_;			/* 28 continuation program counter            */
    yamop *P_;			/* 7 prolog machine program counter           */
    CELL  *YENV_;		/* 5 current environment (may differ from ENV)*/
    CELL  *S_;			/* 6 structure pointer                        */
    CELL  *ENV_;		/* 1 current environment                      */
    CELL  *ASP_;		/* 8 top of local       stack                 */
    CELL  *LCL0_;		/* 3 local stack base                         */
    ADDR   AuxBase_;		/* 9 Auxiliary base  pointer                  */
    CELL  *AuxSp_;		/* 9 Auxiliary stack pointer                  */
    ADDR   AuxTop_;		/* 10 Auxiliary stack top                     */
/* visualc*/
    CELL   EX_;	    	        /* 18                                         */
#ifdef COROUTINING
    Term  DelayedVars_;         /* maximum number of attributed variables     */
#endif
    Term  CurrentModule_;
#if defined(SBA) || defined(TABLING)
    CELL *H_FZ_;
    choiceptr B_FZ_;
    tr_fr_ptr TR_FZ_;
#endif /* SBA || TABLING */
    struct pred_entry *PP_;
#if defined(YAPOR) || defined(THREADS)
    /* recursive write-locks for PredEntry */
    yamop **PREG_ADDR_;
    unsigned int worker_id_;
#ifdef SBA
    choiceptr BSEG_;
    struct or_frame *frame_head_, *frame_tail_;
    char *binding_array_;
    int  sba_offset_;
    int  sba_end_;
    int  sba_size_;
#endif /* SBA */
#endif /* YAPOR || THREADS */
#if PUSH_REGS
    /* On a X86 machine, the best solution is to keep the
       X registers on a global variable, whose address is known between
       runs, and to push the remaining registers to the stack.

       On a register based machine, one can have a dedicated register,
       always pointing to the XREG global variable. This spends an
       extra register, but makes it easier to access X[1].
     */

#ifdef PUSH_X
    Term XTERMS[MaxTemps];	/* 29                                    */
#endif
    yamop *ARITH_EXCEPTION_;
    int isint_[MaxArithms];
    Int Ints_[MaxArithms];
    Float Floats_[MaxArithms];
  }
REGSTORE;

extern REGSTORE *Yap_regp;

#ifdef PUSH_X

#define XREGS  (Yap_REGS.XTERMS)

#else

/* keep X as a global variable */

extern Term Yap_XREGS[MaxTemps];	/* 29                                     */

#define XREGS Yap_XREGS

#endif

#ifdef THREADS

extern pthread_key_t Yap_yaamregs_key;

#define Yap_regp ((REGSTORE *)pthread_getspecific(Yap_yaamregs_key))

#endif

#define Yap_REGS (*Yap_regp)

#else /* !PUSH_REGS */

    Term X[MaxTemps];		/* 29                                     */

#define XREGS	  Yap_REGS.X
  }
REGSTORE;

extern REGSTORE Yap_REGS;
#endif /* PUSH_REGS */

#define MinTrailGap (sizeof(CELL)*1024)
#define MinHeapGap  (sizeof(CELL)*4096)
#define MinStackGap (sizeof(CELL)*8*1024)
extern int Yap_stack_overflows;


#define ENV  Yap_REGS.ENV_	/* current environment                    */
#define ASP  Yap_REGS.ASP_	/* top of local   stack                   */
#define H0   Yap_REGS.H0_	/* base of heap (global) stack            */
#define LCL0 Yap_REGS.LCL0_	/* local stack base                       */

#define YENV2MEM
#define YENV_ADDRESS (&(YENV))
#define MEM2YENV  

#if defined(__GNUC__) && defined(sparc) && !defined(__NetBSD__) && !defined(THREADS)

#define P    Yap_REGS.P_		/* prolog machine program counter         */
#define YENV Yap_REGS.YENV_	/* current environment (may differ from   ENV)*/
#define S    Yap_REGS.S_		/* structure pointer                      */

register CELL *H asm ("g5");
#define TR         Yap_REGS.TR_	/* latest choice point            */
#define  B         Yap_REGS.B_	/* latest choice point            */
#define CP         Yap_REGS.CP_	/* continuation   program counter         */
#define HB         Yap_REGS.HB_	/* heap (global) stack top at time of latest c.p. */
#define CreepFlag  Yap_REGS.CreepFlag_

EXTERN inline void save_machine_regs(void) {
  Yap_REGS.H_   = H;
}

EXTERN inline void restore_machine_regs(void) {
  H = Yap_REGS.H_;
}

#define BACKUP_MACHINE_REGS()           \
  CELL     *BK_H = H;                   \
  restore_machine_regs()

#define RECOVER_MACHINE_REGS()          \
  save_machine_regs();                  \
  H = BK_H

EXTERN inline void save_H(void) {
  Yap_REGS.H_   = H;
}

EXTERN inline void restore_H(void) {
  H = Yap_REGS.H_;
}

#define BACKUP_H()  CELL *BK_H = H;  restore_H()

#define RECOVER_H()   save_H(); H = BK_H

EXTERN inline void save_B(void) {
}

EXTERN inline void restore_B(void) {
}

#if defined(__svr4__)
#define BACKUP_B()

#define RECOVER_B()
#else
#define BACKUP_B()  

#define RECOVER_B()   
#endif

#define restore_TR() 

#elif defined(__GNUC__) && defined(__alpha)

#define P               Yap_REGS.P_	/* prolog machine program counter */
#define YENV            Yap_REGS.YENV_	/* current environment (may differ from ENV) */
register CELL *H asm ("$9");
register CELL *HB asm ("$10");
register choiceptr B asm ("$11");
register yamop *CP asm ("$12");
register CELL *S asm ("$13");
register tr_fr_ptr TR asm ("$14");
/* gcc+debug chokes if $15 is in use on alphas */
#ifdef DEBUG
#define CreepFlag Yap_REGS.CreepFlag_
#else
register CELL CreepFlag asm ("$15");
#endif

/* Interface with foreign code, make sure the foreign code sees all the
   registers the way they used to be */
EXTERN inline void save_machine_regs(void) {
  Yap_REGS.H_   = H;
  Yap_REGS.HB_ = HB;
  Yap_REGS.B_   = B;
  Yap_REGS.CP_ = CP;
#ifndef DEBUG
  Yap_REGS.CreepFlag_ = CreepFlag;
#endif
  Yap_REGS.TR_  = TR;
}

EXTERN inline void restore_machine_regs(void) {
  H = Yap_REGS.H_;
  HB = Yap_REGS.HB_;
  B = Yap_REGS.B_;
  CP = Yap_REGS.CP_;
#ifndef DEBUG
  CreepFlag = Yap_REGS.CreepFlag_;
#endif
  TR = Yap_REGS.TR_;
}

#define BACKUP_MACHINE_REGS()           \
  CELL     *BK_H = H;                   \
  CELL     *BK_HB = HB;                 \
  choiceptr BK_B = B;                   \
  CELL      BK_CreepFlag = CreepFlag;   \
  yamop    *BK_CP = CP;                 \
  tr_fr_ptr BK_TR = TR;                 \
  restore_machine_regs()

#define RECOVER_MACHINE_REGS()          \
  save_machine_regs();                  \
  H = BK_H;                             \
  HB = BK_HB;                           \
  B = BK_B;                             \
  CreepFlag = BK_CreepFlag;             \
  CP = BK_CP;                           \
  TR = BK_TR

EXTERN inline void save_H(void) {
  Yap_REGS.H_   = H;
}

EXTERN inline void restore_H(void) {
  H = Yap_REGS.H_;
}

#define BACKUP_H()  CELL *BK_H = H; restore_H()

#define RECOVER_H()  save_H(); H = BK_H

EXTERN inline void save_B(void) {
  Yap_REGS.B_   = B;
}

EXTERN inline void restore_B(void) {
  B = Yap_REGS.B_;
}

#define BACKUP_B()  choiceptr BK_B = B; restore_B()

#define RECOVER_B()  save_B(); B = BK_B

EXTERN void restore_TR(void);
EXTERN void save_TR(void);

EXTERN inline void save_TR(void) {
  Yap_REGS.TR_ = TR;
}

EXTERN inline void restore_TR(void) {
  TR = Yap_REGS.TR_;
}

#elif defined(__GNUC__) && defined(mips)

#define P               Yap_REGS.P_	/* prolog machine program counter */
#define YENV            Yap_REGS.YENV_	/* current environment (may differ from ENV) */
register CELL *H asm ("$16");
register CELL *HB asm ("$17");
register choiceptr B asm ("$18");
register yamop *CP asm ("$19");
register CELL *S asm ("$20");
register CELL CreepFlag asm ("$21");
register tr_fr_ptr TR asm ("$22");

EXTERN inline void save_machine_regs(void) {
  Yap_REGS.H_   = H;
  Yap_REGS.HB_ = HB;
  Yap_REGS.B_   = B;
  Yap_REGS.CP_  = CP;
  Yap_REGS.CreepFlag_ = CreepFlag;
  Yap_REGS.TR_  = TR;
}

EXTERN inline void restore_machine_regs(void) {
  H = Yap_REGS.H_;
  HB = Yap_REGS.HB_;
  B = Yap_REGS.B_;
  CP = Yap_REGS.CP_;
  CreepFlag = Yap_REGS.CreepFlag_;
  TR = Yap_REGS.TR_;
}

#define BACKUP_MACHINE_REGS()           \
  CELL     *BK_H = H;                   \
  CELL     *BK_HB = HB;                 \
  choiceptr BK_B = B;                   \
  CELL      BK_CreepFlag = CreepFlag;   \
  yamop    *BK_CP = CP;                 \
  tr_fr_ptr BK_TR = TR;                 \
  restore_machine_regs()

#define RECOVER_MACHINE_REGS()          \
  save_machine_regs();                  \
  H = BK_H;                             \
  HB = BK_HB;                           \
  B = BK_B;                             \
  CreepFlag = BK_CreepFlag;             \
  CP = BK_CP;                           \
  TR = BK_TR

EXTERN inline void save_H(void) {
  Yap_REGS.H_   = H;
}

EXTERN inline void restore_H(void) {
  H = Yap_REGS.H_;
}

#define BACKUP_H()  CELL *BK_H = H; restore_H()

#define RECOVER_H()  save_H(); H = BK_H

EXTERN inline void save_B(void) {
  Yap_REGS.B_ = B;
}

EXTERN inline void restore_B(void) {
  B = Yap_REGS.B_;
}

#define BACKUP_B()  choiceptr BK_B = B; restore_B()

#define RECOVER_B()  save_B(); B = BK_B

#define restore_TR() 

#elif defined(__GNUC__) && defined(hppa)

#define P               Yap_REGS.P_	/* prolog machine program counter */
#define YENV            Yap_REGS.YENV_	/* current environment (may differ from ENV) */
register CELL *H asm ("r12");
register CELL *HB asm ("r13");
register choiceptr B asm ("r14");
register yamop *CP asm ("r15");
register CELL *S asm ("r16");
register CELL CreepFlag asm ("r17");
register tr_fr_ptr TR asm ("r18");

EXTERN inline void save_machine_regs(void) {
  Yap_REGS.H_   = H;
  Yap_REGS.HB_ = HB;
  Yap_REGS.B_   = B;
  Yap_REGS.CP_  = CP;
  Yap_REGS.CreepFlag_ = CreepFlag;
  Yap_REGS.TR_  = TR;
}

EXTERN inline void restore_machine_regs(void) {
  H = Yap_REGS.H_;
  HB = Yap_REGS.HB_;
  B = Yap_REGS.B_;
  CP = Yap_REGS.CP_;
  CreepFlag = Yap_REGS.CreepFlag_;
  TR = Yap_REGS.TR_;
}

#define BACKUP_MACHINE_REGS()           \
  CELL     *BK_H = H;                   \
  CELL     *BK_HB = HB;                 \
  choiceptr BK_B = B;                   \
  CELL      BK_CreepFlag = CreepFlag;   \
  yamop    *BK_CP = CP;                 \
  tr_fr_ptr BK_TR = TR;                 \
  restore_machine_regs()

#define RECOVER_MACHINE_REGS()          \
  save_machine_regs();                  \
  H = BK_H;                             \
  HB = BK_HB;                           \
  B = BK_B;                             \
  CreepFlag = BK_CreepFlag;             \
  CP = BK_CP;                           \
  TR = BK_TR

EXTERN inline void save_H(void) {
  Yap_REGS.H_   = H;
}

EXTERN inline void restore_H(void) {
  H = Yap_REGS.H_;
}

#define BACKUP_H()  CELL *BK_H = H; restore_H()

#define RECOVER_H()  save_H(); H = BK_H

EXTERN inline void save_B(void) {
  Yap_REGS.B_ = B;
}

EXTERN inline void restore_B(void) {
  B = Yap_REGS.B_;
}

#define BACKUP_B()  choiceptr BK_B = B; restore_B()

#define RECOVER_B()  save_B(); B = BK_B

EXTERN void restore_TR(void);
EXTERN void save_TR(void);

EXTERN inline void save_TR(void) {
  Yap_REGS.TR_ = TR;
}

EXTERN inline void restore_TR(void) {
  TR = Yap_REGS.TR_;
}

#elif defined(__GNUC__) && (defined(_POWER)  || defined(__POWERPC__))

/* 

   Because of a bug in GCC, we should always start from the first available
   general register. According to rs6000.h, this is r13 everywhere
   except for svr4 machines, where r13 is fixed.

   If we don't do so, GCC will generate move multiple instructions for
   entering complex functions. These instructions will save and
   restore the global registers :-(.

 */
#define CreepFlag Yap_REGS.CreepFlag_
/* 
   vsc: this MUST start from register 13, otherwise GCC will break down in flames and
   use the registers for evil purposes. :-(
 */
#ifndef __svr4__
register tr_fr_ptr TR  asm ("r13");
#else
register tr_fr_ptr TR asm ("r21");
#endif
register CELL *H asm ("r14");
register CELL *HB asm ("r15");
register choiceptr B asm ("r16");
register yamop *CP asm ("r17");
register CELL *S asm ("r18");
register CELL *YENV asm ("r19");
#define P    Yap_REGS.P_		/* prolog machine program counter */

#undef YENV2MEM
#undef YENV_ADDRESS (&(YENV))
#undef MEM2YENV  

#define YENV2MEM     { CELL *YENV_MEM = YENV
#define YENV_ADDRESS   (&YENV_MEM)
#define MEM2YENV       YENV = YENV_MEM; }



EXTERN inline void save_machine_regs(void) {
  Yap_REGS.H_   = H;
  Yap_REGS.HB_ = HB;
  Yap_REGS.B_   = B;
  Yap_REGS.CP_ = CP;
  Yap_REGS.YENV_  = YENV;
  Yap_REGS.TR_  = TR;
}

EXTERN inline void restore_machine_regs(void) {
  H = Yap_REGS.H_;
  HB = Yap_REGS.HB_;
  B = Yap_REGS.B_;
  CP = Yap_REGS.CP_;
  YENV = Yap_REGS.YENV_;
  TR = Yap_REGS.TR_;
}

#define BACKUP_MACHINE_REGS()           \
  CELL     *BK_H = H;                   \
  CELL     *BK_HB = HB;                 \
  choiceptr BK_B = B;                   \
  yamop    *BK_CP = CP;                 \
  tr_fr_ptr BK_TR = TR;                 \
  restore_machine_regs()

#define RECOVER_MACHINE_REGS()          \
  save_machine_regs();                  \
  H = BK_H;                             \
  HB = BK_HB;                           \
  B = BK_B;                             \
  CP = BK_CP;                           \
  TR = BK_TR

EXTERN inline void save_H(void) {
  Yap_REGS.H_   = H;
}

EXTERN inline void restore_H(void) {
  H = Yap_REGS.H_;
}

#define BACKUP_H()  CELL *BK_H = H; restore_H()

#define RECOVER_H()  save_H(); H = BK_H

EXTERN inline void save_B(void) {
  Yap_REGS.B_   = B;
}

EXTERN inline void restore_B(void) {
  B = Yap_REGS.B_;
}

#define BACKUP_B()  choiceptr BK_B = B; restore_B()

#define RECOVER_B()  save_B(); B = BK_B

EXTERN void restore_TR(void);
EXTERN void save_TR(void);

EXTERN inline void save_TR(void) {
  Yap_REGS.TR_ = TR;
}

EXTERN inline void restore_TR(void) {
  TR = Yap_REGS.TR_;
}

#else

#define CP         Yap_REGS.CP_	/* continuation   program counter         */
#define P          Yap_REGS.P_	/* prolog machine program counter */
#define YENV       Yap_REGS.YENV_ /* current environment (may differ from ENV) */
#define S          Yap_REGS.S_	/* structure pointer                      */
#define	H          Yap_REGS.H_	/* top of heap (global)   stack           */
#define B          Yap_REGS.B_	/* latest choice point            */
#define TR         Yap_REGS.TR_	/* top of trail                           */
#define HB         Yap_REGS.HB_	/* heap (global) stack top at time of latest c.p. */
#define CreepFlag Yap_REGS.CreepFlag_

EXTERN inline void save_machine_regs(void) {
}

EXTERN inline void restore_machine_regs(void) {
}

#define BACKUP_MACHINE_REGS()

#define RECOVER_MACHINE_REGS()

EXTERN inline void save_H(void) {
}

EXTERN inline void restore_H(void) {
}

#define BACKUP_H()

#define RECOVER_H()

EXTERN inline void save_B(void) {
}

EXTERN inline void restore_B(void) {
}

#define BACKUP_B()

#define RECOVER_B()

#define restore_TR() 

#endif

#define	AuxBase       Yap_REGS.AuxBase_
#define	AuxSp         Yap_REGS.AuxSp_
#define	AuxTop        Yap_REGS.AuxTop_
#define TopB          Yap_REGS.TopB_
#define DelayedB      Yap_REGS.DelayedB_
#define EX            Yap_REGS.EX_
#define DEPTH	      Yap_REGS.DEPTH_
#if defined(SBA) || defined(TABLING)
#define H_FZ          Yap_REGS.H_FZ_
#define B_FZ          Yap_REGS.B_FZ_
#define TR_FZ         Yap_REGS.TR_FZ_
#endif /* SBA || TABLING */
#define PP	         (Yap_REGS.PP_)
#if defined(YAPOR) || defined(THREADS)
#define worker_id         (Yap_REGS.worker_id_)
#define PREG_ADDR	         (Yap_REGS.PREG_ADDR_)
#ifdef SBA
#define BSEG	      Yap_REGS.BSEG_
#define binding_array Yap_REGS.binding_array_
#define sba_offset    Yap_REGS.sba_offset_
#define sba_end       Yap_REGS.sba_end_
#define sba_size      Yap_REGS.sba_size_
#define frame_head    Yap_REGS.frame_head_
#define frame_tail    Yap_REGS.frame_tail_
#endif /* SBA */
#endif /* YAPOR */
#ifdef COROUTINING
#define DelayedVars   Yap_REGS.DelayedVars_
#endif
#define CurrentModule Yap_REGS.CurrentModule_
#define ARITH_EXCEPTION     Yap_REGS.ARITH_EXCEPTION_
#define Yap_isint     Yap_REGS.isint_
#define Yap_Floats    Yap_REGS.Floats_
#define Yap_Ints      Yap_REGS.Ints_

#define REG_SIZE	sizeof(REGS)/sizeof(CELL *)

#define ARG1	XREGS[1]
#define ARG2	XREGS[2]
#define ARG3    XREGS[3]
#define ARG4	XREGS[4]
#define ARG5	XREGS[5]
#define ARG6	XREGS[6]
#define ARG7	XREGS[7]
#define ARG8	XREGS[8]
#define ARG9	XREGS[9]
#define ARG10	XREGS[10]
#define ARG11	XREGS[11]
#define ARG12	XREGS[12]
#define ARG13	XREGS[13]
#define ARG14	XREGS[14]
#define ARG15	XREGS[15]
#define ARG16	XREGS[16]

#define cut_succeed()	return( ( B = B->cp_b, 1 ))
#define cut_fail()	return( ( B = B->cp_b, 0 ))


/* by default, define HBREG to be HB */

#define HBREG HB

#if defined(SBA) || defined(TABLING)
#define BB            Yap_REGS.BB_
#define BBREG         BB
#endif /* SBA || TABLING */

#if !defined(THREADS)
/* use actual addresses for regs */
#define PRECOMPUTE_REGADDRESS 1
#endif /* THREADS */


/* aggregate several abstract machine operations in a single */
#define AGGREGATE_OPS 1

/* make standard registers globally accessible so that they are there
   when we come from a longjmp */
#if PUSH_REGS
/* In this case we need to initialise the abstract registers */
extern REGSTORE Yap_standard_regs;
#endif /* PUSH_REGS */

/******************* controlling debugging ****************************/
static inline UInt
CalculateStackGap(void)
{
  UInt gmin = (LCL0-H0)>>2;

  if (gmin < MinStackGap) gmin = MinStackGap; 
  //  if (gmin > 1024*1024) return 1024*1024; 
  return gmin;
}
