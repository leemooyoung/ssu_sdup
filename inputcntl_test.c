#include <stdio.h>
#include <stdlib.h>
#include "inputcntl.h"

#define INPUT_SIZE 100

int main(void) {
	char input[INPUT_SIZE];
	off_t size;
	char str[50];

	printf("%s\n", getenv("HOME"));
	while(1) {
		if(get_input(stdin, input, INPUT_SIZE) == NULL) {
			fprintf(stderr, "read error\n");
			break;
		}

		size = strtosize(input);
		printf("size : %ld\n", size);
		size_to_sep_str(str, size);
		printf("sep_str : %s\n", str);
	}

	exit(0);
}
