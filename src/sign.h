#ifndef SIGN_H
#define SIGN_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

struct sig_res {
	char	*signature;
	char	*raw_pub_key;
};

struct sig_res	*sign(char *, char *);
#endif
