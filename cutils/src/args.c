//TODO: Return error message instead of help when wrong args are passed:
//  test_cutils: invalid option: 'h'
//  test_cutils: 'h' requires parameter

#include "args.h"

#include <string.h>

static inline int switch_int(char c)
{
	switch (c) {
	case '0': return 0;
	case '1': return 1;
	default: return -1;
	}
}

static int handle_string(const char *param, void *ret)
{
	*(const char **)ret = param;
	return 0;
}

static int handle_switch(const char *param, void *ret)
{
	if (param[1] != '\0' || (*(int *)ret = switch_int(param[0])) == -1) {
		return 1;
	}
	return 0;
}

// clang-format off
static param_handler_fn s_handlers[] = {
	[PARAM_NONE] = NULL,
	[PARAM_INT] = NULL,
	[PARAM_STR] = handle_string,
	[PARAM_MODE] = NULL,
	[PARAM_SWITCH] = handle_switch,
};
// clang-format on

static inline void header(const char *name, const char *description, FILE *file)
{
	c_fprintf(file, "%s\n", description);
	c_fprintf(file, "\n");
	c_fprintf(file, "Usage\n");
	c_fprintf(file, "  %s [options]\n", name);
	c_fprintf(file, "\n");
}

void args_usage(const char *name, const char *description, FILE *file)
{
	header(name, description, file);
	c_fprintf(file, "Run '%s --help' for more information\n", name);
	c_fflush(file);
}

static inline void help(const char *name, const char *description, const arg_t *args, size_t args_size, const mode_desc_t *modes, size_t modes_size, FILE *file)
{
	header(name, description, file);
	c_fprintf(file, "Options\n");

	size_t args_len = args_size / sizeof(arg_t);
	for (size_t i = 0; i < args_len; i++) {
		c_fprintf(file, "  -%c --%-12s %-10s %s\n", args[i].c, args[i].l, args[i].name, args[i].desc);
	}
	c_fprintf(file, "\n");

	size_t modes_len = modes_size / sizeof(mode_desc_t);
	for (size_t mode = 0; mode < modes_len; mode++) {
		c_fprintf(file, "%s\n", modes[mode].name);
		for (size_t i = 0; i < modes[mode].len; i++) {
			c_fprintf(file, "  %c = %s\n", modes[mode].modes[i].c, modes[mode].modes[i].desc);
		}
		c_fprintf(file, "\n");
	}

	c_fflush(file);
}

static inline int get_arg(const arg_t *args, size_t args_size, int argc, const char **argv, int *index, const char **param)
{
	int arg = -2;

	size_t args_len = args_size / sizeof(arg_t);
	if (argv[*index][0] == '-') {
		if (argv[*index][1] == '-') {
			for (size_t i = 0; i < args_len; i++) {
				if (strcmp(&argv[*index][2], args[i].l) == 0) {
					arg = (int)i;
					break;
				} else if (strcmp(&argv[*index][2], "help") == 0) {
					arg = -1;
					break;
				}
			}
		} else {
			for (size_t i = 0; i < args_len; i++) {
				if (argv[*index][1] == args[i].c) {
					arg = (int)i;
					break;
				} else if (argv[*index][1] == 'H') {
					arg = -1;
					break;
				}
			}
		}
	}

	(*index)++;

	if (arg >= 0 && args[arg].param != PARAM_NONE) {
		if (*index < argc) {
			*param = argv[(*index)++];
		} else {
			arg = -1;
		}
	}

	return arg;
}

static inline int handle_param(const arg_t *args, size_t arg, const char *param, void *ret)
{
	param_handler_fn handler = args[arg].handler ? args[arg].handler : s_handlers[args[arg].param];

	if (handler == NULL) {
		return 0;
	}

	return handler(param, ret);
}

int args_handle(const char *name, const char *description, const arg_t *args, size_t args_size, const mode_desc_t *modes, size_t modes_size, int argc, const char **argv,
		void **params, FILE *file)
{
	int ret = 0;

	int i = 1;
	while (i < argc) {
		const char *param = NULL;

		int arg = get_arg(args, args_size, argc, argv, &i, &param);
		if (arg == -1) {
			help(name, description, args, args_size, modes, modes_size, file);
			ret = 2;
		} else if (arg == -2) {
			ret = 1;
		}

		if (arg >= 0 && handle_param(args, arg, param, params[arg])) {
			ret = 1;
		}
	}

	if (ret == 1) {
		args_usage(name, description, file);
	}

	return ret;
}
