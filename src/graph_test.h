#include "graph.h"
#include <iostream>
#include <cassert>

void graph_test() {
    Graph G;

    // test is_connected edge case
    assert(G.is_connected() == true);

    G.add_node(0);
    G.add_node(1);
    G.add_edge(0,1,0);

    // test num_nodes()
    assert(G.num_nodes() == 2);
    // test add_node()
    assert(G.node_labels[0] == 0);

    // test add_edge()
    assert(G.outgoing[0].size() == 1);
    // test add_edge()
    auto it = G.outgoing[0].find(1);
    assert(it != G.outgoing[0].end());
    // test add_edge()
    assert(G.edge_labels[G.edge2key(0,1)] == 0);

    // test is_connected()
    assert(G.is_connected() == true);

    // test delete_edge()
    G.delete_edge(1, 0);
    it = G.outgoing[0].find(1);
    assert(it == G.outgoing[0].end());
    assert(G.num_edges() == 0);

    // test is_connected()
    assert(G.is_connected() == false);

    // test delete_isolated_node()
    G.delete_isolated_node(0);
    G.delete_isolated_node(1);
    assert(G.num_nodes() == 0);
}
