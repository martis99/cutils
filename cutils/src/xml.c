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

xml_t *xml_init(xml_t *xml, unsigned int cap)
{
	if (tree_init(&xml->tags, cap, sizeof(xml_tag_data_t)) == NULL) {
		return NULL;
	}

	if (tree_init(&xml->attrs, cap, sizeof(xml_attr_data_t)) == NULL) {
		return NULL;
	}

	return xml;
}

static int xml_str_free(xml_str_t *str)
{
	if (str->mem) {
		m_free(str->tdata, (size_t)str->len);
	}
	return 0;
}

static int xml_tag_free(const tree_t *tree, tnode_t node, int depth, int last, void *priv)
{
	xml_tag_data_t *tag_d = tree_get_data(tree, node);

	xml_str_free(&tag_d->name);
	xml_str_free(&tag_d->val);

	return 0;
}

static int xml_attr_free(const tree_t *tree, tnode_t node, int depth, int last, void *priv)
{
	xml_attr_data_t *attr_d = tree_get_data(tree, node);

	xml_str_free(&attr_d->name);
	xml_str_free(&attr_d->val);
	return 0;
}

void xml_free(xml_t *xml)
{
	tree_iterate_pre(&xml->tags, 0, xml_tag_free, NULL);
	tree_free(&xml->tags);

	tree_iterate_pre(&xml->attrs, 0, xml_attr_free, NULL);
	tree_free(&xml->attrs);
}

static xml_attr_t add_attr(xml_t *xml, xml_tag_t tag)
{
	xml_tag_data_t *tag_d = tree_get_data(&xml->tags, tag);

	if (tag_d->attrs == 0) {
		tag_d->attrs = tree_add_child(&xml->attrs, 0);
		return tag_d->attrs;
	}

	return tree_add_child(&xml->attrs, tag_d->attrs);
}

xml_attr_t xml_add_attr_r(xml_t *xml, xml_tag_t tag, const char *name, unsigned int name_len, const char *val, unsigned int val_len, unsigned char val_mem)
{
	xml_attr_t attr		= add_attr(xml, tag);
	xml_attr_data_t *attr_d = tree_get_data(&xml->attrs, attr);

	*attr_d = (xml_attr_data_t){
		.name = { .data = name, .len = name_len, .mem = 0 },
		.val  = { .data = val, .len = val_len, .mem = val_mem },
	};

	return attr;
}

xml_attr_t xml_add_attr(xml_t *xml, xml_tag_t tag, const char *name, unsigned int name_len, const char *val, unsigned int val_len)
{
	return xml_add_attr_r(xml, tag, name, name_len, val, val_len, 0);
}

xml_attr_t xml_add_attr_c(xml_t *xml, xml_tag_t tag, const char *name, unsigned int name_len, const char *val, unsigned int val_len)
{
	unsigned int len = val_len + 1;
	char *data	 = m_malloc((size_t)len);
	m_memcpy(data, len, val, val_len);
	data[len - 1] = '\0';
	return xml_add_attr_r(xml, tag, name, name_len, data, len, 1);
}

xml_attr_t xml_add_attr_v(xml_t *xml, xml_tag_t tag, const char *name, unsigned int name_len, const char *format, va_list args)
{
	unsigned int len = p_vsnprintf(NULL, 0, format, args) + 1;
	char *data	 = m_malloc((size_t)len);
	p_vsnprintf(data, (size_t)len, format, args);
	return xml_add_attr_r(xml, tag, name, name_len, data, len, 1);
}

xml_attr_t xml_add_attr_f(xml_t *xml, xml_tag_t tag, const char *name, unsigned int name_len, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	xml_attr_t attr = xml_add_attr_v(xml, tag, name, name_len, format, args);
	va_end(args);
	return attr;
}

xml_tag_t xml_add_child(xml_t *xml, xml_tag_t tag, const char *name, unsigned int name_len)
{
	xml_tag_t child		= tree_add_child(&xml->tags, tag);
	xml_tag_data_t *child_d = tree_get_data(&xml->tags, child);

	*child_d = (xml_tag_data_t){
		.name  = { .data = name, .len = name_len, .mem = 0 },
		.attrs = 0,
		.val   = { 0 },
	};

	return child;
}

xml_tag_t xml_add_child_val_r(xml_t *xml, xml_tag_t tag, const char *name, unsigned int name_len, const char *val, unsigned int val_len, unsigned char val_mem)
{
	xml_tag_t child		= tree_add_child(&xml->tags, tag);
	xml_tag_data_t *child_d = tree_get_data(&xml->tags, child);

	*child_d = (xml_tag_data_t){
		.name  = { .data = name, .len = name_len, .mem = 0 },
		.attrs = 0,
		.val   = { .data = val, .len = val_len, .mem = val_mem },
	};
	return child;
}

xml_tag_t xml_add_child_val(xml_t *xml, xml_tag_t tag, const char *name, unsigned int name_len, const char *val, unsigned int val_len)
{
	return xml_add_child_val_r(xml, tag, name, name_len, val, val_len, 0);
}

xml_tag_t xml_add_child_val_c(xml_t *xml, xml_tag_t tag, const char *name, unsigned int name_len, const char *val, unsigned int val_len)
{
	unsigned int len = val_len + 1;
	char *data	 = m_malloc((size_t)len);
	m_memcpy(data, len, val, val_len);
	data[len - 1] = '\0';
	return xml_add_child_val_r(xml, tag, name, name_len, data, len, 1);
}

xml_tag_t xml_add_child_val_v(xml_t *xml, xml_tag_t tag, const char *name, unsigned int name_len, const char *format, va_list args)
{
	unsigned int len = p_vsnprintf(NULL, 0, format, args) + 1;
	char *data	 = m_malloc((size_t)len);
	p_vsnprintf(data, (size_t)len, format, args);
	return xml_add_child_val_r(xml, tag, name, name_len, data, len, 1);
}

xml_tag_t xml_add_child_val_f(xml_t *xml, xml_tag_t tag, const char *name, unsigned int name_len, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	xml_tag_t attr = xml_add_child_val_v(xml, tag, name, name_len, format, args);
	va_end(args);
	return attr;
}

static int xml_attr_print_cb(const tree_t *tree, tnode_t node, int last, void *priv)
{
	xml_attr_data_t *attr_d = tree_get_data(tree, node);

	p_fprintf(priv, " %.*s=\"%.*s\"", attr_d->name.len, attr_d->name.data, attr_d->val.len, attr_d->val.data);
	return 0;
}

typedef struct xml_tag_print_cb_priv_s {
	const xml_t *xml;
	FILE *file;
	unsigned int depth;
} xml_tag_print_cb_priv_t;

static int xml_tag_print(const xml_t *xml, xml_tag_t tag, FILE *file, unsigned int depth);

static int xml_tag_print_cb(const tree_t *tree, tnode_t node, int last, void *priv)
{
	xml_tag_print_cb_priv_t *p = priv;

	return xml_tag_print(p->xml, node, p->file, p->depth);
}

static int xml_tag_print(const xml_t *xml, xml_tag_t tag, FILE *file, unsigned int depth)
{
	xml_tag_data_t *tag_d = tree_get_data(&xml->tags, tag);

	p_fprintf(file, "%*s<%.*s", depth * 2, "", tag_d->name.len, tag_d->name.data);

	if (tag_d->attrs != 0) {
		xml_attr_print_cb(&xml->attrs, tag_d->attrs, 0, file);
		tree_iterate_childs(&xml->attrs, tag_d->attrs, xml_attr_print_cb, file);
	}

	xml_tag_print_cb_priv_t priv = {
		.xml   = xml,
		.file  = file,
		.depth = depth + 1,
	};

	if (tree_get_child(&xml->tags, tag) != 0) {
		p_fprintf(file, ">\n");

		tree_iterate_childs(&xml->tags, tag, xml_tag_print_cb, &priv);

		p_fprintf(file, "%*s</%.*s>\n", depth * 2, "", tag_d->name.len, tag_d->name.data);
	} else if (tag_d->val.data) {
		if (tag_d->val.len > 0 && tag_d->val.data[tag_d->val.len - 1] == '\n') {
			p_fprintf(file, ">%.*s%*s</%.*s>\n", tag_d->val.len, tag_d->val.data, depth * 2, "", tag_d->name.len, tag_d->name.data);
		} else {
			p_fprintf(file, ">%.*s</%.*s>\n", tag_d->val.len, tag_d->val.data, tag_d->name.len, tag_d->name.data);
		}
	} else {
		p_fprintf(file, " />\n");
	}

	return 0;
}

int xml_print(const xml_t *xml, FILE *file)
{
	xml_tag_print_cb_priv_t priv = {
		.xml   = xml,
		.file  = file,
		.depth = 0,
	};

	p_fprintf(file, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
	tree_iterate_childs(&xml->tags, 0, xml_tag_print_cb, &priv);
	return 0;
}
