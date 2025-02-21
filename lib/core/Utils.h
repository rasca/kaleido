#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <config.h>

#include <algorithm>
// #include <iterator>
// #include <functional>


int clamp(float x, float lower, float upper)
{
    return min(upper, max(x, lower));
}

int clamp(int x, int upper, int lower)
{
    return min(upper, max(x, lower));
}

// int wrap(int pre) {
//   if (pre < 0) {
//     pre = pre + NUM_LEDS;
//   } else if (pre > NUM_LEDS) {
//     pre = pre - NUM_LEDS;
//   }
//   return pre;
// }

int mapi(int x, int in_min, int in_max, int out_min, int out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

int map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#endif