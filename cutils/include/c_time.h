#ifndef C_TIME_H
#define C_TIME_H

#define C_TIME_BUF_SIZE 24

unsigned long long c_time();
const char *c_time_str(char *buf);

int c_sleep(unsigned long milliseconds);

#endif
