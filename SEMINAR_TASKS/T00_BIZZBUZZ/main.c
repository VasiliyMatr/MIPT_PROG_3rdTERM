
#include <stdio.h>
#include <ctype.h>

enum argProp_t {

    NOT_A_NUMBER    =  'N', // this argument is not a number

  // this argument is a number

    DIVISIBLE_BY_3  =  '3', // this number is divisible by 3
    DIVISIBLE_BY_5  =  '5', // this number is divisible by 5
    DIVISIBLE_BY_15 =  'B', // this number is divisible by 3 and 5
    COMMON          =  'C'  // this number is not divisible by 3 or 5

};

enum argProp_t parseArg( const char * arg );

int main( int argc, char** argv )
{
    for (size_t argId = 1; argId < argc; argId++)
    {
        enum argProp_t argProp = parseArg (argv[argId]);

        switch (argProp)
        {
            case NOT_A_NUMBER:
            case COMMON:
                printf ("%s ", argv[argId]);
                break;
            
            case DIVISIBLE_BY_3:
                printf ("BIZZ ");
                break;
            case DIVISIBLE_BY_5:
                printf ("BUZZ ");
                break;

            case DIVISIBLE_BY_15:
                printf ("BIZZ-BUZZ ");
                break;

            default:
                fprintf (stderr,
                    "ERROR WITH argProp_t VALUE: FILE=%s LINE=%d VALUE=%d" "\n",
                    __FILE__, __LINE__, argProp);
                return -1;
        }
    }

    printf ("\n");

    return 0;
}

enum argProp_t parseArg( const char * arg )
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

    if (symbol != '\0' || symbolId == 1)
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
