#include "xml.h"

#include "mem.h"
#include "print.h"

typedef struct xml_tag_data_s {
	xml_str_t name;
	xml_attr_t attrs;
	xml_str_t val;
} xml_tag_data_t;

typedef struct xml_attr_data_s {
	xml_str_t name;
	xml_str_t val;
} xml_attr_data_t;

static inline xml_tag_data_t *get_tag(const tree_t *tags, xml_tag_t tag)
{
	return tree_get_data(tags, tag);
}

static inline xml_attr_data_t *get_attr(const list_t *attrs, xml_attr_t attr)
{
	return list_get_data(attrs, attr);
}

xml_t *xml_init(xml_t *xml, uint cap)
{
	if (tree_init(&xml->tags, cap, sizeof(xml_tag_data_t)) == NULL) {
		return NULL;
	}

	if (list_init(&xml->attrs, cap, sizeof(xml_attr_data_t)) == NULL) {
		return NULL;
	}

	return xml;
}

static int xml_str_free(xml_str_t *str)
{
	if (str->mem) {
		m_free(str->tdata, str->len);
	}
	return 0;
}

static int xml_tag_free(const tree_t *tree, tnode_t node, void *value, int ret, int depth, int last, void *priv)
{
	xml_tag_data_t *data = value;
	if (data == NULL) {
		return 1;
	}

	xml_str_free(&data->name);
	xml_str_free(&data->val);

	return 0;
}

static int xml_attr_free(const list_t *list, lnode_t node, void *value, int ret, int last, void *priv)
{
	xml_attr_data_t *attr_d = value;

	xml_str_free(&attr_d->name);
	xml_str_free(&attr_d->val);
	return ret;
}

void xml_free(xml_t *xml)
{
	if (xml->tags.cnt > 0) {
		tree_iterate_pre(&xml->tags, 0, xml_tag_free, 0, NULL);
	}
	tree_free(&xml->tags);

	if (xml->attrs.cnt > 0) {
		list_iterate_all(&xml->attrs, xml_attr_free, 0, NULL);
	}
	list_free(&xml->attrs);
}

xml_tag_t xml_add_tag_val_r(xml_t *xml, xml_tag_t tag, const char *name, size_t name_len, const char *val, size_t val_len, bool val_mem)
{
	xml_tag_t child	     = tag == -1 ? tree_add(&xml->tags) : tree_add_child(&xml->tags, tag);
	xml_tag_data_t *data = get_tag(&xml->tags, child);
	if (data == NULL) {
		return -1;
	}

	*data = (xml_tag_data_t){
		.name  = { .data = name, .len = name_len, .mem = 0 },
		.attrs = -1,
		.val   = { .data = val, .len = val_len, .mem = val_mem },
	};
	return child;
}

xml_tag_t xml_add_tag(xml_t *xml, xml_tag_t tag, const char *name, size_t name_len)
{
	return xml_add_tag_val_r(xml, tag, name, name_len, NULL, 0, 0);
}

xml_tag_t xml_add_tag_val(xml_t *xml, xml_tag_t tag, const char *name, size_t name_len, const char *val, size_t val_len)
{
	return xml_add_tag_val_r(xml, tag, name, name_len, val, val_len, 0);
}

xml_tag_t xml_add_tag_val_c(xml_t *xml, xml_tag_t tag, const char *name, size_t name_len, const char *val, size_t val_len)
{
	size_t len = val_len + 1;
	char *data = m_malloc(len);
	if (data == NULL) {
		return -1;
	}

	m_memcpy(data, len, val, val_len);
	data[len - 1] = '\0';
	return xml_add_tag_val_r(xml, tag, name, name_len, data, len, 1);
}

xml_tag_t xml_add_tag_val_v(xml_t *xml, xml_tag_t tag, const char *name, size_t name_len, const char *format, va_list args)
{
	size_t len = c_sprintv(NULL, 0, format, args) + 1;
	char *data = m_malloc(len);
	if (data == NULL) {
		return -1;
	}

	c_sprintv(data, len, format, args);
	return xml_add_tag_val_r(xml, tag, name, name_len, data, len, 1);
}

xml_tag_t xml_add_tag_val_f(xml_t *xml, xml_tag_t tag, const char *name, size_t name_len, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	xml_tag_t attr = xml_add_tag_val_v(xml, tag, name, name_len, format, args);
	va_end(args);
	return attr;
}

void xml_remove_tag(xml_t *xml, xml_tag_t tag)
{
	tree_remove(&xml->tags, tag);
}

bool xml_has_child(const xml_t *xml, xml_tag_t tag)
{
	return tree_has_child(&xml->tags, tag);
}

static xml_attr_t add_attr(xml_t *xml, xml_tag_t tag)
{
	xml_tag_data_t *data = get_tag(&xml->tags, tag);
	if (data == NULL) {
		return -1;
	}

	return data->attrs == -1 ? get_tag(&xml->tags, tag)->attrs = list_add(&xml->attrs) : list_add_next(&xml->attrs, data->attrs);
}

xml_attr_t xml_add_attr_r(xml_t *xml, xml_tag_t tag, const char *name, size_t name_len, const char *val, size_t val_len, bool val_mem)
{
	xml_attr_t attr	      = add_attr(xml, tag);
	xml_attr_data_t *data = get_attr(&xml->attrs, attr);
	if (data == NULL) {
		return -1;
	}

	*data = (xml_attr_data_t){
		.name = { .data = name, .len = name_len, .mem = 0 },
		.val  = { .data = val, .len = val_len, .mem = val_mem },
	};

	return attr;
}

xml_attr_t xml_add_attr(xml_t *xml, xml_tag_t tag, const char *name, size_t name_len, const char *val, size_t val_len)
{
	return xml_add_attr_r(xml, tag, name, name_len, val, val_len, 0);
}

xml_attr_t xml_add_attr_c(xml_t *xml, xml_tag_t tag, const char *name, size_t name_len, const char *val, size_t val_len)
{
	size_t len = val_len + 1;
	char *data = m_malloc((size_t)len);
	if (data == NULL) {
		return -1;
	}

	m_memcpy(data, len, val, val_len);
	data[len - 1] = '\0';
	return xml_add_attr_r(xml, tag, name, name_len, data, len, 1);
}

xml_attr_t xml_add_attr_v(xml_t *xml, xml_tag_t tag, const char *name, size_t name_len, const char *format, va_list args)
{
	size_t len = c_sprintv(NULL, 0, format, args) + 1;
	char *data = m_malloc(len);
	if (data == NULL) {
		return -1;
	}

	c_sprintv(data, len, format, args);
	return xml_add_attr_r(xml, tag, name, name_len, data, len, 1);
}

xml_attr_t xml_add_attr_f(xml_t *xml, xml_tag_t tag, const char *name, size_t name_len, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	xml_attr_t attr = xml_add_attr_v(xml, tag, name, name_len, format, args);
	va_end(args);
	return attr;
}

static int xml_attr_print_cb(const list_t *list, lnode_t node, void *value, int ret, int last, void *priv)
{
	xml_attr_data_t *data = value;
	if (data == NULL) {
		return -1;
	}

	c_fprintf(priv, " %.*s=\"%.*s\"", data->name.len, data->name.data, data->val.len, data->val.data);
	return ret;
}

typedef struct xml_tag_print_cb_priv_s {
	const xml_t *xml;
	FILE *file;
	uint depth;
} xml_tag_print_cb_priv_t;

static int xml_tag_print(const xml_t *xml, xml_tag_t tag, void *value, FILE *file, uint depth);

static int xml_tag_print_cb(const tree_t *tree, tnode_t node, void *value, int ret, int last, void *priv)
{
	xml_tag_print_cb_priv_t *p = priv;

	return xml_tag_print(p->xml, node, value, p->file, p->depth);
}

static int xml_tag_print(const xml_t *xml, xml_tag_t tag, void *value, FILE *file, uint depth)
{
	xml_tag_data_t *data = value;
	if (data == NULL) {
		return 1;
	}

	c_fprintf(file, "%*s<%.*s", depth * 2, "", data->name.len, data->name.data);

	if (data->attrs != -1) {
		list_iterate(&xml->attrs, data->attrs, xml_attr_print_cb, 0, file);
	}

	xml_tag_print_cb_priv_t priv = {
		.xml   = xml,
		.file  = file,
		.depth = depth + 1,
	};

	if (tree_get_child(&xml->tags, tag) != -1) {
		c_fprintf(file, ">\n");

		tree_iterate_childs(&xml->tags, tag, xml_tag_print_cb, 0, &priv);

		c_fprintf(file, "%*s</%.*s>\n", depth * 2, "", data->name.len, data->name.data);
	} else if (data->val.data) {
		if (data->val.len > 0 && data->val.data[data->val.len - 1] == '\n') {
			c_fprintf(file, ">%.*s%*s</%.*s>\n", data->val.len, data->val.data, depth * 2, "", data->name.len, data->name.data);
		} else {
			c_fprintf(file, ">%.*s</%.*s>\n", data->val.len, data->val.data, data->name.len, data->name.data);
		}
	} else {
		c_fprintf(file, " />\n");
	}

	return 0;
}

int xml_print(const xml_t *xml, xml_tag_t tag, FILE *file)
{
	xml_tag_print_cb_priv_t priv = {
		.xml   = xml,
		.file  = file,
		.depth = 0,
	};

	c_fprintf(file, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
	xml_tag_print(xml, tag, get_tag(&xml->tags, tag), file, 0);
	return 0;
}
