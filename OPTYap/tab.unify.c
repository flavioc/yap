/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   Flávio Cruz
  File:        tab.unify.c
  version:        
  comment:     from XSB's emu/tst_unify.c                                
**********************************************************************/

/* ------------------ **
**      Includes      **
** ------------------ */

#include "Yap.h"
#ifdef TABLING
#include <stdio.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#include "Yatom.h"
#include "YapHeap.h"
#include "yapio.h"
#include "tab.macros.h"
#include "tab.utils.h"
#include "tab.tst.h"
#include "tab.utils.h"
#include "tab.xsb_common.h"

/* ========================================================================= */


/* Binding and Trailing
   -------------------- */

/*
 *  Bind the variable 'Symbol', obtained from a deref of a trie symbol, to
 *  the non-variable term 'Subterm'.  Trail the variable Symbol, which may
 *  either be a trie var or a prolog var.  If a trie var, then trail
 *  locally; if a prolog var, then trail on the WAM trail.
 */
#define Bind_and_Trail_Symbol(Symbol,Subterm) { \
  if(IsUnboundTrieVar(Symbol)) {  \
    Trail_Push(Symbol); \
    bld_ref((CPtr)Symbol,Subterm);  \
  } \
  else  \
    Bind_and_Trail_Subterm(Symbol,Subterm)  \
}

/*
 *  Bind the variable 'Subterm', obtained from a deref of a variable
 *  in the answer template, to the non-variable term 'Symbol'.
 *  Conditionally trail 'Subterm' on the system trail.
 */
#define Bind_and_Trail_Subterm(Subterm,Symbol) {  \
    bind_ref((CPtr)Subterm,Symbol);  \
 }
 
/*
 *  Given two variables, one derived from a trie var and the other from
 *  the answer template, determine in which direction to bind one to the
 *  other and whether (and what kind) of trailing is required.  TrieVars
 *  are always bound to Prolog vars and are locally trailed.
 */
#define Bind_and_Trail_Vars(UnknownVar,PrologVar) { \
  if(IsUnboundTrieVar(UnknownVar)) {  \
    Trail_Push(UnknownVar)  \
    bld_ref((CPtr)UnknownVar,PrologVar);  \
  } \
  else  \
    unify(CTXTc UnknownVar,PrologVar);  \
}

/* ========================================================================= */

/* Error Reporting
   --------------- */

#ifndef MULTI_THREAD
static BTNptr gAnsLeaf;    /* answer to consume */
static CPtr gAnsTmplt;      /* ... using this template */
static int gSizeTmplt;      /* ... of this size */
#endif

static void consumption_error(CTXTdeclc char *string) {
  
  char *abort_string;
  
#ifdef MULTI_THREAD
  fprintf(stderr,"This error has occurred, but the following diagnostics may be wrong due to global variables\n");
#endif
  fprintf(stderr,"\nAnswer Return ERROR: Failed to unify answer\n\t");
#ifdef DEBUG_VERBOSE
  printTriePath(stderr,gAnsLeaf,YES);
#else
  printTriePath(stderr,gAnsLeaf,NO);
#endif
  fprintf(stderr,"\nwith ");
  printAnswerTemplate(stderr,gAnsTmplt,gSizeTmplt);
  fprintf(stderr,
    "(ª Note: this template may be partially instantiated *)\n");
#ifdef DEBUG_ASSERTIONS
  /* Get Consumer SF from the CPS, using the ptr to AnsTmplt */
  {
    VariantSF pSF;
    CPtr pCPF;

    pCPF = gAnsTmplt - gSizeTmplt - NLCP_SIZE;
    pSF = (VariantSF)nlcp_subgoal_ptr(pCPF);
    printAnswerList(stderr,subg_answers(pSF));
  }
  abort_string = "";
#else
  abort_string = string;
#endif
  Trail_Unwind_All; /* unbind TrieVarBindings[] elements */
  printf("ABORT: %s\n", abort_string);
  xsb_abort(abort_string);
}

/* ========================================================================= */


/*
 *                   A L G O R I T H M I C   F U N C T I O N S
 *                   ===================================
 */

/*
 *  Given that the current subterm of the Answer Template contains a
 *  constant (int, float, or string), unify it with the current symbol of
 *  the trie.  Both have already been dereferenced.
 */

static inline xsbBool
Unify_Symbol_With_Constant_Subterm(Cell subterm, Cell symbol) {
  if (isref(symbol))
    Bind_and_Trail_Symbol(symbol,subterm)
  else if (symbol != subterm) {
    consumption_error(CTXTc "Unequal Constants");
    return FALSE;
  }
  return TRUE; 
}

/* ------------------------------------------------------------------------- */

/*
 *  Given that the current subterm of the Answer Template contains a
 *  function application, unify it with the current symbol of the trie.
 */

static inline xsbBool
Unify_Symbol_With_Functor_Subterm(CTXTdeclc Cell subterm, Cell symbol, Cell sym_orig_tag) {
  switch(cell_tag(symbol)) {
    case XSB_STRUCT:
      /*
       * Need to be careful here, because TrieVars can be bound to heap-
       * resident structures and a deref of the (trie) symbol doesn't
       * tell you whether we have something in the trie or in the heap.
       * Check that the same PSC Record is referred to by both.
       */
      if ( sym_orig_tag == XSB_STRUCT ) {
        if ( get_str_psc(subterm) == DecodeTrieFunctor(symbol) ) {
	        /*
	         *  There's a corresponding function application in the trie, so
	         *  we must process the rest of the term ourselves.
	         */
	        TermStack_PushFunctorArgs(subterm);
        }
        else {
	        consumption_error(CTXTc "Distinct Functor Symbols");
	        return FALSE;
        }
      }
      else {
        /*
	       * We have a TrieVar bound to a heap STRUCT-term; use a standard
	       * unification algorithm to check the match and perform additional
	       * unifications.
	       */
        if ( ! unify(CTXTc subterm, symbol) ) {
	        consumption_error(CTXTc "Distinct Function Applications");
          return FALSE;
        }
      }
      break;

    case XSB_REF:
#ifdef SUBSUMPTION_XSB
    case XSB_REF1:
#endif
      /*
       * Either an unbound TrieVar or some unbound heap var.  We bind the
       * variable to the entire subterm (functor + args), so we don't need
       * to process its args.
       */
      Bind_and_Trail_Symbol(symbol,subterm)
      break;

    default:
      consumption_error(CTXTc
		       "Trie symbol fails to unify with functor subterm");
      return FALSE;
  }
  return TRUE;
}

/* ------------------------------------------------------------------------- */

/*
 *  Given that the current subterm of the Answer Template contains a
 *  list, unify it with the current symbol of the trie.
 */
static inline xsbBool
Unify_Symbol_With_List_Subterm(CTXTdeclc Cell subterm, Cell symbol, Cell sym_orig_tag) {
  switch(cell_tag(symbol)) {
    case XSB_LIST:
      /*
       * Need to be careful here, because TrieVars can be bound to heap-
       * resident structures and a deref of the (trie) symbol doesn't
       * tell you whether we have something in the trie or in the heap.
       */
      if ( sym_orig_tag == XSB_LIST ) {
        /*
	       *  There's a corresponding list structure in the trie, so we must
	       *  process the rest of the term ourselves.
	       */
        TermStack_PushListArgs(subterm);
      }
      else {
        /*
	       * We have a TrieVar bound to a heap STRUCT-term; use a standard
	       * unification algorithm to check the match and perform additional
	       * unifications.
	       */
        if ( ! unify(CTXTc subterm, symbol) ) {
	        consumption_error(CTXTc "Distinct Lists");
          return FALSE;
        }
      }
      break;

    case XSB_REF:
#ifdef SUBSUMPTION_XSB
    case XSB_REF1:
#endif
      /*
       * Either an unbound TrieVar or some unbound heap var.  We bind the
       * variable to the entire subterm ([First | Rest]), so we don't need
       * to process its args.
       */
      Bind_and_Trail_Symbol(symbol,subterm)
      break;

    default:
      consumption_error(CTXTc
			  "Trie symbol fails to unify with list subterm");
      return FALSE;
  }
  return TRUE;
}

/* ------------------------------------------------------------------------- */

/*
 *  Given that the current subterm of the Answer Template contains a
 *  variable, unify it with the current symbol of the trie.
 */

static inline xsbBool
Unify_Symbol_With_Variable_Subterm(CTXTdeclc Cell subterm, Cell symbol, Cell sym_orig_tag) {
  switch(cell_tag(symbol)) {
    case XSB_INT:
#ifdef SUBSUMPTION_XSB
   case XSB_FLOAT:
#endif
    case XSB_STRING:
      Bind_and_Trail_Subterm(subterm,symbol)
      break;

    case XSB_STRUCT:
      /*
       * Need to be careful here, because TrieVars can be bound to heap-
       * resident structures and a deref of the (trie) symbol doesn't
       * tell you whether we have something in the trie or in the heap.
       */
      if ( sym_orig_tag == XSB_STRUCT ) {
        /*
	       *  Since the TST contains some f/n, create an f(X1,X2,...,Xn)
	       *  structure on the heap so that we can bind the subterm
	       *  variable to it.  Then use this algorithm to find bindings
	       *  for the unbound variables X1,...,Xn along the trie path.
	       */
	      Bind_and_Trail_Subterm(subterm, (Cell)hreg)
        CreateHeapFunctor(symbol);
      }
      else {
        /*
	       *  We have a TrieVar bound to a heap-resident XSB_STRUCT.
	       */
        Bind_and_Trail_Subterm(subterm, symbol)
      }
      break;

    case XSB_LIST:
      if ( sym_orig_tag == XSB_LIST ) {
        /*
	       *  Since the TST contains a (sub)list beginning, create a
	       *  [X1|X2] structure on the heap so that we can bind the subterm
	       *  variable to it.  Then use this algorithm to find bindings for
	       *  the unbound variables X1 & X2 along the trie path.
	       */
        Bind_and_Trail_Subterm(subterm, (Cell)hreg)
        CreateHeapList();
      }
      else {
        /*
	       *  We have a TrieVar bound to a heap-resident XSB_LIST.
	       */
        Bind_and_Trail_Subterm(subterm, symbol)
      }
      break;

    case XSB_REF:
#ifdef SUBSUMPTION_XSB
    case XSB_REF1:
#endif
      /*
       *  The symbol is either an unbound TrieVar or some unbound heap
       *  variable.  If it's an unbound TrieVar, we bind it to the heap
       *  var.  Otherwise, the direction of binding is high mem to low.
       */
      Bind_and_Trail_Vars(symbol,subterm)
      break;
   
    default:
      consumption_error(CTXTc "Unsupported tag on trie node symbol");
      return FALSE;
  }
  return TRUE;
}


/* ========================================================================= */

/*
 *  Given a pointer to the answer template (a high-to-low memory vector),
 *  its size, and an answer leaf, unify the corresponding terms of each
 *  using the system stacks.  Variables that become bound are
 *  conditionally trailed while these values may be built on the heap.
 *  In this way, the bindings are readied for use by the engine, i.e., an
 *  answer is returned to a subsumed call.
 *
 *  Trie variables -- elements of the TrieVarBindings[] array -- may also
 *  become bound.  The bindings are needed during this operation but
 *  these variables should be unbound before leaving this function to
 *  ready TrieVarBindings[] for the next tabling operation.  The tstTrail
 *  is used to note these bindings for untrailing.
 */

void consume_subsumptive_answer(CTXTdeclc BTNptr pAnsLeaf, int sizeTmplt,
				CPtr pAnsTmplt) {

  Cell subterm, symbol, sym_orig_tag;
  xsbBool success;

  /* Set globals for error reporting
     ------------------------------- */
  gAnsLeaf = pAnsLeaf;
  gAnsTmplt = pAnsTmplt;
  gSizeTmplt = sizeTmplt;

  if ( ! IsLeafNode(pAnsLeaf) ) {
    consumption_error(CTXTc "Bad answer handle");
    return;
  }
#ifdef SUBSUMPTION_XSB
#ifndef MULTI_THREAD
  NumSubOps_AnswerConsumption++;
#else
#ifdef NON_OPT_COMPILE
  NumSubOps_AnswerConsumption++;
#else
#endif
#endif
#endif /* SUBSUMPTION_XSB */

  /* Initialize Data Structs
     ----------------------- */
  Trail_ResetTOS;

  TermStack_ResetTOS;
  TermStack_PushHighToLowVector(pAnsTmplt,sizeTmplt);

  SymbolStack_ResetTOS;
  SymbolStack_PushPath(pAnsLeaf);

  /* Consume the Answer
     ------------------ */
  while ( ! TermStack_IsEmpty ) {
    TermStack_Pop(subterm);
    XSB_Deref(subterm);
    SymbolStack_Pop(symbol);
    sym_orig_tag = cell_tag(symbol);
    TrieSymbol_Deref(symbol);
    switch ( cell_tag(subterm) ) {
    case XSB_INT:
    case XSB_STRING:
#ifdef SUBSUMPTION_XSB
    case XSB_FLOAT:
#endif
      success = Unify_Symbol_With_Constant_Subterm(subterm,symbol);
      break;

    case XSB_REF:
#ifdef SUBSUMPTION_XSB
    case XSB_REF1:
#endif
      success = Unify_Symbol_With_Variable_Subterm(CTXTc subterm,symbol,sym_orig_tag);
      break;

    case XSB_STRUCT:
      success = Unify_Symbol_With_Functor_Subterm(CTXTc subterm,symbol,sym_orig_tag);
      break;

    case XSB_LIST:
      success = Unify_Symbol_With_List_Subterm(CTXTc subterm,symbol,sym_orig_tag);
      break;

    default:
      consumption_error(CTXTc "Unsupported subterm tag");
      return;
    }
    
    if(!success)
      return;
  }
  Trail_Unwind_All;  /* unbind TrieVarBindings[] elements */
}
#endif /* TABLING */
