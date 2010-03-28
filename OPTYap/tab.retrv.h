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

#ifdef TABLING_CALL_SUBSUMPTION

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
#define SIZEOF_CHOICE_POINT_FRAME sizeof(tstChoicePointFrame)

struct tstCPStack_t {
	int *top; /* next available location to place an entry */
	int *ceiling; /* overflow pointer: points beyond array end */
	int base[TST_CPSTACK_SIZE * SIZEOF_CHOICE_POINT_FRAME];
};

void initCollectRelevantAnswers(CTXTdecl);

xsbBool tst_collect_relevant_answers(CTXTdeclc TSTNptr tstRoot, TimeStamp ts,
  int numTerms, CPtr termsRev, subcons_fr_ptr sg_fr);

#endif /* TABLING_CALL_SUBSUMPTION */
#endif
