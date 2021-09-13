
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>

#ifndef TRANS_H_INCL
#define TRANS_H_INCL

#include <err.h>

struct errStatus_t transByDescr( int inDescr, int outDescr );
struct errStatus_t transByArgs( int argc, char ** argv );

#endif // #ifndef TRANS_H_INCL