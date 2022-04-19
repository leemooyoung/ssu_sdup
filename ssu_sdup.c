#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#include "hash.h"
#include "inputcntl.h"

#define INPUT_SIZE (100)

#define FIND_MD5_EXEC_PATH ("./find-md5")
#define FIND_SHA1_EXEC_PATH ("./find-sha1")
#define HELP_EXEC_PATH ("./help")

int exec(char *pathname, char *argv[]) {
	// 자식 프로세스 생성 및 대기, 자식 프로세스의 종료 상태 반환

	pid_t pid;
	int status;

	if((pid = fork()) < 0) {
		fprintf(stderr, "fork error\n");
		exit(1);
	} else if(pid == 0) {
		if(execv(pathname, argv) < 0) {
			fprintf(stderr, "execv error\n");
			exit(1);
		}
	} else {
		if(waitpid(pid, &status, 0) != pid) {
			fprintf(stderr, "waitpid error\n");
			exit(1);
		}
	}

	return status;
}

int main(void) {
	char input[INPUT_SIZE];
	char **words = NULL;
	int status;

	while(1) {
		printf("20192433> ");
		if(get_input(stdin, input, INPUT_SIZE) == NULL) {
			fprintf(stderr, "read error\n");
			break;
		}

#ifdef DEBUG
		printf("input : \"%s\"\n", input);
#endif

		words = split_by_word(input);

		if(words == NULL)
			continue;

#ifdef DEBUG
		for(int i = 0; words[i] != NULL; i++)
			printf("%d : \"%s\"\n", i, words[i]);
#endif

		if(strcmp("fmd5", words[0]) == 0) {
			words[0] = FIND_MD5_EXEC_PATH;
			status = exec(FIND_MD5_EXEC_PATH, words);
		} else if(strcmp("fsha1", words[0]) == 0) {
			words[0] = FIND_SHA1_EXEC_PATH;
			status = exec(FIND_SHA1_EXEC_PATH, words);
		} else if(strcmp("exit", words[0]) == 0 && words[1] == NULL) {
			free(words);
			break;
		} else {
			status = exec(HELP_EXEC_PATH, words);
		}

		free(words);
		if(WIFSIGNALED(status)) {
			fprintf(stderr, "status : %d\n", WTERMSIG(status));
		}
	}

	printf("Prompt End\n");
	exit(0);
}
