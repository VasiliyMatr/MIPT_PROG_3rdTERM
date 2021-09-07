
#include <stdio.h>
#include <ctype.h>

int main( int argc, char** argv )
{
    for (size_t argId = 1; argId < argc; argId++)
    {
        int number = 0;
        int scanOut = 0;

        if ((scanOut = sscanf (argv[argId], " %d ", &number)) && scanOut != EOF)
        {
            if (number % 15 == 0)
                printf ("BizzBuzz ");
            else if (number % 5 == 0)
                printf ("Buzz ");
            else if (number % 3 == 0)
                printf ("Bizz ");
            else
                printf ("%s ", argv[argId]);

                continue;
        }

            printf ("%s ", argv[argId]);
    }

    printf ("\n");

    return 0;
}

enum argProp_t {

    NOT_A_NUMBER    =  'N', // this argument is not a number

  // this argument is a number

    DIVISIBLE_BY_3  =  '3', // this number is divisible by 3
    DIVISIBLE_BY_5  =  '5', // this number is divisible by 5
    DIVISIBLE_BY_15 =  'B', // this number is divisible by 3 and 5
    COMMON          =  'C'  // this number is not divisible by 3 or 5

};

enum argProp_t parseArg (const char * arg)
{
    __uint64_t digitsSum = 0;
    size_t symbolId = 0;
    char symbol = '\0';

    while (symbol = arg[symbolId++])
    {
        if (!isdigit (symbol))
            break;

        digitsSum += symbol - '0';
    }

    if (symbol != '0' || symbolId == 1)
        return NOT_A_NUMBER;

    char lastDigit = arg[symbolId - 2];

    if (digitsSum % 3 == 0)
    {
        if (lastDigit == '0' || lastDigit == '5')
            return DIVISIBLE_BY_15;
        
        return DIVISIBLE_BY_3;
    }

    if (lastDigit == '0' || lastDigit == '5')
        return DIVISIBLE_BY_5;

    return COMMON;
}
