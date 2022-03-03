#ifndef _GI_NAUTY_TEST_H
#define _GI_NAUTY_TEST_H

#include "gi_nauty.h"
#include <cassert>

void gi_nauty_test(Graph G_1, Graph G_2) {
    assert(gi_nauty(G_1) == gi_nauty(G_2));
    G_1.add_edge(1,2,0);
    assert(gi_nauty(G_1) != gi_nauty(G_2));
}

#endif
