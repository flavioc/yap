/*************************************************************************
*									 *
*	 YAP Prolog 	@(#)c_interface.h	2.2			 *
*	Yap Prolog was developed at NCCUP - Universidade do Porto	 *
*									 *
* Copyright L.Damas, V.S.Costa and Universidade do Porto 1985-1997	 *
*									 *
**************************************************************************
*									 *
* File:		YapInterface.h						 *
* Last rev:	19/2/88							 *
* mods:									 *
* comments:	c_interface header file for YAP				 *
*									 *
*************************************************************************/

/*******************  IMPORTANT ********************
   Due to a limitation of the DecStation loader any function (including
   library functions) which is linked to yap can not be called directly
   from C code loaded dynamically.
      To go around this problem we adopted the solution of calling such
   functions indirectly
****************************************************/

#include "yap_structs.h"

#ifndef _yap_c_interface_h

#define _yap_c_interface_h 1

/*
   __BEGIN_DECLS should be used at the beginning of the C declarations,
   so that C++ compilers don't mangle their names.  __END_DECLS is used
   at the end of C declarations.
*/
#undef __BEGIN_DECLS
#undef __END_DECLS
#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS /* empty */
# define __END_DECLS /* empty */
#endif /* _cplusplus */

__BEGIN_DECLS

#if defined(_MSC_VER) && defined(YAP_EXPORTS)
#define X_API __declspec(dllexport)
#else
#define X_API
#endif

/* Primitive Functions */

#define YAP_Deref(t)  (t)
extern X_API YAP_Term PROTO(YAP_A,(int));
#define YAP_ARG1	YAP_A(1)
#define YAP_ARG2	YAP_A(2)
#define YAP_ARG3	YAP_A(3)
#define YAP_ARG4	YAP_A(4)
#define YAP_ARG5	YAP_A(5)
#define YAP_ARG6	YAP_A(6)
#define YAP_ARG7	YAP_A(7)
#define YAP_ARG8	YAP_A(8)
#define YAP_ARG9	YAP_A(9)
#define YAP_ARG10	YAP_A(10)
#define YAP_ARG11	YAP_A(11)
#define YAP_ARG12	YAP_A(12)
#define YAP_ARG13	YAP_A(13)
#define YAP_ARG14	YAP_A(14)
#define YAP_ARG15	YAP_A(15)
#define YAP_ARG16	YAP_A(16)

/*  YAP_Bool IsVarTerm(YAP_Term) */
extern X_API YAP_Bool PROTO(YAP_IsVarTerm,(YAP_Term));

/*  YAP_Bool IsNonVarTerm(YAP_Term) */
extern X_API YAP_Bool PROTO(YAP_IsNonVarTerm,(YAP_Term));

/*  YAP_Term  MkVarTerm()  */
extern X_API YAP_Term PROTO(YAP_MkVarTerm,(void));

/*  YAP_Bool IsIntTerm(YAP_Term)  */
extern X_API YAP_Bool PROTO(YAP_IsIntTerm,(YAP_Term));

/*  YAP_Bool IsFloatTerm(YAP_Term)  */
extern X_API YAP_Bool PROTO(YAP_IsFloatTerm,(YAP_Term));

/*  YAP_Bool IsDbRefTerm(YAP_Term)  */
extern X_API YAP_Bool PROTO(YAP_IsDbRefTerm,(YAP_Term));

/*  YAP_Bool IsAtomTerm(YAP_Term)  */
extern X_API YAP_Bool PROTO(YAP_IsAtomTerm,(YAP_Term));

/*  YAP_Bool IsPairTerm(YAP_Term)  */
extern X_API YAP_Bool PROTO(YAP_IsPairTerm,(YAP_Term));

/*  YAP_Bool IsApplTerm(YAP_Term)  */
extern X_API YAP_Bool PROTO(YAP_IsApplTerm,(YAP_Term));

/*    Term MkIntTerm(long int)  */
extern X_API YAP_Term PROTO(YAP_MkIntTerm,(long int));

/*    long int  IntOfTerm(Term) */
extern X_API long int PROTO(YAP_IntOfTerm,(YAP_Term));

/*    Term MkFloatTerm(double)  */
extern X_API YAP_Term PROTO(YAP_MkFloatTerm,(double));

/*    double  FloatOfTerm(YAP_Term) */
extern X_API double PROTO(YAP_FloatOfTerm,(YAP_Term));

/*    Term MkAtomTerm(Atom)  */
extern X_API YAP_Term PROTO(YAP_MkAtomTerm,(YAP_Atom));

/*    YAP_Atom  AtomOfTerm(Term) */
extern X_API YAP_Atom PROTO(YAP_AtomOfTerm,(YAP_Term));

/*    YAP_Atom  LookupAtom(char *) */
extern X_API YAP_Atom PROTO(YAP_LookupAtom,(char *));

/*    YAP_Atom  FullLookupAtom(char *) */
extern X_API YAP_Atom PROTO(YAP_FullLookupAtom,(char *));

/*    char* AtomName(YAP_Atom) */
extern X_API char *PROTO(YAP_AtomName,(YAP_Atom));

/*    YAP_Term  MkPairTerm(YAP_Term Head, YAP_Term Tail) */
extern X_API YAP_Term PROTO(YAP_MkPairTerm,(YAP_Term,YAP_Term));

/*    YAP_Term  MkNewPairTerm(void) */
extern X_API YAP_Term PROTO(YAP_MkNewPairTerm,(void));

/*    Term  HeadOfTerm(Term)  */
extern X_API YAP_Term PROTO(YAP_HeadOfTerm,(YAP_Term));

/*    Term  TailOfTerm(Term)  */
extern X_API YAP_Term PROTO(YAP_TailOfTerm,(YAP_Term));

/*    YAP_Term     MkApplTerm(YAP_Functor f, unsigned long int n, YAP_Term[] args) */
extern X_API YAP_Term PROTO(YAP_MkApplTerm,(YAP_Functor,int,YAP_Term *));

/*    YAP_Term     MkNewApplTerm(YAP_Functor f, unsigned long int n) */
extern X_API YAP_Term PROTO(YAP_MkNewApplTerm,(YAP_Functor,unsigned long int));

/*    YAP_Functor  YAP_FunctorOfTerm(Term)  */
extern X_API YAP_Functor PROTO(YAP_FunctorOfTerm,(YAP_Term));

/*    YAP_Term     ArgOfTerm(unsigned int argno,YAP_Term t) */
extern X_API YAP_Term PROTO(YAP_ArgOfTerm,(unsigned int,YAP_Term));

/*    YAP_Functor  MkFunctor(YAP_Atom a,int arity) */
extern X_API YAP_Functor PROTO(YAP_MkFunctor,(YAP_Atom,unsigned long int));

/*    YAP_Atom     NameOfFunctor(Functor) */
extern X_API YAP_Atom PROTO(YAP_NameOfFunctor,(YAP_Functor));

/*    unsigned long int     YAP_ArityOfFunctor(Functor) */
extern X_API unsigned long int PROTO(YAP_ArityOfFunctor,(YAP_Functor));

/*  void ExtraSpace(void) */
extern X_API void *PROTO(YAP_ExtraSpace,(void));

#define YAP_PRESERVE_DATA(ptr, type) (ptr = (type *)YAP_ExtraSpace())
#define YAP_PRESERVED_DATA(ptr, type) (ptr = (type *)YAP_ExtraSpace())

/*   YAP_Bool      unify(YAP_Term a, YAP_Term b) */
extern X_API YAP_Bool PROTO(YAP_Unify,(YAP_Term, YAP_Term));

/*  void UserCPredicate(char *name, int *fn(), int arity) */
extern X_API void PROTO(YAP_UserCPredicate,(char *, YAP_Bool (*)(void), unsigned long int));

/*  void UserCPredicateWithArgs(char *name, int *fn(), long int arity) */
extern X_API void PROTO(YAP_UserCPredicateWithArgs,(char *, YAP_Bool (*)(void), long int,long int));

/*  void UserBackCPredicate(char *name, int *init(), int *cont(), int
    arity, int extra) */
extern X_API void PROTO(YAP_UserBackCPredicate,(char *, YAP_Bool (*)(void), YAP_Bool (*)(void), unsigned long int, unsigned int));

/*  void CallProlog(YAP_Term t) */
extern X_API YAP_Bool PROTO(YAP_CallProlog,(YAP_Term t));

/*  void cut_fail(void) */
extern X_API void PROTO(YAP_cut_fail,(void));

/*  void cut_succeed(void) */
extern X_API void PROTO(YAP_cut_succeed,(void));

/*  void *AllocSpaceFromYAP_(int) */
extern X_API void *PROTO(YAP_AllocSpaceFromYap,(unsigned int));

/*  void FreeSpaceFromYAP_(void *) */
extern X_API void PROTO(YAP_FreeSpaceFromYap,(void *));

/*  int YAP_RunGoal(YAP_Term) */
extern X_API YAP_Bool PROTO(YAP_RunGoal,(YAP_Term));

/*  int YAP_RestartGoal(void) */
extern X_API YAP_Bool PROTO(YAP_RestartGoal,(void));

/*  int YAP_ContinueGoal(void) */
extern X_API YAP_Bool PROTO(YAP_ContinueGoal,(void));

/*  void YAP_PruneGoal(void) */
extern X_API void PROTO(YAP_PruneGoal,(void));

/*  int YAP_GoalHasException(void) */
extern X_API YAP_Bool PROTO(YAP_GoalHasException,(YAP_Term *));

/*  int YAP_Reset(void) */
extern X_API void PROTO(YAP_Reset,(void));

/*  void YAP_Error(char *) */
extern X_API void PROTO(YAP_Error,(char *));

/*  YAP_Term YAP_Read(int (*)(void)) */
extern X_API YAP_Term PROTO(YAP_Read,(int (*)(void)));

/*  void YAP_Write(YAP_Term,void (*)(int),int) */
extern X_API void PROTO(YAP_Write,(YAP_Term,void (*)(int),int));

/*  char *YAP_CompileClause(YAP_Term) */
extern X_API char *PROTO(YAP_CompileClause,(YAP_Term));

/*  int YAP_Init(YAP_init_args *) */
extern X_API int PROTO(YAP_Init,(YAP_init_args *));

/*  int YAP_FastInit(char *) */
extern X_API int PROTO(YAP_FastInit,(char *));

/*  int YAP_InitConsult(int, char *) */
extern X_API int PROTO(YAP_InitConsult,(int, char *));

/*  int YAP_StartConsult(int, char *) */
extern X_API int PROTO(YAP_EndConsult,(void));

/*  void YAP_Exit(int) */
extern X_API void PROTO(YAP_Exit,(int));

/*  void YAP_PutValue(YAP_Atom, YAP_Term) */
extern X_API void PROTO(YAP_PutValue,(YAP_Atom, YAP_Term));

/*  YAP_Term YAP_GetValue(YAP_Atom) */
extern X_API YAP_Term PROTO(YAP_GetValue,(YAP_Atom));

/*  int StringToBuffer(YAP_Term,char *,unsigned int) */
extern X_API int PROTO(YAP_StringToBuffer,(YAP_Term,char *,unsigned int));

/*  int BufferToString(char *) */
extern X_API YAP_Term PROTO(YAP_BufferToString,(char *));

/*  int BufferToAtomList(char *) */
extern X_API YAP_Term PROTO(YAP_BufferToAtomList,(char *));

/*  void YAP_InitSocks(char *,long) */
extern X_API int PROTO(YAP_InitSocks,(char *,long));

#ifdef  SFUNC

#define SFArity  0
extern X_API YAP_Term *ArgsOfSFTerm();

extern X_API YAP_Term MkSFTerm();

#endif /* SFUNC */

/*  YAP_Term  YAP_SetOutputMessage()  */
extern X_API void PROTO(YAP_SetOutputMessage,(void));

/*  YAP_Term  YAP_SetOutputMessage()  */
extern X_API int PROTO(YAP_StreamToFileNo,(YAP_Term));

/*  YAP_Term  YAP_SetOutputMessage()  */
extern X_API void PROTO(YAP_CloseAllOpenStreams,(void));

#define YAP_INPUT_STREAM	0x01
#define YAP_OUTPUT_STREAM	0x02
#define YAP_APPEND_STREAM	0x04
#define YAP_PIPE_STREAM 	0x08
#define YAP_TTY_STREAM	 	0x10
#define YAP_POPEN_STREAM	0x20
#define YAP_BINARY_STREAM	0x40
#define YAP_SEEKABLE_STREAM	0x80

/*  YAP_Term  YAP_OpenStream()  */
extern X_API YAP_Term PROTO(YAP_OpenStream,(void *, char *, YAP_Term, int));

/*  YAP_Term  *YAP_NewSlots()  */
extern X_API long PROTO(YAP_NewSlots,(int));

/*  YAP_Term  *YAP_InitSlot()  */
extern X_API long PROTO(YAP_InitSlot,(YAP_Term));

/*  YAP_Term  YAP_GetFromSlots(t)  */
extern X_API YAP_Term PROTO(YAP_GetFromSlot,(long int));

/*  YAP_Term  YAP_AddressFromSlots(t)  */
extern X_API YAP_Term *PROTO(YAP_AddressFromSlot,(long int));

/*  YAP_Term  YAP_PutInSlots(t)  */
extern X_API void PROTO(YAP_PutInSlot,(long int, YAP_Term));

/*  void  YAP_RecoverSlots()  */
extern X_API void PROTO(YAP_RecoverSlots,(int));

/*  void  YAP_Throw()  */
extern X_API void PROTO(YAP_Throw,(YAP_Term));

/*  int  YAP_LookupModule()  */
extern X_API int  PROTO(YAP_LookupModule,(YAP_Term));

/*  int  YAP_ModuleName()  */
extern X_API YAP_Term  PROTO(YAP_ModuleName,(int));

/*  int  YAP_Halt()  */
extern X_API int  PROTO(YAP_Halt,(int));

/*  int  YAP_TopOfLocalStack()  */
extern X_API YAP_Term  *PROTO(YAP_TopOfLocalStack,(void));

/*  int  YAP_Predicate()  */
extern X_API void  *PROTO(YAP_Predicate,(YAP_Atom,unsigned long int,int));

/*  int  YAP_Predicate()  */
extern X_API void  PROTO(YAP_PredicateInfo,(void *,YAP_Atom *,unsigned long int*,int*));

/*  int  YAP_Predicate()  */
extern X_API int  PROTO(YAP_CurrentModule,(void));

#define YAP_InitCPred(N,A,F)  YAP_UserCPredicate(N,F,A)

__END_DECLS

#endif

