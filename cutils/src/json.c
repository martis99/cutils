#include "json.h"

#include "print.h"

json_t *json_init(json_t *json, uint values_cap)
{
	if (json == NULL) {
		return NULL;
	}

	if (list_init(&json->values, values_cap, sizeof(json_mem_t)) == NULL) {
		return NULL;
	}

	json->root = JSON_END;

	return json;
}

void json_free(json_t *json)
{
	if (json == NULL) {
		return;
	}

	json_mem_t *mem;
	list_foreach_all(&json->values, mem)
	{
		str_free(&mem->name);
		switch (mem->value.type) {
		case JSON_VAL_STR: str_free(&mem->value.s); break;
		}
	}

	list_free(&json->values);
	json->root = JSON_END;
}

static json_mem_t *json_get_mem(const json_t *json, json_val_t val)
{
	return list_get_data(&json->values, val);
}

json_val_t json_add_val(json_t *json, json_val_t parent, str_t name, json_val_data_t value)
{
	if (json == NULL || ((json->root == JSON_END) != (parent == JSON_END))) {
		return JSON_END;
	}

	json_val_t val;

	if (parent == JSON_END) {
		list_add_next_node(&json->values, json->root, val);
	} else {
		json_mem_t *mem = json_get_mem(json, parent);
		if (mem == NULL) {
			return JSON_END;
		}

		switch (mem->value.type) {
		case JSON_VAL_OBJ: list_add_next_node(&json->values, json_get_mem(json, parent)->value.o.values, val); break;
		case JSON_VAL_ARR: list_add_next_node(&json->values, json_get_mem(json, parent)->value.a.values, val); break;
		default: return JSON_END;
		}
	}

	json_mem_t *mem = list_get_data(&json->values, val);
	if (mem == NULL) {
		return JSON_END;
	}

	*mem = (json_mem_t){
		.name  = name,
		.value = value,
	};

	return val;
}

static int json_mem_print(const json_t *json, json_mem_t *mem, int name, int depth, FILE *file)
{
	if (mem == NULL) {
		return 1;
	}

	int ret = 0;

	for (int i = 0; i < depth; i++) {
		ret |= !c_fprintf(file, "\t");
	}

	if (name) {
		ret |= !c_fprintf(file, "\"%.*s\": ", mem->name.len, mem->name.data);
	}

	json_mem_t *child;

	switch (mem->value.type) {
	case JSON_VAL_INT: ret |= !c_fprintf(file, "%d", mem->value.i); break;
	case JSON_VAL_FLOAT: ret |= !c_fprintf(file, "%.7f", mem->value.f); break;
	case JSON_VAL_DOUBLE: ret |= !c_fprintf(file, "%.15f", mem->value.d); break;
	case JSON_VAL_BOOL: ret |= !c_fprintf(file, "%s", mem->value.b ? "true" : "false"); break;
	case JSON_VAL_STR: ret |= !c_fprintf(file, "\"%.*s\"", mem->value.s.len, mem->value.s.data); break;
	case JSON_VAL_OBJ:
		if (mem->value.o.values == LIST_END) {
			ret |= !c_fprintf(file, "{}");
			break;
		}
		ret |= !c_fprintf(file, "{\n");
		list_foreach(&json->values, mem->value.o.values, child)
		{
			ret |= json_mem_print(json, child, 1, depth + 1, file);
			if (list_get_next(&json->values, _i) != LIST_END) {
				ret |= !c_fprintf(file, ",");
			}
			ret |= !c_fprintf(file, "\n");
		}
		for (int i = 0; i < depth; i++) {
			ret |= !c_fprintf(file, "\t");
		}

		ret |= !c_fprintf(file, "}");
		break;
	case JSON_VAL_ARR:
		if (mem->value.a.values == LIST_END) {
			ret |= !c_fprintf(file, "[]");
			break;
		}
		ret |= !c_fprintf(file, "[\n");

		list_foreach(&json->values, mem->value.a.values, child)
		{
			ret |= json_mem_print(json, child, 0, depth + 1, file);
			if (list_get_next(&json->values, _i) != LIST_END) {
				ret |= !c_fprintf(file, ",");
			}
			ret |= !c_fprintf(file, "\n");
		}
		for (int i = 0; i < depth; i++) {
			ret |= !c_fprintf(file, "\t");
		}

		ret |= !c_fprintf(file, "]");
		break;
	}

	return ret;
}

int json_print(const json_t *json, json_val_t val, FILE *file)
{
	return json_mem_print(json, list_get_data(&json->values, val), 0, 0, file);
}
