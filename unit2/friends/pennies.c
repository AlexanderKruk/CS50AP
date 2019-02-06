// Calculates how many pennies a user would get if their change doubles each day using command line information

#include <cs50.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Defined constants
#define MAX_DAYS 31
#define MIN_DAYS 28

int main(int argc, char *argv[])
{
    // check count of arguments
    if (argc == 3)
    {
        // initial variable
        int days = atoi(argv[1]), pennies = atoi(argv[2]);
        long long total = pennies;

        if (pennies > 0 && (days >= MIN_DAYS && days <= MAX_DAYS))
        {
            // Add double the amount of pennies of the previous day each day
            for (int i = 1; i < days; i++)
            {
                total += pennies * pow(2, i);
            }

            // Output the total as dollars and cents only
            printf("$%.2f\n", ((double) total) / 100);
            return 0;
        }
    }

    printf("Usage: ./pennies <days> <start amount>");
    return 1;
}
