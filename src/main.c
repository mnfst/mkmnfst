/* Copyright 2014 Mike Burns, mnfst */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "compat.h"

#include "sign.h"
#include "text.h"
#include "upload.h"

__dead void	usage();
static void	sign_and_upload(char *, int, char *);

extern char	*optarg;

/*
 * Post a manifest to Mnfst.
 *
 * Usage:
 *
 *   mkmnfst [file]
 */
int
main(int argc, char *argv[])
{
	int	 ch;
	char	*text, *server_name;
	int	 use_https;

	server_name = NULL;
	use_https = 1;
	text = NULL;

	while ((ch = getopt(argc, argv, "0s:")) != -1)
		switch (ch) {
		case '0':
			use_https = 0;
			break;
		case 's':
			server_name = optarg;
			break;
		default:
			usage();
			/* NOTREACHED */
		}
	argc -= optind;
	argv += optind;

	switch (argc) {
	case 0:
		text = text_from_editor();
		break;
	case 1:
		text = text_from_file(argv[0]);
		break;
	default:
		usage();
		/* NOTREACHED */
		break;
	}

	sign_and_upload(text, use_https, server_name);

	free(text);

	return 0;
}

/*
 * Display a usage message and exit.
 */
void
usage()
{
	fprintf(stderr, "usage: mkmnfst [-0] [file]\n");
	exit(64);
}

/*
 * Sign the text, stick it in a JSON, and upload the JSON.
 */
static void
sign_and_upload(char *text, int use_https, char *server_name)
{
	char	*signature;
	char	*json, *json_template;
	int	 len;

	json_template = "{\"status\":{\"signed_body\":\"%s\"}}";

	signature = sign(text);

	len = strlen(signature) + 30;
	if ((json = calloc(len, sizeof(char))) == NULL)
		err(1, "calloc");

	snprintf(json, len, json_template, signature);
	free(signature);

	upload(json, use_https, server_name);
	free(json);
}
