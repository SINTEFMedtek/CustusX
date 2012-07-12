// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.


#include "holger_time.h"
#include <string.h>

int * timings_n = (int *) malloc(sizeof(int)*MAX_TIME_STREAMS);
float * timings = (float *) malloc(sizeof(float)*MAX_TIME_STREAMS*MAX_TIMINGS_PER_STREAM);
const char * * timing_str = (const char * *) malloc(sizeof(char *)*MAX_TIME_STREAMS*MAX_TIMINGS_PER_STREAM);

// Returns the amount of milliseconds elapsed since the UNIX epoch. Works on both Windows and Linux. (Copypasted from Internet)
//_int64 GetTimeMs64() {
unsigned long long GetTimeMs64() {
	#ifdef WIN32
		/* Windows */
		FILETIME ft;
		LARGE_INTEGER li;

		/* Get the amount of 100 nano seconds intervals elapsed since January 1, 1601 (UTC) and copy it
		* to a LARGE_INTEGER structure. */
		GetSystemTimeAsFileTime(&ft);
		li.LowPart = ft.dwLowDateTime;
		li.HighPart = ft.dwHighDateTime;

		unsigned _int64 ret = li.QuadPart;
		ret -= 116444736000000000LL; /* Convert from file time to UNIX epoch time. */
		ret /= 10000; /* From 100 nano seconds (10^-7) to 1 millisecond (10^-3) intervals */

		return ret;
	#else
		/* Linux */
		struct timeval tv;

		gettimeofday(&tv, NULL);

		//uint64 ret = tv.tv_usec;
		unsigned long long ret = tv.tv_usec;
		/* Convert from micro seconds (10^-6) to milliseconds (10^-3) */
		ret /= 1000;

		/* Adds the seconds (10^0) after converting them to milliseconds (10^-3) */
		ret += (tv.tv_sec * 1000);

		return ret;
	#endif
}

void holger_time_start(int idx, const char * str) {
	timings_n[idx] = 0;
	holger_time(idx, str);
}

void holger_time(int idx, const char * str) {
	timings_lookup(idx, timings_n[idx]) = TIME_METHOD-timings_lookup(idx, 0);
	timings_lookup(idx, 0) = TIME_METHOD;
	timing_str_lookup(idx, timings_n[idx]) = str;
	timings_n[idx]++;
}

void holger_time_print(int idx) {
	for (int i = 1; i < timings_n[idx]; i++) {
		for (int j = i+1; j < timings_n[idx]; j++) {
			if (0 == strcmp(timing_str_lookup(idx, i), timing_str_lookup(idx, j))) {
				timings_lookup(idx, i) += timings_lookup(idx, j);
				timings_lookup(idx, j) = -1024;
			}
		}
	}

	printf("%s:\n", timing_str_lookup(idx, 0));
	float total_time = 0;
	for (int i = 1; i < timings_n[idx]; i++) {
		if (timings_lookup(idx, i) < -512) continue;
		printf("\t%-45s %f\n", timing_str_lookup(idx, i), timings_lookup(idx, i));
		total_time += timings_lookup(idx, i);
	}
	printf("\t%-45s %f\n\n", "TOTAL TIME", total_time);
}
