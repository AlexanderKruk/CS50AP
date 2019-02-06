#include <stdio.h>
#include <cs50.h>

int main(void)
{
    //initial variable
    long long isbn = 0;
    int checksum = 0, sum = 0, cur = 0;

    //get positive isbn
    do
    {
        printf("ISBN: ");
        isbn = get_long();
    }
    while (isbn <= 0);

    //calculate checksum
    checksum = isbn % 10;

    //calculate sum
    for (int i = 9; i > 0; i--)
    {
        isbn /= 10;
        cur = isbn % 10;
        sum += cur * i;
    }

    //check checksum
    if (sum % 11 == checksum)
    {
        printf("YES\n");
    }
    else
    {
        printf("NO\n");
    }

    return 0;
}