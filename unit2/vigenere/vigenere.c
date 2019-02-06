#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main(int argc, string argv[])
{

    char crypto(int key_len, bool capital);

    // check parametrs
    if (argc != 2)
    {
        printf("Usage: ./vigenere k\n");
        return 1;
    }
    // check if k is only alpha
    for (int i = 0, n = strlen(argv[1]); i < n; i++)
    {
        if (!(isalpha(argv[1][i])))
        {
            printf("Usage: ./vigenere k\n");
            return 1;
        }
    }


    string key = argv[1];
    int key_len = strlen(key), key_cur;

    // get plaintext
    printf("plaintext: ");
    string text = get_string();

    printf("ciphertext: ");

    for (int i = 0, n = strlen(text), j = 0; i < n; i++)
    {
        // for lowercase
        if (islower(text[i]))
        {
            key_cur = (int)(tolower(key[j % key_len])) - 97;
            printf("%c", (char)((((int)text[i] - 97 + key_cur) % 26) + 97));
            j++;
        }
        // for uppercase
        else if (isupper(text[i]))
        {
            key_cur = (int)(tolower(key[j % key_len])) - 97;
            printf("%c", (char)((((int)text[i] - 65 + key_cur) % 26) + 65));
            j++;
        }
        else
        {
            // for space
            printf("%c", text[i]);
        }
    }
    printf("\n");
    return 0;
}
