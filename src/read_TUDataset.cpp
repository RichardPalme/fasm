#include "read_TUDataset.h"
#include "utils.h"
#include <fstream>

void read_TUDataset(
        std::unordered_map<GraphId, Graph> &dataset,
        std::string dataset_name) {

    auto data_dir = root_dir() / "data" / dataset_name;

    // the i-th line contains the GraphId for node id i
    std::ifstream graph_ids(
            data_dir / (dataset_name + "_graph_indicator.txt"));

    // the i-th line contains the label for node id i
    std::ifstream node_labels(
            data_dir / (dataset_name + "_node_labels.txt"));

    std::unordered_map<NodeId, GraphId> nodeid2graphid;
    GraphId graph_id = 1; // GraphIds start at 1 in TUDataset
    GraphId prev_graph_id = 0;
    int node_label;
    NodeId node_id = 1; // NodeIds start at 1 in TUDataset

    while ((graph_ids >> graph_id) && (node_labels >> node_label)) {
        if (graph_id != prev_graph_id) {
            dataset[graph_id] = Graph();
        }

        dataset[graph_id].add_node(node_label, node_id);
        prev_graph_id = graph_id;

        nodeid2graphid[node_id] = graph_id;
        node_id += 1; // node_id = current line in the file
    }

    // each line contains two node ids. There are two lines per edge.
    std::ifstream edges(data_dir / (dataset_name + "_A.txt"));
    std::ifstream edge_labels(
            data_dir / (dataset_name + "_edge_labels.txt"));

    NodeId u, v;
    char c;
    int edge_label = 0;

    if (edge_labels.fail()) {
        while (edges >> u >> c >> v) {
            graph_id = nodeid2graphid[u];
            dataset[graph_id].add_single_edge(u, v, edge_label);
        }
    } else {
        while ((edges >> u >> c >> v) && (edge_labels >> edge_label)) {
            graph_id = nodeid2graphid[u];
            dataset[graph_id].add_single_edge(u, v, edge_label);
        }
    }

    std::ifstream class_labels(
            data_dir / (dataset_name + "_graph_labels.txt"));

    int class_label;
    graph_id = 1;

    while (class_labels >> class_label) {
        dataset[graph_id].m_class_label = class_label;
        graph_id += 1;
    }
}
