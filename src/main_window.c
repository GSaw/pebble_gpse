#include <pebble.h>
#include "main_window.h"

Window *window = NULL;

static GFont s_time_font;
static GFont s_sensor_font;
static GFont s_icons_font;

static TextLayer *s_time_layer;
static TextLayer *s_date_layer;
static TextLayer *s_dist_layer;
static TextLayer *s_alt_layer;
static TextLayer *s_coords_layer;
static TextLayer *s_sunrise_layer;
static TextLayer *s_sunset_layer;
static TextLayer *s_conn_layer;
static TextLayer *s_watch_batt_layer;
static TextLayer *s_phone_batt_layer;
static TextLayer *s_tracking_layer;

static Layer *s_daynight_layer;

static GColor COLOR_BACKGROUND;
static GColor COLOR_TEXT;


const char* ICON_CONNECTION = "\U0000E812";
const char* ICON_SUN = "\U0000E80E";
const char* ICON_MOON = "\U0000E80F";
const char* ICON_DISTANCE = "\U0000E804";
const char* ICON_ALTITUDE = "\U0000E813";
const char* ICON_COORDS = "\U0000E805";

const char* ICON_BATT_100 = "\U0000E800";
const char* ICON_BATT_75 = "\U0000E801";
const char* ICON_BATT_25 = "\U0000E802";
const char* ICON_BATT_0 = "\U0000E803";

const char* ICON_TRACKING_ON = "\U0000E807";
const char* ICON_TRACKING_OFF = "\U0000E808";

static time_t last_data_update = 0;

static int g_daynight = 0;
static int g_daynight_len = 0;
static int g_daynight_rem = 0;

static void create_text_layer(TextLayer** layer, GRect grect, const char* text, const char* font, GTextAlignment align) {
  *layer = text_layer_create(grect);
  text_layer_set_background_color(*layer, GColorClear);
  text_layer_set_text_color(*layer, COLOR_TEXT);
  text_layer_set_text(*layer, text);

  // Improve the layout to be more like a watchface
  text_layer_set_font(*layer, fonts_get_system_font(font));
  text_layer_set_text_alignment(*layer, align);

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(*layer));
}


static void create_text_layer_centered(TextLayer** layer, GRect grect, const char* text, const char* font) {
    create_text_layer(layer, grect, text, font, GTextAlignmentCenter );
}
static void create_text_layer_left(TextLayer** layer, GRect grect, const char* text, const char* font) {
    create_text_layer(layer, grect, text, font, GTextAlignmentLeft );
}

static void battery_handler(BatteryChargeState charge_state) {
  const char* batt_status = ICON_BATT_0;
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "charged %d%%", charge_state.charge_percent );
  
  if(charge_state.charge_percent > 85) {
      batt_status = ICON_BATT_100;
  } else if(charge_state.charge_percent > 50) {
      batt_status = ICON_BATT_75;
  } else if(charge_state.charge_percent > 15) {
      batt_status = ICON_BATT_25;
  } 
  text_layer_set_text(s_watch_batt_layer, batt_status);
}

static void check_connection() {
  //Connection
  const char* connection = "";
  time_t current_time = time(NULL);
  if(0 < last_data_update && current_time - last_data_update < 61) {
    connection = ICON_CONNECTION;
  } 
  text_layer_set_text(s_conn_layer, connection);
  
}

static void draw_daynight(Layer *this_layer, GContext *ctx) {

  if(g_daynight_len <= g_daynight_rem) {
    return;
  }
  
  GRect bounds = layer_get_bounds(this_layer);
  graphics_draw_rect(ctx, bounds);
  
  GColor c1 = GColorBlack;
  GColor c2 = GColorWhite;
  
  if(g_daynight == 0) {
    c1 = GColorWhite;
    c2 = GColorBlack;
  }

  graphics_context_set_fill_color(ctx, c2);
  graphics_context_set_stroke_color(ctx, c1);
  
  graphics_fill_rect(ctx, GRect(1, 1, bounds.size.w - 1, bounds.size.h - 1), 0, GCornerNone); 
  
  //graphics_context_set_fill_color(ctx, GColorBlack);
  int hours = g_daynight_len / 60;
  int hour_width = bounds.size.w / hours; 
  int fill_mins = (g_daynight_len - g_daynight_rem);
  int fill_width = fill_mins * bounds.size.w / g_daynight_len;
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "mins fill = %d, fill_width = %d, w = %d, daylen = %d", fill_mins, fill_width, bounds.size.w, g_daynight_len );
  
  for(int i = 0; i <= hours; i++) {
    GPoint p0 = GPoint(i * hour_width, 1);
    GPoint p1 = GPoint(i * hour_width, bounds.size.h-1);    
    graphics_draw_line(ctx, p0, p1);
  }

  graphics_context_set_fill_color(ctx, c1);
  graphics_context_set_stroke_color(ctx, c2);

  graphics_fill_rect(ctx, GRect(1, 1, fill_width, bounds.size.h-1), 0, GCornerNone); 
  //graphics_draw_rect(ctx, bounds);
  
  hours = (g_daynight_len - g_daynight_rem) / 60;
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "hours %d", hours );
  for(int i = 0; i <= hours; i++) {
    GPoint p0 = GPoint(i * hour_width, 1);
    GPoint p1 = GPoint(i * hour_width, bounds.size.h-1);    
    graphics_draw_line(ctx, p0, p1);
  }
  
}

void update_data(struct gpse_data gpse_data) {
  //if(NULL != dateString)  text_layer_set_text(s_date_layer, dateString);
  time_t current_time = time(NULL);
  last_data_update = current_time;
  check_connection();
  if(NULL != gpse_data.dist)  text_layer_set_text(s_dist_layer, gpse_data.dist);
  if(NULL != gpse_data.alt)  text_layer_set_text(s_alt_layer, gpse_data.alt);
  if(NULL != gpse_data.coords)  text_layer_set_text(s_coords_layer, gpse_data.coords);
  if(NULL != gpse_data.sunrise)  text_layer_set_text(s_sunrise_layer, gpse_data.sunrise);
  if(NULL != gpse_data.sunset)  text_layer_set_text(s_sunset_layer, gpse_data.sunset);
  

  const char* batt_status = ICON_BATT_0;
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "phone charged %d%%", gpse_data.phone_battery_level );
  
  if(gpse_data.phone_battery_level > 85) {
      batt_status = ICON_BATT_100;
  } else if(gpse_data.phone_battery_level > 50) {
      batt_status = ICON_BATT_75;
  } else if(gpse_data.phone_battery_level > 15) {
      batt_status = ICON_BATT_25;
  } 
  text_layer_set_text(s_phone_batt_layer, batt_status);
  
  if(gpse_data.last_update > 5 * 60) {
      text_layer_set_text(s_tracking_layer, ICON_TRACKING_OFF);
  } else {
      text_layer_set_text(s_tracking_layer, ICON_TRACKING_ON);
  }
  
  g_daynight = gpse_data.daynight;
  g_daynight_len = gpse_data.daynight_len;
  g_daynight_rem = gpse_data.daynight_rem;
  
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "daynight_ley = %d, daynight_rem = %d, daynight = %d", g_daynight_len, g_daynight_rem, g_daynight );
}

static void update_time() {
  // Get a tm structure
  time_t temp = time(NULL); 
  struct tm *tick_time = localtime(&temp);

  // Create a long-lived buffer
  static char buffer[] = "00:00";

  strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);

  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, buffer);

  static char dataStr[] = "12 Okt. 2015, Wed     ";
  strftime(dataStr, sizeof("12 Okt. 2015, Wed     "), "%d:%m:%Y, %a", tick_time);
  text_layer_set_text(s_date_layer, dataStr);
  
  /// Update Indicators
  
  check_connection();
  
  
}

static void tick_handler(struct tm *tick_time_event, TimeUnits units_changed) {
  update_time();
}

static void tick_handler_seconds(struct tm *tick_time_event, TimeUnits units_changed) {

}


static void main_window_load(Window *window) {
  //time
  create_text_layer_centered(&s_time_layer, GRect(0, 0, 144, 42), "00:00", FONT_KEY_BITHAM_42_BOLD);
  text_layer_set_font(s_time_layer, s_time_font);
  //date
  create_text_layer_centered(&s_date_layer, GRect(0, 43, 144, 24), "", FONT_KEY_GOTHIC_24_BOLD);
  //distance
  TextLayer *iconDist = NULL;
  create_text_layer_left(&iconDist, GRect(4, 73, 16, 18), ICON_DISTANCE, FONT_KEY_GOTHIC_18_BOLD);
  text_layer_set_font(iconDist, s_icons_font);
  create_text_layer_left(&s_dist_layer, GRect(22, 66, 144, 24), "", FONT_KEY_GOTHIC_24_BOLD);
  
  //alt
  TextLayer *iconAlt = NULL;
  create_text_layer_left(&iconAlt, GRect(4, 92, 16, 18), ICON_ALTITUDE, FONT_KEY_GOTHIC_18_BOLD);
  text_layer_set_font(iconAlt, s_icons_font);
  create_text_layer_left(&s_alt_layer, GRect(22, 85, 144, 24), "", FONT_KEY_GOTHIC_24_BOLD);
  
  //coords
  TextLayer *iconCoords = NULL;
  create_text_layer_left(&iconCoords, GRect(4, 110, 16, 18), ICON_COORDS, FONT_KEY_GOTHIC_18_BOLD);
  text_layer_set_font(iconCoords, s_icons_font);
  create_text_layer_left(&s_coords_layer, GRect(22, 110, 144, 18), "", FONT_KEY_GOTHIC_18);
  text_layer_set_font(s_coords_layer, s_sensor_font);
  //sunrise
  TextLayer *iconSunrise = NULL;
  create_text_layer_left(&iconSunrise, GRect(4, 128, 16, 18), ICON_SUN, FONT_KEY_GOTHIC_18_BOLD);
  text_layer_set_font(iconSunrise, s_icons_font);
  create_text_layer_left(&s_sunrise_layer, GRect(22, 128, 144, 18), "", FONT_KEY_GOTHIC_18_BOLD);
  text_layer_set_font(s_sunrise_layer, s_sensor_font);
  //sunset
  TextLayer *iconSunset = NULL;
  create_text_layer_left(&iconSunset, GRect(4, 146, 16, 18), ICON_MOON, FONT_KEY_GOTHIC_18_BOLD);
  text_layer_set_font(iconSunset, s_icons_font);
  create_text_layer_left(&s_sunset_layer, GRect(22, 146, 144, 18), "", FONT_KEY_GOTHIC_18_BOLD);
  text_layer_set_font(s_sunset_layer, s_sensor_font);
  
  //connection
  create_text_layer_left(&s_conn_layer, GRect(124, 7, 16, 18), ICON_CONNECTION, FONT_KEY_GOTHIC_18_BOLD);
  text_layer_set_font(s_conn_layer, s_icons_font);
  
  //watch battery
  create_text_layer_left(&s_watch_batt_layer, GRect(4, 7, 16, 18), ICON_BATT_0, FONT_KEY_GOTHIC_18_BOLD);
  text_layer_set_font(s_watch_batt_layer, s_icons_font);

  //phone battery
  create_text_layer_left(&s_phone_batt_layer, GRect(4, 25, 16, 18), "", FONT_KEY_GOTHIC_18_BOLD);
  text_layer_set_font(s_phone_batt_layer, s_icons_font);

  //tracking status
  create_text_layer_left(&s_tracking_layer, GRect(124, 25, 16, 18), ICON_TRACKING_OFF, FONT_KEY_GOTHIC_18_BOLD);
  text_layer_set_font(s_tracking_layer, s_icons_font);

  //daynight
  s_daynight_layer = layer_create(GRect(20+40, 134, 78, 28));
  layer_set_update_proc(s_daynight_layer, draw_daynight);  
  layer_add_child(window_get_root_layer(window), s_daynight_layer);
  
}

static void main_window_unload(Window *window) {
  // Unload GFont
  fonts_unload_custom_font(s_time_font);
  fonts_unload_custom_font(s_sensor_font);
  fonts_unload_custom_font(s_icons_font);
}


void create_window() {
  //Global Constants
  COLOR_BACKGROUND = GColorWhite;
  COLOR_TEXT = GColorBlack;
  
  s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_TIME_42));
  s_sensor_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SENSORS_16));
  s_icons_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_ICONS_16));

  
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "window create");
  window = window_create();
  window_set_background_color(window,COLOR_BACKGROUND);
    
  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });  
  
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "show window");
	window_stack_push(window, true);
  update_time();
  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT , tick_handler);
  // Register for battery status changes
  battery_state_service_subscribe(battery_handler);
  battery_handler(battery_state_service_peek());
  
}


void destroy_window() {
    // Destroy Window
    window_destroy(window);  
}

