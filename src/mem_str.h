#ifndef MEM_STR_H
#define MEM_STR_H

struct mem_str {
	char	*str;	/* The string. */
	size_t	 size;	/* The size of the string. */
};

struct mem_str	*new_mem_str();
void		 free_mem_str(struct mem_str *);

#endif /* MEM_STR_H */
