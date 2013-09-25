include ../config.mk
include ../package.mk

CFLAGS := -Wall -Wextra -Werror -g -std=c99
CPPFLAGS := -I../include
LIBTOOL_VERSION := $(LIBTOOL_CURRENT):$(LIBTOOL_REVISION):$(LIBTOOL_AGE)
LIBRARY_NAME := $(PACKAGE_NAME).la

OBJS := $(patsubst %.c,%.lo,$(wildcard *.c))
HEADERS := $(wildcard *.h)

all: $(LIBRARY_NAME)

$(LIBRARY_NAME): $(OBJS)
	$(LIBTOOL) --mode=link $(CC) $(LDFLAGS) -rpath $(libdir) -version-info $(LIBTOOL_VERSION) -o $@ $^

%.lo: %.c
	$(LIBTOOL) --mode=compile $(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

install: $(LIBRARY_NAME)
	$(INSTALL) -d $(DESTDIR)$(includedir)
	$(INSTALL) -d $(DESTDIR)$(libdir)
	$(INSTALL) --mode=0644 $(HEADERS) $(DESTDIR)$(includedir)
	$(LIBTOOL) --mode=install $(INSTALL) $< $(DESTDIR)$(libdir)

clean:
	rm -rf *.o *.lo $(LIBRARY_NAME) .libs
