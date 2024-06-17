#include <assert.h>
#include <complex.h>
#include <getopt.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static struct option long_options[] = {
  { "help", no_argument, 0, 'h' },
  { "hex", no_argument, 0, 'x' },
  { "bin", no_argument, 0, 'b' },
  { "dec", no_argument, 0, 'd' },
};

enum dtype {
  U8,
  U16,
  U32,
  U64,
  I8,
  I16,
  I32,
  I64,
  F32,
  F64,
  CF64,
  CF128,
};

size_t span(enum dtype dtype)
{
  switch (dtype) {
  case U8: {
    return sizeof(uint8_t);
  }
  case U16: {
    return sizeof(uint16_t);
  }
  case U32: {
    return sizeof(uint32_t);
  }
  case U64: {
    return sizeof(uint64_t);
  }
  case I8: {
    return sizeof(int8_t);
  }
  case I16: {
    return sizeof(int16_t);
  }
  case I32: {
    return sizeof(int32_t);
  }
  case I64: {
    return sizeof(uint64_t);
  }
  case F32: {
    return sizeof(float);
  }
  case F64: {
    return sizeof(double);
  }
  case CF64: {
    return sizeof(complex float);
  }
  case CF128: {
    return sizeof(complex double);
  }
  }
}

static void print_help(char* exe)
{
  printf("\n\nUsage: %s file[start:end_nic] [dtype] [--hex|--bin|--dec] [--help]\n", exe);
  printf("Options:\n");
  printf("    file[start:end_nic]     - Input File Name. Indexing is the same as numpy array indexing. If start:end_nic aren't included, prints the entire file\n");
  printf("    dtype               - Format type to read in. Defaults to U8\n");
  printf("    --help              - Print this help menu\n");
  printf("\n");
  printf("    Print Modes: (Higher priority overrides lower priority). Defaults to dec\n");
  printf("    --hex     - Prints output in hex. Priority: 0\n");
  printf("    --bin     - Prints output in binary. Priority: 1\n");
  printf("    --dec     - Prints output in dec mode. Priority: 2\n");
  printf("\n");
  printf("input_fmt allowed options:\n");
  printf("  U8      - (%zu bytes)\n", span(U8));
  printf("  U16     - (%zu bytes)\n", span(U16));
  printf("  U32     - (%zu bytes)\n", span(U32));
  printf("  U64     - (%zu bytes)\n", span(U64));
  printf("  I8      - (%zu bytes)\n", span(I8));
  printf("  I16     - (%zu bytes)\n", span(I16));
  printf("  I32     - (%zu bytes)\n", span(I32));
  printf("  I64     - (%zu bytes)\n", span(I64));
  printf("  F32     - (%zu bytes)\n", span(F32));
  printf("  F64     - (%zu bytes)\n", span(F64));
  printf("  CF64    - (%zu bytes)\n", span(CF64));
  printf("  CF128   - (%zu bytes)\n", span(CF128));
  printf("\n\n");
  return;
}

enum mode {
  HEX = 0,
  BIN = 1,
  DEC = 2,
};

void print_bin(const void* ptr, size_t size, int little_endian)
{
  const uint8_t* bytes = (const uint8_t*)ptr;
  for (size_t i = 0; i < size; i++) {
    uint8_t byte = little_endian ? bytes[i] : bytes[size - 1 - i];
    for (int bit = 7; bit >= 0; bit--) {
      printf("%c", (byte & (1 << bit)) ? '1' : '0');
    }
    printf(" ");
  }
  printf("\n");
}

void print_hex(const void* ptr, size_t size, int little_endian)
{
  const uint8_t* bytes = (const uint8_t*)ptr;
  for (size_t i = 0; i < size; i++) {
    uint8_t byte = little_endian ? bytes[i] : bytes[size - 1 - i];
    printf("%02X ", byte);
  }
  printf("\n");
}

void print_U8(uint8_t* data, size_t i)
{
  printf("%" PRIu8 "\n", data[i]);
}

void print_U16(uint16_t* data, size_t i)
{
  printf("%" PRIu16 "\n", data[i]);
}

void print_U32(uint32_t* data, size_t i)
{
  printf("%" PRIu32 "\n", data[i]);
}

void print_U64(uint64_t* data, size_t i)
{
  printf("%" PRIu64 "\n", data[i]);
}

void print_I8(int8_t* data, size_t i)
{
  printf("%" PRId8 "\n", data[i]);
}

void print_I16(int16_t* data, size_t i)
{
  printf("%" PRId16 "\n", data[i]);
}

void print_I32(int32_t* data, size_t i)
{
  printf("%" PRId32 "\n", data[i]);
}

void print_I64(int64_t* data, size_t i)
{
  printf("%" PRId64 "\n", data[i]);
}

void print_F32(float* data, size_t i)
{
  printf("%.10f\n", data[i]);
}

void print_F64(double* data, size_t i)
{
  printf("%.10lf\n", data[i]);
}

void print_CF64(complex float* data, size_t i)
{
  float real = crealf(data[i]);
  float imag = cimagf(data[i]);

  if (imag >= 0) {
    printf("%.10f + %.10fi\n", real, imag);
  } else {
    printf("%.10f - %.10fi\n", real, -imag);
  }
}

void print_CF128(complex double* data, size_t i)
{
  double real = crealf(data[i]);
  double imag = cimagf(data[i]);

  if (imag >= 0) {
    printf("%.10f + %.10fi\n", real, imag);
  } else {
    printf("%.10f - %.10fi\n", real, -imag);
  }
}

void print_dec_mode(void* data, size_t i, enum dtype dtype)
{
  switch (dtype) {
  case U8: {
    print_U8(data, i);
    break;
  }
  case U16: {
    print_U16(data, i);
    break;
  }
  case U32: {
    print_U32(data, i);
    break;
  }
  case U64: {
    print_U64(data, i);
    break;
  }
  case I8: {
    print_I8(data, i);
    break;
  }
  case I16: {
    print_I16(data, i);
    break;
  }
  case I32: {
    print_I32(data, i);
    break;
  }
  case I64: {
    print_I64(data, i);
    break;
  }
  case F32: {
    print_F32(data, i);
    break;
  }
  case F64: {
    print_F64(data, i);
    break;
  }
  case CF64: {
    print_CF64(data, i);
    break;
  }
  case CF128: {
    print_CF128(data, i);
    break;
  }
  }
}

void print_hex_mode(void* data, size_t i, enum dtype dtype)
{
  uint8_t* _data = (uint8_t*)data;
  void* head = &_data[i * span(dtype)];
  print_hex(head, span(dtype), 1);
}

void print_bin_mode(void* data, size_t i, enum dtype dtype)
{
  uint8_t* _data = (uint8_t*)data;
  void* head = &_data[i * span(dtype)];
  print_bin(head, span(dtype), 1);
}

void print_elem(void* data, size_t i, enum dtype dtype, enum mode mode)
{
  switch (mode) {
  case DEC: {
    print_dec_mode(data, i, dtype);
    break;
  }
  case HEX: {
    print_hex_mode(data, i, dtype);
    break;
  }
  case BIN: {
    print_bin_mode(data, i, dtype);
    break;
  }
  }
}

void print_data(void* data, size_t len, enum dtype dtype, enum mode mode)
{
  for (int i = len; i < len; ++i)
    print_elem(data, i, dtype, mode);
}

typedef struct {
  enum dtype dtype;
  enum mode mode;
  char* filename;
  int64_t start;
  int64_t end_nic;
  int hex;
  int bin;
  int dec;
} cli_args;

enum mode get_priority_mode(const cli_args* args)
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

int run(const cli_args* args)
{
  assert(args);

  size_t byte_start = args->start * span(args->dtype);
  size_t byte_end_nic = args->end_nic * span(args->dtype);
  size_t num_bytes = byte_end_nic - byte_start;
  // TODO size bounds

  uint8_t* buffer = malloc(num_bytes);
  if (buffer == NULL) {
    printf("Failed to allocate memory for data buffer\n");
    // TODO - print reason
    return -1;
  }

  FILE* fp = fopen(args->filename, "rb");
  if (fp == NULL) {
    printf("Failed to open file: %s\n", args->filename);
    free(buffer);
    // TODO - print reason
    return 1;
  }
  enum mode mode = get_priority_mode(args);

  if (fseek(fp, byte_start, SEEK_CUR) != 0) {
    printf("Failed to seek file\n");
    free(buffer);
    fclose(fp);
    return 1;
  }

  size_t b_size = fread(buffer, 1, num_bytes, fp);
  if (b_size == 0) {
    printf("Number of bytes read was 0. Doing nothing\n");
    free(buffer);
    fclose(fp);
  }
  if (b_size % span(args->dtype) != 0) {
    printf("Number of bytes read: %zu is not a multiple of the data type size: %zu. Doing nothing\n", b_size, span(args->dtype));
    free(buffer);
    fclose(fp);
  }
  print_data(buffer, b_size / span(args->dtype), args->dtype, args->mode);
  return 0;
}

ssize_t first_occurence_of(char* data, char c)
{
  size_t i = 0;
  while (data[i] != '\0') {
    if (data[i] == c)
      return i;
    i++;
  }
  return -1;
}

int is_num_or_minus(char c) {
  return isdigit(c) || c == '-';
}

int parse_range(const char* range_str, int64_t* start, int64_t* end)
{
  assert(range_str);
  assert(range_str[0] == '[');
  assert(strlen(range_str) > 2);
  assert(start);
  assert(end);
  size_t i = 0;
  while(range_str[]
  return 0;
}

int verify_left_right_bracket(const char* str, ssize_t left_bracket, ssize_t right_bracket)
{
  int ret = 0;

  if (right_bracket == -1 && left_bracket != -1) {
    printf("Unterminated range string: %s\n", &str[left_bracket]);
    return -1;
  }
  if (right_bracket != -1 && left_bracket == -1) {
    printf("Invalid range string: %s\n", &str[right_bracket]);
    return -1;
  }
  if (right_bracket <= left_bracket) {
    printf("Malformed range string: %s\n", &str[right_bracket]);
    return -1;
  }
  if (right_bracket != strlen(str) - 1) {
    printf("Invalid range string: %s\n", &str[left_bracket]);
    return -1;
  }
  return 0;
}

int parse_args(int argc, char** argv, cli_args* args)
{
  assert(args);
  assert(argv);

  printf("\n\nUsage: file[start:end_nic] [dtype] [--hex|--bin|--dec] [--help]\n");

  cli_args ret = {
    .filename = NULL,
    .start = 0,
    .end_nic = 0,
    .dtype = U8
  };

  if (argc <= 2) {
    return -1;
  }

  ssize_t left_bracket = first_occurence_of(argv[1], '[');
  ssize_t right_bracket = first_occurence_of(argv[1], ']');

  if (verify_left_right_bracket(argv[1], left_bracket, right_bracket)) {
    return -1;
  }

  char* filename;
  if(left_bracket != -1) {
    filename = malloc(left_bracket + 1);
    memcpy(filename, argv[1], left_bracket);
    filename[left_bracket] = '\0';
    if(parse_range((&argv[1])[left_bracket], &ret.start, &ret.end_nic)){
      free(filename);
      return -1;
    }
  } else {
    size_t len = strlen(argv[1]) + 1;
    filename = malloc(len);
    memcpy(filename, argv[1], len);
  }
  ret.filename = filename;
}

int main(int argc, char** argv)
{
  char c;
  int ind;

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

  if (optind + 4 != argc) {
    print_help(argv[0]);
    return 1;
  }

  args.filename = argv[optind++];
  args.start = atol(argv[optind++]);
  args.end_nic = atol(argv[optind++]);

  return 0;
}
