include config.mk

.PHONY: src test clean

all: lib test

lib:
	$(MAKE) -C src all

test:
	$(MAKE) -C t all

install:
	$(MAKE) -C src install
	$(INSTALL) --mode=0644 libexception.pc $(PKGCONFIGDIR)

clean:
	$(MAKE) -C src clean
	$(MAKE) -C t clean
