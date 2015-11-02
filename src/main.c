#include <pebble.h>

static Window *s_main_window;

static TextLayer *s_date_layer;
static TextLayer *s_time_layer;
static Layer *s_date_time_layer;

static GBitmap *s_image_bitmap = NULL;
static BitmapLayer *s_image_bitmap_layer;

static void update_datetime() {
  time_t ltime = time(NULL); 
  struct tm *tick_time = localtime(&ltime);

  // Create long-lived date and time buffers
  static char date_buffer[] = "Sun, Oct 25";
  static char time_buffer[] = "00:00";

  //Write the current date into the date buffer
  strftime(date_buffer, sizeof("Sun, Oct 25"), "%a, %b %e", tick_time);

  // Write the current hours and minutes into the time buffer
  if(clock_is_24h_style() == true) {
    // Use 24 hour format
    strftime(time_buffer, sizeof("00:00"), "%H:%M", tick_time);
  } else {
    // Use 12 hour format
    strftime(time_buffer, sizeof("00:00"), "%I:%M", tick_time);
  }

  // Display buffers on the TextLayers
  text_layer_set_text(s_date_layer, date_buffer);
  text_layer_set_text(s_time_layer, time_buffer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_datetime();
}

static void draw_circle_layer_update_proc(Layer *this_layer, GContext *ctx) {
  GPoint center = GPoint(72, 88);
  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_circle(ctx, center, 64);
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  s_date_time_layer = layer_create(GRect(0, 0, 144, 168));
  layer_set_update_proc(s_date_time_layer, draw_circle_layer_update_proc);
  layer_add_child(window_layer, s_date_time_layer);

  // Date - begin
  s_date_layer = text_layer_create(GRect(0, 40, 144, 24));
  text_layer_set_background_color(s_date_layer, GColorClear);
  text_layer_set_text_color(s_date_layer, GColorBlack);
  text_layer_set_text(s_date_layer, "00:00");
  text_layer_set_font(s_date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_date_layer, GTextAlignmentCenter);
  layer_add_child(s_date_time_layer, text_layer_get_layer(s_date_layer));
  // Date - end

  // Time - begin
  s_time_layer = text_layer_create(GRect(0, 64, 144, 42));
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorBlack);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
  layer_add_child(s_date_time_layer, text_layer_get_layer(s_time_layer));
  // Time - end

  update_datetime();

  // Image bitmap layer - begin
  s_image_bitmap_layer = bitmap_layer_create(GRect(48, 123, 53, 45));
  bitmap_layer_set_compositing_mode(s_image_bitmap_layer, GCompOpSet);
  s_image_bitmap = gbitmap_create_with_resource(RESOURCE_ID_ABE);
  bitmap_layer_set_bitmap(s_image_bitmap_layer,s_image_bitmap);
  layer_add_child(s_date_time_layer, bitmap_layer_get_layer(s_image_bitmap_layer));
  // Image bitmap layer - end 
}

static void main_window_unload(Window *window) {
  layer_destroy(s_date_time_layer);
  text_layer_destroy(s_date_layer);
  text_layer_destroy(s_time_layer);
  bitmap_layer_destroy(s_image_bitmap_layer);
  gbitmap_destroy(s_image_bitmap);
}

static void init() {
  s_main_window = window_create();
  window_set_background_color(s_main_window, GColorFromHEX(0xFFFF00));

  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload,
  });
  window_stack_push(s_main_window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main() {
  init();
  app_event_loop();
  deinit();
}