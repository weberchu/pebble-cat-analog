#pragma once

#include "pebble_os.h"

static const GPathInfo CAT_PATH_INFO = {
  23,
  (GPoint []) {
    {10, -73},
    {12, -73},
    {13, -70},
    {14, -70},
    {17, -67}
    {17, -65}
    {14, -62}
    {14, -61}
    {20, -57}
    {20, -56}
    {8, -57}
    {-7, -56}
    {-17, -52}
    {-17, -54}
    {-12, -60}
    {-12, -61}
    {-21, -65}
    {-21, -67}
    {-10, -64}
    {-3, -64}
    {7, -66}
    {9, -70}
    {10, -70}
  }
};

static const GPathInfo MINUTE_HAND_POINTS =
{
  4,
  (GPoint []) {
    {-6, -13},
    {-6, -52},
    {6, -52},
    {6, -13},
  }
};

static const GPathInfo HOUR_HAND_POINTS = {
  4, (GPoint []){
    {-8, -13},
    {-8, -42},
    {8, -42},
    {8, -13},
  }
};