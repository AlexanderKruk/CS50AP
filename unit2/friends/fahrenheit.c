// Converts degrees C to degrees F by collecting info at command line

#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // check corectness argument
    if (argc != 2)
    {
        // if argement not 2 return error
        printf("Usage: ./fahrenheit <Celsius>");
        return 1;
    }
    else
    {
        // Convert to fahrenheit; no float errors b/c celsius is a float
        float celsius = atof(argv[1]);
        float fahrenheit = (celsius * 9) / 5 + 32;
        printf("F: %.1f\n", fahrenheit);
    }
}
