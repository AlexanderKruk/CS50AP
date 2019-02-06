// Greets a user by their name

#include <cs50.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    // check for count of arguments
    if (argc != 2)
    {
        // if argument not 2
        printf("Usage: ./hello <name>\n");
        return 1;
    }
    else
    {
        // if 2 arguments
        printf("Hello, %s!\n", argv[1]);
    }

}
