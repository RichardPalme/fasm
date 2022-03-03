#ifndef _SGED_GEDLIB_H
#define _SGED_GEDLIB_H

#include <string>
#include "graph.h"

typedef enum {
    F2,
    RING,
    IPFP,
    BRANCH_FAST,
    REFINE,
    BP_BEAM,
} GEDMethod;

int sged_gedlib(Graph &H, Graph &G, GEDMethod ged_method);

#endif
