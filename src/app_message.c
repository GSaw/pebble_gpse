#include <pebble.h>
#include "main_window.h"
#include "data.h"

  
// Key values for AppMessage Dictionary
enum {
    DATE_KEY = 0,
	DIST_KEY,	
	ALT_KEY,
    COORDS_KEY,
    SUNRISE_KEY,
    SUNSET_KEY,
    LAST_UPDATE_KEY,
    PHONE_BATTERY_KEY,
    SUNRISE_NEXT_DAY_KEY,
    SUNSET_DAY_BEFORE_KEY
};

// Write message to buffer & send
void send_message(void){
	DictionaryIterator *iter;
	
	app_message_outbox_begin(&iter);
	dict_write_uint8(iter, 0, 1);
	
	dict_write_end(iter);
    app_message_outbox_send();
}

static char* read_string(DictionaryIterator *received, int field, char* target) {
    Tuple *tuple = dict_find(received, field);
    if(tuple) {
        strcpy(target, tuple->value->cstring);
    }
    return NULL;
}

// Called when a message is received from PebbleKitJS
static void in_received_handler(DictionaryIterator *received, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Received data" );

    data.update = true;

    Tuple *tuple;

    tuple = dict_find(received, LAST_UPDATE_KEY);
    if(tuple) {
        data.last_track_update = (int)tuple->value->uint32; 
    }
    tuple = dict_find(received, PHONE_BATTERY_KEY);
    if(tuple) {
        data.phone_battery_level = (int)tuple->value->uint32; 
    }

    time_t current_time = time(NULL);
    int mn = current_time - (current_time % (24*60*60));

    tuple = dict_find(received, SUNRISE_KEY);
    if(tuple) {
        data.sunrise = (time_t)(tuple->value->uint32 + mn); 
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "sunrise = %d", (int)data.sunrise );
    }

    tuple = dict_find(received, SUNSET_KEY);
    if(tuple) {
        data.sunset = (time_t)(tuple->value->uint32 + mn); 
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "sunset = %d", (int)data.sunset );
    }

    mn += 24 * 60 * 60;
    tuple = dict_find(received, SUNRISE_NEXT_DAY_KEY);
    if(tuple) {
        data.sunrise_next_day = (int)tuple->value->uint32 + mn; 
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "sunrise_next_day = %d", (int)data.sunrise_next_day );
    }

    mn -= 2 * 24 * 60 * 60;
    tuple = dict_find(received, SUNSET_DAY_BEFORE_KEY);
    if(tuple) {
        data.sunset_day_before = (int)tuple->value->uint32 + mn; 
        //APP_LOG(APP_LOG_LEVEL_DEBUG, "sunset_day_before = %d", (int)data.sunset_day_before );
    }

    read_string(received, DIST_KEY, data.dist);
    read_string(received, ALT_KEY, data.alt);
    read_string(received, COORDS_KEY, data.coords);

    data.last_data_update = time(NULL);

    data.update = false;
    if(data.queried) {
        update_data();
    }
    data.queried = false;
}

// Called when an incoming message from PebbleKitJS is dropped
static void in_dropped_handler(AppMessageResult reason, void *context) {	
}

// Called when PebbleKitJS does not acknowledge receipt of a message
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "SELECT" ); 
    send_message();
    data.queried = true;
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "UP" );
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "DOWN" );
}

static void click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
    window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

void init(void) {

    init_data();
	
	// Register AppMessage handlers
	app_message_register_inbox_received(in_received_handler); 
	app_message_register_inbox_dropped(in_dropped_handler); 
	app_message_register_outbox_failed(out_failed_handler);
		
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	
    create_window();
    window_set_click_config_provider(window, click_config_provider);
    send_message();
}

void deinit(void) {
	//app_message_deregister_callbacks();
	destroy_window();
}

int main( void ) {
	init();
	app_event_loop();
	deinit();
}