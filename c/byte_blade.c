
#include <complex.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct option long_options[] = {
  { "help", no_argument, 0, 'h' },
  { "hex", no_argument, 0, 'x' },
  { "bin", no_argument, 0, 'b' },
  { "dec", no_argument, 0, 'd' },
};

static FILE* fp;
static void* buffer;
static size_t b_capacity = 0; // WRT element size
static size_t b_size = 0;

static void teardown()
{
  if (fp)
    fclose(fp);
  if (buffer)
    free(buffer);
}

// TODO - add sign flag
static void print_help(char* exe)
{
  printf("\n\nUsage: %s file start end dtype [--help]\n", exe);
  printf("Options:\n");
  printf("    file      - Input File Name \n");
  printf("    start     - Output File Name \n");
  printf("    end       - end index\n");
  printf("    dtype     - Format type to read in\n");
  printf("    --help    - Print this help menu\n");
  printf("\n");
  printf("    Print Modes: (Higher priority overrides lower priority). Defaults to dec\n");
  printf("    --hex     - Prints output in hex. Priority: 0\n");
  printf("    --bin     - Prints output in binary. Priority: 1\n");
  printf("    --dec     - Prints output in dec mode. Priority: 2\n");
  printf("\n");
  printf("input_fmt allowed options:\n");
  printf("  CF - Complex float (%zu bytes)\n", sizeof(complex float));
  printf("  SL - Unsigned Integers (%zu bytes)\n", sizeof(uint32_t));
  printf("  SB - Unsigned Bytes (%zu bytes)\n", sizeof(uint8_t));
  printf("\n\n");
  return;
}

enum dtype {
  CF = 0,
  SL = 1,
  SB = 2,
};

enum mode {
  HEX = 0,
  BIN = 1,
  DEC = 2,
};

typedef struct {
  enum dtype dtype;
  enum mode mode;
  char* filename;
  size_t start;
  size_t end;
  int hex;
  int bin;
  int dec;
} cli_args;

enum mode get_priority_mode(cli_args* args)
{
  if (args->dec) {
    return DEC;
  } else if (args->bin) {
    return BIN;
  } else if (args->hex) {
    return HEX;
  } else {
    return DEC;
  }
}

void print_cf(complex float z)
{
  float real = crealf(z);
  float imag = cimagf(z);

  if (imag >= 0) {
    printf("%.10f + %.10fi\n", real, imag);
  } else {
    printf("%.10f - %.10fi\n", real, -imag);
  }
}

void print_cf_array(complex float* arr, size_t len)
{
  for (int i = 0; i < len; ++i) {
    print_cf(arr[i]);
  }
}

void print_sl_bits(uint32_t n)
{
  for (int i = 31; i >= 0; i--) {
    uint32_t mask = 1 << i;
    printf("%d", (n & mask) ? 1 : 0);
    if (i % 8 == 0) {
      printf(" ");
    }
  }
  printf("\n");
}

void print_sl_array(uint32_t* arr, size_t len, enum mode mode)
{
  for (int i = 0; i < len; ++i) {
    switch (mode) {
    case HEX:
      printf("%X\n", arr[i]);
      break;
    case BIN:
      print_sl_bits(arr[i]);
      break;
    case DEC:
      printf("%d\n", arr[i]);
      break;
    }
  }
}

void print_sb_bits(uint32_t n)
{
  for (int i = 7; i >= 0; i--) {
    uint32_t mask = 1 << i;
    printf("%d", (n & mask) ? 1 : 0);
  }
  printf("\n");
}


void print_sb_array(uint8_t* arr, size_t len, enum mode mode)
{
  for (int i = 0; i < len; ++i) {
    switch (mode) {
    case HEX:
      printf("%02X\n", arr[i]);
      break;
    case BIN:
      print_sb_bits(arr[i]);
      break;
    case DEC:
      printf("%d\n", arr[i]);
      break;
    }
  }
}
int main(int argc, char** argv)
{
  atexit(teardown);

  char c;
  int ind;
  cli_args args = {
    .filename = NULL,
    .start = 0,
    .end = 0,
    .dtype = CF
  };

  while ((c = getopt_long(argc, argv, "I:s:e:h", long_options, &ind)) > 0) {
    switch (c) {
    case 'x':
      args.hex = 1;
      break;
    case 'b':
      args.bin = 1;
      break;
    case 'd':
      args.dec = 1;
      break;
    case 'h':
      print_help(argv[0]);
      return 0;
      break;
    default:
      break;
    }
  }

  if(optind + 4 != argc) {
    print_help(argv[0]);
    return 1;
  }

  args.filename = argv[optind++];
  args.start = atol(argv[optind++]);
  args.end = atol(argv[optind++]);

  if (strcmp(argv[optind], "CF") == 0) {
    args.dtype = CF;
  } else if (strcmp(argv[optind], "SL") == 0) {
    args.dtype = SL;
  } else if (strcmp(argv[optind], "SB") == 0) {
    args.dtype = SB;
  } else {
    printf("Failed to parse data type: %s\n", argv[optind]);
    return 1;
  }

  if (args.end <= args.start) {
    printf("Start: %zu must be before end: %zu\n", args.start, args.end);
    print_help(argv[0]);
    return 1;
  }

  fp = fopen(args.filename, "rb");
  if (fp == NULL) {
    printf("Failed to open file: %s\n", args.filename);
    // TODO - print reason
    return 1;
  }
  enum mode mode = get_priority_mode(&args);

  if (args.dtype == CF) {
    b_capacity = (args.end - args.start);
    complex float* typed_buffer = malloc(sizeof *typed_buffer * b_capacity);
    buffer = typed_buffer;

    if (fseek(fp, args.start * sizeof *typed_buffer, SEEK_CUR) != 0) {
      printf("Failed to seek file\n");
      return 1;
    }

    b_size = fread(buffer, sizeof *typed_buffer, b_capacity, fp);
    print_cf_array(typed_buffer, b_size);
  } else if (args.dtype == SL) {
    b_capacity = (args.end - args.start);
    uint32_t* typed_buffer = malloc(sizeof *typed_buffer * b_capacity);
    buffer = typed_buffer;

    if (fseek(fp, args.start * sizeof *typed_buffer, SEEK_CUR) != 0) {
      printf("Failed to seek file\n");
      return 1;
    }

    b_size = fread(buffer, sizeof *typed_buffer, b_capacity, fp);
    print_sl_array(typed_buffer, b_size, mode);
  } else if (args.dtype == SB) {
    b_capacity = (args.end - args.start);
    uint8_t* typed_buffer = malloc(sizeof *typed_buffer * b_capacity);
    buffer = typed_buffer;

    if (fseek(fp, args.start * sizeof *typed_buffer, SEEK_CUR) != 0) {
      printf("Failed to seek file\n");
      return 1;
    }

    b_size = fread(buffer, sizeof *typed_buffer, b_capacity, fp);
    print_sb_array(typed_buffer, b_size, mode);
  } else {
    printf("Unimplemented error\n");
    return 1;
  }

  return 0;
}
