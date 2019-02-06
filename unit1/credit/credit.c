#include <stdio.h>
#include <cs50.h>
#include <math.h>

int main(void)
{
    // initial variable
    long long cc_num = 0, temp_cc_num = 0;
    int count = 0, sum = 0;

    // get cc number
    do
    {
        printf("Number: ");
        cc_num = get_long();
    }
    while (cc_num <= 0);

    // get count of numbers
    temp_cc_num = cc_num;
    while (temp_cc_num > 0)
    {
        temp_cc_num /= 10;
        count++;
    }


    temp_cc_num = cc_num;
    int num = 0;

    // count sum
    while (temp_cc_num > 9)
    {
        // add first from end
        sum += temp_cc_num % 10;
        temp_cc_num /= 10;
        // myltiply 2 and add second
        num = (temp_cc_num % 10) * 2;
        // check for 2 digits multiply number
        if (num > 9)
        {
            sum += num % 10 + num / 10;
        }
        else
        {
            sum += num;
        }
        // go next number
        temp_cc_num /= 10;
    }

    sum += temp_cc_num;

    // check sum
    if (sum % 10 != 0)
    {
        printf("INVALID\n");
        return 0;
    }

    long long pow12 = pow(10, 12), pow13 = pow(10, 13), pow14 = pow(10, 14), pow15 = pow(10, 15);

    // check cc system and count numbers
    if ((count == 13 && (cc_num / pow12) == 4) || (count == 16 && (cc_num / pow15 == 4)))
    {
        printf("VISA\n");
    }
    else if (count == 15 && ((cc_num / pow13 == 34) || (cc_num / pow13 == 37)))
    {
        printf("AMEX\n");
    }
    else if (count == 16 && ((cc_num / pow14 >= 51) && (cc_num / pow14 <= 55)))
    {
        printf("MASTERCARD\n");
    }
    else
    {
        printf("INVALID\n");
    }

    return 0;
}

