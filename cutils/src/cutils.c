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

int c_free(cutils_t *cutils, print_dst_t dst)
{
	if (cutils == NULL) {
		return 1;
	}

	 mem_print(dst);

	if (mem_check()) {
		return 1;
	}

	cplatform_free(&cutils->cplatform);

	return 0;
}
