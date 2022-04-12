CC=gcc
HASH_TEST_OBJECTS=hash.o hash_test.o
HASH_TEST_TARGET=hash_test
LDFLAGS=-lcrypto

.PHONY: clean $(TARGET)
all: ssu_sdup

clean:
	rm $(HASH_TEST_OBJECTS) *.o

# ssu_sdup: 

$(HASH_TEST_TARGET): $(HASH_TEST_OBJECTS)
	$(CC) -o $@ $(HASH_TEST_OBJECTS) $(LDFLAGS)

hash_test.o: hash_test.c hash.h
	$(CC) -c hash_test.c

hash.o: hash.c hash.h
	$(CC) -c hash.c

