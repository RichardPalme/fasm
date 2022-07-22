#ifndef _SGED_GEDLIB_TEST_H
#define _SGED_GEDLIB_TEST_H

#include "sged_gedlib.h"
#include <cassert>

void sged_gedlib_test(Graph G_1, Graph G_2) {
    int result = sged_gedlib(G_1, G_2, F2, std::vector<std::vector<double>>());
    assert(result == 0);
}

#endif
