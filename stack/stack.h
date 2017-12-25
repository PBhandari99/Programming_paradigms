typedef struct {
    int* elems;
    int logical_len;
    int alloc_length;
}stack;

void stackNew(stack* s);
void stackDispose(stack* s);
void stackPush(stack* s, int value);
int stackPop(stack* s);
