#include <string>

#include "format.h"

using namespace std;

string Format::ElapsedTime(long seconds) {
    int hour = seconds / 3600;
    seconds = seconds % 3600;
    int min = seconds / 60;
    int sec = seconds % 60;

    //makestring
    string elapesd_time = SetDoubleDigets(hour)+":"+SetDoubleDigets(min)+":"+SetDoubleDigets(sec);

    return elapesd_time; }

string Format::SetDoubleDigets(int p){
    if (p <=9 ){
        return ("0"+to_string(p));
    }
    return to_string(p);
}