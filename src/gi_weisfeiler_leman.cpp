#include "gi_weisfeiler_leman.h"
#include <vector>
#include <functional> // for std::hash

std::hash<std::string> hasher; // returns size_t

std::string gi_weisfeiler_leman(Graph &G, size_t h) {
    size_t n = G.num_nodes();
    size_t max_iterations = n - 1;

    std::unordered_map<NodeId, std::string> vertex_labels;
    for (const auto &kv : G.node_labels) {
        NodeId v = kv.first;
        int label = kv.second;
        vertex_labels[v] = std::to_string(label);
    }

    for (size_t i = 1; i <= std::min(h, max_iterations); ++i) {
        for (const auto &kv : G.node_labels) {
            NodeId v = kv.first;

            std::vector<std::string> outgoing_labels;
            for (const NodeId &w: G.outgoing[v]) {
                outgoing_labels.push_back(
                    std::to_string(G.edge_labels[G.edge2key(v,w)])
                    + ","
                    + std::to_string(G.node_labels[w])
                    + ",");
            }
            std::sort(outgoing_labels.begin(), outgoing_labels.end());

            std::vector<std::string> incoming_labels;
            if (G.m_directed) {
                for (const NodeId &w : G.incoming[v]) {
                    incoming_labels.push_back(
                        std::to_string(G.edge_labels[G.edge2key(w,v)])
                        + ","
                        + std::to_string(G.node_labels[w])
                        + ",");
                }
                std::sort(
                    incoming_labels.begin(),
                    incoming_labels.end());
            }

            std::string aggregated_labels;
            for (const std::string &label : outgoing_labels) {
                aggregated_labels += label;
            }

            if (G.m_directed) {
                aggregated_labels += "#"; // separator
                for (const std::string &label : incoming_labels) {
                    aggregated_labels += label;
                }
            }

            vertex_labels[v] = std::to_string(
                hasher(vertex_labels[v] + "," + aggregated_labels));
        }
    }

    std::vector<std::string> label_vec;
    label_vec.reserve(n);
    for (const auto &kv : vertex_labels) {
        std::string label = kv.second;
        label_vec.push_back(label + ",");
    }
    std::sort(label_vec.begin(), label_vec.end());

    std::string aggregated_labels;
    for (const std::string &labels: label_vec) {
        aggregated_labels += labels;
    }

    // as soon as we do not use nauty anymore, remove std::to_string!
    return std::to_string(hasher(aggregated_labels));
}

