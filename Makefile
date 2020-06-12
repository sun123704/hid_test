CC              := gcc
CFLAGS          := -W -Wall -g
LDFLAGS         := -g

all: main

uvc-gadget: main.o
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -f *.o
	rm -f hid_demo
