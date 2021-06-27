#ifndef __LOG_HEADER__
#define __LOG_HEADER__
#include <types.h>

/* kernel/log.c */

extern bool trace;

/*
 * the different configurations we can use for the logging output
 */
enum logging_output {
	LOG_SERIAL,
	LOG_CONSOLE,
	LOG_BOTH,
};

/*
 * ask for the logging module to load, use this before any other method call
 * returns: 1 on success, 0 on failure
 */
extern u8 require_log(enum logging_output output);

/*
 * log using a format string
 */
extern void logf(const char *fmt, ...);

#define tracef(fmt, ...) do { \
    if (trace)                          \
		logf("%s :: " fmt, __func__, __VA_ARGS__); \
	} while (0) ;
#endif