#include "call_FSG.h"
#include "utils.h"
#include <fstream>
#include <sstream>
#include <iostream>

void write_FSG_file(std::unordered_map<GraphId, Graph> &dataset, std::string &filename) {
    std::ofstream FSG_file;
    FSG_file.open(root_dir() / "data" / filename);

    for (auto &kv : dataset) {
        Graph &G = kv.second;
        size_t n = G.num_nodes();

        std::vector<NodeId> idx2id(n);
        std::unordered_map<NodeId, size_t> id2idx;

        size_t idx = 0;
        for (const auto &kv : G.node_labels) {
            NodeId u = kv.first;
            idx2id[idx] = u;
            id2idx[u] = idx;
            idx += 1;
        }

        FSG_file << "t\n";
        for(size_t i = 0; i < n; ++i) {
            FSG_file << "v " << i << " ";
            FSG_file << G.node_labels[idx2id[i]] << "\n";
        }
        for(const auto &kv : G.edge_labels) {
            size_t key = kv.first;
            auto e = G.key2edge(key);
            NodeId u = e.first;
            NodeId v = e.second;
            int beta = kv.second;

            FSG_file << "u " << id2idx[u] << " ";
            FSG_file << id2idx[v] << " " << beta << "\n";
        }
    }
    FSG_file.close();
}

// support needs to be in (0, 100]
void call_FSG(std::string filename, double support, size_t maxsize) {
    fs::path executable = root_dir() / "external" / "pafi-1.0.1" / "Linux" / "fsg";
    fs::path data_file = root_dir() / "data" / filename;

    std::stringstream ss;
    ss << executable.string() << " --support=" << support;
    ss << " --maxsize=" << maxsize;
    ss << " --minsize=0 " << data_file.string();

    auto console_output = exec(ss.str().c_str());
    std::cout << console_output;
}
