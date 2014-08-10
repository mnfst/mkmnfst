AM_CPPFLAGS = $(GPGME_CFLAGS) $(CURL_CFLAGS)
LDADD = $(GPGME_LIBS) $(CURL_LIBS)
AM_CFLAGS = -Wall -Wextra -pedantic-errors -Wno-unused-parameter -Werror
bin_PROGRAMS = mkmnfst
dist_mkmnfst_SOURCES = main.c replace_str.c replace_str.h text.c text.h \
											 sign.c sign.h upload.c upload.h \
											 compat.c compat.h
