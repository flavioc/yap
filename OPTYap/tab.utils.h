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

#define TermStack_PushFunctorArgs(CS_Cell)

#endif /* TABLING_CALL_SUBSUMPTION */

#endif