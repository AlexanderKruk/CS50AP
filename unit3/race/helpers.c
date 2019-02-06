// Helper functions for the sort race

#include <cs50.h>
#include "helpers.h"

// Returns true if str is a valid flag (-a, -b, -r, or -s), false otherwise
bool check_flag(char *str)
{
    if (str[0] == '-' && (str[1] == 'a' || str[1] == 'b' || str[1] == 'r' || str[1] == 's') && str[2] == '\0')
    {
        return true;
    }
    return false;
}

// Sorts array of n values using bubble sort
void bubble(int values[], int n)
{
    // count of swap
    int count = 0, temp = 0;

    do
    {
        count = 0;
        // itteration all array
        for (int i = 0; i < n; i++)
        {
            // if left is bigger then right
            if (values[i] > values[i + 1])
            {
                //change left and right values
                temp = values[i];
                values[i] = values[i + 1];
                values[i + 1] = temp;
                // count swap
                count++;
            }
        }
        // while swap not 0
    }
    while (count > 0);
}

// Sorts array of n values using selection sort
void selection(int values[], int n)
{
    // initial variable
    int min = 0, min_index = 0, temp = 0;

    // sorted array
    for (int j = 0; j < n - 1; j++)
    {
        min = values[j];
        // unsorted array
        for (int i = j + 1; i < n; i++)
        {
            // search min value
            if (min > values[i])
            {
                min = values[i];
                min_index = i;
            }
        }
        // add min value in unsorted to sorted array
        if (min != values[j])
        {
            temp = values[j];
            values[j] = values[min_index];
            values[min_index] = temp;
        }
    }
}

// Sorts array of n values using insertion sort
void insertion(int values[], int n)
{
    int temp = 0;

    // itteration array
    for (int i = 0, j = 0; i < n; i++)
    {
        temp = values[i];
        j = i;
        // search place to temp values and move other values to the right
        while (j > 0 && values[j - 1] > temp)
        {
            values[j] = values[j - 1];
            j--;
        }
        values[j] = temp;
    }
}
