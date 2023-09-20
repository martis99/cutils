#include "t_str.h"

#include "str.h"

#include "test.h"

TEST(t_str_null)
{
	START;

	EXPECT_EQ(str_null().data, NULL);

	END;
}

TEST(t_strz)
{
	START;

	strz(0);
	str_t str = strz(1);

	str_free(&str);

	END;
}

TEST(t_strc)
{
	START;

	strc(NULL, 0);

	END;
}

TEST(t_strn)
{
	START;

	strn(NULL, 0, 0);

	strn("abc", 3, 1);
	str_t str = strn("abc", 2, 16);

	EXPECT_STR(str.data, "ab");
	EXPECT_EQ(str.size, 16);
	EXPECT_EQ(str.len, 2);
	EXPECT_EQ(str.ref, 0);

	str_free(&str);

	END;
}

TEST(t_strf)
{
	START;

	strf(NULL);

	str_t str = strf("%s", "a");

	EXPECT_STR(str.data, "a");
	EXPECT_EQ(str.size, 2);
	EXPECT_EQ(str.len, 1);
	EXPECT_EQ(str.ref, 0);

	str_free(&str);

	EXPECT_STR(str.data, NULL);
	EXPECT_EQ(str.size, 0);
	EXPECT_EQ(str.len, 0);
	EXPECT_EQ(str.ref, 0);

	END;
}

TEST(t_strb)
{
	START;

	char buf[16] = "abc";
	str_t str    = strb(buf, sizeof(buf), 3);

	EXPECT_STR(str.data, "abc");
	EXPECT_EQ(str.size, 16);
	EXPECT_EQ(str.len, 3);
	EXPECT_EQ(str.ref, 1);

	str_free(&str);

	EXPECT_STR(str.data, NULL);
	EXPECT_EQ(str.size, 0);
	EXPECT_EQ(str.len, 0);
	EXPECT_EQ(str.ref, 1);

	END;
}

TEST(t_strr)
{
	START;

	str_t str = strr();

	EXPECT_STR(str.data, NULL);
	EXPECT_EQ(str.size, 0);
	EXPECT_EQ(str.len, 0);
	EXPECT_EQ(str.ref, 1);

	str_free(&str);

	EXPECT_STR(str.data, NULL);
	EXPECT_EQ(str.size, 0);
	EXPECT_EQ(str.len, 0);
	EXPECT_EQ(str.ref, 1);

	END;
}

TEST(t_str_free)
{
	START;

	str_t str = strz(16);

	EXPECT_STR(str.data, "");
	EXPECT_EQ(str.size, 16);
	EXPECT_EQ(str.len, 0);
	EXPECT_EQ(str.ref, 0);

	str_free(NULL);
	str_free(&str);

	EXPECT_STR(str.data, NULL);
	EXPECT_EQ(str.size, 0);
	EXPECT_EQ(str.len, 0);
	EXPECT_EQ(str.ref, 0);

	END;
}

TEST(t_str_zero)
{
	START;

	str_t str = strz(16);

	str_zero(NULL);
	str_zero(&str);

	str_free(&str);

	END;
}

TEST(t_str_resize)
{
	START;

	str_t str = strz(16);

	EXPECT_EQ(str_resize(NULL, 0), 1);
	EXPECT_EQ(str_resize(&str, 8), 0);
	EXPECT_EQ(str_resize(&str, 32), 0);

	str_free(&str);

	END;
}

TEST(t_str_catc)
{
	START;

	str_t ref = strc("abc", 3);
	str_t str = strn("abc", 3, 16);

	EXPECT_EQ(str_catc(NULL, "", 0), NULL);
	EXPECT_EQ(str_catc(&str, NULL, 0), NULL);
	EXPECT_EQ(str_catc(&ref, "def", 2), NULL);
	EXPECT_EQ(str_catc(&str, "def", 2), &str);

	EXPECT_STR(str.data, "abcde");
	EXPECT_EQ(str.size, 16);
	EXPECT_EQ(str.len, 5);
	EXPECT_EQ(str.ref, 0);

	str_free(&str);

	END;
}

TEST(t_str_catn)
{
	START;

	str_t str = strn("abc", 3, 16);
	str_t src = strn("def", 3, 16);

	EXPECT_EQ(str_catn(NULL, str, 0), NULL);
	EXPECT_EQ(str_catn(&str, str, 0), &str);
	EXPECT_EQ(str_catn(&str, src, 2), &str);

	EXPECT_STR(str.data, "abcde");
	EXPECT_EQ(str.size, 16);
	EXPECT_EQ(str.len, 5);
	EXPECT_EQ(str.ref, 0);

	str_free(&str);
	str_free(&src);

	END;
}

TEST(t_str_cat)
{
	START;

	str_t str = strn("abc", 3, 16);
	str_t src = strn("def", 3, 16);

	EXPECT_EQ(str_cat(NULL, str), NULL);
	EXPECT_EQ(str_cat(&str, src), &str);

	EXPECT_STR(str.data, "abcdef");
	EXPECT_EQ(str.size, 16);
	EXPECT_EQ(str.len, 6);
	EXPECT_EQ(str.ref, 0);

	str_free(&str);
	str_free(&src);

	END;
}

TEST(t_str_cmpnc)
{
	START;

	str_t str = strn("abc", 3, 16);

	EXPECT_EQ(str_cmpnc(str, NULL, 0, 0), 1);
	EXPECT_EQ(str_cmpnc(str, "abc", 3, 3), 0);

	str_free(&str);

	END;
}

TEST(t_str_cmpc)
{
	START;

	str_t str = strn("abc", 3, 16);

	EXPECT_EQ(str_cmpc(str, NULL, 0), 1);
	EXPECT_EQ(str_cmpc(str, "abc", 3), 0);

	str_free(&str);

	END;
}

TEST(t_str_cmpn)
{
	START;

	str_t str = strn("abc", 3, 16);
	str_t src = strn("abc", 3, 16);

	EXPECT_EQ(str_cmpn(str, src, 0), 0);
	EXPECT_EQ(str_cmpn(str, src, 3), 0);

	str_free(&str);
	str_free(&src);

	END;
}

TEST(t_str_cmp)
{
	START;

	str_t str = strn("abc", 3, 16);
	str_t src = strn("abc", 3, 16);

	EXPECT_EQ(str_cmp(str, src), 0);

	str_free(&str);
	str_free(&src);

	END;
}

TEST(t_str_eqc)
{
	START;

	str_t str = strn("abc", 3, 16);

	EXPECT_EQ(str_eqc(str, NULL, 0), 0);
	EXPECT_EQ(str_eqc(str, "abc", 3), 1);

	str_free(&str);

	END;
}

TEST(t_str_eq)
{
	START;

	str_t str = strn("abc", 3, 16);
	str_t src = strn("abc", 3, 16);

	EXPECT_EQ(str_eq(str, src), 1);

	str_free(&str);
	str_free(&src);

	END;
}

TEST(t_str_cpy)
{
	START;

	str_t src = strc("abc", 3);

	str_t str = str_cpy(src);

	EXPECT_STR(str.data, "abc");
	EXPECT_EQ(str.size, 4);
	EXPECT_EQ(str.len, 3);
	EXPECT_EQ(str.ref, 0);

	str_free(&str);

	END;
}

TEST(t_str_split)
{
	START;

	str_t str = strn("abc", 3, 4);
	str_t ref = strc("abc", 3);

	EXPECT_EQ(str_split(str, 0, NULL, NULL), 1);
	EXPECT_EQ(str_split(str, 0, &str, NULL), 1);
	EXPECT_EQ(str_split(str, 'b', &ref, NULL), 1);
	EXPECT_EQ(str_split(str, 0, NULL, &str), 1);
	EXPECT_EQ(str_split(str, 'b', NULL, &ref), 1);

	END;
}

TEST(t_str_split_ref)
{
	START;

	str_t str = strc("abc defgh ijkl", 14);
	str_t l	  = strr();
	str_t r	  = strr();

	EXPECT_EQ(str_split(str, ' ', &l, &r), 0);

	EXPECT_STRN(l.data, "abc", 3);
	EXPECT_EQ(l.len, 3);
	EXPECT_STR(r.data, "defgh ijkl");
	EXPECT_EQ(r.len, 10);

	END;
}

TEST(t_str_split_buf)
{
	START;

	char lbuf[18] = { "buf1: " };
	char rbuf[18] = { "buf2: " };

	str_t str = strc("abc defgh ijkl", 14);
	str_t l	  = strb(lbuf, sizeof(lbuf), 6);
	str_t r	  = strb(rbuf, sizeof(rbuf), 6);

	str_split(str, ' ', &l, &r);

	EXPECT_STRN(l.data, "buf1: abc", 9);
	EXPECT_EQ(l.len, 9);
	EXPECT_STR(r.data, "buf2: defgh ijkl");
	EXPECT_EQ(r.len, 16);

	END;
}

TEST(t_str_split_own)
{
	START;

	str_t str = strc("abc defgh ijkl", 14);
	str_t l	  = strz(18);
	str_t r	  = strz(18);

	str_split(str, ' ', &l, &r);

	EXPECT_STRN(l.data, "abc", 3);
	EXPECT_EQ(l.len, 3);
	EXPECT_STR(r.data, "defgh ijkl");
	EXPECT_EQ(r.len, 10);

	str_free(&str);
	str_free(&l);
	str_free(&r);

	END;
}

TEST(t_str_rsplit)
{
	START;

	str_t str = strc("abc defgh ijkl", 14);
	str_t ref = strc("abc", 3);
	str_t l	  = strr();
	str_t r	  = strr();

	EXPECT_EQ(str_rsplit(str, 0, NULL, NULL), 1);
	EXPECT_EQ(str_rsplit(str, 0, &str, NULL), 1);
	EXPECT_EQ(str_rsplit(str, ' ', &ref, NULL), 1);
	EXPECT_EQ(str_rsplit(str, 0, NULL, &str), 1);
	EXPECT_EQ(str_rsplit(str, ' ', NULL, &ref), 1);
	EXPECT_EQ(str_rsplit(str, ' ', &l, &r), 0);

	EXPECT_STRN(l.data, "abc defgh", 9);
	EXPECT_EQ(l.len, 9);
	EXPECT_STR(r.data, "ijkl");
	EXPECT_EQ(r.len, 4);

	str_free(&str);
	str_free(&l);
	str_free(&r);

	END;
}

TEST(t_str_replace)
{
	START;

	char buf[32] = "ab<char>de";

	str_t str = strb(buf, 32, 10);

	EXPECT_EQ(str_replace(NULL, str, str), 0);
	EXPECT_EQ(str_replace(&str, STR("<char>"), STR("c")), 1);

	EXPECT_STR(str.data, "abcde");

	END;
}

TEST(t_str_replaces)
{
	START;

	char buf[32] = "ab<char>d<none><ignore>e<str>";

	str_t str = strb(buf, 32, 29);

	EXPECT_EQ(str_replaces(NULL, NULL, NULL, 0), 0);
	EXPECT_EQ(str_replaces(&str, NULL, NULL, 0), 0);

	const str_t from[] = {
		STR("<char>"),
		STR("<ignore>"),
		STR("<none>"),
		STR("<str>"),
	};

	const str_t to[] = {
		STR("c"),
		strb(NULL, 0, 0),
		STR(""),
		STR("string"),
	};

	EXPECT_EQ(str_replaces(&str, from, to, 4), 1);

	EXPECT_STR(str.data, "abcd<ignore>estring");

	END;
}

TEST(t_str_rreplaces)
{
	START;

	char buf[32] = "<string> world";

	str_t str = strb(buf, 32, 14);

	EXPECT_EQ(str_rreplaces(NULL, NULL, NULL, 0), 0);
	EXPECT_EQ(str_rreplaces(&str, NULL, NULL, 0), 0);

	const str_t from[] = {
		STR("<word>"),
		STR("<string>"),
	};

	const str_t to[] = {
		STR("hello"),
		STR("string:<word>"),
	};

	EXPECT_EQ(str_rreplaces(&str, from, to, 2), 1);
	EXPECT_STR(str.data, "string:hello world");

	END;
}

STEST(t_str)
{
	SSTART;
	RUN(t_str_null);
	RUN(t_strz);
	RUN(t_strc);
	RUN(t_strn);
	RUN(t_strf);
	RUN(t_strb);
	RUN(t_strr);
	RUN(t_str_free);
	RUN(t_str_zero);
	RUN(t_str_resize);
	RUN(t_str_catc);
	RUN(t_str_catn);
	RUN(t_str_cat);
	RUN(t_str_cmpnc);
	RUN(t_str_cmpc);
	RUN(t_str_cmpn);
	RUN(t_str_cmp);
	RUN(t_str_eqc);
	RUN(t_str_eq);
	RUN(t_str_cpy);
	RUN(t_str_split);
	RUN(t_str_split_ref);
	RUN(t_str_split_buf);
	RUN(t_str_split_own);
	RUN(t_str_rsplit);
	RUN(t_str_replace);
	RUN(t_str_replaces);
	RUN(t_str_rreplaces);
	SEND;
}
