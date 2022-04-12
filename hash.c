#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "hash.h"

unsigned char *md5hash(int fd, unsigned char *md) {
	MD5_CTX c;
	unsigned char buf[HASH_BUFSIZE];
	int read_cnt;
	off_t offset;

	if(md == NULL) {
		md = (char*)malloc(MD5_DIGEST_LENGTH);
	}

	if((offset = lseek(fd, 0, SEEK_CUR)) < 0) {
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	lseek(fd, 0, SEEK_SET);

	MD5_Init(&c);

	while(1) {
		if((read_cnt = read(fd, buf, HASH_BUFSIZE)) <= 0)
			break;

		MD5_Update(&c, buf, read_cnt);
	}

	MD5_Final(md, &c);

	lseek(fd, offset, SEEK_SET);

	return md;
}

unsigned char *sha1hash(int fd, unsigned char *md) {
	SHA_CTX c;
	unsigned char buf[HASH_BUFSIZE];
	int read_cnt;
	off_t offset;

	if(md == NULL) {
		md = (char*)malloc(SHA_DIGEST_LENGTH);
	}

	if((offset = lseek(fd, 0, SEEK_CUR)) < 0) {
		fprintf(stderr, "lseek error\n");
		exit(1);
	}

	lseek(fd, 0, SEEK_SET);

	SHA1_Init(&c);

	while(1) {
		if((read_cnt = read(fd, buf, HASH_BUFSIZE)) <= 0)
			break;

		SHA1_Update(&c, buf, read_cnt);
	}

	SHA1_Final(md, &c);

	lseek(fd, offset, SEEK_SET);

	return md;
}

char *hash_to_str(char *buf, unsigned char *hash, int hash_len) {
	if(hash == NULL)
		return NULL;
	
	if(buf == NULL)
		buf = (char*)malloc(hash_len * 2 + 1);

	for(int i = 0; i < hash_len; i++)
		sprintf(buf + i * 2, "%02x", hash[i]);

	return buf;
}
