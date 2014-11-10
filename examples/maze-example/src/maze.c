#include "maze.h"

static bool **tiles;
static GSize grid_size;

/******************************* Private **************************************/

static bool is_even(int query) {
  return (query % 2) == 0;
}

static bool test_location(int x, int y) {
  //Check 2 x 2
  for(int j = y; j < y + 2; j++) {
    for(int i = x; i < x + 2; i++) {
      // Cancel if there's a wall
      if(tiles[i][j]) {
        return false;
      }
    }
  }

  return true;
}

static bool room_available() {
  // For all starting locations
  for(int y = 0; y < grid_size.h; y++) {
    for(int x = 0; x < grid_size.w; x++) {
      // If there is a room, end search
      if(test_location(x, y)) {
        return true;
      }
    }
  }

  // No 2x2 left
  return false;
}

static GRect find_room() {
  GRect result = GRect(0, 0, 0, 0);

  // For all starting locations
  for(int y = 0; y < grid_size.h; y++) {
    for(int x = 0; x < grid_size.w; x++) {
      // If there is a room, find its extent
      if(test_location(x, y)) {
        result.origin.x = x;
        result.origin.y = y;

        // Get x extent
        int offset = 0;
        while(!tiles[x + offset][y] && ((x + offset) < grid_size.w)) {
          offset++;
        }
        result.size.w = offset; // 0, 1, 2 = 3

        //Get y extent
        offset = 0;
        while(!tiles[x][y + offset] && ((y + offset) < grid_size.h)) {
          offset++;
        }
        result.size.h = offset;

        // APP_LOG(APP_LOG_LEVEL_DEBUG, "GRect(%d, %d, %d, %d)",
          // result.origin.x, result.origin.y, result.size.w, result.size.h);

        //Done!
        return result;
      }
    }
  }

  //If !room_available(), should never happen
  return result;
}

/******************************** Public **************************************/

void maze_init() {
  srand(time(NULL));

  pge_grid_set_tile_size(MAZE_TILE_SIZE);
  grid_size = pge_grid_get_grid_dimensions();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "grid_size:%dx%d", grid_size.w, grid_size.h);

  int16_t before = heap_bytes_free();
  
  tiles = (bool **)malloc(grid_size.h * sizeof(bool*));
  for(int i = 0; i < grid_size.h; i++) {
    tiles[i] = (bool*)malloc(grid_size.w * sizeof(bool));
  }

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Maze allocation took %d bytes.", (before - heap_bytes_free()));
}

void maze_deinit() {
  for(int i = 0; i < grid_size.h; i++) {
    free(tiles[i]);
  }
  free(tiles);
}

void maze_generate() {
  //Clear
  for(int y = 0; y < grid_size.h; y++) {
    for(int x = 0; x < grid_size.w; x++) {
      tiles[x][y] = false;
    }
  }

  //Frame to prevent wandering
  int x = 0, y = 0;
  for(x = 0; x < grid_size.w; x++) {
    tiles[x][y] = true;
  }
  x = grid_size.w - 1;
  for(y = 0; y < grid_size.h; y++) {
    tiles[x][y] = true;
  }
  y = grid_size.h - 1;
  for(x = 0; x < grid_size.w; x++) {
    tiles[x][y] = true;
  }
  x = 0;
  for(y = 0; y < grid_size.h; y++) {
    tiles[x][y] = true;
  }
}

void maze_bisect() {
  if(room_available()) {
    GRect room = find_room();

    bool vertical;
    if(room.size.w > room.size.h) {
      vertical = true;
    } else if(room.size.h > room.size.w) {
      vertical = false;
    } else {
      vertical = (rand() % 100 > 51);
    }

    //Bisect
    if(vertical) {
      // Get random x
      int x = room.origin.x + rand() % room.size.w;

      /**
       * MAGIC BULLET
       * Walls only on even, doors only on odd
       * http://stackoverflow.com/a/23530960
       */
      while(!is_even(x)) {
        x = room.origin.x + rand() % room.size.w;
      }

      // Make vertical wall
      for(int y = room.origin.y; y < room.origin.y + room.size.h; y++) {
        tiles[x][y] = true;
      }

      // Make hole
      int y = room.origin.y + rand() % room.size.h;
      while(is_even(y)) {
        y = room.origin.y + rand() % room.size.h;
      }

      tiles[x][y] = false;
    } else {
      // Get random y
      int y = room.origin.y + rand() % room.size.h;
      while(!is_even(y)) {
        y = room.origin.y + rand() % room.size.h;
      }

      // Make horizontal wall
      for(int x = room.origin.x; x < room.origin.x + room.size.w; x++) {
        tiles[x][y] = true;
      }

      // Make hole
      int x = room.origin.x + rand() % room.size.w;
      while(is_even(x)) {
        x = room.origin.x + rand() % room.size.w;
      }

      tiles[x][y] = false;
    }
  } else {
    // Entrance always at 1, 0
    tiles[0][1] = false;
  }
}

void maze_draw(GContext *ctx) {
  for(int y = 0; y < grid_size.h; y++) {
    for(int x = 0; x < grid_size.w; x++) {
      if(tiles[x][y]) {
        graphics_context_set_fill_color(ctx, GColorWhite);
      } else {
        graphics_context_set_fill_color(ctx, GColorBlack);
      }
      graphics_fill_rect(ctx, GRect(x * MAZE_TILE_SIZE, y * MAZE_TILE_SIZE, MAZE_TILE_SIZE, MAZE_TILE_SIZE), 0, GCornerNone);
    }
  }
}

bool** maze_get_tiles() {
  return tiles;
}