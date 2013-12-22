include ../VERSION.mk

header = types.h
obj = opaque.o list-and-monitor-udev.o device-evdev.o
lib_a = libspacemouse.a
soname = libspacemouse.so.$(VER_MAJOR)
lib_so = $(soname).$(VER_MINOR)

CC ?= gcc
AR ?= ar
override CFLAGS += -std=c89 -fPIC -pedantic -Wall -fno-strict-aliasing

.PHONY: all
all: $(header) $(lib_a) $(lib_so)

$(lib_a): $(obj)
	$(AR) rcs $@ $(obj)

$(lib_so): $(obj)
	$(CC) -shared -Wl,-soname,$(soname) -o $@ $(obj) $(LDFLAGS) -L. -ludev

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(obj) $(lib_a) $(lib_so)