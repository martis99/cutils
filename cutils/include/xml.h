#ifndef XML_H
#define XML_H

#include "tree.h"

#include <stdarg.h>

typedef struct xml_str_s {
	union {
		const char *data;
		char *tdata;
	};
	unsigned int len;
	unsigned char mem;
} xml_str_t;

typedef tnode_t xml_tag_t;
typedef tnode_t xml_attr_t;

typedef struct xml_s {
	tree_t tags;
	tree_t attrs;
} xml_t;

xml_t *xml_init(xml_t *xml, unsigned int cap);
void xml_free(xml_t *xml);

xml_attr_t xml_add_attr_r(xml_t *xml, xml_tag_t tag, const char *name, unsigned int name_len, const char *val, unsigned int val_len, unsigned char val_mem);
xml_attr_t xml_add_attr(xml_t *xml, xml_tag_t tag, const char *name, unsigned int name_len, const char *val, unsigned int val_len);
xml_attr_t xml_add_attr_c(xml_t *xml, xml_tag_t tag, const char *name, unsigned int name_len, const char *val, unsigned int val_len);
xml_attr_t xml_add_attr_v(xml_t *xml, xml_tag_t tag, const char *name, unsigned int name_len, const char *format, va_list args);
xml_attr_t xml_add_attr_f(xml_t *xml, xml_tag_t tag, const char *name, unsigned int name_len, const char *format, ...);

xml_tag_t xml_add_child(xml_t *xml, xml_tag_t tag, const char *name, unsigned int name_len);

xml_tag_t xml_add_child_val_r(xml_t *xml, xml_tag_t tag, const char *name, unsigned int name_len, const char *val, unsigned int val_len, unsigned char val_mem);
xml_tag_t xml_add_child_val(xml_t *xml, xml_tag_t tag, const char *name, unsigned int name_len, const char *val, unsigned int val_len);
xml_tag_t xml_add_child_val_c(xml_t *xml, xml_tag_t tag, const char *name, unsigned int name_len, const char *val, unsigned int val_len);
xml_tag_t xml_add_child_val_v(xml_t *xml, xml_tag_t tag, const char *name, unsigned int name_len, const char *format, va_list args);
xml_tag_t xml_add_child_val_f(xml_t *xml, xml_tag_t tag, const char *name, unsigned int name_len, const char *format, ...);

int xml_print(const xml_t *xml, FILE *file);

#endif
