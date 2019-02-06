/**
 * recover.c
 *
 * Computer Science 50
 * Problem Set 4
 *
 * Recovers JPEGs from a forensic image.
 */

#include <stdio.h>
#include <stdlib.h>


int main(int argc, char *argv[])
{

    unsigned char buff[512];
    char filename[sizeof "000.jpg"];
    int count = 0;

    //check argc
    if (argc != 2)
    {
        printf("Usage: ./recover image\n");
        return 1;
    }

    //open file for read
    FILE *inptr = fopen("card.raw", "r");

    if (inptr == NULL)
    {
        printf("Could not open");
        return 1;
    }

    // create and name .jpg file
    sprintf(filename, "%03d.jpg", count);
    FILE *image = fopen(filename, "a");

    // read .raw file
    while (fread(&buff, 512, 1, inptr) == 1)
    {
        //search start of .jpg file
        if (buff[0] == 0xff && buff[1] == 0xd8 && buff[2] == 0xff && (buff[3] == 0xe0 || buff[3] == 0xe1))
        {
            //close file image
            fclose(image);
            //named file
            sprintf(filename, "%03d.jpg", count);
            // add data to jpeg file
            image = fopen(filename, "a");
            fwrite(buff, 512, 1, image);
            //count files for names
            count++;
        }
        //write data for .jpg
        else if (count != 0)
        {
            fwrite(buff, 512, 1, image);
        }
    }

    //close file
    fclose(inptr);
}
