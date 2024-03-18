#include "cutils.h"

cutils_t *c_init(cutils_t *cutils)
{
	if (cutils == NULL) {
		return NULL;
	}

	cplatform_init(&cutils->cplatform);
	mem_init(&cutils->mem);

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

	if (mem_check()) {
		return 1;
	}

	cplatform_free(&cutils->cplatform);

	return 0;
}
