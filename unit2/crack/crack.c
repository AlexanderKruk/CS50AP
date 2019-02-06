#define _XOPEN_SOURCE
#define _GNU_SOURCE
#include <unistd.h>
#include <crypt.h>

#include <stdio.h>
#include <cs50.h>
#include <string.h>
#define LENGTH 5

// initial alphabet
char const alpha[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                      'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '\0'
                     };

int main(int argc, char *argv[])
{
    // initial word for check
    char word[LENGTH + 1] = {'\0'};
    word[LENGTH] = '\0';

    // check arguments
    if (argc != 2)
    {
        printf("Usage: ./crack hash");
        return 1;
    }
    else
    {
        // initial solt
        char solt[2] = {argv[1][0], argv[1][1]};

        // 1 letter
        for (int i = 0; i < 53; i++)
        {
            word[0] = alpha[i];
            // 2 letter
            for (int j = 0; j < 53; j++)
            {
                word[1] = alpha[j];
                // 3 letter
                for (int k = 0; k < 53; k++)
                {
                    word[2] = alpha[k];
                    // 4 letter
                    for (int l = 0; l < 53; l++)
                    {
                        word[3] = alpha[l];
                        // 5 letter
                        for (int m = 0; m < 53; m++)
                        {
                            word[4] = alpha[m];
                            if (strcmp(argv[1], crypt(word, solt)) == 0)
                            {
                                printf("Password: %s\n", word);
                                return 0;
                            }
                        }
                    }
                }
            }
        }
    }
    printf("Password not found!");
    return 0;
}