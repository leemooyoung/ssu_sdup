#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "inputcntl.h"
#include "search_dup.h"
#include "hash.h"

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
		fpritnf(stderr, "[FILE_EXTENSION] : * | *.[extension]");
		exit(1);
	}

	// MINSIZE, MAXSIZE 검사
	
}
