/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   R. Rocha and NCC - University of Porto, Portugal
  File:        tab.subsumption.C 
                                                                     
**********************************************************************/

/* ------------------ **
**      Includes      **
** ------------------ */

#include "Yap.h"
#if defined(TABLING) && defined(TABLING_CALL_SUBSUMPTION)
 
#include <stdio.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#include "Yatom.h"
#include "YapHeap.h"
#include "yapio.h"
#include "tab.macros.h"
#include "tab.stack.h"

#if 0
#define SUBSUMPTION_CPSTACK_SIZE 1024

typedef struct {
  int index;             /* location within the tstTermStack... */
  CELL* value;            /* where this value appeared. */
} stack_term_log_frame;

typedef struct {
  // trilha de variaveis aqui!
  int term_log_index;
  CELL* stack_term_top; /* stack term top in the moment of frame creation */
  sg_node_ptr alt_node;
} subsumption_search_frame;

struct subsumption_search_stack {
  subsumption_search_frame *top; /* current frame */
  subsumption_search_frame *ceiling; /* frame max */
  subsumption_search_frame base[SUBSUMPTION_CPSTACK_SIZE];
};

static stack_term_log_frame stack_term_log[SUBSUMPTION_CPSTACK_SIZE];
static CELL* subsumption_vars[MAX_TABLE_VARS];
static struct subsumption_search_stack subsumption_stack;
#endif

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

static inline
CellTag cell_tag(Term t)
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

static int stack_terms_init = FALSE;
static DynamicStack stack_terms;

void subsumptive_search(yamop *preg, CELL **Yaddr)
{
  int i, j, count_vars, arity;
  CELL *stack_vars;
  tab_ent_ptr tab_ent;
  sg_node_ptr current_sg_node;
  
  if(!stack_terms_init) {
    DynStk_Init(&stack_terms, 2048, Term, "stack_terms");
    stack_terms_init = TRUE;
  }
  
  DynStk_ResetTOS(stack_terms);
  
  printf("subsumptive_search(preg, Yaddr)\n");
  
  arity = preg->u.Otapl.s;
  tab_ent = preg->u.Otapl.te;
  count_vars = 0;
  stack_vars = *Yaddr;
  current_sg_node = TabEnt_subgoal_trie(tab_ent);
  
  Term* frame;
  for (i = arity; i > 0; --i) {
    DynStk_Push(stack_terms, frame);
    *(Term*)frame = Deref(XREGS[i]);
  } 
  
  Term t;
  
  do {
    DynStk_Pop(stack_terms, frame);
    
    t = *frame;
    switch(cell_tag(t)) {
      case TAG_REF:
        printf("VAR ");
        break;
      case TAG_ATOM:
        printf("ATOM %s ", AtomName(AtomOfTerm(t)));
        break;
      case TAG_INT:
        printf("INT %d ", IntOfTerm(t));
        break;
      case TAG_STRUCT:
        printf("Appl ");
        break;
    }
  } while (!DynStk_IsEmpty(stack_terms));

  printf("\n");
}

#endif /* TABLING && TABLING_CALL_SUBSUMPTION */