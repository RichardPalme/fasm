#include "pattern_growth.h"
#include "read_TUDataset.h"
#include "read_FSG_file.h"
#include "call_FSG.h"
#include "utils.h"
#include "cross_validation.h"
#include <cmath> // for lround
#include <dlib/svm.h>
#include <iostream>
#include <fstream>

typedef dlib::matrix<double, 0, 1> sample_type;

int main() {
    std::string dataset_name = "NCI1";
    size_t t = 60; // between 0 and 100
    size_t max_size = 9;

    std::ofstream log;
    // append instead of overwrite
    log.open(root_dir() / "logs" / "log.txt", std::ios_base::app);
    std::cout << "dataset_name = " << dataset_name << std::endl;
    std::cout << "t = " << t << " " << std::endl;
    std::cout << "max_size = " << max_size << " " << std::endl;
    log << std::endl << "--------------------" << std::endl;
    log << "dataset_name = " << dataset_name << std::endl;
    log << "t = " << t << " " << std::endl;
    log << "max_size = " << max_size << " " << std::endl;

    // Read TUDataset
    std::unordered_map<GraphId, Graph> dataset;
    read_TUDataset(dataset, dataset_name);

    // Write dataset to FSG file
    std::string FSG_filename = dataset_name
        + "_" + std::to_string(t)
        + "_" + std::to_string(max_size);

    write_FSG_file(dataset, FSG_filename);

    // Call FSG with FSG_filename as input
    // The output will be written into FSG_filename + ".fp"
# if 1
    call_FSG(FSG_filename, t, max_size);
#endif

    std::string FSG_filename_fp = FSG_filename + ".fp";

    // Read the frequent pattern file
    std::vector<Graph> freq_graphs;
    read_FSG_file(freq_graphs, FSG_filename_fp);

    // Add frequent patterns of size 0 to freq_graphs
    size_t num_samples = dataset.size();

    std::unordered_map<int, size_t> label_freq;
    for (const auto &kv : dataset) {
        Graph G = kv.second;
        std::set<int> vlabels;
        std::transform(
            G.node_labels.begin(),
            G.node_labels.end(),
            std::inserter(vlabels, vlabels.end()),
            [](const auto &keyvalue) {return keyvalue.second;});

        for (auto alpha : vlabels) {
            if (label_freq.count(alpha) == 0) {
                label_freq[alpha] = 1;
            } else {
                label_freq[alpha] += 1;
            }
        }
    }

    for (auto &kv : label_freq) {
        int alpha = kv.first;
        size_t freq = kv.second;
        if (freq >= std::lround((double)t / 100 * num_samples)) {
            Graph G;
            G.add_node(alpha);
            freq_graphs.push_back(G);
        }
    }

    // Create the sged_matrix and label vector
    size_t num_patterns = freq_graphs.size();

    dlib::matrix<double> sged_matrix(0, 0);
    sged_matrix.set_size(num_patterns, num_samples);

    std::vector<double> labels(num_samples);

    size_t j = 0;
    for (const auto &kv : dataset) {
        std::cout << "computing sample vector " << j+1 << std::endl;

        Graph G = kv.second;
        labels[j] = G.m_class_label;

        size_t i = 0;
        for (auto &H : freq_graphs) {
            sged_matrix(i, j) = sged_gedlib(H, G, F2, std::vector<std::vector<double>>());
            i += 1;
        }
        j += 1;
    }

    // store the sged_matrix and label vector
    std::string matrix_filename = FSG_filename + ".sged_matrix";
    auto storage_path = root_dir() / "data" / matrix_filename;
    dlib::serialize(storage_path) << sged_matrix << labels;

    dlib::deserialize(storage_path) >> sged_matrix >> labels;


    // For each row of sged_matrix, compute t_0, t_1, ...
    size_t max_sged_val = 16;
    size_t folds = 5;
    size_t num_repetitions = 20;

    dlib::matrix<int> sged_counts(0, 0);
    sged_counts.set_size(num_patterns, max_sged_val + 1);
    for (size_t i = 0; i < num_patterns; ++i) {
        for (size_t k = 0; k <= max_sged_val; ++k) {
            sged_counts(i, k) = 0;
        }
    }

    for (size_t i = 0; i < num_patterns; ++i) {
        for (size_t j = 0; j < num_samples; ++j) {
            int sged_val = sged_matrix(i, j);
            for (size_t k = sged_val; k <= max_sged_val; ++k) {
                sged_counts(i, k) += 1;
            }
        }
    }

    std::cout << "i, t_i, t_0, num_features, mean, stdev, binary_mean, binary_stdev" << std::endl;
    log << "i, t_i, t_0, num_features, mean, stdev, binary_mean, binary_stdev" << std::endl;

    // For different choices of t = t_0, t_1, ...
    for (int i = 1; i <= max_sged_val; i *= 2) {
        for (int t_i = 100; t_i >= 0; t_i -= 20) {
            for (int t_0 = t_i; t_0 >= t; t_0 -= 10) {
                int real_t_i =
                    std::lround((double)t_i / 100 * num_samples);
                int real_t_0 =
                    std::lround((double)t_0 / 100 * num_samples);

                // Figure out which rows of sged_matrix are frequent
                std::vector<size_t> freq_indices;
                for (size_t l = 0; l < num_patterns; ++l) {
                    if ((sged_counts(l, 0) >= real_t_0) && (sged_counts(l, i) >= real_t_i)) {
                        freq_indices.push_back(l);
                    }
                }

                if (freq_indices.size() < 20) {
                    continue;
                }

                // Compute the feature vectors
                std::vector<sample_type> samples;
                std::vector<sample_type> binary_samples;
                for (size_t j = 0; j < num_samples; ++j) {
                    sample_type sample;
                    sample_type binary_sample;
                    sample.set_size(freq_indices.size(), 1);
                    binary_sample.set_size(freq_indices.size(), 1);
                    for (size_t k = 0; k < freq_indices.size(); ++k) {
                        int sged_val = sged_matrix(freq_indices[k], j);
                        sample(k) = sged_val;
                        binary_sample(k) = (sged_val == 0) ? 1 : 0;
                    }
                    samples.push_back(sample);
                    binary_samples.push_back(binary_sample);
                }

                std::cout << i << ", " << t_i << ", " << t_0 << ", "
                    << freq_indices.size() << ", " << std::endl;
                log << i << ", " << t_i << ", " << t_0 << ", "
                    << freq_indices.size() << ", ";

                std::pair mean_stdev = cross_validate(
                    samples, labels, folds, num_repetitions);

                std::cout << mean_stdev.first << ", "
                    << mean_stdev.second << ", " << std::endl;
                log << mean_stdev.first << ", "
                    << mean_stdev.second << ", ";

                std::pair binary_mean_stdev = cross_validate(
                    binary_samples, labels, folds, num_repetitions);

                std::cout << binary_mean_stdev.first << ", "
                    << binary_mean_stdev.second << std::endl;
                log << binary_mean_stdev.first << ", "
                    << binary_mean_stdev.second << std::endl;
            }
        }
    }

    return 0;
}

