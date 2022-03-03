#include "call_FSG.h"
#include <cassert>

void call_FSG_test(Graph G_1, Graph G_2) {
    std::unordered_map<GraphId, Graph> dataset;
    dataset[1] = G_1;
    dataset[2] = G_2;
    std::string filename = "unit_test_data.txt";
    write_FSG_file(dataset, filename);
    double support = 100;
    call_FSG(filename, support);
}
