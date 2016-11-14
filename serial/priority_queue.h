

typedef struct node_t{
	int priority;
	char data;
	struct node_t* left;
	struct node_t* right;
};

typedef struct {
    struct node_t *nodes;
    int len;
    int size;
} heap_t;


heap_t* init_priority_queue();
void push(heap_t *heap, struct node_t *node);
struct node_t* pop(heap_t* heap);
