/* Copyright 2014 Mike Burns */

#include <sys/wait.h>

#include <err.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "main.h"

__dead void	 usage();
char		*text_from_editor();
char		*text_from_file(char *);
void		 sign_and_upload(char *);
static char	*read_fd(int);

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
	char	*text;

	while ((ch = getopt(argc, argv, "")) != -1)
		switch (ch) {
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
	}

	sign_and_upload(text);

	free(text);

	return 0;
}

void
usage()
{
	fprintf(stderr, "usage: mkmnfst [file]\n");
	exit(64);
}

char *
text_from_editor()
{
	int	 fd, status;
	pid_t	 pid;
	char	*contents, template[24];

	strlcpy(template, "/tmp/mkmnfst.XXXXXXXXXX", 24);

	if ((fd = mkstemp(template)) == -1)
		err(1, "mktemp");

	printf("template: %s\n", template);

	switch (pid = fork()) {
	case -1:
		err(1, "fork");
		/* NOTREACHED */
		break;
	case 0:
		/* open $VISUAL | $EDITOR | ed on filename */
		printf("child ... done\n");
		break;
	}

	if (waitpid(pid, &status, 0) == -1)
		err(1, "waitpid");

	if (status == 0) {
		contents = read_fd(fd);
		if (unlink(template) == -1)
			warn("unlink");
		close(fd);
	} else {
		close(fd);
		if (unlink(template) == -1)
			warn("unlink");
		err(2, "bad exit status");
	}

	/* unlink file */
	close(fd);
	err(66, "whatever man");

	return contents;
}

char *
text_from_file(char *argv_filename)
{
	char	*filename, *buf;
	int	 fd;

	if ((filename = realpath(argv_filename, NULL)) == NULL)
		err(1, "readpath");

	if ((fd = open(filename, O_RDONLY)) == -1)
		err(1, "open");
	free(filename);

	buf = read_fd(fd);

	close(fd);

	return buf;
}

void
sign_and_upload(char *text)
{
	printf("final text:\n%s\n", text);
}

static char *
read_fd(int fd)
{
	char	*buf, *nbuf;
	int	 nread;
	ssize_t	 ret;
	size_t	 nbytes;

	nbytes = 1024;
	nread = 0;

	if ((buf = calloc(nbytes, sizeof(char))) == NULL)
		err(1, "calloc");

	while ((ret = read(fd, buf + nread, nbytes)) > 0) {
		nread += nbytes;
		if ((nbuf = reallocarray(buf, nread + nbytes, sizeof(char))) == NULL)
			err(1, "reallocarray");

		buf = nbuf;
	}

	if (ret == -1)
		err(1, "read");

	return buf;
}
