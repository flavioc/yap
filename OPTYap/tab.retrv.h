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

xsbBool tst_collect_relevant_answers(CTXTdeclc TSTNptr tstRoot, TimeStamp ts,
  int numTerms, CPtr termsRev, subcons_fr_ptr sg_fr);

#endif /* TABLING_CALL_SUBSUMPTION */
#endif
