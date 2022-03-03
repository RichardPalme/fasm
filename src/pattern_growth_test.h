#ifndef _PATTERN_GROWTH_TEST_H
#define _PATTERN_GROWTH_TEST_H

#include "pattern_growth.h"
#include <cassert>
#include <cmath> // for std::pow

void pattern_growth_test(Graph G_1, Graph G_2) {
    const size_t k = G_1.num_nodes() - 1;

    std::unordered_map<GraphId, Graph> database;
    database[0] = G_1;
    database[1] = G_2;

    std::vector<size_t> t{2,2};
    size_t max_size = 10;
    size_t max_num_patterns = 1000;
    size_t wl_height = 10;
    bool exact_gi = true;
    bool apriori = true;
    GEDMethod ged_method = F2;

    PatternGrowth patgrow(
        database,
        t,
        max_size,
        max_num_patterns,
        wl_height,
        exact_gi,
        apriori,
        ged_method);

    patgrow.extract_label_alphabet();
    assert(patgrow.vertex_labels.size() == k+1);
    assert(patgrow.edge_labels.size() == 1);

    patgrow.pattern_growth();

    // output = all connected subgraphs of G_1
    assert(patgrow.output.size() == k + std::pow(2, k));

    G_1.node_labels[0] = 100;
    database[0] = G_1;

    patgrow.database = database;
    patgrow.extract_label_alphabet();
    patgrow.pattern_growth();
    assert(patgrow.output.size() == k);

    t[0] = 1;
    t[1] = 2;
    patgrow.t = t;
    patgrow.pattern_growth();
    assert(patgrow.output.size() == std::pow(2,k+1) + k);
}

#endif
