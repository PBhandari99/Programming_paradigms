#include "genericStack.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

void stackNew(stack* s, int elem_size) {
    assert(elem_size>0);
    s->logical_length = 0;
    s->allocated_length = 4;
    s->elem_size = elem_size;
    s->elems = malloc(4 * elem_size);
    assert(s->elems!=NULL);
}

void stackPush(stack* s, void* value_addr) {
    if (s->logical_length == s->allocated_length) {
        s->allocated_length *= 2;
        s->elems = realloc(s->elems, s->allocated_length*s->elem_size);
        assert(s->elems != NULL);
    }
    void* target_addr = (char*)s->elems + (s->logical_length * s->elem_size); 
    memcpy(target_addr, value_addr, s->elem_size);
    s->logical_length++;
}

void stackPop(stack* s, void* buffer) {
    s->logical_length--;
    void* source_addr = (char*)s->elems + (s->logical_length * s->elem_size);
    memcpy(buffer, source_addr, s->elem_size);
}
