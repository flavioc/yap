/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   Flávio Cruz
  File:        tab.retrv.h
  version:       
                                                                     
**********************************************************************/

#ifndef TAB_RETRV_H
#define TAB_RETRV_H

/* ------------------ **
**      Includes      **
** ------------------ */
#include "opt.config.h"

#ifdef TABLING

#include "tab.xsb.h"
#include "tab.utils.h"
#include "tab.tst.h"

typedef struct {
	TSTNptr alt_node; /* sibling of the TSTN whose child ptr we took */
	int ts_top_index; /* current top-of-tstTermStack at CP creation */
	int log_top_index; /* current top-of-tstTermStackLog at CP creation */
	tr_fr_ptr trail_top; /* current top-of-trail at CP creation */
	CPtr heap_bktrk; /* current hbreg at time of CP creation */
} tstChoicePointFrame;

#define TST_CPSTACK_SIZE 1024

struct tstCPStack_t {
	tstChoicePointFrame *top; /* next available location to place an entry */
	tstChoicePointFrame *ceiling; /* overflow pointer: points beyond array end */
	tstChoicePointFrame base[TST_CPSTACK_SIZE];
};

extern struct tstCPStack_t tstCPStack;

#define TrieSymbol_Deref(Symbol)	\
	if(IsTrieVar(Symbol)) {	\
		Symbol = TrieVarBindings[DecodeTrieVar(Symbol)];	\
		XSB_Deref(Symbol);	\
	}

ALNptr tst_collect_relevant_answers(CTXTdeclc TSTNptr tstRoot, TimeStamp ts,
				    int numTerms, CPtr termsRev);

#endif /* TABLING */
#endif
