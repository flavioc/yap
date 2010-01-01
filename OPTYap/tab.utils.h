/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   R. Rocha and NCC - University of Porto, Portugal
  File:        tab.utils.h
                                                                     
**********************************************************************/

#ifndef TAB_UTILS_H
#define TAB_UTILS_H

#include "opt.config.h"

#ifdef TABLING_CALL_SUBSUMPTION
#include "tab.xsb.h"
#include "tab.stack.h"

// emu/tst_aux.h

extern DynamicStack tstTermStack;

#define TST_TERMSTACK_INITSIZE 25

#define TermStack_Top ((CPtr)DynStk_Top(tstTermStack))
#define TermStack_Base ((CPtr)DynStk_Base(tstTermStack))
#define TermStack_NumTerms DynStk_NumFrames(tstTermStack)
#define TermStack_ResetTOS DynStk_ResetTOS(tstTermStack)
#define TermStack_IsEmpty DynStk_IsEmpty(tstTermStack)

#define TermStack_SetTOS(Index) \
  DynStk_Top(tstTermStack) = TermStack_Base + Index
  
#define TermStack_Push(Term) { \
    CPtr nextFrame;     \
    DynStk_Push(tstTermStack, nextFrame); \
    *nextFrame = Term;  \
  }
  
#define TermStack_BlindPush(Term) { \
    CPtr nextFrame; \
    DynStk_BlindPush(tstTermStack, nextFrame);  \
    *nextFrame = Term;  \
  }
  
#define TermStack_Pop(Term) { \
    CPtr curFrame;  \
    DynStk_BlindPop(tstTermStack, curFrame); \
    Term = *curFrame; \
  } 

#define TermStack_Peek(Term) { \
    CPtr curFrame; \
    DynStk_BlindPeek(tstTermStack, curFrame); \
    Term = *curFrame; \
  }
  
#define TermStack_NOOP /* nothing to push when constants match */


#define TermStack_PushFunctorArgs(CS_Cell)  \
  TermStack_PushLowToHighVector(clref_val(CS_Cell) + 1, \
    get_cell_arity(CS_Cell))

// TermStack_PushListArgs XXX

/*
 * The following macros enable the movement of an argument vector to
 * the TermStack.  Two versions are supplied depending on whether the
 * vector is arranged from high-to-low memory, such as an answer
 * template, or from low-to-high memory, such as the arguments of a
 * compound heap term.  The vector pointer is assumed to reference the
 * first element of the vector.
 */
 
#define TermStack_PushLowToHighVector(pVectorLow, Magnitude)  { \
  int i, numElements;  \
  CPtr pElement;   \
  numElements = Magnitude; \
  pElement = pVectorLow + numElements; \
  DynStk_ExpandIfOverflow(tstTermStack, numElements);  \
  for(i = 0; i < numElements; ++i) { \
    pElement--; \
    TermStack_BlindPush(get_term_deref(pElement)); \
  } \
}

#define TermStack_PushHighToLowVector(pVectorHigh, Magnitude) { \
    int i, numElements; \
    CPtr pElement;  \
    numElements = Magnitude;  \
    pElement = pVectorHigh - numElements; \
    DynStk_ExpandIfOverflow(tstTermStack, numElements); \
    for(i = 0; i < numElements; ++i)  { \
      pElement++; \
      TermStack_BlindPush(get_term_deref(pElement)); \
    } \
}

/*
 * This macro copies an array of terms onto the TermStack, checking for
 * overflow only once at the beginning, rather than with each push.  The
 * elements to be pushed are assumed to exist in array elements
 * 0..(NumElements-1).
 */

#define TermStack_PushArray(Array, NumElements) { \
  counter i;  \
  DynStk_ExpandIfOverflow(tstTermStack, NumElements); \
  for(i = 0; i < numElements; ++i)  \
    TermStack_BlindPush(Array[i]);  \
}

#endif /* TABLING_CALL_SUBSUMPTION */

#endif