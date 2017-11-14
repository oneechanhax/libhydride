/*
 * log.c
 *
 *  Created on: Nov 14, 2017
 *      Author: nullifiedcat
 */

#include "log.h"

#include <stdarg.h>
#include <stdio.h>

FILE *log_file = 0;

#if DEBUG

void
log_write(const char *format, ...)
{
    if (log_file == 0)
    {
        log_file = fopen("/tmp/xoverlay.log", "w");
        if (log_file == 0)
        {
            perror("log opening");
            log_file = stderr;
        }
    }

    va_list args;
    va_start(args, format);

    vfprintf(log_file, format, args);
    fflush(log_file);

    va_end(args);
}

#endif
