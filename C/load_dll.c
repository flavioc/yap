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
* File:		load_dl.c						 *
* comments:	dl based dynamic loader  of external routines		 *
*               tested on i486-linuxelf					 *
*************************************************************************/

#include "Yap.h"
#include "Yatom.h"
#include "YapHeap.h"
#include "yapio.h"
#include "Foreign.h"

#if LOAD_DLL

#include <windows.h>

/*
 *   YAP_FindExecutable(argv[0]) should be called on yap initialization to
 *   locate the executable of Yap
*/
void
Yap_FindExecutable(char *name)
{
}


/*
 * LoadForeign(ofiles,libs,proc_name,init_proc) dynamically loads foreign
 * code files and libraries and locates an initialization routine
*/
static Int
LoadForeign(StringList ofiles, StringList libs,
	       char *proc_name,	YapInitProc *init_proc)
{

  while (ofiles) {
    HINSTANCE handle;

    if (Yap_TrueFileName(ofiles->s, Yap_FileNameBuf, TRUE) &&
	(handle=LoadLibrary(Yap_FileNameBuf)) != 0)
      {
	Yap_ErrorSay[0]=~'\0';
	if (*init_proc == NULL)
	  *init_proc = (YapInitProc)GetProcAddress((HMODULE)handle, proc_name);
      } else {
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		      NULL, GetLastError(), 
		      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), Yap_ErrorSay, 256,
		      NULL);
      }
    ofiles = ofiles->next;
  }
  /* load libraries first so that their symbols are available to
     other routines */
  while (libs) {
    HINSTANCE handle;

    if (libs->s[0] == '-') {
      strcat(Yap_FileNameBuf,libs->s+2);
      strcat(Yap_FileNameBuf,".dll");
    } else {
      strcpy(Yap_FileNameBuf,libs->s);
    }

    if((handle=LoadLibrary(Yap_FileNameBuf)) == 0)
    {
/*      strcpy(Yap_ErrorSay,dlerror());*/
      return LOAD_FAILLED;
    }

    if (*init_proc == NULL)
      *init_proc = (YapInitProc)GetProcAddress((HMODULE)handle, proc_name);

    libs = libs->next;
  }

  if(*init_proc == NULL) {
    strcpy(Yap_ErrorSay,"Could not locate initialization routine");
    return LOAD_FAILLED;
  }

  return LOAD_SUCCEEDED;
}

Int
Yap_LoadForeign(StringList ofiles, StringList libs,
	       char *proc_name,	YapInitProc *init_proc)
{
  return LoadForeign(ofiles, libs, proc_name, init_proc);
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

