#ifndef INI_PARSE_H
#define INI_PARSE_H

#include "esyntax.h"
#include "ini.h"

typedef struct ini_prs_s {
	estx_t estx;
	estx_rule_t file;
	estx_rule_t ini;
	estx_rule_t sec;
	estx_rule_t pair;
	estx_rule_t key;
	estx_rule_t vals;
	estx_rule_t valc;
	estx_rule_t val;
} ini_prs_t;

ini_prs_t *ini_prs_init(ini_prs_t *ini_prs);
void ini_prs_free(ini_prs_t *ini_prs);

ini_t *ini_prs_parse(const ini_prs_t *ini_prs, str_t str, ini_t *ini);

#endif
