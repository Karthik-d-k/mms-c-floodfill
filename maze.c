#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "maze.h"

WallInfo WALLS[MAZE_SIZE][MAZE_SIZE];
MazeMask MAZE_MASK = MASK_TREAT_UNSEEN_AS_ABSENT;
CELL CURRENT_CELL = {0, 0};
ABSOLUTE_DIRECTION CURRENT_ABSOLUTE_DIRECTION = NORTH;
extern uint8_t COST[MAZE_SIZE][MAZE_SIZE];

inline ABSOLUTE_DIRECTION right_from(const ABSOLUTE_DIRECTION direction) {
    return (ABSOLUTE_DIRECTION)((direction + 1) % ABS_DIR_COUNT);
}

inline ABSOLUTE_DIRECTION left_from(const ABSOLUTE_DIRECTION direction) {
    return (ABSOLUTE_DIRECTION)((direction + ABS_DIR_COUNT - 1) % ABS_DIR_COUNT);
}

inline ABSOLUTE_DIRECTION ahead_from(const ABSOLUTE_DIRECTION direction) {
    return direction;
}

inline ABSOLUTE_DIRECTION behind_from(const ABSOLUTE_DIRECTION direction) {
    return (ABSOLUTE_DIRECTION)((direction + 2) % ABS_DIR_COUNT);
}

void log_maze(const char* text) {
    fprintf(stderr, "%s\n", text);
    fflush(stderr);
}

void init_walls() {
    for (int r = 0; r < MAZE_SIZE; r++) {
        for (int c = 0; c < MAZE_SIZE; c++) {
            WALLS[r][c].north = WALL_UNSEEN;
            WALLS[r][c].east = WALL_UNSEEN;
            WALLS[r][c].south = WALL_UNSEEN;
            WALLS[r][c].west = WALL_UNSEEN;
        }
    }
    for (int r = 0; r < MAZE_SIZE; r++) {
        WALLS[r][0].south = WALL_PRESENT;
        WALLS[r][MAZE_SIZE - 1].north = WALL_PRESENT;
    }
    for (int c = 0; c < MAZE_SIZE; c++) {
        WALLS[0][c].west = WALL_PRESENT;
        WALLS[MAZE_SIZE - 1][c].east = WALL_PRESENT;
    }

    WALLS[0][0].north = WALL_ABSENT;
    WALLS[0][0].east = WALL_PRESENT;
    WALLS[0][0].west = WALL_PRESENT;
    WALLS[0][0].south = WALL_PRESENT;
}

bool is_cell_accessible(CELL cell, ABSOLUTE_DIRECTION direction) {
    bool result = false;
    WallInfo walls = WALLS[cell.r][cell.c];
    switch (direction) {
    case NORTH:
        result = (walls.north & MAZE_MASK) == WALL_ABSENT;
        break;
    case EAST:
        result = (walls.east & MAZE_MASK) == WALL_ABSENT;
        break;
    case SOUTH:
        result = (walls.south & MAZE_MASK) == WALL_ABSENT;
        break;
    case WEST:
        result = (walls.west & MAZE_MASK) == WALL_ABSENT;
        break;
    default:
        result = false;
        break;
    }

    return result;
}

CELL neighbour_cell(CELL cell, ABSOLUTE_DIRECTION direction) {
    switch (direction) {
    case NORTH: {
        int16_t new_y = cell.c + 1;
        if (new_y >= MAZE_SIZE) {
            return cell;
        }
        CELL neighbour_cell = {cell.r, new_y};
        return neighbour_cell;
    }
    case EAST: {
        int16_t new_x = cell.r + 1;
        if (new_x >= MAZE_SIZE) {
            return cell;
        }
        CELL neighbour_cell = {new_x, cell.c};
        return neighbour_cell;
    }
    case SOUTH: {
        int16_t new_y = cell.c - 1;
        if (new_y < 0) {
            return cell;
        }
        CELL neighbour_cell = {cell.r, new_y};
        return neighbour_cell;
    }
    case WEST: {
        int16_t new_x = cell.r - 1;
        if (new_x < 0) {
            return cell;
        }
        CELL neighbour_cell = {new_x, cell.c};
        return neighbour_cell;
    }
    default:
        // Invalid direction, return current cell
        return cell;
    }
}

uint8_t cost_neighbour_cell(const CELL cell, const ABSOLUTE_DIRECTION direction) {
    char log[100];
    WallInfo walls = WALLS[cell.r][cell.c];
    if (!is_cell_accessible(cell, direction)) {
        sprintf(log, "[fucked]: %2d, %2d  %6d [%d %d %d %d]", cell.r, cell.c, direction, walls.north, walls.east, walls.south, walls.west);
        log_maze(log);
        return MAX_COST;
    }
    sprintf(log, "[fucked]: %2d, %2d  %6d [%d %d %d %d]", cell.r, cell.c, direction, walls.north, walls.east, walls.south, walls.west);
    log_maze(log);
    CELL next_cell = neighbour_cell(cell, direction);

    return COST[next_cell.r][next_cell.c];
}

ABSOLUTE_DIRECTION smallest_neighbour_cell(const CELL cell, const ABSOLUTE_DIRECTION start_direction) {
    char log[100];
    ABSOLUTE_DIRECTION next_direction = start_direction;
    ABSOLUTE_DIRECTION best_direction = BLOCKED;
    uint8_t best_cost = COST[cell.r][cell.c];
    uint8_t cost = 0;

    cost = cost_neighbour_cell(cell, next_direction);
    if (cost < best_cost) {
        best_cost = cost;
        best_direction = next_direction;
    };

    // sprintf(log, "[start_from ]: %2d, %2d  %6d  %6d", cell.r, cell.c, best_cost, best_direction);
    // log_maze(log);

    next_direction = right_from(start_direction);
    cost = cost_neighbour_cell(cell, next_direction);
    // sprintf(log, "{   %6d, %6d   }", next_direction, cost);
    // log_maze(log);

    if (cost < best_cost) {
        best_cost = cost;
        best_direction = next_direction;
    };

    // sprintf(log, "[right_from ]: %2d, %2d  %6d  %6d", cell.r, cell.c, best_cost, best_direction);
    // log_maze(log);

    next_direction = behind_from(start_direction);
    cost = cost_neighbour_cell(cell, next_direction);
    if (cost < best_cost) {
        best_cost = cost;
        best_direction = next_direction;
    };

    // sprintf(log, "[behind_from]: %2d, %2d  %6d  %6d", cell.r, cell.c, best_cost, best_direction);
    // log_maze(log);

    next_direction = left_from(start_direction);
    cost = cost_neighbour_cell(cell, next_direction);
    if (cost < best_cost) {
        best_cost = cost;
        best_direction = next_direction;
    };

    // sprintf(log, "[left_from  ]: %2d, %2d  %6d  %6d", cell.r, cell.c, best_cost, best_direction);
    // log_maze(log);

    if (best_cost == MAX_COST) {
        best_direction = BLOCKED;
    }

    // sprintf(log, "[END        ]: %2d, %2d  %6d  %6d", cell.r, cell.c, best_cost, best_direction);
    // log_maze(log);

    return best_direction;
}

void set_mask(const MazeMask mask) {
    MAZE_MASK = mask;
}

void set_walls(WallState front_wall, WallState right_wall, WallState left_wall) {
    CELL north_cell = neighbour_cell(CURRENT_CELL, NORTH);
    CELL south_cell = neighbour_cell(CURRENT_CELL, EAST);
    CELL east_cell = neighbour_cell(CURRENT_CELL, SOUTH);
    CELL west_cell = neighbour_cell(CURRENT_CELL, WEST);

    switch (CURRENT_ABSOLUTE_DIRECTION) {
    // set current cell front walls along with  it's neighbour cell back wall
    case NORTH: {
        WALLS[CURRENT_CELL.r][CURRENT_CELL.c]
            .north = front_wall;
        WALLS[north_cell.r][north_cell.c].south = front_wall;

        WALLS[CURRENT_CELL.r][CURRENT_CELL.c]
            .east = front_wall;
        WALLS[east_cell.r][east_cell.c].west = right_wall;

        WALLS[CURRENT_CELL.r][CURRENT_CELL.c]
            .west = front_wall;
        WALLS[west_cell.r][west_cell.c].east = left_wall;

        break;
    }
    case EAST:
        WALLS[CURRENT_CELL.r][CURRENT_CELL.c]
            .east = front_wall;
        WALLS[east_cell.r][east_cell.c].west = front_wall;

        WALLS[CURRENT_CELL.r][CURRENT_CELL.c]
            .south = front_wall;
        WALLS[south_cell.r][south_cell.c].north = right_wall;

        WALLS[CURRENT_CELL.r][CURRENT_CELL.c]
            .north = front_wall;
        WALLS[north_cell.r][north_cell.c].south = left_wall;

        break;
    case SOUTH:
        WALLS[CURRENT_CELL.r][CURRENT_CELL.c]
            .south = front_wall;
        WALLS[south_cell.r][south_cell.c].north = front_wall;

        WALLS[CURRENT_CELL.r][CURRENT_CELL.c]
            .west = front_wall;
        WALLS[west_cell.r][west_cell.c].east = right_wall;

        WALLS[CURRENT_CELL.r][CURRENT_CELL.c]
            .east = front_wall;
        WALLS[east_cell.r][east_cell.c].west = left_wall;

        break;
    case WEST:
        WALLS[CURRENT_CELL.r][CURRENT_CELL.c]
            .west = front_wall;
        WALLS[west_cell.r][west_cell.c].east = front_wall;

        WALLS[CURRENT_CELL.r][CURRENT_CELL.c]
            .north = front_wall;
        WALLS[north_cell.r][north_cell.c].south = right_wall;

        WALLS[CURRENT_CELL.r][CURRENT_CELL.c]
            .south = front_wall;
        WALLS[south_cell.r][south_cell.c].north = left_wall;

        break;
    default:
        // ignore any other Directions (blocked)
        break;
    }
}

void update_walls(WallState front_wall, WallState right_wall, WallState left_wall) {
    // Do not update walls info while running (not searching)
    // This check may not be required i guess !!
    if (MAZE_MASK == MASK_TREAT_UNSEEN_AS_PRESENT) {
        return;
    }

    if ((WALLS[CURRENT_CELL.r][CURRENT_CELL.c].north & WALL_UNSEEN) != WALL_UNSEEN) {
        return;
    }
    if ((WALLS[CURRENT_CELL.r][CURRENT_CELL.c].east & WALL_UNSEEN) != WALL_UNSEEN) {
        return;
    }
    if ((WALLS[CURRENT_CELL.r][CURRENT_CELL.c].west & WALL_UNSEEN) != WALL_UNSEEN) {
        return;
    }
    if ((WALLS[CURRENT_CELL.r][CURRENT_CELL.c].south & WALL_UNSEEN) != WALL_UNSEEN) {
        return;
    }
    // set walls only if it is not seen earlier
    set_walls(front_wall, right_wall, left_wall);
}

void print_maze(uint8_t cost[MAZE_SIZE][MAZE_SIZE]) {
    // Print the top border of the maze
    for (int col = MAZE_SIZE - 1; col >= 0; col--) {
        // Print top border of each row
        for (int i = 0; i < MAZE_SIZE; i++) {
            printf("+-------");
        }
        printf("+\n");

        // Print maze cost values
        for (int row = 0; row < MAZE_SIZE; row++) {
            // int linear_index = col * MAZE_SIZE + row;
            printf("| %5d ", cost[col][row]);
        }
        printf("|\n");
    }

    // Print the bottom border of the last row
    for (int i = 0; i < MAZE_SIZE; i++) {
        printf("+-------");
    }
    printf("+\n");
}

void init_maze() {
    init_walls();
    set_mask(MASK_TREAT_UNSEEN_AS_ABSENT);
}
