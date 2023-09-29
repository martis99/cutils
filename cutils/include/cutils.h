#ifndef CUTILS_H
#define CUTILS_H

#include "log.h"
#include "mem.h"

typedef struct cutils_s {
	mem_t mem;
	log_t log;
} cutils_t;

cutils_t *c_init(cutils_t *cutils);
int c_free(cutils_t *cutils, FILE *file);

#endif
