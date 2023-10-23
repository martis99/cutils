#include "t_cutils_c.h"

#include "args.h"
#include "file.h"

#define TEST_FILE "t_args.txt"

#define NAME "test_cutils"
#define DESC "cutils tests"

TEST(t_args_usage, FILE *file)
{
	START;

	{
		file_reopen(TEST_FILE, "wb+", file);
		args_usage(NAME, DESC, file);

		char buf[128] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "cutils tests\n"
				   "\n"
				   "Usage\n"
				   "  test_cutils [options]\n"
				   "\n"
				   "Run 'test_cutils --help' for more information\n";
		EXPECT_STR(buf, exp);
	}

	END;
}

static int handle_dir(const char *param, void *ret)
{
	return 0;
}

TEST(t_args_handle, FILE *file)
{
	START;

	arg_t args[] = {
		[0] = ARG('T', "test", PARAM_NONE, "", "Run tests", NULL),
		[1] = ARG('B', "dir", PARAM_STR, "<dir>", "Set directory", handle_dir),
		[2] = ARG('S', "dir", PARAM_STR, "<dir>", "Set directory", NULL),
		[3] = ARG('D', "debug", PARAM_SWITCH, "<0/1>", "Turn on/off debug messages (default: 0)", NULL),
		[4] = ARG('M', "mode", PARAM_MODE, "<mode>", "Set mode (default: A)", NULL),
	};

	mode_t mode_modes[] = {
		[0] = { .c = 'A', .desc = "A mode" },
		[1] = { .c = 'B', .desc = "B mode" },
	};

	mode_desc_t modes[] = {
		{ .name = "Modes", .modes = mode_modes, .len = 2 },
	};

	char *dir = NULL;
	int debug = 0;

	void *params[] = {
		[2] = &dir,
		[3] = &debug,
	};

	const char *argv[] = {
		NAME, "-T", "-B", "./", "-S", "./", "-M", "B", "--debug", "1",
	};

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(args_handle(NAME, DESC, args, sizeof(args), modes, sizeof(modes), sizeof(argv) / sizeof(char *), argv, params, file), 0);

		char buf[64] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "";
		EXPECT_STR(buf, exp);
	}

	EXPECT_EQ(debug, 1);

	const char *argv_err[] = {
		NAME, "-D", "0", "-D", "2", "-G", "1",
	};

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(args_handle(NAME, DESC, args, sizeof(args), modes, sizeof(modes), sizeof(argv_err) / sizeof(char *), argv_err, params, file), 1);

		char buf[128] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "cutils tests\n"
				   "\n"
				   "Usage\n"
				   "  test_cutils [options]\n"
				   "\n"
				   "Run 'test_cutils --help' for more information\n";
		EXPECT_STR(buf, exp);
	}

	const char *argv_help[] = {
		NAME,
		"--help",
	};

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(args_handle(NAME, DESC, args, sizeof(args), modes, sizeof(modes), sizeof(argv_help) / sizeof(char *), argv_help, params, file), 2);

		char buf[512] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "cutils tests\n"
				   "\n"
				   "Usage\n"
				   "  test_cutils [options]\n"
				   "\n"
				   "Options\n"
				   "  -T --test                    Run tests\n"
				   "  -B --dir          <dir>      Set directory\n"
				   "  -S --dir          <dir>      Set directory\n"
				   "  -D --debug        <0/1>      Turn on/off debug messages (default: 0)\n"
				   "  -M --mode         <mode>     Set mode (default: A)\n"
				   "\n"
				   "Modes\n"
				   "  A = A mode\n"
				   "  B = B mode\n"
				   "\n";
		EXPECT_STR(buf, exp);
	}

	const char *argv_d[] = {
		NAME,
		"-D",
	};

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(args_handle(NAME, DESC, args, sizeof(args), modes, sizeof(modes), sizeof(argv_d) / sizeof(char *), argv_d, params, file), 2);

		char buf[512] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "cutils tests\n"
				   "\n"
				   "Usage\n"
				   "  test_cutils [options]\n"
				   "\n"
				   "Options\n"
				   "  -T --test                    Run tests\n"
				   "  -B --dir          <dir>      Set directory\n"
				   "  -S --dir          <dir>      Set directory\n"
				   "  -D --debug        <0/1>      Turn on/off debug messages (default: 0)\n"
				   "  -M --mode         <mode>     Set mode (default: A)\n"
				   "\n"
				   "Modes\n"
				   "  A = A mode\n"
				   "  B = B mode\n"
				   "\n";
		EXPECT_STR(buf, exp);
	}

	const char *argv_h[] = {
		NAME,
		"-H",
	};

	{
		file_reopen(TEST_FILE, "wb+", file);
		EXPECT_EQ(args_handle(NAME, DESC, args, sizeof(args), modes, sizeof(modes), sizeof(argv_h) / sizeof(char *), argv_h, params, file), 2);

		char buf[512] = { 0 };
		file_read_ft(file, buf, sizeof(buf));

		const char exp[] = "cutils tests\n"
				   "\n"
				   "Usage\n"
				   "  test_cutils [options]\n"
				   "\n"
				   "Options\n"
				   "  -T --test                    Run tests\n"
				   "  -B --dir          <dir>      Set directory\n"
				   "  -S --dir          <dir>      Set directory\n"
				   "  -D --debug        <0/1>      Turn on/off debug messages (default: 0)\n"
				   "  -M --mode         <mode>     Set mode (default: A)\n"
				   "\n"
				   "Modes\n"
				   "  A = A mode\n"
				   "  B = B mode\n"
				   "\n";
		EXPECT_STR(buf, exp);
	}

	END;
}

STEST(t_args)
{
	SSTART;

	FILE *file = file_open(TEST_FILE, "wb+");

	RUN(t_args_usage, file);
	RUN(t_args_handle, file);

	file_close(file);
	file_delete(TEST_FILE);

	SEND;
}
