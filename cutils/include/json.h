#ifndef JSON_H
#define JSON_H

#include "list.h"
#include "str.h"
#include "type.h"

#include <stdarg.h>

#define JSON_END LIST_END

typedef lnode_t json_val_t;

typedef enum json_val_type_e {
	JSON_VAL_EMPTY,
	JSON_VAL_INT,
	JSON_VAL_FLOAT,
	JSON_VAL_DOUBLE,
	JSON_VAL_BOOL,
	JSON_VAL_STR,
	JSON_VAL_OBJ,
	JSON_VAL_ARR,
} json_val_type_t;

typedef struct json_obj_s {
	json_val_t values;
} json_obj_t;

typedef struct json_arr_s {
	json_val_t values;
} json_arr_t;

typedef struct json_val_data_s {
	json_val_type_t type;
	union {
		int i;
		float f;
		double d;
		unsigned char b;
		str_t s;
		json_obj_t o;
		json_arr_t a;
	};
} json_val_data_t;

typedef struct json_mem_s {
	str_t name;
	json_val_data_t value;
} json_mem_t;

typedef struct json_s {
	list_t values;
	json_val_t root;
} json_t;

json_t *json_init(json_t *json, uint values_cap);
void json_free(json_t *json);

json_val_t json_add_val(json_t *json, json_val_t parent, str_t name, json_val_data_t value);

int json_print(const json_t *json, json_val_t val, FILE *file);

#define JSON_INT(val)                           \
	(json_val_data_t)                       \
	{                                       \
		.type = JSON_VAL_INT, .i = val, \
	}

#define JSON_FLOAT(val)                           \
	(json_val_data_t)                         \
	{                                         \
		.type = JSON_VAL_FLOAT, .f = val, \
	}

#define JSON_DOUBLE(val)                           \
	(json_val_data_t)                          \
	{                                          \
		.type = JSON_VAL_DOUBLE, .d = val, \
	}

#define JSON_BOOL(val)                           \
	(json_val_data_t)                        \
	{                                        \
		.type = JSON_VAL_BOOL, .b = val, \
	}

#define JSON_STR(val)                           \
	(json_val_data_t)                       \
	{                                       \
		.type = JSON_VAL_STR, .s = val, \
	}

#define JSON_OBJ()                                                             \
	(json_val_data_t)                                                      \
	{                                                                      \
		.type = JSON_VAL_OBJ, .o = (json_obj_t){ .values = JSON_END }, \
	}

#define JSON_ARR()                                                             \
	(json_val_data_t)                                                      \
	{                                                                      \
		.type = JSON_VAL_ARR, .a = (json_arr_t){ .values = JSON_END }, \
	}

#endif
