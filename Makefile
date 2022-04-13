CC=gcc
LDFLAGS=-lcrypto

HASH_TEST_OBJECTS=hash.o hash_test.o
HASH_TEST_TARGET=hash_test

SDUP_TARGET=ssu_sdup
SDUP_OBJECTS=ssu_sdup.o inputcntl.o

HELP_TARGET=help
HELP_SRC=ssu_help.c

.PHONY: all clean
all: $(SDUP_TARGET) $(HELP_TARGET)

clean:
	rm $(HASH_TEST_TARGET) $(SDUP_TARGET) $(HELP_TARGET) *.o

$(SDUP_TARGET): $(SDUP_OBJECTS)
	$(CC) -o $@ $(SDUP_OBJECTS)

$(HELP_TARGET): $(HELP_SRC)
	$(CC) -o $@ $(HELP_SRC)

$(HASH_TEST_TARGET): $(HASH_TEST_OBJECTS)
	$(CC) -o $@ $(HASH_TEST_OBJECTS) $(LDFLAGS)

hash_test.o: hash_test.c hash.h
	$(CC) -c hash_test.c

hash.o: hash.c hash.h
	$(CC) -c hash.c

ssu_sdup.o: ssu_sdup.c inputcntl.h
	$(CC) -c ssu_sdup.c

inputcntl.o: inputcntl.c inputcntl.h
	$(CC) -c inputcntl.c
