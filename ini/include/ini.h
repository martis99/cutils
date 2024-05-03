#ifndef INI_H
#define INI_H

#include "arr.h"
#include "list.h"
#include "str.h"

#define INI_SEC_END  ARR_END
#define INI_PAIR_END ARR_END
#define INI_VAL_END  ARR_END

typedef uint ini_sec_t;
typedef lnode_t ini_pair_t;
typedef lnode_t ini_val_t;

typedef struct ini_sec_data_s {
	str_t name;
	ini_pair_t pairs;
} ini_sec_data_t;

typedef struct ini_pair_data_s {
	str_t key;
	ini_val_t vals;
} ini_pair_data_t;

typedef struct ini_s {
	arr_t secs;
	list_t pairs;
	list_t vals;
} ini_t;

ini_t *ini_init(ini_t *ini, uint secs_cap, uint pairs_cap, uint vals_cap);
void ini_free(ini_t *ini);

ini_sec_t ini_add_sec(ini_t *ini, str_t name);
ini_pair_t ini_add_pair(ini_t *ini, ini_sec_t sec, str_t key);
ini_val_t ini_add_val(ini_t *ini, ini_pair_t pair, str_t val);

int ini_print(const ini_t *ini, print_dst_t dst);

#define ini_sec_foreach	 arr_foreach
#define ini_pair_foreach list_foreach
#define ini_val_foreach	 list_foreach

#endif
