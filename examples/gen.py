import numpy as np

def generate_and_save_random_data():
    data_types = [
        (np.uint8, "U8.bin", 0, 255),
        (np.uint16, "U16.bin", 0, 65535),
        (np.uint32, "U32.bin", 0, 4294967295),
        (np.uint64, "U64.bin", 0, 18446744073709551615),
        (np.int8, "I8.bin", -128, 127),
        (np.int16, "I16.bin", -32768, 32767),
        (np.int32, "I32.bin", -2147483648, 2147483647),
        (np.int64, "I64.bin", -9223372036854775808, 9223372036854775807),
        (np.float32, "F32.bin", None, None),
        (np.float64, "F64.bin", None, None),
    ]
    
    for dtype, filename, min_val, max_val in data_types:
        if np.issubdtype(dtype, np.floating):
            data = np.random.randn(100).astype(dtype)
        else:
            data = np.random.randn(100)
            data = (data - data.min()) / (data.max() - data.min())
            data = (data * (max_val - min_val) + min_val).astype(dtype)
        data.tofile(filename)
        print(filename, data[0:5])
    
    complex_data_types = [
        (np.csingle, "CF64.bin"),
        (np.cdouble, "CF128.bin")
    ]
    
    for dtype, filename in complex_data_types:
        real_part = np.random.randn(100)
        imag_part = np.random.randn(100)
        complex_data = (real_part + imag_part * 1j).astype(dtype)
        complex_data.tofile(filename)
        print(filename, complex_data[0:5])

generate_and_save_random_data()
