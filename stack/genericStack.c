#include "genericStack.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

/* The 3rd paramaeter is a function pointer that will free any dymanically
 * allocated memory whole addresses are in the elems. for example:
 * if the elems contains char strings, they have to be freed saperately
 * from the s->elems. */
void stackNew(stack* s, int elem_size, void(*freeFunc)(void*)) {
    assert(elem_size>0);
    s->logical_length = 0;
    s->allocated_length = 4;
    s->elem_size = elem_size;
    s->elems = malloc(4 * elem_size);
    assert(s->elems!=NULL);
    s->freeFunc = freeFunc;
}

void stackDecompose(stack* s) {
    if(s->freeFunc != NULL) {
        for(int i=0; i<s->logical_length; ++i) {
            s->freeFunc((char*)s->elems + (i*s->elem_size));
        }
    }
}

void stackPush(stack* s, void* value_addr) {
    /* If the stack is full and more memory needs to be allocated. */
    if (s->logical_length == s->allocated_length) {
        s->allocated_length *= 2;
        s->elems = realloc(s->elems, s->allocated_length*s->elem_size);
        assert(s->elems != NULL);
    }
    /* target_addr when the new element is to be pushed. */
    void* target_addr = (char*)s->elems + (s->logical_length * s->elem_size); 
    memcpy(target_addr, value_addr, s->elem_size);
    s->logical_length++;
}

void stackPop(stack* s, void* buffer) {
    /* The decrement is done here because the logical_length is always the next to
     * the last element and the one to be poped is the last when is 1 less than
     * the logical_length. */
    s->logical_length--;

    /* source_addr from where the element is to be poped.
     * The cast to the char* is done so that the pointer arithmetic can be performed
     * in a very simple way as all char(s) are 1 bytes long, incrementing
     * char* will increament the pointer by 1 unit.
     */
    void* source_addr = (char*)s->elems + (s->logical_length * s->elem_size);
    memcpy(buffer, source_addr, s->elem_size);
}
