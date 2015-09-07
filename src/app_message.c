#include <pebble.h>
#include "main_window.h"
  
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
  DAYNIGHT,
  DAYNIGHT_LENGTH,
  DAYNIGHT_REMAINING
};

static char* read_string(DictionaryIterator *received, int field) {
  Tuple *tuple = dict_find(received, field);
  if(tuple) {
    return tuple->value->cstring;
  }
  return NULL;
}

// Called when a message is received from PebbleKitJS
static void in_received_handler(DictionaryIterator *received, void *context) {
	Tuple *tuple;
  char* date = NULL;
  char* dist = NULL;
  char* alt = NULL;
  char* coords = NULL;
  char* sunrise = NULL;
  char* sunset = NULL;
  int last_update = -1;
  int phone_battery_level = 0;
  int daynight = 0;
  int daynight_len = 0;
  int daynight_rem = 0;
  
	tuple = dict_find(received, LAST_UPDATE_KEY);
	if(tuple) {
		last_update = (int)tuple->value->uint32; 
	}
	tuple = dict_find(received, PHONE_BATTERY_KEY);
	if(tuple) {
		phone_battery_level = (int)tuple->value->uint32; 
	}

	tuple = dict_find(received, DAYNIGHT);
	if(tuple) {
		daynight = (int)tuple->value->uint32; 
	}

 	tuple = dict_find(received, DAYNIGHT_LENGTH);
	if(tuple) {
		daynight_len = (int)tuple->value->uint32; 
	}

	tuple = dict_find(received, DAYNIGHT_REMAINING);
	if(tuple) {
		daynight_rem = (int)tuple->value->uint32; 
	}

  
  date = read_string(received, DATE_KEY);
  dist = read_string(received, DIST_KEY);
  alt = read_string(received, ALT_KEY);
  coords = read_string(received, COORDS_KEY);
  sunrise = read_string(received, SUNRISE_KEY);
  sunset = read_string(received, SUNSET_KEY);
	
  update_data(date,dist, alt, coords, sunrise, sunset, last_update, phone_battery_level, daynight, daynight_len, daynight_rem);
  
}

// Called when an incoming message from PebbleKitJS is dropped
static void in_dropped_handler(AppMessageResult reason, void *context) {	
}

// Called when PebbleKitJS does not acknowledge receipt of a message
static void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
}

void init(void) {

	
	// Register AppMessage handlers
	app_message_register_inbox_received(in_received_handler); 
	app_message_register_inbox_dropped(in_dropped_handler); 
	app_message_register_outbox_failed(out_failed_handler);
		
	app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
	
  create_window();
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