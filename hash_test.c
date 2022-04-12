#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "hash.h"
#include <openssl/md5.h>

int main(int argc, char *argv[]) {
	int fd;
	int i;
	int k;
	unsigned char hashbuf[100];
	char md5hashstr[MD5_DIGEST_LENGTH * 2 + 1];
	char sha1hashstr[SHA_DIGEST_LENGTH * 2 + 1];

	for(i = 1; i < argc; i++) {
		if((fd = open(argv[i], O_RDONLY)) < 0) {
			fprintf(stderr, "open error for %s\n", argv[i]);
			continue;
		}

		md5hash(fd, hashbuf);
		hash_to_str(md5hashstr, hashbuf, MD5_DIGEST_LENGTH);

		sha1hash(fd, hashbuf);
		hash_to_str(sha1hashstr, hashbuf, SHA_DIGEST_LENGTH);

		printf("%s : (md5)%s (sha1)%s\n", argv[i], md5hashstr, sha1hashstr);

		close(fd);
	}

	exit(0);
}
