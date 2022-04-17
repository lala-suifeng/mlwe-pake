CC 		    = /usr/bin/cc
CFLAGS    = -g -Wall -Wextra -O3 -fomit-frame-pointer -fPIC
NISTFLAGS = -O3 -fomit-frame-pointer -fPIC
CLANG     = clang -O3 -fomit-frame-pointer -fwrapv -Qunused-arguments
RM 		    = /bin/rm


all: 	test/mlwe_pak \
		test/speed_us \

SOURCES = pake.c poly.c polyvec.c fips202.o reduce.c cbd.c precomp.c ntt.c randombytes.c key_consensus.c 
HEADERS = params.h poly.h polyvec.h reduce.h cbd.h ntt.h rng.h randombytes.h key_consensus.h

test/mlwe_pak: $(SOURCES) $(HEADERS) test/mlwe_pak.c 
	$(CC) $(CLANGS) -o $@ $(SOURCES) test/mlwe_pak.c

# test/speed: $(SOURCES) $(HEADERS) cpucycles.h cpucycles.c test/speed.c 
# 	$(CC) $(CFLAGS) -o $@ $(SOURCES) cpucycles.c test/speed.c

test/speed_us: $(SOURCES) $(HEADERS) test/speed_us.c 
	$(CC) $(CFLAGS) -o $@ $(SOURCES) test/speed_us.c

fips202.o: fips202.c
	$(CLANG) -c -fPIC $^ -o $@

.PHONY: clean test

test: 
	./test/mlwe_pak
	./test/speed
	./test/speed_us
clean:
	-$(RM) *.o
	-$(RM) -r test/mlwe_pak
	-$(RM) -r test/speed_us
