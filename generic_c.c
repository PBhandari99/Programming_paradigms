#include <stdio.h>
#include <string.h>
#include <assert.h>

int comp_fun(void*, void*);

void swap(void *vp1, void *vp2, int size) {
    char buffer[size];
    memcpy(buffer, vp1, size);
    memcpy(vp1, vp2, size);
    memcpy(vp2, buffer, size);
}

void* lSerach(void* key, void* base, int n, int elemSize) {
    for (int i=0; i<n; i++) {
        /* This method add i time elemSize to the address of base
         * which acts as a base-address.
         * - (char*): This type cast is done because it's confusing to
         *   do pointer arithmetic on void pointers and since char*
         *   are of size 1 byte it's easier. 
         * - And assigning a char* address to void* is legal.*/
        void* elemAddr = (char*)base + i*elemSize;
        if (memcmp(key, elemAddr, elemSize)==0)
            return elemAddr;
    }
    return NULL;
}


int int_comp(void* key, void* elem_addr) {
    /* The cast from void to int is not necessary since 
     * the address already contain int. */
    int* ip1 = key;
    int* ip2 = elem_addr;
    return *ip1 - *ip2;
}


/* This is samee as lsearch but more generic with the own 
 * implementation of the comparision function. */
void* lsearch2(void* key, void* base,
               int n, int elem_size,
               int(*comp_fun)(void*, void*)) {
    for (int i=0; i<n; i++) {
        void * elem_addr = (char*)base + i*elem_size;
        if (comp_fun(key, elem_addr)==0)
            return elem_addr;
    }
    return NULL;
}


int main() {
    int a = 44;
    int b = 55;
    swap(&a, &b, sizeof(int));
    printf("Value of a after swap is: %d.\n", a);
    printf("Value of b after swap is: %d.\n", b);

    int array[] = {4, 2, 3, 7, 11, 6};
    int size = 6;
    int number = 7;
    int* found = lsearch2(&number, array,
                          size, sizeof(int),
                          comp_fun);
    assert(found != NULL);
    return 0;
}
