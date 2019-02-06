/**
 * helpers.c
 *
 * Computer Science 50
 * Problem Set 3
 *
 * Helper functions for Problem Set 3.
 */

#include <cs50.h>

#include "helpers.h"

/**
 * Returns true if value is in array of n values, else false.
 */
bool search(int value, int values[], int n)
{
    // if size array is <= 0
    if (n <= 0)
    {
        return false;
    }

    // search value in array
    for (int i = 0; i <= n; i++)
    {
        if (values[i] == value)
        {
            return true;
        }
    }
    return false;
}

/**
 * Sorts array of n values.
 */
void sort(int values[], int n)
{
    // TODO: implement an O(n^2) sorting algorithm
    int count, temp;

    // bubble sort
    do
    {
        count = 0;
        for (int i = 0; i < n - 1; i++)
        {
            // if left bigger then right swap
            if (values[i] > values[i + 1])
            {
                temp = values[i];
                values[i] = values[i + 1];
                values[i + 1] = temp;
                count++;
            }
        }

        // while swap not  equals 0
    }
    while (count > 0);
}