#include "t_cstr.h"

#include "cstr.h"

#include "test.h"

TEST(t_cstrf_null)
{
	START;

	char buf[16] = { 0 };

	EXPECT_EQ(cstrf(NULL, 1, "%s", "a"), 0);
	EXPECT_EQ(cstrf(buf, 0, NULL), 0);

	END;
}

TEST(t_cstr_catn_null)
{
	START;

	char buf[16] = "abc";
	EXPECT_EQ(cstr_catn(NULL, 0, 0, "def", 3, 0), 0);
	EXPECT_EQ(cstr_catn(buf, 0, 0, NULL, 0, 0), 0);

	END;
}

TEST(t_cstr_cat_null)
{
	START;

	char buf[16] = "abc";
	EXPECT_EQ(cstr_cat(NULL, 0, 0, "def", 0), 0);
	EXPECT_EQ(cstr_cat(buf, 0, 0, NULL, 0), 0);

	END;
}

TEST(t_cstr_len_null)
{
	START;
	EXPECT_EQ(cstr_len(NULL), 0);
	END;
}

TEST(t_cstr_cmpn_null)
{
	START;
	EXPECT_EQ(cstr_cmpn(NULL, 0, NULL, 0, 0), 0);
	EXPECT_EQ(cstr_cmpn("abc", 0, NULL, 0, 0), 1);
	EXPECT_EQ(cstr_cmpn("abc", 3, NULL, 0, 0), 1);
	EXPECT_EQ(cstr_cmpn(NULL, 0, "abc", 0, 0), -1);
	EXPECT_EQ(cstr_cmpn(NULL, 0, "abc", 3, 0), -1);
	END;
}

TEST(t_cstr_cmp_null)
{
	START;
	EXPECT_EQ(cstr_cmp(NULL, 0, NULL, 0), 0);
	EXPECT_EQ(cstr_cmp("abc", 0, NULL, 0), 1);
	EXPECT_EQ(cstr_cmp("abc", 3, NULL, 0), 1);
	EXPECT_EQ(cstr_cmp(NULL, 0, "abc", 0), -1);
	EXPECT_EQ(cstr_cmp(NULL, 0, "abc", 3), -1);
	END;
}

TEST(t_cstr_eq_null)
{
	START;
	EXPECT_EQ(cstr_eq(NULL, 0, NULL, 0), 1);
	EXPECT_EQ(cstr_eq("abc", 0, NULL, 0), 0);
	EXPECT_EQ(cstr_eq("abc", 3, NULL, 0), 0);
	EXPECT_EQ(cstr_eq(NULL, 0, "abc", 0), 0);
	EXPECT_EQ(cstr_eq(NULL, 0, "abc", 3), 0);
	END;
}

TEST(t_cstr_chr_null)
{
	START;
	EXPECT_EQ(cstr_chr(NULL, 0), NULL);
	END;
}

TEST(t_cstr_rchr_null)
{
	START;
	EXPECT_EQ(cstr_rchr(NULL, 0), NULL);
	END;
}

TEST(t_cstr_cstr_null)
{
	START;
	EXPECT_EQ(cstr_cstr(NULL, NULL), NULL);
	EXPECT_EQ(cstr_cstr("", NULL), NULL);
	EXPECT_EQ(cstr_cstr(NULL, ""), NULL);
	END;
}

TEST(t_cstr_cpy_null)
{
	START;
	EXPECT_EQ(cstr_cpy(NULL, 0, NULL, 0), NULL);
	EXPECT_EQ(cstr_cpy("", 0, NULL, 0), NULL);
	EXPECT_EQ(cstr_cpy(NULL, 0, "", 0), NULL);
	END;
}

TEST(t_cstr_replace_null)
{
	START;

	char buf[64] = { 0 };

	EXPECT_EQ(cstr_replace(NULL, 0, 0, NULL, 0, NULL, 0, NULL), 0);
	EXPECT_EQ(cstr_replace(buf, 64, 4, NULL, 0, NULL, 0, NULL), 4);
	END;
}

TEST(t_cstr_replaces_null)
{
	START;

	char buf[64] = { 0 };

	EXPECT_EQ(cstr_replaces(NULL, 0, 0, NULL, NULL, 0, NULL), 0);
	EXPECT_EQ(cstr_replaces(buf, 64, 4, NULL, NULL, 0, NULL), 4);
	END;
}

TEST(t_cstr_rreplaces_null)
{
	START;

	char buf[64] = { 0 };

	EXPECT_EQ(cstr_rreplaces(NULL, 0, 0, NULL, NULL, 0), 0);
	EXPECT_EQ(cstr_rreplaces(buf, 64, 4, NULL, NULL, 0), 4);
	END;
}

TEST(t_cstr_null)
{
	SSTART;
	RUN(t_cstrf_null);
	RUN(t_cstr_cat_null);
	RUN(t_cstr_len_null);
	RUN(t_cstr_cmpn_null);
	RUN(t_cstr_cmp_null);
	RUN(t_cstr_eq_null);
	RUN(t_cstr_chr_null);
	RUN(t_cstr_rchr_null);
	RUN(t_cstr_cstr_null);
	RUN(t_cstr_cpy_null);
	RUN(t_cstr_replace_null);
	RUN(t_cstr_replaces_null);
	RUN(t_cstr_rreplaces_null);
	SEND;
}

TEST(t_cstrf)
{
	START;

	char buf[16] = { 0 };
	size_t len   = cstrf(buf, sizeof(buf), "%s", "a");

	EXPECT_STR(buf, "a");

	END;
}

TEST(t_cstr_catn)
{
	START;

	char buf[16] = "abc";
	size_t len   = cstr_cat(buf, sizeof(buf), 3, "def", 2);

	EXPECT_STR(buf, "abcde");
	EXPECT_EQ(len, 5);

	END;
}

TEST(t_cstr_cat)
{
	START;

	char buf[16] = "abc";
	size_t len   = cstr_cat(buf, sizeof(buf), 3, "def", 3);

	EXPECT_STR(buf, "abcdef");
	EXPECT_EQ(len, 6);

	END;
}

TEST(t_cstr_len)
{
	START;

	size_t len = cstr_len("abc");

	EXPECT_EQ(len, 3);

	END;
}

TEST(t_cstr_cmpn)
{
	START;
	EXPECT_EQ(cstr_cmpn("a", 1, "a", 1, 1), 0);
	EXPECT_GT(cstr_cmpn("b", 1, "a", 1, 1), 0);
	EXPECT_LT(cstr_cmpn("a", 1, "b", 1, 1), 0);
	EXPECT_GT(cstr_cmpn("a", 1, "b", 0, 1), 0);
	EXPECT_LT(cstr_cmpn("ac", 2, "b", 1, 2), 0);
	EXPECT_LT(cstr_cmpn("ac", 2, "bcd", 3, 2), 0);
	EXPECT_EQ(cstr_cmpn("a", 1, "b", 0, 0), 0);
	EXPECT_EQ(cstr_cmpn("a", 1, "b", 1, 0), 0);
	EXPECT_EQ(cstr_cmpn("b", 1, "a", 1, 0), 0);
	END;
}

TEST(t_cstr_cmp)
{
	START;
	EXPECT_EQ(cstr_cmp("a", 1, "a", 1), 0);
	EXPECT_GT(cstr_cmp("b", 1, "a", 1), 0);
	EXPECT_LT(cstr_cmp("a", 1, "b", 1), 0);
	EXPECT_GT(cstr_cmp("a", 1, "b", 0), 0);
	EXPECT_LT(cstr_cmp("ac", 2, "b", 1), 0);
	EXPECT_LT(cstr_cmp("ac", 2, "bcd", 3), 0);
	END;
}

TEST(t_cstr_eq)
{
	START;
	EXPECT_EQ(cstr_eq("a", 1, "a", 1), 1);
	END;
}

TEST(t_cstr_cpy)
{
	START;

	char cstr[16] = { 0 };
	cstr_cpy(cstr, sizeof(cstr), "abcd", 4);

	EXPECT_STR(cstr, "abcd");

	END;
}

TEST(t_cstr_chr)
{
	START;

	const char *cstr = "abcbd";
	EXPECT_EQ(cstr_chr(cstr, 'b'), cstr + 1);

	END;
}

TEST(t_cstr_rchr)
{
	START;

	const char *cstr = "abcbd";
	EXPECT_EQ(cstr_rchr(cstr, 'b'), cstr + 3);

	END;
}

TEST(t_cstr_cstr)
{
	START;

	const char *cstr = "abcbd";
	EXPECT_EQ(cstr_cstr(cstr, "b"), cstr + 1);

	END;
}

TEST(t_cstr_replace_short)
{
	START;

	char cstr[32] = "ab<char>de";

	size_t len = cstr_replace(cstr, 32, 10, "<char>", 6, "c", 1, NULL);

	EXPECT_EQ(len, 5);
	EXPECT_STR(cstr, "abcde");

	END;
}

TEST(t_cstr_replace_long)
{
	START;

	char cstr[32] = "ab<c>op";

	size_t len = cstr_replace(cstr, 32, 7, "<c>", 3, "cdefghijklm", 11, NULL);

	EXPECT_EQ(len, 15);
	EXPECT_STR(cstr, "abcdefghijklmop");

	END;
}

TEST(t_cstr_replace_short_multi)
{
	START;

	char cstr[32] = "<char>ab<char>de<char>";

	size_t len = cstr_replace(cstr, 32, 22, "<char>", 6, "c", 1, NULL);

	EXPECT_EQ(len, 7);
	EXPECT_STR(cstr, "cabcdec");

	END;
}

TEST(t_cstr_replace_long_multi)
{
	START;

	char cstr[64] = "<c>ab<c>op<c>";

	size_t len = cstr_replace(cstr, 64, 13, "<c>", 3, "cdefghijklm", 11, NULL);

	EXPECT_EQ(len, 37);
	EXPECT_STR(cstr, "cdefghijklmabcdefghijklmopcdefghijklm");

	END;
}

TEST(t_cstr_replace_overflow)
{
	START;

	char cstr[12] = "ab<c>de";

	size_t len = cstr_replace(cstr, 12, 7, "<c>", 3, "cdefghijklm", 11, NULL);

	EXPECT_EQ(len, 0);
	EXPECT_STR(cstr, "ab<c>de");

	END;
}

TEST(t_cstr_replaces)
{
	START;

	char cstr[32] = "ab<char>d<none><ignore>e<str>";

	const char *from[] = {
		"<char>",
		"<ignore>",
		"<none>",
		"<str>",
	};

	const char *to[] = {
		"c",
		NULL,
		"",
		"string",
	};

	size_t len = cstr_replaces(cstr, 32, 29, from, to, 4, NULL);

	EXPECT_EQ(len, 19);
	EXPECT_STR(cstr, "abcd<ignore>estring");

	END;
}

TEST(t_cstr_rreplaces)
{
	START;

	char cstr[32] = "<string> world";

	const char *from[] = {
		"<word>",
		"<string>",
	};

	const char *to[] = {
		"hello",
		"string:<word>",
	};

	size_t len = cstr_rreplaces(cstr, 32, 14, from, to, 2);

	EXPECT_EQ(len, 18);
	EXPECT_STR(cstr, "string:hello world");

	END;
}

STEST(t_cstr_replace)
{
	SSTART;
	RUN(t_cstr_replace_short);
	RUN(t_cstr_replace_long);
	RUN(t_cstr_replace_short_multi);
	RUN(t_cstr_replace_long_multi);
	RUN(t_cstr_replace_overflow);
	RUN(t_cstr_replaces);
	RUN(t_cstr_rreplaces);
	SEND;
}

STEST(t_cstr)
{
	SSTART;
	RUN(t_cstr_null);
	RUN(t_cstrf);
	RUN(t_cstr_catn);
	RUN(t_cstr_cat);
	RUN(t_cstr_len);
	RUN(t_cstr_cmpn);
	RUN(t_cstr_cmp);
	RUN(t_cstr_eq);
	RUN(t_cstr_cpy);
	RUN(t_cstr_chr);
	RUN(t_cstr_rchr);
	RUN(t_cstr_cstr);
	RUN(t_cstr_replace);
	SEND;
}
