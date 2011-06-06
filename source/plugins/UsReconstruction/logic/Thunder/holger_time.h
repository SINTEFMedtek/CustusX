#ifndef _HOLGER_TIME
#define _HOLGER_TIME

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#include <ctime>
#endif

#define MAX_TIME_STREAMS 64
#define MAX_TIMINGS_PER_STREAM 512

#define timings_lookup(idx, n) timings[(idx)*MAX_TIMINGS_PER_STREAM + n] 
#define timing_str_lookup(idx, n) timing_str[(idx)*MAX_TIMINGS_PER_STREAM + n] 

// Three alternatives for current time in seconds:
#define TIME_METHOD (clock()/(float)CLOCKS_PER_SEC)
#define TIME_METHOD2 (GetTimeMs64()/1000.0f)
#ifdef WIN32
#define TIME_METHOD3 (timeGetTime()/1000.0f)
#endif

void holger_time_start(int idx, const char * str);
void holger_time(int idx, const char * str);
void holger_time_print(int idx);

/* Example use of Holger Time:

int main() {
	holger_time_start(0, "My timings");
	foo();
	holger_time(0, "foo computation time");
	a = 2+3;
	holger_time(0, "complex computations");
	holger_time_print(0);
}

void foo() {
	holger_time_start(1, "Internal timings of foo");
	for () {
		b = 2+5;
		holger_time(1, "foo step 1");
		c = b*b;
		holger_time(1, "foo step 2");
	}
	holger_time_print(1);
}

*/

#endif