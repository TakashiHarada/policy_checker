#ifndef FUNC_H_
#define FUNC_H_

/**
 * Writes a dot file representing the argument DDs
 * @param the node object
 */
#include <stdio.h>
#include <stdlib.h>

#ifndef CUDD_H_
#include "cudd.h"
#endif

void write_dd (DdManager *gbm,DdNode *dd, char* filename);
void print_dd (DdManager *gbm,DdNode *dd, int n, int pr, char *name);

#endif
