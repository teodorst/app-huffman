

typedef struct {
	int priority;
	char *data;
} node_t;

typedef struct {
    node_t *nodes;
    int len;
    int size;
} heap_t;


heap_t* init_priority_queue();
void push(heap_t *heap, int priority, char* data);
char* pop(heap_t* heap);


