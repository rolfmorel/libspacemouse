CC = gcc
CFLAGS = -std=c89 -pedantic -Wall -g -I../. -I/usr/local/include
LDFLAGS = -L../. -L/usr/local/lib -lspacemouse -ludev

.PHONY: all
all: simple_list_and_monitor list_monitor_open_events

simple_list_and_monitor: simple_list_and_monitor.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

list_monitor_open_events: list_monitor_open_events.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

.PHONY: clean
clean:
	rm -f simple_list_and_monitor list_monitor_open_events
