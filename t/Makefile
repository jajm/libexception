include ../config.mk

CFLAGS := -Wall -Wextra -Werror -g -std=c99
CPPFLAGS := -I../src $(CPPFLAGS)

PROGRAMS := $(patsubst %.c,%,$(wildcard *.c))

all: $(PROGRAMS)

$(PROGRAMS): % : %.o ../src/libexception.la
	$(LIBTOOL) --mode=link $(CC) -o $@ $^

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf *.o $(PROGRAMS) .libs
