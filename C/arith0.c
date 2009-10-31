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
* File:		eval.c							 *
* Last rev:								 *
* mods:									 *
* comments:	arithmetical expression evaluation			 *
*									 *
*************************************************************************/
#ifdef SCCS
static char     SccsId[] = "%W% %G%";
#endif

/*
 * This file implements arithmetic operations 
 *
 */

#include "Yap.h"
#include "Yatom.h"
#include "YapHeap.h"
#include "eval.h"


#ifndef PI
#ifdef M_PI
#define PI M_PI
#else
#define PI 3.14159265358979323846
#endif
#endif

#ifndef M_E
#define M_E 2.7182818284590452354
#endif

#ifndef INFINITY
#define INFINITY (1.0/0.0)
#endif

#ifndef NAN
#define NAN      (0.0/0.0)
#endif

/* copied from SWI-Prolog */
#ifndef DBL_EPSILON /* normal for IEEE 64-bit double */
#define DBL_EPSILON 0.00000000000000022204
#endif

static Term
eval0(Int fi) {
  arith0_op fop = fi;
  switch (fop) {
  case op_pi:
    {
      RFLOAT(PI);
    }
  case op_e:
    {
      RFLOAT(M_E);
    }
  case op_epsilon:
    {
      RFLOAT(DBL_EPSILON);
    }
  case op_inf:
    {
#ifdef _MSC_VER /* Microsoft's Visual C++ Compiler */
      Yap_Error(TYPE_ERROR_EVALUABLE, TermNil, "evaluating infinity");
      P = (yamop *)FAILCODE;
      RERROR();
#else
      if (yap_flags[LANGUAGE_MODE_FLAG] == 1) {/* iso */
	Yap_Error(TYPE_ERROR_EVALUABLE, TermNil, "evaluating infinity");
	P = (yamop *)FAILCODE;
	RERROR();
      } else {
	RFLOAT(INFINITY);
      }
#endif
    }
  case op_nan:
    {
#ifdef _MSC_VER /* Microsoft's Visual C++ Compiler */
      Yap_Error(TYPE_ERROR_EVALUABLE, TermNil, "evaluating infinity");
      P = (yamop *)FAILCODE;
      RERROR();
#else
      if (yap_flags[LANGUAGE_MODE_FLAG] == 1) {/* iso */
	Yap_Error(TYPE_ERROR_EVALUABLE, TermNil, "evaluating not-a-number");
	P = (yamop *)FAILCODE;
	RERROR();
      } else {
	RFLOAT(NAN);
      }
#endif
    }
  case op_random:
    {
      RFLOAT(Yap_random());
    }
  case op_cputime:
    {
      RFLOAT((Float)Yap_cputime()/1000.0);
    }
  case op_heapused:
    RINT(HeapUsed);
  case op_localsp:
#if SBA
    RINT((Int)ASP);
#else
    RINT(LCL0 - ASP);
#endif
  case op_b:
#if SBA
    RINT((Int)B);
#else
    RINT(LCL0 - (CELL *)B);
#endif
  case op_env:
#if SBA
    RINT((Int)YENV);
#else
    RINT(LCL0 - YENV);
#endif
  case op_tr:
#if SBA
    RINT(TR);
#else
    RINT(((CELL *)TR)-LCL0);
#endif
  case op_stackfree:
    RINT(Unsigned(ASP) - Unsigned(H));
  case op_globalsp:
#if SBA
    RINT((Int)H);
#else
    RINT(H - H0);
#endif
  }
  RERROR();
}

Term Yap_eval_atom(Int f)
{
  return eval0(f);
}

typedef struct init_const_eval {
  char          *OpName;
  arith0_op      f;
} InitConstEntry;

static InitConstEntry InitConstTab[] = {
  {"pi", op_pi},
  {"e", op_e},
  {"epsilon", op_epsilon},
  {"inf", op_inf},
  {"nan", op_nan},
  {"random", op_random},
  {"cputime", op_cputime},
  {"heapused", op_heapused},
  {"local_sp", op_localsp},
  {"global_sp", op_globalsp},
  {"$last_choice_pt", op_b},
  {"$env", op_env},
  {"$tr", op_tr},
  {"stackfree", op_stackfree},
};

void
Yap_InitConstExps(void)
{
  unsigned int    i;
  ExpEntry       *p;

  for (i = 0; i < sizeof(InitConstTab)/sizeof(InitConstEntry); ++i) {
    AtomEntry *ae = RepAtom(Yap_LookupAtom(InitConstTab[i].OpName));
    if (ae == NULL) {
      Yap_Error(OUT_OF_HEAP_ERROR,TermNil,"at InitConstExps");
      return;
    }
    WRITE_LOCK(ae->ARWLock);
    if (Yap_GetExpPropHavingLock(ae, 0)) {
      WRITE_UNLOCK(ae->ARWLock);
      break;
    }
    p = (ExpEntry *) Yap_AllocAtomSpace(sizeof(ExpEntry));
    p->KindOfPE = ExpProperty;
    p->ArityOfEE = 0;
    p->ENoOfEE = 0;
    p->FOfEE = InitConstTab[i].f;
    p->NextOfPE = ae->PropsOfAE;
    ae->PropsOfAE = AbsExpProp(p);
    WRITE_UNLOCK(ae->ARWLock);
  }
}

/* This routine is called from Restore to make sure we have the same arithmetic operators */
int
Yap_ReInitConstExps(void)
{
  return TRUE;
}

