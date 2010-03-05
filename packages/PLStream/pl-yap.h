#ifndef PL_YAP_H
#define PL_YAP_H

#ifdef __YAP_PROLOG__

#if HAVE_CTYPE_H
#include <ctype.h>
#endif

#define SIZE_VOIDP SIZEOF_INT_P

#if SIZE_DOUBLE==SIZEOF_INT_P
#define WORDS_PER_DOUBLE 1
#else
#define WORDS_PER_DOUBLE 2
#endif

#if SIZEOF_LONG_INT==4
#define INT64_FORMAT "%lld"
#define INTBITSIZE 32
#else
#define INT64_FORMAT "%ld"
#define INTBITSIZE 64
#endif

typedef uintptr_t		word;		/* Anonymous 4 byte object */
typedef YAP_Term	        Word;		/* Anonymous 4 byte object */
typedef YAP_Atom		Atom;
//move this to SWI

typedef uintptr_t	PL_atomic_t;	/* same a word */


#ifdef __SWI_PROLOG__

/* just to make clear how it would look in SWI */

#define INIT_DEF(Type, Name, Size) \
  static void init_ ## Name (void) {}		\
  static const Type Name[] {

#define ADD_DEF2(Atom, Type) \
  { Atom, Type },
#define ADD_DEF5(Atom, Type, Reverse, Arity, Ctx) \
  { Atom, Type, Reverse, Arity, Ctx },
			     \
#define END_DEFS(Atom, F) \
  { Atom, F }			 
}

#endif

#define INIT_DEF(Type, Name, Size) \
  static Type Name[Size];	     \
  static void init_ ## Name (void) { \
  int i = 0;

#define ADD_DEF2(Atom, Type) \
  char_types[i].name = Atom; \
  char_types[i].test = Type; \
  i++;
#define ADD_DEF5(Atom, Type, Reverse, Arity, Ctx) \
  char_types[i].name = Atom; \
  char_types[i].test = Type; \
  char_types[i].reverse = Reverse; \
  char_types[i].arity = Arity; \
  char_types[i].ctx_type = Ctx; \
  i++;
#define END_DEFS(Atom, F) \
  char_types[i].name = Atom; \
  char_types[i].test = F; \
}

#define ADD_ENCODING(Atom, Type) \
  encoding_names[i].code = Atom;	 \
  encoding_names[i].name = Type; \
  i++;
#define END_ENCODINGS(Atom, F) \
  encoding_names[i].code = Atom; \
  encoding_names[i].name = F; \
}

#define ADD_OPEN4_OPT(Atom, Type) \
  open4_options[i].name = Atom;	 \
  open4_options[i].type = Type; \
  i++;
#define END_OPEN4_DEFS(Atom, F) \
  open4_options[i].name = Atom; \
  open4_options[i].type = F; \
}

#define ADD_CLOSE2_OPT(Atom, Type) \
  close2_options[i].name = Atom;	 \
  close2_options[i].type = Type; \
  i++;
#define END_CLOSE2_DEFS(Atom, F) \
  close2_options[i].name = Atom; \
  close2_options[i].type = F; \
}

#define ADD_SPROP(F1, F2) \
  sprop_list[i].functor = F1;	 \
  sprop_list[i].function = F2; \
  i++;
#define END_SPROP_DEFS(F1, F2) \
  sprop_list[i].functor = F1;	 \
  sprop_list[i].function = F2; \
}

#define ADD_STDSTREAM(Atom) \
  standardStreams[i] = Atom;	 \
  i++;
#define END_STDSTREAMS(Atom) \
  standardStreams[i] = Atom; \
}


#define MK_ATOM(X) ((atom_t)YAP_LookupAtom(X))
#define MKFUNCTOR(X,Y) ((functor_t)YAP_MkFunctor((YAP_Atom)(X),Y))

/*** memory allocation stuff: SWI wraps around malloc  */

#define allocHeap(X) YAP_AllocSpaceFromYap(X)

#define freeHeap(X,Size) YAP_FreeSpaceFromYap(X)


#define stopItimer()

/* TBD */

extern atom_t codeToAtom(int chrcode);

static inline word
INIT_SEQ_CODES(size_t n)
{
  return (word)YAP_OpenList(n);
}

static inline word
EXTEND_SEQ_CODES(word gstore, int c) {
  return (word)YAP_ExtendList((YAP_Term)gstore, YAP_MkIntTerm(c));
}

static inline word
EXTEND_SEQ_ATOMS(word gstore, int c) {
  return (word)YAP_ExtendList((YAP_Term)gstore, codeToAtom(c));
}

static inline int 
CLOSE_SEQ_OF_CODES(word gstore, word lp, word arg2, word arg3, term_t l) {
  if (arg3 == (word)ATOM_nil) {
    if (!YAP_CloseList((YAP_Term)gstore, YAP_TermNil()))
      return FALSE;
  } else {
    if (!YAP_CloseList((YAP_Term)gstore, YAP_GetFromSlot(arg2)))
      return FALSE;
  }
  return YAP_Unify(YAP_GetFromSlot(arg3), lp);
}

static inline Word
valHandle(term_t tt)
{
  return (word)YAP_GetFromSlot(tt);
}
#define arityFunctor(f) YAP_ArityOfFunctor((YAP_Functor)f)

#define stringAtom(w)	YAP_AtomName((YAP_Atom)(w))
#define isInteger(A) (YAP_IsIntTerm((A)) && YAP_IsBigNumTerm((A)))
#define isString(A) FALSE
#define isAtom(A) YAP_IsAtomTerm((A))
#define isList(A) YAP_IsPairTerm((A))
#define isNil(A) ((A) == YAP_TermNil())
#define isReal(A) YAP_IsFloatTerm((A))
#define isFloat(A) YAP_IsFloatTerm((A))
#define isVar(A) YAP_IsVarTerm((A))
#define varName(l, buf) buf
#define valReal(w) YAP_FloatOfTerm((w))
#define valFloat(w) YAP_FloatOfTerm((w))
#define AtomLength(w) YAP_AtomNameLength(w)
#define atomValue(atom) ((YAP_Atom)atom)
#define argTermP(w,i) ((Word)((YAP_ArgsOfTerm(w)+(i))))
#define deRef(t) (t = YAP_Deref(t))
#define canBind(t) FALSE

#define clearNumber(n)

#endif /* __YAP_PROLOG__ */

#if IN_PL_OS_C
static int
stripostfix(const char *s, const char *e)
{ size_t ls = strlen(s);
  size_t le = strlen(e);

  if ( ls >= le )
    return strcasecmp(&s[ls-le], e) == 0;

  return FALSE;
} 
#endif

#define ERR_NO_ERROR		0
#define ERR_INSTANTIATION	1	/* void */
#define ERR_TYPE		2	/* atom_t expected, term_t value */
#define ERR_DOMAIN		3	/* atom_t domain, term_t value */
#define ERR_REPRESENTATION	4	/* atom_t what */
#define ERR_MODIFY_STATIC_PROC	5	/* predicate_t proc */
#define ERR_EVALUATION		6	/* atom_t what */
#define ERR_AR_TYPE		7	/* atom_t expected, Number value */
#define ERR_NOT_EVALUABLE	8	/* functor_t func */
#define ERR_DIV_BY_ZERO		9	/* void */
#define ERR_FAILED	       10	/* predicate_t proc */
#define ERR_FILE_OPERATION     11	/* atom_t action, atom_t type, term_t */
#define ERR_PERMISSION	       12	/* atom_t type, atom_t op, term_t obj*/
#define ERR_NOT_IMPLEMENTED 13	/* const char *what */
#define ERR_EXISTENCE	       14	/* atom_t type, term_t obj */
#define ERR_STREAM_OP	       15	/* atom_t action, term_t obj */
#define ERR_RESOURCE	       16	/* atom_t resource */
#define ERR_NOMEM	       17	/* void */
#define ERR_SYSCALL	       18	/* void */
#define ERR_SHELL_FAILED       19	/* term_t command */
#define ERR_SHELL_SIGNALLED    20	/* term_t command, int signal */
#define ERR_AR_UNDEF	       21	/* void */
#define ERR_AR_OVERFLOW	       22	/* void */
#define ERR_AR_UNDERFLOW       23	/* void */
#define ERR_UNDEFINED_PROC     24	/* Definition def */
#define ERR_SIGNALLED	       25	/* int sig, char *name */
#define ERR_CLOSED_STREAM      26	/* IOSTREAM * */
#define ERR_BUSY	       27	/* mutexes */
#define ERR_PERMISSION_PROC    28	/* op, type, Definition */
#define ERR_DDE_OP	       29	/* op, error */
#define ERR_SYNTAX	       30	/* what */
#define ERR_SHARED_OBJECT_OP   31	/* op, error */
#define ERR_TIMEOUT	       32	/* op, object */
#define ERR_NOT_IMPLEMENTED_PROC 33	/* name, arity */
#define ERR_FORMAT	       34	/* message */
#define ERR_FORMAT_ARG	       35	/* seq, term */
#define ERR_OCCURS_CHECK       36	/* Word, Word */
#define ERR_CHARS_TYPE	       37	/* char *, term */
#define ERR_MUST_BE_VAR	       38	/* int argn, term_t term */

#endif /* PL_YAP_H */
