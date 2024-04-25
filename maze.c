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

void init_walls() {
    for (int r = 0; r < MAZE_SIZE; r++) {
        for (int c = 0; c < MAZE_SIZE; c++) {
            WALLS[r][c].north = WALL_UNSEEN;
            WALLS[r][c].east = WALL_UNSEEN;
            WALLS[r][c].south = WALL_UNSEEN;
            WALLS[r][c].west = WALL_UNSEEN;
        }
    }
    for (int i = 0; i < MAZE_SIZE; i++) {
        WALLS[MAZE_SIZE - 1][i].north = WALL_PRESENT;
        WALLS[i][MAZE_SIZE - 1].east = WALL_PRESENT;
        WALLS[0][i].south = WALL_PRESENT;
        WALLS[i][0].west = WALL_PRESENT;
    }

    // Not true for all mazes
    // WALLS[0][0].north = WALL_ABSENT;
    // WALLS[0][0].east = WALL_PRESENT;
    // WALLS[0][0].west = WALL_PRESENT;
    // WALLS[0][0].south = WALL_PRESENT;
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
        int16_t new_x = cell.r + 1;
        if (new_x >= MAZE_SIZE) {
            return cell;
        }
        CELL neighbour_cell = {new_x, cell.c};
        return neighbour_cell;
    }
    case EAST: {
        int16_t new_y = cell.c + 1;
        if (new_y >= MAZE_SIZE) {
            return cell;
        }
        CELL neighbour_cell = {cell.r, new_y};
        return neighbour_cell;
    }
    case SOUTH: {
        int16_t new_x = cell.r - 1;
        if (new_x < 0) {
            return cell;
        }
        CELL neighbour_cell = {new_x, cell.c};
        return neighbour_cell;
    }
    case WEST: {
        int16_t new_y = cell.c - 1;
        if (new_y < 0) {
            return cell;
        }
        CELL neighbour_cell = {cell.r, new_y};
        return neighbour_cell;
    }
    default:
        // Invalid direction, return current cell
        return cell;
    }
}

uint8_t cost_neighbour_cell(const CELL cell, const ABSOLUTE_DIRECTION direction) {
    if (!is_cell_accessible(cell, direction)) {
        return MAX_COST;
    }
    CELL next_cell = neighbour_cell(cell, direction);

    return COST[next_cell.r][next_cell.c];
}

ABSOLUTE_DIRECTION smallest_neighbour_cell(const CELL cell, const ABSOLUTE_DIRECTION start_direction) {
    ABSOLUTE_DIRECTION next_direction = start_direction;
    ABSOLUTE_DIRECTION best_direction = BLOCKED;
    uint8_t best_cost = COST[cell.r][cell.c];
    uint8_t cost;

    cost = cost_neighbour_cell(cell, next_direction);
    if (cost < best_cost) {
        best_cost = cost;
        best_direction = next_direction;
    };

    next_direction = right_from(start_direction);
    cost = cost_neighbour_cell(cell, next_direction);

    if (cost < best_cost) {
        best_cost = cost;
        best_direction = next_direction;
    };

    next_direction = behind_from(start_direction);
    cost = cost_neighbour_cell(cell, next_direction);
    if (cost < best_cost) {
        best_cost = cost;
        best_direction = next_direction;
    };

    next_direction = left_from(start_direction);
    cost = cost_neighbour_cell(cell, next_direction);
    if (cost < best_cost) {
        best_cost = cost;
        best_direction = next_direction;
    };

    if (best_cost == MAX_COST) {
        best_direction = BLOCKED;
    }

    return best_direction;
}

void set_mask(const MazeMask mask) {
    MAZE_MASK = mask;
}

WallState get_wall_state(WallState current_state, WallState new_state) {
    if ((current_state & WALL_UNSEEN) == WALL_UNSEEN) { // Check if the wall is unseen before updating
        return new_state;
    }
    return current_state; // Return current state if not unseen
}
void update_walls(WallState front_wall, WallState right_wall, WallState left_wall) {
    CELL north_cell = neighbour_cell(CURRENT_CELL, NORTH);
    CELL east_cell = neighbour_cell(CURRENT_CELL, EAST);
    CELL south_cell = neighbour_cell(CURRENT_CELL, SOUTH);
    CELL west_cell = neighbour_cell(CURRENT_CELL, WEST);

    switch (CURRENT_ABSOLUTE_DIRECTION) {
    case NORTH: {
        WallState ns = get_wall_state(WALLS[north_cell.r][north_cell.c].south, front_wall);
        WALLS[CURRENT_CELL.r][CURRENT_CELL.c].north = ns;
        WALLS[north_cell.r][north_cell.c].south = ns;

        WallState ew = get_wall_state(WALLS[east_cell.r][east_cell.c].west, right_wall);
        WALLS[CURRENT_CELL.r][CURRENT_CELL.c].east = ew;
        WALLS[east_cell.r][east_cell.c].west = ew;

        WallState we = get_wall_state(WALLS[west_cell.r][west_cell.c].east, left_wall);
        WALLS[CURRENT_CELL.r][CURRENT_CELL.c].west = we;
        WALLS[west_cell.r][west_cell.c].east = we;
        break;
    }
    case EAST: {
        WallState ew = get_wall_state(WALLS[east_cell.r][east_cell.c].west, front_wall);
        WALLS[CURRENT_CELL.r][CURRENT_CELL.c].east = ew;
        WALLS[east_cell.r][east_cell.c].west = ew;

        WallState sn = get_wall_state(WALLS[south_cell.r][south_cell.c].north, right_wall);
        WALLS[CURRENT_CELL.r][CURRENT_CELL.c].south = sn;
        WALLS[south_cell.r][south_cell.c].north = sn;

        WallState ns = get_wall_state(WALLS[north_cell.r][north_cell.c].south, left_wall);
        WALLS[CURRENT_CELL.r][CURRENT_CELL.c].north = ns;
        WALLS[north_cell.r][north_cell.c].south = ns;
        break;
    }
    case SOUTH: {
        WallState sn = get_wall_state(WALLS[south_cell.r][south_cell.c].north, front_wall);
        WALLS[CURRENT_CELL.r][CURRENT_CELL.c].south = sn;
        WALLS[south_cell.r][south_cell.c].north = sn;

        WallState we = get_wall_state(WALLS[west_cell.r][west_cell.c].east, right_wall);
        WALLS[CURRENT_CELL.r][CURRENT_CELL.c].west = we;
        WALLS[west_cell.r][west_cell.c].east = we;

        WallState ew = get_wall_state(WALLS[east_cell.r][east_cell.c].west, left_wall);
        WALLS[CURRENT_CELL.r][CURRENT_CELL.c].east = ew;
        WALLS[east_cell.r][east_cell.c].west = ew;
        break;
    }
    case WEST: {
        WallState we = get_wall_state(WALLS[west_cell.r][west_cell.c].east, front_wall);
        WALLS[CURRENT_CELL.r][CURRENT_CELL.c].west = we;
        WALLS[west_cell.r][west_cell.c].east = we;

        WallState ns = get_wall_state(WALLS[north_cell.r][north_cell.c].south, right_wall);
        WALLS[CURRENT_CELL.r][CURRENT_CELL.c].north = ns;
        WALLS[north_cell.r][north_cell.c].south = ns;

        WallState sn = get_wall_state(WALLS[south_cell.r][south_cell.c].north, left_wall);
        WALLS[CURRENT_CELL.r][CURRENT_CELL.c].south = sn;
        WALLS[south_cell.r][south_cell.c].north = sn;
        break;
    }
    default:
        // Ignore any other directions
        break;
    }
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
