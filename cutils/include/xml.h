#ifndef XML_H
#define XML_H

#include "list.h"
#include "str.h"
#include "tree.h"
#include "type.h"

#include <stdarg.h>

typedef tnode_t xml_tag_t;
typedef lnode_t xml_attr_t;

typedef struct xml_s {
	tree_t tags;
	list_t attrs;
} xml_t;

xml_t *xml_init(xml_t *xml, uint tags_cap, uint attrs_cap);
void xml_free(xml_t *xml);

xml_tag_t xml_add_tag(xml_t *xml, xml_tag_t tag, str_t name);
xml_tag_t xml_add_tag_val(xml_t *xml, xml_tag_t tag, str_t name, str_t val);

int xml_remove_tag(xml_t *xml, xml_tag_t tag);
bool xml_has_child(const xml_t *xml, xml_tag_t tag);

xml_attr_t xml_add_attr(xml_t *xml, xml_tag_t tag, str_t name, str_t val);

int xml_print(const xml_t *xml, xml_tag_t tag, FILE *file);

#endif
