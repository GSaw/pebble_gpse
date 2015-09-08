#pragma once

extern Window *window;  
  
void create_window();
void destroy_window();

struct gpse_data {
  const char* dateString;
  const char* dist;
  const char* alt;
  const char* coords;
  const char* sunrise;
  const char* sunset;
  int last_update;
  int phone_battery_level;
  int daynight;
  int daynight_len;
  int daynight_rem;
};

void update_data(struct gpse_data);
