/*************************************************************************
 *									 *
 *	 YAP Prolog    @(#)amidefs.h	1.3 3/15/90                      *
 *									 *
 *	Yap Prolog was developed at NCCUP - Universidade do Porto	 *
 *									 *
 * Copyright L.Damas, V.S.Costa and Universidade do Porto 1985-1997	 *
 *									 *
 **************************************************************************
 *									 *
 * File:		amidefs.h						 *
 * comments:	Abstract machine peculiarities				 *
 *									 *
 * Last rev:     $Date: 2008-07-22 23:34:49 $							 *
 * $Log: not supported by cvs2svn $
 * Revision 1.33  2007/11/26 23:43:09  vsc
 * fixes to support threads and assert correctly, even if inefficiently.
 *
 * Revision 1.32  2006/10/10 14:08:17  vsc
 * small fixes on threaded implementation.
 *
 * Revision 1.31  2006/09/20 20:03:51  vsc
 * improve indexing on floats
 * fix sending large lists to DB
 *
 * Revision 1.30  2005/12/17 03:25:39  vsc
 * major changes to support online event-based profiling
 * improve error discovery and restart on scanner.
 *
 * Revision 1.29  2005/07/06 15:10:15  vsc
 * improvements to compiler: merged instructions and fixes for ->
 *
 * Revision 1.28  2005/05/30 06:07:35  vsc
 * changes to support more tagging schemes from tabulation.
 *
 * Revision 1.27  2005/04/10 04:01:13  vsc
 * bug fixes, I hope!
 *
 * Revision 1.26  2004/09/30 21:37:41  vsc
 * fixes for thread support
 *
 * Revision 1.25  2004/09/27 20:45:04  vsc
 * Mega clauses
 * Fixes to sizeof(expand_clauses) which was being overestimated
 * Fixes to profiling+indexing
 * Fixes to reallocation of memory after restoring
 * Make sure all clauses, even for C, end in _Ystop
 * Don't reuse space for Streams
 * Fix Stream_F on StreaNo+1
 *
 * Revision 1.24  2004/04/14 19:10:40  vsc
 * expand_clauses: keep a list of clauses to expand
 * fix new trail scheme for multi-assignment variables
 *
 * Revision 1.23  2004/03/31 01:03:10  vsc
 * support expand group of clauses
 *
 * Revision 1.22  2004/03/10 14:59:55  vsc
 * optimise -> for type tests
 *									 *
 *									 *
 *************************************************************************/

#ifndef NULL
#include <stdio.h>
#endif 

#ifdef FROZEN_STACKS
#ifdef SBA
#define PROTECT_FROZEN_H(CPTR)                                  \
       ((Unsigned((Int)((CPTR)->cp_h)-(Int)(H_FZ)) <            \
	 Unsigned((Int)(B_FZ)-(Int)(H_FZ))) ?                   \
	(CPTR)->cp_h : H_FZ)
#define PROTECT_FROZEN_B(CPTR)                                  \
       ((Unsigned((Int)(CPTR)-(Int)(H_FZ)) <                    \
	 Unsigned((Int)(B_FZ)-(Int)(H_FZ)))  ?                  \
	(CPTR) : B_FZ)
	 /*
#define PROTECT_FROZEN_H(CPTR) ((CPTR)->cp_h > H_FZ && (CPTR)->cp_h < (CELL *)B_FZ ? (CPTR)->cp_h : H_FZ )

#define PROTECT_FROZEN_B(CPTR)  ((CPTR) < B_FZ && (CPTR) > (choiceptr)H_FZ ? (CPTR) : B_FZ )
	 */
#else /* TABLING */
#define PROTECT_FROZEN_B(CPTR)  (YOUNGER_CP(CPTR, B_FZ) ? CPTR        : B_FZ)
#define PROTECT_FROZEN_H(CPTR)  (((CPTR)->cp_h > H_FZ) ? (CPTR)->cp_h : H_FZ)
#endif /* SBA */
#else
#define PROTECT_FROZEN_B(CPTR)  (CPTR)
#define PROTECT_FROZEN_H(CPTR)  (CPTR)->cp_h
#endif /* FROZEN_STACKS */

#if ALIGN_LONGS
/*   */ typedef Int DISPREG;
/*   */ typedef CELL SMALLUNSGN;
/*   */ typedef Int  OPREG;
/*   */ typedef CELL UOPREG;

#else
/*   */ typedef Short DISPREG;
/*   */ typedef BITS16 SMALLUNSGN;
/*   */ typedef SBITS16 OPREG;
/*   */ typedef SBITS16 UOPREG;

#endif


typedef Int (*CPredicate)(void);

typedef Int (*CmpPredicate)(Term, Term);


#define OpRegSize    sizeof(OPREG)

/*
  Possible arguments to YAP emulator:
  wamreg describes an A or X register;
  yslot describes an Y slot
  COUNT is a small number (eg, number of arguments to a choicepoint,
  number of permanent variables in a environment
*/

typedef OPREG  wamreg;
typedef OPREG  yslot;
typedef OPREG  COUNT;


/*
  This is a table with the codes for YAP instructions
*/
typedef enum {
#define OPCODE(OP,TYPE) _##OP
#include "YapOpcodes.h"
#undef  OPCODE
} op_numbers;

#define _std_top	_or_last

/* use similar trick for keeping instruction names */
#if defined(ANALYST) || defined(DEBUG)
extern char *Yap_op_names[_std_top + 1];
#endif

typedef enum {
  _atom,
  _atomic,
  _integer,
  _compound,
  _float,
  _nonvar,
  _number,
  _var,
  _cut_by,
  _db_ref,
  _primitive,
  _dif,
  _eq,
  _equal,
  _plus,
  _minus,
  _times,
  _div,
  _and,
  _or,
  _sll,
  _slr,
  _arg,
  _functor,
  _p_put_fi,
  _p_put_i,
  _p_put_f,
  _p_a_eq_float,
  _p_a_eq_int,
  _p_a_eq,
  _p_ltc_float,
  _p_ltc_int,
  _p_lt,
  _p_gtc_float,
  _p_gtc_int,
  _p_get_fi,
  _p_get_i,
  _p_get_f,
  _p_add_float_c,
  _p_add_int_c,
  _p_add,
  _p_sub_float_c,
  _p_sub_int_c,
  _p_sub,
  _p_mul_float_c,
  _p_mul_int_c,
  _p_mul,
  _p_fdiv_c1,
  _p_fdiv_c2,
  _p_fdiv,
  _p_idiv_c1,
  _p_idiv_c2,
  _p_idiv,
  _p_mod_c1,
  _p_mod_c2,
  _p_mod,
  _p_rem_c1,
  _p_rem_c2,
  _p_rem,
  _p_land_c,
  _p_land,
  _p_lor_c,
  _p_lor,
  _p_xor_c,
  _p_xor,
  _p_uminus,
  _p_sr_c1,
  _p_sr_c2,
  _p_sr,
  _p_sl_c1,
  _p_sl_c2,
  _p_sl,
  _p_label_ctl
} basic_preds;

#if USE_THREADED_CODE

#if ALIGN_LONGS
/*   */ typedef CELL OPCODE;
#else

#if LOW_ABSMI
/*   */ typedef BITS16 OPCODE;
#else
/*   */ typedef CELL OPCODE;
#endif
#endif /* ALIGN_LONGS */
#else /* if does not USE_THREADED_CODE */
/*   */ typedef op_numbers OPCODE;
#endif
#define OpCodeSize   sizeof(OPCODE)


/*

  Types of possible YAAM instructions.

  The meaning and type of the symbols in a abstract machine instruction is:

  b: arity (Int)
  b: bitmap (CELL *)
  c: constant, is a Term
  d: double (functor + unaligned double)
  f: functor
  F: Function, CPredicate
  i: large integer (functor + long)
  I: logic upd index (struct logic_upd_index *)
  l: label, yamop *
  L: logic upd clause, logic_upd_clause *
  m: module, Term
  n: number, Integer
  o: opcode, OPCODE
  O: OR-parallel information, used by YAPOR, unsigned int
  p: predicate, struct pred_entry *
  s: small integer, COUNT
  t: pointer to table entry, used by yaptab, struct table_entry *
  x: wam register, wamreg
  y: environment slot

*/
/* This declaration is going to be parsed by a Prolog program, so:
   comments are welcome, but they should take a whole line,
   every field declaration should also take a single line,
   please check the Prolog program if you come up with a complicated C-type that does not start by unsigned or struct.
*/
typedef struct yami {
  OPCODE opc;
  union {
    struct {
      CELL next;
    } e;
    struct {
      Term                c;
      CELL next;
    } c;
    struct {
      Term                c1;
      Term                c2;
      CELL next;
    } cc;
    struct {
      Term                c1;
      Term                c2;
      Term                c3;
      CELL next;
    } ccc;
    struct {
      Term                c1;
      Term                c2;
      Term                c3;
      Term                c4;
      CELL next;
    } cccc;
    struct {
      Term                c1;
      Term                c2;
      Term                c3;
      Term                c4;
      Term                c5;
      CELL next;
    } ccccc;
    struct {
      Term                c1;
      Term                c2;
      Term                c3;
      Term                c4;
      Term                c5;
      Term                c6;
      CELL next;
    } cccccc;
    struct {
      Term                c;
      struct yami        *l1;
      struct yami        *l2;
      struct yami        *l3;
      CELL next;
    } clll;
    struct {
      CELL    d[1+SIZEOF_DOUBLE/SIZEOF_INT_P];
      CELL next;
    } d;
    struct {
      struct logic_upd_clause *ClBase;
      CELL  next;
    } L;
    struct {
      Functor             f;
      Int                 a;
      CELL next;
    } fa;
    struct {
      CELL    i[2];
      CELL next;
    } i;
    struct {
      struct logic_upd_index  *I;
      struct yami             *l1;
      struct yami             *l2;
      COUNT                    s;
      CELL next;
    } Ills;
    struct {
      struct yami   *l;
      CELL next;
    } l;
    struct {
#ifdef YAPOR
      unsigned int        or_arg;
#endif /* YAPOR */
#ifdef TABLING
      struct table_entry *te; /* pointer to table entry */
#endif /* TABLING */
      Int               s;
      struct pred_entry  *p;
      struct yami              *d;
      CELL next;
    } Otapl;
    struct {
      /* call counter */
      COUNT                n;
      /* native code pointer */
      CPredicate           native;
      /* next instruction to execute after native code if the predicate was not fully compiled */
      struct yami *native_next;
      /* Pointer to pred */
      struct pred_entry   *p;
      CELL next;              
    } aFlp;
    /* The next two instructions are twin: they both correspond to the old ldd. */
    /* The first one, aLl, handles try_logical and retry_logical, */
    /* Ill handles trust_logical. */
    /* They must have the same fields. */
    
    struct {
#ifdef YAPOR
      unsigned int               or_arg;
#endif /* YAPOR */
#ifdef TABLING
      /* pointer to table entry */
      struct table_entry        *te;
#endif
      /* number of arguments */
      COUNT                    s;
      struct logic_upd_clause   *d;
      struct yami               *n;
      CELL                       next;
    } OtaLl;
    struct {
#ifdef YAPOR
      unsigned int               or_arg;
#endif
#ifdef TABLING
      /* pointer to table entry */
      struct table_entry        *te;
#endif /* TABLING */
      /* number of arguments */
      struct logic_upd_index  *block;
      struct logic_upd_clause   *d;
      struct yami               *n;
      CELL                       next;
    } OtILl;
    struct {
#ifdef YAPOR
      unsigned int        or_arg;
#endif
#ifdef TABLING
       /* pointer to table entry */
      struct table_entry *te;
#endif
      Int               s;
      struct pred_entry  *p;
      CPredicate          f;
      COUNT               extra;
      CELL next;
    } OtapFs;
    struct {
      struct yami               *l1;
      struct yami               *l2;
      struct yami               *l3;
      CELL next;
    } lll;
    struct {
      struct yami               *l1;
      struct yami               *l2;
      struct yami               *l3;
      struct yami               *l4;
      CELL next;
    } llll;
    struct {
      wamreg                     x;
      struct yami               *l1;
      struct yami               *l2;
      struct yami               *l3;
      struct yami               *l4;
      CELL next;
    } xllll;
    struct {
      COUNT                      s;
      struct yami               *l1;
      struct yami               *l2;
      struct yami               *l3;
      struct yami               *l4;
      CELL next;
    } sllll;
    struct {
      struct pred_entry    *p;
      struct yami          *f;
      wamreg                x1;
      wamreg                x2;
      COUNT                flags;
      CELL next;
    } plxxs;
    struct {
      struct pred_entry    *p;
      struct yami          *f;
      wamreg                x;
      yslot                 y;
      COUNT                 flags;
      CELL next;
    } plxys;
    struct {
      struct pred_entry    *p;
      struct yami          *f;
      wamreg                y1;
      yslot                 y2;
      COUNT                 flags;
      CELL next;
    } plyys;
    struct {
      OPCODE              pop;
      struct yami               *l1;
      struct yami               *l2;
      struct yami               *l3;
      struct yami               *l4;
      CELL next;
    } ollll;
    struct {
      struct yami        *l;
      struct pred_entry  *p;
      CELL next;
    } lp;
    struct {
      OPCODE              opcw;
      CELL next;
    } o;
    struct {
      OPCODE              opcw;
      Term                c;
      CELL next;
    } oc;
    struct {
      OPCODE              opcw;
      CELL    d[1+SIZEOF_DOUBLE/SIZEOF_INT_P];
      CELL next;
    } od;
    struct {
      OPCODE              opcw;
      Functor             f;
      Int                 a;
      CELL next;
    } ofa;
    struct {
      OPCODE              opcw;
      CELL		     i[2];
      CELL next;
    } oi;
    struct {
      OPCODE              opcw;
      COUNT               s;
      CELL                c;
      CELL next;
    } osc;
    struct {
      OPCODE              opcw;
      COUNT               s;
      CELL next;
    } os;
    struct {
      OPCODE              opcw;
      wamreg                x;
      CELL next;
    } ox;
    struct {
      OPCODE              opcw;
      wamreg                xl;
      wamreg                xr;
      CELL next;
    } oxx;
    struct {
      OPCODE              opcw;
      yslot                y;
      CELL next;
    } oy;
    struct {
      struct pred_entry   *p;
      CELL next;
    } p;
    struct {
      struct pred_entry   *p;
      struct pred_entry   *p0;
      CELL next;
    } pp;
    struct {
      COUNT               s;
      CELL next;
    } s;
    /* format of expand_clauses */
    struct {
      COUNT               s1;
      COUNT               s2;
      COUNT               s3;
      struct yami  *sprev;
      struct yami  *snext;
      struct pred_entry  *p;
      CELL next;
    } sssllp;
    struct {
      COUNT               s;
      CELL                c;
      CELL next;
    } sc;
    struct {
      COUNT               s;
      CELL    d[1+SIZEOF_DOUBLE/SIZEOF_INT_P];
      struct yami        *F;
      struct yami        *T;
      CELL next;
    } sdll;
    struct {
      COUNT               s;
      struct yami        *l;
      struct pred_entry  *p;
      CELL next;
    } slp;
    struct {
      COUNT               s;
      Int                 I;
      struct yami        *F;
      struct yami        *T;
      CELL next;
    } snll;
    struct {
      COUNT               s0;
      COUNT               s1;
      CELL    d[1+SIZEOF_DOUBLE/SIZEOF_INT_P];
      CELL next;
    } ssd;
    struct {
      COUNT               s0;
      COUNT               s1;
      Int		  n;
      CELL next;
    } ssn;
    struct {
      COUNT               s0;
      COUNT               s1;
      COUNT               s2;
      CELL next;
    } sss;
    struct {
      COUNT               s1;
      COUNT               s2;
      struct yami        *F;
      struct yami        *T;
      CELL next;
    } ssll;
    struct {
      COUNT               s;
      wamreg              x;
      struct yami        *l;
      CELL next;
    } sxl;
    struct {
      COUNT               s;
      wamreg              x;
      struct yami        *F;
      struct yami        *T;
      CELL next;
    } sxll;
    struct {
      COUNT               s;
      yslot               y;
      struct yami        *l;
      CELL next;
    } syl;
    struct {
      COUNT               s;
      yslot               y;
      struct yami        *F;
      struct yami        *T;
      CELL next;
    } syll;
    /* the next 3 instructions must have same size and have fields in same order! */
    /* also check env for yes and trustfail code before making any changes */
    /* last, Osblp is known to the buildops script */
    struct {
#ifdef YAPOR
      unsigned int        or_arg;
#endif
      COUNT               s;
      CELL               *bmap;
      struct yami *l;
      struct pred_entry  *p0;
      CELL next;
    } Osblp;
    struct {
#ifdef YAPOR
      unsigned int        or_arg;
#endif
      COUNT               s;
      CELL               *bmap;
      struct pred_entry  *p;
      struct pred_entry  *p0;
      CELL next;
    } Osbpp;
    struct {
#ifdef YAPOR
      unsigned int        or_arg;
#endif
      COUNT               s;
      CELL               *bmap;
      Term  mod;
      struct pred_entry  *p0;
      CELL next;
    } Osbmp;
    struct {
      /* size of table */
      COUNT               s;
      /* live entries */
      COUNT               e;
      /* pending suspended blocks */
      COUNT               w;
      struct yami        *l;
      CELL next;
    } sssl;
    struct {
      wamreg                x;
      CELL next;
    } x;
    struct {
      wamreg                x;
      struct pred_entry    *p0;
      CELL next;
    } xp;
    struct {
      wamreg                x;
      CELL                  c;
      CELL next;
    } xc;
    struct {
      wamreg                x;
      CELL    d[1+SIZEOF_DOUBLE/SIZEOF_INT_P];
      CELL next;
    } xd;
    struct {
      wamreg                x;
      Functor             f;
      Int                 a;
      CELL next;
    } xfa;
    struct {
      wamreg                x;
      struct yami          *F;
      CELL next;
    } xl;
    struct {
      wamreg                x;
      CELL    i[2];
      CELL next;
    } xi;
    struct {
      wamreg                x;
      struct yami	       *l1;
      struct yami	       *l2;
      CELL next;
    } xll; 
    struct {
      wamreg                xl;
      wamreg                xr;
      CELL next;
    } xx;
    struct {
      wamreg                x;
      wamreg                xi;
      Term                  c;
      CELL next;
    } xxc;
    struct {
      wamreg                x;
      wamreg                xi;
      Int                   c;
      CELL next;
    } xxn;
    struct {
      wamreg                x;
      wamreg                x1;
      wamreg                x2;
      CELL next;
    } xxx;
    struct {
      wamreg                xl1;
      wamreg                xl2;
      wamreg                xr1;
      wamreg                xr2;
      CELL next;
    } xxxx;
    struct {
      wamreg                x;
      wamreg                x1;
      yslot                y2;
      CELL next;
    } xxy;
    struct {
      yslot                y;
      CELL next;
    } y;
    struct {
      yslot                y;
      struct pred_entry   *p0;
      CELL next;
    } yp;
    struct {
      yslot                y;
      struct yami         *F;
      CELL next;
    } yl;
    struct {
      yslot                y;
      wamreg                x;
      CELL next;
    } yx;
    struct {
      yslot                y;
      wamreg                x1;
      wamreg                x2;
      CELL next;
    } yxx;
    struct {
      yslot                y1;
      yslot                y2;
      wamreg                x;
      CELL next;
    } yyx;
    struct {
      yslot                y1;
      yslot                y2;
      wamreg               x1;
      wamreg               x2;
      CELL next;
    } yyxx;
    struct {
      yslot                y;
      yslot                y1;
      yslot                y2;
      CELL next;
    } yyy;
    struct {
      yslot                y;
      wamreg               xi;
      Int                  c;
      CELL next;
    } yxn;
  } u;
} yamop;

typedef yamop yamopp;

#define OPCR                opc
#define OPCW                u.ox.opcw


#define NEXTOP(V,TYPE)    ((yamop *)(&((V)->u.TYPE.next)))

#define PREVOP(V,TYPE)    ((yamop *)((CODEADDR)(V)-(CELL)NEXTOP((yamop *)NULL,TYPE)))

#if defined(TABLING) || defined(SBA)
typedef struct trail_frame {
  Term term;
  CELL value;
} *tr_fr_ptr;

#define TrailTerm(X)   ((X)->term)
#define TrailVal(X)    ((X)->value)
#else
typedef Term *tr_fr_ptr;

#define TrailTerm(X)   (*(X))
#define TrailVal(X)    OOOOOOPS: this program should not compile
#endif /* TABLING || SBA  */


/*
  Choice Point Structure

  6 fixed fields (TR,AP,H,B,ENV,CP) plus arguments
*/

#ifdef DETERMINISTIC_TABLING
struct deterministic_choicept {
  yamop *cp_ap;
  struct choicept *cp_b;
  tr_fr_ptr cp_tr;  
#ifdef DEPTH_LIMIT
  CELL cp_depth;
#endif /* DEPTH_LIMIT */
#ifdef YAPOR
  int cp_lub;           /* local untried branches */
  struct or_frame *cp_or_fr;  /* or-frame pointer */
#endif /* YAPOR */
  CELL *cp_h;  /* necessary, otherwise we get in trouble */
};

typedef struct choicept {
  yamop *cp_ap;
  struct choicept *cp_b;
  tr_fr_ptr cp_tr;  
#ifdef DEPTH_LIMIT
  CELL cp_depth;
#endif /* DEPTH_LIMIT */
#ifdef YAPOR
  int cp_lub;           /* local untried branches */
  struct or_frame *cp_or_fr;  /* or-frame pointer */
#endif /* YAPOR */
  CELL *cp_h;
  yamop *cp_cp;
#else
typedef struct choicept {
  tr_fr_ptr cp_tr;
  CELL *cp_h;
  struct choicept *cp_b;
#ifdef DEPTH_LIMIT
  CELL cp_depth;
#endif /* DEPTH_LIMIT */
  yamop *cp_cp;
#ifdef YAPOR
  int cp_lub;           /* local untried branches */
  struct or_frame *cp_or_fr;  /* or-frame pointer */
#endif /* YAPOR */
  yamop *cp_ap;
#endif /* DETERMINISTIC_TABLING */
#if MIN_ARRAY == 0
  CELL *cp_env;
  /* GNUCC understands empty arrays */
  CELL cp_args[MIN_ARRAY];
#define cp_a1		cp_args[0]
#define cp_a2		cp_args[1]
#define cp_a3		cp_args[2]
#define cp_a4		cp_args[3]
#define cp_a5		cp_args[4]
#define cp_a6		cp_args[5]
#define cp_a7		cp_args[6]
#define cp_a8		cp_args[7]
#define EXTRA_CBACK_ARG(Arity,Offset)  B->cp_args[(Arity)+(Offset)-1]
#else
  /* Otherwise, we need a very dirty trick to access the arguments */
  union {
    CELL *cp_uenv;
    CELL  cp_args[1];
  } cp_last;
#define cp_env		cp_last.cp_uenv
#define cp_a1		cp_last.cp_args[1]
#define cp_a2		cp_last.cp_args[2]
#define cp_a3		cp_last.cp_args[3]
#define cp_a4		cp_last.cp_args[4]
#define cp_a5		cp_last.cp_args[5]
#define cp_a6		cp_last.cp_args[6]
#define cp_a7		cp_last.cp_args[7]
#define cp_a8		cp_last.cp_args[8]
#define EXTRA_CBACK_ARG(Arity,Offset)  B->cp_last.cp_args[(Arity)+(Offset)]
#endif
} *choiceptr;

/* This has problems with \+ \+ a, !, b. */
#define SHOULD_CUT_UP_TO(X,Y)  ((X) != (Y))
/* #define SHOULD_CUT_UP_TO(X,Y)  ((X)  (Y)) */

#ifdef SBA
#define SHARED_CP(CP)  ((CP) >= B_FZ || (CP) < (choiceptr)H_FZ)

#define YOUNGER_CP(CP1, CP2)						\
  (SHARED_CP(CP1) ?							\
   (SHARED_CP(CP2) ? OrFr_depth((CP1)->cp_or_fr) > OrFr_depth((CP2)->cp_or_fr) : FALSE)	\
   :									\
   (SHARED_CP(CP2) ? TRUE : CP1 < CP2)					\
   )

#define EQUAL_OR_YOUNGER_CP(CP1, CP2)					\
  (SHARED_CP(CP1) ?							\
   (SHARED_CP(CP2) ? OrFr_depth((CP1)->cp_or_fr) >= OrFr_depth((CP2)->cp_or_fr) : FALSE) \
   :									\
   (SHARED_CP(CP2) ? TRUE : CP1 <= CP2)					\
   )

#define YOUNGER_H(H1, H2) FIXMEE!!!!


#else /* ENV_COPY || ACOW */
#define YOUNGER_CP(CP1, CP2)           ((CP1) <  (CP2))
#define EQUAL_OR_YOUNGER_CP(CP1, CP2)  ((CP1) <= (CP2))

#define YOUNGER_H(H1, H2)           ((CELL *)(H1) > (CELL *)(H2))

#endif /* SBA */

#define YOUNGEST_CP(CP1, CP2)           (YOUNGER_CP(CP1,CP2) ? (CP1) : (CP2))

#define YOUNGEST_H(H1, H2)           (YOUNGER_H(H1,H2) ? (CELL *)(H1) : (CELL *)(H2))



/*
  Environment Structure (CP, E, and CUT_B). Yap always saves the B
  where to cut to, even if not needed.
*/
#define E_CP		-1
#define E_E		-2
#define E_CB		-3
#ifdef TABLING
#define E_B		-4
#ifdef  DEPTH_LIMIT
#define E_DEPTH         -5
#define EnvSizeInCells   5
#else
#define EnvSizeInCells   4
#endif  /* DEPTH_LIMIT */
#else   /* TABLING */
#ifdef  DEPTH_LIMIT
#define E_DEPTH         -4
#define EnvSizeInCells   4
#else
#define EnvSizeInCells   3
#endif  /* DEPTH_LIMIT */
#endif  /* TABLING */

#if MSHIFTOFFS
#define FixedEnvSize		EnvSizeInCells
#else
#define FixedEnvSize		(EnvSizeInCells*sizeof(CELL))
#endif
#define RealEnvSize	(EnvSizeInCells*sizeof(CELL))

static inline
CELL *ENV_Parent(CELL *env)
{
  return (CELL *)env[E_E];
}

static inline 
UInt ENV_Size(yamop *cp)
{
  return (((yamop *)((CODEADDR)(cp) - (CELL)NEXTOP((yamop *)NULL,Osbpp)))->u.Osbpp.s);
}

static inline 
struct pred_entry *ENV_ToP(yamop *cp)
{
  return (((yamop *)((CODEADDR)(cp) - (CELL)NEXTOP((yamop *)NULL,Osbpp)))->u.Osbpp.p);
}

static inline 
OPCODE ENV_ToOp(yamop *cp)
{
  return (((yamop *)((CODEADDR)(cp) - (CELL)NEXTOP((yamop *)NULL,Osbpp)))->opc);
}

static inline 
UInt EnvSize(yamop *cp)
{
  return ((-ENV_Size(cp))/(OPREG)sizeof(CELL));
}

static inline 
CELL *EnvBMap(yamop *p)
{
  return (((yamop *)((CODEADDR)(p) - (CELL)NEXTOP((yamop *)NULL,Osbpp)))->u.Osbpp.bmap);
}

static inline 
struct pred_entry *EnvPreg(yamop *p)
{
  return (((yamop *)((CODEADDR)(p) - (CELL)NEXTOP((yamop *)NULL,Osbpp)))->u.Osbpp.p0);
}

/* access to instructions */

#if USE_THREADED_CODE
extern void **Yap_ABSMI_OPCODES;

#define absmadr(i) ((OPCODE)(Yap_ABSMI_OPCODES[(i)]))
#else
#define absmadr(i) ((OPCODE)(i))
#endif

/* used to find out how many instructions of each kind are executed */
#ifdef ANALYST
extern YAP_ULONG_LONG Yap_opcount[_std_top + 1];

extern YAP_ULONG_LONG Yap_2opcount[_std_top + 1][_std_top + 1];
#endif /* ANALYST */

#if DEPTH_LIMIT
/*
  Make this into an even number so that the system will know
  it should ignore the depth limit
*/   
#define RESET_DEPTH() DEPTH = MkIntTerm(MAX_ABS_INT-1)
#else

#endif


