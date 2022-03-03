#ifndef _READ_TUDATASET_TEST_H
#define _READ_TUDATASET_TEST_H

#include <iostream>
#include <cassert>
#include <string>
#include "read_TUDataset.h"

void read_TUDataset_test() {
    std::unordered_map<GraphId, Graph> dataset;
    std::string dataset_name = "MUTAG";
    read_TUDataset(dataset, dataset_name);

    assert(dataset.size() == 188);

    Graph G = dataset[1];
    assert(G.num_nodes() == 17);
    assert(G.num_edges() == 19);

/*
    for (auto kv : G.node_labels) {
        int label = kv.second;
        std::cout << label << ", ";
    }
    std::cout << std::endl;

    for (auto kv : G.edge_labels) {
        int label = kv.second;
        std::cout << label << ", ";
    }
    std::cout << std::endl;
*/
}

#endif
