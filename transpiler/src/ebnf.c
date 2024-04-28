#include "ebnf.h"

#include "bnf.h"
#include "log.h"
#include "token.h"

ebnf_t *ebnf_init(ebnf_t *ebnf)
{
	if (ebnf == NULL) {
		return NULL;
	}

	return ebnf;
}

void ebnf_free(ebnf_t *ebnf)
{
	if (ebnf == NULL) {
		return;
	}

	stx_free(&ebnf->stx);
}

const stx_t *ebnf_get_stx(ebnf_t *ebnf)
{
	if (ebnf == NULL) {
		return NULL;
	}

	str_t sbnf = STR("<file>    ::= <ebnf> EOF\n"
			 "<ebnf>    ::= <rules>\n"
			 "<rules>   ::= <rule> <rules> | <rule>\n"
			 "<rule>    ::= <rname> <spaces> '=' <space> <alt> NL\n"
			 "<rname>   ::= LOWER <rchars> | LOWER\n"
			 "<rchars>  ::= <rchar> <rchars> | <rchar>\n"
			 "<rchar>   ::= LOWER | '_'\n"
			 "<alt>     ::= <concat> <space> '|' <space> <alt> | <concat>\n"
			 "<concat>  ::= <factor> <space> <concat> | <factor>\n"
			 "<factor>  ::= <term> <opt> | <term> <rep> | <term> <opt-rep> | <term>\n"
			 "<opt>     ::= '?'\n"
			 "<rep>     ::= '+'\n"
			 "<opt-rep> ::= '*'\n"
			 "<term>    ::= <literal> | <token> | <rname> | <group>\n"
			 "<literal> ::= \"'\" <tdouble> \"'\" | '\"' <tsingle> '\"'\n"
			 "<token>   ::= UPPER <token> | UPPER\n"
			 "<group>   ::= '(' <alt> ')'\n"
			 "<tdouble> ::= <cdouble> <tdouble> | <cdouble>\n"
			 "<tsingle> ::= <csingle> <tsingle> | <csingle>\n"
			 "<cdouble> ::= <char> | '\"'\n"
			 "<csingle> ::= <char> | \"'\"\n"
			 "<char>    ::= ALPHA | DIGIT | SYMBOL | ' '\n"
			 "<spaces>  ::= <space> <spaces> | <space>\n"
			 "<space>   ::= ' '\n");

	lex_t lex = { 0 };
	if (lex_init(&lex, 100) == NULL) {
		log_error("cutils", "bnf", NULL, "failed to intialize lexer");
		return NULL;
	}

	lex_tokenize(&lex, sbnf);

	prs_t prs = { 0 };
	prs_init(&prs, 100);

	bnf_t bnf = { 0 };
	bnf_get_stx(&bnf);
	prs_node_t prs_root = prs_parse(&prs, &bnf.stx, bnf.file, &lex);
	bnf_free(&bnf);

	stx_init(&ebnf->stx, 10, 10);
	stx_from_bnf(&bnf, &prs, prs_root, &ebnf->stx);

	ebnf->file    = stx_get_rule(&ebnf->stx, STR("file"));
	ebnf->ebnf    = stx_get_rule(&ebnf->stx, STR("ebnf"));
	ebnf->rules   = stx_get_rule(&ebnf->stx, STR("rules"));
	ebnf->rule    = stx_get_rule(&ebnf->stx, STR("rule"));
	ebnf->rname   = stx_get_rule(&ebnf->stx, STR("rname"));
	ebnf->alt     = stx_get_rule(&ebnf->stx, STR("alt"));
	ebnf->concat  = stx_get_rule(&ebnf->stx, STR("concat"));
	ebnf->factor  = stx_get_rule(&ebnf->stx, STR("factor"));
	ebnf->term    = stx_get_rule(&ebnf->stx, STR("term"));
	ebnf->literal = stx_get_rule(&ebnf->stx, STR("literal"));
	ebnf->tdouble = stx_get_rule(&ebnf->stx, STR("tdouble"));
	ebnf->tsingle = stx_get_rule(&ebnf->stx, STR("tsingle"));
	ebnf->token   = stx_get_rule(&ebnf->stx, STR("token"));
	ebnf->group   = stx_get_rule(&ebnf->stx, STR("group"));
	ebnf->opt     = stx_get_rule(&ebnf->stx, STR("opt"));
	ebnf->rep     = stx_get_rule(&ebnf->stx, STR("rep"));
	ebnf->opt_rep = stx_get_rule(&ebnf->stx, STR("opt-rep"));

	prs_free(&prs);
	lex_free(&lex);

	return &ebnf->stx;
}

static void alt_from_ebnf(const ebnf_t *ebnf, const prs_t *prs, prs_node_t node, estx_t *estx, estx_term_t parent, estx_rule_t rule, int recursive);

static void term_from_ebnf(const ebnf_t *ebnf, const prs_t *prs, prs_node_t node, estx_t *estx, estx_term_t parent, estx_term_occ_t occ, estx_rule_t rule)
{
	const prs_node_t prs_rname = prs_get_rule(prs, node, ebnf->rname);
	if (prs_rname < prs->nodes.cnt) {
		str_t rname = strz(16);
		prs_get_str(prs, prs_rname, &rname);
		estx_rule_t new_rule = estx_get_rule(estx, rname);
		if (new_rule >= estx->rules.cnt) {
			new_rule = estx_add_rule(estx, rname);
		} else {
			str_free(&rname);
		}

		if (rule != ESTX_RULE_END) {
			estx_rule_set_term(estx, rule, ESTX_TERM_RULE(estx, new_rule, occ));
		} else {
			estx_term_add_term(estx, parent, ESTX_TERM_RULE(estx, new_rule, occ));
		}
		return;
	}

	const prs_node_t prs_literal = prs_get_rule(prs, node, ebnf->literal);
	if (prs_literal < prs->nodes.cnt) {
		const prs_node_t prs_text_double = prs_get_rule(prs, prs_literal, ebnf->tdouble);
		const prs_node_t prs_text_single = prs_get_rule(prs, prs_literal, ebnf->tsingle);

		str_t literal = strz(16);

		if (prs_text_double < prs->nodes.cnt) {
			prs_get_str(prs, prs_text_double, &literal);
		} else if (prs_text_single < prs->nodes.cnt) {
			prs_get_str(prs, prs_text_single, &literal);
		} else {
			str_free(&literal);
			return;
		}

		if (rule != ESTX_RULE_END) {
			estx_rule_set_term(estx, rule, ESTX_TERM_LITERAL(estx, literal, occ));
		} else {
			estx_term_add_term(estx, parent, ESTX_TERM_LITERAL(estx, literal, occ));
		}
		return;
	}

	const prs_node_t prs_token = prs_get_rule(prs, node, ebnf->token);
	if (prs_token < prs->nodes.cnt) {
		str_t token = strz(16);
		prs_get_str(prs, prs_token, &token);
		if (rule != ESTX_RULE_END) {
			estx_rule_set_term(estx, rule, ESTX_TERM_TOKEN(estx, token_type_enum(token), occ));
		} else {
			estx_term_t tok = ESTX_TERM_TOKEN(estx, token_type_enum(token), occ);
			estx_term_add_term(estx, parent, tok);
		}
		str_free(&token);
		return;
	}

	const prs_node_t prs_group = prs_get_rule(prs, node, ebnf->group);
	if (prs_group < prs->nodes.cnt) {
		const estx_term_t group	 = rule == ESTX_RULE_END ? estx_term_add_term(estx, parent, ESTX_TERM_GROUP(estx, occ)) :
								   estx_rule_set_term(estx, rule, ESTX_TERM_GROUP(estx, occ));
		const prs_node_t prs_alt = prs_get_rule(prs, prs_group, ebnf->alt);
		alt_from_ebnf(ebnf, prs, prs_alt, estx, group, ESTX_RULE_END, 0);
	}
}

static void factor_from_ebnf(const ebnf_t *ebnf, const prs_t *prs, prs_node_t node, estx_t *estx, estx_term_t parent, estx_rule_t rule)
{
	const prs_node_t prs_term = prs_get_rule(prs, node, ebnf->term);
	estx_term_occ_t occ	  = ESTX_TERM_OCC_ONE;

	const prs_node_t prs_opt = prs_get_rule(prs, node, ebnf->opt);
	if (prs_opt < prs->nodes.cnt) {
		occ = ESTX_TERM_OCC_OPT;
	}

	const prs_node_t prs_rep = prs_get_rule(prs, node, ebnf->rep);
	if (prs_rep < prs->nodes.cnt) {
		occ = ESTX_TERM_OCC_REP;
	}

	const prs_node_t prs_opt_rep = prs_get_rule(prs, node, ebnf->opt_rep);
	if (prs_opt_rep < prs->nodes.cnt) {
		occ = ESTX_TERM_OCC_OPT | ESTX_TERM_OCC_REP;
	}

	term_from_ebnf(ebnf, prs, prs_term, estx, parent, occ, rule);
}

static void concat_from_ebnf(const ebnf_t *ebnf, const prs_t *prs, prs_node_t node, estx_t *estx, estx_term_t parent, estx_rule_t rule, int recursive)
{
	const prs_node_t prs_factor = prs_get_rule(prs, node, ebnf->factor);

	const prs_node_t prs_concat = prs_get_rule(prs, node, ebnf->concat);
	if (prs_concat < prs->nodes.cnt) {
		estx_term_t concat = parent;
		if (rule != ESTX_RULE_END) {
			concat = estx_rule_set_term(estx, rule, ESTX_TERM_CON(estx));
		} else if (!recursive) {
			concat = estx_term_add_term(estx, parent, ESTX_TERM_CON(estx));
		}
		factor_from_ebnf(ebnf, prs, prs_factor, estx, concat, STX_RULE_END);
		concat_from_ebnf(ebnf, prs, prs_concat, estx, concat, STX_RULE_END, 1);
	} else {
		factor_from_ebnf(ebnf, prs, prs_factor, estx, parent, rule);
	}
}

static void alt_from_ebnf(const ebnf_t *ebnf, const prs_t *prs, prs_node_t node, estx_t *estx, estx_term_t parent, estx_rule_t rule, int recursive)
{
	const prs_node_t prs_concat = prs_get_rule(prs, node, ebnf->concat);

	const prs_node_t prs_alt = prs_get_rule(prs, node, ebnf->alt);
	if (prs_alt < prs->nodes.cnt) {
		estx_term_t alt = parent;
		if (rule != ESTX_RULE_END) {
			alt = estx_rule_set_term(estx, rule, ESTX_TERM_ALT(estx));
		} else if (!recursive) {
			alt = estx_term_add_term(estx, parent, ESTX_TERM_ALT(estx));
		}
		concat_from_ebnf(ebnf, prs, prs_concat, estx, alt, STX_RULE_END, 0);
		alt_from_ebnf(ebnf, prs, prs_alt, estx, alt, STX_RULE_END, 1);
	} else {
		concat_from_ebnf(ebnf, prs, prs_concat, estx, parent, rule, 0);
	}
}

static estx_rule_t rules_from_ebnf(const ebnf_t *ebnf, const prs_t *prs, prs_node_t node, estx_t *estx)
{
	const prs_node_t prs_rule  = prs_get_rule(prs, node, ebnf->rule);
	const prs_node_t prs_rname = prs_get_rule(prs, prs_rule, ebnf->rname);

	str_t rname = strz(16);
	prs_get_str(prs, prs_rname, &rname);
	estx_rule_t rule = estx_get_rule(estx, rname);
	if (rule >= estx->rules.cnt) {
		rule = estx_add_rule(estx, rname);
	} else {
		str_free(&rname);
	}

	const prs_node_t prs_alt = prs_get_rule(prs, prs_rule, ebnf->alt);
	alt_from_ebnf(ebnf, prs, prs_alt, estx, STX_TERM_END, rule, 0);

	const prs_node_t rules = prs_get_rule(prs, node, ebnf->rules);
	if (rules >= prs->nodes.cnt) {
		return rule;
	}

	rules_from_ebnf(ebnf, prs, rules, estx);

	return rule;
}

estx_rule_t estx_from_ebnf(const ebnf_t *ebnf, const prs_t *prs, prs_node_t root, estx_t *estx)
{
	prs_node_t febnf = prs_get_rule(prs, root, ebnf->ebnf);
	prs_node_t rules = prs_get_rule(prs, febnf, ebnf->rules);

	estx_rule_t estx_root = rules_from_ebnf(ebnf, prs, rules, estx);

	estx_compile(estx);

	return root;
}
