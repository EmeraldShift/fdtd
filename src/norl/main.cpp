#include "fdtd.hpp"

#include <iostream>
#include <cstdlib>
#include <argp.h>

static argp_option options[] = {
	{"print", 'p', nullptr, 0, "Print result grid values", 0 },
	{"threads", 'q', "#", 0, "Number of cores to distribute kernels across", 0 },
	{"dynamic", 'd', nullptr, 0, "Use dynamic-size queues", 0 },
	{ nullptr },
};

static char args_doc[] = "x y z t";

static error_t parse_opt(int key, char *arg, argp_state *state)
{
	auto *cfg = (Configuration *)state->input;

	switch (key) {
	case 'p':
		cfg->flags |= FLAG_PRT;
		break;
	case 'd':
		cfg->flags |= FLAG_DYN;
		break;
	case 'q': {
		if (!arg)
			argp_usage(state);
		auto threads = std::strtoul(arg, nullptr, 0);
		if (threads > 0 && 6 % threads != 0) // We allow 1, 2, 3, and 6 only
			argp_usage(state);
		cfg->threads = threads;
		break;
	}
	case ARGP_KEY_ARG:
		if (state->arg_num >= 4)
			argp_usage(state);
		cfg->args[state->arg_num] = std::strtoul(arg, nullptr, 0);
		break;
	case ARGP_KEY_END:
		if (state->arg_num < 4)
			argp_usage(state);
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static argp argp = { options, parse_opt, args_doc, nullptr, nullptr, nullptr, nullptr };

int main(int argc, char **argv)
{
	Configuration cfg;
	argp_parse(&argp, argc, argv, 0, nullptr, &cfg);
	return fdtd(cfg);
}
