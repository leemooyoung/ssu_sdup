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

// 중복 파일 리스트 생성
LNKLIST *search_dup(char *path, off_t llimit, off_t ulimit, char *fextension, char *(*hashfunc)(int));

// 중복 파일 리스트의 내용 출력
void print_dup_list(FILE *fp, LNKLIST *head);

#endif
