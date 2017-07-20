#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

static bool logger_initialized = false;

static FILE* log_file;
static int log_level;
static int log_location;
static int should_colorize;

#ifdef LOG_MULTI_THREADED
#include <pthread.h>
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

static void close_log_file(void)
{
	fclose(log_file);
}

#ifdef LOG_MULTI_THREADED
static void destory_log_mutex(void)
{
	pthread_mutex_destroy(&log_mutex);
}
#endif

static void logger_init(void)
{
	/* determine log_file */
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

	/* determine log_level */
	const char* l = getenv("LOG_LEVEL");
	log_level = l ? atoi(l) : LOG_LEVEL_DEFAULT;

	/* determine log_location */
	log_location = getenv("LOG_LOCATION") != NULL;

	/* determine should_colorize */
	should_colorize = false;
	if (getenv("DISABLE_LOG_COLOR") == NULL) {
		should_colorize = (log_file == stderr && isatty(STDERR_FILENO))
		               || (log_file == stdout && isatty(STDOUT_FILENO));
	}

#ifdef LOG_MULTI_THREADED
	/* setup mutex */
	pthread_mutex_init(&log_mutex, NULL);
	atexit(destory_log_mutex);
#endif

	logger_initialized = true;
}

void logger_log(int level, const char* color, bool terminate, const char* file,
                long line, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	if (!logger_initialized) {
		logger_init();
	}

	if (log_level < level) {
		goto done;
	}

#ifdef LOG_MULTI_THREADED
	if (pthread_mutex_lock(&log_mutex) != 0) {
		perror("pthread_mutex_lock");
		exit(1);
	}

	/* ignore return value of mutex unlock */
	pthread_cleanup_push((void (*)(void*)) pthread_mutex_unlock, &log_mutex);
#endif

	if (should_colorize) {
		fputs(color, log_file);
	}

	/* timestamp */
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

	if (log_location) {
		fprintf(log_file, "%s:%ld | ", file, line);
	}

	/* actual log message */
	vfprintf(log_file, fmt, args);

	if (should_colorize) {
		fputs(LOG_COLOR_NRM, log_file);
	}

	fputs("\n", log_file);

	if (log_file != stderr && (level == LOG_LEVEL_WARNING || level == LOG_LEVEL_ERROR)) {
		fflush(log_file);
	}

#ifdef LOG_MULTI_THREADED
	pthread_cleanup_pop(1);
#endif

done:
	va_end(args);

	if (terminate) {
		exit(1);
	}
}
