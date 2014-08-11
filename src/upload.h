#ifndef UPLOAD_H
#define UPLOAD_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define DEFAULT_SERVER_NAME "www.mnfst.io"
#define POST_STATUS "statuses"

void	upload(char *, int, char *);
#endif
