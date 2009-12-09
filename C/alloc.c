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
* File:		alloc.c							 *
* Last rev:								 *
* mods:									 *
* comments:	allocating space					 *
* version:$Id: alloc.c,v 1.95 2008-05-10 23:24:11 vsc Exp $		 *
*************************************************************************/
#ifdef SCCS
static char SccsId[] = "%W% %G%";

#endif

#include "Yap.h"
#include "Yatom.h"
#include "YapHeap.h"
#include "alloc.h"
#include "yapio.h"
#if HAVE_STRING_H
#include <string.h>
#endif
#if HAVE_MALLOC_H
#include <malloc.h>
#endif
#if USE_DL_MALLOC
#include "dlmalloc.h"
#endif
#if HAVE_MEMORY_H
#include <memory.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_FCNTL_H
#include <fcntl.h>
#endif
#if HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#include <stdlib.h>
#include <stdio.h>

#if __simplescalar__
#ifdef USE_MMAP
#undef USE_MMAP
#endif
#ifdef USE_SBRK
#undef USE_SBRK
#endif
#endif


/************************************************************************/
/* Yap workspace management                                             */

#if USE_SYSTEM_MALLOC
#define my_malloc(sz) malloc(sz)
#define my_realloc(ptr, sz, osz, safe) realloc(ptr, sz)
#define my_realloc0(ptr, sz) realloc(ptr, sz)
#define my_free(ptr) free(ptr)
#else
#define my_malloc(sz) Yap_dlmalloc(sz)
#define my_realloc0(ptr, sz) Yap_dlrealloc(ptr, sz)
#define my_free(sz) Yap_dlfree(sz)

static char * my_realloc(char *ptr, UInt sz, UInt osz, int safe)
{
  char *nptr;

 restart:
  /* simple case */
  if (ptr < Yap_HeapBase || ptr > HeapTop) {
    /* we have enough room */
    nptr = Yap_dlmalloc(sz);
    if (nptr) {
      memmove(nptr, ptr, osz);
      free(ptr);
    }
  } else {
    nptr = Yap_dlrealloc(ptr, sz);
  }
  if (nptr) {
    return nptr;
  }
  /* we do not have enough room */
  if (safe) {
    if (Yap_growheap(FALSE, sz, NULL)) {
      /* now, we have room */
      goto restart;
    }
  }
  /* no room in Heap, gosh */
  if (ptr < Yap_HeapBase || ptr > HeapTop) {
    /* try expanding outside the heap */
    nptr = realloc(ptr, sz);
    if (nptr) {
      memmove(nptr, ptr, osz);
    }
  } else {
    /* try calling the outside world for help */
    nptr = malloc(sz);
    if (!nptr)
      return NULL;
    memmove(nptr, ptr, osz);
    Yap_dlfree(ptr);
  }
  /* did we suceed? at this point we could not care less */
  return nptr;
}
#endif

#if USE_SYSTEM_MALLOC||USE_DL_MALLOC

long long unsigned int mallocs, reallocs, frees;
long long unsigned int tmalloc;

#if INSTRUMENT_MALLOC
static void
minfo(char mtype)
{
  struct mallinfo minfo = mallinfo();

  fprintf(stderr,"%c %lld (%lld), %lld, %lld %d/%d/%d\n", mtype, mallocs, tmalloc, reallocs, frees,minfo.arena,minfo.ordblks,minfo.fordblks);
}
#endif

static inline char *
call_malloc(unsigned long int size)
{
  char *out;
#if INSTRUMENT_MALLOC
  if (mallocs % 1024*4 == 0) 
    minfo('A');
  mallocs++;
  tmalloc += size;
#endif
  Yap_PrologMode |= MallocMode;
  out = (char *) my_malloc(size);
  Yap_PrologMode &= ~MallocMode;
  return out;
}

char *
Yap_AllocCodeSpace(unsigned long int size)
{
  return  call_malloc(size);
}

static inline char *
call_realloc(char *p, unsigned long int size)
{
  char *out;
#if INSTRUMENT_MALLOC
  if (mallocs % 1024*4 == 0) 
    minfo('A');
  mallocs++;
  tmalloc += size;
#endif
  Yap_PrologMode |= MallocMode;
  out = (char *) my_realloc0(p, size);
  Yap_PrologMode &= ~MallocMode;
  return out;
}

char *
Yap_ReallocCodeSpace(char *p, unsigned long int size)
{
  return  call_realloc(p, size);
}

void
Yap_FreeCodeSpace(char *p)
{
  Yap_PrologMode |= MallocMode;

#if INSTRUMENT_MALLOC
  if (frees % 1024*4 == 0) 
    minfo('F');
  frees++;
#endif
  my_free (p);
  Yap_PrologMode &= ~MallocMode;
}

char *
Yap_AllocAtomSpace(unsigned long int size)
{
  return call_malloc(size);
}

void
Yap_FreeAtomSpace(char *p)
{
  Yap_PrologMode |= MallocMode;
#if INSTRUMENT_MALLOC
  if (frees % 1024*4 == 0) 
    minfo('F');
  frees++;
#endif
  my_free (p);
  Yap_PrologMode &= ~MallocMode;
}

#endif

/* If you need to dinamically allocate space from the heap, this is
 * the macro you should use */
ADDR
Yap_InitPreAllocCodeSpace(void)
{
  char *ptr;
  UInt sz = ScratchPad.msz;
  if (ScratchPad.ptr == NULL) {
    Yap_PrologMode |= MallocMode;
    while (!(ptr = my_malloc(sz))) {
      Yap_PrologMode &= ~MallocMode;
      if (!Yap_growheap(FALSE, Yap_Error_Size, NULL)) {
	Yap_Error(OUT_OF_HEAP_ERROR, TermNil, Yap_ErrorMessage);
	return(NULL);
      }
      Yap_PrologMode |= MallocMode;
    }
    Yap_PrologMode &= ~MallocMode;
    ScratchPad.ptr = ptr;
  } else {
    ptr = ScratchPad.ptr;
  }
  AuxBase = (ADDR)(ptr);
  AuxSp = (CELL *)(AuxTop = AuxBase+ScratchPad.sz);
  return ptr;
}

ADDR
Yap_ExpandPreAllocCodeSpace(UInt sz0, void *cip, int safe)
{
  char *ptr;
  UInt sz = ScratchPad.msz;
  if (sz0 < SCRATCH_INC_SIZE)
    sz0 = SCRATCH_INC_SIZE;
  if (sz0 < ScratchPad.sz)
    sz = ScratchPad.sz+sz0;
  else 
    sz = sz0;
  sz = AdjustLargePageSize(sz+sz/4);

#if INSTRUMENT_MALLOC
  if (reallocs % 1024*4 == 0) 
    minfo('R');
  reallocs++;
#endif
  Yap_PrologMode |= MallocMode;
  if (!(ptr = my_realloc(ScratchPad.ptr, sz, ScratchPad.sz, safe))) {
    Yap_PrologMode &= ~MallocMode;
    return NULL;
  }
  Yap_PrologMode &= ~MallocMode;
  ScratchPad.sz = ScratchPad.msz = sz;
  ScratchPad.ptr = ptr;
  AuxBase = ptr;
  AuxSp = (CELL *)(AuxTop = ptr+sz);
  return ptr;
}

#if USE_SYSTEM_MALLOC

struct various_codes *Yap_heap_regs;

static void
InitHeap(void)
{
  Yap_heap_regs = (struct various_codes *)calloc(1, sizeof(struct various_codes));
#if defined(YAPOR) || defined(TABLING)
  LOCAL = REMOTE; /* point to the first area */
#endif /* YAPOR || TABLING */
}

void
Yap_InitHeap(void *heap_addr)
{
  InitHeap();
  Yap_HoleSize = 0;
  HeapMax = 0;
}

static void
InitExStacks(int Trail, int Stack)
{
  UInt pm, sa;

  /* sanity checking for data areas */
  if (Trail < MinTrailSpace)
    Trail = MinTrailSpace;
  if (Stack < MinStackSpace)
    Stack = MinStackSpace;
  pm = (Trail + Stack)*K;	/* memory to be
				 * requested         */
  sa = Stack*K;			/* stack area size   */

#ifdef THREADS
  if (worker_id)
    Yap_GlobalBase = (ADDR)ThreadHandle[worker_id].stack_address;
#endif
  Yap_TrailTop = Yap_GlobalBase + pm;
  Yap_LocalBase = Yap_GlobalBase + sa;
  Yap_TrailBase = Yap_LocalBase + sizeof(CELL);

  ScratchPad.ptr = NULL;
  ScratchPad.sz = ScratchPad.msz = SCRATCH_START_SIZE;
 AuxSp = NULL;

#ifdef DEBUG
  if (Yap_output_msg) {
    UInt ta;

    fprintf(stderr, "HeapBase = %p  GlobalBase = %p\n  LocalBase = %p  TrailTop = %p\n",
	       Yap_HeapBase, Yap_GlobalBase, Yap_LocalBase, Yap_TrailTop);

    ta = Trail*K;			/* trail area size   */
    fprintf(stderr, "Heap+Aux: %lu\tLocal+Global: %lu\tTrail: %lu\n",
	       (long unsigned)(pm - sa - ta), (long unsigned)sa, (long unsigned)ta);
  }
#endif /* DEBUG */
}

void
Yap_InitExStacks(int Trail, int Stack)
{
  InitExStacks(Trail, Stack);
}

#if defined(THREADS)
void
Yap_KillStacks(int wid)
{
  ADDR gb = ThreadHandle[wid].stack_address;
  if (gb) {
    free(gb);
    ThreadHandle[wid].stack_address = NULL;
  }
}
#else
void
Yap_KillStacks(void)
{
  if (Yap_GlobalBase) {
    free(Yap_GlobalBase);
    Yap_GlobalBase = NULL;
  }
}
#endif

void
Yap_InitMemory(UInt Trail, UInt Heap, UInt Stack)
{
  InitHeap();
}

int
Yap_ExtendWorkSpace(Int s)
{
  void *basebp = (void *)Yap_GlobalBase, *nbp;
  UInt s0 = (char *)Yap_TrailTop-(char *)Yap_GlobalBase;
  nbp = realloc(basebp, s+s0);
  if (nbp == NULL) 
    return FALSE;
#if defined(THREADS)
  ThreadHandle[worker_id].stack_address = (char *)nbp;
#endif
  Yap_GlobalBase = (char *)nbp;
  return TRUE;
}

UInt
Yap_ExtendWorkSpaceThroughHole(UInt s)
{
  return -1;
}

void
Yap_AllocHole(UInt actual_request, UInt total_size)
{
}

#if HAVE_MALLINFO
UInt
Yap_givemallinfo(void)
{
  struct mallinfo mi = mallinfo();
  return mi.uordblks;
}
#endif


#else

#if HAVE_SNPRINTF
#define snprintf3(A,B,C)  snprintf(A,B,C)
#define snprintf4(A,B,C,D)  snprintf(A,B,C,D)
#define snprintf5(A,B,C,D,E)  snprintf(A,B,C,D,E)
#else
#define snprintf3(A,B,C)  sprintf(A,C)
#define snprintf4(A,B,C,D)  sprintf(A,C,D)
#define snprintf5(A,B,C,D,E)  sprintf(A,C,D,E)
#endif

#ifdef LIGHT
#include <stdlib.h>
#endif

#if !USE_DL_MALLOC

STATIC_PROTO(void FreeBlock, (BlockHeader *));
STATIC_PROTO(BlockHeader *GetBlock, (unsigned long int));
STATIC_PROTO(char *AllocHeap, (unsigned long int));
STATIC_PROTO(void RemoveFromFreeList, (BlockHeader *));
STATIC_PROTO(void AddToFreeList, (BlockHeader *));

#define MinHGap   256*K

static void
RemoveFromFreeList(BlockHeader *b)
{
  BlockHeader *p;

  p = b->b_next_size;
  LOCK(HeapUsedLock);
  HeapUsed += (b->b_size + 1) * sizeof(YAP_SEG_SIZE);
  UNLOCK(HeapUsedLock);

  if (p && b->b_size == p->b_size) {
    b = b->b_next;
    p->b_next = b;
    if (b)
      b->b_next_size = p;
  }
  else {
    BlockHeader **q = &FreeBlocks;

    while ((*q) != b)
      q = &((*q)->b_next_size);
    if (b->b_next) {
      p = b->b_next;
      *q = p;
      p->b_next_size = b->b_next_size;
    }
    else {
      *q = b->b_next_size;
    }
  }
}

static void
AddToFreeList(BlockHeader *b)
{
  BlockHeader **q, *p;
  YAP_SEG_SIZE *sp;

  /* insert on list of free blocks */
  q = &FreeBlocks;
  sp = &(b->b_size) + b->b_size;
  *sp = b->b_size;
  LOCK(HeapUsedLock);
  HeapUsed -= (b->b_size + 1) * sizeof(YAP_SEG_SIZE);
  UNLOCK(HeapUsedLock);

  while ((p = *q) && p->b_size < b->b_size)
    q = &p->b_next_size;
  if (p && p->b_size == b->b_size) {
    b->b_next = p;
    b->b_next_size = p->b_next_size;
    p->b_next_size = b;
  }
  else {
    b->b_next = NIL;
    b->b_next_size = p;
  }
  *q = b;
}

static void
FreeBlock(BlockHeader *b)
{
  BlockHeader *p;
  YAP_SEG_SIZE *sp;

  /*  {
    static long long int vsc_free_ops;
 
    vsc_free_ops++;
    BlockHeader *q = FreeBlocks;
    while (q) q = q->b_next_size;
    }*/

  /* sanity check */
  sp = &(b->b_size) + (b->b_size & ~InUseFlag);
  if (!(b->b_size & InUseFlag) || *sp != b->b_size) {
#if !SHORT_INTS
    fprintf(stderr, "%% YAP INTERNAL ERROR: sanity check failed in FreeBlock %p %x %x\n",
	       b, b->b_size, Unsigned(*sp));
#else
    fprintf(stderr, "%% YAP INTERNAL ERROR: sanity check failed in FreeBlock %p %lx %lx\n",
	       b, b->b_size, *sp);
#endif
    return;
  }
  b->b_size &= ~InUseFlag;
  LOCK(FreeBlocksLock);
  /* check if we can collapse with other blocsks */
  /* check previous */
  sp = &(b->b_size) - 1;
  if (!(*sp & InUseFlag)) {	/* previous block is free */
    p = (BlockHeader *) (sp - *sp);
    RemoveFromFreeList(p);
    p->b_size += b->b_size + 1;
    b = p;
  }
  /* check following */
  sp = &(b->b_size) + b->b_size + 1;
  if (!(*sp & InUseFlag)) {	/* following block is free */
    p = (BlockHeader *) sp;
    RemoveFromFreeList(p);
    b->b_size += p->b_size + 1;
  }
  LOCK(HeapTopLock);
  if (sp == (YAP_SEG_SIZE *)HeapTop) {
    LOCK(HeapUsedLock);
    HeapUsed -= (b->b_size + 1) * sizeof(YAP_SEG_SIZE);
    UNLOCK(HeapUsedLock);
    HeapTop = (ADDR)b;
    *((YAP_SEG_SIZE *) HeapTop) = InUseFlag;
  } else {
  /* insert on list of free blocks */
    AddToFreeList(b);
  }
  UNLOCK(HeapTopLock);
  UNLOCK(FreeBlocksLock);
}

static BlockHeader *
GetBlock(unsigned long int n)
{				/* get free block with size at least n */
  register BlockHeader **p, *b, *r;

  if (FreeBlocks == NIL)
    return (NIL);
  /* check for bugs */
  p = &FreeBlocks;
  /* end check for bugs */
  p = &FreeBlocks;
  while (((b = *p) != NIL) && b->b_size < n)
    p = &b->b_next_size;
  if (b == NIL || b->b_size < n)
    return (NIL);
  if ((r = b->b_next) == NIL)
    *p = b->b_next_size;
  else {
    r->b_next_size = b->b_next_size;
    *p = r;
  }
  LOCK(HeapUsedLock);
  HeapUsed += (b->b_size + 1) * sizeof(YAP_SEG_SIZE);
  if (HeapUsed > HeapMax)
    HeapMax = HeapUsed;
  UNLOCK(HeapUsedLock);
  return (b);
}

static char *
AllocHeap(unsigned long int size)
{
  BlockHeader *b, *n;
  YAP_SEG_SIZE *sp;
  UInt align, extra;

  /*  {
    static long long int vsc_alloc_ops;
    vsc_alloc_ops++;
    BlockHeader *q = FreeBlocks;
    while (q) q = q->b_next_size;
    }*/

  extra = size/16;
#if SIZEOF_INT_P==4
  align = 2*sizeof(CELL);	/* size in dwords + 2 */
#endif
#if SIZEOF_INT_P==8
  align = sizeof(CELL);
#endif
  while (align < extra) align *= 2;
  size = ALIGN_SIZE(size,align);
  if (size < sizeof(BlockHeader))
    size = sizeof(BlockHeader);
  size += sizeof(YAP_SEG_SIZE);
  /* change units to cells */
  size = size/sizeof(CELL);
  LOCK(FreeBlocksLock);
  if ((b = GetBlock(size))) {
    if (b->b_size >= size+24+1) {
      n = (BlockHeader *) (((YAP_SEG_SIZE *) b) + size + 1);
      n->b_size = b->b_size - size - 1;
      b->b_size = size;
      AddToFreeList(n);
    }
    sp = &(b->b_size) + b->b_size;
    *sp = b->b_size | InUseFlag;
    b->b_size |= InUseFlag;
    UNLOCK(FreeBlocksLock);
    return (Addr(b) + sizeof(YAP_SEG_SIZE));
  }
  LOCK(HeapTopLock);
  UNLOCK(FreeBlocksLock);
  b = (BlockHeader *) HeapTop;
  HeapTop += size * sizeof(CELL) + sizeof(YAP_SEG_SIZE);
  LOCK(HeapUsedLock);
  HeapUsed += size * sizeof(CELL) + sizeof(YAP_SEG_SIZE);

#ifdef YAPOR
  if (HeapTop > Addr(Yap_GlobalBase) - MinHeapGap)
    Yap_Error(INTERNAL_ERROR, TermNil, "no heap left (AllocHeap)");
#else
  if (HeapTop > HeapLim - MinHeapGap) {
    HeapTop -= size * sizeof(CELL) + sizeof(YAP_SEG_SIZE);
    HeapUsed -= size * sizeof(CELL) + sizeof(YAP_SEG_SIZE);
    if (HeapTop > HeapLim) {
      UNLOCK(HeapUsedLock);
      UNLOCK(HeapTopLock);
      /* we destroyed the stack */
      Yap_Error(OUT_OF_HEAP_ERROR, TermNil, "Stack Crashed against Heap...");
      return(NULL);
    } else {
      if (HeapTop + size * sizeof(CELL) + sizeof(YAP_SEG_SIZE) < HeapLim) {
	/* small allocations, we can wait */
	HeapTop += size * sizeof(CELL) + sizeof(YAP_SEG_SIZE);
	HeapUsed += size * sizeof(CELL) + sizeof(YAP_SEG_SIZE);
	UNLOCK(HeapUsedLock);
	UNLOCK(HeapTopLock);
	Yap_signal(YAP_CDOVF_SIGNAL);
      } else {
	if (size > SizeOfOverflow)
	  SizeOfOverflow = size*sizeof(CELL) + sizeof(YAP_SEG_SIZE);
	/* big allocations, the caller must handle the problem */
	UNLOCK(HeapUsedLock);
	UNLOCK(HeapTopLock);
	return(NULL);
      }
    }
  }
#endif /* YAPOR */
  *((YAP_SEG_SIZE *) HeapTop) = InUseFlag;
  if (HeapUsed > HeapMax)
    HeapMax = HeapUsed;
  UNLOCK(HeapUsedLock);
  b->b_size = size | InUseFlag;
  sp = &(b->b_size) + size;
  *sp = b->b_size;
  UNLOCK(HeapTopLock);
  return (Addr(b) + sizeof(YAP_SEG_SIZE));
}

/* If you need to dinamically allocate space from the heap, this is
 * the macro you should use */
static void
FreeCodeSpace(char *p)
{
  FreeBlock(((BlockHeader *) (p - sizeof(YAP_SEG_SIZE))));
}

static char *
AllocCodeSpace(unsigned long int size)
{
  if (size < SmallSize + 2 * OpCodeSize + 3 * CellSize)
    return (AllocHeap(SmallSize + 2 * OpCodeSize + 3 * CellSize));
  return (AllocHeap(size));
}


#if DEBUG_ALLOC
int vsc_mem_trace;
#endif

/* If you need to dinamically allocate space from the heap, this is
 * the macro you should use */
void
Yap_FreeCodeSpace(char *p)
{
#if DEBUG_ALLOC
  if (vsc_mem_trace)
    printf("-%p\n",p);
#endif
  FreeCodeSpace(p);
}

char *
Yap_AllocAtomSpace(unsigned long int size)
{
  char *out = AllocHeap(size);
#if DEBUG_ALLOC
  if (vsc_mem_trace) printf("+%p/%d\n",out,size);
#endif
  return out;
}

void
Yap_FreeAtomSpace(char *p)
{
#if DEBUG_ALLOC
  if (vsc_mem_trace)
    printf("-%p\n",p);
#endif
  FreeCodeSpace(p);
}

char *
Yap_AllocCodeSpace(unsigned long int size)
{
  char *out = AllocCodeSpace(size);
#if DEBUG_ALLOC
  if (vsc_mem_trace) printf("+%p/%d\n",out,size);
#endif
  return out;
}

#endif


/************************************************************************/
/* Workspace allocation                                                 */
/*                                                                      */
/* We provide four alternatives for workspace allocation.               */
/* - use 'mmap'                                                         */
/* - use 'shmat'                                                        */
/* - use 'sbrk' and provide a replacement to the 'malloc' library       */
/* - use 'malloc'                                                       */
/*                                                                      */
/*  In any of the alternatives the interface is through the following   */
/* functions:                                                           */
/*   void *InitWorkSpace(int s) - initial workspace allocation          */
/*   int ExtendWorkSpace(int s) - extend workspace                      */
/*   int Yap_FreeWorkSpace() - release workspace                            */
/************************************************************************/

#if defined(_WIN32) || defined(__CYGWIN__)

#undef DEBUG_WIN32_ALLOC

#include "windows.h"

static LPVOID brk;

static int
ExtendWorkSpace(Int s, int fixed_allocation)
{
  LPVOID b = brk;
  prolog_exec_mode OldPrologMode = Yap_PrologMode;



  Yap_PrologMode = ExtendStackMode;

#if DEBUG_WIN32_ALLOC
  fprintf(stderr,"trying: %p--%x %d\n",b, s, fixed_allocation);
#endif
  if (fixed_allocation) {
    b = VirtualAlloc(b, s, MEM_RESERVE, PAGE_NOACCESS);
  } else {
    b = VirtualAlloc(NULL, s, MEM_RESERVE, PAGE_NOACCESS);
    if (b && b < brk) {
      return ExtendWorkSpace(s, fixed_allocation);
    }
  }
  if (!b) {
    Yap_PrologMode = OldPrologMode;
#if DEBUG_WIN32_ALLOC
    {
      char msg[256];
      FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		    NULL, GetLastError(), 
		    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), msg, 256,
		    NULL);
      fprintf(stderr,"NOT OK1: %p--%p %s\n",b, brk, msg);
    }
#endif
    return FALSE;
  }
  b = VirtualAlloc(b, s, MEM_COMMIT, PAGE_READWRITE);
  if (!b) {
    Yap_ErrorMessage = Yap_ErrorSay;
    snprintf4(Yap_ErrorMessage, MAX_ERROR_MSG_SIZE,
	      "VirtualAlloc could not commit %ld bytes",
	      (long int)s);
    Yap_PrologMode = OldPrologMode;
#if DEBUG_WIN32_ALLOC
    fprintf(stderr,"NOT OK2: %p--%p\n",b,brk);
#endif
    return FALSE;
  }
  brk = (LPVOID) ((Int) b + s);
#if DEBUG_WIN32_ALLOC
  fprintf(stderr,"OK: %p--%p\n",b,brk);
#endif
  Yap_PrologMode = OldPrologMode;
  return TRUE;
}

static MALLOC_T
InitWorkSpace(Int s)
{
  SYSTEM_INFO si;

  GetSystemInfo(&si);
  Yap_page_size = si.dwPageSize;
  s = ((s+ (YAP_ALLOC_SIZE-1))/YAP_ALLOC_SIZE)*YAP_ALLOC_SIZE;
  brk = (LPVOID)Yap_page_size;
  if (!ExtendWorkSpace(s,0))
    return FALSE;
  return (MALLOC_T)brk-s;
}

int
Yap_FreeWorkSpace(void)
{
  return TRUE;
}

#elif USE_MMAP

#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_SYS_MMAN_H
#include <sys/mman.h>
#endif
#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#if HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef MMAP_ADDR
#define USE_FIXED 1
#endif

#ifndef MAP_FIXED
#define MAP_FIXED 1
#endif

static MALLOC_T WorkSpaceTop;

static MALLOC_T
InitWorkSpace(Int s)
{
  MALLOC_T a;
#if !defined(_AIX) && !defined(__APPLE__) &&  !__hpux
  int fd;
#endif
#if defined(_AIX)
  a = mmap(0, (size_t) s, PROT_READ | PROT_WRITE | PROT_EXEC,
	   MAP_PRIVATE | MAP_ANONYMOUS | MAP_VARIABLE, -1, 0);
#elif __hpux
  a = mmap(((void *)MMAP_ADDR), (size_t) s, PROT_READ | PROT_WRITE | PROT_EXEC,
	   MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0);
  if (a != (MALLOC_T)MMAP_ADDR) {
    Yap_Error(FATAL_ERROR, TermNil, "mmap could not map ANON at %p, got %p", (void *)MMAP_ADDR, a);
    return(NULL);
  }
#elif defined(__APPLE__)
  a = mmap(((void *)MMAP_ADDR), (size_t) s, PROT_READ | PROT_WRITE | PROT_EXEC,
	   MAP_PRIVATE | MAP_ANON | MAP_FIXED, -1, 0);
  if (a != (MALLOC_T)MMAP_ADDR) {
    Yap_Error(FATAL_ERROR, TermNil, "mmap could not map ANON at %p, got %p", (void *)MMAP_ADDR,a );
    return(NULL);
  }
#else
  fd = open("/dev/zero", O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
  if (fd < 0) {
#if HAVE_MKSTEMP
    char file[256];
    strncpy(file,"/tmp/YAP.TMPXXXXXX", 256);
    if (mkstemp(file) == -1) {
#if HAVE_STRERROR
      Yap_Error(FATAL_ERROR, TermNil, "mkstemp could not create temporary file %s (%s)", file, strerror(errno));
#else
      Yap_Error(FATAL_ERROR, TermNil, "mkstemp could not create temporary file %s", file);
#endif
      return NULL;
    }
#else
#if HAVE_TMPNAM
    char *file = tmpnam(NULL);
#else
    char file[YAP_FILENAME_MAX];
    strcpy(file,"/tmp/mapfile");
    itos(getpid(), &file[12]);
#endif /* HAVE_TMPNAM */
#endif /* HAVE_MKSTEMP */
    fd = open(file, O_CREAT|O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) {
      Yap_Error(FATAL_ERROR, TermNil, "mmap could not open %s", file);
      return NULL;
    }
    if (lseek(fd, s, SEEK_SET) < 0) {
      Yap_Error(FATAL_ERROR, TermNil, "mmap could not lseek in mmapped file %s", file);
      close(fd);
      return FALSE;
    }
    if (write(fd, "", 1) < 0) {
      Yap_Error(FATAL_ERROR, TermNil, "mmap could not write in mmapped file %s", file);
      close(fd);
      return NULL;
    }
    if (unlink(file) < 0) {
      Yap_Error(FATAL_ERROR,TermNil, "mmap could not unlink mmapped file %s", file);
      close(fd);
      return NULL;
    }
  }
#if USE_FIXED
  a = mmap(((void *)MMAP_ADDR), (size_t) s, PROT_READ | PROT_WRITE | PROT_EXEC,
	   MAP_PRIVATE | MAP_FIXED, fd, 0);
  if (a != (MALLOC_T)MMAP_ADDR) {
    Yap_Error(FATAL_ERROR, TermNil, "mmap could not map at %p, got %p", (void *)MMAP_ADDR, a);
    return NULL;
  }
#else
  a = mmap(0, (size_t) s, PROT_READ | PROT_WRITE | PROT_EXEC,
	   MAP_PRIVATE, fd, 0);
  if ((CELL)a & YAP_PROTECTED_MASK) {
    close(fd);
    Yap_Error(FATAL_ERROR, TermNil, "mmapped address %p collides with YAP tags", a);
    return NULL;
  }
  if (close(fd) == -1) {
    Yap_Error(FATAL_ERROR, TermNil, "while closing mmaped file");
    return NULL;
  }
#endif
#endif
  if
#ifdef MMAP_FAILED
	 (a == (MALLOC_T) MMAP_FAILED)
#else
	 (a == (MALLOC_T) - 1)
#endif
    {
      Yap_Error(FATAL_ERROR, TermNil, "mmap cannot allocate memory ***");
      return(NULL);
    }
  WorkSpaceTop = (char *) a + s;
  return (void *) a;
}

static MALLOC_T
mmap_extension(Int s, MALLOC_T base, int fixed_allocation)
{
  MALLOC_T a;

#if !defined(_AIX) && !defined(__hpux) && !defined(__APPLE__)
   int fd;  
#endif
#if defined(_AIX) || defined(__hpux)
   a = mmap(base, (size_t) s, PROT_READ | PROT_WRITE | PROT_EXEC,
		    MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

#elif defined(__APPLE__)
   a = mmap(base, (size_t) s, PROT_READ | PROT_WRITE | PROT_EXEC,
	    MAP_PRIVATE | MAP_ANON | fixed_allocation, -1, 0);
#else
   fd = open("/dev/zero", O_RDWR);
   if (fd < 0) {
#if HAVE_MKSTEMP
     char file[256];
     strncpy(file,"/tmp/YAP.TMPXXXXXX",256);
     if (mkstemp(file) == -1) {
       Yap_ErrorMessage = Yap_ErrorSay;
#if HAVE_STRERROR
       snprintf5(Yap_ErrorMessage, MAX_ERROR_MSG_SIZE,
		 "mkstemp could not create temporary file %s (%s)",
		 file, strerror(errno));
#else
       snprintf4(Yap_ErrorMessage, MAX_ERROR_MSG_SIZE,
		 "mkstemp could not create temporary file %s", file);
#endif /* HAVE_STRERROR */
       return (MALLOC_T)-1;
     }
#else
#if HAVE_TMPNAM
     char *file = tmpnam(NULL);
#else
     char file[YAP_FILENAME_MAX];
     strcpy(file,"/tmp/mapfile");
    itos(getpid(), &file[12]);
#endif /* HAVE_TMPNAM */
#endif /* HAVE_MKSTEMP */
    fd = open(file, O_CREAT|O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (fd < 0) {
      Yap_ErrorMessage = Yap_ErrorSay;
      snprintf4(Yap_ErrorMessage, MAX_ERROR_MSG_SIZE,
		"mmap could not open %s", file);
      return (MALLOC_T)-1;
    }
    if (lseek(fd, s, SEEK_SET) < 0) {
      Yap_ErrorMessage = Yap_ErrorSay;
      snprintf4(Yap_ErrorMessage, MAX_ERROR_MSG_SIZE,
		"mmap could not lseek in mmapped file %s", file);
      close(fd);
      return (MALLOC_T)-1;
    }
    if (write(fd, "", 1) < 0) {
      Yap_ErrorMessage = Yap_ErrorSay;
      snprintf4(Yap_ErrorMessage, MAX_ERROR_MSG_SIZE,
		"mmap could not write in mmapped file %s", file);
      close(fd);
      return (MALLOC_T)-1;
    }
    if (unlink(file) < 0) {
      Yap_ErrorMessage = Yap_ErrorSay;
      snprintf4(Yap_ErrorMessage, MAX_ERROR_MSG_SIZE,
		"mmap could not unlink mmapped file %s", file);
      close(fd);
      return (MALLOC_T)-1;
    }
  }
  a = mmap(base, (size_t) s, PROT_READ | PROT_WRITE | PROT_EXEC,
		    MAP_PRIVATE
#if !defined(__linux)
	   /* use  MAP_FIXED, otherwise God knows where you will be placed */
	   |fixed_allocation
#endif
	   , fd, 0);
  if (close(fd) == -1) {
    Yap_ErrorMessage = Yap_ErrorSay;
#if HAVE_STRERROR
    snprintf4(Yap_ErrorMessage, MAX_ERROR_MSG_SIZE,
	      "mmap could not close file (%s) ]\n", strerror(errno));
#else
    snprintf3(Yap_ErrorMessage, MAX_ERROR_MSG_SIZE,
	      "mmap could not close file ]\n");
#endif
    return (MALLOC_T)-1;
  }
#endif
  return a;
}

static int
ExtendWorkSpace(Int s, int fixed_allocation)
{
#ifdef YAPOR
  Yap_Error(INTERNAL_ERROR, TermNil, "cannot extend stacks (ExtendWorkSpace)");
  return(FALSE);
#else
  MALLOC_T a;
  prolog_exec_mode OldPrologMode = Yap_PrologMode;
  MALLOC_T base = WorkSpaceTop;

  if (fixed_allocation == MAP_FIXED)
    base = WorkSpaceTop;
  else
    base = 0L;
  Yap_PrologMode = ExtendStackMode;
  a = mmap_extension(s, base, fixed_allocation);
  Yap_PrologMode = OldPrologMode;
  if (a == (MALLOC_T) - 1) {
    Yap_ErrorMessage = Yap_ErrorSay;
#if HAVE_STRERROR
    snprintf5(Yap_ErrorMessage, MAX_ERROR_MSG_SIZE,
	      "could not allocate %d bytes (%s)", (int)s, strerror(errno));
#else
    snprintf4(Yap_ErrorMessage, MAX_ERROR_MSG_SIZE,
	      "could not allocate %d bytes", (int)s);
#endif
    return FALSE;
  }
  if (fixed_allocation) {
    if (a != WorkSpaceTop) {
      munmap((void *)a, (size_t)s);
      Yap_ErrorMessage = Yap_ErrorSay;
      snprintf5(Yap_ErrorMessage, MAX_ERROR_MSG_SIZE,
		"mmap could not grow memory at %p, got %p", WorkSpaceTop, a );
      Yap_PrologMode = OldPrologMode;
      return FALSE;
    }
  } else if (a < WorkSpaceTop) {
    /* try again */
    int res = ExtendWorkSpace(s, fixed_allocation);
    /* release memory back to system */
    munmap(a, s);
    return res;
  }
  WorkSpaceTop = (char *) a + s;
  Yap_PrologMode = OldPrologMode;
  return TRUE;
#endif /* YAPOR */
}

int
Yap_FreeWorkSpace(void)
{
  return 1;
}

#elif USE_SHM

#if HAVE_SYS_SHM_H
#include <sys/shm.h>
#endif

#ifndef MMAP_ADDR
#define MMAP_ADDR 0x0L
#endif

static MALLOC_T WorkSpaceTop;

static MALLOC_T
InitWorkSpace(Int s)
{
  MALLOC_T ptr;
  int shm_id;

  /* mapping heap area */
  if((shm_id = shmget(IPC_PRIVATE, (size_t)s, SHM_R|SHM_W)) == -1) {
    Yap_Error(FATAL_ERROR, TermNil, "could not shmget %d bytes", s);
    return(NULL);
   }
  if((ptr = (MALLOC_T)shmat(shm_id, (void *) MMAP_ADDR, 0)) == (MALLOC_T) -1) {
    Yap_Error(FATAL_ERROR, TermNil, "could not shmat at %p", MMAP_ADDR);
    return(NULL);
  }
  if (shmctl(shm_id, IPC_RMID, 0) != 0) {
    Yap_Error(FATAL_ERROR, TermNil, "could not remove shm segment", shm_id);
    return(NULL);
  }
  WorkSpaceTop = (char *) ptr + s;
  return(ptr);
}

static int
ExtendWorkSpace(Int s)
{
  MALLOC_T ptr;
  int shm_id;
  prolog_exec_mode OldPrologMode = Yap_PrologMode;

  Yap_PrologMode = ExtendStackMode;
  /* mapping heap area */
  if((shm_id = shmget(IPC_PRIVATE, (size_t)s, SHM_R|SHM_W)) == -1) {
    Yap_ErrorMessage = Yap_ErrorSay;
    snprintf4(Yap_ErrorMessage, MAX_ERROR_MSG_SIZE,
	      "could not shmget %d bytes", s);
    Yap_PrologMode = OldPrologMode;
    return(FALSE);
   }
  if((ptr = (MALLOC_T)shmat(shm_id, WorkSpaceTop, 0)) == (MALLOC_T) -1) {
    Yap_ErrorMessage = Yap_ErrorSay;
    snprintf4(Yap_ErrorMessage, MAX_ERROR_MSG_SIZE,
	      "could not shmat at %p", MMAP_ADDR);
    Yap_PrologMode = OldPrologMode;
    return(FALSE);
  }
  if (shmctl(shm_id, IPC_RMID, 0) != 0) {
    Yap_ErrorMessage = Yap_ErrorSay;
    snprintf4(Yap_ErrorMessage, MAX_ERROR_MSG_SIZE,
	      "could not remove shm segment", shm_id);
    Yap_PrologMode = OldPrologMode;
    return(FALSE);
  }
  WorkSpaceTop = (char *) ptr + s;
  Yap_PrologMode = OldPrologMode;
  return(TRUE);
}

int
Yap_FreeWorkSpace(void)
{
  return TRUE;
}

#elif USE_SBRK

/***********************************************************************\
* Worspace allocation based on 'sbrk'					*
*   We have to provide a replacement for the 'malloc' functions.        *
*   The situation is further complicated by the need to provide         *
* temporary 'malloc' space when restoring a previously saved state.	*
\***********************************************************************/

#ifdef _AIX
char *STD_PROTO(sbrk, (int));

#endif

int in_limbo;		/* non-zero when restoring a saved state */

#ifndef LIMBO_SIZE
#define LIMBO_SIZE 32*K
#endif

static char limbo_space[LIMBO_SIZE];	/* temporary malloc space */
static char *limbo_p = limbo_space, *limbo_pp = 0;

static MALLOC_T
InitWorkSpace(Int s)
{
  MALLOC_T ptr = (MALLOC_T)sbrk(s);

  if (ptr == ((MALLOC_T) - 1)) {
     Yap_Error(FATAL_ERROR, TermNil, "could not allocate %d bytes", s);
     return(NULL);
  }
  return(ptr);
}

static int
ExtendWorkSpace(Int s)
{
  MALLOC_T ptr = (MALLOC_T)sbrk(s);
  prolog_exec_mode OldPrologMode = Yap_PrologMode;

  Yap_PrologMode = ExtendStackMode;
  if (ptr == ((MALLOC_T) - 1)) {
    Yap_ErrorMessage = Yap_ErrorSay;
    snprintf4(Yap_ErrorMessage, MAX_ERROR_MSG_SIZE,
	      "could not expand stacks over %d bytes", s);
    Yap_PrologMode = OldPrologMode;
    return(FALSE);
  }
  Yap_PrologMode = OldPrologMode;
  return TRUE;
}

int
Yap_FreeWorkSpace(void)
{
  return TRUE;
}

MALLOC_T
malloc(size_t size)
{
  if (in_limbo) {
    limbo_pp = limbo_p;
    limbo_p += (size + 7) & 0xffff8;
    if (limbo_p >= &limbo_space[LIMBO_SIZE])
      return(NULL);
    return (limbo_pp);
  }
  else {
    CODEADDR codep = (CODEADDR)AllocCodeSpace(size + 2*sizeof(void *));
    if (codep == NIL)
      return(NIL);
    else
      return(codep + 2*sizeof(void *));
  }
}

void
free(MALLOC_T ptr)
{
  BlockHeader *b = (BlockHeader *) (((char *) ptr) - 2*sizeof(void *) - sizeof(YAP_SEG_SIZE));

  if (ptr == limbo_pp) {
    limbo_p = limbo_pp;
    return;
  }
  if (!ptr)
    return;
  if ((char *) ptr < Yap_HeapBase || (char *) ptr > HeapTop)
    return;
  if (!(b->b_size & InUseFlag))
    return;
  FreeCodeSpace((char *) ptr - 2*sizeof(void *));
}

MALLOC_T
XX realloc(MALLOC_T ptr, size_t size)
{
  MALLOC_T new = malloc(size);

  if (ptr)
    memcpy(new, ptr, size);
  free(ptr);
  return (new);
}

MALLOC_T
calloc(size_t n, size_t e)
{
  unsigned k = n * e;
  MALLOC_T p = malloc(k);

  memset(p, 0, k);
  return (p);
}

#ifdef M_MXFAST
int
mallopt(cmd, value)
{
  return (value);
}

static struct mallinfo xmall;

struct mallinfo
mallinfo(void)
{
  return (xmall);
}
#endif

#else

/* use malloc to initiliase memory */

/* user should ask for a lot of memory first */

#ifdef __linux
#define MAX_SPACE 420*1024*1024
#else
#define MAX_SPACE 128*1024*1024
#endif

static int total_space;

static MALLOC_T
InitWorkSpace(Int s)
{
  MALLOC_T ptr;

#ifdef M_MMAP_MAX
  mallopt(M_MMAP_MAX, 0);
#endif
  ptr = (MALLOC_T)calloc(MAX_SPACE,1);
  total_space = s;

  if (ptr == NULL) {
     Yap_Error(FATAL_ERROR, TermNil, "could not allocate %d bytes", s);
     return(NULL);
  }
  return(ptr);
}

static int
ExtendWorkSpace(Int s)
{
  MALLOC_T ptr;
  prolog_exec_mode OldPrologMode = Yap_PrologMode;

  Yap_PrologMode = ExtendStackMode;
  total_space += s;
  if (total_space < MAX_SPACE) return TRUE;
  ptr = (MALLOC_T)realloc((void *)Yap_HeapBase, total_space);
  if (ptr == NULL) {
    Yap_ErrorMessage = Yap_ErrorSay;
    snprintf4(Yap_ErrorMessage, MAX_ERROR_MSG_SIZE,
	      "could not allocate %d bytes", s);
    Yap_PrologMode = OldPrologMode;
    return FALSE;
  }
  if (ptr != (MALLOC_T)Yap_HeapBase) {
    Yap_ErrorMessage = Yap_ErrorSay;
    snprintf4(Yap_ErrorMessage, MAX_ERROR_MSG_SIZE,
	      "could not expand contiguous stacks  %d bytes", s);
    Yap_PrologMode = OldPrologMode;
    return FALSE;
  }
  if ((CELL)ptr & MBIT) {
    Yap_ErrorMessage = Yap_ErrorSay;
    snprintf5(Yap_ErrorMessage, MAX_ERROR_MSG_SIZE,
	      "memory at %p conflicts with MBIT %lx", ptr, (unsigned long)MBIT);
    Yap_PrologMode = OldPrologMode;
    return FALSE;
  }
  Yap_PrologMode = OldPrologMode;
  return TRUE;
}

int
Yap_FreeWorkSpace(void)
{
  return TRUE;
}
#endif

static void
InitHeap(void *heap_addr)
{
  /* allocate space */
  Yap_HeapBase = heap_addr;

  /* reserve space for specially allocated functors and atoms so that
     their values can be known statically */
  HeapTop = Yap_HeapBase + AdjustSize(sizeof(all_heap_codes));
  Yap_HoleSize = 0;
#if USE_DL_MALLOC
  Yap_initdlmalloc();
#else
  HeapMax = HeapUsed = HeapTop-Yap_HeapBase;
  /* notice that this forces odd addresses */
  *((YAP_SEG_SIZE *) HeapTop) = InUseFlag;
  HeapTop = HeapTop + sizeof(YAP_SEG_SIZE);
  *((YAP_SEG_SIZE *) HeapTop) = InUseFlag;
#endif

  FreeBlocks = NIL;

#if defined(YAPOR) || defined(TABLING)
  LOCAL = REMOTE; /* point to the first area */
#endif /* YAPOR || TABLING */
}

void
Yap_InitHeap(void *heap_addr)
{
  InitHeap(heap_addr);
}

void
Yap_InitMemory(UInt Trail, UInt Heap, UInt Stack)
{
  UInt pm, sa, ta;
  void *addr;

  pm = (Trail + Heap + Stack);	/* memory to be
				 * requested         */
  sa = Stack;			/* stack area size   */
  ta = Trail;			/* trail area size   */


#if RANDOMIZE_START_ADDRESS
  srand(time(NULL));
  UInt x = (rand()% 100)*YAP_ALLOC_SIZE ;
  fprintf(stderr,"x=%lx\n", (unsigned long int)x);
  pm += x;
#endif
  addr = InitWorkSpace(pm);
#if RANDOMIZE_START_ADDRESS
  addr = (char *)addr+x;
  pm -= x;
  fprintf(stderr,"addr=%p\n", addr);
#endif

  InitHeap(addr);

  Yap_TrailTop = Yap_HeapBase + pm;
  Yap_LocalBase = Yap_TrailTop - ta;
  Yap_TrailBase = Yap_LocalBase + sizeof(CELL);

  Yap_GlobalBase = Yap_LocalBase - sa;
  HeapLim = Yap_GlobalBase;	/* avoid confusions while
					 * * restoring */
#if !USE_DL_MALLOC
  AuxTop = (ADDR)(AuxSp = (CELL *)Yap_GlobalBase);
#endif

#ifdef DEBUG
#if SIZEOF_INT_P!=SIZEOF_INT
  if (Yap_output_msg) {
    fprintf(stderr, "HeapBase = %p  GlobalBase = %p\n  LocalBase = %p  TrailTop = %p\n",
	       Yap_HeapBase, Yap_GlobalBase, Yap_LocalBase, Yap_TrailTop);
#else
  if (Yap_output_msg) {
    fprintf(stderr, "HeapBase = %x  GlobalBase = %x\n  LocalBase = %x  TrailTop = %x\n",
	       (UInt) Yap_HeapBase, (UInt) Yap_GlobalBase,
	       (UInt) Yap_LocalBase, (UInt) Yap_TrailTop);
#endif

#if !SHORT_INTS
    fprintf(stderr, "Heap+Aux: %d\tLocal+Global: %d\tTrail: %d\n",
	       pm - sa - ta, sa, ta);
#else /* SHORT_INTS */
    fprintf(stderr, "Heap+Aux: %ld\tLocal+Global: %ld\tTrail: %ld\n",
	       pm - sa - ta, sa, ta);
#endif /* SHORT_INTS */
  }
#endif /* DEBUG */
}

void
Yap_InitExStacks(int Trail, int Stack)
{
#if USE_DL_MALLOC
  ScratchPad.ptr = NULL;
  ScratchPad.sz = ScratchPad.msz = SCRATCH_START_SIZE;
  AuxSp = NULL;
#endif
}

#if defined(_WIN32) || defined(__CYGWIN__)
#define WorkSpaceTop brk
#define MAP_FIXED 1
#endif

#if !USE_DL_MALLOC
/* dead code */
void Yap_add_memory_hole(ADDR Start, ADDR End)
{
  Yap_HoleSize += Start-End;
}
#endif

int
Yap_ExtendWorkSpace(Int s)
{
#if USE_MMAP
  return ExtendWorkSpace(s, MAP_FIXED);
#elif defined(_WIN32)
  return ExtendWorkSpace(s, MAP_FIXED);
#else
  return ExtendWorkSpace(s);
#endif
}

UInt
Yap_ExtendWorkSpaceThroughHole(UInt s)
{
#if USE_MMAP || defined(_WIN32) || defined(__CYGWIN__)
  MALLOC_T WorkSpaceTop0 = WorkSpaceTop;
#if SIZEOF_INT_P==4
  while (WorkSpaceTop < (MALLOC_T)0xc0000000L) {
    /* progress 1 MB */
    WorkSpaceTop += 512*1024;
    if (ExtendWorkSpace(s, MAP_FIXED)) {
      Yap_add_memory_hole((ADDR)WorkSpaceTop0, (ADDR)WorkSpaceTop-s);
      Yap_ErrorMessage = NULL;
      return WorkSpaceTop-WorkSpaceTop0;
    }
#if defined(_WIN32)
    /* 487 happens when you step over someone else's memory */
    if (GetLastError() != 487) {
      WorkSpaceTop = WorkSpaceTop0;
      return -1;
    }
#endif
  }
  WorkSpaceTop = WorkSpaceTop0;
#endif
  if (ExtendWorkSpace(s, 0)) {
    Yap_add_memory_hole((ADDR)WorkSpaceTop0, (ADDR)WorkSpaceTop-s);
    Yap_ErrorMessage = NULL;
    return WorkSpaceTop-WorkSpaceTop0;
  }
#endif
  return -1;
}

void
Yap_AllocHole(UInt actual_request, UInt total_size)
{
#if (USE_MMAP || defined(_WIN32) || defined(__CYGWIN__)) && !USE_DL_MALLOC
  /* where we were when the hole was created,
   also where is the hole store */
  ADDR WorkSpaceTop0 = WorkSpaceTop-total_size;
  BlockHeader *newb = (BlockHeader *)HeapTop;
  BlockHeader *endb = (BlockHeader *)(WorkSpaceTop0-sizeof(YAP_SEG_SIZE));
  YAP_SEG_SIZE bsiz = (WorkSpaceTop0-HeapTop)/sizeof(CELL)-2*sizeof(YAP_SEG_SIZE)/sizeof(CELL);

  /* push HeapTop to after hole */
  HeapTop = WorkSpaceTop-(actual_request-sizeof(YAP_SEG_SIZE));
  ((YAP_SEG_SIZE *) HeapTop)[0] = InUseFlag;
  /* now simulate a block */
  ((YAP_SEG_SIZE *) HeapTop)[-1] =
    endb->b_size =
    (HeapTop-WorkSpaceTop0)/sizeof(YAP_SEG_SIZE) | InUseFlag;
  newb->b_size = bsiz;
  AddToFreeList(newb);
#endif
}

#endif /* USE_SYSTEM_MALLOC */


