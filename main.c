
#include <complex.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct option long_options[] = {
  { "dtype", required_argument, 0, 'I' },
  { "file", required_argument, 0, 'f' },
  { "start", required_argument, 0, 's' },
  { "end", required_argument, 0, 'e' },
  { "help", required_argument, 0, 'h' },
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

static void print_help(char* exe)
{
  printf("\n\nUsage: %s\n", exe);
  printf("Options:\n");
  printf("    --file  - Input File Name \n");
  printf("    --start - Output File Name \n");
  printf("    --end   - end index\n");
  printf("    --dtype - Format type to read in\n");
  printf("    --help  - Print this help menu\n");
  printf("input_fmt allowed options:\n");
  printf("  CF - Complex float (%zu bytes)\n", sizeof(complex float));
  printf("\n\n");
  return;
}

enum dtype {
  CF = 0,
};

typedef struct {
  int dtype;
  char* filename;
  size_t start;
  size_t end;
} cli_args;

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

  while ((c = getopt_long(argc, argv, "I:f:", long_options, &ind)) > 0) {
    switch (c) {
    case 'I':
      if (strcmp(optarg, "CF") == 0) {
        args.dtype = CF;
      } else {
        printf("Failed to parse data type: %s\n", optarg);
        return 1;
      }
      break;
    case 'f':
      args.filename = optarg;
      break;
    case 's':
      args.start = atol(optarg);
      break;
    case 'e':
      args.end = atol(optarg);
      break;
    case 'h':
      print_help(argv[0]);
      return 0;
      break;
    default:
      break;
    }
  }

  if (args.end <= args.start) {
    printf("Start: %zu must be before end: %zu\n", args.start, args.end);
    print_help(argv[0]);
    return 1;
  }
  if (args.filename == NULL) {
    printf("Please supply a file name\n");
    print_help(argv[0]);
    return 1;
  }

  fp = fopen(args.filename, "rb");
  if (fp == NULL) {
    printf("Failed to open file: %s\n", args.filename);
    // TODO - print reason
    return 1;
  }

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
  } else {
    printf("Unimplemented error\n");
    return 1;
  }

  return 0;
}
