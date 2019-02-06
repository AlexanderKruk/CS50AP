#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // check count of arguments
    if (argc == 4)
    {
        // initial variable
        char op  = argv[2][0];
        float value1 = atof(argv[1]), value2 = atof(argv[3]);
        int temp = 0;
        double result = 0.0;

        // check operation
        if (op == '+' || op == '-' || op == 'x' || op == '/' || op == '%')
        {
            // switch for operations
            switch (op)
            {
                case '+':
                    result = value1 + value2;
                    break;
                case '-':
                    result = value1 - value2;
                    break;
                case 'x':
                    result = value1 * value2;
                    break;
                case '/':
                    result = value1 / value2;
                    break;
                case '%':
                    temp = value1 / value2;
                    result = value1 - (value2 * temp);
                    break;
            }
            printf("%f\n", result);
            return 0;
        }
    }
    // if error
    printf("Uses: ./calc <value1> <op> <value2>\n");
    return 1;
}