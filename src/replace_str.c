/*
 * Public domain. You may use this code in any way you see fit, optionally
 * crediting its author (Laird Shaw, with assistance from comp.lang.c).
 * 
 * http://creativeandcritical.net/str-replace-c/
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "compat.h"

/*
 * In str, replace old with new. Leave str unchanged, and return the string
 * with the substitutions.
 */
char *replace_str(const char *str, const char *old, const char *new)
{
	char *ret, *r;
	const char *p, *q;
	size_t count, retlen, newlen, oldlen;
	ptrdiff_t l;

	oldlen = strlen(old);
	newlen = strlen(new);

	if (oldlen != newlen) {
		for (count = 0, p = str; (q = strstr(p, old)) != NULL; p = q + oldlen)
			count++;
		/* this is undefined if p - str > PTRDIFF_MAX */
		retlen = p - str + strlen(p) + count * (newlen - oldlen);
	} else
		retlen = strlen(str);

	if ((ret = malloc(retlen + 1)) == NULL)
		return NULL;

	for (r = ret, p = str; (q = strstr(p, old)) != NULL; p = q + oldlen) {
		/* this is undefined if q - p > PTRDIFF_MAX */
		l = q - p;
		memcpy(r, p, l);
		r += l;
		memcpy(r, new, newlen);
		r += newlen;
	}
	strlcpy(r, p, retlen);

	return ret;
}
