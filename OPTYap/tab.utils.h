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
    get_arity((Psc)*clref_val(CS_Cell)))

#define TermStack_PushListArgs(LIST_Cell) { \
  CPtr pListHeadCell = clrefp_val(LIST_Cell);  \
  DynStk_ExpandIfOverflow(tstTermStack, 2); \
  TermStack_BlindPush(*(pListHeadCell + 1));  \
  TermStack_BlindPush(*(pListHeadCell));  \
}

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
    TermStack_BlindPush(*pElement); \
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
      TermStack_BlindPush(*pElement); \
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
  for(i = 0; i < NumElements; ++i)  \
    TermStack_BlindPush(Array[i]);  \
}

/* ------------------------------------------------------------------------- */

/*
 *  tstTermStackLog
 *  ---------------
 *  For noting the changes made to the tstTermStack during processing
 *  where backtracking is required.  Only the changes necessary to
 *  transform the tstTermStack from its current state to a prior state
 *  are logged.  Therefore, we only need record values popped from the
 *  tstTermStack.
 *
 *  Each frame of the log consists of the index of a tstTermStack
 *  frame and the value that was stored there.  tstTermStack values
 *  are reinstated as a side effect of a tstTermStackLog_Pop.
 */
 
typedef struct {
  int index;
  Cell value;
} tstLogFrame;

typedef tstLogFrame *pLogFrame;

#define LogFrame_Index(Frame) ((Frame)->index)
#define LogFrame_Value(Frame) ((Frame)->value)

extern DynamicStack tstTermStackLog;

#define TST_TERMSTACKLOG_INITSIZE 20

#define TermStackLog_Top ((pLogFrame)DynStk_Top(tstTermStackLog))
#define TermStackLog_Base ((pLogFrame)DynStk_Base(tstTermStackLog))
#define TermStackLog_ResetTOS DynStk_ResetTOS(tstTermStackLog)

#define TermStackLog_PushFrame {  \
    pLogFrame nextFrame; \
    DynStk_Push(tstTermStackLog, nextFrame);  \
    LogFrame_Index(nextFrame) = TermStack_Top - TermStack_Base; \
    LogFrame_Value(nextFrame) = *(TermStack_Top); \
}

#define TermStackLog_PopAndReset { \
    pLogFrame curFrame; \
    DynStk_BlindPop(tstTermStackLog, curFrame); \
    TermStack_Base[LogFrame_Index(curFrame)] = LogFrame_Value(curFrame);  \
}

/*
 * Reset the TermStack elements down to and including the Index-th
 * entry in the Log.
 */
#define TermStackLog_Unwind(Index)  { \
    pLogFrame unwindBase = TermStackLog_Base + Index;  \
    while(TermStackLog_Top > unwindBase)  \
      TermStackLog_PopAndReset; \
}

/* ------------------------------------------------------------------------- */

/*
 *  tstSymbolStack
 *  ---------------
 *  For constructing terms from the symbols stored along a path in the trie.
 */

extern DynamicStack tstSymbolStack;

#define TST_SYMBOLSTACK_INITSIZE 25

#define SymbolStack_Top   ((CPtr)DynStk_Top(tstSymbolStack))
#define SymbolStack_Base  ((CPtr)DynStk_Base(tstSymbolStack))
#define SymbolStack_NumSymbols  (SymbolStack_Top - SymbolStack_Base)
#define SymbolStack_ResetTOS    DynStk_ResetTOS(tstSymbolStack)
#define SymbolStack_IsEmpty   DynStk_IsEmpty(tstSymbolStack)
#define SymbolStack_BlindDrop DynStk_BlindDrop(tstSymbolStack) /* NEW */

#define SymbolStack_Push(Symbol)  { \
    CPtr nextFrame; \
    DynStk_Push(tstSymbolStack, nextFrame);  \
    *nextFrame = Symbol;  \
}

#define SymbolStack_Pop(Symbol)   {   \
    CPtr curFrame;  \
    DynStk_BlindPop(tstSymbolStack, curFrame);  \
    Symbol = *curFrame; \
}

#define SymbolStack_Peek(Symbol) {  \
    CPtr curFrame;  \
    DynStk_BlindPeek(tstSymbolStack, curFrame); \
    Symbol = *curFrame; \
}

#define SymbolStack_PushPathRoot(Leaf, Root) {  \
    BTNptr btn = (BTNptr)Leaf;  \
    while(!IsTrieRoot(btn)) { \
      SymbolStack_Push(BTN_Symbol(btn));  \
      btn = BTN_Parent(btn);  \
    } \
    Root = (void*)btn;  \
}

#define SymbolStack_PushPath(Leaf) {  \
    BTNptr root;  \
    SymbolStack_PushPathRoot(Leaf, root); \
}

/* ------------------------------------------------------------------------- */

/*
 *  tstTrail
 *  ---------
 *  For recording bindings made during processing.  This Trail performs
 *  simple WAM trailing -- it saves address locations only.
 */
 
extern DynamicStack tstTrail;
 
#define TST_TRAIL_INITSIZE  20

#define Trail_Top           ((CPtr *)DynStk_Top(tstTrail))
#define Trail_Base          ((CPtr *)DynStk_Base(tstTrail))
#define Trail_NumBindings   DynStk_NumFrames(tstTrail)
#define Trail_ResetTOS      DynStk_ResetTOS(tstTrail)

#define Trail_Push(Addr) {  \
    CPtr *nextFrame;  \
    DynStk_Push(tstTrail, nextFrame); \
    *nextFrame = (CPtr)(Addr);  \
}

#define Trail_PopAndReset { \
    CPtr *curFrame; \
    DynStk_BlindPop(tstTrail, curFrame);  \
    bld_free(*curFrame);  \
}

#define Trail_Unwind_All  Trail_Unwind(0)

/*
 * Untrail down to and including the Index-th element.
 */
#define Trail_Unwind(Index) { \
   CPtr *unwindBase = Trail_Base + Index; \
   while(Trail_Top > unwindBase)  \
    Trail_PopAndReset; \
}

/* --------------------------------------------- */

#define ProcessNextSubtermFromTrieStacks(Symbol,StdVarNum) {  \
  Cell subterm; \
  TermStack_Pop(subterm); \
  printf("Poping first %x\n", subterm); \
  XSB_Deref(subterm); \
  printf("Deref %x %d %d\n", subterm, cell_tag(subterm), IsAtomOrIntTerm(subterm));  \
  switch(cell_tag(subterm)) { \
    case XSB_REF: \
    printf("uma variavel\n"); \
      if(!IsStandardizedVariable(subterm)) {  \
        StandardizeVariable(subterm, StdVarNum);  \
        Trail_Push(subterm);  \
        Symbol = EncodeNewTrieVar(StdVarNum); \
        StdVarNum++;  \
      } \
      else  \
        Symbol = EncodeTrieVar(IndexOfStdVar(subterm)); \
      break;  \
    case XSB_STRING:  \
    case XSB_INT: \
    printf("Int / Atom\n"); \
      Symbol = EncodeTrieConstant(subterm); \
      break;  \
    case XSB_STRUCT:  \
    printf("uma struct\n"); \
      Symbol = EncodeTrieFunctor(subterm);  \
      TermStack_PushFunctorArgs(subterm); \
      break;  \
    case XSB_LIST:  \
    printf("uma lista\n");  \
      Symbol = EncodeTrieList(subterm); \
      TermStack_PushListArgs(subterm);  \
      break;  \
    default:  \
    printf("Unknown subterm\n");  \
      Symbol = 0; \
      TrieError_UnknownSubtermTag(subterm); \
    } \
}
// TODO: floats, longints

/* --------------------------------------------- */

/* emu/tries.h */
typedef enum Trie_Path_Type {
  NO_PATH, VARIANT_PATH, SUBSUMPTIVE_PATH
} TriePathType;

typedef struct {
  BTNptr alt_node;
  BTNptr var_chain;
  int termstk_top_index;
  int log_top_index;
  int trail_top_index;
} tstCallChoicePointFrame;
  
typedef tstCallChoicePointFrame *pCPFrame;

#define CALL_CPSTACK_SIZE 1024

struct tstCCPStack_t {
  pCPFrame top;
  pCPFrame ceiling;
  tstCallChoicePointFrame base[CALL_CPSTACK_SIZE];
};

extern struct tstCCPStack_t tstCCPStack;

/* -------------------------------- */
/* emu/tries.h */

struct VariantContinuation {
  BTNptr last_node_matched;
  struct subterms_desc {
    counter num; /* number of subterms in the stack */
    struct termstack_desc {
      size_t size; /* number of elements in the stack */
      Cell *ptr; /* dynamic memory allocated for the stack */
    } stack;
  } subterms;
  struct bindings_desc {
    counter num; /* number of bindings */
    struct trail_desc {
      size_t size; /* number of elements in the trail */
      struct frame {
        CPtr var;
        Cell value;
      } *ptr; /* dynamic memory allocated for the trail */
    } stack;
  } bindings;
};

extern struct VariantContinuation variant_cont;

extern Cell TrieVarBindings[MAX_TABLE_VARS];

/* emu/trie_internals.h */
#define IsUnboundTrieVar(dFreeVar)    \
  ( ((CPtr)(dFreeVar) >= TrieVarBindings) &&  \
    ((CPtr)(dFreeVar) <= (TrieVarBindings + MAX_TABLE_VARS - 1))  )

typedef enum {
  TAG_ATOM,
  TAG_INT,
  TAG_LONG_INT,
  TAG_BIG_INT,
  TAG_FLOAT,
  TAG_STRUCT,
  TAG_LIST,
  TAG_REF,
  TAG_DB_REF,
  TAG_TrieVar,
  TAG_UNKNOWN
} CellTag;

CellTag cell_tag(Term t);
CellTag TrieSymbolType(Term t);
xsbBool are_identical_terms(Cell term1, Cell term2);
void printSubgoalTriePath(CTXTdeclc FILE *fp, BTNptr pLeaf, tab_ent_ptr tab_entry);
void printTrieNode(FILE *fp, BTNptr pTN);
void printSubterm(FILE *fp, Term term);
void printCalledSubgoal(FILE *fp, yamop *preg);
void printSubstitutionFactor(FILE *fp, CELL* factor);

extern int AnsVarCtr;

#define TrieError_UnknownSubtermTagMsg				\
   "Trie Subterm-to-Symbol Conversion\nUnknown subterm type (%d)"
   
#define TrieError_UnknownSubtermTag(Subterm)			\
   xsb_abort(TrieError_UnknownSubtermTagMsg, cell_tag(Subterm))

#endif /* TABLING_CALL_SUBSUMPTION */

#endif