CC=gcc
CFLAGS=-I.
FILES=basic1 basic2 basic3

all: $(FILES)

%.o: %.c 
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(FILES)
