/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   Flávio Cruz
  File:        tab.unify.h
  version:        
  comment:                                                
**********************************************************************/

#ifndef TAB_UNIFY_H
#define TAB_UNIFY_H

/* ------------------ **
**      Includes      **
** ------------------ */
#include "opt.config.h"

#ifdef TABLING

#include "tab.xsb.h"
#include "tab.utils.h"
#include "tab.tst.h"

void consume_subsumptive_answer(CTXTdeclc BTNptr pAnsLeaf, int sizeTmplt, CPtr pAnsTmplt);

#endif /* TABLING */

#endif