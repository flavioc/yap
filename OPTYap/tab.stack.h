/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   R. Rocha and NCC - University of Porto, Portugal
  File:        tab.stack.h
                                                                     
**********************************************************************/

#ifndef TAB_STACK_H
#define TAB_STACK_H

#ifdef TABLING_CALL_SUBSUMPTION

// emu/dynamic_stack.h
typedef struct {
  struct {
    void *top; /* next available frame */
    void *base; /* stack bottom */
    void *ceiling; /* off-end pointer for determining fullness */
  } stack;
  struct {
    size_t frame; /* size of a frame */
    size_t stackinit; /* initial size of stack in number of frames */
    size_t stackcur; /* current size of stack in number of frames */
  } size;
  char *name;
} DynamicStack;

#define DynStk_Top(DS)        ((DS).stack.top)
#define DynStk_Base(DS)       ((DS).stack.base)
#define DynStk_Ceiling(DS)    ((DS).stack.ceiling)
#define DynStk_FrameSize(DS)  ((DS).size.frame)
#define DynStk_InitSize(DS)   ((DS).size.stackinit)
#define DynStk_CurSize(DS)    ((DS).size.stackcur)
#define DynStk_Name(DS)       ((DS).name)

#define DynStk_CurBytes(DS)  \
  ((char *)DynStk_Top(DS) - (char *)DynStk_Base(DS))

#define DynStk_NumFrames(DS)  \
  (DynStk_CurBytes(DS) / DynStk_FrameSize(DS))

/* Top-of-Stack manipulations */
#define DynStk_NextFrame(DS)  \
    (void *)((char *)DynStk_Top(DS) + DynStk_FrameSize(DS))
#define DynStk_PrevFrame(DS)  \
    (void *)((char *)DynStk_Top(DS) - DynStk_FrameSize(DS))

/* stack maintenance */
extern void dynamic_stack_print(DynamicStack);
extern void dynamic_stack_init(DynamicStack *, size_t, size_t, char*);
extern void dynamic_stack_expand(DynamicStack *, int);

#define DynStk_Init(DS, NUM_ELEM, FRAME_TYPE, DESC) \
  dynamic_stack_init(DS, NUM_ELEM, sizeof(FRAME_TYPE), DESC)

#define DynStk_ResetTOS(DS)   DynStk_Top(DS) = DynStk_Base(DS)
#define DynStk_IsEmpty(DS)    (DynStk_Top(DS) == DynStk_Base(DS))
#define DynStk_IsFull(DS)     (DynStk_Top(DS) >= DynStk_Ceiling(DS))

#define DynStk_ErrorIfFull(DS)  \
    if(DynStk_IsFull(DS)) \
      Yap_Error(FATAL_ERROR, TermNil, "dynamic stack %s full", DynStk_Name(DS))

#define DynStk_WillOverflow(DS, NFrames)  \
      ((char *)DynStk_Top(DS) + NFrames * DynStk_FrameSize(DS)  \
        > (char *)DynStk_Ceiling(DS))
        
#define DynStk_ExpandIfFull(DS) { \
    if(DynStk_IsFull(DS)) \
      dynamic_stack_expand(&(DS), 1); \
    }
    
#define DynStk_ExpandIfOverflow(DS, N) { \
  if(DynStk_WillOverflow(DS, N)) \
    dynamic_stack_expand(&(DS), N); \
  }

/* stack operations with error checking */
#define DynStk_Push(DS, Frame) { \
      DynStk_ExpandIfFull(DS); \
      DynStk_BlindPush(DS, Frame); \
    }

#define DynStk_Pop(DS, Frame) { \
  if(!DynStk_IsEmpty(DS)) \
    DynStk_BlindPop(DS, Frame) \
  else \
    Frame = NULL; \
  }
  
#define DynStk_Peek(DS, Frame) { \
  if(!DynStk_IsEmpty(DS)) \
    DynStk_BlindPeek(DS, Frame); \
  else \
    Frame = NULL; \
  }
  
/* operations without bounds checking */
#define DynStk_BlindPush(DS, Frame) { \
  Frame = DynStk_Top(DS); \
  DynStk_Top(DS) = DynStk_NextFrame(DS); \
}

#define DynStk_BlindPop(DS, Frame) { \
  DynStk_Top(DS) = DynStk_PrevFrame(DS); \
  Frame = DynStk_Top(DS); \
}

#define DynStk_BlindPeek(DS, Frame) \
  Frame = DynStk_PrevFrame(DS)

#endif /* TABLING_CALL_SUBSUMPTION */
#endif