#include <fstream>
#include <sstream>
#include "utils.h"
#include "pattern_growth.h"


void read_FSG_file(
        std::vector<Graph> &dataset,
        std::string filename) {

    auto FSG_file = root_dir() / "data" / filename;
    std::ifstream frequent_graphs(FSG_file);

    GraphId graph_id = 0;
    std::string line;
    while (std::getline(frequent_graphs, line)) {
        std::istringstream iss(line);
        char c;
        iss >> c;
        if (c == 't') {
            dataset.push_back(Graph());
            graph_id = dataset.size() - 1;
        }
        if (c == 'v') {
            NodeId u;
            int alpha;
            iss >> u >> alpha;
            dataset[graph_id].add_node(alpha, u);
        }
        if (c == 'e') {
            NodeId u;
            NodeId v;
            int beta;
            iss >> u >> v >> beta;
            dataset[graph_id].add_edge(u, v, beta);
        }
    }
}
