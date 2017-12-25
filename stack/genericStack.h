typedef struct{
    void* elems;
    int logical_length;
    int allocated_length;
    int elem_size;
}stack;

void stackNew(stack* s, int elem_size);
void stackDecompose(stack *s);
void stackPush(stack* s, void* value_addr);
void stackPop(stack* s, void* buffer);
