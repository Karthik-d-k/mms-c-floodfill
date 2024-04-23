#include <stdio.h>

#include "floodfill.h"
#include "maze.h"
#include "queue.h"

uint8_t COST[MAZE_SIZE][MAZE_SIZE];
extern WallInfo WALLS[MAZE_SIZE][MAZE_SIZE];

void floodfill(const CELL target) {
    // 1. Set all cells cost to "blank state" (255)
    for (int r = 0; r < MAZE_SIZE; r++) {
        for (int c = 0; c < MAZE_SIZE; c++) {
            COST[r][c] = (uint8_t)MAX_COST;
        }
    }

    // 2. Set target cell cost value to 0 and add target cell to queue
    QUEUE queue;
    queue_init(&queue);
    COST[target.r][target.c] = 0;
    queue_push_rear(&queue, target);

    // 3. While queue is not empty
    while (!queue_is_empty(&queue)) {
        // 3a. Take first pushed cell from queue
        CELL current_cell = queue_pop_front(&queue);
        // 3b. Calculate new cell cost value based on current cell
        uint16_t newCost = COST[current_cell.r][current_cell.c] + 1;

        // 3c. Set all "blank" and "accessible(no wall)" cells cost to new cost
        for (int direction = 0; direction <= 3; direction++) {
            if (is_cell_accessible(current_cell, (ABSOLUTE_DIRECTION)direction)) {
                CELL nextCell = neighbour_cell(current_cell, (ABSOLUTE_DIRECTION)direction);
                if (COST[nextCell.r][nextCell.c] > newCost) {
                    COST[nextCell.r][nextCell.c] = newCost;
                    // 3d. Add cells we just processed to queue
                    queue_push_rear(&queue, nextCell);
                }
            }
        }
    }
}

// -------------------TEST / USAGE ------------------------- //
int test_floodfill() {
    CELL goal = {7, 7};
    floodfill(goal);
    print_maze(COST);

    return 0;
}
