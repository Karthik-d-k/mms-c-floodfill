#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stdint.h>

#include "maze.h"

#define QUEUE_SIZE 256 // Maximum number of elements of the circular buffer

typedef struct {
    CELL data[QUEUE_SIZE];
    uint8_t front;
    uint8_t rear;
    uint8_t size;
} QUEUE;

void queue_init(QUEUE* queue);

bool queue_is_empty(QUEUE* queue);

void queue_push_rear(QUEUE* queue, CELL item);

CELL queue_pop_front(QUEUE* queue);

int test_queue();

#endif
