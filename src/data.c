#include <pebble.h>
#include "data.h"
  
struct gpse_data data;  

void init_data() {
    data.dist[0] = '\0';
    data.alt[0] = '\0';
    data.coords[0] = '\0';
    data.sunrise = 0;
    data.sunset = 0;
    data.sunrise_next_day = 0;
    data.sunset_day_before = 0;
    data.last_track_update = -1;
    data.phone_battery_level = 0;
    data.last_data_update = 0;
    data.last_data_shown = 0;
    data.update = false;
    data.queried = false;
}