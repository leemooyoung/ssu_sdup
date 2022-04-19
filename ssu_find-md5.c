#include <stdio.h>
#include <stdlib.h>
#include "hash.h"
#include "find_command.h"

char *md5hashstr(int fd) {
	unsigned char md[MD5_DIGEST_LENGTH];

	md5hash(fd, md);
	return hash_to_str(NULL, md, MD5_DIGEST_LENGTH);
}

int main(int argc, char *argv[]) {
	exit(find_command(argc, argv, md5hashstr));
}
