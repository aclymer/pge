#ifndef MAZE_H
#define MAZE_H

#include "main.h"

#define MAZE_TILE_SIZE 10
#define MAZE_GRID_W 9
#define MAZE_GRID_H 11
#define MAZE_DRAW_BIAS 75

void maze_init();

void maze_deinit();

void maze_generate();

void maze_bisect();

void maze_draw(GContext *ctx);

bool** maze_get_tiles();

#endif