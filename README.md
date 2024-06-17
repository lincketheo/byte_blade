# Byte Blade
A binary data analyzer with numpy array like syntax.

Have a data file and you don't want to open a python shell and use numpy? Not to worry, byte\_blade is here!

## Building:
```sh
$ make clean
$ make
$ ./bb --help
```

## Usage:
By default byte blade assumes `uint8\_t` data in dec mode. So if you just run:
```
$ ./bb <filename>
```

It will print out all the bytes of the file.

You can specify the data type with various flags. For example, let's suppose your data is a string of complex floats, then run

```
$ ./bb <filename> CF64
```

You can see a list of all supported data types by running `./bb --help`

If you want to see your data in hex format, you can run with the `--hex` flag (there's `--hex`, `--bin` and `--dec`):

```
$ ./bb <filename> --hex CF64
```

You can also specify the endianess of your output:

```
$ ./bb <filename> --hex --big CF64
```

Too much data? Not to worry, byte\_blade supports indexing. You can add numpy-like indexing (with negative numbers included). Here are some examples:

Prints element 5
```
$ ./bb <filename> --hex --big CF64 5
```

Prints elements 5-\>size-10
```
$ ./bb <filename> --hex --big CF64 5:-10
```

Prints elements 5-\>end
```
$ ./bb <filename> --hex --big CF64 5:
```

Prints elements 0-\>size-1
```
$ ./bb <filename> --hex --big CF64 :-1
```

Prints elements 0-\>10
```
$ ./bb <filename> --hex --big CF64 :10
```

Prints elements size-5-\>10
```
$ ./bb <filename> --hex --big CF64 -5:10
```

Prints all elements
```
$ ./bb <filename> --hex --big CF64 :
```

Also Prints all elements
```
$ ./bb <filename> --hex --big CF64
```
