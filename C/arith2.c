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
 * File:		arith2.c						 *
 * Last rev:								 *
 * mods:									 *
 * comments:	arithmetical expression evaluation			 *
 *									 *
 *************************************************************************/
#ifdef SCCS
static char     SccsId[] = "%W% %G%";
#endif

/*
 * This file implements unary arithmetic operations in YAP
 *
 */

#include "Yap.h"
#include "Yatom.h"
#include "YapHeap.h"
#include "eval.h"

#include "arith2.h"

typedef struct init_un_eval {
  char          *OpName;
  arith2_op      f;
} InitBinEntry;


#ifdef USE_GMP
static Float
fdiv_bigint(MP_INT *b1,MP_INT *b2)
{
  Float f1 = mpz_get_d(b1);
  Float f2 = mpz_get_d(b2);
  if (1) {
    mpf_t f1,f2;
    Float res;

    mpf_init(f1);
    mpf_init(f2);
    mpf_set_z(f1, b1);
    mpf_set_z(f2, b2);
    mpf_div(f1, f1, f2);
    res = mpf_get_d(f1);
    mpf_clear(f1);
    mpf_clear(f2);
    return(res);
  } else {
    return(f1/f2);
  }
}
#endif

static Term
p_mod(Term t1, Term t2) {
  switch (ETypeOfTerm(t1)) {
  case (CELL)long_int_e:
    switch (ETypeOfTerm(t2)) {
    case (CELL)long_int_e:
      /* two integers */
      {
	Int i1 = IntegerOfTerm(t1);
	Int i2 = IntegerOfTerm(t2);
	Int mod;

	if (i2 == 0) goto zero_divisor;
	if (i1 == Int_MIN && i2 == -1) {
#ifdef USE_GMP
	  return Yap_gmp_add_ints(Int_MAX, 1);	  
#else
	  return Yap_ArithError(EVALUATION_ERROR_INT_OVERFLOW, t1,
		    "// /2 with %d and %d", i1, i2);
#endif
	}
	mod = i1%i2;
	if (mod && (mod ^ i2) < 0)
	  mod += i2;
	RINT(mod);
      }
    case (CELL)double_e:
      return Yap_ArithError(TYPE_ERROR_INTEGER, t2, "mod/2");
    case (CELL)big_int_e:
#ifdef USE_GMP
      return Yap_gmp_mod_int_big(IntegerOfTerm(t1), Yap_BigIntOfTerm(t2));
#endif
    default:
      RERROR();
      break;
    }
  case (CELL)double_e:
    return Yap_ArithError(TYPE_ERROR_INTEGER, t2, "mod/2");
  case (CELL)big_int_e:
#ifdef USE_GMP
    switch (ETypeOfTerm(t2)) {
    case long_int_e:
      /* modulo between bignum and integer */
      {
	Int i2 = IntegerOfTerm(t2);

	if (i2 == 0) goto zero_divisor;
	return Yap_gmp_mod_big_int(Yap_BigIntOfTerm(t1), i2);
      }
    case (CELL)big_int_e:
      /* two bignums */
      return Yap_gmp_mod_big_big(Yap_BigIntOfTerm(t1), Yap_BigIntOfTerm(t2));
    case double_e:
      return Yap_ArithError(TYPE_ERROR_INTEGER, t2, "mod/2");
    default:
      RERROR();
    }
#endif
  default:
    RERROR();
  }
zero_divisor:
  return Yap_ArithError(EVALUATION_ERROR_ZERO_DIVISOR, t2, "X is mod 0");
}

static Term
p_rem(Term t1, Term t2) {
  switch (ETypeOfTerm(t1)) {
  case (CELL)long_int_e:
    switch (ETypeOfTerm(t2)) {
    case (CELL)long_int_e:
      /* two integers */
      {
	Int i1 = IntegerOfTerm(t1);
	Int i2 = IntegerOfTerm(t2);
	Int mod;

	if (i2 == 0) goto zero_divisor;
	if (i1 == Int_MIN && i2 == -1) {
#ifdef USE_GMP
	  return Yap_gmp_add_ints(Int_MAX, 1);	  
#else
	  return Yap_ArithError(EVALUATION_ERROR_INT_OVERFLOW, t1,
		    "rem/2 with %d and %d", i1, i2);
#endif
	}
	mod = i1%i2;
	RINT(i1%i2);
      }
    case (CELL)double_e:
      return Yap_ArithError(TYPE_ERROR_INTEGER, t2, "mod/2");
    case (CELL)big_int_e:
#ifdef USE_GMP
      /* I know the term is much larger, so: */
      RINT(IntegerOfTerm(t1));
#endif
    default:
      RERROR();
    }
    break;
  case (CELL)double_e:
    return Yap_ArithError(TYPE_ERROR_INTEGER, t1, "mod/2");
  case (CELL)big_int_e:
#ifdef USE_GMP
    switch (ETypeOfTerm(t2)) {
    case long_int_e:
      /* modulo between bignum and integer */
      {
	mpz_t tmp;
	MP_INT new;
	Int i2 = IntegerOfTerm(t2);

	if (i2 == 0) goto zero_divisor;
	mpz_init(&new);
	mpz_init_set_si(tmp, i2);
	mpz_tdiv_r(&new, Yap_BigIntOfTerm(t1), tmp);
	mpz_clear(tmp);
	RBIG(&new);
      }
    case (CELL)big_int_e:
      /* two bignums */
      {
	MP_INT new;

	mpz_init(&new);
	mpz_tdiv_r(&new, Yap_BigIntOfTerm(t1), Yap_BigIntOfTerm(t2));
	RBIG(&new);
      }
    case double_e:
      return Yap_ArithError(TYPE_ERROR_INTEGER, t2, "mod/2");
    default:
      RERROR();
    }
#endif
  default:
    RERROR();
  }
 zero_divisor:
  return Yap_ArithError(EVALUATION_ERROR_ZERO_DIVISOR, t2, "X is mod 0");
}


/*
  Floating point division: /
*/
static Term
p_fdiv(Term t1, Term t2)
{
  switch (ETypeOfTerm(t1)) {
  case long_int_e:
    switch (ETypeOfTerm(t2)) {
    case long_int_e:
      {
	Int i2 = IntegerOfTerm(t2);

	/* two integers */
	RFLOAT((((Float)IntegerOfTerm(t1))/(Float)i2));
      }
    case double_e:
      {
	/* integer, double */
	Float fl1 = (Float)IntegerOfTerm(t1);
	Float fl2 = FloatOfTerm(t2);
	RFLOAT(fl1/fl2);
      }
    case (CELL)big_int_e:
#ifdef USE_GMP
      {
	Int i1 = IntegerOfTerm(t1);
	Float f2 = mpz_get_d(Yap_BigIntOfTerm(t2));
	RFLOAT(((Float)i1/f2));
      }
#endif
    default:
      RERROR();
    }
    break;
  case double_e:
    switch (ETypeOfTerm(t2)) {
    case long_int_e:
      /* float / integer */
      {
	Int i2 = IntegerOfTerm(t2);
	RFLOAT(FloatOfTerm(t1)/(Float)i2);
      }
    case double_e:
      {
	Float f2 = FloatOfTerm(t2);
	RFLOAT(FloatOfTerm(t1)/f2);
      }
    case big_int_e:
#ifdef USE_GMP
      {
	RFLOAT(FloatOfTerm(t1)/mpz_get_d(Yap_BigIntOfTerm(t2)));
      }
#endif
    default:
      RERROR();
    }
    break;
  case big_int_e:
#ifdef USE_GMP
    switch (ETypeOfTerm(t2)) {
    case long_int_e:
      {
	Int i = IntegerOfTerm(t2);
	RFLOAT(mpz_get_d(Yap_BigIntOfTerm(t1))/(Float)i);
      }
    case big_int_e:
      /* two bignums*/
      RFLOAT(fdiv_bigint(Yap_BigIntOfTerm(t1),Yap_BigIntOfTerm(t2)));
      //      RFLOAT(mpz_get_d(Yap_BigIntOfTerm(t1))/mpz_get_d(Yap_BigIntOfTerm(t2)));
    case double_e:
      {
	Float dbl = FloatOfTerm(t2);
	RFLOAT(mpz_get_d(Yap_BigIntOfTerm(t1))/dbl);
      }
    default:
      RERROR();
    }
#endif
  default:
    RERROR();
  }
  RERROR();
}

#if USE_GMP
#if !defined(HAVE_MPZ_XOR)
static void
mpz_xor(MP_INT *new, MP_INT *r1, MP_INT *r2)
{
  MP_INT *n2, *n3;
  
  mpz_new(n2);
  mpz_new(n3);
  mpz_ior(new, r1, r2);
  mpz_com(n2,  r1);
  mpz_and(n2, n2, new);
  mpz_com(n3,  r2);
  mpz_and(n3, n3, new);
  mpz_ior(new, n2, n3);
  mpz_clear(n2);
  mpz_clear(n3);
}
#endif
#endif

/*
  xor #
*/
static Term
p_xor(Term t1, Term t2)
{
  switch (ETypeOfTerm(t1)) {
  case long_int_e:
    
    switch (ETypeOfTerm(t2)) {
    case long_int_e:
      /* two integers */
      RINT(IntegerOfTerm(t1) ^ IntegerOfTerm(t2));
    case double_e:
      return Yap_ArithError(TYPE_ERROR_INTEGER, t2, "#/2");
    case big_int_e:
#ifdef USE_GMP
      {
	MP_INT new;

	mpz_init_set_si(&new,IntegerOfTerm(t1));
	mpz_xor(&new, &new, Yap_BigIntOfTerm(t2));
	RBIG(&new);
      }
#endif
    default:
      RERROR();
    }
    break;
  case double_e:
    return Yap_ArithError(TYPE_ERROR_INTEGER, t1, "#/2");
  case big_int_e:
#ifdef USE_GMP
    switch (ETypeOfTerm(t2)) {
    case long_int_e:
      {
	MP_INT new;

	mpz_init_set_si(&new,IntegerOfTerm(t2));
	mpz_xor(&new, Yap_BigIntOfTerm(t1), &new);
	RBIG(&new);
      }
    case big_int_e:
      /* two bignums */
      {
	MP_INT new;

	mpz_init_set(&new, Yap_BigIntOfTerm(t1));
	mpz_xor(&new, &new, Yap_BigIntOfTerm(t2));
	RBIG(&new);
      }
    case double_e:
      return Yap_ArithError(TYPE_ERROR_INTEGER, t2, "#/2");
    default:
      RERROR();
    }
#endif
  default:
    RERROR();
  }
  RERROR();
}

/*
  atan2: arc tangent x/y
*/
static Term
p_atan2(Term t1, Term t2)
{
  switch (ETypeOfTerm(t1)) {
  case long_int_e:
    switch (ETypeOfTerm(t2)) {
    case long_int_e:
      /* two integers */
      RFLOAT(atan2(IntegerOfTerm(t1),IntegerOfTerm(t2)));
    case double_e:
      RFLOAT(atan2(IntegerOfTerm(t1),FloatOfTerm(t2)));
    case big_int_e:
#ifdef USE_GMP
      {
	Int i1 = IntegerOfTerm(t1);
	Float f2 = mpz_get_d(Yap_BigIntOfTerm(t2));
	RFLOAT(atan2(i1,f2));
      }
#endif
    default:
      RERROR();
      break;
    }
  case double_e:
    switch (ETypeOfTerm(t2)) {
    case long_int_e:
      /* float / integer */
      {
	Int i2 = IntegerOfTerm(t2);
	RFLOAT(atan2(FloatOfTerm(t1),i2));
      }
    case double_e:
      {
	Float f2 = FloatOfTerm(t2);
	RFLOAT(atan2(FloatOfTerm(t1),f2));
      }
    case big_int_e:
#ifdef USE_GMP
      {
	RFLOAT(atan2(FloatOfTerm(t1),mpz_get_d(Yap_BigIntOfTerm(t2))));
      }
#endif
    default:
      RERROR();
    }
    break;
  case big_int_e:
#ifdef USE_GMP
    switch (ETypeOfTerm(t2)) {
    case long_int_e:
      {
	Int i = IntegerOfTerm(t2);
	RFLOAT(atan2(mpz_get_d(Yap_BigIntOfTerm(t1)),i));
      }
    case big_int_e:
      /* two bignums */
      RFLOAT(atan2(mpz_get_d(Yap_BigIntOfTerm(t1)),mpz_get_d(Yap_BigIntOfTerm(t2))));
    case double_e:
      {
	Float dbl = FloatOfTerm(t2);
	RFLOAT(atan2(mpz_get_d(Yap_BigIntOfTerm(t1)),dbl));
      }
    default:
      RERROR();
    }
#endif
  default:
    RERROR();
  }
  RERROR();
}


/*
  power: x^y
*/
static Term
p_power(Term t1, Term t2)
{
  switch (ETypeOfTerm(t1)) {
  case long_int_e:
    switch (ETypeOfTerm(t2)) {
    case long_int_e:
      {
	Int i2 = IntegerOfTerm(t2);

	/* two integers */
	RFLOAT(pow(IntegerOfTerm(t1),i2));
      }
    case double_e:
      {
	/* integer, double */
	Float fl1 = (Float)IntegerOfTerm(t1);
	Float fl2 = FloatOfTerm(t2);
	RFLOAT(pow(fl1,fl2));
      }
    case big_int_e:
#ifdef USE_GMP
      {
	Int i1 = IntegerOfTerm(t1);
	Float f2 = mpz_get_d(Yap_BigIntOfTerm(t2));
	RFLOAT(pow(i1,f2));
      }
#endif
    default:
      RERROR();
    }
    break;
  case double_e:
    switch (ETypeOfTerm(t2)) {
    case long_int_e:
      /* float / integer */
      {
	Int i2 = IntegerOfTerm(t2);
	RFLOAT(pow(FloatOfTerm(t1),i2));
      }
    case double_e:
      {
	Float f2 = FloatOfTerm(t2);
	RFLOAT(pow(FloatOfTerm(t1),f2));
      }
    case big_int_e:
#ifdef USE_GMP
      {
	RFLOAT(pow(FloatOfTerm(t1),mpz_get_d(Yap_BigIntOfTerm(t2))));
      }
#endif
    default:
      RERROR();
    }
    break;
  case big_int_e:
#ifdef USE_GMP
    switch (ETypeOfTerm(t2)) {
    case long_int_e:
      {
	Int i = IntegerOfTerm(t2);
	RFLOAT(pow(mpz_get_d(Yap_BigIntOfTerm(t1)),i));
      }
    case big_int_e:
      /* two bignums */
      RFLOAT(pow(mpz_get_d(Yap_BigIntOfTerm(t1)),mpz_get_d(Yap_BigIntOfTerm(t2))));
    case double_e:
      {
	Float dbl = FloatOfTerm(t2);
	RFLOAT(pow(mpz_get_d(Yap_BigIntOfTerm(t1)),dbl));
      }
    default:
      RERROR();
    }
#endif
  default:
    RERROR();
  }
  RERROR();
}

/* next function is adapted from:
   Inline C++ integer exponentiation routines 
   Version 1.01
   Copyright (C) 1999-2004 John C. Bowman <bowman@math.ualberta.ca>
*/
static inline Int
ipow(Int x, Int p)
{
  Int r;

  if (p == 0) return 1L;
  if (x == 0 && p > 0) return 0L;
  if(p < 0) 
    return (-p % 2) ? x : 1L;
	
  r = 1L;
  for(;;) {
    if(p & 1) {
      if (mul_overflow((r*x), r, x)) {
	return 0;
      }
      r *= x;
    }
    if((p >>= 1) == 0)	return r;
    if (mul_overflow((x*x), x, x)) {
      return 0;
    }
    x *= x;
  }
}


/*
  power: x^y
*/
static Term
p_exp(Term t1, Term t2)
{
  switch (ETypeOfTerm(t1)) {
  case long_int_e:
    switch (ETypeOfTerm(t2)) {
    case long_int_e:
      {
	Int i1 = IntegerOfTerm(t1);
	Int i2 = IntegerOfTerm(t2);
	Int pow = ipow(i1,i2);

	if (i2 < 0) {
	  return Yap_ArithError(DOMAIN_ERROR_NOT_LESS_THAN_ZERO, t2,
		    "%d ^ %d", i1, i2);
	}
#ifdef USE_GMP
	/* two integers */
	if ((i1 && !pow)) {
	  /* overflow */
	  return Yap_gmp_exp_ints(i1, i2);
	}
#endif
	RINT(pow);
      }
    case double_e:
      {
	/* integer, double */
	Float fl1 = (Float)IntegerOfTerm(t1);
	Float fl2 = FloatOfTerm(t2);
	RFLOAT(pow(fl1,fl2));
      }
    case big_int_e:
#ifdef USE_GMP
      {
	return Yap_ArithError(RESOURCE_ERROR_HUGE_INT, t2, "^/2");
      }
#endif
    default:
      RERROR();
    }
    break;
  case double_e:
    switch (ETypeOfTerm(t2)) {
    case long_int_e:
      /* float / integer */
      {
	Int i2 = IntegerOfTerm(t2);
	RFLOAT(pow(FloatOfTerm(t1),i2));
      }
    case double_e:
      {
	Float f2 = FloatOfTerm(t2);
	RFLOAT(pow(FloatOfTerm(t1),f2));
      }
    case big_int_e:
#ifdef USE_GMP
      {
	return Yap_ArithError(RESOURCE_ERROR_HUGE_INT, t2, "^/2");
      }
#endif
    default:
      RERROR();
    }
    break;
  case big_int_e:
#ifdef USE_GMP
    switch (ETypeOfTerm(t2)) {
    case long_int_e:
      {
	Int i = IntegerOfTerm(t2);
	return Yap_gmp_exp_big_int(Yap_BigIntOfTerm(t1),i);
      }
    case big_int_e:
      /* two bignums, makes no sense */
      //
      return Yap_ArithError(RESOURCE_ERROR_HUGE_INT, t1, "^/2");
    case double_e:
      {
	Float dbl = FloatOfTerm(t2);
	RFLOAT(pow(mpz_get_d(Yap_BigIntOfTerm(t1)),dbl));
      }
    default:
      RERROR();
    }
#endif
  default:
    RERROR();
  }
  RERROR();
}

static Int
gcd(Int m11,Int m21)
{
  /* Blankinship algorithm, provided by Miguel Filgueiras */
  Int m12=1, m22=0, k;

  while (m11>0 && m21>0)
    if (m11<m21) {
      k = m21/m11;  m21 -= k*m11;  m22 -= k*m12;
    } else {
      k=m11/m21;  m11 -= k*m21;  m12 -= k*m22;
    }
  if (m11<0 || m21<0) {		/* overflow? */
    /*    Oflow = 1; */
    Yap_ArithError(EVALUATION_ERROR_INT_OVERFLOW, MkIntegerTerm(m11),
	      "gcd/2 with %d and %d", m11, m21);
    return(1);
  }
  if (m11)  return(m11);
  return(m21);
}

#ifdef GCD_MULT
Int gcdmult(Int m11,Int m21,Int *pm11)	/* *pm11 gets multiplier of m11 */
{
  Int m12=1, m22=0, k;

  while (m11 && m21)
    if (m11<m21) {
      k = m21/m11;  m21 -= k*m11;  m22 -= k*m12;
    } else {
      k=m11/m21;  m11 -= k*m21;  m12 -= k*m22;
    }
  if (m11<0 || m21<0) {		/* overflow? */
    /*    Oflow = 1; */
    Yap_ArithError(EVALUATION_ERROR_INT_OVERFLOW, MkIntegerTerm(m11),
	      "gcdmult/2 with %d and %d", m11, m21);
    return(1);
  }
  if (m11) {
    *pm11 = m12;  return(m11);
  }
  *pm11 = m22;
  return(m21);
}
#endif

/*
  module gcd
*/
static Term
p_gcd(Term t1, Term t2)
{
  switch (ETypeOfTerm(t1)) {
  case long_int_e:
    switch (ETypeOfTerm(t2)) {
    case long_int_e:
      /* two integers */
      {
	Int i1 = IntegerOfTerm(t1), i2 = IntegerOfTerm(t2);
	i1 = (i1 >= 0 ? i1 : -i1);
	i2 = (i2 >= 0 ? i2 : -i2);
      
	RINT(gcd(i1,i2));
      }
    case double_e:
      return Yap_ArithError(TYPE_ERROR_INTEGER, t2, "gcd/2");
    case big_int_e:
#ifdef USE_GMP
      /* I know the term is much larger, so: */
      {
	Int i = IntegerOfTerm(t1);

	if (i > 0) {
	  RINT(mpz_gcd_ui(NULL,Yap_BigIntOfTerm(t2),i));
	} else if (i == 0) {
	  RINT(0);
	} else {
	  RINT(mpz_gcd_ui(NULL,Yap_BigIntOfTerm(t2),-i));
	}
      }
#endif
    default:
      RERROR();
    }
    break;
  case double_e:
    return Yap_ArithError(TYPE_ERROR_INTEGER, t1, "gcd/2");
  case big_int_e:
#ifdef USE_GMP
    switch (ETypeOfTerm(t2)) {
    case long_int_e:
      /* modulo between bignum and integer */
      {
	Int i = IntegerOfTerm(t2);

	if (i > 0) {
	  RINT(mpz_gcd_ui(NULL,Yap_BigIntOfTerm(t1),i));
	} else if (i == 0) {
	  RINT(0);
	} else {
	  RINT(mpz_gcd_ui(NULL,Yap_BigIntOfTerm(t1),-i));
	}
      }
    case big_int_e:
      /* two bignums */
      {
	MP_INT new;

	mpz_init_set(&new, Yap_BigIntOfTerm(t1));
	mpz_gcd(&new, &new, Yap_BigIntOfTerm(t2));
	RBIG(&new);
      }
    case double_e:
      return Yap_ArithError(TYPE_ERROR_INTEGER, t2, "gcd/2");
    default:
      RERROR();
    }
#endif
  default:
    RERROR();
  }
  RERROR();
}

/*
  minimum: min(x,y)
*/
static Term
p_min(Term t1, Term t2)
{
  switch (ETypeOfTerm(t1)) {
  case long_int_e:
    switch (ETypeOfTerm(t2)) {
    case long_int_e:
      {
	Int i1 = IntegerOfTerm(t1);
	Int i2 = IntegerOfTerm(t2);
	return((i1 < i2 ? t1 : t2));
      }
    case double_e:
      {
	/* integer, double */
	Int i = IntegerOfTerm(t1);
	Float fl = FloatOfTerm(t2);
	if (i <= fl) {
	  return t1;
	}
	return t2;
      }
    case big_int_e:
#ifdef USE_GMP
      {
	Int i = IntegerOfTerm(t1);
	MP_INT *b = Yap_BigIntOfTerm(t2);

	if (mpz_cmp_si(b,i) < 0) {
	  return t2;
	}
	return t1;
      }
#endif
    default:
      RERROR();
    }
    break;
  case double_e:
    switch (ETypeOfTerm(t2)) {
    case long_int_e:
      /* float / integer */
      {
	Int i = IntegerOfTerm(t2);
	Float fl = FloatOfTerm(t1);
	if (i <= fl) {
	  return t2;
	}
	return t1;
      }
    case double_e:
      {
	Float fl1 = FloatOfTerm(t1);
	Float fl2 = FloatOfTerm(t2);
	if (fl1 <= fl2) {
	  return t1;
	}
	return t2;
      }
    case big_int_e:
#ifdef USE_GMP
      {
	Float fl1 = FloatOfTerm(t1);
	Float fl2 = mpz_get_d(Yap_BigIntOfTerm(t2));
	if (fl1 <= fl2) {
	  return t1;
	} else {
	  return t2;
	}
      }
#endif
    default:
      RERROR();
    }
    break;
  case big_int_e:
#ifdef USE_GMP
    switch (ETypeOfTerm(t2)) {
    case long_int_e:
      {
	Int i = IntegerOfTerm(t2);
	MP_INT *b = Yap_BigIntOfTerm(t1);

	if (mpz_cmp_si(b,i) < 0) {
	  return t1;
	}
	return t2;
      }
    case big_int_e:
      /* two bignums */
      {
	MP_INT *b1 = Yap_BigIntOfTerm(t1);
	MP_INT *b2 = Yap_BigIntOfTerm(t2);

	if (mpz_cmp(b1,b2) < 0) {
	  return t1;
	} else {
	  return t2;
	}
      }
    case double_e:
      {
	Float fl1 = FloatOfTerm(t2);
	Float fl2 = mpz_get_d(Yap_BigIntOfTerm(t1));
	if (fl1 <= fl2) {
	  return t2;
	} else {
	  return t1;
	}
      }
    default:
      RERROR();
    }
#endif
  default:
    RERROR();
  }
  RERROR();
}

/*
  maximum: max(x,y)
*/
static Term
p_max(Term t1, Term t2)
{
  switch (ETypeOfTerm(t1)) {
  case long_int_e:
    switch (ETypeOfTerm(t2)) {
    case long_int_e:
      {
	Int i1 = IntegerOfTerm(t1);
	Int i2 = IntegerOfTerm(t2);
	return((i1 > i2 ? t1 : t2));
      }
    case double_e:
      {
	/* integer, double */
	Int i = IntegerOfTerm(t1);
	Float fl = FloatOfTerm(t2);
	if (i >= fl) {
	  return t1;
	}
	return t2;
      }
    case big_int_e:
#ifdef USE_GMP
      {
	Int i = IntegerOfTerm(t1);
	MP_INT *b = Yap_BigIntOfTerm(t2);

	if (mpz_cmp_si(b,i) > 0) {
	  return t2;
	}
	return t1;
      }
#endif
    default:
      RERROR();
    }
    break;
  case double_e:
    switch (ETypeOfTerm(t2)) {
    case long_int_e:
      /* float / integer */
      {
	Int i = IntegerOfTerm(t2);
	Float fl = FloatOfTerm(t1);
	if (i >= fl) {
	  return t2;
	}
	return t1;
      }
    case double_e:
      {
	Float fl1 = FloatOfTerm(t1);
	Float fl2 = FloatOfTerm(t2);
	if (fl1 >= fl2) {
	  return t1;
	}
	return t2;
      }
    case big_int_e:
#ifdef USE_GMP
      {
	Float fl1 = FloatOfTerm(t1);
	Float fl2 = mpz_get_d(Yap_BigIntOfTerm(t2));
	if (fl1 >= fl2) {
	  return t1;
	} else {
	  return t2;
	}
      }
#endif
    default:
      RERROR();
    }
    break;
  case big_int_e:
#ifdef USE_GMP
    switch (ETypeOfTerm(t2)) {
    case long_int_e:
      {
	Int i = IntegerOfTerm(t2);
	MP_INT *b = Yap_BigIntOfTerm(t1);

	if (mpz_cmp_si(b,i) > 0) {
	  return t1;
	}
	return t2;
      }
    case big_int_e:
      /* two bignums */
      {
	MP_INT *b1 = Yap_BigIntOfTerm(t1);
	MP_INT *b2 = Yap_BigIntOfTerm(t2);

	if (mpz_cmp(b1,b2) > 0) {
	  return t1;
	} else {
	  return t2;
	}
      }
    case double_e:
      {
	Float fl1 = FloatOfTerm(t2);
	Float fl2 = mpz_get_d(Yap_BigIntOfTerm(t1));
	if (fl1 >= fl2) {
	  return t2;
	} else {
	  return t1;
	}
      }
    default:
      RERROR();
    }
#endif
  default:
    RERROR();
  }
  RERROR();
}

static Term
eval2(Int fi, Term t1, Term t2) {
  arith2_op f = fi;
  switch (f) {
  case op_plus:
    return p_plus(t1, t2);
  case op_minus:
    return p_minus(t1, t2);
  case op_times:
    return p_times(t1, t2);
  case op_div:
    return p_div(t1, t2);
  case op_and:
    return p_and(t1, t2);
  case op_or:
    return p_or(t1, t2);
  case op_sll:
    return p_sll(t1, t2);
  case op_slr:
    return p_slr(t1, t2);
  case op_mod:
    return p_mod(t1, t2);
  case op_rem:
    return p_rem(t1, t2);
  case op_fdiv:
    return p_fdiv(t1, t2);
  case op_xor:
    return p_xor(t1, t2);
  case op_atan2:
    return p_atan2(t1, t2);
  case op_power:
    return p_exp(t1, t2);
  case op_power2:
    return p_power(t1, t2);
  case op_gcd:
    return p_gcd(t1, t2);
  case op_min:
    return p_min(t1, t2);
  case op_max:
    return p_max(t1, t2);
  }
  RERROR();
}

Term Yap_eval_binary(Int f, Term t1, Term t2)
{
  return eval2(f,t1,t2);
}

static InitBinEntry InitBinTab[] = {
  {"+", op_plus},
  {"-", op_minus},
  {"*", op_times},
  {"/", op_fdiv},
  {"mod", op_mod},
  {"rem", op_rem},
  {"//", op_div},
  {"<<", op_sll},
  {">>", op_slr},
  {"/\\", op_and},
  {"\\/", op_or},
  {"#", op_xor},
  {"><", op_xor},
  {"atan", op_atan2},
  {"atan2", op_atan2},
  /* C-Prolog exponentiation */
  {"^", op_power},
  /* ISO-Prolog exponentiation */
  {"**", op_power2},
  /* Quintus exponentiation */
  {"exp", op_power2},
  {"gcd", op_gcd},
  {"min", op_min},
  {"max", op_max}
};

static Int 
p_binary_is(void)
{				/* X is Y	 */
  Term t = Deref(ARG2);
  Term t1, t2;

  if (IsVarTerm(t)) {
    Yap_ArithError(INSTANTIATION_ERROR,t, "X is Y");
    return(FALSE);
  }
  t1 = Yap_Eval(Deref(ARG3));
  if (!Yap_FoundArithError(t1, ARG3)) {
    return FALSE;
  }
  t2 = Yap_Eval(Deref(ARG4));
  if (!Yap_FoundArithError(t2, ARG4)) {
    return FALSE;
  }
  if (IsIntTerm(t)) {
    Term tout = Yap_FoundArithError(eval2(IntOfTerm(t), t1, t2), 0L);
    if (!tout)
      return FALSE;
    return Yap_unify_constant(ARG1,tout);
  }
  if (IsAtomTerm(t)) {
    Atom name = AtomOfTerm(t);
    ExpEntry *p;
    Term out;

    if (EndOfPAEntr(p = RepExpProp(Yap_GetExpProp(name, 2)))) {
      Term ti[2];

      /* error */
      ti[0] = t;
      ti[1] = MkIntTerm(1);
      t = Yap_MkApplTerm(FunctorSlash, 2, ti);
      Yap_Error(TYPE_ERROR_EVALUABLE, t,
		"functor %s/%d for arithmetic expression",
		RepAtom(name)->StrOfAE,2);
      P = FAILCODE;
      return(FALSE);
    }
    if (!(out=Yap_FoundArithError(eval2(p->FOfEE, t1, t2), 0L)))
      return FALSE;
    return Yap_unify_constant(ARG1,out);
  }
  return FALSE;
}

static Int 
do_arith23(arith2_op op)
{				/* X is Y	 */
  Term t = Deref(ARG1);
  Int out;
  Term t1, t2;

  if (IsVarTerm(t)) {
    Yap_ArithError(INSTANTIATION_ERROR,t, "X is Y");
    return(FALSE);
  }
  t1 = Yap_Eval(t);
  if (t1 == 0L)
    return FALSE;
  t2 = Yap_Eval(Deref(ARG2));
  if (t2 == 0L)
    return FALSE;
  if (!(out=Yap_FoundArithError(eval2(op, t1, t2), 0L)))
    return FALSE;
  return Yap_unify_constant(ARG3,out);
}

static Int 
export_p_plus(void)
{				/* X is Y	 */
  return do_arith23(op_plus);
}

static Int 
export_p_minus(void)
{				/* X is Y	 */
  return do_arith23(op_minus);
}

static Int 
export_p_times(void)
{				/* X is Y	 */
  return do_arith23(op_times);
}

static Int 
export_p_div(void)
{				/* X is Y	 */
  return do_arith23(op_div);
}

static Int 
export_p_and(void)
{				/* X is Y	 */
  return do_arith23(op_and);
}

static Int 
export_p_or(void)
{				/* X is Y	 */
  return do_arith23(op_or);
}

static Int 
export_p_slr(void)
{				/* X is Y	 */
  return do_arith23(op_slr);
}

static Int 
export_p_sll(void)
{				/* X is Y	 */
  return do_arith23(op_sll);
}

static Int 
p_binary_op_as_integer(void)
{				/* X is Y	 */
  Term t = Deref(ARG1);

  if (IsVarTerm(t)) {
    Yap_Error(INSTANTIATION_ERROR,t, "X is Y");
    return(FALSE);
  }
  if (IsIntTerm(t)) {
    return Yap_unify_constant(ARG2,t);
  }
  if (IsAtomTerm(t)) {
    Atom name = AtomOfTerm(t);
    ExpEntry *p;

    if (EndOfPAEntr(p = RepExpProp(Yap_GetExpProp(name, 2)))) {
      return Yap_unify(ARG1,ARG2);
    }
    return Yap_unify_constant(ARG2,MkIntTerm(p->FOfEE));
  }
  return(FALSE);
}


void
Yap_InitBinaryExps(void)
{
  unsigned int    i;
  ExpEntry       *p;

  for (i = 0; i < sizeof(InitBinTab)/sizeof(InitBinEntry); ++i) {
    AtomEntry *ae = RepAtom(Yap_LookupAtom(InitBinTab[i].OpName));
    if (ae == NULL) {
      Yap_Error(OUT_OF_HEAP_ERROR,TermNil,"at InitBinaryExps");
      return;
    }
    WRITE_LOCK(ae->ARWLock);
    if (Yap_GetExpPropHavingLock(ae, 2)) {
      WRITE_UNLOCK(ae->ARWLock);
      break;
    }
    p = (ExpEntry *) Yap_AllocAtomSpace(sizeof(ExpEntry));
    p->KindOfPE = ExpProperty;
    p->ArityOfEE = 2;
    p->ENoOfEE = 2;
    p->FOfEE = InitBinTab[i].f;
    p->NextOfPE = ae->PropsOfAE;
    ae->PropsOfAE = AbsExpProp(p);
    WRITE_UNLOCK(ae->ARWLock);
  }
  Yap_InitCPred("is", 4, p_binary_is, TestPredFlag | SafePredFlag);
  Yap_InitCPred("$binary_op_as_integer", 2, p_binary_op_as_integer, TestPredFlag|SafePredFlag);
  Yap_InitAsmPred("$plus", 3, _plus, export_p_plus, SafePredFlag);
  Yap_InitAsmPred("$minus", 3, _minus, export_p_minus, SafePredFlag);
  Yap_InitAsmPred("$times", 3, _times, export_p_times, SafePredFlag);
  Yap_InitAsmPred("$div", 3, _div, export_p_div, SafePredFlag);
  Yap_InitAsmPred("$and", 3, _and, export_p_and, SafePredFlag);
  Yap_InitAsmPred("$or", 3, _or, export_p_or, SafePredFlag);
  Yap_InitAsmPred("$sll", 3, _sll, export_p_sll, SafePredFlag);
  Yap_InitAsmPred("$slr", 3, _slr, export_p_slr, SafePredFlag);
}

/* This routine is called from Restore to make sure we have the same arithmetic operators */
int
Yap_ReInitBinaryExps(void)
{
  return(TRUE);
}

