#include "t_str.h"

#include "str.h"

#include "test.h"

TEST(t_str_cstr_null)
{
	START;

	str_cstr(NULL, 0);

	END;
}

TEST(t_str_cstrn_null)
{
	START;

	str_cstrn(NULL, 0, 0);

	END;
}

TEST(t_str_cstrv_null)
{
	START;

	str_cstrf(NULL);

	END;
}

TEST(t_str_buf_null)
{
	START;

	str_buf(NULL, 0, 0);

	END;
}

TEST(t_str_free_null)
{
	START;

	str_free(NULL);

	END;
}

TEST(t_str_catc_null)
{
	START;

	str_t str = { 0 };

	EXPECT_EQ(str_catc(NULL, "", 0), NULL);
	EXPECT_EQ(str_catc(&str, NULL, 0), NULL);

	END;
}

TEST(t_str_catn_null)
{
	START;

	str_t str = { 0 };

	EXPECT_EQ(str_catn(NULL, str, 0), NULL);
	EXPECT_EQ(str_catn(&str, str, 0), NULL);

	END;
}

TEST(t_str_cat_null)
{
	START;

	str_t str = { 0 };

	EXPECT_EQ(str_cat(NULL, str), NULL);
	EXPECT_EQ(str_cat(&str, str), NULL);

	END;
}

TEST(t_str_split_null)
{
	START;

	str_t str = str_ref();

	EXPECT_EQ(str_split(str, 0, NULL, NULL), 1);
	EXPECT_EQ(str_split(str, 0, &str, NULL), 1);
	EXPECT_EQ(str_split(str, 0, NULL, &str), 1);

	END;
}

TEST(t_str_rsplit_null)
{
	START;

	str_t str = str_ref();

	EXPECT_EQ(str_split(str, 0, NULL, NULL), 1);
	EXPECT_EQ(str_split(str, 0, &str, NULL), 1);
	EXPECT_EQ(str_split(str, 0, NULL, &str), 1);

	END;
}

TEST(t_str_null)
{
	SSTART;
	RUN(t_str_cstr_null);
	RUN(t_str_cstrn_null);
	RUN(t_str_cstrv_null);
	RUN(t_str_buf_null);
	RUN(t_str_free_null);
	RUN(t_str_catc_null);
	RUN(t_str_catn_null);
	RUN(t_str_cat_null);
	RUN(t_str_split_null);
	RUN(t_str_rsplit_null);
	SEND;
}

TEST(t_str_init)
{
	START;

	str_t str = str_init(16);

	EXPECT_STR(str.data, "");
	EXPECT_EQ(str.size, 16);
	EXPECT_EQ(str.len, 0);
	EXPECT_EQ(str.ref, 0);

	str_free(&str);

	EXPECT_STR(str.data, NULL);
	EXPECT_EQ(str.size, 0);
	EXPECT_EQ(str.len, 0);
	EXPECT_EQ(str.ref, 0);

	END;
}

TEST(t_str_cstrn)
{
	START;

	str_t str = str_cstrn("abc", 2, 16);

	EXPECT_STR(str.data, "ab");
	EXPECT_EQ(str.size, 16);
	EXPECT_EQ(str.len, 2);
	EXPECT_EQ(str.ref, 0);

	str_free(&str);

	END;
}

TEST(t_str_cstrf)
{
	START;

	str_t str = str_cstrf("%s", "a");

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

TEST(t_str_buf)
{
	START;

	char buf[16] = "abc";
	str_t str    = str_buf(buf, sizeof(buf), 3);

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

TEST(t_str_ref)
{
	START;

	str_t str = str_ref();

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

TEST(t_str_create)
{
	SSTART;
	RUN(t_str_init);
	RUN(t_str_cstrn);
	RUN(t_str_cstrf);
	RUN(t_str_buf);
	RUN(t_str_ref);
	SEND;
}

TEST(t_str_catc)
{
	START;

	str_t str = str_cstrn("abc", 3, 16);

	str_catc(&str, "def", 2);

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

	str_t str = str_cstrn("abc", 3, 16);
	str_t src = str_cstrn("def", 3, 16);

	str_catn(&str, src, 2);

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

	str_t str = str_cstrn("abc", 3, 16);
	str_t src = str_cstrn("def", 3, 16);

	str_cat(&str, src);

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

	str_t str = str_cstrn("abc", 3, 16);

	EXPECT_EQ(str_cmpnc(str, "abc", 3, 3), 0);

	str_free(&str);

	END;
}

TEST(t_str_cmpc)
{
	START;

	str_t str = str_cstrn("abc", 3, 16);

	EXPECT_EQ(str_cmpc(str, "abc", 3), 0);

	str_free(&str);

	END;
}

TEST(t_str_cmpn)
{
	START;

	str_t str = str_cstrn("abc", 3, 16);
	str_t src = str_cstrn("abc", 3, 16);

	EXPECT_EQ(str_cmpn(str, src, 3), 0);

	str_free(&str);
	str_free(&src);

	END;
}

TEST(t_str_cmp)
{
	START;

	str_t str = str_cstrn("abc", 3, 16);
	str_t src = str_cstrn("abc", 3, 16);

	EXPECT_EQ(str_cmp(str, src), 0);

	str_free(&str);
	str_free(&src);

	END;
}

TEST(t_str_eqc)
{
	START;

	str_t str = str_cstrn("abc", 3, 16);

	EXPECT_EQ(str_eqc(str, "abc", 3), 1);

	str_free(&str);

	END;
}

TEST(t_str_eq)
{
	START;

	str_t str = str_cstrn("abc", 3, 16);
	str_t src = str_cstrn("abc", 3, 16);

	EXPECT_EQ(str_eq(str, src), 1);

	str_free(&str);
	str_free(&src);

	END;
}

TEST(t_str_cpy)
{
	START;

	str_t src = str_cstr("abc", 3);

	str_t str = str_cpy(src);

	EXPECT_STR(str.data, "abc");
	EXPECT_EQ(str.size, 4);
	EXPECT_EQ(str.len, 3);
	EXPECT_EQ(str.ref, 0);

	str_free(&str);

	END;
}

TEST(t_str_split_ref)
{
	START;

	str_t str = str_cstr("abc defgh ijkl", 14);
	str_t l	  = str_ref();
	str_t r	  = str_ref();

	str_split(str, ' ', &l, &r);

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

	str_t str = str_cstr("abc defgh ijkl", 14);
	str_t l	  = str_buf(lbuf, sizeof(lbuf), 6);
	str_t r	  = str_buf(rbuf, sizeof(rbuf), 6);

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

	str_t str = str_cstr("abc defgh ijkl", 14);
	str_t l	  = str_init(18);
	str_t r	  = str_init(18);

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

	str_t str = str_cstr("abc defgh ijkl", 14);
	str_t l	  = str_ref();
	str_t r	  = str_ref();

	str_rsplit(str, ' ', &l, &r);

	EXPECT_STRN(l.data, "abc defgh", 9);
	EXPECT_EQ(l.len, 9);
	EXPECT_STR(r.data, "ijkl");
	EXPECT_EQ(r.len, 4);

	str_free(&str);
	str_free(&l);
	str_free(&r);

	END;
}

STEST(t_str)
{
	SSTART;
	RUN(t_str_null);
	RUN(t_str_create);
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
	RUN(t_str_split_ref);
	RUN(t_str_split_buf);
	RUN(t_str_split_own);
	RUN(t_str_rsplit);
	SEND;
}
