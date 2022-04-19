#ifndef _SDUP_HASH_H
#define _SDUP_HASH_H

#include <openssl/md5.h>
#include <openssl/sha.h>

#define HASH_BUFSIZE 1024*16

unsigned char *md5hash(int fd, unsigned char *md);

unsigned char *sha1hash(int fd, unsigned char *md);

char *hash_to_str(char *buf, unsigned char *hash, int hash_len);

#endif
