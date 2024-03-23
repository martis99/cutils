#include "json.h"

#include "log.h"
#include "print.h"

json_t *json_init(json_t *json, uint values_cap)
{
	if (json == NULL) {
		return NULL;
	}

	if (list_init(&json->values, values_cap, sizeof(json_mem_t)) == NULL) {
		return NULL;
	}

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
		if (mem->value.type == JSON_VAL_STR) {
			str_free(&mem->value.val.s);
		}
	}

	list_free(&json->values);
}

static json_mem_t *json_get_mem(const json_t *json, json_val_t val)
{
	return list_get_data(&json->values, val);
}

json_val_t json_add_val(json_t *json, json_val_t parent, str_t name, json_val_data_t value)
{
	if (json == NULL) {
		return JSON_END;
	}

	json_val_t val;

	if (parent == JSON_END) {
		list_add_next_node(&json->values, parent, val);
	} else {
		json_mem_t *mem = json_get_mem(json, parent);
		if (mem == NULL) {
			return JSON_END;
		}

		switch (mem->value.type) {
		case JSON_VAL_OBJ: list_add_next_node(&json->values, json_get_mem(json, parent)->value.val.o.values, val); break;
		case JSON_VAL_ARR: list_add_next_node(&json->values, json_get_mem(json, parent)->value.val.a.values, val); break;
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

static int json_mem_print(const json_t *json, json_mem_t *mem, print_dst_t dst, int name, int depth, const char *indent)
{
	if (mem == NULL) {
		return 0;
	}

	int off = dst.off;

	for (int i = 0; i < depth; i++) {
		dst.off += dprintf(dst, "%s", indent);
	}

	if (name) {
		dst.off += dprintf(dst, "\"%.*s\": ", mem->name.len, mem->name.data);
	}

	json_mem_t *child;

	switch (mem->value.type) {
	case JSON_VAL_INT: dst.off += dprintf(dst, "%d", mem->value.val.i); break;
	case JSON_VAL_FLOAT: dst.off += dprintf(dst, "%.7f", mem->value.val.f); break;
	case JSON_VAL_DOUBLE: dst.off += dprintf(dst, "%.15f", mem->value.val.d); break;
	case JSON_VAL_BOOL: dst.off += dprintf(dst, "%s", mem->value.val.b ? "true" : "false"); break;
	case JSON_VAL_STR: dst.off += dprintf(dst, "\"%.*s\"", mem->value.val.s.len, mem->value.val.s.data); break;
	case JSON_VAL_OBJ:
		if (mem->value.val.o.values == LIST_END) {
			dst.off += dprintf(dst, "{}");
			break;
		}
		dst.off += dprintf(dst, "{\n");
		list_foreach(&json->values, mem->value.val.o.values, child)
		{
			dst.off += json_mem_print(json, child, dst, 1, depth + 1, indent);
			if (list_get_next(&json->values, _i) != LIST_END) {
				dst.off += dprintf(dst, ",");
			}
			dst.off += dprintf(dst, "\n");
		}
		for (int i = 0; i < depth; i++) {
			dst.off += dprintf(dst, "%s", indent);
		}

		dst.off += dprintf(dst, "}");
		break;
	case JSON_VAL_ARR:
		if (mem->value.val.a.values == LIST_END) {
			dst.off += dprintf(dst, "[]");
			break;
		}
		dst.off += dprintf(dst, "[\n");

		list_foreach(&json->values, mem->value.val.a.values, child)
		{
			dst.off += json_mem_print(json, child, dst, 0, depth + 1, indent);
			if (list_get_next(&json->values, _i) != LIST_END) {
				dst.off += dprintf(dst, ",");
			}
			dst.off += dprintf(dst, "\n");
		}
		for (int i = 0; i < depth; i++) {
			dst.off += dprintf(dst, "%s", indent);
		}

		dst.off += dprintf(dst, "]");
		break;
	default:
		dst.off = off;
		log_warn("cutils", "json", NULL, "unknown value type: %d", mem->value.type);
		break;
	}

	return dst.off - off;
}

int json_print(const json_t *json, json_val_t val, print_dst_t dst, const char *indent)
{
	return json_mem_print(json, list_get_data(&json->values, val), dst, 0, 0, indent);
}
