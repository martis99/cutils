#include "cutils.h"

cutils_t *c_init(cutils_t *cutils)
{
	if (cutils == NULL) {
		return NULL;
	}

	mem_init(&cutils->mem);
	log_init(&cutils->log);

	log_set_level(LOG_DEBUG);

	return cutils;
}

int c_free(cutils_t *cutils, FILE *file)
{
	if (cutils == NULL) {
		return 1;
	}

	if (file != NULL && mem_print(file) == 0) {
		return 1;
	}

	return 0;
}
