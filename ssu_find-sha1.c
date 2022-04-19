#include <stdio.h>
#include <stdlib.h>
#include "hash.h"
#include "find_command.h"

char *sha1hashstr(int fd) {
	unsigned char md[SHA_DIGEST_LENGTH];

	sha1hash(fd, md);
	return hash_to_str(NULL, md, SHA_DIGEST_LENGTH);
}

int main(int argc, char *argv[]) {
	exit(find_command(argc, argv, sha1hashstr));
}
