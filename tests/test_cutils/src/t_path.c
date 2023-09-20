#include "t_path.h"

#include "cstr.h"
#include "path.h"

#include "test.h"

TEST(t_path_init)
{
	START;

	path_t path = { 0 };

	EXPECT_EQ(path_init(NULL, NULL, 0), NULL);
	EXPECT_EQ(path_init(&path, NULL, 0), NULL);
	EXPECT_EQ(path_init(&path, "", 1), &path);

	END;
}

TEST(t_path_child_s)
{
	START;

	path_t path = { 0 };
	path_init(&path, CSTR("a"));

	EXPECT_EQ(path_child_s(NULL, NULL, 0, 0), NULL);
	EXPECT_EQ(path_child_s(&path, NULL, 0, 0), &path);
	EXPECT_EQ(path_child_s(&path, CSTR("b"), '/'), &path);

	EXPECT_EQ(path.len, 3);
	EXPECT_STR(path.path, "a/b");

	END;
}

TEST(t_path_child)
{
	START;

	path_t path = { 0 };
	path_init(&path, CSTR("a"));

	EXPECT_EQ(path_child(NULL, NULL, 0), NULL);
	EXPECT_EQ(path_child(&path, NULL, 0), &path);
	EXPECT_EQ(path_child(&path, CSTR("b")), &path);

	EXPECT_EQ(path.len, 3);

	END;
}

TEST(t_path_parent)
{
	START;

	path_t path = { 0 };
	path_init(&path, CSTR("a/b\\c"));

	EXPECT_EQ(path_parent(NULL), NULL);
	EXPECT_EQ(path_parent(&path), &path);
	EXPECT_STR(path.path, "a/b");
	EXPECT_EQ(path_parent(&path), &path);
	EXPECT_STR(path.path, "a");
	EXPECT_EQ(path_parent(&path), NULL);

	END;
}

TEST(t_path_set_len)
{
	START;

	path_t path = { 0 };
	path_init(&path, CSTR("a"));

	EXPECT_EQ(path_set_len(NULL, 0), NULL);
	EXPECT_EQ(path_set_len(&path, 0), &path);

	EXPECT_EQ(path.len, 0);
	EXPECT_STR(path.path, "");

	END;
}

TEST(t_path_ends)
{
	START;

	path_t path = { 0 };
	path_init(&path, CSTR("a/b/c"));

	EXPECT_EQ(path_ends(NULL, NULL, 0), 0);
	EXPECT_EQ(path_ends(&path, NULL, 0), 1);
	EXPECT_EQ(path_ends(&path, CSTR("c")), 1);

	END;
}

TEST(t_path_calc_rel)
{
	START;

	path_t path = { 0 };

	EXPECT_EQ(path_calc_rel(NULL, 0, NULL, 0, NULL), 1);
	EXPECT_EQ(path_calc_rel(CSTR("a/b/c/"), NULL, 0, NULL), 1);
	EXPECT_EQ(path_calc_rel(CSTR("a/b/c/"), CSTR("a/b/c/d/"), NULL), 1);
	EXPECT_EQ(path_calc_rel(CSTR("a/b/c/"), CSTR("a/b/c/d/"), &path), 0);
	EXPECT_STR(path.path, "/d/");
	EXPECT_EQ(path_calc_rel(CSTR("a/b/c"), CSTR("a/d/e/f"), &path), 0);
	EXPECT_STR(path.path, "../../d/e/f");

	END;
}

TEST(t_pathv_path)
{
	START;

	path_t path = { 0 };
	path_init(&path, CSTR("a/b/c"));

	pathv_t pathv = { 0 };

	EXPECT_EQ(pathv_path(NULL, NULL), NULL);
	EXPECT_EQ(pathv_path(&pathv, NULL), NULL);
	EXPECT_EQ(pathv_path(&pathv, &path), &pathv);
	EXPECT_EQ(pathv.len, path.len);
	EXPECT_EQ(pathv.path, path.path);

	END;
}

TEST(t_pathv_sub)
{
	START;

	path_t l = { 0 };
	path_init(&l, CSTR("a/b/c"));
	path_t r = { 0 };
	path_init(&r, CSTR("a"));

	pathv_t pathv = { 0 };

	EXPECT_EQ(pathv_sub(NULL, NULL, NULL), NULL);
	EXPECT_EQ(pathv_sub(&pathv, NULL, NULL), NULL);
	EXPECT_EQ(pathv_sub(&pathv, &l, NULL), NULL);
	EXPECT_EQ(pathv_sub(&pathv, &l, &r), &pathv);

	EXPECT_STR(pathv.path, "b/c");

	END;
}

TEST(t_pathv_folder)
{
	START;

	path_t path = { 0 };
	path_init(&path, CSTR("a/b/c"));

	pathv_t pathv = { 0 };

	EXPECT_EQ(pathv_folder(NULL, NULL), NULL);
	EXPECT_EQ(pathv_folder(&pathv, NULL), NULL);
	EXPECT_EQ(pathv_folder(&pathv, &path), &pathv);
	EXPECT_STR(pathv.path, "c");

	END;
}

STEST(t_path)
{
	SSTART;
	RUN(t_path_init);
	RUN(t_path_child_s);
	RUN(t_path_child);
	RUN(t_path_parent);
	RUN(t_path_set_len);
	RUN(t_path_ends);
	RUN(t_path_calc_rel);
	RUN(t_pathv_path);
	RUN(t_pathv_sub);
	RUN(t_pathv_folder);
	SEND;
}
