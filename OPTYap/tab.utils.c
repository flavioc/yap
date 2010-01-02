/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   R. Rocha and NCC - University of Porto, Portugal
  File:        tab.utils.h
                                                                     
**********************************************************************/

#include "Yap.h"
#include "tab.utils.h"
#include "Yatom.h"

#ifdef TABLING_CALL_SUBSUMPTION

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

xsbBool are_identical_terms(Cell term1, Cell term2)
{
begin_are_identical_terms:
  XSB_Deref(term1);
  XSB_Deref(term2);
  
  if(term1 == term2)
    return TRUE;
  
  if(cell_tag(term1) != cell_tag(term2))
    return FALSE;
  
  if(cell_tag(term1) == XSB_STRUCT) {
    CPtr cptr1 = clref_val(term1);
    CPtr cptr2 = clref_val(term2);
    Psc psc1 = (Psc)*cptr1;
    int i;
    
    if(psc1 != (Psc)*cptr2)
      return FALSE;
    
    for(cptr1++, cptr2++, i = 0; i < (int)get_arity(psc1)-1; cptr1++, cptr2++, i++ )
      if(!are_identical_terms(*cptr1, *cptr2))
        return FALSE;
    term1 = *cptr1;
    term2 = *cptr2;
    goto begin_are_identical_terms;  
  }
  else if (cell_tag(term1) == XSB_LIST) {
    CPtr cptr1 = clrefp_val(term1);
    CPtr cptr2 = clrefp_val(term2);
    
    if(are_identical_terms(*cptr1, *cptr2)) {
      term1 = *(cptr1 + 1);
      term2 = *(cptr2 + 1);
      
      goto begin_are_identical_terms;
    } else return FALSE;
  }
  else return FALSE;
  // TODO: long int e double
}

#endif /* TABLING_CALL_SUBSUMPTION */