#pragma once

extern Window *window;  
  
void create_window();
void destroy_window();
void update_data(const char* dateString, const char* dist, const char* alt, const char* coords, const char* sunrise, const char* sunset, int last_update, int phone_battery_level, int daynight, int daynight_len, int daynight_rem);
