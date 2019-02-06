// Prototypes for the Sort Race

#include <cs50.h>
#include <string.h>

// Returns true if str is a valid flag (-a, -b, -r, or -s), false otherwise
bool check_flag(char *str);

// Sorts array of n values using bubble sort
void bubble(int values[], int n);

// Sorts array of n values using selection sort
void selection(int values[], int n);

// Sorts array of n values using insertion sort
void insertion(int values[], int n);
