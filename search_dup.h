#ifndef _SDUP_SEARCH_DUP
#define _SDUP_SEARCH_DUP

#include <sys/types.h>
#include "linkedlist.h"

#define TIMESTR_BUF_SIZE 20

typedef struct {
	off_t size;
	char *pathname;
	char *hash;
	time_t mtime;
	time_t atime;
} FILEHASH;

void free_filehash(void* fh);

// FILEHASH를 원소로 가지는 linkedlist를 지우기 위한 함수. 주로 lnklist_destroy의 두 번째 인자로 들어간다.
void lnklist_dest_fh(void *head);

// 중복 파일 리스트 생성
LNKLIST *search_dup(char *path, off_t llimit, off_t ulimit, char *fextension, char *(*hashfunc)(int));

// 중복 파일 리스트의 내용 출력
void print_dup_list(FILE *fp, LNKLIST *head);

// time_t 변수를 %Y-%m-%d %H:%M:%S 의 형식으로 buf에 쓴다
void timestr(char *buf, time_t t);

#endif
