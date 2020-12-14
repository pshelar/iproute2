/*
 * iplink_gtp.c	GTP device support
 *
 *              This program is free software; you can redistribute it and/or
 *              modify it under the terms of the GNU General Public License
 *              as published by the Free Software Foundation; either version
 *              2 of the License, or (at your option) any later version.
 *
 * Authors:     Pravin B Shelar <pbshelar@fb.com>
 */

#include <stdio.h>

#include "rt_names.h"
#include "utils.h"
#include "ip_common.h"

#define GTP_ATTRSET(attrs, type) (((attrs) & (1L << (type))) != 0)

static void print_explain(FILE *f)
{
	fprintf(f,
		"Usage: ... gtp\n"
		"		[ [no]external ]\n"
		"\n"
	);
}

static void explain(void)
{
	print_explain(stderr);
}

static void check_duparg(__u64 *attrs, int type, const char *key,
			 const char *argv)
{
	if (!GTP_ATTRSET(*attrs, type)) {
		*attrs |= (1L << type);
		return;
	}
	duparg2(key, argv);
}

static int gtp_parse_opt(struct link_util *lu, int argc, char **argv,
			  struct nlmsghdr *n)
{
	bool metadata = 0;
	__u64 attrs = 0;

	while (argc > 0) {
		if (!matches(*argv, "external")) {
			check_duparg(&attrs, IFLA_GTP_COLLECT_METADATA,
				     *argv, *argv);
			metadata = true;
		}  else if (matches(*argv, "help") == 0) {
			explain();
			return -1;
		} else {
			fprintf(stderr, "gtp: unknown command \"%s\"?\n", *argv);
			explain();
			return -1;
		}

		argc--, argv++;
	}

	if (!metadata) {
		fprintf(stderr, "gtp: 'external' not specified\n");
		return -1;
	}

	if (metadata)
		addattr(n, 1024, IFLA_GTP_COLLECT_METADATA);

	return 0;
}

static void gtp_print_opt(struct link_util *lu, FILE *f, struct rtattr *tb[])
{
	if (!tb)
		return;

	if (tb[IFLA_GTP_COLLECT_METADATA]) {
		print_bool(PRINT_ANY, "external", "external ", true);
		return;
	}

	if (tb[IFLA_GTP_ROLE])
		print_uint(PRINT_ANY,
			   "role",
			   "role %u ",
			   rta_getattr_u32(tb[IFLA_GTP_ROLE]));

	if (tb[IFLA_GTP_PDP_HASHSIZE])
		print_uint(PRINT_ANY,
			   "hash_table",
			   "hash table %u ",
			   rta_getattr_u32(tb[IFLA_GTP_PDP_HASHSIZE]));
}

static void gtp_print_help(struct link_util *lu, int argc, char **argv,
	FILE *f)
{
	print_explain(f);
}

struct link_util gtp_link_util = {
	.id		= "gtp",
	.maxattr	= IFLA_GTP_MAX,
	.parse_opt	= gtp_parse_opt,
	.print_opt	= gtp_print_opt,
	.print_help	= gtp_print_help,
};
