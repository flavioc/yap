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
* File:		write.c							 *
* Last rev:								 *
* mods:									 *
* comments:	Writing a Prolog Term					 *
*									 *
*************************************************************************/
#ifdef SCCS
static char     SccsId[] = "%W% %G%";
#endif

#include <stdlib.h>
#include "Yap.h"
#include "Yatom.h"
#include "YapHeap.h"
#include "yapio.h"
#if COROUTINING
#include "attvar.h"
#endif

#if HAVE_STRING_H
#include <string.h>
#endif
#if HAVE_CTYPE_H
#include <ctype.h>
#endif

/* describe the type of the previous term to have been written */
typedef enum {
  separator,	/* the previous term was a separator like ',', ')', ... */
  alphanum,	/* the previous term was an atom or number */
  symbol        /* the previous term was a symbol like +, -, *, .... */
} wtype;

static wtype lastw;

typedef  int      (*wrf) (int, wchar_t);

typedef struct union_slots {
  long old;
  long ptr;
} uslots;

typedef struct union_direct {
  Term old;
  CELL *ptr;
} udirect;


typedef  struct  rewind_term {
  struct rewind_term *parent;
  union {
    struct union_slots s;
    struct union_direct d;
  } u;
} rwts;

typedef struct write_globs {
  wrf      writewch;
  int      Quote_illegal, Ignore_ops, Handle_vars, Use_portray;
  int      keep_terms;
  int      Write_Loops;
  UInt     MaxDepth, MaxArgs;
} wglbs;

STATIC_PROTO(void wrputn, (Int, wrf));
STATIC_PROTO(void wrputs, (char *, wrf));
STATIC_PROTO(void wrputf, (Float, wrf));
STATIC_PROTO(void wrputref, (CODEADDR, int, wrf));
STATIC_PROTO(int legalAtom, (unsigned char *));
STATIC_PROTO(int LeftOpToProtect, (Atom, int));
STATIC_PROTO(int RightOpToProtect, (Atom, int));
STATIC_PROTO(wtype AtomIsSymbols, (unsigned char *));
STATIC_PROTO(void putAtom, (Atom, int, wrf));
STATIC_PROTO(void writeTerm, (Term, int, int, int, struct write_globs *, struct rewind_term *));

#define wrputc(X,WF)	((*WF)(Yap_c_output_stream,X))	/* writes a character */

static void 
wrputn(Int n, wrf writewch)	/* writes an integer	 */
	                  
{
  char s[256], *s1=s; /* that should be enough for most integers */
  if (n < 0) {
    if (lastw == symbol)
      wrputc(' ', writewch);  
  } else {
    if (lastw == alphanum)
      wrputc(' ', writewch);
  }
#if HAVE_SNPRINTF
#if SHORT_INTS
  snprintf(s, 256, "%ld", n);
#else
  snprintf(s, 256, "%d", n);
#endif
#else
#if SHORT_INTS
  sprintf(s, "%ld", n);
#else
  sprintf(s, "%d", n);
#endif
#endif
  while (*s1)
    wrputc(*s1++, writewch);
  lastw = alphanum;
}

static void 
wrputs(char *s, wrf writewch)		/* writes a string	 */
{
  while (*s) {
    wrputc((unsigned char)(*s++), writewch);
  }
}

static void 
wrputws(wchar_t *s, wrf writewch)		/* writes a string	 */
{
  while (*s)
    wrputc(*s++, writewch);
}

static void 
wrputf(Float f, wrf writewch)		/* writes a float	 */
	                  
{
  char            s[256], *pt = s, ch;

  if (f < 0) {
    if (lastw == symbol)
      wrputc(' ', writewch);  
  } else {
    if (lastw == alphanum)
      wrputc(' ', writewch);
  }
  lastw = alphanum;
  //  sprintf(s, "%.15g", f);
  sprintf(s, RepAtom(AtomFloatFormat)->StrOfAE, f);
  while (*pt == ' ')
    pt++;
  if (*pt == 'i' || *pt == 'n')  /* inf or nan */ {
    wrputc('(', writewch);    
    wrputc('+', writewch);    
    wrputs(pt, writewch);
    wrputc(')', writewch);    
  } else {    
    wrputs(pt, writewch);
  }
  if (*pt == '-') pt++;
  while ((ch = *pt) != '\0') {
    if (ch < '0' || ch > '9')
      return;
    pt++;
  }
  wrputs(".0", writewch);    
}

static void 
wrputref(CODEADDR ref, int Quote_illegal, wrf writewch)			/* writes a data base reference */
	                    
{
  char            s[256];

  putAtom(AtomDBref, Quote_illegal, writewch);
#if defined(__linux__) || defined(__APPLE__)
  sprintf(s, "(%p,0)", ref);
#else
  sprintf(s, "(0x%p,0)", ref);
#endif
  wrputs(s, writewch);
  lastw = alphanum;
}

static int 
legalAtom(unsigned char *s)			/* Is this a legal atom ? */
	                  
{
  wchar_t ch = *s;

  if (ch == '\0')
    return(FALSE);
  if (Yap_chtype[ch] != LC) {
    if (ch == '[')
      return (*++s == ']' && !(*++s));
    else if (ch == '{')
      return (*++s == '}' && !(*++s));
    else if (Yap_chtype[ch] == SL)
      return (!*++s);
    else if ((ch == ',' || ch == '.') && !s[1])
      return FALSE;
    else
      while (ch) {
	if (Yap_chtype[ch] != SY)
	  return FALSE;
	ch = *++s;
      }
    return TRUE;
  } else
    while ((ch = *++s) != 0)
      if (Yap_chtype[ch] > NU)
	return FALSE;
  return (TRUE);
}

static int LeftOpToProtect(Atom at, int p)
{
  return Yap_IsOpMaxPrio(at) > p;
}

static int RightOpToProtect(Atom at, int p)
{
  return Yap_IsOpMaxPrio(at) > p;
}

static wtype 
AtomIsSymbols(unsigned char *s)		/* Is this atom just formed by symbols ? */
{
  int ch;
  if (Yap_chtype[(int)s[0]] == SL && s[1] == '\0')
    return(separator);
  while ((ch = *s++) != '\0') {
    if (Yap_chtype[ch] != SY)
      return(alphanum);
  }
  return(symbol);
}

static void
write_quoted(int ch, int quote, wrf writewch)
{
  if (yap_flags[CHARACTER_ESCAPE_FLAG] == CPROLOG_CHARACTER_ESCAPES) {
    wrputc(ch, writewch);
    if (ch == '\'')
      wrputc('\'', writewch);	/* be careful about quotes */
    return;
  }
  if (!(ch < 0xff  && chtype(ch) == BS) && ch != '\'' && ch != '\\') {
    wrputc(ch, writewch);
  } else {
    switch (ch) {
    case '\\':
    case '\'':
      wrputc('\\', writewch);	
      wrputc(ch, writewch);	
      break;
    case 7:
      wrputc('\\', writewch);	
      wrputc('a', writewch);	
      break;
    case '\b':
      wrputc('\\', writewch);
      wrputc('b', writewch);	
      break;
    case '\t':
      wrputc('\\', writewch);
      wrputc('t', writewch);	
      break;
    case ' ':
    case 160:
      wrputc(' ', writewch);
      break;
    case '\n':
      wrputc('\\', writewch);
      wrputc('n', writewch);	
      break;
    case 11:
      wrputc('\\', writewch);
      wrputc('v', writewch);	
      break;
    case '\r':
      wrputc('\\', writewch);
      wrputc('r', writewch);	
      break;
    case '\f':
      wrputc('\\', writewch);
      wrputc('f', writewch);	
      break;
    default:
      if ( ch <= 0xff ) {
	char esc[8];
	
	if (yap_flags[CHARACTER_ESCAPE_FLAG] == SICSTUS_CHARACTER_ESCAPES) {
	  sprintf(esc, "\\%03o", ch);
	} else {
	  /* last backslash in ISO mode */
	  sprintf(esc, "\\%03o\\", ch);
	}
	wrputs(esc, writewch);
      }
    }
  }
}


static void 
putAtom(Atom atom, int Quote_illegal, wrf writewch)			/* writes an atom	 */
	                     
{
  unsigned char           *s = (unsigned char *)RepAtom(atom)->StrOfAE;
  wtype          atom_or_symbol = AtomIsSymbols(s);

  /* #define CRYPT_FOR_STEVE 1*/
#ifdef CRYPT_FOR_STEVE
  if (Yap_GetValue(AtomCryptAtoms) != TermNil && Yap_GetAProp(atom, OpProperty) == NIL) {
    char s[16];
    sprintf(s,"x%x", (CELL)s);
    wrputs(s, writewch);
    return;
  }
#endif
  if (IsWideAtom(atom)) {
    wchar_t *ws = (wchar_t *)s;

    if (Quote_illegal) {
      wrputc('\'', writewch);
      while (*ws) {
	wchar_t ch = *ws++;
	write_quoted(ch, '\'', writewch);
      }
      wrputc('\'', writewch);
    } else {
      wrputws(ws, writewch);
    }
    return;
  }
  if (lastw == atom_or_symbol && atom_or_symbol != separator /* solo */)
    wrputc(' ', writewch);
  lastw = atom_or_symbol;
  if (!legalAtom(s) && Quote_illegal) {
    wrputc('\'', writewch);
    while (*s) {
      wchar_t ch = *s++;
      write_quoted(ch, '\'', writewch);
    }
    wrputc('\'', writewch);
  } else {
    wrputs((char *)s, writewch);
  }
}

static int 
IsStringTerm(Term string)	/* checks whether this is a string */
{
  if (IsVarTerm(string))
    return FALSE;
  do {
    Term hd;
    int ch;

    if (!IsPairTerm(string)) return(FALSE);
    hd = HeadOfTerm(string);
    if (IsVarTerm(hd)) return(FALSE);
    if (!IsIntTerm(hd)) return(FALSE);
    ch = IntOfTerm(HeadOfTerm(string));
    if ((ch < ' ' || ch > MAX_ISO_LATIN1) && ch != '\n' && ch != '\t')
      return(FALSE);
    string = TailOfTerm(string);
    if (IsVarTerm(string)) return(FALSE);
  } while (string != TermNil);
  return(TRUE);
}

static void 
putString(Term string, wrf writewch)		/* writes a string	 */
	                     
{
  wrputc('"', writewch);
  while (string != TermNil) {
    int ch = IntOfTerm(HeadOfTerm(string));
    write_quoted(ch, '"', writewch);
    string = TailOfTerm(string);
  }
  wrputc('"', writewch);
  lastw = alphanum;
}

static void 
putUnquotedString(Term string, wrf writewch)	/* writes a string	 */
	                     
{
  while (string != TermNil) {
    int ch = IntOfTerm(HeadOfTerm(string));
    wrputc(ch, writewch);
    string = TailOfTerm(string);
  }
  lastw = alphanum;
}


static void
write_var(CELL *t,  struct write_globs *wglb, struct rewind_term *rwt) 
{
  if (lastw == alphanum) {
    wrputc(' ', wglb->writewch);
  }
  wrputc('_', wglb->writewch);
  /* make sure we don't get no creepy spaces where they shouldn't be */
  lastw = separator;
  if (IsAttVar(t)) {
    Int vcount = (t-H0);
#if COROUTINING
#if DEBUG
    if (Yap_Portray_delays) {
      exts ext = ExtFromCell(t);

      Yap_Portray_delays = FALSE;
      if (ext == attvars_ext) {
	attvar_record *attv = RepAttVar(t);
	long sl = 0;
	Term l = attv->Atts;

	wrputs("$AT(",wglb->writewch);
	write_var(t, wglb, rwt);
	wrputc(',', wglb->writewch);      
	if (wglb->keep_terms) {
	  /* garbage collection may be called */
	  sl = Yap_InitSlot((CELL)attv);
	}
	writeTerm((Term)&(attv->Value), 999, 1, FALSE, wglb, rwt);
	wrputc(',', wglb->writewch);
	writeTerm(l, 999, 1, FALSE, wglb, rwt);
	if (wglb->keep_terms) {
	  attv = (attvar_record *)Yap_GetFromSlot(sl);
	  Yap_RecoverSlots(1);
	}
	wrputc(')', wglb->writewch);
      }
      Yap_Portray_delays = TRUE;
      return;
    }
#endif
    wrputc('D', wglb->writewch);
    wrputn(vcount,wglb->writewch);
#endif
  } else {
    wrputn(((Int) (t- H0)),wglb->writewch);
  }
}

static Term
from_pointer(CELL *ptr, struct rewind_term *rwt, struct write_globs *wglb)
{
  Term t;

  while (IsVarTerm(*ptr) && !IsUnboundVar(ptr))
    ptr = (CELL *)*ptr;
  t = *ptr;
  if (!IsVarTerm(t)) {
    if (wglb->keep_terms) {
      rwt->u.s.old = Yap_InitSlot(t);
      rwt->u.s.ptr = Yap_InitSlot((CELL)ptr);
    } else {
      rwt->u.d.old = t;
      rwt->u.d.ptr = ptr;
    }
    *ptr = TermFoundVar;
  }
  return t;
}

static void
restore_from_write(struct rewind_term *rwt, struct write_globs *wglb)
{
  Term t;
  if (rwt->u.s.ptr) {
    CELL *ptr;
    if (wglb->keep_terms) {
      ptr = (CELL *)Yap_GetPtrFromSlot(rwt->u.s.ptr);
      t = Yap_GetPtrFromSlot(rwt->u.s.old);
      Yap_RecoverSlots(2);
    } else {
      ptr = rwt->u.d.ptr;
      t = rwt->u.d.old;
    }
    *ptr = t;
  }
  rwt->u.s.ptr = 0;
}

static void
write_list(Term t, int direction, int depth, struct write_globs *wglb, struct rewind_term *rwt)
{
  Term ti;
  struct rewind_term nrwt;
  nrwt.parent = rwt;
  nrwt.u.s.ptr = 0;

  while (1) {
    long            sl= 0;
    int ndirection;
    int do_jump;

    if (wglb->keep_terms) {
      /* garbage collection may be called */
      sl = Yap_InitSlot(t);
    }
    writeTerm(from_pointer(RepPair(t), &nrwt, wglb), 999, depth+1, FALSE, wglb, &nrwt);
    restore_from_write(&nrwt, wglb);
    if (wglb->keep_terms) {
      t = Yap_GetFromSlot(sl);
      Yap_RecoverSlots(1);
    }
    ti = TailOfTerm(t);
    if (IsVarTerm(ti))
      break;
    if (!IsPairTerm(ti))
      break;
    ndirection = RepPair(ti)-RepPair(t);
    /* make sure we're not trapped in loops */
    if (ndirection > 0) {
      do_jump = (direction < 0);
    } else if (ndirection == 0) {
      wrputc(',', wglb->writewch);
      putAtom(AtomFoundVar, wglb->Quote_illegal, wglb->writewch);
      lastw = separator;
      return;
    } else {
      do_jump = (direction > 0);
    }
    if (wglb->MaxDepth != 0 && depth > wglb->MaxDepth) {
      wrputc('|', wglb->writewch);
      putAtom(Atom3Dots, wglb->Quote_illegal, wglb->writewch);
      return;
    }
    wrputc(',', wglb->writewch);
    lastw = separator;
    direction = ndirection;
    depth++;
    if (do_jump)
      break;
    t = ti;
  }
  if (IsPairTerm(ti)) {
    write_list(from_pointer(RepPair(t)+1, &nrwt, wglb), direction, depth, wglb, &nrwt);
    restore_from_write(&nrwt, wglb);
  } else if (ti != MkAtomTerm(AtomNil)) {
    wrputc('|', wglb->writewch);
    lastw = separator;
    writeTerm(from_pointer(RepPair(t)+1, &nrwt, wglb), 999, depth, FALSE, wglb, &nrwt);
    restore_from_write(&nrwt, wglb);
  }
 }

static void 
writeTerm(Term t, int p, int depth, int rinfixarg, struct write_globs *wglb, struct rewind_term *rwt)
/* term to write			 */
/* context priority			 */
                     
{
  struct rewind_term nrwt;
  nrwt.parent = rwt;
  nrwt.u.s.ptr = 0;

  if (wglb->MaxDepth != 0 && depth > wglb->MaxDepth) {
    putAtom(Atom3Dots, wglb->Quote_illegal, wglb->writewch);
    return;
  }
  if (EX != 0)
    return;
  t = Deref(t);
  if (IsVarTerm(t)) {
    write_var((CELL *)t, wglb, &nrwt);
  } else if (IsIntTerm(t)) {
    wrputn((Int) IntOfTerm(t),wglb->writewch);
  } else if (IsAtomTerm(t)) {
    putAtom(AtomOfTerm(t), wglb->Quote_illegal, wglb->writewch);
  } else if (IsPairTerm(t)) {
    if (wglb->Use_portray) {
      Term targs[1];
      Term old_EX = 0L;
      long sl = 0;

      targs[0] = t;
      Yap_PutValue(AtomPortray, MkAtomTerm(AtomNil));
      if (EX != 0L) old_EX = EX;
      sl = Yap_InitSlot(t);      
      Yap_execute_goal(Yap_MkApplTerm(FunctorPortray, 1, targs), 0, 1);
      t = Yap_GetFromSlot(sl);
      Yap_RecoverSlots(1);
      if (old_EX != 0L) EX = old_EX;
      if (Yap_GetValue(AtomPortray) == MkAtomTerm(AtomTrue))
	return;
    }
    if (yap_flags[WRITE_QUOTED_STRING_FLAG] && IsStringTerm(t)) {
      putString(t, wglb->writewch);
    } else {
      Term ls  = t;
      wrputc('[', wglb->writewch);
      lastw = separator;
      write_list(from_pointer(&ls, &nrwt, wglb), 0, depth, wglb, &nrwt);
      restore_from_write(&nrwt, wglb);
      wrputc(']', wglb->writewch);
      lastw = separator;
    }
  } else {		/* compound term */
    Functor         functor = FunctorOfTerm(t);
    int             Arity;
    Atom            atom;
    int             op, lp, rp;

    if (IsExtensionFunctor(functor)) {
      switch((CELL)functor) {
      case (CELL)FunctorDouble:
	wrputf(FloatOfTerm(t),wglb->writewch);
	return;
      case (CELL)FunctorAttVar:	
	write_var(RepAppl(t)+1, wglb, &nrwt);
	return;
      case (CELL)FunctorDBRef:
	wrputref(RefOfTerm(t), wglb->Quote_illegal, wglb->writewch);
	return;
      case (CELL)FunctorLongInt:
	wrputn(LongIntOfTerm(t),wglb->writewch);
	return;
      case (CELL)FunctorBigInt:
#ifdef USE_GMP
	{
	  MP_INT *big = Yap_BigIntOfTerm(t);
	  char *s;
	  s = (char *) Yap_PreAllocCodeSpace();
	  while (s+3+mpz_sizeinbase(big, 10) >= (char *)AuxSp) {
#if USE_SYSTEM_MALLOC
	    /* may require stack expansion */
	    if (!Yap_ExpandPreAllocCodeSpace(3+mpz_sizeinbase(big, 10), NULL, TRUE)) {
	      s = NULL;
	      break;
	    }
	    s = (char *) Yap_PreAllocCodeSpace();
#else
	    s = NULL;
#endif
	  }
	  if (!s) {
	    s = (char *)TR;
	    while (s+3+mpz_sizeinbase(big, 10) >= Yap_TrailTop) {
	      if (!Yap_growtrail((3+mpz_sizeinbase(big, 10))/sizeof(CELL), FALSE)) {
		s = NULL;
		break;
	      }
	      s = (char *)TR;
	    }
	  }
	  if (!s) {
	    s = (char *)H;
	    if (s+3+mpz_sizeinbase(big, 10) >= (char *)ASP) {
	      Yap_Error(OUT_OF_STACK_ERROR,TermNil,"not enough space to write bignum: it requires %d bytes", 3+mpz_sizeinbase(big, 10));
	      s = NULL;
	    }
	  }
	  if (mpz_sgn(big) < 0) {
	    if (lastw == symbol)
	      wrputc(' ', wglb->writewch);  
	  } else {
	    if (lastw == alphanum)
	      wrputc(' ', wglb->writewch);
	  }
	  if (!s) {
	    s = mpz_get_str(NULL, 10, big);
	    if (!s)
	      return;
	    wrputs(s,wglb->writewch);
	    free(s);
	  } else {
	    mpz_get_str(s, 10, big);
	    wrputs(s,wglb->writewch);
	  }
	}
#else
	{
	  wrputs("0",wglb->writewch);
	}
#endif
	return;
      }
    }
    Arity = ArityOfFunctor(functor);
    atom = NameOfFunctor(functor);
#ifdef SFUNC
    if (Arity == SFArity) {
      int             argno = 1;
      CELL           *p = ArgsOfSFTerm(t);
      putAtom(atom, wglb->Quote_illegal, wglb->writewch);
      wrputc('(', wglb->writewch);
      lastw = separator;
      while (*p) {
	long sl = 0;

	while (argno < *p) {
	  wrputc('_', wglb->writewch), wrputc(',', wglb->writewch);
	  ++argno;
	}
	*p++;
	lastw = separator;
	/* cannot use the term directly with the SBA */
	if (wglb->keep_terms) {
	  /* garbage collection may be called */
	  sl = Yap_InitSlot((CELL)p);
	}
	writeTerm(from_pointer(p++, &nrwt, wglb), 999, depth + 1, FALSE, wglb, &nrwt);
	restore_from_write(&nrwt, wglb);
	if (wglb->keep_terms) {
	  /* garbage collection may be called */
	  p = (CELL *)Yap_GetFromSlot(sl);
	  Yap_RecoverSlots(1);
	}
	if (*p)
	  wrputc(',', wglb->writewch);
	argno++;
      }
      wrputc(')', wglb->writewch);
      lastw = separator;
      return;
    }
#endif
    if (wglb->Use_portray) {
      Term targs[1];
      Term old_EX = 0L;
      long sl = 0;

      targs[0] = t;
      Yap_PutValue(AtomPortray, MkAtomTerm(AtomNil));
      if (EX != 0L) old_EX = EX;
      sl = Yap_InitSlot(t);      
      Yap_execute_goal(Yap_MkApplTerm(FunctorPortray, 1, targs),0, 1);
      t = Yap_GetFromSlot(sl);
      Yap_RecoverSlots(1);
      if (old_EX != 0L) EX = old_EX;
      if (Yap_GetValue(AtomPortray) == MkAtomTerm(AtomTrue) || EX != 0L)
	return;
    }
    if (!wglb->Ignore_ops &&
	Arity == 1 &&  Yap_IsPrefixOp(atom, &op, &rp)
#ifdef DO_NOT_WRITE_PLUS_AND_MINUS_AS_PREFIX
	&&
	/* never write '+' and '-' as infix
	   operators */
	( (RepAtom(atom)->StrOfAE[0] != '+' &&
	   RepAtom(atom)->StrOfAE[0] != '-') ||
	  RepAtom(atom)->StrOfAE[1] )
#endif /* DO_NOT_WRITE_PLUS_AND_MINUS_AS_PREFIX */
	) {
      Term  tright = ArgOfTerm(1, t);
      int            bracket_right =
	!IsVarTerm(tright) && IsAtomTerm(tright) &&
	RightOpToProtect(AtomOfTerm(tright), rp);
      if (op > p) {
	/* avoid stuff such as \+ (a,b) being written as \+(a,b) */
	if (lastw != separator && !rinfixarg)
	  wrputc(' ', wglb->writewch);
	wrputc('(', wglb->writewch);
	lastw = separator;
      }
      putAtom(atom, wglb->Quote_illegal, wglb->writewch);
      if (bracket_right) {
	wrputc('(', wglb->writewch);
	lastw = separator;
      }
      writeTerm(from_pointer(RepAppl(t)+1, &nrwt, wglb), rp, depth + 1, FALSE, wglb, &nrwt);
      restore_from_write(&nrwt, wglb);
      if (bracket_right) {
	wrputc(')', wglb->writewch);
	lastw = separator;
      }
      if (op > p) {
	wrputc(')', wglb->writewch);
	lastw = separator;
      }
    } else if (!wglb->Ignore_ops &&
	       Arity == 1 &&
	       Yap_IsPosfixOp(atom, &op, &lp)) {
      Term  tleft = ArgOfTerm(1, t);
      long sl = 0;
      int            bracket_left =
	!IsVarTerm(tleft) && IsAtomTerm(tleft) &&
	LeftOpToProtect(AtomOfTerm(tleft), lp); 
      if (op > p) {
	/* avoid stuff such as \+ (a,b) being written as \+(a,b) */
	if (lastw != separator && !rinfixarg)
	  wrputc(' ', wglb->writewch);
	wrputc('(', wglb->writewch);
	lastw = separator;
      }
      if (bracket_left) {
	wrputc('(', wglb->writewch);
	lastw = separator;
      }
      if (wglb->keep_terms) {
	/* garbage collection may be called */
	sl = Yap_InitSlot(t);      
      }
      writeTerm(from_pointer(RepAppl(t)+1, &nrwt, wglb), lp, depth + 1, rinfixarg, wglb, &nrwt);
      restore_from_write(&nrwt, wglb);
      if (wglb->keep_terms) {
	/* garbage collection may be called */
	t = Yap_GetFromSlot(sl);
	Yap_RecoverSlots(1);
      }
      if (bracket_left) {
	wrputc(')', wglb->writewch);
	lastw = separator;
      }
      putAtom(atom, wglb->Quote_illegal, wglb->writewch);
      if (op > p) {
	wrputc(')', wglb->writewch);
	lastw = separator;
      }
    } else if (!wglb->Ignore_ops &&
	       Arity == 2  && Yap_IsInfixOp(atom, &op, &lp,
						 &rp) ) {
      Term  tleft = ArgOfTerm(1, t);
      Term  tright = ArgOfTerm(2, t);
      long sl = 0;
      int   bracket_left =
	!IsVarTerm(tleft) && IsAtomTerm(tleft) &&
	LeftOpToProtect(AtomOfTerm(tleft), lp);
      int   bracket_right =
	!IsVarTerm(tright) && IsAtomTerm(tright) &&
	RightOpToProtect(AtomOfTerm(tright), rp);

      if (op > p) {
	/* avoid stuff such as \+ (a,b) being written as \+(a,b) */
	if (lastw != separator && !rinfixarg)
	  wrputc(' ', wglb->writewch);
	wrputc('(', wglb->writewch);
	lastw = separator;
      }
      if (bracket_left) {
	wrputc('(', wglb->writewch);
	lastw = separator;
      }
      if (wglb->keep_terms) {
	/* garbage collection may be called */
	sl = Yap_InitSlot(t);      
      }
      writeTerm(from_pointer(RepAppl(t)+1, &nrwt, wglb), lp, depth + 1, rinfixarg, wglb, &nrwt);
      restore_from_write(&nrwt, wglb);
      if (wglb->keep_terms) {
	/* garbage collection may be called */
	t = Yap_GetFromSlot(sl);
	Yap_RecoverSlots(1);
      }
      if (bracket_left) {
	wrputc(')', wglb->writewch);
	lastw = separator;
      }
      /* avoid quoting commas */
      if (strcmp(RepAtom(atom)->StrOfAE,","))
	putAtom(atom, wglb->Quote_illegal, wglb->writewch);
      else {
	wrputc(',', wglb->writewch);
	lastw = separator;
      }
      if (bracket_right) {
	wrputc('(', wglb->writewch);
	lastw = separator;
      }
      writeTerm(from_pointer(RepAppl(t)+2, &nrwt, wglb), rp, depth + 1, TRUE, wglb, &nrwt);
      restore_from_write(&nrwt, wglb);
      if (bracket_right) {
	wrputc(')', wglb->writewch);
	lastw = separator;
      }
      if (op > p) {
	wrputc(')', wglb->writewch);
	lastw = separator;
      }
    } else if (wglb->Handle_vars && functor == FunctorVar) {
      Term ti = ArgOfTerm(1, t);
      if (lastw == alphanum) {
	wrputc(' ', wglb->writewch);
      }
      if (!IsVarTerm(ti) && (IsIntTerm(ti) || IsStringTerm(ti))) {
	if (IsIntTerm(ti)) {
	  Int k = IntOfTerm(ti);
	  if (k == -1)  {
	    wrputc('_', wglb->writewch);
	    lastw = alphanum;
	    return;
	  } else {
	    wrputc((k % 26) + 'A', wglb->writewch);
	    if (k >= 26) {
	      /* make sure we don't get confused about our context */
	      lastw = separator;
	      wrputn( k / 26 ,wglb->writewch);
	    } else
	      lastw = alphanum;
	  }
	} else {
	  putUnquotedString(ti, wglb->writewch);
	}
      } else {
	long sl = 0;

	wrputs("'$VAR'(",wglb->writewch);
	lastw = separator;
	if (wglb->keep_terms) {
	  /* garbage collection may be called */
	  sl = Yap_InitSlot(t);      
	}
	writeTerm(from_pointer(RepAppl(t)+1, &nrwt, wglb), 999, depth + 1, FALSE, wglb, &nrwt);
	restore_from_write(&nrwt, wglb);
	if (wglb->keep_terms) {
	  /* garbage collection may be called */
	  t = Yap_GetFromSlot(sl);
	  Yap_RecoverSlots(1);
	}
	wrputc(')', wglb->writewch);
	lastw = separator;
      }
    } else if (!wglb->Ignore_ops && functor == FunctorBraces) {
      wrputc('{', wglb->writewch);
      lastw = separator;
      writeTerm(from_pointer(RepAppl(t)+1, &nrwt, wglb), 1200, depth + 1, FALSE, wglb, &nrwt);
      restore_from_write(&nrwt, wglb);
      wrputc('}', wglb->writewch);
      lastw = separator;
    } else  if (atom == AtomArray) {
      long sl = 0;

      wrputc('{', wglb->writewch);
      lastw = separator;
      for (op = 1; op <= Arity; ++op) {
	if (op == wglb->MaxArgs) {
	  wrputc('.', wglb->writewch);
	  wrputc('.', wglb->writewch);
	  wrputc('.', wglb->writewch);
	  break;
	}
	if (wglb->keep_terms) {
	  /* garbage collection may be called */
	  sl = Yap_InitSlot(t);      
	}
	writeTerm(from_pointer(RepAppl(t)+op, &nrwt, wglb), 999, depth + 1, FALSE, wglb, &nrwt);
	restore_from_write(&nrwt, wglb);
	if (wglb->keep_terms) {
	  /* garbage collection may be called */
	  t = Yap_GetFromSlot(sl);
	  Yap_RecoverSlots(1);
	}
	if (op != Arity) {
	  wrputc(',', wglb->writewch);
	  lastw = separator;
	}
      }
      wrputc('}', wglb->writewch);
      lastw = separator;
    } else {
      putAtom(atom, wglb->Quote_illegal, wglb->writewch);
      lastw = separator;
      wrputc('(', wglb->writewch);
      for (op = 1; op <= Arity; ++op) {
	long sl = 0;

	if (op == wglb->MaxArgs) {
	  wrputc('.', wglb->writewch);
	  wrputc('.', wglb->writewch);
	  wrputc('.', wglb->writewch);
	  break;
	}
	if (wglb->keep_terms) {
	  /* garbage collection may be called */
	  sl = Yap_InitSlot(t);      
	}
	writeTerm(from_pointer(RepAppl(t)+op, &nrwt, wglb), 999, depth + 1, FALSE, wglb, &nrwt);
	restore_from_write(&nrwt, wglb);
	if (wglb->keep_terms) {
	  /* garbage collection may be called */
	  t = Yap_GetFromSlot(sl);
	  Yap_RecoverSlots(1);
	}
	if (op != Arity) {
	  wrputc(',', wglb->writewch);
	  lastw = separator;
	}
      }
      wrputc(')', wglb->writewch);
      lastw = separator;
    }
  }
}

void 
Yap_plwrite(Term t, int (*mywrite) (int, wchar_t), int flags, int priority)
     /* term to be written			 */
     /* consumer				 */
     /* write options			 */
{
  struct write_globs wglb;
  struct rewind_term rwt;
  rwt.parent = NULL;
  rwt.u.s.ptr = 0;

  wglb.writewch = mywrite;
  lastw = separator;
  wglb.Quote_illegal = flags & Quote_illegal_f;
  wglb.Handle_vars = flags & Handle_vars_f;
  wglb.Use_portray = flags & Use_portray_f;
  wglb.MaxDepth = max_depth;
  wglb.MaxArgs = max_write_args;
  /* notice: we must have ASP well set when using portray, otherwise
     we cannot make recursive Prolog calls */
  wglb.keep_terms = (flags & (Use_portray_f|To_heap_f)); 
  wglb.Ignore_ops = flags & Ignore_ops_f;
  /* protect slots for portray */
  writeTerm(from_pointer(&t, &rwt, &wglb), priority, 1, FALSE, &wglb, &rwt);
  restore_from_write(&rwt, &wglb);
}

