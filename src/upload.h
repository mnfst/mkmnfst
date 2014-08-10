#ifndef UPLOAD_H
#define UPLOAD_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define DOMAIN_NAME "www.mnfst.io"
#define CREATE_STATUS "http://"DOMAIN_NAME"/statuses"

void	upload(char *);
#endif
