#include "logger.h"
#include "time.h"

int main(void)
{
	/* Lets print a regular log message. */
	INFO("Hello World");

	/* Debugging information is hidden by default, set LOG_LEVEL=4 to enable
	 * this output. */
	DEBUG("Now you see me, now you...");

	/* Sometimes things go wrong. */
	WARNING("Looks like the primary buffer panel just flew off.");
	ERROR("You have no respect for logic.");

	/* These macros can be used just like printf(3). */
	INFO("First equation 2 + 3 = %d", 5);

	/* You could also define your own macros, attaching a tag to each message. */
	#define INFO_DATABASE(f_, ...) INFO(("[ Database ] " f_), ##__VA_ARGS__)
	INFO_DATABASE("Found %d entries, now inserting `Robert'); DROP TABLE users; --`", 507);

	/* For library functions manipulating errno. */
	if (time(NULL) == (time_t) -1) {
		FATALno("time");
	}

	/* And there are times, where recovery is not an option. */
	FATAL("Abandon the Sinking Ship!");
}
