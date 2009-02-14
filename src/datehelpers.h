#ifndef DATEHELPERS_H_
#define DATEHELPERS_H_

#include <string>

extern int timediff;

struct tm *makedate(const char *datestring);
void set_clockformat(int dateformat, int timeformat);
std::string gen_time_text(time_t time);

#endif
