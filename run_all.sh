#!/bin/bash

./bb ./examples/U8.bin U8 :5 --little
./bb ./examples/U16.bin U16 :5 --little
./bb ./examples/U32.bin U32 :5 --little
./bb ./examples/U64.bin U64 :5 --little
./bb ./examples/I8.bin I8 :5 --little
./bb ./examples/I16.bin I16 :5 --little
./bb ./examples/I32.bin I32 :5 --little
./bb ./examples/I64.bin I64 :5 --little
./bb ./examples/F32.bin F32 :5 --little
./bb ./examples/F64.bin F64 :5 --little
./bb ./examples/CF64.bin CF64 :5 --little
./bb ./examples/CF128.bin CF128 :5 --little


./bb ./examples/U8.bin U8 :5 --big
./bb ./examples/U16.bin U16 :5 --big
./bb ./examples/U32.bin U32 :5 --big
./bb ./examples/U64.bin U64 :5 --big
./bb ./examples/I8.bin I8 :5 --big
./bb ./examples/I16.bin I16 :5 --big
./bb ./examples/I32.bin I32 :5 --big
./bb ./examples/I64.bin I64 :5 --big
./bb ./examples/F32.bin F32 :5 --big
./bb ./examples/F64.bin F64 :5 --big
./bb ./examples/CF64.bin CF64 :5 --big
./bb ./examples/CF128.bin CF128 :5 --big
