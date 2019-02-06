#include <cs50.h>
#include <stdio.h>
#include <math.h>

int main(void)
{
    //initial variable
    float dollar = 0.0;
    int count = 0;
    int cent = 0;

    // Input data
    do
    {
        printf("Change owed: ");
        dollar = get_float();
    }
    while (dollar <= 0);

    //Convert to cent with round
    cent = roundf(dollar * 100.00);

    //Count coins
    while (cent > 0)
    {
        if (cent >= 25)
        {
            cent -= 25;
        }
        else if (cent >= 10)
        {
            cent -= 10;
        }
        else if (cent >= 5)
        {
            cent -= 5;
        }
        else if (cent >= 1)
        {
            cent -= 1;
        }
        count++;
    }
    //print count
    printf("%i\n", count);
}