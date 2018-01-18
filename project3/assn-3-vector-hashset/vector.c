#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation)
{
    assert(elemSize > 0);
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
    return NULL; 
}

void VectorReplace(vector *v, const void *elemAddr, int position)
{}

void VectorInsert(vector *v, const void *elemAddr, int position)
{


}

void VectorAppend(vector *v, const void *elemAddr)
{
    if (v->vectorLength == v->maxVectorLength) {
        v->maxVectorLength = 2*v->maxVectorLength;
        v->elements = realloc(v->elements, v->maxVectorLength);
        assert(v->elements != NULL);
    }
    void* targetAddress = (char*)v->elements + ((v->vectorLength-1) * v->elemSize);
    memcpy(targetAddress, elemAddr, v->elemSize);
    v->vectorLength++;
}

void VectorDelete(vector *v, int position)
{}

void VectorSort(vector *v, VectorCompareFunction compare)
{}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData)
{}

static const int kNotFound = -1;

int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex, bool isSorted)
{ return kNotFound; } 
