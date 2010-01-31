/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   R. Rocha and NCC - University of Porto, Portugal
  File:        tab.subsumption.h
                                                                     
**********************************************************************/

#ifndef TAB_SUBSUMPTION_H
#define TAB_SUBSUMPTION_H

#include "opt.config.h"

#ifdef TABLING_CALL_SUBSUMPTION
#include "tab.xsb.h"
#include "tab.utils.h"

sg_node_ptr subsumptive_search(yamop *preg, CELL **Yaddr, TriePathType* path_type);

#endif /* TABLING_CALL_SUBSUMPTION */

#endif