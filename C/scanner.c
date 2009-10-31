/*************************************************************************
*									 *
*	 YAP Prolog 							 *
*									 *
*	Yap Prolog was developed at NCCUP - Universidade do Porto	 *
*									 *
* Copyright L.Damas, V.S.Costa and Universidade do Porto 1985-2003	 *
*									 *
**************************************************************************
*									 *
* File:		%W% %G%						 *
* Last rev:	22-1-03							 *
* mods:									 *
* comments:	Prolog's scanner					 *
*									 *
*************************************************************************/

/*
 * Description: 
 *
 * This module produces a list of tokens for use by the parser. The calling
 * program should supply a routine int nextch(charpos) int *charpos; which,
 * when called should produce the next char or -1 if none availlable. The
 * scanner will stop producing tokens when it either finds an end of file
 * (-1) or a token consisting of just '.' followed by a blank or control
 * char. Scanner errors will be signalled by the scanner exiting with a non-
 * zero  ErrorMsg and ErrorPos. Note that, even in this case, the scanner
 * will try to find the end of the term. A function char
 * *AllocScannerMemory(nbytes) should be supplied for allocating (temporary)
 * space for strings and for the table of prolog variables occurring in the
 * term. 
 *
 */

#include "Yap.h"
#include "Yatom.h"
#include "YapHeap.h"
#include "yapio.h"
#include "alloc.h"
#include "eval.h"
#if _MSC_VER || defined(__MINGW32__) 
#if HAVE_FINITE==1
#undef HAVE_FINITE
#endif
#include <windows.h>
#endif
#include "iopreds.h"
#if HAVE_STRING_H
#include <string.h>
#endif

/* You just can't trust some machines */
#define my_isxdigit(C,SU,SL)	(chtype(C) == NU || (C >= 'A' &&	\
				 C <= (SU)) || (C >= 'a' && C <= (SL)))
#define my_isupper(C)	( C >= 'A' && C <= 'Z' )
#define my_islower(C)	( C >= 'a' && C <= 'z' )

STATIC_PROTO(int my_getch, (int (*) (int)));
STATIC_PROTO(Term float_send, (char *, int));
STATIC_PROTO(Term get_num, (int *, int *, int, int (*) (int), int (*) (int),char *,UInt,int));

/* token table with some help from Richard O'Keefe's PD scanner */
static char chtype0[NUMBER_OF_CHARS+1] =
{
EF,
/* nul soh stx etx eot enq ack bel  bs  ht  nl  vt  np  cr  so  si */
  BS, BS, BS, BS, BS, BS, BS, BS, BS, BS, BS, BS, BS, BS, BS, BS,

/* dle dc1 dc2 dc3 dc4 nak syn etb can  em sub esc  fs  gs  rs  us */
  BS, BS, BS, BS, BS, BS, BS, BS, BS, BS, BS, BS, BS, BS, BS, BS,

/* sp   !   "   #   $   %   &   '   (   )   *   +   ,   -   .   / */
  BS, SL, DC, SY, LC, CC, SY, QT, BK, BK, SY, SY, BK, SY, SY, SY,

/* 0   1   2   3   4   5   6   7   8   9   :   ;   <   =   >   ? */
  NU, NU, NU, NU, NU, NU, NU, NU, NU, NU, SY, SL, SY, SY, SY, SY,

/* @   A   B   C   D   E   F   G   H   I   J   K   L   M   N   O */
  SY, UC, UC, UC, UC, UC, UC, UC, UC, UC, UC, UC, UC, UC, UC, UC,

/* P   Q   R   S   T   U   V   W   X   Y   Z   [   \   ]   ^   _ */
  UC, UC, UC, UC, UC, UC, UC, UC, UC, UC, UC, BK, SY, BK, SY, UL,

/* `   a   b   c   d   e   f   g   h   i   j   k   l   m   n   o */
  SY, LC, LC, LC, LC, LC, LC, LC, LC, LC, LC, LC, LC, LC, LC, LC,

/* p   q   r   s   t   u   v   w   x   y   z   {   |   }   ~ del */
  LC, LC, LC, LC, LC, LC, LC, LC, LC, LC, LC, BK, BK, BK, SY, BS,

/* 128 129 130 131 132 133 134 135 136 137 138 139 140 141 142 143 */
  BS, BS,  BS, BS, BS, BS, BS, BS, BS, BS, BS, BS, BS, BS, BS, BS,

/* 144 145 �   147 148 149 150 151 152 153 154 155 156 157 158 159 */
   BS, BS, BS, BS, BS, BS, BS, BS, BS, BS, BS, BS, BS, BS, BS, BS,

/* �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   */
   BS, SY, SY, SY, SY, SY, SY, SY, SY, SY, LC, SY, SY, SY, SY, SY,

/* �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   */
   SY, SY, LC, LC, SY, SY, SY, SY, SY, LC, LC, SY, SY, SY, SY, SY,

/* �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �    */
   UC, UC, UC, UC, UC, UC, UC, UC, UC, UC, UC, UC, UC, UC, UC, UC,

/* �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �    */
#ifdef  vms
   UC, UC, UC, UC, UC, UC, UC, UC, UC, UC, UC, UC, UC, UC, UC, LC,
#else
   UC, UC, UC, UC, UC, UC, UC, SY, UC, UC, UC, UC, UC, UC, UC, LC,
#endif
/* �   �   �   �   �   �   �   �   �   �   �   �   �   �   �   �    */
   LC, LC, LC, LC, LC, LC, LC, LC, LC, LC, LC, LC, LC, LC, LC, LC,

/* �   �   �   �   �   �   �   �   �   �   �   �   �   cannot write the last three because of lcc    */
#ifdef  vms
   LC, LC, LC, LC, LC, LC, LC, LC, LC, LC, LC, LC, LC, LC, LC, LC
#else
   LC, LC, LC, LC, LC, LC, LC, SY, LC, LC, LC, LC, LC, LC, LC, LC
#endif
};

char *Yap_chtype = chtype0+1;

/* in case there is an overflow */
typedef struct scanner_extra_alloc {
  struct scanner_extra_alloc *next;
  void *filler;
} ScannerExtraBlock;

static char *
AllocScannerMemory(unsigned int size)
{
  char *AuxSpScan;

  AuxSpScan = ScannerStack;
  size = AdjustSize(size);
  if (ScannerExtraBlocks) {
    struct scanner_extra_alloc *ptr;

    if (!(ptr = (struct scanner_extra_alloc *)malloc(size+sizeof(ScannerExtraBlock)))) {
      return NULL;
    }
    ptr->next = ScannerExtraBlocks;
    ScannerExtraBlocks = ptr;
    return (char *)(ptr+1);
  } else if (Yap_TrailTop <= AuxSpScan+size) {
    UInt alloc_size = sizeof(CELL) * 16 * 1024L;
 
    if (size > alloc_size)
      alloc_size = size;
    if(!Yap_growtrail(alloc_size, TRUE)) {
      struct scanner_extra_alloc *ptr;

      if (!(ptr = (struct scanner_extra_alloc *)malloc(size+sizeof(ScannerExtraBlock)))) {
	return NULL;
      }
      ptr->next = ScannerExtraBlocks;
      ScannerExtraBlocks = ptr;
      return (char *)(ptr+1);
    }
  }
  ScannerStack = AuxSpScan+size;
  return AuxSpScan;
}

static void
PopScannerMemory(char *block, unsigned int size)
{
  if (block == ScannerStack-size) {
    ScannerStack -= size;
  } else if (block == (char *)(ScannerExtraBlocks+1)) {
    struct scanner_extra_alloc *ptr = ScannerExtraBlocks;

    ScannerExtraBlocks = ptr->next;
    free(ptr);
  }
}

char *
Yap_AllocScannerMemory(unsigned int size)
{
  /* I assume memory has been initialised */
  return AllocScannerMemory(size);
}

inline static int
my_getch(int (*Nextch) (int))
{
  int ch = (*Nextch) (Yap_c_input_stream); 
#ifdef DEBUG
  if (Yap_Option[1])
    fprintf(Yap_stderr, "[getch %c]", ch);
#endif
  return(ch);
}

extern double atof(const char *);

static Term
float_send(char *s, int sign)
{
  Float f = (Float)atof(s);
#if HAVE_FINITE
  if (yap_flags[LANGUAGE_MODE_FLAG] == 1) { /* iso */
    if (!finite(f)) {
      Yap_ErrorMessage = "Float overflow while scanning";
      return(MkEvalFl(0.0));
    }
  }
#endif
  return (MkEvalFl(f*sign));
}

/* we have an overflow at s */
static Term
read_int_overflow(const char *s, Int base, Int val, int sign)
{
#ifdef USE_GMP
  /* try to scan it as a bignum */
  mpz_t new;
  Term t;

  mpz_init_set_str (new, s, base);
  if (sign < 0)
    mpz_neg(new, new);
  t = Yap_MkBigIntTerm(new);
  mpz_clear(new);
  return t;
#else
  /* try to scan it as a float */
  return MkIntegerTerm(val);
#endif    
}

static wchar_t
read_quoted_char(int *scan_nextp, int inp_stream, int (*QuotedNxtch)(int))
{
  int ch;

  /* escape sequence */
 restart:
  ch = QuotedNxtch(inp_stream);
  switch (ch) {
  case 10:
    ch = QuotedNxtch(inp_stream);
    if (ch == '\\') goto restart;
    return ch;
  case 'a':
    return '\a';
  case 'b':
    return '\b';
  case 'c':
    if (yap_flags[CHARACTER_ESCAPE_FLAG] == ISO_CHARACTER_ESCAPES) {
      Yap_ErrorMessage = "invalid escape sequence \\c";
      return 0;
    } else {
      /* sicstus */
      ch = QuotedNxtch(inp_stream);
      if (chtype(ch) == SL) {
	goto restart;
      } else {
	return 'c';
      }
    }
  case 'd':
    return 127;
  case 'e':
    return '\x1B';  /* <ESC>, a.k.a. \e */
  case 'f':
    return '\f';
  case 'n':
    return '\n';
  case 'r':
    return '\r';
  case 't':
    return '\t';
  case 'u':
    {
      int i;
      wchar_t wc='\0';

      for (i=0; i< 4; i++) {
	ch = QuotedNxtch(inp_stream);
	if (ch>='0' && ch <= '9') {
	  wc += (ch-'0')<<((3-i)*4);
	} else if (ch>='a' && ch <= 'f') {
	  wc += ((ch-'a')+10)<<((3-i)*4);
	} else if (ch>='A' && ch <= 'F') {
	  wc += ((ch-'A')+10)<<((3-i)*4);
	} else {
	  Yap_ErrorMessage = "invalid escape sequence";
	  return 0;
	}
      }
      return wc;
    }
  case 'U':
    {
      int i;
      wchar_t wc='\0';

      for (i=0; i< 8; i++) {
	ch = QuotedNxtch(inp_stream);
	if (ch>='0' && ch <= '9') {
	  wc += (ch-'0')<<((7-i)*4);
	} else if (ch>='a' && ch <= 'f') {
	  wc += ((ch-'a')+10)<<((7-i)*4);
	} else if (ch>='A' && ch <= 'F') {
	  wc += ((ch-'A')+10)<<((7-i)*4);
	} else {
	  Yap_ErrorMessage = "invalid escape sequence";
	  return 0;
	}
      }
      return wc;
    }
  case 'v':
    return '\v';
  case '\\':
    return '\\';
  case '\'':
    return '\'';
  case '"':
    return '"';
  case '`':
    return '`';
  case '^':
    if (yap_flags[CHARACTER_ESCAPE_FLAG] == ISO_CHARACTER_ESCAPES) {
      Yap_ErrorMessage = "invalid escape sequence";
      return 0;
    } else {
      ch = QuotedNxtch(inp_stream);
      if (ch ==  '?') {/* delete character */
	return 127;
      } else if (ch >= 'a' && ch < 'z') {/* octal */
	return ch - 'a';
      } else if (ch >= 'A' && ch < 'Z') {/* octal */
	return ch - 'A';
      } else {
	 return '^';
      }
    }
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
    /* character in octal: maximum of 3 digits, terminates with \ */
    if (yap_flags[CHARACTER_ESCAPE_FLAG] == ISO_CHARACTER_ESCAPES) {
      unsigned char so_far = ch-'0';
      ch = QuotedNxtch(inp_stream);
      if (ch >= '0' && ch < '8') {/* octal */
	so_far = so_far*8+(ch-'0');
	ch = QuotedNxtch(inp_stream);
	if (ch >= '0' && ch < '8') { /* octal */
	  so_far = so_far*8+(ch-'0');
	  ch = QuotedNxtch(inp_stream);
	  if (ch != '\\') {
	    Yap_ErrorMessage = "invalid octal escape sequence";
	    return 0;
	  }
	} else if (ch == '\\') {
	  return so_far;
	} else {
	  Yap_ErrorMessage = "invalid octal escape sequence";
	  return 0;
	}
      } else if (ch == '\\') {
	return so_far;
      } else {
	Yap_ErrorMessage = "invalid octal escape sequence";
	return 0;
      }
    } else {
      /* sicstus */
      unsigned char so_far = ch-'0';
      ch = QuotedNxtch(inp_stream);
      if (ch >= '0' && ch < '8') {/* octal */
	so_far = so_far*8+(ch-'0');
	ch = QuotedNxtch(inp_stream);
	if (ch >= '0' && ch < '8') { /* octal */
	  return so_far*8+(ch-'0');
	} else {
	  *scan_nextp = FALSE;
	  return so_far;
	}
      } else {
	*scan_nextp = FALSE;
	return so_far;
      }
    }
  case 'x':
    /* hexadecimal character (YAP allows empty hexadecimal  */
    if (yap_flags[CHARACTER_ESCAPE_FLAG] == ISO_CHARACTER_ESCAPES) {
      unsigned char so_far = 0; 
      ch = QuotedNxtch(inp_stream);
      if (my_isxdigit(ch,'f','F')) {/* hexa */
	so_far = so_far * 16 + (chtype(ch) == NU ? ch - '0' :
				(my_isupper(ch) ? ch - 'A' : ch - 'a') + 10);
	ch = QuotedNxtch(inp_stream);
	if (my_isxdigit(ch,'f','F')) { /* hexa */
	  so_far = so_far * 16 + (chtype(ch) == NU ? ch - '0' :
				  (my_isupper(ch) ? ch - 'A' : ch - 'a') + 10);
	  ch = QuotedNxtch(inp_stream);
	  if (ch == '\\') {
	    return so_far;
	  } else {
	    Yap_ErrorMessage = "invalid hexadecimal escape sequence";
	    return 0;
	  }
	} else if (ch == '\\') {
	  return so_far;
	} else {
	  Yap_ErrorMessage = "invalid hexadecimal escape sequence";
	  return 0;
	} 
      } else if (ch == '\\') {
	return so_far;
      } else {
	Yap_ErrorMessage = "invalid hexadecimal escape sequence";
	return 0;
      }
    } else {
      /* sicstus mode */
      unsigned char so_far = 0;
      ch = QuotedNxtch(inp_stream);
      so_far = (chtype(ch) == NU ? ch - '0' :
		my_isupper(ch) ? ch - 'A' + 10 : 
		my_islower(ch) ? ch - 'a' +10 : 0);
      ch = QuotedNxtch(inp_stream);
      return so_far*16 + (chtype(ch) == NU ? ch - '0' :
		       my_isupper(ch) ? ch - 'A' +10 :
		       my_islower(ch) ? ch - 'a' + 10 : 0);
    }
  default:
    /* accept sequence. Note that the ISO standard does not
       consider this sequence legal, whereas SICStus would
       eat up the escape sequence. */
    if (yap_flags[CHARACTER_ESCAPE_FLAG] == ISO_CHARACTER_ESCAPES) {
      Yap_ErrorMessage = "invalid escape sequence";
      return 0;
    } else {
      /* sicstus */
      if (chtype(ch) == SL) {
	goto restart;
      } else {
	return ch;
      }
    }
  }
}
	    
/* reads a number, either integer or float */

static Term
get_num(int *chp, int *chbuffp, int inp_stream, int (*Nxtch) (int), int (*QuotedNxtch) (int), char *s, UInt max_size, int sign)
{
  char *sp = s;
  int ch = *chp;
  Int val = 0L, base = ch - '0';
  int might_be_float = TRUE, has_overflow = FALSE;

  *sp++ = ch;
  ch = Nxtch(inp_stream);
  /*
   * because of things like 00'2, 03'2 and even better 12'2, I need to
   * do this (have mercy) 
   */
  if (chtype(ch) == NU) {
    *sp++ = ch;
    if (--max_size == 0) {
      Yap_ErrorMessage = "Number Too Long";
      return TermNil;
    }
    base = 10 * base + ch - '0';
    ch = Nxtch(inp_stream);
  }
  if (ch == '\'') {
    if (base > 36) {
      Yap_ErrorMessage = "Admissible bases are 0..36";
      return TermNil;
    }
    might_be_float = FALSE;
    if (--max_size == 0) {
      Yap_ErrorMessage = "Number Too Long";
      return TermNil;
    }
    *sp++ = ch;
    ch = Nxtch(inp_stream);
    if (base == 0) {
      wchar_t ascii = ch;
      int scan_extra = TRUE;

      if (ch == '\\' &&
	  yap_flags[CHARACTER_ESCAPE_FLAG] != CPROLOG_CHARACTER_ESCAPES) {
	ascii = read_quoted_char(&scan_extra, inp_stream, QuotedNxtch);
      }
      /* a quick way to represent ASCII */
      if (scan_extra)
	*chp = Nxtch(inp_stream);
      if (sign == -1) {
	return MkIntegerTerm(-ascii);
      }
      return MkIntegerTerm(ascii);
    } else if (base >= 10 && base <= 36) {
      int upper_case = 'A' - 11 + base;
      int lower_case = 'a' - 11 + base;

      while (my_isxdigit(ch, upper_case, lower_case)) {
	Int oval = val;
	int chval = (chtype(ch) == NU ? ch - '0' :
		     (my_isupper(ch) ? ch - 'A' : ch - 'a') + 10);
	if (--max_size == 0) {
	  Yap_ErrorMessage = "Number Too Long";
	  return TermNil;
	}
	*sp++ = ch;
	val = oval * base + chval;
	if (oval != (val-chval)/base) /* overflow */
	  has_overflow = (has_overflow || TRUE);
	ch = Nxtch(inp_stream);
      }
    }
  } else if ((ch == 'x' || ch == 'X') && base == 0) {
    might_be_float = FALSE;
    if (--max_size == 0) {
      Yap_ErrorMessage = "Number Too Long";
      return TermNil;
    }
    *sp++ = ch;
    ch = Nxtch(inp_stream);
    while (my_isxdigit(ch, 'F', 'f')) {
      Int oval = val;
      int chval = (chtype(ch) == NU ? ch - '0' :
		   (my_isupper(ch) ? ch - 'A' : ch - 'a') + 10);
      if (--max_size == 0) {
	Yap_ErrorMessage = "Number Too Long";
	return TermNil;
      }
      *sp++ = ch;
      val = val * 16 + chval;
      if (oval != (val-chval)/16) /* overflow */
	has_overflow = TRUE;
      ch = Nxtch(inp_stream);
    }
    *chp = ch;
  }
  else if ((ch == 'o') && base == 0) {
    might_be_float = FALSE;
    base = 8;
    if (--max_size == 0) {
      Yap_ErrorMessage = "Number Too Long";
      return (TermNil);
    }
    *sp++ = ch;
    *chp = Nxtch(inp_stream);
  }
  else {
    val = base;
    base = 10;
  }
  while (chtype(ch) == NU) {
    Int oval = val;
    if (!(val == 0 && ch == '0') || has_overflow) {
      if (--max_size == 0) {
	Yap_ErrorMessage = "Number Too Long";
	return (TermNil);
      }
      *sp++ = ch;
    }
    if (ch - '0' >= base) {
      if (sign == -1)
	return MkIntegerTerm(-val);
      return MkIntegerTerm(val);
    }
    val = val * base + ch - '0';
    if (val/base != oval || val -oval*base != ch-'0') /* overflow */
      has_overflow = TRUE;
    ch = Nxtch(inp_stream);
  }
  if (might_be_float && (ch == '.' || ch == 'e' || ch == 'E')) {
    if (ch == '.') {
      if (--max_size == 0) {
	Yap_ErrorMessage = "Number Too Long";
	return TermNil;
      }
      *sp++ = '.';
      if (chtype(ch = Nxtch(inp_stream)) != NU) {
	*chbuffp = '.';
	*chp = ch;
	*--sp = '\0';
	if (has_overflow)
	  return read_int_overflow(s,base,val,sign);
	if (sign == -1)
	  return MkIntegerTerm(-val);
	return MkIntegerTerm(val);
      }
      do {
	if (--max_size == 0) {
	  Yap_ErrorMessage = "Number Too Long";
	  return TermNil;
	}
	*sp++ = ch;
      }
      while (chtype(ch = Nxtch(inp_stream)) == NU);
    }
    if (ch == 'e' || ch == 'E') {
      char *sp0 = sp;
      char cbuff = ch;

      if (--max_size == 0) {
	Yap_ErrorMessage = "Number Too Long";
	return TermNil;
      }
      *sp++ = ch;
      ch = Nxtch(inp_stream);
      if (ch == '-') {
	cbuff = '-';
	if (--max_size == 0) {
	  Yap_ErrorMessage = "Number Too Long";
	  return TermNil;
	}
	*sp++ = '-';
	ch = Nxtch(inp_stream);
      } else if (ch == '+') {
	cbuff = '+';
	ch = Nxtch(inp_stream);
      }
      if (chtype(ch) != NU) {
	/* error */
	char *sp;
	*chp = ch;
	if (*sp0 == 'E') {
	  /* code the fact that we have E and not e */
	  if (cbuff == '+')
	    *chbuffp = '=';
	  else
	    *chbuffp = '_';
	} else {
	  *chbuffp = cbuff;
	}
	*sp0 = '\0';
	for (sp = s; sp < sp0; sp++) {
	  if (*sp == '.')
	    return float_send(s,sign);
	}
	return MkIntegerTerm(sign*val);
      }
      do {
	if (--max_size == 0) {
	  Yap_ErrorMessage = "Number Too Long";
	  return TermNil;
	}
	*sp++ = ch;
      } while (chtype(ch = Nxtch(inp_stream)) == NU);
    }
    *sp = '\0';
    *chp = ch;
    return float_send(s,sign);
  } else if (has_overflow) {
    *sp = '\0';
    /* skip base */
    *chp = ch;
    if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
      return read_int_overflow(s+2,16,val,sign);
    if (s[1] == '\'')
      return read_int_overflow(s+2,base,val,sign);
    if (s[2] == '\'')
      return read_int_overflow(s+3,base,val,sign);
    return read_int_overflow(s,base,val,sign);
  } else {
    *chp = ch;
    return MkIntegerTerm(val*sign);
  }
}

/* given a function Nxtch scan until we  either find the number
   or end of file */
Term
Yap_scan_num(int (*Nxtch) (int))
{
  Term out;
  int sign = 1;
  int ch, cherr;
  char *ptr;

  Yap_ErrorMessage = NULL;
  ScannerStack = (char *)TR;
  ScannerExtraBlocks = NULL;
  if (!(ptr = AllocScannerMemory(4096))) {
    Yap_ErrorMessage = "Trail Overflow";
    Yap_Error_TYPE = OUT_OF_TRAIL_ERROR;	            
    return TermNil;
  }
  ch = Nxtch(-1);
  if (ch == '-') {
    sign = -1;
    ch = Nxtch(-1);
  } else if (ch == '+') {
    ch = Nxtch(-1);
  }
  if (chtype(ch) != NU) {
    Yap_clean_tokenizer(NULL, NULL, NULL);
    return TermNil;
  }
  cherr = '\0';
  if (ASP-H < 1024)
    return TermNil;
  out = get_num(&ch, &cherr, -1, Nxtch, Nxtch, ptr, 4096, sign); /*  */
  PopScannerMemory(ptr, 4096);
  Yap_clean_tokenizer(NULL, NULL, NULL);
  if (Yap_ErrorMessage != NULL || ch != -1 || cherr)
    return TermNil;
  return out;
}


static wchar_t *
ch_to_wide(char *base, char *charp)
{
  int n = charp-base, i;
  wchar_t *nb = (wchar_t *)base;

  if ((nb+n) + 1024 > (wchar_t *)AuxSp) {
    Yap_Error_TYPE = OUT_OF_AUXSPACE_ERROR;	  
    Yap_ErrorMessage = "Heap Overflow While Scanning: please increase code space (-h)";
    return NULL;
  }
  for (i=n; i > 0; i--) {
    nb[i-1] = base[i-1];
  }
  return nb+n;
}

TokEntry *
Yap_tokenizer(int inp_stream, Term *tposp)
{
  TokEntry *t, *l, *p;
  enum TokenKinds kind;
  int solo_flag = TRUE;
  int ch;
  wchar_t *wcharp;
  int (*Nxtch) (int) = Stream[inp_stream].stream_wgetc_for_read;
  int (*QuotedNxtch) (int) = Stream[inp_stream].stream_wgetc;

  Yap_ErrorMessage = NULL;
  Yap_Error_Size = 0;
  Yap_VarTable = NULL;
  Yap_AnonVarTable = NULL;
  Yap_eot_before_eof = FALSE;
  ScannerStack = (char *)TR;
  ScannerExtraBlocks = NULL;
  l = NULL;
  p = NULL;			/* Just to make lint happy */
  LOCK(Stream[inp_stream].streamlock);
  ch = Nxtch(inp_stream);
  while (chtype(ch) == BS) {
    ch = Nxtch(inp_stream);
  }
  *tposp = Yap_StreamPosition(inp_stream);
  do {
    wchar_t och;
    int quote, isvar;
    char *charp, *mp;
    unsigned int len;
    char *TokImage = NULL;


    t = (TokEntry *) AllocScannerMemory(sizeof(TokEntry));
    t->TokNext = NULL;
    if (t == NULL) {
      Yap_ErrorMessage = "Trail Overflow";
      Yap_Error_TYPE = OUT_OF_TRAIL_ERROR;	            
      if (p)
	p->Tok = Ord(kind = eot_tok);
      /* serious error now */
      UNLOCK(Stream[inp_stream].streamlock);
      return l;
    }
    if (!l)
      l = t;
    else
      p->TokNext = t;
    p = t;
  restart:
    while (chtype(ch) == BS) {
      ch = Nxtch(inp_stream);
    }
    t->TokPos = GetCurInpPos(inp_stream);

    switch (chtype(ch)) {

    case CC:
      while ((ch = Nxtch(inp_stream)) != 10 && chtype(ch) != EF);
      if (chtype(ch) != EF) {
	/* blank space */
	if (t == l) {
	  /* we found a comment before reading characters */
	  while (chtype(ch) == BS) {
	    ch = Nxtch(inp_stream);
	  }
	  *tposp = Yap_StreamPosition(inp_stream);
	}
	goto restart;
      } else {
	t->Tok = Ord(kind = eot_tok);
      }
      break;

    case UC:
    case UL:
    case LC:
      och = ch;
      ch = Nxtch(inp_stream);
    scan_name:
      TokImage = ((AtomEntry *) ( Yap_PreAllocCodeSpace()))->StrOfAE;
      charp = TokImage;
      isvar = (chtype(och) != LC);
      *charp++ = och;
      for (; chtype(ch) <= NU; ch = Nxtch(inp_stream)) {
	if (charp == (char *)AuxSp-1024) {
	huge_var_error:
	  /* huge atom or variable, we are in trouble */
	  Yap_ErrorMessage = "Code Space Overflow due to huge atom";
	  Yap_Error_TYPE = OUT_OF_AUXSPACE_ERROR;	  
	  Yap_ReleasePreAllocCodeSpace((CODEADDR)TokImage);
	  if (p)
	    p->Tok = Ord(kind = eot_tok);
	  /* serious error now */
	  UNLOCK(Stream[inp_stream].streamlock);
	  return l;
	}
	*charp++ = ch;
      }
      while (ch == '\'' && isvar && yap_flags[VARS_CAN_HAVE_QUOTE_FLAG]) {
	if (charp == (char *)AuxSp-1024) {
	  goto huge_var_error;
	}
	*charp++ = ch;
	ch = Nxtch(inp_stream);
      }
      *charp++ = '\0';
      if (!isvar) {
	/* don't do this in iso */
	Atom ae = Yap_LookupAtom(TokImage);
	if (ae == NIL) {
	  Yap_Error_TYPE = OUT_OF_HEAP_ERROR;	  
	  Yap_ErrorMessage = "Code Space Overflow";
	  if (p)
	    t->Tok = Ord(kind = eot_tok);
	  /* serious error now */
	  UNLOCK(Stream[inp_stream].streamlock);
	  return l;
	}
	t->TokInfo = Unsigned(ae);
	Yap_ReleasePreAllocCodeSpace((CODEADDR)TokImage);
	if (ch == '(')
	  solo_flag = FALSE;
	t->Tok = Ord(kind = Name_tok);
      } else {
	t->TokInfo = Unsigned(Yap_LookupVar(TokImage));
	Yap_ReleasePreAllocCodeSpace((CODEADDR)TokImage);
	t->Tok = Ord(kind = Var_tok);
      }
      break;

    case NU:
      {
	int cherr;
	int cha = ch;
	char *ptr;

	cherr = 0;
	if (!(ptr = AllocScannerMemory(4096))) {
	  UNLOCK(Stream[inp_stream].streamlock);
	  Yap_ErrorMessage = "Trail Overflow";
	  Yap_Error_TYPE = OUT_OF_TRAIL_ERROR;	            
	  if (p)
	    t->Tok = Ord(kind = eot_tok);
	  /* serious error now */
	  return l;
	}
	if (ASP-H < 1024) {
	  UNLOCK(Stream[inp_stream].streamlock);
	  Yap_ErrorMessage = "Stack Overflow";
	  Yap_Error_TYPE = OUT_OF_STACK_ERROR;	            
	  Yap_Error_Size = 0L;	            
	  if (p)
	    p->Tok = Ord(kind = eot_tok);
	  /* serious error now */
	  return l;
	}
	if ((t->TokInfo = get_num(&cha,&cherr,inp_stream,Nxtch,QuotedNxtch,ptr,4096,1)) == 0L) {
	  UNLOCK(Stream[inp_stream].streamlock);
	  if (p)
	    p->Tok = Ord(kind = eot_tok);
	  /* serious error now */
	  return l;
	}
	PopScannerMemory(ptr, 4096);
	ch = cha;
	if (cherr) {
	  TokEntry *e;
	  t->Tok = Number_tok;
	  t->TokPos = GetCurInpPos(inp_stream);
	  e = (TokEntry *) AllocScannerMemory(sizeof(TokEntry));
	  if (e == NULL) {
	    UNLOCK(Stream[inp_stream].streamlock);
	    Yap_ErrorMessage = "Trail Overflow";
	    Yap_Error_TYPE = OUT_OF_TRAIL_ERROR;	            
	    if (p)
	      p->Tok = Ord(kind = eot_tok);
	    /* serious error now */
	    return l;
	  } else {
	    e->TokNext = NULL;
	  }
	  t->TokNext = e;
	  t = e;
	  p = e;
	  switch (cherr) {
	  case 'e':
	  case 'E':
	    och = cherr;
	    goto scan_name;
	    break;
	  case '=':
	  case '_':
	    /* handle error while parsing a float */
	    {
	      TokEntry *e2;

	      t->Tok = Ord(Var_tok);
	      t->TokInfo = Unsigned(Yap_LookupVar("E"));
	      t->TokPos = GetCurInpPos(inp_stream);
	      e2 = (TokEntry *) AllocScannerMemory(sizeof(TokEntry));
	      if (e2 == NULL) {
		UNLOCK(Stream[inp_stream].streamlock);
		Yap_ErrorMessage = "Trail Overflow";
		Yap_Error_TYPE = OUT_OF_TRAIL_ERROR;	            
		if (p)
		  p->Tok = Ord(kind = eot_tok);
		/* serious error now */
		return l;
	      } else {
		e2->TokNext = NULL;
	      }
	      t->TokNext = e2;
	      t = e2;
	      p = e2;
	      if (cherr == '=')
		och = '+';
	      else
		och = '-';
	    }
	    goto enter_symbol;
	  case '+':
	  case '-':
	    /* handle error while parsing a float */
	    {
	      TokEntry *e2;

	      t->Tok = Name_tok;
	      if (ch == '(')
		solo_flag = FALSE;
	      t->TokInfo = Unsigned(AtomE);
	      t->TokPos = GetCurInpPos(inp_stream);
	      e2 = (TokEntry *) AllocScannerMemory(sizeof(TokEntry));
	      if (e2 == NULL) {
		UNLOCK(Stream[inp_stream].streamlock);
		Yap_ErrorMessage = "Trail Overflow";
		Yap_Error_TYPE = OUT_OF_TRAIL_ERROR;	            
		t->Tok = Ord(kind = eot_tok);
		/* serious error now */
		return l;
	      } else {
		e2->TokNext = NULL;
	      }
	      t->TokNext = e2;
	      t = e2;
	      p = e2;
	    }
	  default:
	    och = cherr;
	    goto enter_symbol;
	  }
	} else {
	  t->Tok = Ord(kind = Number_tok);
	}
      }
      break;

    case QT:
    case DC:
      TokImage = ((AtomEntry *) ( Yap_PreAllocCodeSpace()))->StrOfAE;
      charp = TokImage;
      quote = ch;
      len = 0;
      ch = QuotedNxtch(inp_stream);
      wcharp = NULL;

      while (TRUE) {
	if (wcharp && wcharp + 1024 > (wchar_t *)AuxSp) {
	  Yap_Error_TYPE = OUT_OF_AUXSPACE_ERROR;	  
	  Yap_ErrorMessage = "Heap Overflow While Scanning: please increase code space (-h)";
	  break;
	} else if (charp + 1024 > (char *)AuxSp) {
	  Yap_Error_TYPE = OUT_OF_AUXSPACE_ERROR;	  
	  Yap_ErrorMessage = "Heap Overflow While Scanning: please increase code space (-h)";
	  break;
	}
	if (!wcharp && ch > MAX_ISO_LATIN1){
	  /* does not fit in ISO-LATIN */
	  wcharp = ch_to_wide(TokImage, charp);
	}
	if (ch == 10  &&  yap_flags[CHARACTER_ESCAPE_FLAG] == ISO_CHARACTER_ESCAPES) {
	  /* in ISO a new line terminates a string */
	  Yap_ErrorMessage = "layout character \n inside quotes";
	  break;
	}
	if (ch == quote) {
	  ch = QuotedNxtch(inp_stream);
	  if (ch != quote)
	    break;
	  if (wcharp) 
	    *wcharp++ = ch;
	  else
	    *charp++ = ch;
	  ch = QuotedNxtch(inp_stream);
	} else if (ch == '\\' && yap_flags[CHARACTER_ESCAPE_FLAG] != CPROLOG_CHARACTER_ESCAPES) {
	  int scan_next = TRUE;
	  if (wcharp) 
	    *wcharp++ = read_quoted_char(&scan_next, inp_stream, QuotedNxtch);
	  else {
	    wchar_t next = read_quoted_char(&scan_next, inp_stream, QuotedNxtch);
	    if (next > MAX_ISO_LATIN1){
	      /* does not fit in ISO-LATIN */
	      wcharp = ch_to_wide(TokImage, charp);
	      *wcharp++ = next;
	    } else {
	      *charp++ = next;
	    }
	  }
	  if (scan_next) {
	    ch = QuotedNxtch(inp_stream);
	  }
	} else if (chtype(ch) == EF && ch <= MAX_ISO_LATIN1) {
	  Yap_ReleasePreAllocCodeSpace((CODEADDR)TokImage);
	  t->Tok = Ord(kind = eot_tok);
	  break;
	} else {
	  if (wcharp) 
	    *wcharp++ = ch;
	  else
	    *charp++ = ch;
	  ch = QuotedNxtch(inp_stream);
	}
	++len;
	if (charp > (char *)AuxSp - 1024) {
	  UNLOCK(Stream[inp_stream].streamlock);
	  /* Not enough space to read in the string. */
	  Yap_Error_TYPE = OUT_OF_AUXSPACE_ERROR;	  
	  Yap_ErrorMessage = "not enough space to read in string or quoted atom";
	  /* serious error now */
	  Yap_ReleasePreAllocCodeSpace((CODEADDR)TokImage);
	  t->Tok = Ord(kind = eot_tok);
	  return l;
	}
      }
      if (wcharp) {
	*wcharp = '\0';
      }  else  {
	*charp = '\0';
      }
      if (quote == '"') {
	if (wcharp) {
	  mp = AllocScannerMemory(sizeof(wchar_t)*(len+1));
	} else {
	  mp = AllocScannerMemory(len + 1);
	}
	if (mp == NULL) {
	  UNLOCK(Stream[inp_stream].streamlock);
	  Yap_ErrorMessage = "not enough heap space to read in string or quoted atom";
	  Yap_ReleasePreAllocCodeSpace((CODEADDR)TokImage);
	  t->Tok = Ord(kind = eot_tok);
	  return l;
	}
	if (wcharp) 
	  wcscpy((wchar_t *)mp,(wchar_t *)TokImage);
	else
	  strcpy(mp, TokImage);
	t->TokInfo = Unsigned(mp);
	Yap_ReleasePreAllocCodeSpace((CODEADDR)TokImage);
	if (wcharp) {
	  t->Tok = Ord(kind = WString_tok);
	} else {
	  t->Tok = Ord(kind = String_tok);
	}
      } else {
	if (wcharp) {
	  t->TokInfo = Unsigned(Yap_LookupWideAtom((wchar_t *)TokImage));
	} else {
	  t->TokInfo = Unsigned(Yap_LookupAtom(TokImage));
	  if (t->TokInfo == (CELL)NIL) {
	    Yap_Error_TYPE = OUT_OF_HEAP_ERROR;	  
	    Yap_ErrorMessage = "Code Space Overflow";
	    if (p)
	      t->Tok = Ord(kind = eot_tok);
	    /* serious error now */
	    UNLOCK(Stream[inp_stream].streamlock);
	    return l;
	  }
	}
	Yap_ReleasePreAllocCodeSpace((CODEADDR)TokImage);
	t->Tok = Ord(kind = Name_tok);
	if (ch == '(')
	  solo_flag = FALSE;
      }
      break;

    case SY:
      och = ch;
      ch = Nxtch(inp_stream);
      if (och == '/' && ch == '*') {
	while ((och != '*' || ch != '/') && chtype(ch) != EF) {
	  och = ch;
	  ch = Nxtch(inp_stream);
	}
	if (chtype(ch) == EF) {
	  t->Tok = Ord(kind = eot_tok);
	}
	ch = Nxtch(inp_stream);
	if (t == l) {
	  /* we found a comment before reading characters */
	  while (chtype(ch) == BS) {
	    ch = Nxtch(inp_stream);
	  }
	  *tposp = Yap_StreamPosition(inp_stream);
	}
	goto restart;
      }
    enter_symbol:
      if (och == '.' && (chtype(ch) == BS || chtype(ch) == EF
			 || chtype(ch) == CC)) {
	Yap_eot_before_eof = TRUE;
	if (chtype(ch) == CC)
	  while ((ch = Nxtch(inp_stream)) != 10 && chtype(ch) != EF);
	t->Tok = Ord(kind = eot_tok);
      }
      else {
	TokImage = ((AtomEntry *) ( Yap_PreAllocCodeSpace()))->StrOfAE;
	charp = TokImage;
	*charp++ = och;
	for (; chtype(ch) == SY; ch = Nxtch(inp_stream))
	  *charp++ = ch;
	*charp = '\0';
	t->TokInfo = Unsigned(Yap_LookupAtom(TokImage));
	if (t->TokInfo == (CELL)NIL) {
	  Yap_Error_TYPE = OUT_OF_HEAP_ERROR;	  
	  Yap_ErrorMessage = "Code Space Overflow";
	  if (p)
	    t->Tok = Ord(kind = eot_tok);
	  /* serious error now */
	  UNLOCK(Stream[inp_stream].streamlock);
	  return l;
	}
	Yap_ReleasePreAllocCodeSpace((CODEADDR)TokImage);
	t->Tok = Ord(kind = Name_tok);
	if (ch == '(')
	  solo_flag = FALSE;
      }
      break;
    
    case SL:
      {
	char chs[2];
	chs[0] = ch;
	chs[1] = '\0';
	ch = Nxtch(inp_stream);
	t->TokInfo = Unsigned(Yap_LookupAtom(chs));
	t->Tok = Ord(kind = Name_tok);
	if (ch == '(')
	  solo_flag = FALSE;
      }
      break;

    case BK:
      och = ch;
      ch = Nxtch(inp_stream);
      t->TokInfo = och;
      if (t->TokInfo == '(' && !solo_flag) {
	t->TokInfo = 'l';
	solo_flag = TRUE;
      }
      t->Tok = Ord(kind = Ponctuation_tok);
      break;

    case EF:
      t->Tok = Ord(kind = eot_tok);
      break;

    default:
#ifdef DEBUG
      fprintf(Yap_stderr, "\n++++ token: wrong char type %c %d\n", ch, chtype(ch));
#endif
      t->Tok = Ord(kind = eot_tok);
    }
#ifdef DEBUG
    if(Yap_Option[2]) fprintf(Yap_stderr,"[Token %d %ld]",Ord(kind),(unsigned long int)t->TokInfo);
#endif
    if (Yap_ErrorMessage) {
      /* insert an error token to inform the system of what happened */
      TokEntry *e = (TokEntry *) AllocScannerMemory(sizeof(TokEntry));
      if (e == NULL) {
	UNLOCK(Stream[inp_stream].streamlock);
	Yap_ErrorMessage = "Trail Overflow";
	Yap_Error_TYPE = OUT_OF_TRAIL_ERROR;	            
	p->Tok = Ord(kind = eot_tok);
	/* serious error now */
	return l;
      }
      p->TokNext = e;
      e->Tok = Error_tok;
      e->TokInfo = MkAtomTerm(Yap_LookupAtom(Yap_ErrorMessage));
      e->TokPos = GetCurInpPos(inp_stream);
      e->TokNext = NULL;
      Yap_ErrorMessage = NULL;
      p = e;
    }
  } while (kind != eot_tok);
  UNLOCK(Stream[inp_stream].streamlock);
  return (l);
}

void
Yap_clean_tokenizer(TokEntry *tokstart, VarEntry *vartable, VarEntry *anonvartable)
{
  struct scanner_extra_alloc *ptr = ScannerExtraBlocks;
  while (ptr) {
    struct scanner_extra_alloc *next = ptr->next;
    free(ptr);
    ptr = next;
  }
}

