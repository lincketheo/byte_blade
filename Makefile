

all: byte_blade

clean:
	rm -f byte_blade

byte_blade:
	gcc ./c/byte_blade.c -lm -o byte_blade

install: byte_blade ~/.local/bin
	cp byte_blade ~/.local/bin
