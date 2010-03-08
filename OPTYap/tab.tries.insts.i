/**********************************************************************
                                                               
                       The OPTYap Prolog system                
  OPTYap extends the Yap Prolog system to support or-parallel tabling
                                                               
  Copyright:   R. Rocha and NCC - University of Porto, Portugal
  File:        tab.tries.insts.i
  version:     $Id: tab.tries.insts.i,v 1.12 2007-04-26 14:11:08 ricroc Exp $   
                                                                     
**********************************************************************/

/* --------------------------------------------------------- **
**      Trie instructions: auxiliary stack organization      **
** --------------------------------------------------------- **
                 STANDARD_TRIE
              -------------------
              | ha = heap_arity | 
              -------------------  --
              |   heap ptr 1    |    |
              -------------------    |
              |       ...       |    -- heap_arity
              -------------------    |
              |   heap ptr ha   |    |
              -------------------  --
              | va = vars_arity |
              -------------------
              | sa = subs_arity |
              -------------------  --
              |   subs ptr sa   |    |
              -------------------    |
              |       ...       |    -- subs_arity 
              -------------------    |
              |   subs ptr 1    |    |
              -------------------  --
              |    var ptr va   |    |
              -------------------    |
              |       ...       |    -- vars_arity
              -------------------    |
              |    var ptr 1    |    |
              -------------------  -- 


                  GLOBAL_TRIE
              -------------------
              | va = vars_arity |
              -------------------  --
              |    var ptr va   |    |
              -------------------    |
              |       ...       |    -- vars_arity
              -------------------    |
              |    var ptr 1    |    |
              -------------------  -- 
              | sa = subs_arity |
              -------------------  --
              |   subs ptr sa   |    |
              -------------------    |
              |       ...       |    -- subs_arity 
              -------------------    |
              |   subs ptr 1    |    |
              -------------------  --
** --------------------------------------------------------- */



/* --------------------------------------------- **
**      Trie instructions: auxiliary macros      **
** --------------------------------------------- */

#ifdef GLOBAL_TRIE
#define copy_arity_stack()                                      \
        { int size = subs_arity + vars_arity + 2;               \
          YENV -= size;                                         \
          memcpy(YENV, aux_stack_ptr, size * sizeof(CELL *));   \
          aux_stack_ptr = YENV;                                 \
	}
#else

#define CALCULATE_STACK_SIZE() (heap_arity + subs_arity + vars_arity + 3)
#define copy_arity_stack()                                      \
  {       int size = CALCULATE_STACK_SIZE();                    \
          YENV -= size;                                         \
          memcpy(YENV, aux_stack_ptr, size * sizeof(CELL *));   \
          aux_stack_ptr = YENV;                                 \
	}
#endif /* GLOBAL_TRIE */

#define next_trie_instruction(NODE) \
        next_node_instruction(TrNode_child(NODE))
        
#define next_node_instruction(NODE) {                           \
        PREG = (yamop *)(NODE);                                 \
        PREFETCH_OP(PREG);                                      \
        GONext();                                               \
      }

#define next_instruction(CONDITION, NODE)                       \
        if (CONDITION) {                                        \
          PREG = (yamop *) TrNode_child(NODE);                  \
        } else {                                                \
          /* procceed */                                        \
	        PREG = (yamop *) CPREG;                               \
	        YENV = ENV;                                           \
        }                                                       \
        PREFETCH_OP(PREG);                                      \
        GONext()



/* ---------------------------------------------------------------------------- **
** the 'store_trie_node', 'restore_trie_node' and 'pop_trie_node' macros do not **
** include the 'set_cut' macro because there are no cuts in trie instructions.  **
** ---------------------------------------------------------------------------- */
        
#define store_trie_node(AP)                           \
        { register choiceptr cp;                      \
          dprintf("store_trie_node\n");                \
          YENV = (CELL *) (NORM_CP(YENV) - 1);        \
          cp = NORM_CP(YENV);                         \
          HBREG = H;                                  \
          store_yaam_reg_cpdepth(cp);                 \
          cp->cp_tr = TR;                             \
          cp->cp_h  = H;                              \
          cp->cp_b  = B;                              \
          cp->cp_cp = CPREG;                          \
          cp->cp_ap = (yamop *) AP;                   \
          cp->cp_env= ENV;                            \
          B = cp;                                     \
          YAPOR_SET_LOAD(B);                          \
          SET_BB(B);                                  \
          TABLING_ERRORS_check_stack;                 \
	      }                                             \
        copy_arity_stack()

#define restore_trie_node(AP)                         \
        dprintf("restore_trie_node\n");                \
        H = HBREG = PROTECT_FROZEN_H(B);              \
        restore_yaam_reg_cpdepth(B);                  \
        CPREG = B->cp_cp;                             \
        ENV = B->cp_env;                              \
        YAPOR_update_alternative(PREG, (yamop *) AP)  \
        B->cp_ap = (yamop *) AP;                      \
        YENV = (CELL *) PROTECT_FROZEN_B(B);          \
        SET_BB(NORM_CP(YENV));                        \
        copy_arity_stack()
        
#define really_pop_trie_node()                        \
        dprintf("really_pop_trie_node\n");            \
        YENV = (CELL *) PROTECT_FROZEN_B((B + 1));    \
        H = PROTECT_FROZEN_H(B);                      \
        pop_yaam_reg_cpdepth(B);                      \
	      CPREG = B->cp_cp;                             \
        TABLING_close_alt(B);                         \
        ENV = B->cp_env;                              \
	      B = B->cp_b;                                  \
        HBREG = PROTECT_FROZEN_H(B);                  \
        SET_BB(PROTECT_FROZEN_B(B));                  \
        if ((choiceptr) YENV == B_FZ) {               \
          copy_arity_stack();                         \
        }

#ifdef YAPOR
#define pop_trie_node()                               \
        dprintf("pop_trie_node\n");                    \
        if (SCH_top_shared_cp(B)) {                   \
          restore_trie_node(NULL);                    \
        } else {                                      \
          really_pop_trie_node();                     \
        }
#else
#define pop_trie_node()  {  \
        dprintf("pop_trie_node\n");  \
      really_pop_trie_node()  \
    }
#endif /* YAPOR */



/* ------------------- **
**      trie_null      **
** ------------------- */

#define stack_trie_null_instr()                              \
dprintf("stack_trie_null_instr\n");    \
        next_trie_instruction(node)

#ifdef TRIE_COMPACT_PAIRS
/* trie compiled code for term 'CompactPairInit' */
#define stack_trie_null_in_new_pair_instr()                  \
        if (heap_arity) {                                    \
          aux_stack_ptr++;                                   \
          Bind_Global((CELL *) *aux_stack_ptr, AbsPair(H));  \
          *aux_stack_ptr-- = (CELL) (H + 1);                 \
          *aux_stack_ptr-- = (CELL) H;                       \
          *aux_stack_ptr = heap_arity - 1 + 2;               \
          YENV = aux_stack_ptr;                              \
        } else {                                             \
          int i;                                             \
          *aux_stack_ptr-- = (CELL) (H + 1);                 \
          *aux_stack_ptr-- = (CELL) H;                       \
          *aux_stack_ptr = 2;                                \
          YENV = aux_stack_ptr;                              \
          aux_stack_ptr += 2 + 2;                            \
          *aux_stack_ptr = subs_arity - 1;                   \
          aux_stack_ptr += subs_arity;                       \
          Bind((CELL *) *aux_stack_ptr, AbsPair(H));         \
          for (i = 0; i < vars_arity; i++) {                 \
            *aux_stack_ptr = *(aux_stack_ptr + 1);           \
            aux_stack_ptr++;                                 \
          }                                                  \
        }                                                    \
        H += 2;                                              \
        next_trie_instruction(node)
#endif /* TRIE_COMPACT_PAIRS */



/* ------------------ **
**      trie_var      **
** ------------------ */

// OK
#define stack_trie_var_instr()                                   \
dprintf("stack_trie_var_instr\n");                                 \
        if (heap_arity) {                                        \
          CELL term;                                              \
          int i;                                                 \
          *aux_stack_ptr = heap_arity - 1;                       \
          term = Deref(*++aux_stack_ptr);                        \
          for (i = 0; i < heap_arity - 1; i++) {                 \
            *aux_stack_ptr = *(aux_stack_ptr + 1);               \
            aux_stack_ptr++;                                     \
          }                                                      \
          *aux_stack_ptr++ = vars_arity + 1;                     \
          *aux_stack_ptr++ = subs_arity;                         \
          for (i = 0; i < subs_arity; i++) {                     \
            *aux_stack_ptr = *(aux_stack_ptr + 1);               \
            aux_stack_ptr++;                                     \
          }                                                      \
          *aux_stack_ptr = term;                                 \
          next_instruction(heap_arity - 1 || subs_arity, node);  \
        } else {                                                 \
          *++aux_stack_ptr = vars_arity + 1;                     \
          *++aux_stack_ptr = subs_arity - 1;                     \
          /* binding is done automatically */                    \
          next_instruction(subs_arity - 1, node);                \
        }

#ifdef TRIE_COMPACT_PAIRS
#define stack_trie_var_in_new_pair_instr()                       \
        if (heap_arity) {                                        \
          int i;                                                 \
          *aux_stack_ptr-- = (CELL) (H + 1);                     \
          *aux_stack_ptr = heap_arity - 1 + 1;                   \
          YENV = aux_stack_ptr;                                  \
          aux_stack_ptr += 2;                                    \
          Bind_Global((CELL *) *aux_stack_ptr, AbsPair(H));      \
          for (i = 0; i < heap_arity - 1; i++) {                 \
            *aux_stack_ptr = *(aux_stack_ptr + 1);               \
            aux_stack_ptr++;                                     \
          }                                                      \
          *aux_stack_ptr++ = vars_arity + 1;                     \
          *aux_stack_ptr++ = subs_arity;                         \
          for (i = 0; i < subs_arity; i++) {                     \
            *aux_stack_ptr = *(aux_stack_ptr + 1);               \
            aux_stack_ptr++;                                     \
          }                                                      \
          *aux_stack_ptr = (CELL) H;                             \
        } else {                                                 \
          *aux_stack_ptr-- = (CELL) (H + 1);                     \
          *aux_stack_ptr = 1;                                    \
          YENV = aux_stack_ptr;                                  \
          aux_stack_ptr += 2;                                    \
          *aux_stack_ptr++ = vars_arity + 1;                     \
          *aux_stack_ptr = subs_arity - 1;                       \
          aux_stack_ptr += subs_arity;                           \
          Bind((CELL *) *aux_stack_ptr, AbsPair(H));             \
          *aux_stack_ptr = (CELL) H;                             \
        }                                                        \
        RESET_VARIABLE((CELL) H);                                \
        H += 2;                                                  \
        next_trie_instruction(node)
#endif /* TRIE_COMPACT_PAIRS */



/* ------------------ **
**      trie_val      **
** ------------------ */

#define UNIFY_VAR() \
  switch(cell_tag(aux_sub)) { \
      case TAG_REF: \
          switch(cell_tag(aux_var)) { \
            case TAG_REF: \
              if (aux_sub > aux_var) {                                                          \
    	          if ((CELL *) aux_sub <= H) {                                                          \
                  Bind_Global((CELL *) aux_sub, aux_var);                                       \
                } else if ((CELL *) aux_var <= H) {                                             \
                  Bind_Local((CELL *) aux_sub, aux_var);                                        \
                } else {                                                                        \
                  Bind_Local((CELL *) aux_var, aux_sub);                                        \
                  *vars_ptr = aux_sub;                                                          \
                }                                                                               \
              } else {                                                                          \
    	          if ((CELL *) aux_var <= H) {                                                    \
                  Bind_Global((CELL *) aux_var, aux_sub);                                       \
                  *vars_ptr = aux_sub;                                                          \
                } else if ((CELL *) aux_sub <= H) {                                             \
                  Bind_Local((CELL *) aux_var, aux_sub);                                        \
                  *vars_ptr = aux_sub;                                                          \
                } else {                                                                        \
                  Bind_Local((CELL *) aux_sub, aux_var);                                        \
                }                                                                               \
              }                                                                                 \
              break;  \
              default:  \
                dprintf("AUX_SUB VAR TAG_REF DEFAULT\n");  \
                Bind_Global((CELL *)aux_sub, aux_var);  \
                break;  \
          } \
        break;  \
      default:  \
        switch(cell_tag(aux_var)) { \
          case TAG_REF: \
            if((CELL *) aux_var <= H) { \
              Bind_Global((CELL *) aux_var, aux_sub); \
              *vars_ptr = aux_sub;  /* ok */ \
              dprintf("Bind Global 1\n");  \
            } else {  /* XXX */ \
              Bind_Local((CELL *) aux_var, aux_sub);  \
              *vars_ptr = aux_sub;  \
              dprintf("Bind local 1\n"); \
            } \
            break;      \
          default:  \
            /* run yap unification algorithm */ \
            dprintf("Run yap unify \n"); \
            if(!Yap_unify(aux_var, aux_sub)) {  \
              dprintf("Unification failed\n"); \
              goto fail;  \
            } \
            break;  \
        } \
  }

#define stack_trie_val_instr()                                                              \
dprintf("stack_trie_val_instr\n");                                                           \
        if (heap_arity) {                                                                   \
          CELL aux_sub, aux_var, *vars_ptr;				                                          \
          YENV = ++aux_stack_ptr;                                                           \
          vars_ptr = aux_stack_ptr + heap_arity + 1 + subs_arity + vars_arity - var_index;  \
          aux_sub = Deref(*aux_stack_ptr); /* substitution var */                                  \
          aux_var = Deref(*vars_ptr);                                                              \
          UNIFY_VAR();                                                                      \
          INC_HEAP_ARITY(-1);                                                               \
          next_instruction(heap_arity - 1 || subs_arity, node);                             \
        } else {                                                                            \
          CELL aux_sub, aux_var, *vars_ptr;                                                 \
          aux_stack_ptr += 2;                                                               \
          *aux_stack_ptr = subs_arity - 1;                                                  \
          aux_stack_ptr += subs_arity;                                                      \
          vars_ptr = aux_stack_ptr + vars_arity - var_index; /* pointer to trie var */      \
          aux_sub = Deref(*aux_stack_ptr);  /* substitution var */                                 \
          aux_var = Deref(*vars_ptr);  /* trie var */                                       \
          UNIFY_VAR();                                                                      \
          ALIGN_STACK_LEFT();                                                               \
          next_instruction(subs_arity - 1, node);                                           \
        }

#ifdef TRIE_COMPACT_PAIRS      
#define stack_trie_val_in_new_pair_instr()                                                  \
        if (heap_arity) {                                                                   \
          CELL aux_sub, aux_var, *vars_ptr;	      	               		                      \
          aux_stack_ptr++;				                                                          \
          Bind_Global((CELL *) *aux_stack_ptr, AbsPair(H));                                 \
          *aux_stack_ptr = (CELL) (H + 1);                                                  \
          aux_sub = (CELL) H;                                                               \
          vars_ptr = aux_stack_ptr + heap_arity + 1 + subs_arity + vars_arity - var_index;  \
          aux_var = *vars_ptr;                                                              \
          if (aux_sub > aux_var) {                                                          \
            Bind_Global((CELL *) aux_sub, aux_var);                                         \
          } else {                                                                          \
            RESET_VARIABLE(aux_sub);                                                        \
  	    Bind_Local((CELL *) aux_var, aux_sub);                                              \
            *vars_ptr = aux_sub;                                                            \
          }                                                                                 \
        } else {                                                                            \
          CELL aux_sub, aux_var, *vars_ptr;                                                 \
          int i;                                                                            \
          *aux_stack_ptr-- = (CELL) (H + 1);                                                \
          *aux_stack_ptr = 1;                                                               \
          YENV = aux_stack_ptr;                                                             \
          aux_stack_ptr += 1 + 2;                                                           \
          aux_sub = (CELL) H;                                                               \
          vars_ptr = aux_stack_ptr + subs_arity + vars_arity - var_index;                   \
          aux_var = *vars_ptr;                                                              \
          if (aux_sub > aux_var) {                                                          \
            Bind_Global((CELL *) aux_sub, aux_var);                                         \
          } else {                                                                          \
            RESET_VARIABLE(aux_sub);                                                        \
	          Bind_Local((CELL *) aux_var, aux_sub);                                          \
            *vars_ptr = aux_sub;                                                            \
          }                                                                                 \
          *aux_stack_ptr = subs_arity - 1;                                                  \
          aux_stack_ptr += subs_arity;                                                      \
          Bind((CELL *) *aux_stack_ptr, AbsPair(H));                                        \
          for (i = 0; i < vars_arity; i++) {                                                \
            *aux_stack_ptr = *(aux_stack_ptr + 1);                                          \
            aux_stack_ptr++;                                                                \
          }                                                                                 \
        }                                                                                   \
        H += 2;                                                                             \
        next_trie_instruction(node)
#endif /* TRIE_COMPACT_PAIRS */



/* ------------------- **
**      trie_atom      **
** ------------------- */

// OK
#define stack_trie_atom_instr()                                      \
dprintf("stack_trie_atom_instr\n");                                    \
        dprintf("Heap arity: %d\n", heap_arity); \
        if (heap_arity) {                                            \
          YENV = ++aux_stack_ptr;                                    \
          CELL term = Deref(*aux_stack_ptr);                         \
          if(IsVarTerm(term)) {                                        \
            Bind_Global((CELL *) *aux_stack_ptr, TrNode_entry(node));  \
          } else {                                                  \
            if(term != TrNode_entry(node)) {                        \
              goto fail;                                            \
            }                                                       \
          }                                                         \
          INC_HEAP_ARITY(-1);                                        \
          next_instruction(heap_arity - 1 || subs_arity, node);      \
        } else {                                                     \
          aux_stack_ptr += 2;                                        \
          *aux_stack_ptr = subs_arity - 1;                           \
          aux_stack_ptr += subs_arity;                               \
          CELL term = Deref(*aux_stack_ptr);                         \
          if(IsVarTerm(term)) {                                      \
            dprintf("aux_stack_ptr is var\n");                        \
            Bind((CELL *) term, TrNode_entry(node));       \
          } else {                                                   \
            if(term != TrNode_entry(node)) {               \
              dprintf("No match\n");                                  \
              goto fail;                                             \
            }                                                        \
          }                                                          \
          ALIGN_STACK_LEFT();                                        \
          next_instruction(subs_arity - 1, node);                    \
        }

#ifdef TRIE_COMPACT_PAIRS
#define stack_trie_atom_in_new_pair_instr()                          \
        if (heap_arity) {                                            \
          aux_stack_ptr++;		                             \
          Bind_Global((CELL *) *aux_stack_ptr, AbsPair(H));          \
          *aux_stack_ptr = (CELL) (H + 1);                           \
        } else {                                                     \
          int i;                                                     \
          *aux_stack_ptr-- = (CELL) (H + 1);                         \
          *aux_stack_ptr = 1;                                        \
          YENV = aux_stack_ptr;                                      \
          aux_stack_ptr += 1 + 2;                                    \
          *aux_stack_ptr = subs_arity - 1;                           \
          aux_stack_ptr += subs_arity;                               \
          Bind((CELL *) *aux_stack_ptr, AbsPair(H));                 \
          for (i = 0; i < vars_arity; i++) {                         \
            *aux_stack_ptr = *(aux_stack_ptr + 1);                   \
            aux_stack_ptr++;                                         \
          }                                                          \
        }                                                            \
        Bind_Global(H, TrNode_entry(node));                          \
        H += 2;                                                      \
        next_trie_instruction(node)
#endif /* TRIE_COMPACT_PAIRS */



/* ------------------- **
**      trie_pair      **
** ------------------- */

#define PUSH_LIST_ARGS(TERM) {  \
  *aux_stack_ptr-- = *(RepPair(TERM) + 1);  \
  *aux_stack_ptr-- = *(RepPair(TERM) + 0);  \
}

#define PUSH_NEW_LIST() { \
  *aux_stack_ptr-- = (CELL) (H + 1);                 \
  *aux_stack_ptr-- = (CELL) H;                       \
}

#define MARK_HEAP_LIST() {  \
    RESET_VARIABLE(H);  \
    RESET_VARIABLE(H+1);  \
    H += 2; \
  }

#ifdef TRIE_COMPACT_PAIRS
/* trie compiled code for term 'CompactPairEndList' */
#define stack_trie_pair_instr()		                     \
        if (heap_arity) {                                    \
          aux_stack_ptr++;                                   \
          Bind_Global((CELL *) *aux_stack_ptr, AbsPair(H));  \
          *aux_stack_ptr = (CELL) H;                         \
	} else {                                             \
          int i;                                             \
          *aux_stack_ptr-- = (CELL) H;                       \
          *aux_stack_ptr = 1;                                \
          YENV = aux_stack_ptr;                              \
          aux_stack_ptr += 1 + 2;                            \
          *aux_stack_ptr = subs_arity - 1;                   \
          aux_stack_ptr += subs_arity;                       \
          Bind((CELL *) *aux_stack_ptr, AbsPair(H));         \
          for (i = 0; i < vars_arity; i++) {                 \
            *aux_stack_ptr = *(aux_stack_ptr + 1);           \
            aux_stack_ptr++;                                 \
          }                                                  \
	}                                                    \
        Bind_Global(H + 1, TermNil);                         \
        H += 2;                                              \
        next_trie_instruction(node)
#else
#define stack_trie_pair_instr()                              \
        dprintf("stack_trie_pair_instr\n");                    \
        if (heap_arity) {                                    \
          aux_stack_ptr++;                                   \
          Term term = Deref(*aux_stack_ptr);                 \
          switch(cell_tag(term)) {                           \
            case TAG_LIST: {                                 \
                PUSH_LIST_ARGS(term);                        \
                INC_HEAP_ARITY(1);                            \
                YENV = aux_stack_ptr;                         \
              }                                                   \
              break;                                              \
            case TAG_REF: {                                     \
                Bind_Global((CELL *) *aux_stack_ptr, AbsPair(H));  \
                PUSH_NEW_LIST();                                   \
                INC_HEAP_ARITY(1);                                \
                YENV = aux_stack_ptr;                           \
                MARK_HEAP_LIST();                               \
              }                                                \
              break;                                          \
            default:                                          \
              goto fail;                                     \
            }                                               \
        } else {                                             \
          CELL term = Deref(*(aux_stack_ptr + 2 + subs_arity)); \
          switch(cell_tag(term))  { \
            case TAG_LIST:  { \
                PUSH_LIST_ARGS(term); \
                INC_HEAP_ARITY(2);  \
                YENV = aux_stack_ptr; \
                aux_stack_ptr += 2 + 2; /* jump to subs */ \
                *aux_stack_ptr = subs_arity - 1; /* update subs arity */ \
                aux_stack_ptr += subs_arity;        \
                ALIGN_STACK_LEFT();     \
              } \
              break;  \
            case TAG_REF: { \
                PUSH_NEW_LIST();  \
                INC_HEAP_ARITY(2);  \
                YENV = aux_stack_ptr; \
                aux_stack_ptr += 2 + 2; /* jump to subs */ \
                *aux_stack_ptr = subs_arity - 1; /* change subs arity */ \
                aux_stack_ptr += subs_arity;  \
                Bind((CELL *) *aux_stack_ptr, AbsPair(H));         \
                ALIGN_STACK_LEFT(); \
                MARK_HEAP_LIST(); \
              } \
            break;  \
            default:  \
                goto fail;  \
          } \
        }                                                    \
        next_trie_instruction(node)
#endif /* TRIE_COMPACT_PAIRS */


#define ALIGN_STACK_LEFT() { \
      int i;    \
      for(i = 0; i < vars_arity; i++, aux_stack_ptr++) { \
        *aux_stack_ptr = *(aux_stack_ptr + 1);  \
      } \
    }

/* given a functor term this put
   starting from aux_stack_ptr (from high to low)
   the functor arguments of TERM */
#define PUSH_FUNCTOR_ARGS(TERM)  { \
    int i;  \
    for(i = 0; i < func_arity; ++i) { \
      *aux_stack_ptr-- = (CELL)*(RepAppl(TERM) + func_arity - i);    \
      dprintf("Pushed one old arg\n"); \
    } \
  }
  
#define PUSH_NEW_FUNCTOR()  { \
    int i;                                                 \
    for (i = 0; i < func_arity; i++) {                       \
    *aux_stack_ptr-- = (CELL) (H + func_arity - i);      \
    dprintf("Pushed one arg\n"); \
  } \
  }

/* tag a functor on the heap */
#define MARK_HEAP_FUNCTOR() { \
  *H = (CELL)func;  \
  int i;  \
  for(i = 0; i < func_arity; ++i) \
    RESET_VARIABLE(H + 1 + i);  \
  H += 1 + func_arity;  \
}

/* if aux_stack_ptr is positioned on the heap arity cell, increment it by TOTAL */
#define INC_HEAP_ARITY(TOTAL)  { \
    *aux_stack_ptr = heap_arity + (TOTAL);  \
  }

/* --------------------- **
**      trie_struct      **
** --------------------- */

#define stack_trie_struct_instr()                                \
dprintf("stack_trie_struct_instr\n");                            \
        if (heap_arity) {                                        \
          dprintf("struct heap arity %d\n", heap_arity); \
          aux_stack_ptr++;                                       \
          CELL term = Deref(*aux_stack_ptr);                     \
          switch(cell_tag(term))  {                               \
            case TAG_STRUCT:  {                                   \
              dprintf("TAG_STRUCT\n"); \
              Functor func2 = FunctorOfTerm(term);                \
              if(func != func2) {                                 \
                dprintf("NOT SAME FUNCTOR\n");                     \
                goto fail;                                        \
              }                                                   \
              dprintf("Pushing already built functor on the stack with arity %d\n", func_arity); \
              PUSH_FUNCTOR_ARGS(term);  \
              YENV = aux_stack_ptr; \
              INC_HEAP_ARITY(func_arity - 1); \
            } \
            break;  \
            case TAG_REF: { \
              dprintf("TAG_REF\n");  \
              /* bind this variable to a new functor  \
                 that is built using the arguments on the trie  \
                 */ \
              Bind_Global((CELL *) term, AbsAppl(H)); \
              PUSH_NEW_FUNCTOR(); \
              YENV = aux_stack_ptr; \
              INC_HEAP_ARITY(func_arity - 1); \
              MARK_HEAP_FUNCTOR();  \
            } \
            break;  \
            default:  \
              dprintf("??\n"); \
              goto fail;  \
          } \
        } else {                                                 \
          CELL term = Deref(*(aux_stack_ptr + 2 + subs_arity));               \
          switch(cell_tag(term))  { \
            case TAG_STRUCT: {  \
              dprintf("TAG_STRUCT NON HEAP\n");  \
              Functor func2 = FunctorOfTerm(term);     \
              if(func != func2) {  \
                dprintf("NOT A FUNCTOR\n");  \
                goto fail;                                          \
              }             \
              dprintf("Pushing already built functor on the stack with arity %d\n", func_arity); \
              /* push already built functor terms on the stack */         \
              PUSH_FUNCTOR_ARGS(term);  \
              YENV = aux_stack_ptr; \
              INC_HEAP_ARITY(func_arity); \
              aux_stack_ptr += func_arity + 2; /* jump to subs*/ \
              *aux_stack_ptr = subs_arity - 1; /* new subs arity */ \
              aux_stack_ptr += subs_arity;  \
              ALIGN_STACK_LEFT(); \
            } \
            break;  \
            case TAG_REF:     {                                      \
              dprintf("TAG_REF NON HEAP\n"); \
              PUSH_NEW_FUNCTOR();                                     \
              INC_HEAP_ARITY(func_arity); \
              YENV = aux_stack_ptr;                                  \
              aux_stack_ptr += func_arity + 2; /* jump to subs */    \
              *aux_stack_ptr = subs_arity - 1; /* new subs arity */  \
              aux_stack_ptr += subs_arity;                           \
              Bind((CELL *) *aux_stack_ptr, AbsAppl(H));      \
              ALIGN_STACK_LEFT(); \
              MARK_HEAP_FUNCTOR();  \
            } \
              break;  \
            default:  \
              goto fail;  \
          } \
        }                                                        \
        next_trie_instruction(node)

#ifdef TRIE_COMPACT_PAIRS
#define stack_trie_struct_in_new_pair_instr()	                 \
        if (heap_arity) {                                        \
          int i;                                                 \
          aux_stack_ptr++;		                         \
          Bind_Global((CELL *) *aux_stack_ptr, AbsPair(H));      \
          *aux_stack_ptr-- = (CELL) (H + 1);                     \
          for (i = 0; i < func_arity; i++)                       \
            *aux_stack_ptr-- = (CELL) (H + 2 + func_arity - i);  \
          *aux_stack_ptr = heap_arity - 1 + 1 + func_arity;      \
          YENV = aux_stack_ptr;                                  \
        } else {                                                 \
          int i;                                                 \
          *aux_stack_ptr-- = (CELL) (H + 1);                     \
          for (i = 0; i < func_arity; i++)                       \
            *aux_stack_ptr-- = (CELL) (H + 2 + func_arity - i);  \
          *aux_stack_ptr = 1 + func_arity;                       \
          YENV = aux_stack_ptr;                                  \
          aux_stack_ptr += 1 + func_arity + 2;                   \
          *aux_stack_ptr = subs_arity - 1;                       \
          aux_stack_ptr += subs_arity;                           \
          Bind((CELL *) *aux_stack_ptr, AbsPair(H));             \
          for (i = 0; i < vars_arity; i++) {                     \
            *aux_stack_ptr = *(aux_stack_ptr + 1);               \
            aux_stack_ptr++;                                     \
          }                                                      \
        }                                                        \
        Bind_Global(H, AbsAppl(H + 2));                          \
        H += 2;                                                  \
        *H = (CELL) func;                                        \
        H += 1 + func_arity;                                     \
        next_trie_instruction(node)
#endif /* TRIE_COMPACT_PAIRS */



/* ------------------------ **
**      trie_extension      **
** ------------------------ */

#define stack_trie_extension_instr()                               \
dprintf("stack_trie_extension_instr\n");       \
        *aux_stack_ptr-- = 0;  /* float/longint extension mark */  \
        *aux_stack_ptr-- = TrNode_entry(node);                     \
        *aux_stack_ptr = heap_arity + 2;                           \
        YENV = aux_stack_ptr;                                      \
        next_trie_instruction(node)



/* ---------------------------- **
**      trie_float_longint      **
** ---------------------------- */

#define stack_trie_float_longint_instr()                         \
dprintf("stack_trie_float_longint_instr\n");     \
        if (heap_arity) {                                        \
          YENV = ++aux_stack_ptr;                                \
          Bind_Global((CELL *) *aux_stack_ptr, t);               \
          *aux_stack_ptr = heap_arity - 1;                       \
          next_instruction(heap_arity - 1 || subs_arity, node);  \
        } else {                                                 \
          int i;                                                 \
          YENV = aux_stack_ptr;                                  \
          *aux_stack_ptr = 0;                                    \
          aux_stack_ptr += 2;                                    \
          *aux_stack_ptr = subs_arity - 1;                       \
          aux_stack_ptr += subs_arity;                           \
          Bind((CELL *) *aux_stack_ptr, t);                      \
          for (i = 0; i < vars_arity; i++) {                     \
            *aux_stack_ptr = *(aux_stack_ptr + 1);               \
            aux_stack_ptr++;                                     \
          }                                                      \
	  next_instruction(subs_arity - 1, node);                \
        }



/* --------------------------- **
**      Trie instructions      **
** --------------------------- */

  PBOp(trie_do_null, e)
  dprintf("trie_do_null\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;

    stack_trie_null_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_do_null)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_trust_null, e)
    dprintf("trie_trust_null\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = (CELL *) (B + 1);
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    pop_trie_node();
    stack_trie_null_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_trust_null)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_try_null, e)
    dprintf("trie_try_null\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    store_trie_node(TrNode_next(node));
    stack_trie_null_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_try_null)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_retry_null, e)
    dprintf("trie_retry_null\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = (CELL *) (B + 1);
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    restore_trie_node(TrNode_next(node));
    stack_trie_null_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_retry_null)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_do_null_in_new_pair, e)
    dprintf("trie_do_null_in_new_pair\n");
#if defined(TRIE_COMPACT_PAIRS) && !defined(GLOBAL_TRIE)
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    stack_trie_null_in_new_pair_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_do_null_in_new_pair)");
#endif /* TRIE_COMPACT_PAIRS && GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_trust_null_in_new_pair, e)
    dprintf("trie_trust_null_in_new_pair\n");
#if defined(TRIE_COMPACT_PAIRS) && !defined(GLOBAL_TRIE)
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = (CELL *) (B + 1);
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    pop_trie_node();
    stack_trie_null_in_new_pair_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_trust_null_in_new_pair)");
#endif /* TRIE_COMPACT_PAIRS && GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_try_null_in_new_pair, e)
    dprintf("trie_try_null_in_new_pair\n");
#if defined(TRIE_COMPACT_PAIRS) && !defined(GLOBAL_TRIE)
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    store_trie_node(TrNode_next(node));
    stack_trie_null_in_new_pair_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_try_null_in_new_pair)");
#endif /* TRIE_COMPACT_PAIRS && GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_retry_null_in_new_pair, e)
    dprintf("trie_retry_null_in_new_pair\n");
#if defined(TRIE_COMPACT_PAIRS) && !defined(GLOBAL_TRIE)
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = (CELL *) (B + 1);
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    restore_trie_node(TrNode_next(node));
    stack_trie_null_in_new_pair_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_retry_null_in_new_pair)");
#endif /* TRIE_COMPACT_PAIRS && GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_do_var, e)
    dprintf("trie_do_var\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    stack_trie_var_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_do_var)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_trust_var, e)
    dprintf("trie_trust_var\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = (CELL *) (B + 1);
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    pop_trie_node();
    stack_trie_var_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_trust_var)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_try_var, e)
    dprintf("trie_try_var\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    store_trie_node(TrNode_next(node));
    stack_trie_var_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_try_var)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_retry_var, e)
    dprintf("trie_retry_var\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = (CELL *) (B + 1);
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    restore_trie_node(TrNode_next(node));
    stack_trie_var_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_retry_var)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_do_var_in_new_pair, e)
    dprintf("trie_do_var_in_new_pair\n");
#if defined(TRIE_COMPACT_PAIRS) && !defined(GLOBAL_TRIE)
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    stack_trie_var_in_new_pair_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_do_var_in_new_pair)");
#endif /* TRIE_COMPACT_PAIRS && GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_trust_var_in_new_pair, e)
    dprintf("trie_trust_var_in_new_pair\n");
#if defined(TRIE_COMPACT_PAIRS) && !defined(GLOBAL_TRIE)
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = (CELL *) (B + 1);
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    pop_trie_node();
    stack_trie_var_in_new_pair_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_trust_var_in_new_pair)");
#endif /* TRIE_COMPACT_PAIRS && GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_try_var_in_new_pair, e)
    dprintf("trie_try_var_in_new_pair\n");
#if defined(TRIE_COMPACT_PAIRS) && !defined(GLOBAL_TRIE)
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    store_trie_node(TrNode_next(node));
    stack_trie_var_in_new_pair_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_try_var_in_new_pair)");
#endif /* TRIE_COMPACT_PAIRS && GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_retry_var_in_new_pair, e)
    dprintf("trie_retry_var_in_new_pair\n");
#if defined(TRIE_COMPACT_PAIRS) && !defined(GLOBAL_TRIE)
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = (CELL *) (B + 1);
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    restore_trie_node(TrNode_next(node));
    stack_trie_var_in_new_pair_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_retry_var_in_new_pair)");
#endif /* TRIE_COMPACT_PAIRS && GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_do_val, e)
    dprintf("trie_do_val\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);
    int var_index = VarIndexOfTableTerm(TrNode_entry(node));

    stack_trie_val_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_do_val)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_trust_val, e)
    dprintf("trie_trust_val\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = (CELL *) (B + 1);
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);
    int var_index = VarIndexOfTableTerm(TrNode_entry(node));

    pop_trie_node();
    stack_trie_val_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_trust_val)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_try_val, e)
    dprintf("trie_try_val\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);
    int var_index = VarIndexOfTableTerm(TrNode_entry(node));

    store_trie_node(TrNode_next(node));
    stack_trie_val_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_try_val)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_retry_val, e)
    dprintf("trie_retry_val\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = (CELL *) (B + 1);
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);
    int var_index = VarIndexOfTableTerm(TrNode_entry(node));

    restore_trie_node(TrNode_next(node));
    stack_trie_val_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_retry_val)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_do_val_in_new_pair, e)
    dprintf("trie_do_val_in_new_pair\n");
#if defined(TRIE_COMPACT_PAIRS) && !defined(GLOBAL_TRIE)
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);
    int var_index = VarIndexOfTableTerm(TrNode_entry(node));

    stack_trie_val_in_new_pair_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_do_val_in_new_pair)");
#endif /* TRIE_COMPACT_PAIRS && GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_trust_val_in_new_pair, e)
    dprintf("trie_trust_val_in_pair\n");
#if defined(TRIE_COMPACT_PAIRS) && !defined(GLOBAL_TRIE)
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = (CELL *) (B + 1);
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);
    int var_index = VarIndexOfTableTerm(TrNode_entry(node));

    pop_trie_node();
    stack_trie_val_in_new_pair_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_trust_val_in_new_pair)");
#endif /* TRIE_COMPACT_PAIRS && GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_try_val_in_new_pair, e)
    dprintf("trie_retry_val_in_new_pair\n");
#if defined(TRIE_COMPACT_PAIRS) && !defined(GLOBAL_TRIE)
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);
    int var_index = VarIndexOfTableTerm(TrNode_entry(node));

    store_trie_node(TrNode_next(node));
    stack_trie_val_in_new_pair_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_try_val_in_new_pair)");
#endif /* TRIE_COMPACT_PAIRS && GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_retry_val_in_new_pair, e)
    dprintf("trie_retry_val_in_new_pair\n");
#if defined(TRIE_COMPACT_PAIRS) && !defined(GLOBAL_TRIE)
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = (CELL *) (B + 1);
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);
    int var_index = VarIndexOfTableTerm(TrNode_entry(node));

    restore_trie_node(TrNode_next(node));
    stack_trie_val_in_new_pair_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_retry_val_in_new_pair)");
#endif /* TRIE_COMPACT_PAIRS && GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_do_atom, e)
    dprintf("trie_do_atom\n");
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
#ifdef GLOBAL_TRIE
    int subs_arity = *(aux_stack_ptr + *aux_stack_ptr + 1);
    YENV = aux_stack_ptr = load_substitution_variable(TrNode_entry(node), aux_stack_ptr);
    next_instruction(subs_arity - 1 , node);
#else
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    stack_trie_atom_instr();
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_trust_atom, e)
    dprintf("trie_trust_atom\n");
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = (CELL *) (B + 1);
#ifdef GLOBAL_TRIE
    int vars_arity = *(aux_stack_ptr);
    int subs_arity = *(aux_stack_ptr + vars_arity + 1);
#else
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);
#endif /* GLOBAL_TRIE */
    pop_trie_node();
#ifdef GLOBAL_TRIE
    YENV = aux_stack_ptr = load_substitution_variable(TrNode_entry(node), aux_stack_ptr);
    next_instruction(subs_arity - 1 , node);
#else
    stack_trie_atom_instr();
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_try_atom, e)
    dprintf("trie_try_atom\n");
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
#ifdef GLOBAL_TRIE
    int vars_arity = *(aux_stack_ptr);
    int subs_arity = *(aux_stack_ptr + vars_arity + 1);
#else
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);
#endif /* GLOBAL_TRIE */
    store_trie_node(TrNode_next(node));
#ifdef GLOBAL_TRIE
    YENV = aux_stack_ptr = load_substitution_variable(TrNode_entry(node), aux_stack_ptr);
    next_instruction(subs_arity - 1, node); 
#else
    stack_trie_atom_instr();
#endif /* GLOBAL_TRIE */    
  ENDPBOp();


  PBOp(trie_retry_atom, e)
    dprintf("trie_retry_atom\n");
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = (CELL *) (B + 1);
#ifdef GLOBAL_TRIE
    int vars_arity = *(aux_stack_ptr);
    int subs_arity = *(aux_stack_ptr + vars_arity + 1);
#else
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);
#endif /* GLOBAL_TRIE */
    restore_trie_node(TrNode_next(node));
#ifdef GLOBAL_TRIE
    YENV = aux_stack_ptr = load_substitution_variable(TrNode_entry(node), aux_stack_ptr);
    next_instruction(subs_arity - 1, node); 
#else
    stack_trie_atom_instr();
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_do_atom_in_new_pair, e)
    dprintf("trie_do_atom_in_new_pair\n");
#if defined(TRIE_COMPACT_PAIRS) && !defined(GLOBAL_TRIE)
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    stack_trie_atom_in_new_pair_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_do_atom_in_new_pair)");
#endif /* TRIE_COMPACT_PAIRS && GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_trust_atom_in_new_pair, e)
    dprintf("trie_trust_atom_in_new_pair\n");
#if defined(TRIE_COMPACT_PAIRS) && !defined(GLOBAL_TRIE)
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = (CELL *) (B + 1);
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    pop_trie_node();
    stack_trie_atom_in_new_pair_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_trust_atom_in_new_pair)");
#endif /* TRIE_COMPACT_PAIRS && GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_try_atom_in_new_pair, e)
    dprintf("trie_try_atom_in_new_pair\n");
#if defined(TRIE_COMPACT_PAIRS) && !defined(GLOBAL_TRIE)
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    store_trie_node(TrNode_next(node));
    stack_trie_atom_in_new_pair_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_try_atom_in_new_pair)");
#endif /* TRIE_COMPACT_PAIRS && GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_retry_atom_in_new_pair, e)
    dprintf("trie_retry_atom_in_new_pair\n");
#if defined(TRIE_COMPACT_PAIRS) && !defined(GLOBAL_TRIE)
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = (CELL *) (B + 1);
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    restore_trie_node(TrNode_next(node));
    stack_trie_atom_in_new_pair_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_retry_atom_in_new_pair)");
#endif /* TRIE_COMPACT_PAIRS && GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_do_pair, e)
    dprintf("trie_do_pair\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    stack_trie_pair_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_do_pair)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_trust_pair, e)
    dprintf("trie_trust_pair\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = (CELL *) (B + 1);
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    pop_trie_node();
    stack_trie_pair_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_trust_pair)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_try_pair, e)
    dprintf("trie_try_pair\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    store_trie_node(TrNode_next(node));
    stack_trie_pair_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_try_pair)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_retry_pair, e)
    dprintf("trie_retry_pair\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = (CELL *) (B + 1);
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    restore_trie_node(TrNode_next(node));
    stack_trie_pair_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_retry_pair)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_do_struct, e)
    dprintf("trie_do_struct\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);
    Functor func = (Functor) RepAppl(TrNode_entry(node));
    int func_arity = ArityOfFunctor(func);

    stack_trie_struct_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_do_struct)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_trust_struct, e)
    dprintf("trie_trust_struct\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = (CELL *) (B + 1);
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);
    Functor func = (Functor) RepAppl(TrNode_entry(node));
    int func_arity = ArityOfFunctor(func);

    pop_trie_node();
    stack_trie_struct_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_trust_struct)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_try_struct, e)
    dprintf("trie_try_struct\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);
    Functor func = (Functor) RepAppl(TrNode_entry(node));
    int func_arity = ArityOfFunctor(func);

    store_trie_node(TrNode_next(node));
    stack_trie_struct_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_try_struct)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_retry_struct, e)
    dprintf("trie_retry_struct\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = (CELL *) (B + 1);
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);
    Functor func = (Functor) RepAppl(TrNode_entry(node));
    int func_arity = ArityOfFunctor(func);

    restore_trie_node(TrNode_next(node));
    stack_trie_struct_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_retry_struct)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_do_struct_in_new_pair, e)
    dprintf("trie_do_struct_in_new_pair\n");
#if defined(TRIE_COMPACT_PAIRS) && !defined(GLOBAL_TRIE)
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);
    Functor func = (Functor) RepAppl(TrNode_entry(node));
    int func_arity = ArityOfFunctor(func);

    stack_trie_struct_in_new_pair_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_do_struct_in_new_pair)");
#endif /* TRIE_COMPACT_PAIRS && GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_trust_struct_in_new_pair, e)
    dprintf("trie_trust_struct_in_new_pair\n");
#if defined(TRIE_COMPACT_PAIRS) && !defined(GLOBAL_TRIE)
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = (CELL *) (B + 1);
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);
    Functor func = (Functor) RepAppl(TrNode_entry(node));
    int func_arity = ArityOfFunctor(func);

    pop_trie_node();
    stack_trie_struct_in_new_pair_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_trust_struct_in_new_pair)");
#endif /* TRIE_COMPACT_PAIRS && GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_try_struct_in_new_pair, e)
    dprintf("trie_try_struct_in_new_pair\n");
#if defined(TRIE_COMPACT_PAIRS) && !defined(GLOBAL_TRIE)
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);
    Functor func = (Functor) RepAppl(TrNode_entry(node));
    int func_arity = ArityOfFunctor(func);

    store_trie_node(TrNode_next(node));
    stack_trie_struct_in_new_pair_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_try_struct_in_new_pair)");
#endif /* TRIE_COMPACT_PAIRS && GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_retry_struct_in_new_pair, e)
    dprintf("trie_retry_struct_in_new_pair\n");
#if defined(TRIE_COMPACT_PAIRS) && !defined(GLOBAL_TRIE)
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = (CELL *) (B + 1);
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);
    Functor func = (Functor) RepAppl(TrNode_entry(node));
    int func_arity = ArityOfFunctor(func);

    restore_trie_node(TrNode_next(node));
    stack_trie_struct_in_new_pair_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_retry_struct_in_new_pair)");
#endif /* TRIE_COMPACT_PAIRS && GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_do_extension, e)
    dprintf("trie_do_extension\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
    int heap_arity = *aux_stack_ptr;

    stack_trie_extension_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_do_extension)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_trust_extension, e)
    dprintf("trie_trust_extension\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = (CELL *) (B + 1);
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    pop_trie_node();
    stack_trie_extension_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_trust_extension)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_try_extension, e)
    dprintf("trie_try_extension\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    store_trie_node(TrNode_next(node));
    stack_trie_extension_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_try_extension)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_retry_extension, e)
    dprintf("trie_retry_extension\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = (CELL *) (B + 1);
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);

    restore_trie_node(TrNode_next(node));
    stack_trie_extension_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_retry_extension)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  PBOp(trie_do_float, e)
    dprintf("trie_do_float\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);
    volatile Float dbl;
    volatile Term *t_dbl = (Term *)((void *) &dbl);
    Term t;

#if SIZEOF_DOUBLE == 2 * SIZEOF_INT_P
    heap_arity -= 4;
    t_dbl[0] = *++aux_stack_ptr;
    ++aux_stack_ptr;  /* jump the float/longint extension mark */
    t_dbl[1] = *++aux_stack_ptr;
#else /* SIZEOF_DOUBLE == SIZEOF_INT_P */
    heap_arity -= 2;
    *t_dbl = *++aux_stack_ptr;
#endif /* SIZEOF_DOUBLE x SIZEOF_INT_P */
    ++aux_stack_ptr;  /* jump the float/longint extension mark */
    t = MkFloatTerm(dbl);
    stack_trie_float_longint_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_do_float)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  BOp(trie_trust_float, e)
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_trust_float)");
  ENDBOp();


  BOp(trie_try_float, e)
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_try_float)");
  ENDBOp();


  BOp(trie_retry_float, e)
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_retry_float)");
  ENDBOp();


  PBOp(trie_do_long, e)
    dprintf("trie_do_long\n");
#ifndef GLOBAL_TRIE
    register ans_node_ptr node = (ans_node_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);
    Term t;

    heap_arity -= 2;
    t = MkLongIntTerm(*++aux_stack_ptr);
    ++aux_stack_ptr;  /* jump the float/longint extension mark */
    stack_trie_float_longint_instr();
#else
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_do_long)");
#endif /* GLOBAL_TRIE */
  ENDPBOp();


  BOp(trie_trust_long, e)
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_trust_long)");
  ENDBOp();


  BOp(trie_try_long, e)
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_try_long)");
  ENDBOp();


  BOp(trie_retry_long, e)
    Yap_Error(INTERNAL_ERROR, TermNil, "invalid instruction (trie_retry_long)");
  ENDBOp();
  
#define store_hash_node()                             \
        { register choiceptr cp;                      \
          YENV = (CELL *)(HASH_CP(YENV) - 1);         \
          cp = NORM_CP(YENV);                         \
          HBREG = H;                                  \
          store_yaam_reg_cpdepth(cp);                 \
          cp->cp_tr = TR;                             \
          cp->cp_h = H;                               \
          cp->cp_b = B;                               \
          cp->cp_cp = CPREG;                          \
          cp->cp_ap = TRIE_RETRY_HASH;                \
          cp->cp_env = ENV;                           \
          B = cp;                                     \
          YAPOR_SET_LOAD(B);                          \
          SET_BB(B);                                  \
          TABLING_ERRORS_check_stack;                 \
        }                                             \
        if(heap_arity)                                \
          aux_stack_ptr--;                            \
        else                                          \
          aux_stack_ptr -= (2 + subs_arity);          \
        copy_arity_stack()
  
#define restore_hash_node()                               \
    /* restore choice point */                            \
    H = HBREG = PROTECT_FROZEN_H(B);                      \
    restore_yaam_reg_cpdepth(B);                          \
    CPREG = B->cp_cp;                                     \
    ENV = B->cp_env;                                      \
    YENV = (CELL *)PROTECT_FROZEN_B(B);                   \
    SET_BB(NORM_CP(YENV));                                \
                                                          \
    register CELL *aux_stack_ptr = (CELL *)(hash_cp + 1); \
    int heap_arity = *aux_stack_ptr;                      \
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);   \
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);   \
    copy_arity_stack()
    
#define pop_hash_node()                                   \
    hash_cp++;                                            \
    YENV = (CELL *) PROTECT_FROZEN_B((choiceptr)(hash_cp));    \
    H = PROTECT_FROZEN_H(B);                              \
    pop_yaam_reg_cpdepth(B);                              \
    CPREG = B->cp_cp;                                     \
    TABLING_close_alt(B);                                 \
    ENV = B->cp_env;                                      \
    B = B->cp_b;                                          \
    HBREG = PROTECT_FROZEN_H(B);                          \
    SET_BB(PROTECT_FROZEN_B(B));                          \
    if ((choiceptr) YENV == B_FZ) {                       \
      dprintf("Loop!\n");                                 \
    } else {                                              \
      CELL *aux_stack_ptr = YENV;                         \
      int heap_arity = *aux_stack_ptr;                    \
      int vars_arity = *(aux_stack_ptr + heap_arity + 1); \
      int subs_arity = *(aux_stack_ptr + heap_arity + 2); \
      YENV += CALCULATE_STACK_SIZE();                     \
    }
  
  BOp(trie_do_hash, e)
    register tst_ans_hash_ptr hash = (tst_ans_hash_ptr) PREG;
    register CELL *aux_stack_ptr = YENV;
    int heap_arity = *aux_stack_ptr;
    int vars_arity = *(aux_stack_ptr + heap_arity + 1);
    int subs_arity = *(aux_stack_ptr + heap_arity + 2);
    
    dprintf("trie_do_hash\n");
    
    if(heap_arity)
      aux_stack_ptr++;
    else
      aux_stack_ptr += 2 + subs_arity;
    
    CELL term = Deref(*aux_stack_ptr);
    
    if(IsVarTerm(term)) {
      store_hash_node();
      
      hash_cp_ptr hash_cp = HASH_CP(B);
      
      hash_cp->last_bucket = TSTHT_buckets(hash);
      
      // find first valid bucket
      while(!*(hash_cp->last_bucket))
        hash_cp->last_bucket++;
      
      hash_cp->is_variable = TRUE;
      hash_cp->hash = hash;
      
      dprintf("trie_do_hash is variable\n");
      
      next_node_instruction(*(hash_cp->last_bucket));
    } else {
      switch(cell_tag(term)) {
        case TAG_ATOM:
        case TAG_INT:
          break;
        case TAG_STRUCT:
          term = EncodeTrieFunctor(term);
          break;
        case TAG_LIST:
          term = EncodeTrieList(term);
          break;
        default:
          break;
      }
      
      dprintf("trie_do_hash other\n");
      
      tst_node_ptr *bucket_ptr = Hash_bucket(hash, HASH_ENTRY(term, Hash_seed(hash)));
      tst_node_ptr *var_bucket_ptr = Hash_bucket(hash, TRIEVAR_BUCKET);
      tst_node_ptr bucket = *bucket_ptr;
      tst_node_ptr var_bucket = *var_bucket_ptr;
      
      if(bucket == NULL && var_bucket == NULL) {
        dprintf("trie_do_hash all null\n");
        goto fail;
      }
      
      if(bucket != NULL && var_bucket != NULL) {
        dprintf("trie_do_hash two\n");
        store_hash_node();

        hash_cp_ptr hash_cp = HASH_CP(B);
        
        hash_cp->is_variable = FALSE;
        hash_cp->last_bucket = var_bucket_ptr;
        
        next_node_instruction(bucket);
      }
      
      /* run the valid bucket */
      if(bucket) {
        dprintf("trie_do_hash concrete\n");
        next_node_instruction(bucket);
      }
      else {
        dprintf("trie_do_hash var\n");
        next_node_instruction(var_bucket);
      }
    }
  ENDBOp();
  
  BOp(trie_retry_hash, e)
    hash_cp_ptr hash_cp = HASH_CP(B);
    
  dprintf("trie_retry_hash\n");
  
    if(hash_cp->is_variable) {
      dprintf("is variable\n");
      tst_ans_hash_ptr hash = hash_cp->hash;
      tst_node_ptr *last_bucket = TSTHT_buckets(hash) + TSTHT_num_buckets(hash);
      
      do {
        hash_cp->last_bucket++;
        
        if(last_bucket == hash_cp->last_bucket) {
          dprintf("No more items in hash table\n");
          pop_hash_node();
          goto fail; /* it is over */
        }
        
        if(*(hash_cp->last_bucket)) {
          restore_hash_node();
          next_node_instruction(*(hash_cp->last_bucket));
        }
      } while(TRUE);
    } else {
      dprintf("is other\n");
      /* consume the var bucket */
      pop_hash_node();
        
      next_node_instruction(*(hash_cp->last_bucket));
    }
  ENDBOp();
