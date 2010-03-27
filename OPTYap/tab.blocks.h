
#ifndef TAB_BLOCKS_H
#define TAB_BLOCKS_H

#ifdef TABLING_ANSWER_BLOCKS

#define NEXT_BLOCK_MASK 0x01
#define UNTAG_BLOCK_MASK(PTR) ((unsigned int)(PTR) & ~(NEXT_BLOCK_MASK))
#define TAG_BLOCK_MASK(PTR) ((unsigned int)(PTR) | NEXT_BLOCK_MASK)
#define IS_BLOCK_TAG(PTR) ((unsigned int)(PTR) & NEXT_BLOCK_MASK)

#define block_new(BLOCK, SIZE) {                  \
  int new_size = (SIZE) + 1;                      \
  ALLOC_BLOCK(BLOCK, new_size * sizeof(void *));  \
  memset(BLOCK, 0, sizeof(void *) * new_size);  \
  BLOCK[SIZE] = (void*)NEXT_BLOCK_MASK;                  \
}  
  
#define block_free(BLOCK) FREE_BLOCK(BLOCK)

#define blocks_free(BLOCK, SIZE) {           \
    void* next;                              \
    void** block = (void **)(BLOCK);                  \
    while(block) {                           \
      next = (void*)UNTAG_BLOCK_MASK(block[SIZE]);  \
      block_free(block);                     \
      block = (void**)next;                  \
    }                                        \
  }
  
#define block_set(BLOCK_POS, ELEM, SIZE)  {     \
    if(IS_BLOCK_TAG(*(BLOCK_POS))) {            \
      void** new_block;                         \
      block_new(new_block, SIZE);               \
      *(BLOCK_POS) = (void*)TAG_BLOCK_MASK(new_block); \
      BLOCK_POS = new_block;                    \
    }                                           \
    *BLOCK_POS = ELEM;                          \
  }
  
#define block_push(ELEM, FIRST, LAST, SIZE, TYPE) { \
    if((FIRST) == NULL) { \
      void** new_block;   \
      block_new(new_block, SIZE); \
      new_block[0] = (void*)(ELEM); \
      LAST = FIRST = (TYPE)new_block; \
    } else { \
      void** next = (void**)++(LAST); \
      block_set(next, ELEM, SIZE);  \
      LAST = (TYPE)next;  \
    } \
  }

static inline void**
block_get_next(void **block_pos)
{
  void** next = ++block_pos;
  void* next_cont = *next;
  
  if(next_cont == NULL)
    return NULL;
  
  if(IS_BLOCK_TAG(next_cont))
    next = (void**)UNTAG_BLOCK_MASK(next_cont);
    
  return next;
}

#define block_has_next(BLOCK_POS) (unsigned int)*(++BLOCK_POS) > (unsigned int)NEXT_BLOCK_MASK;
  
#endif /* TABLING_ANSWER_BLOCKS */

#endif