CC=gcc
LDFLAGS=-lcrypto

ifeq ($(DEBUG), 1)
DBG_FLAGS=-g
endif

HASH_TEST_TARGET=hash_test
HASH_TEST_OBJECTS=hash.o hash_test.o

INPUTCNTL_TEST_TARGET=inputcntl_test
INPUTCNTL_TEST_OBJECTS=inputcntl_test.o inputcntl.o

SDUP_TARGET=ssu_sdup
SDUP_OBJECTS=ssu_sdup.o inputcntl.o

FIND_MD5_TARGET=find-md5
FIND_MD5_OBJECTS=ssu_find-md5.o hash.o find_command.o search_dup.o inputcntl.o queue.o linkedlist.o

FIND_SHA1_TARGET=find-sha1
FIND_SHA1_OBJECTS=ssu_find-sha1.o hash.o find_command.o search_dup.o inputcntl.o queue.o linkedlist.o

HELP_TARGET=help
HELP_SRC=ssu_help.c

.PHONY: all debug clean
all: $(SDUP_TARGET) $(FIND_MD5_TARGET) $(FIND_SHA1_TARGET) $(HELP_TARGET)

debug: DBG_FLAGS=-g
debug: all

clean:
	-rm $(SDUP_TARGET) $(FIND_MD5_TARGET) $(HELP_TARGET) $(HASH_TEST_TARGET) $(INPUTCNTL_TEST_TARGET) *.o

$(SDUP_TARGET): $(SDUP_OBJECTS)
	$(CC) -o $@ $(SDUP_OBJECTS)

$(FIND_MD5_TARGET): $(FIND_MD5_OBJECTS)
	$(CC) -o $@ $(FIND_MD5_OBJECTS) $(LDFLAGS)

$(FIND_SHA1_TARGET): $(FIND_SHA1_OBJECTS)
	$(CC) -o $@ $(FIND_SHA1_OBJECTS) $(LDFLAGS)

$(HELP_TARGET): $(HELP_SRC)
	$(CC) -o $@ $(HELP_SRC) $(DBG_FLAGS)

$(HASH_TEST_TARGET): $(HASH_TEST_OBJECTS)
	$(CC) -o $@ $(HASH_TEST_OBJECTS) $(LDFLAGS)

$(INPUTCNTL_TEST_TARGET): $(INPUTCNTL_TEST_OBJECTS)
	$(CC) -o $@ $(INPUTCNTL_TEST_OBJECTS)

hash_test.o: hash_test.c hash.h
	$(CC) -c hash_test.c $(DBG_FLAGS)

ssu_sdup.o: ssu_sdup.c inputcntl.h
	$(CC) -c ssu_sdup.c $(DBG_FLAGS)

ssu_find-md5.o: ssu_find-md5.c hash.h find_command.h
	$(CC) -c ssu_find-md5.c $(DBG_FLAGS)

ssu_find-sha1.o: ssu_find-sha1.c hash.h find_command.h
	$(CC) -c ssu_find-sha1.c $(DBG_FLAGS)

search_dup.o: search_dup.c search_dup.h inputcntl.h queue.h linkedlist.h
	$(CC) -c search_dup.c $(DBG_FLAGS)

find_command.o: find_command.c find_command.h search_dup.h inputcntl.h linkedlist.h
	$(CC) -c find_command.c $(DBG_FLAGS)

hash.o: hash.c hash.h
	$(CC) -c hash.c $(DBG_FLAGS)

inputcntl.o: inputcntl.c inputcntl.h
	$(CC) -c inputcntl.c $(DBG_FLAGS)

queue.o: queue.c queue.h
	$(CC) -c queue.c $(DBG_FLAGS)

linkedlist.o: linkedlist.c linkedlist.h
	$(CC) -c linkedlist.c $(DBG_FLAGS)
