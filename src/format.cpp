#include <string>

#include "format.h"

using std::string;
using std::to_string;

string Format::ElapsedTime(long seconds) {
    int sec, min, hour;
    int hour_unit = 60*60;
    int min_unit = 60;
    string elapesd_time;

    //get hours
    int leftover_seconds = seconds%hour_unit;
    hour = (seconds-leftover_seconds)/hour_unit;

    //get minutes (and seconds)
    sec = leftover_seconds%min_unit;
    min = (leftover_seconds-sec)/min_unit;

    //makestring
    elapesd_time = to_string(hour)+":"+to_string(min)+":"+to_string(sec);

    return elapesd_time; }