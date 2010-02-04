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

Cell* construct_variant_answer_template(Cell *var_vector);
sg_node_ptr variant_call_cont_insert(tab_ent_ptr tab_ent, sg_node_ptr current_node, int count_vars);
void variant_call_search(TabledCallInfo *call_info, CallLookupResults *results);

#endif