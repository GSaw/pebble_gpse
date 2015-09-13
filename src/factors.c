#include <pebble.h>
#include "factors.h"

  
static int factors[] = {
10,
12,
14,
18,
22,
27,
33,
40,
49,
60,
73,
90,
110,
134,
164,
200,
245,
299,
365,
447,
545,
666,
814,
994,
1215,
1484,
1812,
2214,
2704,
3302,
4034,
4927,
6018,
7350,
8978,
10966,
13394,
16359,
19981,
24406,
29809,
36409
};

double get_factor(int minutes_max, int minute, int actual_mi) {
  int last_hours = 180; // 3 * 60
  if(actual_mi < minutes_max/2) {
    int start_idx = 30 * (last_hours - actual_mi) / last_hours;
    
    if(minute > last_hours) {
      return 1.0f;
    }
    //expand from the beginning of indicator, but from array starting at the end
    int start_min = last_hours - minute; //start position
    //convert in dec, we have 10*3 hours values
    int arr_idx = start_min * start_idx / last_hours;
    APP_LOG(APP_LOG_LEVEL_DEBUG, "-minutes_max=%d, lh=%d, m=%d, am=%d, sm=%d, ai=%d", minutes_max, last_hours, minute, actual_mi, start_min, arr_idx);
    return factors[arr_idx] / 10.0f;
  } else {
   // int start_idx = 30 * (last_hours - (minutes_max - actual_mi) / last_hours;

    if(minute < minutes_max - last_hours) {
      return 1.0f;
    }

    //expand to the end of indicator, but from array's start
    int start_min = minute; //start position
    //convert in dec, we have 10*3 hours values
    int arr_idx = start_min * 30 / last_hours;
    return factors[arr_idx] / 10.0f;
    
  }
}
  
