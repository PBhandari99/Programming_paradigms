#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
    assert(elemSize >= 0);
    assert(initialAllocation > 0);
    v->elemSize = elemSize;
    v->maxVectorLength = initialAllocation;
    v->vectorLength = 0;
    v->elements = malloc(initialAllocation * elemSize);
    assert(v->elements != NULL);
    v->freeFn = freeFn; 
}


void VectorDispose(vector *v)
{
    if (v->freeFn != NULL) {
        for (int i=0; i<v->vectorLength; ++i) {
            v->freeFn((char*)v->elements + (i*v->elemSize));
        }
    }
    free(v->elements);
}


int VectorLength(const vector *v)
{ 
    return v->vectorLength; 
}


void *VectorNth(const vector *v, int position)
{ 
    assert(position>=0 || position < v->vectorLength);
    return ((char*)v->elements + (position * v->elemSize));
}


void VectorReplace(vector *v, const void *elemAddr, int position)
{
    assert(position>=0 || position < v->vectorLength);
    void* targetAddress = (char*)v->elements + (position * v->elemSize);
    if(v->freeFn != NULL) {
        v->freeFn(targetAddress);
    }
    memcpy(targetAddress, elemAddr, v->elemSize);
}


void VectorInsert(vector *v, const void *elemAddr, int position)
{
    assert(position >= 0 || position < v->vectorLength);
    if (v->vectorLength == v->maxVectorLength) {
        v->maxVectorLength = 2*v->maxVectorLength;
        v->elements = realloc(v->elements, v->maxVectorLength*v->elemSize);
        assert(v->elements != NULL);
    }
    void* targetAddress = (char*)v->elements + (v->elemSize*position);
    int sizetoMove = (v->vectorLength - position)*v->elemSize;
    char buffer[sizetoMove];
    memcpy(buffer,targetAddress, sizetoMove);
    memmove(targetAddress, elemAddr, v->elemSize);
    memcpy((char*)targetAddress+v->elemSize, buffer, sizetoMove);
    v->vectorLength++;
}


void VectorAppend(vector *v, const void *elemAddr)
{
    if (v->vectorLength == v->maxVectorLength) {
        v->maxVectorLength = 2*v->maxVectorLength;
        v->elements = realloc(v->elements, v->maxVectorLength*v->elemSize);
        assert(v->elements != NULL);
    }
    void* targetAddress = (char*)v->elements + ((v->vectorLength) * v->elemSize);
    memcpy(targetAddress, elemAddr, v->elemSize);
    v->vectorLength++;
}


void VectorDelete(vector *v, int position)
{
    assert(position>=0 || position < v->vectorLength);
    void* targetAddress = (char*)v->elements + (position*v->elemSize);
    if(v->freeFn != NULL) {
        v->freeFn(targetAddress);
    }
    int sizetoMove = (v->vectorLength - position -1)*v->elemSize;
    memmove(targetAddress, (char*)targetAddress+v->elemSize, sizetoMove);
    v->vectorLength--;
}


void VectorSort(vector *v, VectorCompareFunction compare)
{
    assert(compare != NULL);
    qsort(v->elements, v->vectorLength, v->elemSize, compare);
    return;
}


void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{
    assert(mapFn != NULL);
    for (int i=0; i < v->vectorLength; i++) {
        mapFn((char*)v->elements+(i*v->elemSize), auxData);
    } 
    return;
}


static const int kNotFound = -1;

int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex,
        bool isSorted)
{ 
    assert(startIndex>=0 || startIndex < (v->vectorLength) || searchFn != NULL);
    if (isSorted) {
        void* found = bsearch(key, (char*)v->elements + (startIndex*v->elemSize),
                v->vectorLength-startIndex, v->elemSize, searchFn);
        if (found != NULL){
            return ((char*)v->elements - (char*)found)/v->elemSize;
        }
        return kNotFound;
    }
    for (int i = startIndex; i<v->vectorLength; i++) {
        if (searchFn((char*)v->elements + (i*v->elemSize), key) == 0) {
            return i;
        }
    }
    return kNotFound;
} 



