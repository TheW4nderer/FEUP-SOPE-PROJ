#pragma once

#include <stdlib.h>

typedef struct place_queue
{
    unsigned front, back, size, capacity;
    unsigned *array;
} place_queue;

place_queue createPlaceQueue(unsigned capacity) {
    place_queue pq;
    pq.capacity = capacity;
    pq.size = 0;
    pq.front = 0;
    pq.back = capacity - 1;
    pq.array = (unsigned*) malloc(capacity * sizeof(unsigned));
    return pq;
}

int isFull(place_queue* pq) {
    return (pq->size == pq->capacity);
}

int isEmpty(place_queue* pq) {
    return (pq->size == 0);
}

void makePlaceAvailable(place_queue* pq, unsigned item) {
    if (isFull(pq)) {
        return;
    }
    pq->back = (pq->back + 1) % pq->capacity;
    pq->array[pq->back] = item;
    pq->size++;
}

unsigned usePlace(place_queue* pq) {
    if (isEmpty(pq)) {
        return 0;
    }
    unsigned item = pq->array[pq->front];
    pq->front = (pq->front + 1) % pq->capacity;
    pq->size--;
    return item;
}

void fillPlaceQueue(place_queue* pq) {
    for (unsigned i = 1; i <= pq->capacity; i++) {
        makePlaceAvailable(pq, i);
    }
}