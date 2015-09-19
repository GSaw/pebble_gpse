#pragma once
  
struct gpse_data {
  bool update;
  bool queried;
  time_t last_data_update;
  time_t last_data_shown;

  char dist[8];
  char alt[6];
  char coords[21];
  time_t sunrise;
  time_t sunset;
  time_t sunrise_next_day;
  time_t sunset_day_before;
  int last_track_update; //seconds?
  int phone_battery_level;
};

extern struct gpse_data data;

void init_data();