/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   Flávio Cruz
  File:        tab.specific.h
  version:       
                                                                     
**********************************************************************/

#ifndef TAB_SPECIFIC_H
#define TAB_SPECIFIC_H

/* ------------------ **
**      Includes      **
** ------------------ */
#include "opt.config.h"

#ifdef TABLING_CALL_SUBSUMPTION

#include "tab.xsb.h"
#include "tab.utils.h"
#include "tab.structs.h"

ALNptr collect_specific_generator_goals(yamop *code);

#endif /* TABLING_CALL_SUBSUMPTION */
#endif
