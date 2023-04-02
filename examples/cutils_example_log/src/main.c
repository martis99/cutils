#include "log.h"

int main(int argc, char **argv)
{
	log_t log = { 0 };

	log_init(&log);

	log_trace("trace");
	log_debug("debug");
	log_info("info");
	log_warn("warn");
	log_error("error");
	log_fatal("fatal");

	return 0;
}
