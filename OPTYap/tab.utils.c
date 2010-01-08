/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   R. Rocha and NCC - University of Porto, Portugal
  File:        tab.utils.h
                                                                     
**********************************************************************/

#include "Yap.h"
#include "tab.utils.h"
#include "Yatom.h"
#include "tab.macros.h"

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

static inline
CellTag TrieSymbolType(Term t)
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
    Functor f = (Functor) RepAppl(t);
    if (f == FunctorDouble)
      return TAG_FLOAT;
    
    if(f == FunctorLongInt)
      return TAG_LONG_INT;
    
    return TAG_STRUCT;
  }
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

static
void printTrieNode(FILE *fp, BTNptr pTN) {
  fprintf(fp, "Trie Node: Addr(%p)", pTN);
  // XXX
}

void printTrieSymbol(FILE* fp, Cell symbol) {
  if(symbol == ESCAPE_NODE_SYMBOL)
    fprintf(fp, "%lu [ESCAPE_NODE_SYMBOL]", ESCAPE_NODE_SYMBOL);
  else {
    switch(TrieSymbolType(symbol)) {
      case XSB_INT:
        fprintf(fp, IntegerFormatString, int_val(symbol));
        break;
      case XSB_FLOAT:
        /// XXXX
        break;
      case XSB_STRING:
        fprintf(fp, "%s", string_val(symbol));
        break;
      case XSB_TrieVar:
        fprintf(fp, "V" IntegerFormatString, DecodeTrieVar(symbol));
        break;
      case XSB_STRUCT:
        {
          Psc psc;
          /// XXX Float??
          psc = DecodeTrieFunctor(symbol);
          fprintf(fp, "%s/%d", get_name(psc), get_arity(psc));
          break;
        }
      case XSB_LIST:
        // TODO
        break;
      default:
        fprintf(fp, "Unknown symbol (tag = %ld)", cell_tag(symbol));
        break;
    }
  }
}

static
void symstkPrintNextTerm(CTXTdeclc FILE *fp, xsbBool list_recursion) {
  Cell symbol;
  
  if(SymbolStack_IsEmpty) {
    fprintf(fp, "<no subterm>");
    return;
  }
  
  SymbolStack_Pop(symbol);
  
  switch(TrieSymbolType(symbol)) {
    case XSB_INT:
      if(list_recursion)
        fprintf(fp, "|" IntegerFormatString "]", int_val(symbol));
      else
        fprintf(fp, IntegerFormatString, int_val(symbol));
      break;
    case XSB_FLOAT:
      // XXX
      break;
    case XSB_STRING:
      {
        char *string = string_val(symbol);
        
        if(list_recursion) {
          if(symbol == TermNil) /// XXX Diferente!
            fprintf(fp, "]");
          else
            fprintf(fp, "|%s]", string);
        }
        else
          fprintf(fp, "%s", string);
      }
      break;
    case XSB_TrieVar:
      if(list_recursion)
        fprintf(fp, "|V" IntegerFormatString "]", DecodeTrieVar(symbol));
      else
        fprintf(fp, "V" IntegerFormatString, DecodeTrieVar(symbol));
      break;
    case XSB_STRUCT:
      {
        Psc psc;
        
        int i;
        
        /// Boxed float XXX
        if(list_recursion)
          fprintf(fp, "|");
        psc = DecodeTrieFunctor(symbol);
        
        fprintf(fp, "%s(", get_name(psc));
        for(i = 1; i < (int)get_arity(psc); i++) {
          symstkPrintNextTerm(CTXTc fp, FALSE);
          fprintf(fp, ",");
        }
        symstkPrintNextTerm(CTXTc fp, FALSE);
        fprintf(fp, ")");
        if(list_recursion)
          fprintf(fp, "]");
      }
      break;
    case XSB_LIST:
    ///XXX
    break;
    default:
      fprintf(fp, "<unknown symbol>");
      break;
  }
}

void printTriePath(CTXTdeclc FILE *fp, BTNptr pLeaf, xsbBool printLeafAddr)
{
  BTNptr pRoot;
  
  if(IsNULL(pLeaf)) {
    fprintf(fp, "NULL");
    return;
  }
  
  /*if(!IsLeafNode(pLeaf)) { // XXX
    fprintf(fp, "printTriePath() called with non-Leaf node!\n");
    printTrieNode(fp, pLeaf);
    return;
  }*/
  
  if(printLeafAddr)
    fprintf(fp, "Leaf %p: ", pLeaf);
  
  if(IsEscapeNode(pLeaf)) {
    pRoot = BTN_Parent(pLeaf);
    if(IsNonNULL(pRoot))
      printTrieSymbol(fp, BTN_Symbol(pRoot));
    else
      fprintf(fp, "ESCAPE node");
    return;
  }
  
  SymbolStack_ResetTOS;
  SymbolStack_PushPathRoot(pLeaf, pRoot);
  //printf("Num symbols: %d", SymbolStack_NumSymbols);
  
  if(IsTrieFunctor(BTN_Symbol(pRoot))) {
    //printf("Is trieFunctor!!\n");
    SymbolStack_Push(BTN_Symbol(pRoot));
    symstkPrintNextTerm(CTXTc fp, FALSE);
  }
  else {
    //printTrieSymbol(fp, BTN_Symbol(pRoot));
    fprintf(fp, "(");
    symstkPrintNextTerm(CTXTc fp, FALSE);
    while(!SymbolStack_IsEmpty) {
      fprintf(fp, ",");
      symstkPrintNextTerm(CTXTc fp, FALSE);
    }
    fprintf(fp, ")");
  }
}

#endif /* TABLING_CALL_SUBSUMPTION */