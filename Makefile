CC=gcc
BIN=terminax
CFLAGS=-I/usr/include -L/usr/lib -lX11

all: $(BIN)

$(BIN): %: %.c
	$(CC) -o $@ $< $(CFLAGS)

clean:
	rm $(BIN)

install:
	cp $(BIN) /usr/bin/$(BIN)
	cp terminax.1 /usr/share/man/man1/

strip:
	strip -S --strip-unneeded --remove-section=.note.gnu.gold-version --remove-section=.comment --remove-section=.note --remove-section=.note.gnu.build-id --remove-section=.note.ABI-tag $(BIN)
