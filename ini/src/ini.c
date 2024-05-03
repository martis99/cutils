#include "ini.h"

#include "log.h"

typedef struct sec_data_s {
	str_t name;
	ini_pair_t pairs;
} sec_data_t;

typedef struct pair_data_s {
	str_t key;
	ini_val_t vals;
} pair_data_t;

ini_t *ini_init(ini_t *ini, uint secs_cap, uint pairs_cap, uint vals_cap)
{
	if (ini == NULL) {
		return NULL;
	}

	if (arr_init(&ini->secs, secs_cap, sizeof(sec_data_t)) == NULL) {
		return NULL;
	}

	if (list_init(&ini->pairs, pairs_cap, sizeof(pair_data_t)) == NULL) {
		return NULL;
	}

	if (list_init(&ini->vals, vals_cap, sizeof(str_t)) == NULL) {
		return NULL;
	}

	return ini;
}

void ini_free(ini_t *ini)
{
	if (ini == NULL) {
		return;
	}

	str_t *val;
	list_foreach_all(&ini->vals, val)
	{
		str_free(val);
	}
	list_free(&ini->vals);

	pair_data_t *pair;
	list_foreach_all(&ini->pairs, pair)
	{
		str_free(&pair->key);
	}
	list_free(&ini->pairs);

	sec_data_t *sec;
	arr_foreach(&ini->secs, sec)
	{
		str_free(&sec->name);
	}
	arr_free(&ini->secs);
}

ini_sec_t ini_add_sec(ini_t *ini, str_t name)
{
	if (ini == NULL) {
		return INI_SEC_END;
	}

	ini_sec_t sec	 = arr_add(&ini->secs);
	sec_data_t *data = arr_get(&ini->secs, sec);
	if (data == NULL) {
		log_error("cutils", "ini", NULL, "failed to add section");
		return INI_SEC_END;
	}

	*data = (sec_data_t){
		.name  = name,
		.pairs = INI_PAIR_END,
	};

	return sec;
}

ini_pair_t ini_add_pair(ini_t *ini, ini_sec_t sec, str_t key)
{
	if (ini == NULL) {
		return INI_PAIR_END;
	}

	sec_data_t *secd = arr_get(&ini->secs, sec);
	if (secd == NULL) {
		log_error("cutils", "ini", NULL, "failed to get section");
		return INI_PAIR_END;
	}

	ini_pair_t pair;
	list_add_next_node(&ini->pairs, secd->pairs, pair);

	pair_data_t *data = list_get_data(&ini->pairs, pair);
	if (data == NULL) {
		log_error("cutils", "ini", NULL, "failed to add value");
		return INI_PAIR_END;
	}

	*data = (pair_data_t){
		.key  = key,
		.vals = INI_VAL_END,
	};

	return pair;
}

ini_val_t ini_add_val(ini_t *ini, ini_pair_t pair, str_t val)
{
	if (ini == NULL) {
		return INI_VAL_END;
	}

	pair_data_t *paird = list_get_data(&ini->pairs, pair);
	if (paird == NULL) {
		log_error("cutils", "ini", NULL, "failed to get pair");
		return INI_VAL_END;
	}

	ini_val_t vali;
	list_add_next_node(&ini->vals, paird->vals, vali);

	str_t *data = list_get_data(&ini->vals, vali);
	if (data == NULL) {
		log_error("cutils", "ini", NULL, "failed to get value");
		return INI_VAL_END;
	}

	*data = val;

	return pair;
}

int ini_print(const ini_t *ini, print_dst_t dst)
{
	if (ini == NULL) {
		return 0;
	}

	int off	  = dst.off;
	int first = 1;
	sec_data_t *sec;
	arr_foreach(&ini->secs, sec)
	{
		if (!first) {
			dst.off += dprintf(dst, "\n");
		}

		if (sec->name.data != NULL) {
			dst.off += dprintf(dst, "[%.*s]\n", sec->name.len, sec->name.data);
		}

		pair_data_t *pair;
		list_foreach(&ini->pairs, sec->pairs, pair)
		{
			if (pair->key.data) {
				dst.off += dprintf(dst, "%.*s =", pair->key.len, pair->key.data);
			}
			str_t *val;
			int f = 1;
			list_foreach(&ini->vals, pair->vals, val)
			{
				dst.off += dprintf(dst, f ? (pair->key.data ? " %.*s" : "%.*s") : ", %.*s", val->len, val->data);
				f = 0;
			}
			dst.off += dprintf(dst, "\n");
		}
		first = 0;
	}

	return dst.off - off;
}
