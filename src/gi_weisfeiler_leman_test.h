#ifndef _GI_WEISFEILER_LEMAN_TEST_H
#define _GI_WEISFEILER_LEMAN_TEST_H

#include "gi_weisfeiler_leman.h"
#include <cassert>

// Note that Weisfeiler-Leman is a heuristic algorithm, so be careful with
// assertions about its result
void gi_weisfeiler_leman_test(Graph G_1, Graph G_2) {
    assert(gi_weisfeiler_leman(G_1) == gi_weisfeiler_leman(G_2));
    G_1.add_edge(1,2,0);
    assert(gi_weisfeiler_leman(G_1) != gi_weisfeiler_leman(G_2));
}

#endif
