#include <cs50.h>
#include <stdio.h>

int main(void)
{
    //initial var
    float c = 0.0, f = 0.0;

    // get value C:
    printf("C: ");
    c = get_float();

    //calculate f
    f = ((c * 9) / 5) + 32;
    //print f
    printf("F: %.1f\n", f);

}