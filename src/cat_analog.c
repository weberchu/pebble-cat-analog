#include "simple_analog.h"

#include "pebble_os.h"
#include "pebble_app.h"
#include "pebble_fonts.h"

#include "resource_ids.auto.h"

#include "string.h"
#include "stdlib.h"

#define MY_UUID {0x24, 0xF8, 0xD9, 0xD9, 0xE8, 0x05, 0x44, 0x30, 0x85, 0xC8, 0x81, 0x15, 0x3C, 0xEB, 0x14, 0xB9}
PBL_APP_INFO(MY_UUID,
             "Cat Analog",
             "Weber Chu",
             1, 0,
             DEFAULT_MENU_ICON,
             APP_INFO_WATCH_FACE);

static struct SimpleAnalogData {
  Layer simple_bg_layer;

  Layer date_layer;
  TextLayer day_label;
  char day_buffer[6];
  TextLayer num_label;
  char num_buffer[4];

  GPath minute_arrow, hour_arrow;
  //GPath tick_paths[NUM_CLOCK_TICKS];
  Layer hands_layer;
  Window window;
  GPath cat_path;
} s_data;

static void bg_update_proc(Layer* me, GContext* ctx) {

  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, me->bounds, 0, GCornerNone);

/*  graphics_context_set_fill_color(ctx, GColorWhite);
  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    gpath_draw_filled(ctx, &s_data.tick_paths[i]);
  }*/
}

static void hands_update_proc(Layer* me, GContext* ctx) {
  const GPoint center = grect_center_point(&me->bounds);
  const int16_t secondHandLength = me->bounds.size.w / 2;

  PblTm t;
  get_time(&t);

  int32_t second_angle = TRIG_MAX_ANGLE * t.tm_sec / 60;

  // minute/hour hand
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_context_set_stroke_color(ctx, GColorBlack);

  gpath_rotate_to(&s_data.minute_arrow, TRIG_MAX_ANGLE * t.tm_min / 60);
  gpath_draw_filled(ctx, &s_data.minute_arrow);
  gpath_draw_outline(ctx, &s_data.minute_arrow);

  gpath_rotate_to(&s_data.hour_arrow, (TRIG_MAX_ANGLE * (((t.tm_hour % 12) * 6) + (t.tm_min / 10))) / (12 * 6));
  gpath_draw_filled(ctx, &s_data.hour_arrow);
  gpath_draw_outline(ctx, &s_data.hour_arrow);
  
  gpath_rotate_to(&s_data.cat_path, TRIG_MAX_ANGLE * t.tm_sec / 60);
  graphics_context_set_stroke_color(ctx, GColorClear);
  graphics_context_set_fill_color(ctx, GColorBlack);
  gpath_draw_filled(ctx, &s_data.cat_path);
  gpath_draw_outline(ctx, &s_data.cat_path);

  // dot in the middle
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, GRect(me->bounds.size.w / 2-1, me->bounds.size.h / 2-1, 3, 3), 0, GCornerNone);
}

static void date_update_proc(Layer* me, GContext* ctx) {

  PblTm t;
  get_time(&t);

  string_format_time(s_data.day_buffer, sizeof(s_data.day_buffer), "%a", &t);
  text_layer_set_text(&s_data.day_label, s_data.day_buffer);

  string_format_time(s_data.num_buffer, sizeof(s_data.num_buffer), "%d", &t);
  text_layer_set_text(&s_data.num_label, s_data.num_buffer);
}

static void handle_init(AppContextRef app_ctx) {
  window_init(&s_data.window, "Simple Analog Watch");

  s_data.day_buffer[0] = '\0';
  s_data.num_buffer[0] = '\0';

  // init hand paths
  gpath_init(&s_data.cat_path, &CAT_PATH_INFO);
  gpath_init(&s_data.minute_arrow, &MINUTE_HAND_POINTS);
  gpath_init(&s_data.hour_arrow, &HOUR_HAND_POINTS);

  const GPoint center = grect_center_point(&s_data.window.layer.bounds);
  gpath_move_to(&s_data.cat_path, center);
  gpath_move_to(&s_data.minute_arrow, center);
  gpath_move_to(&s_data.hour_arrow, center);

  // init clock face paths
/*  for (int i = 0; i < NUM_CLOCK_TICKS; ++i) {
    gpath_init(&s_data.tick_paths[i], &ANALOG_BG_POINTS[i]);
  }*/

  // init layers
  layer_init(&s_data.simple_bg_layer, s_data.window.layer.frame);
  s_data.simple_bg_layer.update_proc = &bg_update_proc;
  layer_add_child(&s_data.window.layer, &s_data.simple_bg_layer);

  // init date layer -> a plain parent layer to create a date update proc
  layer_init(&s_data.date_layer, s_data.window.layer.frame);
  s_data.date_layer.update_proc = &date_update_proc;
  layer_add_child(&s_data.window.layer, &s_data.date_layer);

  // init day
  text_layer_init(&s_data.day_label, GRect(46, 114, 27, 20));
  text_layer_set_text(&s_data.day_label, s_data.day_buffer);
  text_layer_set_background_color(&s_data.day_label, GColorWhite);
  text_layer_set_text_color(&s_data.day_label, GColorBlack);
  GFont norm18 = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  text_layer_set_font(&s_data.day_label, norm18);

  layer_add_child(&s_data.date_layer, &s_data.day_label.layer);

  // init num
  text_layer_init(&s_data.num_label, GRect(73, 114, 18, 20));

  text_layer_set_text(&s_data.num_label, s_data.num_buffer);
  text_layer_set_background_color(&s_data.num_label, GColorWhite);
  text_layer_set_text_color(&s_data.num_label, GColorBlack);
  GFont bold18 = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  text_layer_set_font(&s_data.num_label, bold18);

  layer_add_child(&s_data.date_layer, &s_data.num_label.layer);

  // init hands
  layer_init(&s_data.hands_layer, s_data.simple_bg_layer.frame);
  s_data.hands_layer.update_proc = &hands_update_proc;
  layer_add_child(&s_data.window.layer, &s_data.hands_layer);
	
  // Push the window onto the stack
  const bool animated = true;
  window_stack_push(&s_data.window, animated);
}

static void handle_second_tick(AppContextRef ctx, PebbleTickEvent* t) {
  layer_mark_dirty(&s_data.window.layer);
}

void pbl_main(void* params) {
  PebbleAppHandlers handlers = {
    .init_handler = &handle_init,
    .tick_info = {
      .tick_handler = &handle_second_tick,
      .tick_units = SECOND_UNIT
    }
  };
  app_event_loop(params, &handlers);
}