#include "graph.h"
#include "graph_test.h"
#include "sged_gedlib_test.h"
#include "gi_nauty_test.h"
#include "gi_weisfeiler_leman_test.h"
#include "read_TUDataset_test.h"
#include "pattern_growth_test.h"
#include "call_FSG_test.h"

int main() {
    // test the graph datastructure
    graph_test();

    const NodeId k = 4;
    const NodeId n = k+1;

    // Star graph with k leaves
    // NodeId and node label are identical
    // The NodeId of the root is 0
    // Edge labels are all 0
    Graph G_1;
    for (int i = 0; i < n; ++i) {
        G_1.add_node(i);
    }
    for (NodeId i = 1; i < n; ++i) {
        G_1.add_edge(0, i, 0);
    }

    // Star graph with k leaves
    // NodeId 0 gets node label n-1 etc.
    // The NodeId of the root is n-1 (so the label of the root is still 0)
    // Edge labels are all 0
    Graph G_2;
    for (int i = n-1; i >= 0; --i) {
        G_2.add_node(i);
    }
    for (NodeId i = 0; i < n-1; ++i) {
        G_2.add_edge(i, n-1, 0);
    }

    // test my wrapper code for gedlib
    sged_gedlib_test(G_1, G_2);

    // test my wrapper code for nauty
    gi_nauty_test(G_1, G_2);

    // test Weisfeiler-Leman
    gi_weisfeiler_leman_test(G_1, G_2);

    read_TUDataset_test();

    pattern_growth_test(G_1, G_2);

    call_FSG_test(G_1, G_2);

    return 0;
}
