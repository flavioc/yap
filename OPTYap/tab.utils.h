/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   R. Rocha and NCC - University of Porto, Portugal
  File:        tab.utils.h
                                                                     
**********************************************************************/

#ifndef TAB_UTILS_H
#define TAB_UTILS_H

#include "opt.config.h"

#ifdef TABLING
#include "tab.xsb.h"
#include "tab.stack.h"

#define POWER_OF_TWO(X) (((X) & ((X) - 1)) == 0)

#ifdef BITS64
#define IntegerFormatString	"%ld"
#else
#define IntegerFormatString	"%d"
#endif

#if SHORT_INTS
#define LongIntFormatString "%ld"
#else
#define LongIntFormatString "%d"
#endif

#if SIZEOF_DOUBLE == 2 * SIZEOF_INT_P
#define FloatFormatString "%lf"
#else
#define FloatFormatString "%f"
#endif

// emu/tst_aux.h

extern DynamicStack tstTermStack;

#define TST_TERMSTACK_INITSIZE 100

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

#define TST_TERMSTACKLOG_INITSIZE 100

#define TermStackLog_Top ((pLogFrame)DynStk_Top(tstTermStackLog))
#define TermStackLog_Base ((pLogFrame)DynStk_Base(tstTermStackLog))
#define TermStackLog_ResetTOS DynStk_ResetTOS(tstTermStackLog)

#define TermStackLog_PushFrame {                                \
    pLogFrame nextFrame;                                        \
    DynStk_Push(tstTermStackLog, nextFrame);                    \
    LogFrame_Index(nextFrame) = TermStack_Top - TermStack_Base; \
    LogFrame_Value(nextFrame) = *(TermStack_Top);               \
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

#define TST_SYMBOLSTACK_INITSIZE 100

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

#define SymbolStack_PopOther(Symbol, Cast) {  \
  CPtr curFrame;  \
  DynStk_BlindPop(tstSymbolStack, curFrame);  \
  Symbol = (Cast)*curFrame; \
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

#define SymbolStack_PushPathRootNodes(Leaf, Root) {  \
    BTNptr btn = (BTNptr)Leaf;  \
    while(!IsTrieRoot(btn)) { \
      if(TrNode_is_float(btn) || TrNode_is_long(btn)) { \
        SymbolStack_Push((CELL)btn);  \
      } \
      SymbolStack_Push(BTN_Symbol(btn));  \
      btn = BTN_Parent(btn);  \
    } \
    Root = (void*)btn;  \
}

#define SymbolStack_PushPathNodes(Leaf) {  \
    BTNptr root;  \
    SymbolStack_PushPathRootNodes(Leaf, root); \
}

/* ------------------------------------------------------------------------- */

/*
 *  tstTrail
 *  ---------
 *  For recording bindings made during processing.  This Trail performs
 *  simple WAM trailing -- it saves address locations only.
 */
 
extern DynamicStack tstTrail;
 
#define TST_TRAIL_INITSIZE  100

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
  TAG_UNKNOWN
} CellTag;

#define TAG_TrieVar TAG_REF

static inline CellTag cell_tag(Term t)
{
  if(IsVarTerm(t))
    return TAG_REF;
  
  if(IsAtomTerm(t))
    return TAG_ATOM;
  
  if(IsIntTerm(t))
    return TAG_INT;
  
  if(IsPairTerm(t))
    return TAG_LIST;
  
  if (IsApplTerm(t)) {
    Functor f = FunctorOfTerm(t);
    
    if (f == FunctorDouble)
      return TAG_FLOAT;
      
    if (f == FunctorLongInt)
      return TAG_LONG_INT;
    
    if (f == FunctorDBRef)
      return TAG_DB_REF;
    
    if (f == FunctorBigInt)
      return TAG_BIG_INT;
    
    return TAG_STRUCT;
  }
  
  return TAG_UNKNOWN;
}

/* --------------------------------------------- */

#define TrieError_UnknownSubtermTagMsg				\
   "Trie Subterm-to-Symbol Conversion\nUnknown subterm type (%d)"

#define TrieError_UnknownSubtermTag(Subterm)			\
   xsb_abort(TrieError_UnknownSubtermTagMsg, cell_tag(Subterm))

#define ProcessNextSubtermFromTrieStacks(Symbol,NodeType,StdVarNum) {  \
 Cell subterm; \
 TermStack_Pop(subterm); \
 XSB_Deref(subterm); \
 NodeType = INTERIOR_NT; \
 switch(cell_tag(subterm)) { \
   case XSB_REF: \
     if(!IsStandardizedVariable(subterm)) {  \
       if (StdVarNum == MAX_TABLE_VARS)                \
         Yap_Error(INTERNAL_ERROR, TermNil, "MAX_TABLE_VARS exceeded");  \
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
     Symbol = EncodeTrieConstant(subterm); \
     break;  \
   case XSB_STRUCT:  \
     Symbol = EncodeTrieFunctor(subterm);  \
     TermStack_PushFunctorArgs(subterm); \
     break;  \
   case XSB_LIST:  \
     Symbol = EncodeTrieList(subterm); \
     TermStack_PushListArgs(subterm);  \
     break;  \
   case TAG_LONG_INT:                  \
     li = LongIntOfTerm(subterm);      \
     Symbol = (Cell)&li;               \
     NodeType |= LONG_INT_NT;          \
     break;  \
   case TAG_FLOAT:                     \
     flt = FloatOfTerm(subterm);     \
     symbol = (Cell)&flt;              \
     NodeType |= FLOAT_NT;                 \
     break;                                \
   default:  \
     Symbol = 0; \
     TrieError_UnknownSubtermTag(subterm); \
   } \
}

/* --------------------------------------------- */

void printTrieSymbol(FILE* fp, Cell symbol);
void printAnswerTriePath(FILE *fp, ans_node_ptr leaf);

void printSubterm(FILE *fp, Term term);
void printCalledSubgoal(FILE *fp, yamop *preg);
void printAnswerTemplate(FILE *fp, CPtr ans_tmplt, int size);
void printSubstitutionFactor(FILE *fp, CELL* factor);
void printTermStack(FILE *fp);
void printTriePath(CTXTdeclc FILE *fp, sg_node_ptr pLeaf, xsbBool print_address);
void printSubgoalTriePath(CTXTdeclc FILE *fp, sg_fr_ptr sg_fr);
void printSubgoalTriePathAll(CTXTdeclc FILE *fp, sg_node_ptr leaf, tab_ent_ptr tab_ent);

#ifdef TABLING_CALL_SUBSUMPTION

static inline CellTag TrieSymbolType(Term t)
{
  if(IsVarTerm(t))
    return XSB_TrieVar;
    
  if(IsIntTerm(t))
    return TAG_INT;
  
  if(IsAtomTerm(t))
    return TAG_ATOM;
  
  if(IsPairTerm(t))
    return TAG_LIST;
  
  if(IsApplTerm(t)) {
    Functor f = (Functor)RepAppl(t);
    
    switch ((CELL)f) {
      case (CELL)FunctorDouble: return TAG_FLOAT;
      case (CELL)FunctorLongInt: return TAG_LONG_INT;
      case (CELL)FunctorDBRef: return TAG_DB_REF;
      case (CELL)FunctorBigInt: return TAG_BIG_INT;
      default: /* functor maybe somewhere else */
        f = (Functor)*(CELL *)f;
        switch((CELL)f) {
          case (CELL)FunctorDouble: return TAG_FLOAT;
          case (CELL)FunctorLongInt: return TAG_LONG_INT;
          case (CELL)FunctorDBRef: return TAG_DB_REF;
          case (CELL)FunctorBigInt: return TAG_BIG_INT;
          default: return TAG_STRUCT;
        }
    }
  }
  
  return TAG_UNKNOWN;
}

xsbBool are_identical_terms(Cell term1, Cell term2);
int answer_template_size(CELL *ans_tmplt);
int copy_answer_template(CELL *ans_tmplt, CELL *dest);
void printSubsumptiveAnswer(FILE *fp, CELL* vars);
CELL* construct_subgoal_heap(BTNptr pLeaf, CPtr* var_pointer, int arity, int pushArguments);
CPtr reconstruct_template_for_producer_no_args(SubProdSF subsumer, CELL* ans_tmplt);
#ifdef TABLING_RETROACTIVE
CELL* construct_answer_template_from_sg(CELL* subgoal_args, int arity, CELL* ans_tmplt);
#endif /* TABLING_RETROACTIVE */
void printTrieNode(FILE *fp, BTNptr pTN);

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

extern Cell TrieVarBindings[MAX_TABLE_VARS];

/* emu/trie_internals.h */
#define IsUnboundTrieVar(dFreeVar)    \
  ( ((CPtr)(dFreeVar) >= TrieVarBindings) &&  \
    ((CPtr)(dFreeVar) <= (TrieVarBindings + MAX_TABLE_VARS - 1))  )


extern int AnsVarCtr;

#define TrieSymbol_Deref(Symbol)	\
 if(IsTrieVar(Symbol)) {	\
   Symbol = TrieVarBindings[DecodeTrieVar(Symbol)];	\
   XSB_Deref(Symbol);	\
 } 

/* emu/tries.h */
typedef enum Trie_Path_Type {
 NO_PATH, VARIANT_PATH, SUBSUMPTIVE_PATH
} TriePathType;

extern struct VariantContinuation variant_cont;

extern CELL* AT;
extern int AT_SIZE;

#define IsAnswerTemplateVar(VAR) ((CPtr)(VAR) >= AT && ((CPtr)(VAR) <= (AT + AT_SIZE)))

#endif /* TABLING_CALL_SUBSUMPTION */

// deactivate to test
#undef __USE_XOPEN2K8
//#define FDEBUG
#ifdef FDEBUG
#define dprintf(...) fprintf(stdout,  __VA_ARGS__)
#else
#define dprintf(MESG, ARGS...)
#endif
//#define RETRO_CHECKS 1

#define EFFICIENT_SUBSUMED_COLLECT
#define SUBSUMED_CHECK_BEFORE_MATCHING
//#define BENCHMARK_EXECUTION
//#define TIME_SUBSUMED_COLLECT

#ifdef BENCHMARK_EXECUTION
#define BENCHMARK_TOTAL 25
#define DECLARE_BENCHMARK() extern double benchmark_array[BENCHMARK_TOTAL]; extern int benchmark_hits_array[BENCHMARK_TOTAL]
#define GET_BENCHMARK(IDX) benchmark_array[IDX]
#define GET_BENCHMARK_HITS(IDX) benchmark_hits_array[IDX]
#define start_benchmark()																															\
		struct rusage rusage_start;		\
		getrusage(RUSAGE_SELF, &rusage_start)
#define end_benchmark(IDX)																														\
		DECLARE_BENCHMARK();																															\
		struct rusage rusage_end;																													\
		getrusage(RUSAGE_SELF, &rusage_end);																							\
		double clock_elapsed = (rusage_end.ru_utime.tv_sec - rusage_start.ru_utime.tv_sec) * 1000.0 +	\
				(rusage_end.ru_utime.tv_usec - rusage_start.ru_utime.tv_usec) / 1000.0;					\
		GET_BENCHMARK_HITS(IDX)++; 																												\
		GET_BENCHMARK(IDX) += clock_elapsed
#define increment_benchmark(IDX) \
		extern int benchmark_hits_array[BENCHMARK_TOTAL]; \
	GET_BENCHMARK_HITS(IDX)++
#else
#define increment_benchmark(IDX)
/*
	 old version:
#define start_benchmark()																															\
		clock_t clock_start = clock()
#define end_benchmark(IDX)																														\
		DECLARE_BENCHMARK();																															\
		double clock_elapsed = ((double)(clock()-clock_start)) / (CLOCKS_PER_SEC / 1000);	\
		GET_BENCHMARK_HITS(IDX)++; 																												\
		GET_BENCHMARK(IDX) += clock_elapsed
		*/
#define start_benchmark()
#define end_benchmark(IDX)
#endif

#define TIME_SUBSUMED_BENCHMARK 0
#define CREATE_TSI_BENCHMARK 1
#define UPDATE_TIMESTAMPS_BENCHMARK 2
#define INSERT_ANSWER_BENCHMARK 3
#define PENDING_ANSWER_INDEX_BENCHMARK 4
#define PRUNING_BENCHMARK 5
#define LOOKUP_SUBGOAL_TRIE_BENCHMARK 6
#define INITIAL_COLLECT_BENCHMARK 7
#define RELEVANT_COLLECT_BENCHMARK 8
#define IN_EVAL_BENCHMARK 9
#define TOP_GEN_BENCHMARK 10

#ifdef TIME_SUBSUMED_BENCHMARK
#define start_time_subsumed()	start_benchmark()
#define end_time_subsumed()		end_benchmark(TIME_SUBSUMED_BENCHMARK)
#else
#define end_time_subsumed()
#define start_time_subsumed()
#endif

#ifdef CREATE_TSI_BENCHMARK
#define start_createtsi_benchmark() start_benchmark()
#define end_createtsi_benchmark() end_benchmark(CREATE_TSI_BENCHMARK)
#else
#define start_createtsi_benchmark()
#define end_createtsi_benchmark()
#endif

#ifdef UPDATE_TIMESTAMPS_BENCHMARK
#define start_update_timestamps_benchmark() start_benchmark()
#define end_update_timestamps_benchmark() end_benchmark(UPDATE_TIMESTAMPS_BENCHMARK)
#else
#define start_update_timestamps_benchmark()
#define end_update_timestamps_benchmark()
#endif

#ifdef INSERT_ANSWER_BENCHMARK
#define start_insert_answer_benchmark() start_benchmark()
#define end_insert_answer_benchmark() end_benchmark(INSERT_ANSWER_BENCHMARK)
#else
#define start_insert_answer_benchmark()
#define end_insert_answer_benchmark()
#endif

#ifdef PENDING_ANSWER_INDEX_BENCHMARK
#define start_pending_answer_index_benchmark() start_benchmark()
#define end_pending_answer_index_benchmark() end_benchmark(PENDING_ANSWER_INDEX_BENCHMARK)
#else
#define start_pending_answer_index_benchmark()
#define end_pending_answer_index_benchmark()
#endif

#ifdef PRUNING_BENCHMARK
#define start_pruning_benchmark() start_benchmark()
#define end_pruning_benchmark() end_benchmark(PRUNING_BENCHMARK)
#else
#define start_pruning_benchmark()
#define end_pruning_benchmark()
#endif

#ifdef LOOKUP_SUBGOAL_TRIE_BENCHMARK
#define start_lookup_subgoal_trie_benchmark() start_benchmark()
#define end_lookup_subgoal_trie_benchmark() end_benchmark(LOOKUP_SUBGOAL_TRIE_BENCHMARK)
#else
#define start_lookup_subgoal_trie_benchmark() start_benchmark()
#define end_lookup_subgoal_trie_benchmark() end_benchmark(LOOKUP_SUBGOAL_TRIE_BENCHMARK)
#endif

#ifdef INITIAL_COLLECT_BENCHMARK
#define start_initial_collect_benchmark() start_benchmark()
#define end_initial_collect_benchmark() end_benchmark(INITIAL_COLLECT_BENCHMARK)
#else
#define start_initial_collect_benchmark()
#define end_initial_collect_benchmark()
#endif

#ifdef RELEVANT_COLLECT_BENCHMARK
#define start_relevant_collect_benchmark() start_benchmark()
#define end_relevant_collect_benchmark() end_benchmark(RELEVANT_COLLECT_BENCHMARK)
#else
#define start_relevant_collect_benchmark()
#define end_relevant_collect_benchmark()
#endif

#ifdef IN_EVAL_BENCHMARK
#define start_in_eval_benchmark() start_benchmark()
#define end_in_eval_benchmark() end_benchmark(IN_EVAL_BENCHMARK)
#else
#define start_in_eval_benchmark()
#define end_in_eval_benchmark()
#endif

#ifdef TOP_GEN_BENCHMARK
#define start_top_gen_benchmark() start_benchmark()
#define end_top_gen_benchmark() end_benchmark(TOP_GEN_BENCHMARK)
#else
#define start_top_gen_benchmark()
#define end_top_gen_benchmark()
#endif

#endif /* TABLING */

#endif
