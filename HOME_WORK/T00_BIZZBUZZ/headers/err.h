
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>

#ifndef ERR_H_INCL
#define ERR_H_INCL

// error codes enum for bizz-buzz functionality (EC = error code)
enum errCode_t
{
  // no errors
    EC_OK_        = 0,

  // undefined error code value - for init
    EC_UNDEF_ERR_ = -0xffff,

  // fatal errors
    EC_BAD_PTR_,
    EC_BAD_FILE_DESCR_,

  // args errors
    EC_2MANY_ARGS_,
    EC_2FEW_ARGS_,

  // input file errors
    EC_IN_ERR_,

  // output file errors
    EC_OUT_ERR_,
};

static const char EM_OK_[] =
    "No errors.";
static const char EM_UNDEF_ERR_[] =
    "Undefined error code. Please, contact developer.";
static const char EM_2MANY_ARGS_[] =
    "Too many arguments.";
static const char EM_2FEW_ARGS_[] =
    "Too few arguments.";
static const char EM_IN_ERR_[] =
    "Error with input file.";
static const char EM_OUT_ERR_[] =
    "Error with output file.";

struct errStatus_t
{
    enum errCode_t errCode_;
    int64_t errno_;
};

const char* getErrMsg( enum errCode_t errCode );

int isBadPtr( void* ptr );

#endif // #ifndef ERR_H_INCL