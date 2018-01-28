#include "hashset.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn,
                HashSetFreeFunction freefn)
{
    // Pointers to the vectors where the data is.
    h->elements = (vector*)malloc(numBuckets*sizeof(vector));
    for (int i=0; i<numBuckets; i++) {
        printf("%d\n", i);
        vector* target = (h->elements + i);
        VectorNew(target, elemSize, freefn, 1);
    }
    h->setSize = 0;
    h->elemSize = elemSize;
    h->numBuckets = numBuckets;
    h->hashfn = hashfn;
    h->comparefn = comparefn;
}


void HashSetDispose(hashset *h)
{
    for (int i=0; i<h->numBuckets; ++i) {
        VectorDispose(h->elements + i);
    }
    free(h->elements);
    h->setSize = 0;
}


int HashSetCount(const hashset *h)
{ 
    return h->setSize;
}


void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData)
{
    assert(mapfn != NULL);
    for (int i=0; i<h->numBuckets; ++i) {
        vector* targetVector = h->elements + i;
        VectorMap(targetVector, mapfn, auxData);
    }
}


void HashSetEnter(hashset *h, const void *elemAddr)
{
    assert(elemAddr != NULL);
    int hash_loc = h->hashfn(elemAddr, h->numBuckets);
    assert(hash_loc >=0 && hash_loc < h->numBuckets);

    vector* targetVector = h->elements + (hash_loc);
    int vectorLength = VectorLength(targetVector);
    if(vectorLength>0) {
        for(int i=0; i<vectorLength; ++i) {
            int compare = h->comparefn(elemAddr, VectorNth(targetVector, i));
            if(compare==0) {
               VectorReplace(targetVector, elemAddr, i);
               return;
            }
        }        
    }
    VectorAppend(targetVector, elemAddr);
    h->setSize++;
}


void *HashSetLookup(const hashset *h, const void *elemAddr)
{ 
    assert(elemAddr != NULL);
    int hash_loc = h->hashfn(elemAddr, h->numBuckets);
    assert(hash_loc >=0 && hash_loc < h->numBuckets);
    vector* targetVector = h->elements + (hash_loc);
    int vectorLength = VectorLength(targetVector);
    if(vectorLength>0) {
        for(int i=0; i<vectorLength; ++i) {
            void* vectElement = VectorNth(targetVector, i);
            int compare = h->comparefn(elemAddr, VectorNth(targetVector, i));
            if(compare==0) {
               return vectElement;
            }
        }        
    }
    return NULL;
}
