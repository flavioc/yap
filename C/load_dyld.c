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
* File:		load_dyld.c						 *
* comments:	dyld based dynamic loaderr of external routines		 *
*               tested on MacOS						 *
*************************************************************************/

#include "Yap.h"
#include "Yatom.h"
#include "YapHeap.h"
#include "yapio.h"
#include "Foreign.h"

#if LOAD_DYLD

#include <string.h>

/* Code originally  from Rex A. Dieter's posting in comp.sys.next.programmer
   and from dynload_next.c in the Python sources 
*/
#import <mach-o/dyld.h>

static int dl_errno;

static char *
mydlerror(void)
{
  char *errString;
  switch(dl_errno) {
  default:
  case NSObjectFileImageFailure:
  case NSObjectFileImageFormat:
    /* for these a message is printed on stderr by dyld */
    errString = "Can't create object file image";
    break;
  case NSObjectFileImageSuccess:
    errString = NULL;
    break;
  case NSObjectFileImageInappropriateFile:
    errString = "Inappropriate file type for dynamic loading";
    break;
  case NSObjectFileImageArch:
    errString = "Wrong CPU type in object file";
    break;
  case NSObjectFileImageAccess:
    errString = "Can't read object file (no access)";
    break;
  }
  return(errString);
}


/*
 *   YAP_FindExecutable(argv[0]) should be called on yap initialization to
 *   locate the executable of Yap
*/
void
Yap_FindExecutable(char *name)
{
}


static void *
mydlopen(char *path)
{
    int dyld_result;
    NSObjectFileImage ofile;
    NSModule handle = NULL;
    dyld_result = NSCreateObjectFileImageFromFile(path, &ofile);
    if (dyld_result != NSObjectFileImageSuccess) {
      dl_errno = dyld_result;
    } else {
      /* NSLinkModule will cause the run to abort on any link error's */
      /* not very friendly but the error recovery functionality is limited */
        handle = NSLinkModule(ofile, path, TRUE);
    } return handle;
}

static void *
mydlsym(char *symbol)
{
    void *addr;
    char funcname[256];

#if HAVE_SNPRINTF
    snprintf(funcname, sizeof(funcname), "_%.200s", symbol);
#else
    sprintf(funcname, "_%.200s", symbol);
#endif
    if (NSIsSymbolNameDefined(funcname))
        addr = NSAddressOfSymbol(NSLookupAndBindSymbol(funcname));
    else
        addr = NULL;
    return addr;
} 

static int
mydlclose(void *handle)
{
  NSUnLinkModule(handle, NSUNLINKMODULE_OPTION_NONE);
  return TRUE;
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
    void *handle;

    /* mydlopen wants to follow the LD_CONFIG_PATH */
    if (!Yap_TrueFileName(ofiles->s, Yap_FileNameBuf, TRUE)) {
      strcpy(Yap_ErrorSay, "%% Trying to open unexisting file in LoadForeign");
      return LOAD_FAILLED;
    }
    if((handle=mydlopen(Yap_FileNameBuf)) == 0)
    {
      fprintf(stderr,"calling dlopen with error %s\n", mydlerror());
/*      strcpy(Yap_ErrorSay,dlerror());*/
      return LOAD_FAILLED;
    }

    ofiles->handle = handle;

    ofiles = ofiles->next;
  }
  /* load libraries first so that their symbols are available to
     other routines */
  while (libs) {

    if (libs->s[0] == '-') {
      strcpy(Yap_FileNameBuf,"lib");
      strcat(Yap_FileNameBuf,libs->s+2);
      strcat(Yap_FileNameBuf,".so");
    } else {
      strcpy(Yap_FileNameBuf,libs->s);
    }

    if((libs->handle=mydlopen(Yap_FileNameBuf)) == NULL)
    {
      strcpy(Yap_ErrorSay,mydlerror());
      return LOAD_FAILLED;
    }
    libs = libs->next;
  }

  *init_proc = (YapInitProc) mydlsym(proc_name);

  if(! *init_proc) {
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
  ForeignObj *f_code;

  f_code = ForeignCodeLoaded;
  while (f_code != NULL) {
    StringList objs, libs;

    objs = f_code->objs;
    while (objs != NULL) {
      if (mydlclose(objs->handle) != 0)
	return; /* ERROR */
      objs = objs->next;
    }
    libs = f_code->libs;
    while (libs != NULL) {
      if (mydlclose(libs->handle) != 0)
	return; /* ERROR */
      objs = libs->next;
    }
    f_code = f_code->next;
  }
}

Int
Yap_ReLoadForeign(StringList ofiles, StringList libs,
	       char *proc_name,	YapInitProc *init_proc)
{
  return(LoadForeign(ofiles,libs, proc_name, init_proc));
}

#endif
