// Copies a BMP file

#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(int argc, char *argv[])
{
    int size = atoi(argv[1]), header_size = 0;

    // ensure proper usage
    if (argc != 4 || size < 0 || size >= 100)
    {
        fprintf(stderr, "Usage: resize size[0-100] infile outfile\n");
        return 1;
    }

    // remember filenames
    char *infile = argv[2];
    char *outfile = argv[3];
    int count = 0, old_pad = 0, new_pad = 0;

    // open input file
    FILE *inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }

    // open output file
    FILE *outptr = fopen(outfile, "w");
    if (outptr == NULL)
    {
        fclose(inptr);
        fprintf(stderr, "Could not create %s.\n", outfile);
        return 3;
    }

    // read infile's BITMAPFILEHEADER
    BITMAPFILEHEADER bf;
    fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

    // read infile's BITMAPINFOHEADER
    BITMAPINFOHEADER bi;
    fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

    // ensure infile is (likely) a 24-bit uncompressed BMP 4.0
    if (bf.bfType != 0x4d42 || bf.bfOffBits != 54 || bi.biSize != 40 ||
        bi.biBitCount != 24 || bi.biCompression != 0)
    {
        fclose(outptr);
        fclose(inptr);
        fprintf(stderr, "Unsupported file format.\n");
        return 4;
    }
    // old padding
    old_pad = ((4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4);

    // header size
    header_size = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // new header info
    bi.biHeight *= size;
    int oldbiWidth = bi.biWidth;
    bi.biWidth *= size;

    //new padding
    new_pad = ((4 - (bi.biWidth * sizeof(RGBTRIPLE)) % 4) % 4);

    //new file meta info
    bi.biSizeImage = ((sizeof(RGBTRIPLE) * bi.biWidth) + new_pad) * abs(bi.biHeight);
    bf.bfSize = bi.biSizeImage + header_size;

    // write outfile's BITMAPFILEHEADER
    fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

    // write outfile's BITMAPINFOHEADER
    fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

    int width = oldbiWidth + old_pad / 3;

    // iterate over infile's scanlines
    for (int i = 0, biHeight = abs(bi.biHeight); i < biHeight; i++)
    {
        // iterate over pixels in scanline
        for (int j = 0; j < width; j++)
        {
            // temporary storage
            RGBTRIPLE triple;

            // read RGB triple from infile
            fread(&triple, sizeof(RGBTRIPLE), 1, inptr);

            // write RGB triple to outfile
            if (!(triple.rgbtBlue == 0 && triple.rgbtGreen == 0 && triple.rgbtRed == 0))
            {
                for (int k = 0; k < size; k++)
                {
                    fwrite(&triple, sizeof(RGBTRIPLE), 1, outptr);
                }
            }
        }

        // then add it back (to demonstrate how)
        for (int k = 0; k < new_pad; k++)
        {
            fputc(0x00, outptr);
        }

        count++;
        //forward inptr (size - 1) time
        if (count != size)
        {
            fseek(inptr, -(sizeof(RGBTRIPLE)*width), SEEK_CUR);
        }
        else if (count == size)
        {
            count = 0;
        }

    }

    // close infile
    fclose(inptr);

    // close outfile
    fclose(outptr);

    // success
    return 0;
}
