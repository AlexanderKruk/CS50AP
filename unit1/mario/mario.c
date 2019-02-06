#include <cs50.h>
#include <stdio.h>

int main(void)
{
    int height = 0;

    // get height
    do
    {
        printf("height: ");
        height = get_int();
    }
    while (height < 0 || height > 23);

    // main loop
    for (int i = 1; i <= height; i++)
    {
        // left colomn
        for (int j = 1; j <= height; j++)
        {
            if (height - j - i >= 0)
            {
                printf(" ");
            }
            else
            {
                printf("#");
            }
        }
        // center
        printf("  ");
        // right colomn
        for (int j = 1; j <= height; j++)
        {
            if (j <= i)
            {
                printf("#");
            }
            else
            {
                break;
            }
        }
        printf("\n");
    }
}