#ifndef MAZE_H
#define MAZE_H

#include <stdbool.h>
#include <stdint.h>

// MAZE CONSTANTS
#define MAZE_SIZE 16
#define MAZE_CELL_COUNT (MAZE_SIZE * MAZE_SIZE)
#define MAX_COST (MAZE_CELL_COUNT - 1)

#define START \
    { 0, 0 }
#define END \
    { 15, 15 }

typedef struct {
    uint8_t r;
    uint8_t c;
} CELL;

// Absolute direction represents cordinal compass direction
typedef enum { NORTH,
               EAST,
               SOUTH,
               WEST,
               ABS_DIR_COUNT,
               BLOCKED = 99 } ABSOLUTE_DIRECTION;

// Relative direction represents robot's orientation
typedef enum { AHEAD,
               RIGHT,
               BACK,
               LEFT,
               REL_DIR_COUNT } RELATIVE_DIRECTION;

typedef enum {
    WALL_ABSENT = 0x00,  // a wall that has been seen and confirmed absent
    WALL_PRESENT = 0x01, // a wall that has been seen and confirmed present
    WALL_UNSEEN = 0x02,  // a wall that has not yet been seen
    WALL_VIRTUAL = 0x03, // a wall that does not exist in the physical maze, used for special cases
} WallState;

typedef struct {
    WallState north : 2;
    WallState east : 2;
    WallState south : 2;
    WallState west : 2;
} WallInfo;

typedef enum {
    MASK_TREAT_UNSEEN_AS_ABSENT = 0x01,  // treat unseen walls as absent during search
    MASK_TREAT_UNSEEN_AS_PRESENT = 0x03, // treat unseen walls as present during speed run
} MazeMask;

ABSOLUTE_DIRECTION right_from(const ABSOLUTE_DIRECTION heading);

ABSOLUTE_DIRECTION left_from(const ABSOLUTE_DIRECTION heading);

ABSOLUTE_DIRECTION ahead_from(const ABSOLUTE_DIRECTION heading);

ABSOLUTE_DIRECTION behind_from(const ABSOLUTE_DIRECTION heading);

void init_walls();

bool is_cell_accessible(CELL cell, ABSOLUTE_DIRECTION direction);

CELL neighbour_cell(CELL cell, ABSOLUTE_DIRECTION direction);

uint8_t cost_neighbour_cell(const CELL cell, const ABSOLUTE_DIRECTION direction);

ABSOLUTE_DIRECTION smallest_neighbour_cell(const CELL cell, const ABSOLUTE_DIRECTION start_direction);

void set_mask(const MazeMask mask);

WallState get_wall_state(WallState current_state, WallState new_state);

void update_walls(WallState front_wall, WallState right_wall, WallState left_wall);

void print_maze(uint8_t cost[MAZE_SIZE][MAZE_SIZE]);

void init_maze();

#endif
