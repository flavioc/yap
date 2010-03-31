/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   Flávio Cruz
  File:        tab.collect.h
  version:       
                                                                     
**********************************************************************/

#ifndef TAB_COLLECT_H
#define TAB_COLLECT_H

/* ------------------ **
**      Includes      **
** ------------------ */
#include "opt.config.h"

#ifdef TABLING_CALL_SUBSUMPTION

#include "tab.xsb.h"
#include "tab.utils.h"

#include "xsb.collect.h"

#endif /* TABLING_CALL_SUBSUMPTION */
#endif /* !TAB_COLLECT_H */