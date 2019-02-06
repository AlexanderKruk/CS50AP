#include <stdio.h>
#include <cs50.h>

int main(void)
{
    //initial variable
    int day = 0;
    long double sum_pennies = 0, cur_pennies = 0;

    //get and check input
    do
    {
        printf("Days in month: ");
        day = get_int();
    }
    while (day < 28 || day > 31);

    //get and check pennies on first day
    do
    {
        printf("Pennies on first day: ");
        cur_pennies = get_int();
    }
    while (cur_pennies <= 0);

    //calculate sum pennies
    for (int i = 0; i < day; i++)
    {
        sum_pennies += cur_pennies;
        cur_pennies *= 2;
    }
    //output sum of pennies
    printf("$%.2Lf\n", sum_pennies / 100);

}