#ifndef CUTILS_H
#define CUTILS_H

#include "cplatform.h"
#include "mem.h"

typedef struct cutils_s {
	cplatform_t cplatform;
	mem_t mem;
} cutils_t;

cutils_t *c_init(cutils_t *cutils);
int c_free(cutils_t *cutils, FILE *file);

#endif
