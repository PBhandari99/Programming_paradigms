#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn,
                HashSetFreeFunction freefn)
{
    // Pointers to the vectors where the data is.
    h->elements = malloc(numBuckets*sizeof(vector*));
    for (int i=0; i<numBuckets; ++i) {
        void* target = (char*)h->elements + (i*sizeof(vector*)); 
        target = malloc(sizeof(vector));
        vector newVector;
        memcpy(target, &newVector, sizeof(vector));
        VectorNew((vector*)target, elemSize, freefn, 1);
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
        VectorDispose((vector*)(char*)h->elements + (i*sizeof(vector*)));
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
        void* targetVector = (char*)h->elements + (i*sizeof(vector*));
        int vectorLength = VectorLength((vector*)targetVector);
        if (vectorLength<0) {
            for (int j=0; j<vectorLength; ++j){
                void* elem = VectorNth((vector*)targetVector, j);
                mapfn(elem, auxData);
            } 
        }
    }
}


void HashSetEnter(hashset *h, const void *elemAddr)
{
    assert(elemAddr != NULL);
    int hash_loc = h->hashfn(elemAddr, h->numBuckets);
    assert(hash_loc >=0 && hash_loc < h->numBuckets);

    void* targetVector = (char*)h->elements + (hash_loc*sizeof(vector*));
    int vectorLength = VectorLength((vector*)targetVector);
    if(vectorLength>0) {
        for(int i=0; i<vectorLength; ++i) {
            int compare = h->comparefn(elemAddr, VectorNth((vector*)targetVector, i));
            if(compare==0) {
               VectorReplace((vector*)targetVector, elemAddr, i);
               return;
            }
        }        
    }
    VectorAppend((vector*)targetVector, elemAddr);
    h->setSize++;
}


void *HashSetLookup(const hashset *h, const void *elemAddr)
{ 
    assert(elemAddr == NULL);
    int hash_loc = h->hashfn(elemAddr, h->numBuckets);
    assert(hash_loc >=0 && hash_loc < h->numBuckets);
    void* targetVector = (char*)h->elements + (hash_loc*sizeof(vector*));

    int vectorLength = VectorLength((vector*)targetVector);
    if(vectorLength>0) {
        for(int i=0; i<vectorLength; ++i) {
            void* vectElement = VectorNth((vector*)targetVector, i);
            int compare = h->comparefn(elemAddr, VectorNth((vector*)targetVector, i));
            if(compare==0) {
               return vectElement;
            }
        }        
    }
    return NULL;
}
