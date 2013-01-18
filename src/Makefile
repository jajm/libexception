include ../config.mk

CFLAGS := -Wall -Wextra -Werror -g -std=c99
CPPFLAGS := -I../include
LIBTOOL_VERSION := $(LIBTOOL_CURRENT):$(LIBTOOL_REVISION):$(LIBTOOL_AGE)

OBJS := $(patsubst %.c,%.lo,$(wildcard *.c))
HEADERS := $(wildcard *.h)

all: libexception.la

libexception.la: $(OBJS)
	$(LIBTOOL) --mode=link $(CC) $(LDFLAGS) -rpath $(LIBDIR) -version-info $(LIBTOOL_VERSION) -o $@ $^

%.lo: %.c
	$(LIBTOOL) --mode=compile $(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

install: libexception.la
	$(INSTALL) -d $(INCLUDEDIR)
	$(INSTALL) --mode=0644 $(HEADERS) $(INCLUDEDIR)
	$(LIBTOOL) --mode=install $(INSTALL) libexception.la $(LIBDIR)

clean:
	rm -rf *.o *.lo libexception.la .libs
