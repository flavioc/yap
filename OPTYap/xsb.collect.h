typedef enum {
  DESCEND_MODE,
  BACKTRACK_MODE
} SearchMode;

typedef struct {
	void* alt_node; /* sibling of the node whose child ptr we took */
	int ts_top_index; /* current top-of-tstTermStack at CP creation */
	int log_top_index; /* current top-of-tstTermStackLog at CP creation */
	tr_fr_ptr trail_top; /* current top-of-trail at CP creation */
	CPtr heap_bktrk; /* current hbreg at time of CP creation */
} collectChoicePointFrame;

#define COLLECT_CPSTACK_SIZE 1024

struct collectCPStack_t {
	collectChoicePointFrame *top; /* next available location to place an entry */
	collectChoicePointFrame *ceiling; /* overflow pointer: points beyond array end */
	collectChoicePointFrame base[COLLECT_CPSTACK_SIZE];
};

extern struct collectCPStack_t collectCPStack;

#define collectTop        (collectCPStack.top)
#define collectBase       (collectCPStack.base)
#define collectCeiling    (collectCPStack.ceiling)

void initCollectStack(CTXTdecl);