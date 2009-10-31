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
* File:		load_coff.c						 *
* comments:	coff based dynamic loader of external routines		 *
*									 *
*************************************************************************/

#include "Yap.h"
#include "Yatom.h"
#include "YapHeap.h"
#include "Foreign.h"

#ifdef COFF

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <a.out.h>

#define oktox(n) \
	(0==stat(n,&stbuf)&&(stbuf.st_mode&S_IFMT)==S_IFREG&&0==access(n,X_OK))
#define oktow(n) \
	(0==stat(n,&stbuf)&&(stbuf.st_mode&S_IFMT)==S_IFDIR&&0==access(n,W_OK))

#ifdef mips
#define MAXSECTIONS 100
#else
#define MAXSECTIONS 20
#endif				/* mips */

#ifdef sgi
#include <symbol.h>
#endif				/* sgi */

#define N_TXTOFF(x) (sizeof(struct filehdr)+(x).f_opthdr+(x).f_nscns*sizeof(struct scnhdr))

static char YapExecutable[YAP_FILE_MAX];


/*
 *   YAP_FindExecutable(argv[0]) should be called on yap initialization to
 *   locate the executable of Yap
*/
void
Yap_FindExecutable(char *name)
{
  register char  *cp, *cp2;
  struct stat     stbuf;


  cp = (char *)getenv("PATH");
  if (cp == NULL)
    cp = ".:/usr/ucb:/bin:/usr/bin:/usr/local/bin";
  if (*Yap_argv[0] == '/') {
    if (oktox(Yap_argv[0])) {
      strcpy(Yap_FileNameBuf, Yap_argv[0]);
      Yap_TrueFileName(Yap_FileNameBuf, YapExecutable, TRUE);
      return;
    }
  }
  if (*cp == ':')
    cp++;
  for (; *cp;) {
    /*
     * copy over current directory and then append
     * argv[0] 
     */
      
    for (cp2 = Yap_FileNameBuf; (*cp) != 0 && (*cp) != ':';)
      *cp2++ = *cp++;
    *cp2++ = '/';
    strcpy(cp2, Yap_argv[0]);
    if (*cp)
      cp++;
    if (!oktox(Yap_FileNameBuf))
      continue;
    Yap_TrueFileName(Yap_FileNameBuf, YapExecutable, TRUE);
    return;
  }
  /* one last try for dual systems */
  strcpy(Yap_FileNameBuf, Yap_argv[0]);
  Yap_TrueFileName(Yap_FileNameBuf, YapExecutable, TRUE);
  if (oktox(YapExecutable))
    return;
  else
    Yap_Error(SYSTEM_ERROR,MkAtomTerm(Yap_LookupAtom(YapExecutable)),
	  "cannot find file being executed");
}


/*
 * LoadForeign(ofiles,libs,proc_name,init_proc) dynamically loads foreign
 * code files and libraries and locates an initialization routine
*/
static Int
LoadForeign(StringList ofiles,
	    StringList libs,
	    char *proc_name,
	    YapInitProc *init_proc)
{
  char		  command[2*MAXPATHLEN];
  char            o_files[1024];    /* list of objects we want to load
				       */
  char            l_files[1024];    /* list of libraries we want to
				       load */ 
  char            tmp_buff[32] = "/tmp/YAP_TMP_XXXXXX";    /* used for
							 mktemp */
  char           *tfile;	    /* name of temporary file */
  int             fildes;	    /* temp file descriptor */
  struct aouthdr  sysHeader;
  struct filehdr  fileHeader;
  struct scnhdr   sectionHeader[MAXSECTIONS];
  struct exec     header;	    /* header for loaded file */
  unsigned long   loadImageSize, firstloadImSz;  /* size of image we will load */
  char           *FCodeBase;  /* where we load foreign code */

  /*
   * put in a string the names of the files you want to load and of any
   * libraries you want to use 
   */
  /* files first */
  *o_files = '\0';
  {
    StringList tmp = ofiles;

    while(tmp != NULL) {
      strcat(o_files," ");
      strcat(o_files,tmp->s);
      tmp = tmp->next;
    }
  }
  /* same_trick for libraries */
  *l_files = '\0';
  {
    StringList tmp = libs;

    while(tmp != NULL) {
      strcat(l_files," ");
      strcat(l_files,tmp->s);
      tmp = tmp->next;
    }
  }
  /* next, create a temp file to serve as loader output */
  tfile = mktemp(tmp_buff);

  /* prepare the magic */
  if (strlen(o_files) + strlen(l_files) + strlen(proc_name) +
	    strlen(YapExecutable) > 2*MAXPATHLEN) {
    strcpy(Yap_ErrorSay, " too many parameters in load_foreign/3 ");
    return LOAD_FAILLED;
  }
  sprintf(command, "/usr/bin/ld -N -A %s -o %s %s %s -lc",
	  YapExecutable,
	  tfile, o_files, l_files);
  /* now, do the magic */
  if (system(command) != 0) {
    unlink(tfile);
    strcpy(Yap_ErrorSay," ld returned error status in load_foreign_files ");
    return LOAD_FAILLED;
  }
  /* now check the music has played */
  if ((fildes = open(tfile, O_RDONLY)) < 0) {
    strcpy(Yap_ErrorSay," unable to open temp file in load_foreign_files ");
    return LOAD_FAILLED;
  }
  /* it did, get the mice */
  /* first, get the header */
  read(fildes, (char *) &fileHeader, sizeof(fileHeader));
  read(fildes, (char *) &sysHeader, sizeof(sysHeader));
  { int i;
    for (i = 0; i < fileHeader.f_nscns; i++)
      read(fildes, (char *) &sectionHeader[i],
	   sizeof(*sectionHeader));
  }
  close(fildes);
  /* get the full size of what we need to load */
  loadImageSize = sysHeader.tsize + sysHeader.dsize + sysHeader.bsize;
#ifdef mips
  /* add an extra page in mips machines */
  loadImageSize += 4095 + 16;
#else
  /* add 16 just to play it safe */
  loadImageSize += 16;
#endif
  /* keep this copy */
  firstloadImSz = loadImageSize;
  /* now fetch the space we need */
  if (!(FCodeBase = Yap_AllocCodeSpace((int) loadImageSize))
#ifdef pyr
      || activate_code(ForeignCodeBase, u1)
#endif				/* pyr */
      ) {
    strcpy(Yap_ErrorSay," unable to allocate space for external code ");
    return LOAD_FAILLED;
  }
#ifdef mips
  FCodeBase = (char *) (Unsigned(FCodeBase + PAGESIZE - 1) & ~(PAGESIZE - 1));
#endif

  /* now, a new incantation to load the new foreign code */
#ifdef convex
  /* No -N flag in the Convex loader */
  /* -T option does not want MallocBase bit set */
  sprintf(command, "ld -x -A %s -T %lx -o %s -u %s %s %s -lc",
	  ostabf,
	  ((unsigned long) (((unsigned long) (ForeignCodeBase)) &
			    ((unsigned long) (~Yap_HeapBase))
			    )
	   ), tfile, entry_point, o_files, l_files);
#else
#ifdef mips
  sprintf(command, "ld -systype bsd43 -N -A %s -T %lx -o %s -u %s %s %s -lc",
	  ostabf,
	  (unsigned long) ForeignCodeBase,
	  tfile, entry_point, o_files, l_files);
#else
  sprintf(command, "ld -N -A %s -T %lx -o %s -e %s -u _%s %s -lc",
	  ostabf,
	  (unsigned long) ForeignCodeBase,
	  tfile, entry_point, o_files, l_files);
#endif				/* mips */
#endif				/* convex */
  /* and do it */ 
  if (system(command) != 0) {
    unlink(tfile);
    strcpy(Yap_ErrorSay," ld returned error status in load_foreign_files ");
    return LOAD_FAILLED;
  }
  if ((fildes = open(tfile, O_RDONLY)) < 0) {
    strcpy(Yap_ErrorSay," unable to open temp file in load_foreign_files ");
    return LOAD_FAILLED;
  }
  read(fildes, (char *) &fileHeader, sizeof(fileHeader));
  read(fildes, (char *) &sysHeader, sizeof(sysHeader));
  {
    int i;
    for (i = 0; i < fileHeader.f_nscns; i++)
      read(fildes, (char *) &sectionHeader[i], sizeof(*sectionHeader));
  }
  loadImageSize = sysHeader.tsize + sysHeader.dsize + sysHeader.bsize;
  if (firstloadImSz < loadImageSize) {
    strcpy(Yap_ErrorSay," miscalculation in load_foreign/3 ");
    return LOAD_FAILLED;
  }
  /* now search for our init function */
  {
    char entry_fun[256];
    struct nlist    func_info[2];
#if defined(mips) || defined(I386)
    char            NAME1[128], NAME2[128];
    func_info[0].n_name = NAME1;
    func_info[1].n_name = NAME2;
#endif				/* COFF */
    sprintf(entry_fun, "_%s", proc_name);
    func_info[0].n_name = entry_fun;
    func_info[1].n_name = NULL;
    if (nlist(tfile, func_info) == -1) {
      strcpy(Yap_ErrorSay," in nlist(3) ");
      return LOAD_FAILLED;
    }
    if (func_info[0].n_type == 0) {
      strcpy(Yap_ErrorSay," in nlist(3) ");
      return LOAD_FAILLED;
    }
    *init_proc = (YapInitProc)(func_info[0].n_value);
  }
  /* ok, we got our init point */
  /* now read our text */
  lseek(fildes, (long)(N_TXTOFF(header)), 0);
  {
    unsigned int u1 = header.a_text + header.a_data;
    read(fildes, (char *) FCodeBase, u1);
    /* zero the BSS segment */
    while (u1 < loadImageSize)
      FCodeBase[u1++] = 0;
  }
  close(fildes);
  unlink(tfile);
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




