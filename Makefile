

all: bb

clean:
	rm -f bb

bb:
	gcc ./c/byte_blade.c -lm -o bb

install: bb ~/.local/bin
	cp bb ~/.local/bin
