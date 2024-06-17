
all: bb 

clean:
	rm -f bb

bb:
	gcc ./byte_blade.c -lm -o bb -g

install: bb ~/.local/bin
	cp bb ~/.local/bin

