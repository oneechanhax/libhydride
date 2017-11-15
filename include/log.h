/*
 * log.h
 *
 *  Created on: Nov 14, 2017
 *      Author: nullifiedcat
 */

#pragma once

#define DEBUG 1

#if DEBUG

void
log_write(const char *format, ...);

#else

#define log_write(...)

#endif
