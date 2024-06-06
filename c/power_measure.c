
#include <complex.h>
#include <getopt.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct option long_options[] = {
  { "help", no_argument, 0, 'h' },
};

static FILE* fp;
static complex float* buffer;
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
  printf("\n\nUsage: %s file [--help]\n", exe);
  printf("Options:\n");
  printf("    file      - Input File Name \n");
  printf("    --help    - Print this help menu\n");
  printf("\n");
  return;
}

double calc_pwr(float complex* sig, u_int32_t len)
{
  double avg = 0;
  for (u_int32_t ii = 0; ii < len; ii++) {
    avg += pow(cabsf(sig[ii]), 2);
  }
  avg = avg / len;
  return avg;
}

ssize_t get_file_size_bytes(const char* filename)
{
  FILE* file = fopen(filename, "rb");
  if (file == NULL) {
    printf("Error opening file.\n");
    return -1;
  }
  fseek(file, 0, SEEK_END);
  size_t size = ftell(file);
  fclose(file);
  return size;
}

typedef struct {
  char* filename;
} cli_args;

int main(int argc, char** argv)
{
  atexit(teardown);

  char c;
  int ind;
  cli_args args = {
    .filename = NULL,
  };

  while ((c = getopt_long(argc, argv, "I:s:e:h", long_options, &ind)) > 0) {
    switch (c) {
    case 'h':
      print_help(argv[0]);
      return 0;
      break;
    default:
      break;
    }
  }

  if (optind + 1 != argc) {
    print_help(argv[0]);
    return 1;
  }

  args.filename = argv[optind++];

  ssize_t file_size = get_file_size_bytes(args.filename);
  if (file_size < 0)
    return 1;

  if (file_size % 8 != 0) {
    printf("File %s is not composed of complex floats. Size must be multiple of %zu\n", args.filename, sizeof(complex float));
    return 1;
  }

  fp = fopen(args.filename, "rb");
  if (fp == NULL) {
    printf("Failed to open file: %s\n", args.filename);
    // TODO - print reason
    return 1;
  }

  b_capacity = file_size / sizeof(complex float);
  buffer = malloc(sizeof *buffer * b_capacity);
  if(!buffer) {
    printf("Failed to allocate memory for buffer\n");
    return 1;
  }
  
  b_size = fread(buffer, sizeof *buffer, b_capacity, fp);
  double power = calc_pwr(buffer, b_size);

  printf("Power: %f\n", power);

  return 0;
}
