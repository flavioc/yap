/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   R. Rocha and NCC - University of Porto, Portugal
  File:        tab.subsumption.h
                                                                     
**********************************************************************/

#ifndef TAB_SUBSUMPTION_H
#define TAB_SUBSUMPTION_H

#include "opt.config.h"
#include "tab.xsb.h"
#include "tab.utils.h"

void subsumptive_call_search(TabledCallInfo *call_info, CallLookupResults *results);

#endif