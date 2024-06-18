#include <assert.h>
#include <complex.h>
#include <ctype.h>
#include <getopt.h>
#include <inttypes.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

enum dtype {
  CHAR,
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

// Raw arguments from the command line - unchanged
typedef struct {
  enum dtype dtype;
  char* filename;
  char* span_str;
  int hex;
  int bin;
  int dec;
  int little;
  int big;
} cli_args;

enum endianess {
  LITTLE,
  BIG,
};

enum mode {
  HEX = 0,
  BIN = 1,
  DEC = 2,
};

// Transformed arguments with validations
typedef struct {
  enum dtype dtype;
  enum mode mode;
  enum endianess endianess;
  char* filename;
  size_t elem_start;
  size_t elem_end;
} validated_cli_args;

// Alias for sizeof
size_t span(enum dtype dtype)
{
  switch (dtype) {
  case CHAR: {
    return sizeof(char);
  }
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
  printf("\nUsage: %s file [start:end] [dtype] [--hex|--bin|--dec] [--little|--big] [--help]\n", exe);
  printf("Options:\n");
  printf("    file                - Input file name.\n");
  printf("    [start:end]         - Indexing has the same behavior as numpy array indexing. If start:end aren't included, prints the entire file\n");
  printf("    dtype               - Format type to read in. Defaults to U8\n");
  printf("    --little            - Reads in little endian\n");
  printf("    --big               - Reads in big endian\n");
  printf("    --help              - Print this help menu\n");
  printf("\n");
  printf("    Print Modes: (Higher priority overrides lower priority). Defaults to dec\n");
  printf("    --hex               - Prints output in hex. Priority: 0\n");
  printf("    --bin               - Prints output in binary. Priority: 1\n");
  printf("    --dec               - Prints output in dec mode. Priority: 2\n");
  printf("\n");
  printf("input_fmt allowed options:\n");
  printf("  CHAR                  - (%zu bytes)\n", span(CHAR));
  printf("  U8                    - (%zu bytes)\n", span(U8));
  printf("  U16                   - (%zu bytes)\n", span(U16));
  printf("  U32                   - (%zu bytes)\n", span(U32));
  printf("  U64                   - (%zu bytes)\n", span(U64));
  printf("  I8                    - (%zu bytes)\n", span(I8));
  printf("  I16                   - (%zu bytes)\n", span(I16));
  printf("  I32                   - (%zu bytes)\n", span(I32));
  printf("  I64                   - (%zu bytes)\n", span(I64));
  printf("  F32                   - (%zu bytes)\n", span(F32));
  printf("  F64                   - (%zu bytes)\n", span(F64));
  printf("  CF64                  - (%zu bytes)\n", span(CF64));
  printf("  CF128                 - (%zu bytes)\n", span(CF128));
  printf("\n");
  return;
}

// Prints data in binary format with a space in between each byte
void print_bin(const void* ptr, size_t size, enum endianess endianess)
{
  const uint8_t* bytes = (const uint8_t*)ptr;
  for (size_t i = 0; i < size; i++) {
    uint8_t byte = endianess == LITTLE ? bytes[i] : bytes[size - 1 - i];
    for (int bit = 7; bit >= 0; bit--) {
      printf("%c", (byte & (1 << bit)) ? '1' : '0');
    }
    printf(" ");
  }
  printf("\n");
}

// Prints data in hex format byte by byte with a space in between each byte
void print_hex(const void* ptr, size_t size, enum endianess endianess)
{
  const uint8_t* bytes = (const uint8_t*)ptr;
  for (size_t i = 0; i < size; i++) {
    uint8_t byte = endianess == LITTLE ? bytes[i] : bytes[size - 1 - i];
    printf("%02X ", byte);
  }
  printf("\n");
}

void print_CHAR(char* data, size_t i, enum endianess endianess)
{
  printf("%c\n", data[i]);
}

void print_U8(uint8_t* data, size_t i, enum endianess endianess)
{
  printf("%" PRIu8 "\n", data[i]);
}

void print_U16(uint16_t* data, size_t i, enum endianess endianess)
{
  if(endianess == LITTLE){
    printf("%" PRIu16 "\n", htole16(data[i]));
  } else {
    printf("%" PRIu16 "\n", htobe16(data[i]));
  }
}

void print_U32(uint32_t* data, size_t i, enum endianess endianess)
{
  if(endianess == LITTLE){
    printf("%" PRIu32 "\n", htole32(data[i]));
  } else {
    printf("%" PRIu32 "\n", htobe32(data[i]));
  }
}

void print_U64(uint64_t* data, size_t i, enum endianess endianess)
{
  if(endianess == LITTLE){
    printf("%" PRIu64 "\n", htole64(data[i]));
  } else {
    printf("%" PRIu64 "\n", htobe64(data[i]));
  }
}

void print_I8(int8_t* data, size_t i, enum endianess endianess)
{
  printf("%" PRId8 "\n", data[i]);
}

void print_I16(int16_t* data, size_t i, enum endianess endianess)
{
  if(endianess == LITTLE){
    printf("%" PRId16 "\n", htole16(data[i]));
  } else {
    printf("%" PRId16 "\n", htobe16(data[i]));
  }
}

void print_I32(int32_t* data, size_t i, enum endianess endianess)
{
  if(endianess == LITTLE){
    printf("%" PRId32 "\n", htole32(data[i]));
  } else {
    printf("%" PRId32 "\n", htobe32(data[i]));
  }
}

void print_I64(int64_t* data, size_t i, enum endianess endianess)
{
  if(endianess == LITTLE){
    printf("%" PRId64 "\n", htole64(data[i]));
  } else {
    printf("%" PRId64 "\n", htobe64(data[i]));
  }
}

float fhtole32(const float i) {
  uint32_t int_value;
  float ret;
  memcpy(&int_value, &i, sizeof ret);
  int_value = htole32(int_value);
  memcpy(&ret, &int_value, sizeof ret);
  return ret;
}

float fhtobe32(const float i) {
  uint32_t int_value;
  float ret;
  memcpy(&int_value, &i, sizeof ret);
  int_value = htobe32(int_value);
  memcpy(&ret, &int_value, sizeof ret);
  return ret;
}

void print_F32(float* data, size_t i, enum endianess endianess)
{
  if(endianess == LITTLE){
    printf("%f\n", fhtole32(data[i]));
  } else {
    printf("%f\n", fhtobe32(data[i]));
  }
}

float fhtole64(const float i) {
  uint64_t int_value;
  float ret;
  memcpy(&int_value, &i, sizeof ret);
  int_value = htole64(int_value);
  memcpy(&ret, &int_value, sizeof ret);
  return ret;
}

float fhtobe64(const float i) {
  uint64_t int_value;
  float ret;
  memcpy(&int_value, &i, sizeof ret);
  int_value = htobe64(int_value);
  memcpy(&ret, &int_value, sizeof ret);
  return ret;
}

void print_F64(double* data, size_t i, enum endianess endianess)
{
  if(endianess == LITTLE){
    printf("%lf\n", fhtole64(data[i]));
  } else {
    printf("%lf\n", fhtobe64(data[i]));
  }
}

void print_CF64(complex float* data, size_t i, enum endianess endianess)
{
  float real, imag;
  if(endianess == LITTLE){
    real = fhtole32(creal(data[i]));
    imag = fhtole32(cimag(data[i]));
  } else {
    real = fhtobe32(creal(data[i]));
    imag = fhtobe32(cimag(data[i]));
  }

  if (imag >= 0) {
    printf("%f + %fi\n", real, imag);
  } else {
    printf("%f - %fi\n", real, -imag);
  }
}

void print_CF128(complex double* data, size_t i, enum endianess endianess)
{
  double real, imag;
  if(endianess == LITTLE){
    real = fhtole64(creal(data[i]));
    imag = fhtole64(cimag(data[i]));
  } else {
    real = fhtobe64(creal(data[i]));
    imag = fhtobe64(cimag(data[i]));
  }

  if (imag >= 0) {
    printf("%lf + %lfi\n", real, imag);
  } else {
    printf("%lf - %lfi\n", real, -imag);
  }
}

void print_dec_mode(void* data, size_t i, enum dtype dtype, enum endianess endianess)
{
  switch (dtype) {
  case CHAR: {
    print_CHAR(data, i, endianess);
    break;
  }
  case U8: {
    print_U8(data, i, endianess);
    break;
  }
  case U16: {
    print_U16(data, i, endianess);
    break;
  }
  case U32: {
    print_U32(data, i, endianess);
    break;
  }
  case U64: {
    print_U64(data, i, endianess);
    break;
  }
  case I8: {
    print_I8(data, i, endianess);
    break;
  }
  case I16: {
    print_I16(data, i, endianess);
    break;
  }
  case I32: {
    print_I32(data, i, endianess);
    break;
  }
  case I64: {
    print_I64(data, i, endianess);
    break;
  }
  case F32: {
    print_F32(data, i, endianess);
    break;
  }
  case F64: {
    print_F64(data, i, endianess);
    break;
  }
  case CF64: {
    print_CF64(data, i, endianess);
    break;
  }
  case CF128: {
    print_CF128(data, i, endianess);
    break;
  }
  }
}

void print_hex_mode(void* data, size_t i, enum dtype dtype, enum endianess endianess)
{
  uint8_t* _data = (uint8_t*)data;
  void* head = &_data[i * span(dtype)];
  print_hex(head, span(dtype), endianess);
}

void print_bin_mode(void* data, size_t i, enum dtype dtype, enum endianess endianess)
{
  uint8_t* _data = (uint8_t*)data;
  void* head = &_data[i * span(dtype)];
  print_bin(head, span(dtype), endianess);
}

void print_elem(void* data, size_t i, enum dtype dtype, enum mode mode, enum endianess endianess)
{
  switch (mode) {
  case DEC: {
    print_dec_mode(data, i, dtype, endianess);
    break;
  }
  case HEX: {
    print_hex_mode(data, i, dtype, endianess);
    break;
  }
  case BIN: {
    print_bin_mode(data, i, dtype, endianess);
    break;
  }
  }
}

void print_data(void* data, size_t len, enum dtype dtype, enum mode mode, enum endianess endianess)
{
  for (int i = 0; i < len; ++i)
    print_elem(data, i, dtype, mode, endianess);
}


typedef struct {
  uint8_t* data;
  size_t data_len_bytes;
  size_t data_capacity_bytes;
} file_read;

enum endianess get_system_endianess()
{
  volatile uint32_t i = 0x01234567;
  return (*((uint8_t*)(&i))) == 0x67 ? LITTLE : BIG;
}

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

enum endianess get_endianess(const cli_args* args)
{
  if (args->little)
    return LITTLE;
  if (args->big)
    return BIG;
  return get_system_endianess();
}

ssize_t get_file_size_in_bytes(const char* file_name)
{
  FILE* fp = fopen(file_name, "r");

  if (fp == NULL) {
    printf("File Not Found\n");
    return -1;
  }

  if (fseek(fp, 0L, SEEK_END) != 0) {
    printf("Failed to seek file\n");
    fclose(fp);
    return -1;
  }

  long int res = ftell(fp);

  fclose(fp);

  return res;
}

ssize_t first_occurence_of(const char* data, char c)
{
  assert(data);
  size_t i = 0;
  while (data[i] != '\0') {
    if (data[i] == c)
      return i;
    i++;
  }
  return -1;
}

size_t unsign_index(const int64_t index, const size_t data_len)
{
  if (index < 0) {
    if (labs(index) > data_len)
      return 0;
    return index + data_len;
  }
  if (index > data_len)
    return data_len;
  return index;
}

void print_index_error(const int64_t index, const size_t data_len)
{
  printf("index %ld is out of bounds for size %lu\n", index, data_len);
}

ssize_t unsign_index_bounds_check(const int64_t index, const size_t data_len)
{
  if (index < 0) {
    if (labs(index) > data_len) {
      print_index_error(index, data_len);
      return -1;
    }
    return index + data_len;
  }
  if (index >= data_len) {
    print_index_error(index, data_len);
    return -1;
  }
  return index;
}

int parse_range(char* arg, size_t* start, size_t* end, size_t num_elements)
{
  size_t _start = 0;
  size_t _end = 0;

  if (arg == NULL) {
    *start = 0;
    *end = num_elements;
    return 0;
  }

  ssize_t colon = first_occurence_of(arg, ':');

  if (colon == -1) {
    _start = unsign_index_bounds_check(atol(arg), num_elements);
    if (_start == -1)
      return -1;
    _end = _start + 1;
  } else {
    if (arg[colon + 1] == '\0') {
      _end = num_elements;
    } else {
      _end = unsign_index(atol(&arg[colon + 1]), num_elements);
    }
    arg[colon] = '\0';

    if (arg[0] == '\0') {
      _start = 0;
    } else {
      _start = unsign_index(atol(arg), num_elements);
    }
  }

  if(_end <= _start) {
    _start = _end;
  }

  *start = _start;
  *end = _end;
  return 0;
}

int cli_args_to_validated_cli_args(validated_cli_args* dest, const cli_args* args)
{
  assert(dest);
  assert(args);

  dest->filename = args->filename;
  dest->dtype = args->dtype;
  dest->mode = get_priority_mode(args);
  dest->endianess = get_endianess(args);

  ssize_t file_size = get_file_size_in_bytes(dest->filename);
  if (file_size < 0)
    return -1;
  if (file_size % span(dest->dtype) != 0) {
    printf("File size: %lu is not a multiple of element size: %lu\n",
        file_size, span(dest->dtype));
    return -1;
  }

  size_t num_elems = file_size / span(dest->dtype);

  if (parse_range(args->span_str, &dest->elem_start, &dest->elem_end, num_elems))
    return -1;

  return 0;
}

void print_args(cli_args* args)
{
  printf("Datatype = %d\n", args->dtype);
  printf("Filename = %s\n", args->filename);
  printf("Little Endian = %d\n", args->little);
  printf("Big Endian = %d\n", args->big);
  printf("Range String = %s\n", args->span_str);
  printf("hex = %d\n", args->hex);
  printf("bin = %d\n", args->bin);
  printf("dec = %d\n", args->dec);
}

void print_val_args(validated_cli_args* args)
{
  printf("Datatype = %d\n", args->dtype);
  printf("Filename = %s\n", args->filename);
  printf("Endian = %d\n", args->endianess);
  printf("mode = %d\n", args->mode);
  printf("start = %lu\n", args->elem_start);
  printf("end = %lu\n", args->elem_end);
}

int run(const validated_cli_args* args)
{
  size_t num_elems = args->elem_end - args->elem_start;
  size_t elem_size = span(args->dtype);
  size_t byte_size = num_elems * elem_size;

  uint8_t* buffer = malloc(byte_size);
  if (buffer == NULL) {
    printf("Failed to allocate memory for data buffer\n");
    return -1;
  }

  FILE* fp = fopen(args->filename, "r");
  assert(fp != NULL);
  fseek(fp, elem_size * args->elem_start, SEEK_SET);

  size_t read_size = fread(buffer, 1, byte_size, fp);
  assert(read_size == byte_size);
  fclose(fp);

  print_data(buffer, byte_size / span(args->dtype), args->dtype, args->mode, args->endianess);
  free(buffer);
  return 0;
}

int is_num_or_minus(char c)
{
  return isdigit(c) || c == '-';
}

int is_dec(const char* str)
{
  const char* c = str;
  if (*c == '-')
    c++;
  while (*c != '\0') {
    if (!isdigit(*c))
      return 0;
    c++;
  }
  return 1;
}

int valid_range_str(const char* rng)
{
  ssize_t colon = first_occurence_of(rng, ':');
  if (colon == -1)
    return is_dec(rng);

  char* ret = malloc(strlen(rng) + 1);
  if (ret == NULL)
    return -1;

  strcpy(ret, rng);
  int end_valid = is_dec(&ret[colon + 1]);
  ret[colon] = '\0';
  int start_valid = is_dec(ret);

  free(ret);
  return end_valid && start_valid;
}

int parse_arg(cli_args* ret, char* arg)
{
  assert(ret);
  assert(arg);

  if (strcmp(arg, "--hex") == 0) {
    ret->hex = 1;
  } else if (strcmp(arg, "--bin") == 0) {
    ret->bin = 1;
  } else if (strcmp(arg, "--dec") == 0) {
    ret->dec = 1;
  } else if (strcmp(arg, "--little") == 0) {
    ret->little = 1;
  } else if (strcmp(arg, "--big") == 0) {
    ret->big = 1;
  } else if (strcmp(arg, "CHAR") == 0) {
    ret->dtype = CHAR;
  } else if (strcmp(arg, "U8") == 0) {
    ret->dtype = U8;
  } else if (strcmp(arg, "U16") == 0) {
    ret->dtype = U16;
  } else if (strcmp(arg, "U32") == 0) {
    ret->dtype = U32;
  } else if (strcmp(arg, "U64") == 0) {
    ret->dtype = U64;
  } else if (strcmp(arg, "I8") == 0) {
    ret->dtype = I8;
  } else if (strcmp(arg, "I16") == 0) {
    ret->dtype = I16;
  } else if (strcmp(arg, "I32") == 0) {
    ret->dtype = I32;
  } else if (strcmp(arg, "I64") == 0) {
    ret->dtype = I64;
  } else if (strcmp(arg, "F32") == 0) {
    ret->dtype = F32;
  } else if (strcmp(arg, "F64") == 0) {
    ret->dtype = F64;
  } else if (strcmp(arg, "CF64") == 0) {
    ret->dtype = CF64;
  } else if (strcmp(arg, "CF128") == 0) {
    ret->dtype = CF128;
  } else if (strcmp(arg, "--help") == 0) {
    return -1;
  } else if (valid_range_str(arg)) {
    ret->span_str = arg;
  } else {
    printf("Unrecognized argument: %s\n", arg);
    return -1;
  }
  return 0;
}

int parse_args(int argc, char** argv, cli_args* args)
{
  assert(args);
  assert(argv);

  cli_args ret = {
    .filename = NULL,
    .span_str = NULL,
    .dtype = U8,
    .hex = 0,
    .bin = 0,
    .dec = 0,
    .little = 0,
    .big = 0,
  };

  if (argc < 2) {
    printf("Filename is required: %s filename ...\n", argv[0]);
    return -1;
  }
  if(strcmp(argv[1], "--help") == 0){
    print_help(argv[0]);
    return -1;
  }

  ret.filename = argv[1];

  for (int i = 2; i < argc; ++i)
    if (parse_arg(&ret, argv[i]))
      return -1;

  memcpy(args, &ret, sizeof ret);
  return 0;
}

int main(int argc, char** argv)
{
  cli_args args;
  validated_cli_args val_args;

  if (parse_args(argc, argv, &args)) {
    printf("Run with --help for detailed arguments\n");
    return 1;
  }

  if (cli_args_to_validated_cli_args(&val_args, &args)) {
    printf("Run with --help for detailed arguments\n");
    return 1;
  }

  return run(&val_args);
}
