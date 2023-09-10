#include "xml.h"

#include "mem.h"
#include "print.h"

typedef struct xml_tag_data_s {
	str_t name;
	xml_attr_t attrs;
	str_t val;
} xml_tag_data_t;

typedef struct xml_attr_data_s {
	str_t name;
	str_t val;
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
	if (xml == NULL) {
		return NULL;
	}

	if (tree_init(&xml->tags, cap, sizeof(xml_tag_data_t)) == NULL) {
		return NULL;
	}

	if (list_init(&xml->attrs, cap, sizeof(xml_attr_data_t)) == NULL) {
		return NULL;
	}

	return xml;
}

void xml_free(xml_t *xml)
{
	if (xml == NULL) {
		return;
	}

	xml_tag_t tag;
	tree_foreach_all(&xml->tags, tag)
	{
		xml_tag_data_t *data = tree_get_data(&xml->tags, tag);
		str_free(&data->name);
		str_free(&data->val);
	}

	tree_free(&xml->tags);

	xml_attr_data_t *attr;
	list_foreach_all(&xml->attrs, attr)
	{
		str_free(&attr->name);
		str_free(&attr->val);
	}

	list_free(&xml->attrs);
}

xml_tag_t xml_add_tag_val(xml_t *xml, xml_tag_t tag, str_t name, str_t val)
{
	if (xml == NULL) {
		return TREE_END;
	}

	xml_tag_t child;
	tree_add_child_node(&xml->tags, tag, child);
	xml_tag_data_t *data = get_tag(&xml->tags, child);
	if (data == NULL) {
		return TREE_END;
	}

	*data = (xml_tag_data_t){
		.name  = name,
		.attrs = LIST_END,
		.val   = val,
	};

	return child;
}

xml_tag_t xml_add_tag(xml_t *xml, xml_tag_t tag, str_t name)
{
	return xml_add_tag_val(xml, tag, name, str_null());
}

int xml_remove_tag(xml_t *xml, xml_tag_t tag)
{
	if (xml == NULL) {
		return 1;
	}

	return tree_remove(&xml->tags, tag);
}

bool xml_has_child(const xml_t *xml, xml_tag_t tag)
{
	if (xml == NULL) {
		return 0;
	}

	return tree_has_child(&xml->tags, tag);
}

static inline xml_attr_t add_attr(xml_t *xml, xml_tag_t tag)
{
	xml_tag_data_t *data = get_tag(&xml->tags, tag);

	if (data == NULL) {
		return LIST_END;
	}

	xml_attr_t attr;
	list_add_next_node(&xml->attrs, get_tag(&xml->tags, tag)->attrs, attr);
	return attr;
}

xml_attr_t xml_add_attr(xml_t *xml, xml_tag_t tag, str_t name, str_t val)
{
	if (xml == NULL) {
		return LIST_END;
	}

	xml_attr_t attr	      = add_attr(xml, tag);
	xml_attr_data_t *data = get_attr(&xml->attrs, attr);

	if (data == NULL) {
		return LIST_END;
	}

	*data = (xml_attr_data_t){
		.name = name,
		.val  = val,
	};

	return attr;
}

static int xml_tag_print(const xml_t *xml, xml_tag_t tag, FILE *file, uint depth)
{
	if (xml == NULL) {
		return 1;
	}

	const xml_tag_data_t *data = get_tag(&xml->tags, tag);

	if (data == NULL) {
		return 1;
	}

	int ret = 0;
	ret |= !c_fprintf(file, "%*s<%.*s", depth * 2, "", data->name.len, data->name.data);

	if (data->attrs != LIST_END) {
		xml_attr_data_t *attr;
		list_foreach(&xml->attrs, 0, attr)
		{
			ret |= !c_fprintf(file, " %.*s=\"%.*s\"", attr->name.len, attr->name.data, attr->val.len, attr->val.data);
		}
	}

	if (tree_get_child(&xml->tags, tag) != TREE_END) {
		ret |= !c_fprintf(file, ">\n");

		xml_tag_t child;
		tree_foreach_child(&xml->tags, tag, child)
		{
			ret |= xml_tag_print(xml, child, file, depth + 1);
		}

		ret |= !c_fprintf(file, "%*s</%.*s>\n", depth * 2, "", data->name.len, data->name.data);
	} else if (data->val.data) {
		if (data->val.len > 0 && data->val.data[data->val.len - 1] == '\n') {
			ret |= !c_fprintf(file, ">%.*s%*s</%.*s>\n", data->val.len, data->val.data, depth * 2, "", data->name.len, data->name.data);
		} else {
			ret |= !c_fprintf(file, ">%.*s</%.*s>\n", data->val.len, data->val.data, data->name.len, data->name.data);
		}
	} else {
		ret |= !c_fprintf(file, " />\n");
	}

	return ret;
}

int xml_print(const xml_t *xml, xml_tag_t tag, FILE *file)
{
	int ret = 0;
	ret |= !c_fprintf(file, "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
	ret |= xml_tag_print(xml, tag, file, 0);
	return ret;
}
