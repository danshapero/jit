
typedef struct {
    int len, capacity;
    int *entries;
} Stack;


void initStack(Stack *s);
void destroyStack(Stack *s);

void push(Stack *s, int k);
int getEntry(Stack *s, int i);
void setEntry(Stack *s, int k, int i);
int pop(Stack *s);
