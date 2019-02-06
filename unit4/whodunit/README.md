# Questions

## What's `stdint.h`?

Library which contains integer data types

## What's the point of using `uint8_t`, `uint32_t`, `int32_t`, and `uint16_t` in a program?

More flexible management of integer data types

## How many bytes is a `BYTE`, a `DWORD`, a `LONG`, and a `WORD`, respectively?

BYTE - 1 byte, DWORD, LONG - 4 bytes , WORD - 2 bytes

## What (in ASCII, decimal, or hexadecimal) must the first two bytes of any BMP file be? Leading bytes used to identify file formats (with high probability) are generally called "magic numbers."

0x42 0x4D

## What's the difference between `bfSize` and `biSize`?

BfSize total size of file in bytes. BiSizeImage without header

## What does it mean if `biHeight` is negative?

The bitmap is a top-down DIB with the origin at the upper left corner.

## What field in `BITMAPINFOHEADER` specifies the BMP's color depth (i.e., bits per pixel)?

biBitCount

## Why might `fopen` return `NULL` in lines 24 and 32 of `copy.c`?

Check for error in open files for read or write

## Why is the third argument to `fread` always `1` in our code?

We read 1 size of struct

## What value does line 65 of `copy.c` assign to `padding` if `bi.biWidth` is `3`?

3 bytes

## What does `fseek` do?

Move cursor to new position

## What is `SEEK_CUR`?

The current position of the cursor

## Whodunit?

Rick Astley
