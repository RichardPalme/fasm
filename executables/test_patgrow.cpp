#include "pattern_growth.h"
#include "call_FSG.h"
#include "read_TUDataset.h"
#include <iostream>
#include <numeric> // for std::accumulate
#include <cmath> // for std::lround

int main() {
#if 0
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

    for (NodeId i = 1; i < n-1; ++i) {
        G_1.add_edge(i, i+1, 0);
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

    for (NodeId i = 0; i < n-2; ++i) {
        G_2.add_edge(i, i+1, 0);
    }


    std::unordered_map<GraphId, Graph> database;
    database[0] = G_1;
    database[1] = G_2;
#endif

    std::string dataset_name = "MUTAG";

    std::unordered_map<GraphId, Graph> full_database;
    std::unordered_map<GraphId, Graph> database;
    read_TUDataset(full_database, dataset_name);
    database = full_database;
    size_t num_samples = database.size();

    std::vector<size_t> t(2);
    double support = 20; // between 0 and 100
    t[0] = std::lround(support / 100 * num_samples);
    t[1] = t[0];
    size_t max_size = 10;
    size_t max_num_patterns = 10;
    size_t wl_height = 10;
    bool exact_gi = true;
    bool apriori = true;
    GEDMethod ged_method = F2;

    std::cout << "t_0 = " << t[0] << ", t_1 = " << t[1] << std::endl;

    std::string filename = "test_patgrow_data.txt";
    write_FSG_file(database, filename);
    call_FSG(filename, support);

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
    patgrow.pattern_growth();

    std::cout << std::endl;

    std::vector<size_t> pattern_sizes(max_size+1, 0);
    for (Graph &G : patgrow.output) {
        pattern_sizes[G.num_edges()] += 1;
    }

    // number of patterns of size at least 1
    //
    // // number of patterns of size at least 1
    size_t num_patterns = std::accumulate(pattern_sizes.begin()+1, pattern_sizes.end(), 0);

    for (size_t i = 1; i < pattern_sizes.size(); ++i) {
        std::cout << pattern_sizes[i] << " freq. patterns of size " << i << std::endl;
    }

    std::cout << "total # of freq. patterns of size at least 1: " << num_patterns << std::endl;

    return 0;
}

