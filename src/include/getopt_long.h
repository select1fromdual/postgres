
#ifndef GETOPT_LONG_H
#define GETOPT_LONG_H

#include "pg_getopt.h"

#ifndef HAVE_STRUCT_OPTION

struct option {
  const char *name;
  int has_arg;
  int *flag;
  int val;
};

#define no_argument 0
#define required_argument 1
#define optional_argument 2
#endif

#ifndef HAVE_GETOPT_LONG

extern int getopt_long(int argc, char *const argv[], const char *optstring, const struct option *longopts,
                       int *longindex);
#endif

#endif /* GETOPT_LONG_H */
