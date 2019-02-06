#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, string argv[])
{
    // check arguments
    if (argc != 2)
    {
        printf("Usage: ./caesar k\n");
        return 1;
    }

    int k = atoi(argv[1]);

    // get plaintext
    string text = get_string("plaintext: ");

    printf("ciphertext: ");

    //convert plaintext to ciphertext
    for (int i = 0, n = strlen(text); i < n; i++)
    {
        // if small letter
        if (text[i] >= 'a' && text[i] <= 'z')
        {
            printf("%c", (char)((((int)text[i] - 97 + k) % 26) + 97));
        }
        // if capitall letter
        else if (text[i] >= 'A' && text[i] <= 'Z')
        {
            printf("%c", (char)((((int)text[i] - 65 + k) % 26) + 65));
        }
        // if other symbols like space and etc.
        else
        {
            printf("%c", text[i]);
        }
    }
    printf("\n");
    return 0;
}