

all: bb pwr

clean:
	rm -f bb
	rm -f pwr

bb:
	gcc ./c/byte_blade.c -lm -o bb -g

pwr:
	gcc ./c/power_measure.c -lm -o pwr -g

install: bb pwr ~/.local/bin
	cp bb ~/.local/bin
	cp pwr ~/.local/bin
