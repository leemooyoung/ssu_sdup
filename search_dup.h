#ifndef _SDUP_SEARCH_DUP
#define _SDUP_SEARCH_DUP

#include <sys/types.h>
#include "linkedlist.h"

typedef struct {
	off_t size;
	char *pathname;
	char *hash;
	time_t mtime;
	time_t atime;
} FILEHASH;

LNKLIST *search_dup(char *path, off_t llimit, off_t ulimit, char *fextension, char *(*hashfunc)(int));

#endif
