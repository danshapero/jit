.SECONDARY = 
.PHONY = clean

LIBJIT_PATH = /usr/local
LIBJIT_INCLUDE_PATH = $(LIBJIT_PATH)/include
LIBJIT_LIB_PATH = $(LIBJIT_PATH)/lib
LIBJIT_AR = $(LIBJIT_LIB_PATH)/libjit.a

CC = gcc
LD = gcc
CCOPT = -O0
CCFLAGS = -c $(CCOPT)
LDFLAGS = -lm -lpthread

%.o: %.c
	$(CC) -I$(LIBJIT_INCLUDE_PATH) -I. $(CCFLAGS) $<

test: linalg.o test.o
	$(LD) $^ $(LIBJIT_AR) $(LDFLAGS) -o $@

clean:
	rm -rf test *.o *~

