#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/time.h>
#include "inputcntl.h"
#include "search_dup.h"
#include "hash.h"
#include "linkedlist.h"

char *md5hashstr(int fd) {
	unsigned char md[MD5_DIGEST_LENGTH];

	md5hash(fd, md);
	return hash_to_str(NULL, md, MD5_DIGEST_LENGTH);
}

int main(int argc, char *argv[]) {
	char *fext;
	off_t llimit;
	off_t ulimit;
	char *target_dir;
	char *env_home;
	int env_home_len;
	LNKLIST *head;
	DIR *dp;

	if(argc != 5) {
		fprintf(stderr, "%s [FILE_EXTENSION] [MINSIZE] [MAXSIZE] [TARGET_DIRECTORY]\n", argv[0]);
		exit(1);
	}

	// FILE_EXTENSION 인자 검사
	if(strcmp(argv[1], "*") == 0) {
		fext = NULL;
	} else if(strncmp(argv[1], "*.", 2) == 0) {
		fext = argv[1] + 1;
	} else {
		fprintf(stderr, "[FILE_EXTENSION] : * | *.[extension]");
		exit(1);
	}

	// MINSIZE, MAXSIZE 검사
	if((llimit = strtosize(argv[2])) == -2) {
		fprintf(stderr, "[MINSIZE] : ([integer] | [float])(kb | KB | mb | MB | gb | GB)");
		exit(1);
	}

	if((ulimit = strtosize(argv[3])) == -2) {
		fprintf(stderr, "[MAXSIZE] : ([integer] | [float])(kb | KB | mb | MB | gb | GB)");
		exit(1);
	}

	env_home = getenv("HOME");
	if(strcmp(argv[4], "~") == 0 && env_home != NULL) {
		target_dir = env_home;
	} else if(strncmp(argv[4], "~/", 2) == 0 && env_home != NULL) {
		env_home_len = strlen(env_home);
		target_dir = (char*)malloc(sizeof(char) * (env_home_len + strlen(argv[4])));
		// ~ 의 자리는 \0이 대체하기 때문에 길이에서 하나 빼줄 필요는 없음
		strcpy(target_dir, env_home);
		strcpy(target_dir + env_home_len, argv[4] + 1);
	} else
		target_dir = argv[4];

	if((dp = opendir(target_dir)) == NULL) {
		fprintf(stderr, "[TARGET_DIRECTORY] is wrong\n");
		exit(1);
	}

	closedir(dp);

	head = search_dup(target_dir, llimit, ulimit, fext, md5hashstr);
	print_dup_list(stdout, head);

	if(target_dir != argv[4] && target_dir != env_home)
		free(target_dir);

	exit(0);
}
