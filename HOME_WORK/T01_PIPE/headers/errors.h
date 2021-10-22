
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef ERRORS_H_INCL
#define ERRORS_H_INCL

void condPrintNExit( bool condVal, const char * errorMsg, int errnoVal );

#endif // #ifndef ERRORS_H_INCL