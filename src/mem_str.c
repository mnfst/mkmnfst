#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <err.h>
#include <stdlib.h>

#include "mem_str.h"

/*
 * Allocate a new memory string structure, of size zero.
 */
struct mem_str *
new_mem_str()
{
	struct mem_str	*m;

	if ((m = malloc(sizeof(struct mem_str))) == NULL)
		err(1, "malloc");

	m->size = 0;
	if ((m->str = calloc(1, sizeof(char))) == NULL)
		err(1, "calloc");

	return m;
}

/*
 * Free a mem_str structure.
 */
void
free_mem_str(struct mem_str *m)
{
	m->size = 0;
	free(m->str);
	m->str = NULL;
	free(m);
}
