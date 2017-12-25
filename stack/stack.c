#include "stack.h"

#include <malloc.h>
#include <assert.h>

void stackNew(stack* s) {
    s->logical_len = 0;
    s->alloc_length = 4;
    s->elems = malloc(4*sizeof(int));
    assert(s->elems != NULL);
}

void stackDispose(stack* s) {
    free(s->elems);
}

void stackPush(stack* s, int value) {
    if (s->logical_len == s->alloc_length) {
        s->alloc_length *= 2;
        s->elems = realloc(s->elems, s->alloc_length*sizeof(int));
        assert(s->elems != NULL);
    }
    s->elems[s->logical_len] = value;
}

int stackPop(stack* s) {
    assert(s->logical_len > 0);
    s->logical_len--;
    return s->elems[s->logical_len];
}
