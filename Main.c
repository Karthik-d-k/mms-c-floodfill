#include <stdint.h>
#include <stdio.h>

#include "API.h"
#include "floodfill.h"
#include "maze.h"
#include "queue.h"

extern CELL CURRENT_CELL;
extern ABSOLUTE_DIRECTION CURRENT_ABSOLUTE_DIRECTION;

void init() {
    init_maze();
}

void log_text(const char* text) {
    fprintf(stderr, "%s\n", text);
    fflush(stderr);
}

int main(int argc, char* argv[]) {
    init();
    log_text("Running...");
    API_setColor(0, 0, 'G');
    API_setText(0, 0, "START");
    API_setColor(7, 7, 'R');
    API_setText(7, 7, "END");

    CELL target = END;
    floodfill(target);
    while ((CURRENT_CELL.r != target.r) || (CURRENT_CELL.c != target.c)) {
        WallState front_wall = API_wallFront() ? WALL_PRESENT : WALL_ABSENT;
        WallState right_wall = API_wallRight() ? WALL_PRESENT : WALL_ABSENT;
        WallState left_wall = API_wallLeft() ? WALL_PRESENT : WALL_ABSENT;

        CURRENT_CELL = neighbour_cell(CURRENT_CELL, CURRENT_ABSOLUTE_DIRECTION);
        update_walls(front_wall, right_wall, left_wall);
        floodfill(target);
        ABSOLUTE_DIRECTION new_direction = smallest_neighbour_cell(CURRENT_CELL, CURRENT_ABSOLUTE_DIRECTION);
        ABSOLUTE_DIRECTION direction_change = (new_direction - CURRENT_ABSOLUTE_DIRECTION) & 0x3;

        if ((CURRENT_CELL.r != target.r) || (CURRENT_CELL.c != target.c)) {
            switch (direction_change) {
            case AHEAD:
                API_moveForward();
                CURRENT_CELL = neighbour_cell(CURRENT_CELL, CURRENT_ABSOLUTE_DIRECTION);
                break;
            case RIGHT:
                API_turnRight();
                CURRENT_ABSOLUTE_DIRECTION = right_from(CURRENT_ABSOLUTE_DIRECTION);
                API_moveForward();
                CURRENT_CELL = neighbour_cell(CURRENT_CELL, CURRENT_ABSOLUTE_DIRECTION);
                break;
            case LEFT:
                API_turnLeft();
                CURRENT_ABSOLUTE_DIRECTION = left_from(CURRENT_ABSOLUTE_DIRECTION);
                API_moveForward();
                CURRENT_CELL = neighbour_cell(CURRENT_CELL, CURRENT_ABSOLUTE_DIRECTION);
                break;
            case BACK:
                API_turnRight();
                API_turnRight();
                CURRENT_ABSOLUTE_DIRECTION = behind_from(CURRENT_ABSOLUTE_DIRECTION);
                API_moveForward();
                CURRENT_CELL = neighbour_cell(CURRENT_CELL, CURRENT_ABSOLUTE_DIRECTION);
                break;
            }
        }
    }
}
