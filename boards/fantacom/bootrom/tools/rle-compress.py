#!/usr/bin/env python3
"""
RLE compression tool for font data.

Compression format:
- For repeated bytes: [128-255][byte]
  (first byte value - 126) gives the compressed sequence length between 2 and 129
  The following byte contains the repeated value

- For unique sequences: [0-127][byte 0][byte 1]...[byte n]
  (first byte value + 1) gives the uncompressed sequence length between 1 and 128
  The following n bytes contain the unique values
"""

import sys
import os
import argparse


def compress_rle(data):
    """Compress data using the RLE format described above."""
    if not data:
        return bytearray()

    result = bytearray()
    i = 0
    data_len = len(data)

    while i < data_len:
        # Try to find a sequence of repeating bytes
        repeat_byte = data[i]
        repeat_count = 1
        j = i + 1
        
        # Count repeating bytes
        while j < data_len and data[j] == repeat_byte and repeat_count < 129:
            repeat_count += 1
            j += 1
            
        # If we have at least 2 repeating bytes, encode as compressed sequence
        if repeat_count >= 2:
            result.append(126 + repeat_count)
            result.append(repeat_byte)
            i += repeat_count
        else:
            # Try to find a sequence of unique bytes
            unique_start = i
            unique_count = 1
            j = i + 1
            
            # Count unique bytes (where no two adjacent bytes are the same)
            # but stop if we see a potential RLE sequence (2+ identical bytes)
            while j < data_len and unique_count < 128:
                if j + 1 < data_len and data[j] == data[j + 1]:
                    # Found a potential RLE sequence, stop here
                    break
                unique_count += 1
                j += 1
                
            # Encode as unique sequence
            result.append(unique_count - 1)
            result.extend(data[unique_start:unique_start + unique_count])
            i += unique_count

    return result


def decompress_rle(data):
    """Decompress data using the RLE format described above."""
    if not data:
        return bytearray()

    result = bytearray()
    i = 0
    data_len = len(data)

    while i < data_len:
        control = data[i]
        i += 1

        if control >= 128:
            # Compressed sequence
            repeat_count = control - 126
            repeat_byte = data[i]
            i += 1
            result.extend([repeat_byte] * repeat_count)
        else:
            # Unique sequence
            unique_count = control + 1
            result.extend(data[i:i + unique_count])
            i += unique_count

    return result


def main():
    parser = argparse.ArgumentParser(description="RLE compression tool for font data")
    parser.add_argument("input", help="Input file to compress")
    parser.add_argument("-o", "--output", help="Output file (default: input.rle)")
    parser.add_argument("-d", "--decompress", action="store_true", help="Decompress instead of compress")
    parser.add_argument("-v", "--verbose", action="store_true", help="Print detailed information")
    parser.add_argument("-t", "--test", action="store_true", help="Verify compression/decompression works correctly")

    args = parser.parse_args()

    # Default output filename
    if not args.output:
        if args.decompress:
            base, ext = os.path.splitext(args.input)
            args.output = base + ".bin"
        else:
            args.output = args.input + ".rle"

    # Read input file
    with open(args.input, "rb") as f:
        data = f.read()

    if args.decompress:
        # Decompress
        result = decompress_rle(data)
        mode = "Decompression"
    else:
        # Compress
        result = compress_rle(data)
        mode = "Compression"

    # Write output file
    with open(args.output, "wb") as f:
        f.write(result)

    # Print stats
    if args.verbose:
        compression_ratio = (len(data) - len(result)) / len(data) * 100
        print(f"Original size: {len(data)} bytes")
        print(f"New size: {len(result)} bytes")
        
        if args.decompress:
            print(f"Expansion ratio: {-compression_ratio:.2f}%")
        else:
            print(f"Compression ratio: {compression_ratio:.2f}%")

    if args.test:
        # Verify compression/decompression works
        if args.decompress:
            test_data = compress_rle(result)
            if test_data == data:
                print("Test passed: Compression of decompressed data matches original")
            else:
                print("Test failed: Compression of decompressed data doesn't match original")
        else:
            test_data = decompress_rle(result)
            if test_data == data:
                print("Test passed: Decompression of compressed data matches original")
            else:
                print("Test failed: Decompression of compressed data doesn't match original")

    print(f"{mode} complete: {args.input} → {args.output}")


if __name__ == "__main__":
    main()