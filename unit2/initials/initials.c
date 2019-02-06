#include <cs50.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(void)
{
    // get string
    string name = get_string();

    // main loop
    for (int i = 0, n = strlen(name); i < n; i++)
    {
        // check is alpha and it's first letter of word
        if (isalpha(name[i]) && (i == 0 || name[i - 1] == ' '))
        {
            //printf in upper case
            printf("%c", toupper(name[i]));
        }
    }
    printf("\n");
}