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
#ifdef TABLING_CALL_SUBSUMPTION
 
#include <stdio.h>
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#include "Yatom.h"
#include "YapHeap.h"
#include "yapio.h"
#include "tab.macros.h"
#include "tab.stack.h"
#include "tab.xsb.h"
#include "tab.utils.h"

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
  int i, count_vars, arity;
  CELL *stack_vars;
  tab_ent_ptr tab_ent;
  sg_node_ptr current_sg_node;
  
  TermStack_ResetTOS;
  
  printf("subsumptive_search(preg, Yaddr)\n");
  
  arity = preg->u.Otapl.s;
  tab_ent = preg->u.Otapl.te;
  count_vars = 0;
  stack_vars = *Yaddr;
  current_sg_node = TabEnt_subgoal_trie(tab_ent);
  
  for (i = arity; i > 0; --i) {
    TermStack_Push(Deref(XREGS[i]));
  } 
  
  Term t;
  
  do {
    TermStack_Pop(t);
  
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
      default:
        break;
    }
  } while (!TermStack_IsEmpty);

  printf("\n");
}

#endif /* TABLING && TABLING_CALL_SUBSUMPTION */