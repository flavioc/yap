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

static void printTrieSymbol(FILE* fp, Cell symbol);

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
#ifdef SUBSUMPTION_YAP
  else if(cell_tag(term1) == TAG_LONG_INT) {
    dprintf("identical long int\n");
    Int v1 = LongIntOfTerm(term1);
    Int v2 = LongIntOfTerm(term2);
    
    return v1 == v2;
  }
  else if(cell_tag(term1) == TAG_FLOAT) {
    dprintf("identical float term\n");
    
    Float dbl1 = FloatOfTerm(term1);
    Float dbl2 = FloatOfTerm(term2);
    
    return dbl1 == dbl2;
  }
#endif /* SUBSUMPTION_YAP */
  else return FALSE;
}

void printTrieNode(FILE *fp, BTNptr pTN)
{
  fprintf(fp, "Trie Node: Addr(%p) Symbol: ", pTN);
  printTrieSymbol(fp, BTN_Symbol(pTN));
}

static void printTrieSymbol(FILE* fp, Cell symbol)
{
  switch(TrieSymbolType(symbol)) {
    case XSB_INT:
      fprintf(fp, IntegerFormatString, int_val(symbol));
      break;
    case XSB_FLOAT:
      fprintf(fp, "float");
      break;
    case XSB_STRING:
      fprintf(fp, "%s", string_val(symbol));
      break;
    case XSB_TrieVar:
      fprintf(fp, "VAR" IntegerFormatString, DecodeTrieVar(symbol));
      break;
    case XSB_STRUCT:
      {
        Psc psc;

        psc = DecodeTrieFunctor(symbol);
        fprintf(fp, "%s/%d", get_name(psc), get_arity(psc));
        break;
      }
    case XSB_LIST:
      fprintf(fp, "LIST");
      break;
    case TAG_LONG_INT:
      fprintf(fp, "LONG INT");
      break;
    default:
      fprintf(fp, "Unknown symbol (tag = %d)", cell_tag(symbol));
      break;
  }
}

static
void symstkPrintNextTrieTerm(CTXTdeclc FILE *fp, xsbBool list_recursion)
{
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
      fprintf(fp, "|VAR" IntegerFormatString "]", DecodeTrieVar(symbol));
    else
      fprintf(fp, "VAR" IntegerFormatString, DecodeTrieVar(symbol));
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
        fprintf(fp, "|" FloatFormatString "]", dbl);
      else
        fprintf(fp, FloatFormatString, dbl);
        
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
        symstkPrintNextTrieTerm(CTXTc fp, FALSE);
        fprintf(fp, ",");
      }
      symstkPrintNextTrieTerm(CTXTc fp, FALSE);
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
          symstkPrintNextTrieTerm(CTXTc fp, FALSE);
          fprintf(fp, "]");
          break;
        } else if(symbol == CompactPairEndTerm) {
          SymbolStack_BlindDrop;
          if(cnt)
            fprintf(fp, "|");
          symstkPrintNextTrieTerm(CTXTc fp, FALSE);
          fprintf(fp, "]");
          break;
        }
        
        if(cnt)
          fprintf(fp, ",");
        
        symstkPrintNextTrieTerm(CTXTc fp, FALSE);
      }
    }
#else
    if(list_recursion)
      fprintf(fp, ",");
    else
      fprintf(fp, "[");
  
    symstkPrintNextTrieTerm(CTXTc fp, FALSE);
    symstkPrintNextTrieTerm(CTXTc fp, TRUE);
#endif
  } else {
    if(list_recursion)
      fprintf(fp, "uknown_symbol]");
    else
      fprintf(fp, "unknown_symbol");
  }
}


void printTriePath(CTXTdeclc FILE *fp, BTNptr pLeaf, xsbBool print_address)
{
  if(print_address)
    fprintf(fp, "Leaf Address: %x ", (unsigned int)pLeaf);
  
  BTNptr pRoot;
  
  SymbolStack_ResetTOS;
  SymbolStack_PushPathRoot(pLeaf, pRoot);
  
  fprintf(fp, "(");
  symstkPrintNextTrieTerm(CTXTc fp, FALSE);
  while(!SymbolStack_IsEmpty) {
    fprintf(fp, ",");
    symstkPrintNextTrieTerm(CTXTc fp, FALSE);
  }
  fprintf(fp, ")");
}

void printSubgoalTriePath(CTXTdeclc FILE *fp, BTNptr pLeaf, tab_ent_ptr tab_entry)
{
  fprintf(fp, "%s", string_val((Term)TabEnt_atom(tab_entry)));
  
  printTriePath(fp, pLeaf, NO);
}

void printAnswerTriePath(FILE *fp, ans_node_ptr leaf)
{
  SymbolStack_ResetTOS;
  SymbolStack_PushPath(leaf);
  
  fprintf(fp, "{");
  symstkPrintNextTrieTerm(CTXTc fp, FALSE);
  while(!SymbolStack_IsEmpty) {
    fprintf(fp, ",");
    symstkPrintNextTrieTerm(CTXTc fp, FALSE);
  }
  fprintf(fp, "}");
}

static int variable_counter = 0;

static void
recursivePrintSubterm(FILE *fp, Term symbol, xsbBool list_recursion)
{
  XSB_Deref(symbol);
  
  if(IsIntTerm(symbol)) {
    if(list_recursion)
      fprintf(fp, "|" IntegerFormatString "]", int_val(symbol));
    else
      fprintf(fp, IntegerFormatString, int_val(symbol));
  } else if(IsVarTerm(symbol)) {
    if(list_recursion)
      fprintf(fp, "|");
    
    int var_index;
            
    if(IsStandardizedVariable(symbol)) {
      var_index = IndexOfStdVar(symbol);
    } else {
      Trail_Push(symbol);
      StandardizeVariable(symbol, variable_counter);
      var_index = variable_counter++;
    }
    
    fprintf(fp, "VAR" IntegerFormatString, var_index);  
    
    if(list_recursion)
      fprintf(fp, "]");
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
    Functor f = FunctorOfTerm(symbol);
    
    if(f == FunctorDouble) {
      Float dbl = FloatOfTerm(symbol);
      
      if(list_recursion)
        fprintf(fp, "|" FloatFormatString "]", dbl);
      else
        fprintf(fp, FloatFormatString, dbl);
    } else if(f == FunctorLongInt) {
      Int li = LongIntOfTerm(symbol);
      
      if(list_recursion)
        fprintf(fp, "|" LongIntFormatString "]", li);
      else
        fprintf(fp, LongIntFormatString, li);
    } else {
      int i;
      
      if(list_recursion)
        fprintf(fp, "|");
      
      fprintf(fp, "%s(", get_name(f));
      
      for(i = 1; i <= (int)get_arity(f); i++) {
        if(i > 1)
          fprintf(fp, ",");
        recursivePrintSubterm(fp, *(RepAppl(symbol) + i), FALSE);
      }
      
      fprintf(fp, ")");
      
      if(list_recursion)
        fprintf(fp, "]");
    }
  } else if(IsPairTerm(symbol)) {
    if(list_recursion)
      fprintf(fp, ",");
    else
      fprintf(fp, "[");
    
    recursivePrintSubterm(fp, *(RepPair(symbol)), FALSE);
    recursivePrintSubterm(fp, *(RepPair(symbol) + 1), TRUE);
  }
}

void printSubterm(FILE *fp, Term term)
{
  int bindings = Trail_NumBindings;
  
  variable_counter = 0;
  recursivePrintSubterm(fp, term, FALSE);
  
  Trail_Unwind(bindings);
}

void printCalledSubgoal(FILE *fp, yamop *preg)
{
  int bindings = Trail_NumBindings;
  int i, arity = preg->u.Otapl.s;
  tab_ent_ptr tab_ent = preg->u.Otapl.te;
  
  variable_counter = 0;
  
  fprintf(fp, "SUBGOAL: %s(", string_val((Term)TabEnt_atom(tab_ent)));
  for(i = 1; i <= arity; ++i) {
    if(i > 1)
      fprintf(fp, ",");
    recursivePrintSubterm(fp, XREGS[i], FALSE);
  }
  fprintf(fp, ")");
  
  Trail_Unwind(bindings);
}

void printAnswerTemplate(FILE *fp, CPtr ans_tmplt, int size)
{
  int bindings = Trail_NumBindings;
  
  fprintf(fp, "[");
  variable_counter = 0;
  
  for(size--; size >= 0; size--) {
    recursivePrintSubterm(fp, (Term)(ans_tmplt+size), FALSE);
    
    if(size > 0)
      fprintf(fp, ", ");
  }
  
  fprintf(fp, "]\n");
  
  Trail_Unwind(bindings);
}

void printSubstitutionFactor(FILE *fp, CELL* factor)
{
  int size = (int)*factor;
  
  fprintf(fp, "Substitution factor with size %d ", size);
  
  printAnswerTemplate(fp, factor + 1, size);
}

static CELL*
recursive_construct_subgoal(CELL* trie_vars)
{
  CELL symbol;
  
  SymbolStack_Pop(symbol);
  
  if(IsAtomOrIntTerm(symbol)) {
    dprintf("New constant\n");
    *H = symbol;
    return H++;
  } else if(IsVarTerm(symbol)) {
    int index = DecodeTrieVar(symbol);
    
    if(IsNewTableVarTerm(symbol)) {
      *H = (CELL)H;
      *(trie_vars - index) = (CELL)H;
      
      ++variable_counter;
    } else {
      *H = *(trie_vars - index);
    }
    
    dprintf("New var\n");
    
    return H++;
  } else if(IsApplTerm(symbol)) {
    Functor f = DecodeTrieFunctor(symbol);

    if(f == FunctorDouble) {
      // XXX IMPLEMENT
    } else if(f == FunctorLongInt) {
      // XXX IMPLEMENT
    } else {
      int i, arity = ArityOfFunctor(f);
      
      *H = AbsAppl(H + 1);
      ++H;
      *H++ = (CELL)f;
      
      CELL* arguments = H;
      H += arity;

      for(i = 0; i < arity; ++i) {
        *(arguments + i) = recursive_construct_subgoal(trie_vars);
      }
      
      return arguments - 2;
    }
  } else if(IsPairTerm(symbol)) {
    CELL* arguments = H + 1;
    
    *H = AbsPair(H + 1);
    H += 3;
    
    *(arguments + 0) = recursive_construct_subgoal(trie_vars);
    *(arguments + 1) = recursive_construct_subgoal(trie_vars);
    
    return arguments - 1;
  } else {
    dprintf("BIG ERROR!!!\n");
  }
}

static void
recursive_put_argument_stack(CELL *trie_vars)
{
  CELL *argument = recursive_construct_subgoal(trie_vars);
  
  if(!SymbolStack_IsEmpty)
    recursive_put_argument_stack(trie_vars);
    
  TermStack_Push(*argument);
}

CELL* construct_subgoal_heap(BTNptr pLeaf, CPtr* var_pointer)
{
  CELL* orig_hreg = H;
  CELL* trie_vars = *var_pointer;
  
  variable_counter = 0;
  
  TermStack_ResetTOS;
  SymbolStack_ResetTOS;
  SymbolStack_PushPath(pLeaf);
  
  recursive_put_argument_stack(trie_vars);
  *var_pointer = trie_vars - variable_counter;
  **var_pointer = variable_counter;
  
  /* return original H register */
  return orig_hreg;
}

CPtr reconstruct_template_for_producer_no_args(SubProdSF subsumer, CPtr ans_tmplt) {
  int sizeAnsTmplt;
  Cell subterm, symbol;
  
  /*
   * Store the symbols along the path of the more general call.
   */
  SymbolStack_ResetTOS;
  SymbolStack_PushPath(subg_leaf_ptr(subsumer));
    
  /*
   * Create the answer template while we process.  Since we know we have a
   * more general subsuming call, we can greatly simplify the "matching"
   * process: we know we either have exact matches of non-variable symbols
   * or a variable paired with some subterm of the current call.
   */
  sizeAnsTmplt = 0;
  while(!TermStack_IsEmpty) {
    TermStack_Pop(subterm);
    XSB_Deref(subterm);
    SymbolStack_Pop(symbol);
    if(IsTrieVar(symbol) && IsNewTrieVar(symbol)) {
      *ans_tmplt-- = subterm;
      sizeAnsTmplt++;
    }
    else if(IsTrieFunctor(symbol))
      TermStack_PushFunctorArgs(subterm)
    else if(IsTrieList(symbol))
      TermStack_PushListArgs(subterm)
  }
  *ans_tmplt = makeint(sizeAnsTmplt);
  return ans_tmplt;
}

void
printTermStack(FILE *fp)
{
  int bindings = Trail_NumBindings;
  CELL* start = TermStack_Base;
  CELL* end = TermStack_Top;
  int total = TermStack_NumTerms - 1;
  
  variable_counter = 0;
  
  fprintf(fp, "(");
  for(--end; end >= start; --end, --total) {
    recursivePrintSubterm(fp, *end, FALSE);
    if(total)
      fprintf(fp, ",");
  }
  fprintf(fp, ")");
  
  Trail_Unwind(bindings);
}

void printSubsumptiveAnswer(FILE *fp, CELL* vars)
{
  int total = (int)*vars;
  vars += total;
  int i = 0;
  
  fprintf(fp, "    ");
  for(i = 0; i < total; ++i, --vars) {
    if(i)
      fprintf(fp, "    ");
    
    fprintf(fp, "VAR%d: ", i);
    recursivePrintSubterm(fp, *vars, FALSE);
  }
  
  fprintf(fp, "\n");
}

#endif /* TABLING_CALL_SUBSUMPTION */