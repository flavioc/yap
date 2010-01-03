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

typedef enum Search_Strategy_Mode {
  MATCH_SYMBOL_EXACTLY, MATCH_WITH_TRIEVAR
} SearchMode;

/**** Use these to access the frame to which `top' points ****/
#define CPF_AlternateNode ((tstCCPStack.top)->alt_node)
#define CPF_VariableChain ((tstCCPStack.top)->var_chain)
#define CPF_TermStackTopIndex ((tstCCPStack.top)->termstk_top_index)
#define CPF_TermStackLogTopIndex ((tstCCPStack.top)->log_top_index)
#define CPF_TrailTopIndex ((tstCCPStack.top)->trail_top_index)

/**** TST CP Stack Operations ****/
#define CPStack_ResetTOS  tstCCPStack.top = tstCCPStack.base
#define CPStack_IsEmpty   (tstCCPStack.top == tstCCPStack.base)
#define CPStack_IsFull    (tstCCPStack.top == tstCCPStack.ceiling)
#define CPStack_OverflowCheck \
  if(CPStack_IsFull)  \
    SubsumptiveTrieLookupError("tstCCPStack overflow!")

/*
 *  All that is assumed on CP creation is that the term stack and log have
 *  been altered for the subterm under consideration.  Any additional changes
 *  necessary to continue the search, e.g., pushing functor args or trailing
 *  a variable, must be made AFTER a CPF is pushed.  In this way, the old
 *  state is still accessible.
 */
#define CPStack_PushFrame(AltNode, VarChain)  { \
    CPStack_OverflowCheck; \
    CPF_AlternateNode = AltNode; \
    CPF_VariableChain = VarChain;  \
    CPF_TermStackTopIndex =  \
      TermStack_Top - TermStack_Base + 1;  \
    CPF_TermStackLogTopIndex =  \
      TermStackLog_Top - TermStackLog_Base - 1; \
    CPF_TrailTopIndex = Trail_Top - Trail_Base; \
    tstCCPStack.top++;  \
}

/*
 *  Resume the state of a saved point of choice.
 */
#define CPStack_PopFrame(CurNode, VarChain) { \
   tstCCPStack.top--; \
   CurNode = CPF_AlternateNode; \
   VarChain = CPF_VariableChain;  \
   TermStackLog_Unwind(CPF_TermStackLogTopIndex); \
   TermStack_SetTOS(CPF_TermStackTopIndex); \
   Trail_Unwind(CPF_TrailTopIndex); \
}

/*
 * This assumes that the state of the search is being saved when its
 * ONLY modification since the last successful match is the pop of the
 * failed-to-match subterm off of the TermStack (and a note in the
 * TermStackLog).  No additional bindings, stack ops, etc., have
 * occurred.  This assumes some knowledge of the workings of the
 * TermStack and Trail: that the top pointer points to the next
 * available location in the stack.
 */

static xsbBool save_variant_continuation(CTXTdeclc BTNptr last_node_match) {
  // FIXME
  return FALSE;
}

/*
 *  Given a TrieVar number and a marked PrologVar (bound to a
 *  VarEnumerator cell), bind the TrieVar to the variable subterm
 *  represented by the marked PrologVar, and trail the TrieVar.
 */
#define TrieVar_BindToSubterm(TrieVarNum, Subterm)  \
    TrieVarBindings[TrieVarNum] = Subterm; \
    Trail_Push(&TrieVarBindings[TrieVarNum])

/*
 *  Given a TrieVar number and a marked PrologVar (bound to a
 *  VarEnumerator cell), bind the TrieVar to the variable subterm
 *  represented by the marked PrologVar, and trail the TrieVar.
 */
#define TrieVar_BindToMarkedPrologVar(TrieVarNum, PrologVarMarker)  \
  TrieVarBindings[TrieVarNum] = \
    TrieVarBindings[PrologVar_Index(PrologVarMarker)];  \
  Trail_Push(&TrieVarBindings[TrieVarNum])
  
/*
 *  Given an address into VarEnumerator, determine its index in this array.
 *  (This index will also correspond to the first trie variable that bound
 *   itself to it.)
 */
#define PrologVar_Index(VarEnumAddr)  IndexOfStdVar(VarEnumAddr)

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* Manipulating Choice Points During the Search
   -------------------------------------------- */

/*
 * Conditional Choice Points are used after successfully finding an
 * exact match between a non-variable Prolog subterm and a symbol in the
 * trie.  The condition is whether a TrieVar exists among the siblings
 * which could alternately be unified with the subterm.  This check can
 * be performed quickly and may result in the avoidance of choice point
 * creation.  (Whether the discovered variable is relevant is another
 * matter...)
 */
 
#define Conditionally_Create_ChoicePoint(VariableChain) { \
  BTNptr alternateBTN = VariableChain; \
    \
  while (IsNonNULL(alternateBTN)) { \
    if(IsTrieVar(BTN_Symbol(alternateBTN))) { \
      CPStack_PushFrame(alternateBTN, VariableChain)  \
      break;  \
    } \
    alternateBTN = BTN_Sibling(alternateBTN); \
  } \
}

/*
 * We unconditionally lay a Choice Point whenever we've found a trievar
 * binding identical to the call's subterm.  A computation which may
 * result in avoiding CPF creation would be expensive (and useless if we
 * never return to this CPF), so it might be best to just lay one and
 * take your chances that we never use it.  If we do use it, however,
 * the additional overhead of having the main algorithm discover that it
 * is (nearly) useless isn't too high compared to the computation we
 * could have done.  The benefit is that we don't do it unless we have to.
 */
 
#define Create_ChoicePoint(AlternateBTN, VariableChain) \
  CPStack_PushFrame(AlternateBTN, VariableChain)
  
/*
 * Resuming a point of choice additionally involves indicating to the
 * search algorithm what sort of pairings should be sought.
 */
#define BacktrackSearch { \
    CPStack_PopFrame(pCurrentBTN, variableChain);  \
    search_mode = MATCH_WITH_TRIEVAR; \
 }

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*
 *  Given an unmarked, dereferenced Prolog variable and a TrieVar number,
 *  mark the variable with this number by setting it to point to the
 *  Index-th cell of the VarEnumerator array, and trail the variable.
 */
#define PrologVar_MarkIt(DerefedVar, Index) \
  StandardizeVariable(DerefedVar, Index);  \
  Trail_Push((CPtr)DerefedVar)

/*
 *  Given a dereferenced Prolog variable, determine whether it has already
 *  been marked, i.e. seen during prior processing and hence bound to a
 *  VarEnumerator cell.
 */
#define PrologVar_IsMarked(pDerefedPrologVar) \
    IsStandardizedVariable(pDerefedPrologVar)

/*
 *  When first stepping onto a particular trie level, we may find
 *  ourselves either looking at a hash table header or a trie node in a
 *  simple chain.  Given the object first encountered at this level
 *  (pointed to by 'pCurrentBTN') and a trie-encoded symbol, determine
 *  the node chains on this level which would contain that symbol or
 *  contain any trie variables, should either exist.
 */
#define Set_Matching_and_TrieVar_Chains(Symbol, MatchChain, VarChain) \
  if (IsNonNULL(pCurrentBTN) && IsHashHeader(pCurrentBTN)) { \
    BTNptr *buckets;  \
    BTHTptr pBTHT;  \
    pBTHT = (BTHTptr)pCurrentBTN; \
    buckets = BTHT_BucketArray(pBTHT);  \
    MatchChain = buckets[TrieHash(Symbol, BTHT_GetHashSeed(pBTHT))];  \
    VarChain = buckets[TRIEVAR_BUCKET]; \
  } \
  else  /* simple chain of nodes */ \
    VarChain = MatchChain = pCurrentBTN
    
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/*
 *  As soon as it is known that no variant set of terms exists in the   
 *  trie, that component of the current state of the search which is
 *  useful for later insertion of the given set of terms is saved in
 *  the global VariantContinuation structure.
 */
#define SetNoVariant(LastNodeMatched) \
  if(variant_path == YES) { \
    if(!save_variant_continuation(CTXTc LastNodeMatched)) \
      SubsumptiveTrieLookupError("Memory exhausted.");  \
    variant_path = NO;  \
  }
    
/* 
 *  Exact Matches for Non-Variable Subterms
 *  ---------------------------------------
 *  Exact matches for non-variable subterms of the call are always looked
 *  for first.  Once an exact match has been found, there is no need to
 *  search further, since there is at most one occurrence of a symbol at
 *  a level below a particular node.  Further exploration will
 *  concentrate on pairing the subterm with trie variables.
 *
 *  After a successful match, a choice point frame is laid, with
 *  VariableChain providing the trie-path continuation.  We step down onto
 *  the next level of the trie, below the matched node, and then branch
 *  back to the major loop of the algorithm.  If no match is found, then
 *  execution exits this block with a NULL MatchChain.  MatchChain may be
 *  NULL at block entry.
 *
 *  TermStack_PushOp is provided so that this macro can be used for
 *  constants, functor symbols, and lists.  Constants pass in a
 *  TermStack_NOOP, while functors use TermStack_PushFunctorArgs(), and
 *  lists use TermStack_PushListArgs().
 */
#define NonVarSearchChain_ExactMatch(Symbol, MatchChain, VariableChain, \
          TermStack_PushOp) \
    while(IsNonNULL(MatchChain)) {  \
      if(Symbol == BTN_Symbol(MatchChain)) { \
        Conditionally_Create_ChoicePoint(VariableChain) \
        TermStack_PushOp  \
        Descend_In_Trie_and_Continue(MatchChain); \
      } \
      MatchChain = BTN_Sibling(MatchChain); \
    }
    
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* 
 *  Matching Non-Variable Subterms with Bound Trievars
 *  --------------------------------------------------
 *  After having either (1) failed to find an exact match for the
 *  call's non-variable subterm, or (2) backtracked to explore other
 *  unifications, we proceed to check for equality of the subterm and
 *  the bindings made to previously seen trievars.  (A trie variable
 *  whose first occurrence (along the path from the root to here) is
 *  at this level would, of course, not have a binding.)  There may be
 *  several trievars at a level with such a binding.
 *
 *  We check for true equality, meaning that variables appearing in both
 *  must be identical.  (This is because no part of the call may become
 *  futher instantiated as a result of the check/insert operation.)  If
 *  they are the same, we succeed, else we continue to look in the chain
 *  for a suitable trievar.  Processing a success entails (1) laying a
 *  choice point since there may be other pairings in this node chain, (2)
 *  moving to the child of the matching node, (3) resetting our search mode
 *  to exact matches, and (4) branching back to the major loop of the
 *  algorithm.
 *
 *  If no match is found, then execution exits this block with a NULL
 *  'CurNODE'.  'CurNODE' may be NULL at block entry.
 */
#define NonVarSearchChain_BoundTrievar(Subterm, CurNODE, VarChain)  { \
    int trievar_index; \
      \
    while(IsNonNULL(CurNODE)) { \
      if(IsTrieVar(BTN_Symbol(CurNODE)) &&  \
        !IsNewTrieVar(BTN_Symbol(CurNODE))) { \
          trievar_index = DecodeTrieVar(BTN_Symbol(CurNODE)); \
          if(are_identical_terms(TrieVarBindings[trievar_index],  \
            Subterm)) { \
              Create_ChoicePoint(BTN_Sibling(CurNODE), VarChain)  \
              Descend_In_Trie_and_Continue(CurNODE);  \
          } \
      } \
      CurNODE = BTN_Sibling(CurNODE); \
    } \
}

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* 
 *  Matching Non-Variable Subterms with Unbound Trievars
 *  ----------------------------------------------------
 *  Unifying a call's non-variable subterm with an unbound trievar is
 *  the last pairing operation explored.  Only one unbound trievar may
 *  occur below a particular node; it is a new (first occurrence of this
 *  particular) variable lying along the subpath from the root to the
 *  current position in the trie.  Such trievars are tagged with a
 *  "first occurrence" marker.  If we don't find one, we exit with a
 *  NULL 'VarChain'.  (Node: Because of the hashing scheme, VarChain may
 *  contain symbols other than variables; furthermore, it may be empty
 *  altogether.)
 */

#define NonVarSearchChain_UnboundTrieVar(Subterm, VarChain) { \
    int trievar_index; \
      \
    while(IsNonNULL(VarChain)) {  \
      if(IsTrieVar(BTN_Symbol(VarChain)) && \
        IsNewTrieVar(BTN_Symbol(VarChain))) { \
        trievar_index = DecodeTrieVar(BTN_Symbol(VarChain));  \
        TrieVar_BindToSubterm(trievar_index, Subterm);  \
        Descend_In_Trie_and_Continue(VarChain); \
      } \
      VarChain = BTN_Sibling(VarChain); \
    } \
}
    
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

/* On a Successful Unification
 * ---------------------------
 * For continuing with forward execution.  When we find a successful
 * pairing, we continue the search with the next subterm on the
 * tstTermStack and the children of the trie node that was paired.
 */

#define Descend_In_Trie_and_Continue(PairedBTN) \
    pParentBTN = PairedBTN;                     \
    pCurrentBTN = BTN_Child(PairedBTN);         \
    search_mode = MATCH_SYMBOL_EXACTLY;         \
    goto While_TermStack_NotEmpty

static int AnsVarCtr;
    
static
void *iter_sub_trie_lookup(CTXTdeclc void *trieNode, TriePathType *pathType)
{
  BTNptr pParentBTN;
  BTNptr pCurrentBTN;
  BTNptr variableChain;
  Cell subterm;
  Cell symbol;
  int trievar_index;
  xsbBool variant_path = YES;
  SearchMode search_mode;
  
  pParentBTN = trieNode;
  CPStack_ResetTOS;
  pCurrentBTN = BTN_Child(pParentBTN);
  variableChain = NULL;
  search_mode = MATCH_SYMBOL_EXACTLY;
  
While_TermStack_NotEmpty:
  
  while(!TermStack_IsEmpty) {
    TermStack_Pop(subterm);
    TermStackLog_PushFrame;
    XSB_Deref(subterm);
    
    switch(cell_tag(subterm)) {
      case XSB_STRING:
      case XSB_INT:
      case XSB_FLOAT:
        /*
         *  NOTE:  A Trie constant looks like a Heap constant.
         */
        if(search_mode == MATCH_SYMBOL_EXACTLY) {
          symbol = EncodeTrieConstant(subterm);
          Set_Matching_and_TrieVar_Chains(symbol, pCurrentBTN, variableChain);
          NonVarSearchChain_ExactMatch(symbol, pCurrentBTN, variableChain,
              TermStack_NOOP)
          /*
        	 *  We've failed to find an exact match of the constant in a node
        	 *  of the trie, so now we consider bound trievars whose bindings
        	 *  exactly match the constant.
        	 */
          pCurrentBTN = variableChain;
          SetNoVariant(pParentBTN);
        }
        NonVarSearchChain_BoundTrievar(subterm, pCurrentBTN, variableChain);
        /*
      	 *  We've failed to find an exact match of the constant with a
      	 *  binding of a trievar.  Our last alternative is to bind an
      	 *  unbound trievar to this constant.
      	 */
        NonVarSearchChain_UnboundTrieVar(subterm, variableChain);
        break;
      case XSB_STRUCT:
        if(search_mode == MATCH_SYMBOL_EXACTLY) {
          symbol = EncodeTrieFunctor(subterm);
          Set_Matching_and_TrieVar_Chains(symbol, pCurrentBTN, variableChain);
          NonVarSearchChain_ExactMatch(symbol, pCurrentBTN, variableChain,
            TermStack_PushFunctorArgs(subterm))
          /*
  	       *  We've failed to find an exact match of the functor's name in
  	       *  a node of the trie, so now we consider bound trievars whose
  	       *  bindings exactly match the subterm.
  	       */
          pCurrentBTN = variableChain;
          SetNoVariant(pParentBTN); 
        }
        NonVarSearchChain_BoundTrievar(subterm, pCurrentBTN, variableChain);
        /*
  	     *  We've failed to find an exact match of the function expression
  	     *  with a binding of a trievar.  Our last alternative is to bind
  	     *  an unbound trievar to this subterm.
  	     */
        NonVarSearchChain_UnboundTrieVar(subterm, variableChain);
        break;
      case XSB_REF:
      //case XSB_REF1:
        /*
         *  A never-before-seen variable in the call must always match a
         *  free variable in the trie.  We can determine this by checking
         *  for a "first occurrence" tag in the trievar encoding.  Let Num
         *  be the index of this trievar variable.  Then we bind
         *  TrieVarBindings[Num] to 'subterm', the address of the deref'ed
         *  unbound call variable.  We also bind the call variable to
         *  VarEnumerator[Num] so that we can recognize that the call
         *  variable has already been seen.
         *
         *  When such a call variable is re-encountered, we know which
         *  trievar was the first to bind itself to this call variable: we
         *  used its index in marking the call variable when we bound it
         *  to VarEnumerator[Num].  This tagging scheme allows us to match
         *  additional unbound trie variables to it.  Recall that the
         *  TrieVarBindings array should contain *real* subterms, and not
         *  the callvar tags that we've constructed (the pointers into the
         *  VarEnumerator array).  So we must reconstruct a
         *  previously-seen variable's *real* address in order to bind a
         *  new trievar to it.  We can do this by computing the index of
         *  the trievar that first bound itself to it, and look in that
         *  cell of the TrieVarBindings array to get the call variable's
         *  *real* address.
         *
         *  Notice that this allows us to match variants.  For if we have
         *  a variant up to the point where we encounter a marked callvar,
         *  there can be at most one trievar which exactly matches it.  An
         *  unbound callvar, then, matches exactly only with an unbound
         *  trievar.  Therefore, only when a previously seen callvar must
         *  be paired with an unbound trievar to continue the search
         *  operation do we say that no variant exists.  (Just as is the
         *  case for other call subterm types, the lack of an exact match
         *  and its subsequent pairing with an unbound trievar destroys
         *  the possibility of a variant.)
         */
        if(search_mode == MATCH_SYMBOL_EXACTLY) {
          if(IsNonNULL(pCurrentBTN) && IsHashHeader(pCurrentBTN))
            pCurrentBTN = variableChain =
              BTHT_BucketArray((BTHTptr)pCurrentBTN)[TRIEVAR_BUCKET];
          else
            variableChain = pCurrentBTN;
          
          if(!PrologVar_IsMarked(subterm)) {
            AnsVarCtr++;
            /*
      	     *  The subterm is a call variable that has not yet been seen
      	     *  (and hence is not tagged).  Therefore, it can only be paired
      	     *  with an unbound trievar, and there can only be one of these
      	     *  in a chain.  If we find it, apply the unification, mark the
      	     *  callvar, trail them both, and continue.  Otherwise, fail.
      	     *  Note we don't need to lay a CPF since this is the only
      	     *  possible pairing that could result.
      	     */
      	  
      	   while(IsNonNULL(pCurrentBTN)) {
      	     if(IsTrieVar(BTN_Symbol(pCurrentBTN)) &&
      	        IsNewTrieVar(BTN_Symbol(pCurrentBTN))) {
                trievar_index = DecodeTrieVar(BTN_Symbol(pCurrentBTN));
                TrieVar_BindToSubterm(trievar_index, subterm);
                PrologVar_MarkIt(subterm, trievar_index);
                Descend_In_Trie_and_Continue(pCurrentBTN);  
      	      }
              pCurrentBTN = BTN_Sibling(pCurrentBTN);
      	  }
          SetNoVariant(pParentBTN);
          break; /* no pairing, so backtrack */
        }
      }
      /*
       *  We could be in a forward or backward execution mode.  In either
       *  case, the call variable has been seen before, and we first look
       *  to pair this occurrence of the callvar with a trievar that was
       *  previously bound to this particular callvar.  Note that there
       *  could be several such trievars.  Once we have exhausted this
       *  possibility, either immediately or through backtracking, we then
       *  allow the binding of an unbound trievar to this callvar.
       */
       while(IsNonNULL(pCurrentBTN)) {
         if(IsTrieVar(BTN_Symbol(pCurrentBTN)) &&
           !IsNewTrieVar(BTN_Symbol(pCurrentBTN))) {
            trievar_index = DecodeTrieVar(BTN_Symbol(pCurrentBTN));
            if(are_identical_terms(TrieVarBindings[trievar_index],
                subterm)) {
              Create_ChoicePoint(BTN_Sibling(pCurrentBTN), variableChain);
              Descend_In_Trie_and_Continue(pCurrentBTN);    
            }
         }
         pCurrentBTN = BTN_Sibling(pCurrentBTN);
       }
       /*
        *  We may have arrived here under several circumstances, but notice
        *  that the path we are on cannot be a variant one.  In case the
        *  possibility of a variant entry being present was still viable up
        *  to now, we save state info in case we need to create a variant
        *  entry later.  We now go to our last alternative, that of
        *  checking for an unbound trievar to pair with the marked callvar.
        *  If one is found, we trail the trievar, create the binding, and
        *  continue.  No CPF need be created since there can be at most one
        *  new trievar below any given node.
        */
       SetNoVariant(pParentBTN);
       
       while(IsNonNULL(variableChain)) {
         if(IsTrieVar(BTN_Symbol(variableChain)) &&
           IsNewTrieVar(BTN_Symbol(variableChain))) {
           trievar_index = DecodeTrieVar(BTN_Symbol(variableChain));
           TrieVar_BindToMarkedPrologVar(trievar_index, subterm);
           Descend_In_Trie_and_Continue(variableChain);  
         }
         variableChain = BTN_Sibling(variableChain);
       }
       break;
    /* lists and others XXX */
    default:
      TrieError_UnknownSubtermTag(subterm);
      break;
    } /* END switch(cell_tag(subterm)) */
    
    /*
     *  We've reached a dead-end since we were unable to match the
     *  current subterm to a trie node.  Therefore, we backtrack to
     *  continue the search, or, if there are no more choice point
     *  frames -- in which case the trie has been completely searched --
     *  we return and indicate that no subsuming path was found.
     */
    if(!CPStack_IsEmpty)
      BacktrackSearch
    else {
      *pathType = NO_PATH;
      return NULL;
    }
  } /* END while(!TermStack_IsEmpty) */
  
  /*
   *  The TermStack is empty, so we've reached a leaf node representing
   *  term(s) which subsumes the given term(s).  Return this leaf and an
   *  indication as to whether this path is a variant of or properly
   *  subsumes the given term(s).
   */
  if(variant_path)
    *pathType = VARIANT_PATH;
  else
    *pathType = SUBSUMPTIVE_PATH;
  
  return pParentBTN;
}

void subsumptive_search(yamop *preg, CELL **Yaddr)
{
  int arity;
  tab_ent_ptr tab_ent;
  BTNptr btRoot;
  BTNptr btn;
  TriePathType path_type;
  
  printf("subsumptive_search(preg, Yaddr)\n");
  
  AnsVarCtr = 0; /// XXX
  arity = preg->u.Otapl.s;
  tab_ent = preg->u.Otapl.te;
  btRoot = TabEnt_subgoal_trie(tab_ent);
  
  /* emu/sub_tables_xsb_i.h */
  TermStack_ResetTOS;
  TermStackLog_ResetTOS;
  Trail_ResetTOS;
  TermStack_PushLowToHighVector(XREGS + 1, arity);
  
  btn = iter_sub_trie_lookup(CTXTc btRoot, &path_type);
}

#endif /* TABLING && TABLING_CALL_SUBSUMPTION */