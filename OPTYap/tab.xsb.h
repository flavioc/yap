/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   R. Rocha and NCC - University of Porto, Portugal
  File:        tab.xsb.h
                                                                     
**********************************************************************/

#ifndef TAB_XSB_H
#define TAB_XSB_H

#include "opt.config.h"

#ifdef TABLING_CALL_SUBSUMPTION

typedef Term *CPtr;
typedef unsigned long int counter;
typedef Functor Psc;

#define get_cell_arity(CS_Cell) ArityOfFunctor(FunctorOfTerm(CS_Cell))
#define get_arity(FUNCTOR)  ArityOfFunctor(FunctorOfTerm(FUNCTOR))
#define clref_val(REF)  RepAppl(REF)
#define get_term_deref(REF) Deref(*(REF))

#endif /* TABLING_CALL_SUBSUMPTION */

#endif