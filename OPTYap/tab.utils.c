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
        fprintf(fp, "LIST");
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
  
  if(IsIntTerm(symbol)) {
    if(list_recursion)
      fprintf(fp, "|" IntegerFormatString "]", int_val(symbol));
    else
      fprintf(fp, IntegerFormatString, int_val(symbol));
  } else if(IsVarTerm(symbol)) {
    if(list_recursion)
      fprintf(fp, "|V" IntegerFormatString "]", DecodeTrieVar(symbol));
    else
      fprintf(fp, "V" IntegerFormatString, DecodeTrieVar(symbol));
  } else if(IsAtomTerm(symbol)) {
    char *string = string_val(symbol);
    
    if(list_recursion) {
      if(symbol == TermNil)
        fprintf(fp, "]");
      else
        fprintf(fp, "|%s]", string);
    }
    else
      fprintf(fp, "%s", string);
  } else if(IsApplTerm(symbol)) {
    Functor f = (Functor) RepAppl(symbol);
    
    if(f == FunctorDouble) {
      volatile Float dbl;
      volatile Term *t_dbl = (Term *)((void *) &dbl);
      
#if SIZEOF_DOUBLE == 2 * SIZEOF_INT_P
      SymbolStack_Pop(*(t_dbl + 1));
#endif /* SIZEOF_DOUBLE x SIZEOF_INT_P */
      SymbolStack_Pop(*t_dbl);
      
      if(list_recursion)
        fprintf(fp, "|%f]", dbl);
      else
        fprintf(fp, "%f", dbl);
    } else if(f == FunctorLongInt) {
      Int li;
      SymbolStack_Pop(li);
      if(list_recursion)
        fprintf(fp, "|" LongIntFormatString "]", li);
      else
        fprintf(fp, LongIntFormatString, li);
    } else {
      int i;
      
      if(list_recursion)
        fprintf(fp, "|");
      
      fprintf(fp, "%s(", get_name(f));
      for(i = 1; i < (int)get_arity(f); i++) {
        symstkPrintNextTerm(CTXTc fp, FALSE);
        fprintf(fp, ",");
      }
      symstkPrintNextTerm(CTXTc fp, FALSE);
      fprintf(fp, ")");
      if(list_recursion)
        fprintf(fp, "]");
    }
  } else if(IsPairTerm(symbol)) {
#ifdef TRIE_COMPACT_PAIRS
    if(symbol == CompactPairInit) {
      int cnt;
      
      fprintf(fp, "[");
      
      for(cnt = 0; ; ++cnt) {
        SymbolStack_Peek(symbol);
        
        if(symbol == CompactPairEndList) {
          SymbolStack_BlindDrop;
          if(cnt)
            fprintf(fp, ",");
          symstkPrintNextTerm(CTXTc fp, FALSE);
          fprintf(fp, "]");
          break;
        } else if(symbol == CompactPairEndTerm) {
          SymbolStack_BlindDrop;
          if(cnt)
            fprintf(fp, "|");
          symstkPrintNextTerm(CTXTc fp, FALSE);
          fprintf(fp, "]");
          break;
        }
        
        if(cnt)
          fprintf(fp, ",");
        
        symstkPrintNextTerm(CTXTc fp, FALSE);
      }
    }
#else
    if(list_recursion)
      fprintf(fp, ",");
    else
      fprintf(fp, "[");
  
    symstkPrintNextTerm(CTXTc fp, FALSE);
    symstkPrintNextTerm(CTXTc fp, TRUE);
#endif
  } else {
    if(list_recursion)
      fprintf(fp, "uknown_symbol]");
    else
      fprintf(fp, "unknown_symbol");
  }
}

void printSubgoalTriePath(CTXTdeclc FILE *fp, BTNptr pLeaf, tab_ent_ptr tab_entry)
{
  BTNptr pRoot;
  
  if(IsNULL(pLeaf)) {
    fprintf(fp, "NULL");
    return;
  }
  
  SymbolStack_ResetTOS;
  SymbolStack_PushPathRoot(pLeaf, pRoot);
  
  fprintf(fp, "%s", string_val(TabEnt_atom(tab_entry)));
  fprintf(fp, "(");
  symstkPrintNextTerm(CTXTc fp, FALSE);
  while(!SymbolStack_IsEmpty) {
    fprintf(fp, ",");
    symstkPrintNextTerm(CTXTc fp, FALSE);
  }
  fprintf(fp, ")");
}

#endif /* TABLING_CALL_SUBSUMPTION */