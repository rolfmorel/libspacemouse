include VERSION.mk

PREFIX ?= /usr/local
LIBDIR ?= lib
INCLUDEDIR ?= include

lib_hdr = libspacemouse.h
lib_a = libspacemouse.a
lib_so_link_link = libspacemouse.so
lib_so_link = $(lib_so_link_link).$(VER_MAJOR)
lib_so = $(lib_so_link).$(VER_MINOR)

all: src

.PHONY: src
src:
	@$(MAKE) -C src
	cp src/$(lib_hdr) src/$(lib_a) src/$(lib_so) $(CURDIR)

.PHONY: install
install: $(lib_hdr) $(lib_a) $(lib_so)
	install -D -m 644 $(lib_hdr) $(DESTDIR)$(PREFIX)/$(INCLUDEDIR)/$(lib_hdr)
	install -D -m 644 $(lib_a) $(DESTDIR)$(PREFIX)/$(LIBDIR)/$(lib_a)
	install -D -m 755 $(lib_so) $(DESTDIR)$(PREFIX)/$(LIBDIR)/$(lib_so)
	ln -f -s $(lib_so) $(DESTDIR)$(PREFIX)/$(LIBDIR)/$(lib_so_link)
	ln -f -s $(lib_so_link) $(DESTDIR)$(PREFIX)/$(LIBDIR)/$(lib_so_link_link)

.PHONY: uninstall
uninstall:
	rm -f $(DESTDIR)$(PREFIX)/$(INCLUDEDIR)/$(lib_hdr) \
	$(addprefix, $(DESTDIR)$(PREFIX)/$(LIBDIR)/, \
	  $(lib_a) $(lib_so) $(lib_so_link) $(lib_so_link_link))

.PHONY: clean
clean:
	@$(MAKE) -C src clean
	rm -f $(lib_a) $(lib_so) $(lib_hdr)

.PHONY: distclean
distclean: clean
	@$(MAKE) -C examples clean

.PHONY: examples
examples: $(lib_hdr) $(lib_a) $(lib_so)
	@$(MAKE) -C examples
