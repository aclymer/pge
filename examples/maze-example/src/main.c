#include "main.h"

static Window *s_window;
static Robot *s_robot;

static int s_direction;

static void logic() {
  maze_bisect();
}

static void draw(GContext *ctx) {
  maze_draw(ctx);
  robot_render(s_robot, ctx);
}

static void click(int button_id) {
  switch(button_id) {
    case BUTTON_ID_UP:
      s_direction--;
      if(s_direction < DIRECTION_UP) {
        s_direction = DIRECTION_LEFT;
      }
      break;

    case BUTTON_ID_SELECT:
    {
      GPoint pos = s_robot->sprite->position;
      bool** tiles = maze_get_tiles();

        // Advance with collision
        switch(s_robot->direction) {
          case DIRECTION_UP:
            if(!tiles[pos.x / 10][(pos.y / 10) - 1])
              pos = pge_grid_move(pos, 0, -1);
            break;
          case DIRECTION_RIGHT:
            if(!tiles[(pos.x / 10) + 1][pos.y / 10])
              pos = pge_grid_move(pos, 1, 0);
            break;
          case DIRECTION_DOWN:
            if(!tiles[pos.x / 10][(pos.y / 10) + 1])
              pos = pge_grid_move(pos, 0, 1);
            break;
          case DIRECTION_LEFT:
            if(!tiles[(pos.x / 10) - 1][pos.y / 10])
              pos = pge_grid_move(pos, -1, 0);
          break;
        }

        pge_sprite_set_position(s_robot->sprite, pos);
      }
      break;

    case BUTTON_ID_DOWN:
      s_direction++;
      if(s_direction > DIRECTION_LEFT) {
        s_direction = DIRECTION_UP;
      }
      break;
  }

  // Update the Robot entity
  robot_set_direction(s_robot, s_direction);
}

static void window_load(Window *window) {
  pge_begin(window, logic, draw, click);
  pge_set_framerate(25);
}

static void window_unload(Window *window) {
  pge_finish();

  window_destroy(window);
}

static void init(void) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "sizeof(bool)=%d", sizeof(bool));

  maze_init();
  maze_generate();

  s_robot = robot_create(GPoint(0, 10));

  s_window = window_create();
  window_set_fullscreen(s_window, true);
  window_set_background_color(s_window, GColorBlack);
  window_set_window_handlers(s_window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(s_window, true);
}

static void deinit(void) {
  maze_deinit();
  robot_destroy(s_robot);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
