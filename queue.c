#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "queue.h"

void queue_init(QUEUE* queue) {
    queue->front = 0;
    queue->rear = 0;
    queue->size = 0;
}

bool queue_is_empty(QUEUE* queue) {
    return queue->size == 0;
}

void queue_push_rear(QUEUE* queue, CELL item) {
    queue->data[queue->rear] = item;
    queue->rear = (queue->rear + 1) % QUEUE_SIZE;
    queue->size++;
}

CELL queue_pop_front(QUEUE* queue) {
    CELL item = queue->data[queue->front];
    queue->front = (queue->front + 1) % QUEUE_SIZE;
    queue->size--;

    return item;
}

// -------------------TEST / USAGE ------------------------- //
int test_queue() {
    QUEUE queue;
    queue_init(&queue);

    CELL value1 = {0, 0};
    CELL value2 = {7, 7};
    CELL value3 = {15, 15};
    queue_push_rear(&queue, value1);
    queue_push_rear(&queue, value2);
    queue_push_rear(&queue, value3);
    for (int i = 0; i < 3; i++) {
        CELL pop_value = queue_pop_front(&queue);
        printf("Read [%d, %d]\n", pop_value.r, pop_value.c);
    }

    return 0;
}
