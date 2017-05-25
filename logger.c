#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static int log_level = -1;
static int log_location = -1;
static int should_colorize = -1;
static FILE* log_file;

static void close_log_file(void)
{
	fclose(log_file);
}

void logger_log(int level, const char* color, bool terminate, const char* file,
                long line, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	// determine log level
	if (log_level == -1) {
		const char* l = getenv("LOG_LEVEL");
		log_level = l ? atoi(l) : LOG_LEVEL_DEFAULT;
	}

	if (log_level < level) {
		goto done;
	}

	// determine log file
	if (log_file == NULL) {
		const char* f = getenv("LOG_FILE");
		if (f && strcmp(f, "-") != 0) {
			log_file = fopen(f, "a");
			if (!log_file) {
				perror("Logger");
				exit(1);
			}
			atexit(close_log_file);
		} else if (f) {
			log_file = stdout;
		} else {
			log_file = stderr;
		}
	}

	// determine colorization
	if (should_colorize == -1) {
		const char* t = getenv("TERM");
		should_colorize =  getenv("DISABLE_LOG_COLOR") == NULL
		                && (log_file == stderr || log_file == stdout)
		                && (t && strcmp(t, "dumb"));
	}

	if (should_colorize) {
		fputs(color, log_file);
	}

	// time
	{
		time_t raw_now;
		if (time(&raw_now) == -1) {
			perror("time");
			exit(1);
		}

		struct tm *now = localtime(&raw_now);
		if (!now) {
			perror("localtime");
			exit(1);
		}

		char now_sz[20];
		now_sz[0] = '\0';
		strftime(now_sz, sizeof(now_sz), "%F %T", now);

		fputs(now_sz, log_file);
	}

	switch(level) {
		case LOG_LEVEL_DEBUG:    fputs(" DEBUG   ", log_file); break;
		case LOG_LEVEL_WARNING:  fputs(" WARNING ", log_file); break;
		case LOG_LEVEL_ERROR:    fputs(" ERROR   ", log_file); break;
		default:                 fputs(" INFO    ", log_file);
	}

	/* determine whether source locations should be printed */
	if (log_location == -1) {
		log_location = getenv("LOG_LOCATION") != NULL;
	}

	if (log_location) {
		fprintf(log_file, "%s:%ld | ", file, line);
	}

	vfprintf(log_file, fmt, args);

	if (should_colorize) {
		fputs(LOG_COLOR_NRM, log_file);
	}

	fputs("\n", log_file);

	if (log_file != stderr && (level == LOG_LEVEL_WARNING || level == LOG_LEVEL_ERROR)) {
		fflush(log_file);
	}

done:
	va_end(args);

	if (terminate) {
		exit(1);
	}
}
