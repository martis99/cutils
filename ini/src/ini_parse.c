#include "ini_parse.h"

#include "ebnf.h"
#include "eparser.h"

ini_prs_t *ini_prs_init(ini_prs_t *ini_prs)
{
	if (ini_prs == NULL) {
		return NULL;
	}

	ebnf_t ebnf = { 0 };
	if (ebnf_get_stx(&ebnf) == NULL) {
		return NULL;
	}

	str_t sbnf = STR("file   = ini EOF\n"
			 "ini    = ('[' sec ']' NL | pair | NL)*\n"
			 "sec    = ALPHA (ALPHA | DIGIT | '_' | '.')*\n"
			 "pair   = key spaces '=' spaces vals? NL | vals NL\n"
			 "key    = ALPHA (ALPHA | '_')*\n"
			 "vals   = val (',' spaces val)*\n"
			 "val    = char+\n"
			 "char   = ALPHA | DIGIT | '/' | '\\' | ':' | '.' | ' '\n"
			 "spaces = ' '*\n");

	lex_t lex = { 0 };
	lex_init(&lex, 100);
	lex_tokenize(&lex, sbnf);

	prs_t prs = { 0 };
	prs_init(&prs, 100);

	prs_node_t prs_root = prs_parse(&prs, &ebnf.stx, ebnf.file, &lex);

	estx_init(&ini_prs->estx, 10, 10);
	estx_from_ebnf(&ebnf, &prs, prs_root, &ini_prs->estx);
	ebnf_free(&ebnf);

	ini_prs->file = estx_get_rule(&ini_prs->estx, STR("file"));
	ini_prs->ini  = estx_get_rule(&ini_prs->estx, STR("ini"));
	ini_prs->sec  = estx_get_rule(&ini_prs->estx, STR("sec"));
	ini_prs->pair = estx_get_rule(&ini_prs->estx, STR("pair"));
	ini_prs->key  = estx_get_rule(&ini_prs->estx, STR("key"));
	ini_prs->vals = estx_get_rule(&ini_prs->estx, STR("vals"));
	ini_prs->val  = estx_get_rule(&ini_prs->estx, STR("val"));

	lex_free(&lex);
	prs_free(&prs);

	return ini_prs;
}

ini_prs_t *ini_prs_free(ini_prs_t *ini_prs)
{
	if (ini_prs == NULL) {
		return NULL;
	}

	estx_free(&ini_prs->estx);
}

ini_sec_t ini_parse_sec(const ini_prs_t *ini_prs, eprs_t *eprs, eprs_node_t sec, ini_t *ini)
{
	str_t name = strz(16);
	eprs_get_str(eprs, sec, &name);
	return ini_add_sec(ini, name);
}

void ini_parse_pair(const ini_prs_t *ini_prs, eprs_t *eprs, eprs_node_t prs_pair, ini_t *ini, ini_sec_t sec)
{
	str_t key;
	eprs_node_t prs_key = eprs_get_rule(eprs, prs_pair, ini_prs->key);
	if (prs_key < eprs->nodes.cnt) {
		key = strz(16);
		eprs_get_str(eprs, prs_key, &key);
	} else {
		key = str_null();
	}
	ini_pair_t pair = ini_add_pair(ini, sec, key);

	eprs_node_t prs_vals = eprs_get_rule(eprs, prs_pair, ini_prs->vals);
	eprs_node_t child;
	tree_foreach_child(&eprs->nodes, prs_vals, child)
	{
		eprs_node_t prs_val = eprs_get_rule(eprs, child, ini_prs->val);
		if (prs_val < eprs->nodes.cnt) {
			str_t val = strz(16);
			eprs_get_str(eprs, prs_val, &val);
			ini_add_val(ini, pair, val);
		}
	}
}

void ini_parse_file(const ini_prs_t *ini_prs, eprs_t *eprs, eprs_node_t file, ini_t *ini)
{
	eprs_node_t prs_ini = eprs_get_rule(eprs, file, ini_prs->ini);

	ini_sec_t sec = INI_SEC_END;

	eprs_node_t child;
	tree_foreach_child(&eprs->nodes, prs_ini, child)
	{
		eprs_node_t prs_sec = eprs_get_rule(eprs, child, ini_prs->sec);
		if (prs_sec < eprs->nodes.cnt) {
			sec = ini_parse_sec(ini_prs, eprs, prs_sec, ini);
			continue;
		}

		eprs_node_t prs_pair = eprs_get_rule(eprs, child, ini_prs->pair);
		if (prs_pair < eprs->nodes.cnt) {
			sec = sec == INI_SEC_END ? ini_add_sec(ini, str_null()) : sec;
			ini_parse_pair(ini_prs, eprs, prs_pair, ini, sec);
			continue;
		}
	}
}

ini_t *ini_prs_parse(const ini_prs_t *ini_prs, str_t str, ini_t *ini)
{
	if (ini_prs == NULL || ini == NULL) {
		return NULL;
	}

	lex_t lex = { 0 };
	lex_init(&lex, 100);
	lex_tokenize(&lex, str);

	eprs_t eprs = { 0 };
	eprs_init(&eprs, 100);

	eprs_node_t prs_root = eprs_parse(&eprs, &ini_prs->estx, ini_prs->file, &lex);

	ini_parse_file(ini_prs, &eprs, prs_root, ini);

	lex_free(&lex);
	eprs_free(&eprs);

	return ini;
}
