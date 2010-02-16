/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   Flávio Cruz
  File:        tab.xsb_common.h
  version:        
  comment:                                                
**********************************************************************/

#ifdef SUBSUMPTION_XSB
#define CreateHeapFunctor(Symbol) { \
  CPtr heap_var_ptr;  \
  int arity, i; \
  Psc symbolPsc;  \
  symbolPsc = (Psc)cs_val(Symbol);  \
  arity = get_arity(symbolPsc); \
  bld_cs(hreg, hreg + 1); \
  bld_functor(++hreg, symbolPsc); \
  for(heap_var_ptr = hreg + arity, i = 0; \
      i < arity;  \
      heap_var_ptr--, i++) {  \
    bld_free(heap_var_ptr); \
    TermStack_Push((Cell)heap_var_ptr); \
  } \
  hreg = hreg + arity + 1;  \
}
#define CreateHeapList() {  \
  bld_list(hreg, hreg + 1); \
  hreg = hreg + 3;  \
  bld_free(hreg - 1); \
  TermStack_Push((Cell)(hreg - 1)); \
  bld_free(hreg - 2); \
  TermStack_Push((Cell)(hreg - 2)); \
}
#else
#define CreateHeapFunctor(Symbol) { \
  Functor functor;													\
	int arity, i;													\
  CPtr old_h = H++;               \
																	\
	functor = (Functor)RepAppl(Symbol);	\
	arity = ArityOfFunctor(functor);  \
	Term tf = Yap_MkNewApplTerm(functor,arity);	\
  *old_h = tf;        \
	for (i = arity; i >= 1; i--)			{		\
    Cell c = *(RepAppl(tf) + i);  \
		TermStack_Push(c);	\
	} \
}
#define CreateHeapList() {  \
  CPtr old_h = H++;  \
  Term tl = Yap_MkNewPairTerm();	\
  *old_h = tl;  \
	TermStack_Push(*(RepPair(tl) + 1));	\
	TermStack_Push(*(RepPair(tl)));	\
}

/* define WAM registers */
#define trreg TR
#define hreg H
#define hbreg HB
#define ereg E
#define trfreg TR_FZ
#define cpreg CP
#define top_of_trail ((trreg > trfreg) ? trreg : trfreg)

#define unify(TERM1, TERM2) Yap_unify(TERM1, TERM2)

#endif