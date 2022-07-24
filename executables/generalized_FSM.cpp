#include "pattern_growth.h"
#include "read_TUDataset.h"
#include "utils.h"
#include <cmath> // for lround
#include <cstring>
#include <cstdlib>
#include <dlib/svm.h>
#include <iostream>
#include <fstream>

//enum NodeNames {In = 0, Na = 4, Cl = 6, Br = 8, F = 9, K = 10, Cu = 11, Zn = 12, I = 13, Ba = 14, Sn = 15, Pb = 16, Ca = 17, L1 = 18, L2 = 19, L3 = 20, L4 = 21, L5 = 22, L6 = 23, L7 = 24, L8 = 25, L9 = 26};

int main(int argc, char **argv) {

    std::string dataset_name = "MUTAG";
    size_t max_size = 20;
    size_t max_num_patterns = 10000;
    size_t wl_height = 100;
    bool exact_gi = true;
    bool apriori = true;
    GEDMethod ged_method = F2;
    size_t t_0 = 42;

    size_t num_new_labels = 5;

    for (size_t i = 1; i < argc; i++) {
        if (std::strcmp(argv[i], "-dataset_name")==0) {
            dataset_name = argv[i + 1];
        } else if (std::strcmp(argv[i], "-max_size")==0) {
            max_size = std::atoi(argv[i + 1]);
        } else if (std::strcmp(argv[i], "-exact_gi")==0) {
            exact_gi = std::atoi(argv[i + 1]);
        } else if (std::strcmp(argv[i], "-ged_method")==0) {
            ged_method = (GEDMethod) std::atoi(argv[i + 1]);
        } else if (std::strcmp(argv[i], "-num_new_labels")==0) {
            num_new_labels = std::atoi(argv[i + 1]);
        } else if (std::strcmp(argv[i], "-t")==0) {
            t_0 = std::atoi(argv[i + 1]);
        }
    }

    std::unordered_map<GraphId, Graph> full_dataset;
    read_TUDataset(full_dataset, dataset_name);

    std::vector<std::vector<double>> cost_matrix;
    read_cost_matrix(cost_matrix, dataset_name);

#if 0
    std::unordered_map<GraphId, Graph> dataset;
    for (const auto &kv : full_dataset) {
        GraphId g = kv.first;
        Graph G = kv.second;
        bool keep = false;
        for (const auto &kv2 : G.node_labels) {
            int a = kv2.second;
            if (a==3 || a==4 || a==5 || a==6) {
                keep = true;
            }
        }
        if (keep == true) {
            dataset[g] = G;
        }
    }
#endif
#if 1
    auto dataset = full_dataset;
#endif

    size_t num_samples = dataset.size();
    std::cout << "num_samples = " << num_samples << std::endl;

    size_t tau = 0;
    std::vector<size_t> t(tau+1);
    for (size_t i = 0; i < tau; ++i) {
        t[i] = 0;
    }
    t[tau] = std::lround((double)t_0 / 100 * num_samples);

    PatternGrowth patgrow(
        dataset,
        t,
        max_size,
        max_num_patterns,
        wl_height,
        exact_gi,
        apriori,
        ged_method,
        cost_matrix);

    patgrow.extract_label_alphabet();


    int max_label = *max_element(patgrow.vertex_labels.begin(), patgrow.vertex_labels.end());
    for (size_t i = 1; i <= num_new_labels; ++i) {
        patgrow.vertex_labels.push_back(max_label + i);
    }

    patgrow.pattern_growth();

    std::cout << "Selection of the frequent generalized patterns:" << std::endl;
    for (Graph &G : patgrow.output) {
        bool found = false;
        // only print frequent patterns that contain new nodes
        for (const auto &kv2 : G.node_labels) {
            int alpha = kv2.second;
            if (alpha > max_label) {
                found = true;
            }
        }
        // only print frequent patterns that contain a cycle
        if (found && (G.num_edges() >= G.num_nodes())) {
            G.print();
            /*
            for (const auto &kv : dataset) {
                Graph H = kv.second;
                if (sged_gedlib(G, H, F2) < 1000) {
                    H.print();
                }
            }
            */
        }
    }

#if 0
    //Create the SGED matrix

    std::cout << "Create the SGED matrix" << std::endl;

    size_t num_rows = patgrow.output.size();
    dlib::matrix<double> sged_matrix(0,0);
    sged_matrix.set_size(num_rows, num_samples);

    size_t j = 0;
    for (const auto &kv : dataset) {
        Graph G = kv.second;
        size_t i = 0;
        for (auto &H : patgrow.output) {
            sged_matrix(i, j) = sged_gedlib(H, G, F2);
            i += 1;
        }
        j += 1;
    }


    std::ofstream out(root_dir() / "data" / "Mutag_sged_matrix.csv");
    for (size_t i = 0; i < num_rows; ++i) {
        for (size_t j = 0; j < num_samples; ++j) {
            out << sged_matrix(i, j) << ',';
        }
        out << '\n';
    }
#endif

}
