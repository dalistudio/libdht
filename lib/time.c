/*
 * Copyright (c) 2020 naturalpolice
 * SPDX-License-Identifier: MIT
 *
 * Licensed under the MIT License (see LICENSE).
 */

#include "time.h"

#ifdef _WIN32
#include <winsock2.h> /* For struct timeval */
#include <stdint.h>

/*
 * 来源：
 * https://stackoverflow.com/questions/10905892/equivalent-of-gettimeday-for-windows
 */
int gettimeofday(struct timeval * tp, struct timezone * tzp)
{
    /*
     * Note: some broken versions only have 8 trailing zero's, the correct
     * epoch has 9 trailing zero's
     * This magic number is the number of 100 nanosecond intervals since
     * January 1, 1601 (UTC) until 00:00:00 January 1, 1970
     */
    static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;

    GetSystemTime( &system_time );
    SystemTimeToFileTime( &system_time, &file_time );
    time =  ((uint64_t)file_time.dwLowDateTime )      ;
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec  = (long) ((time - EPOCH) / 10000000L);
    tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
    return 0;
}
#endif
