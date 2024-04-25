#include <stdint.h>
#include <stdio.h>

#include "API.h"
#include "floodfill.h"
#include "maze.h"
#include "queue.h"

extern CELL CURRENT_CELL;
extern ABSOLUTE_DIRECTION CURRENT_ABSOLUTE_DIRECTION;
extern uint8_t COST[MAZE_SIZE][MAZE_SIZE];
extern WallInfo WALLS[MAZE_SIZE][MAZE_SIZE];

void init() {
    init_maze();
}

void log_text(const char* text) {
    fprintf(stderr, "%s\n", text);
    fflush(stderr);
}

void log_walls(WallInfo walls) {
    char log[100];
    switch (CURRENT_ABSOLUTE_DIRECTION) {
    case NORTH:
        sprintf(log, "[ NORTH ]: [%d  %d  %d  %d]", walls.north, walls.east, walls.south, walls.west);
        log_text(log);
        break;
    case EAST:
        sprintf(log, "[ EAST  ]: [%d  %d  %d  %d]", walls.east, walls.south, walls.west, walls.north);
        log_text(log);
        break;
    case SOUTH:
        sprintf(log, "[ SOUTH ]: [%d  %d  %d  %d]", walls.south, walls.west, walls.north, walls.east);
        log_text(log);
        break;
    case WEST:
        sprintf(log, "[ WEST  ]: [%d  %d  %d  %d]", walls.west, walls.north, walls.east, walls.south);
        log_text(log);
        break;
    default:
        // Ignore any other directions
        break;
    }
}

void test_0_9() {
    CELL cell = {0, 9};
    CURRENT_CELL = cell;
    CURRENT_ABSOLUTE_DIRECTION = EAST;
    WallState front_wall = WALL_ABSENT;
    WallState right_wall = WALL_PRESENT;
    WallState left_wall = WALL_ABSENT;
    WallInfo walls = WALLS[CURRENT_CELL.r][CURRENT_CELL.c];
    printf("(%d, %d)\n", CURRENT_CELL.r, CURRENT_CELL.c);
    log_walls(walls);
    printf("[ SENS  ]: [0  0  1  2]\n");
    update_walls(front_wall, right_wall, left_wall);
    log_walls(walls);
}

// int main() {
//     // char log[100];
//     init();

//     test_0_9();
// }

int main() {
    char log[100];
    init();

    log_text("Running...\n");
    API_setColor(0, 0, 'G');
    API_setText(0, 0, "START");
    API_setColor(15, 15, 'R');
    API_setText(15, 15, "END");

    CELL target = END;
    while ((CURRENT_CELL.r != target.r) || (CURRENT_CELL.c != target.c)) {
        WallState front_wall = API_wallFront() ? WALL_PRESENT : WALL_ABSENT;
        WallState right_wall = API_wallRight() ? WALL_PRESENT : WALL_ABSENT;
        WallState left_wall = API_wallLeft() ? WALL_PRESENT : WALL_ABSENT;

        update_walls(front_wall, right_wall, left_wall);
        WallInfo walls = WALLS[CURRENT_CELL.r][CURRENT_CELL.c];

        floodfill(target);
        ABSOLUTE_DIRECTION new_direction = smallest_neighbour_cell(CURRENT_CELL, CURRENT_ABSOLUTE_DIRECTION);
        RELATIVE_DIRECTION direction_change = (new_direction - CURRENT_ABSOLUTE_DIRECTION) & 0x3;

        switch (direction_change) {
        case AHEAD:
            API_moveForward();
            break;
        case RIGHT:
            API_turnRight();
            API_moveForward();
            break;
        case BACK:
            API_turnRight();
            API_turnRight();
            API_moveForward();
            break;
        case LEFT:
            API_turnLeft();
            API_moveForward();
            break;
        default:
            // Ignore any other directions
            break;
        }

        sprintf(log, "cell: [%d,%d]", CURRENT_CELL.r, CURRENT_CELL.c);
        log_text(log);
        sprintf(log, "[SENSORS]: [%d  %d  X  %d]", front_wall, right_wall, left_wall);
        log_text(log);
        log_walls(walls);
        sprintf(log, "[DIR (B)]: %d", CURRENT_ABSOLUTE_DIRECTION);
        log_text(log);

        // update to new direction and new cell
        CURRENT_ABSOLUTE_DIRECTION = new_direction;
        CURRENT_CELL = neighbour_cell(CURRENT_CELL, CURRENT_ABSOLUTE_DIRECTION);

        sprintf(log, "[DIR (A)]: %d", CURRENT_ABSOLUTE_DIRECTION);
        log_text(log);
    }

    log_text("SUCCESS !!");
}
