#ifndef XML_H
#define XML_H

#include "list.h"
#include "tree.h"
#include "type.h"

#include <stdarg.h>

typedef struct xml_str_s {
	union {
		const char *data;
		char *tdata;
	};
	size_t len;
	bool mem;
} xml_str_t;

typedef tnode_t xml_tag_t;
typedef lnode_t xml_attr_t;

typedef struct xml_s {
	tree_t tags;
	list_t attrs;
} xml_t;

xml_t *xml_init(xml_t *xml, uint cap);
void xml_free(xml_t *xml);

xml_tag_t xml_add_tag_val_r(xml_t *xml, xml_tag_t tag, const char *name, size_t name_len, const char *val, size_t val_len, bool val_mem);
xml_tag_t xml_add_tag(xml_t *xml, xml_tag_t tag, const char *name, size_t name_len);
xml_tag_t xml_add_tag_val(xml_t *xml, xml_tag_t tag, const char *name, size_t name_len, const char *val, size_t val_len);
xml_tag_t xml_add_tag_val_c(xml_t *xml, xml_tag_t tag, const char *name, size_t name_len, const char *val, size_t val_len);
xml_tag_t xml_add_tag_val_v(xml_t *xml, xml_tag_t tag, const char *name, size_t name_len, const char *format, va_list args);
xml_tag_t xml_add_tag_val_f(xml_t *xml, xml_tag_t tag, const char *name, size_t name_len, const char *format, ...);

xml_attr_t xml_add_attr_r(xml_t *xml, xml_tag_t tag, const char *name, size_t name_len, const char *val, size_t val_len, bool val_mem);
xml_attr_t xml_add_attr(xml_t *xml, xml_tag_t tag, const char *name, size_t name_len, const char *val, size_t val_len);
xml_attr_t xml_add_attr_c(xml_t *xml, xml_tag_t tag, const char *name, size_t name_len, const char *val, size_t val_len);
xml_attr_t xml_add_attr_v(xml_t *xml, xml_tag_t tag, const char *name, size_t name_len, const char *format, va_list args);
xml_attr_t xml_add_attr_f(xml_t *xml, xml_tag_t tag, const char *name, size_t name_len, const char *format, ...);

int xml_print(const xml_t *xml, xml_tag_t tag, FILE *file);

#endif
