include config.mk

.PHONY: src test clean

all: lib test

lib:
	$(MAKE) -C src all

test:
	$(MAKE) -C t all

install:
	$(MAKE) -C src install
	$(INSTALL) -d $(DESTDIR)$(pkgconfigdir)
	$(INSTALL) --mode=0644 libexception.pc $(DESTDIR)$(pkgconfigdir)

clean:
	$(MAKE) -C src clean
	$(MAKE) -C t clean
