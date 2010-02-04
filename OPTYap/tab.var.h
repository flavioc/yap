/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   R. Rocha and NCC - University of Porto, Portugal
  File:        tab.var.h
  version:     $Id: tab.macros.h,v 1.22 2008-05-23 18:28:58 ricroc Exp $   
                                                                     
**********************************************************************/

#ifndef TAB_VAR_H
#define TAB_VAR_H

/* ------------------ **
**      Includes      **
** ------------------ */
#include "opt.config.h"
#include "tab.xsb.h"
#include "tab.utils.h"

inline CPtr extract_template_from_insertion(CTXTdeclc CPtr ans_tmplt);
sg_node_ptr variant_call_cont_insert(tab_ent_ptr tab_ent, sg_node_ptr current_node, int count_vars);
void variant_call_search(TabledCallInfo *call_info, CallLookupResults *results);
ans_node_ptr variant_answer_search(sg_fr_ptr sg_fr, CELL *subs_ptr);

#endif