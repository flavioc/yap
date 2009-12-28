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

void subsumptive_search(yamop *preg, CELL **Yaddr)
{
  int i, j, count_vars, arity;
  CELL *stack_vars, *stack_terms_limit, *stack_terms_base, *stack_terms;
  tab_ent_ptr tab_ent;
  sg_node_ptr current_sg_node;
  
  printf("subsumptive_search(preg, Yaddr)\n");
  
  arity = preg->u.Otapl.s;
  tab_ent = preg->u.Otapl.te;
  count_vars = 0;
  stack_vars = *Yaddr;
  stack_terms_limit = (CELL *)TR;
  stack_terms_base = stack_terms = (CELL *)Yap_TrailTop;
  current_sg_node = TabEnt_subgoal_trie(tab_ent);
  
  for (i = arity; i > 0; --i) {
    STACK_CHECK_EXPAND(stack_terms, stack_terms_limit, stack_terms_base);
    STACK_PUSH_UP(Deref(XREGS[i]), stack_terms);
  } 
  
  do {
    Term t = STACK_POP_DOWN(stack_terms);
      
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
  } while (STACK_NOT_EMPTY(stack_terms, stack_terms_base));

  printf("\n");
}

#endif /* TABLING && TABLING_CALL_SUBSUMPTION */