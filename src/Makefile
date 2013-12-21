header = types.h
obj = opaque.o list-and-monitor-udev.o device-evdev.o
lib_a = libspacemouse.a
soname = libspacemouse.so.0
lib_so = $(soname).1

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
	echo $(CC)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f $(obj) $(lib_a) $(lib_so)