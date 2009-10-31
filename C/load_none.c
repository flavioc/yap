/*************************************************************************
*									 *
*	 YAP Prolog 							 *
*									 *
*	Yap Prolog was developed at NCCUP - Universidade do Porto	 *
*									 *
* Copyright L.Damas, V.S.Costa and Universidade do Porto 1985-1997	 *
*									 *
**************************************************************************
*									 *
* File:		load_none.c						 *
* comments:	dummy dynamic loaderr of external routines		 *
*************************************************************************/

#include "Yap.h"
#include "Foreign.h"
#include "Yatom.h"
#include "YapHeap.h"
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef NO_DYN

/*
 *   YAP_FindExecutable(argv[0]) should be called on yap initialization to
 *   locate the executable of Yap
*/
void
Yap_FindExecutable(char *name)
{
}


/*
 * LoadForeign(file_name,proc_name,init_proc) dynamically loads a foreign
 * code file and locates an initialization routine
*/
static Int
LoadForeign(StringList ofiles, StringList libs,
	    char *proc_name, YapInitProc *init_proc)
{
  strcpy(Yap_ErrorSay,"load_foreign not supported in this version of Yap");
  return LOAD_FAILLED;
}

Int
Yap_LoadForeign(StringList ofiles, StringList libs,
	    char *proc_name, YapInitProc *init_proc)
{
  return(LoadForeign(ofiles,libs, proc_name, init_proc));
}

void 
Yap_ShutdownLoadForeign(void)
{
}

Int
Yap_ReLoadForeign(StringList ofiles, StringList libs,
	       char *proc_name,	YapInitProc *init_proc)
{
  return(LoadForeign(ofiles,libs, proc_name, init_proc));
}

#endif

