#include "pattern_growth.h"
#include "read_TUDataset.h"
#include "utils.h"
#include <dlib/svm.h>
#include <cmath> // for lround
#include <iostream>
#include <fstream>

typedef dlib::matrix<double, 0, 1> sample_type;
typedef dlib::linear_kernel<sample_type> kernel_type;

fs::path storage_path(
        std::string &dataset_name, size_t t_0, size_t t_1) {

    std::string file_name =
        dataset_name
        + "_" + std::to_string(t_0)
        + "_" + std::to_string(t_1)
        + ".dat";
    return root_dir() / "data" / "experiments" / file_name;
}

bool feature_mining(PatternGrowth &patgrow, fs::path &stored_vectors) {
    size_t num_samples = patgrow.database.size();

    patgrow.extract_label_alphabet();
    bool max_patterns_exceeded = patgrow.pattern_growth();
    if (max_patterns_exceeded) {
        return true;
    }

    size_t num_features = patgrow.output.size();

    std::vector<sample_type> samples;
    std::vector<double> labels;
    samples.reserve(num_samples);
    labels.reserve(num_samples);

    std::cout << "Creating feature vectors ..." << std::endl;
    size_t ten_percent = (size_t) (0.1 * num_samples); // just for printing purposes
    size_t countdown = ten_percent;
    size_t progress = 0;

    // create the feature vectors (=samples)
    for (auto &kv : patgrow.database) {
        Graph &G = kv.second;

        sample_type sample; // represent G as feature vector
        sample.set_size(num_features, 1);
        //sample.reserve(num_features);
        for (size_t i = 0; i < num_features; ++i) {
            Graph &H = patgrow.output[i];
            sample(i) = sged_gedlib(H, G, patgrow.ged_method);
        }
        samples.push_back(sample);
        labels.push_back(G.m_class_label);

        // print a message after each 10% of progress
        if (--countdown == 0) {
            progress += 10;
            countdown = ten_percent;
            std::cout << progress << "\%" << std::endl;
        }
    }

    dlib::serialize(stored_vectors) << samples << labels;
    return false;
}

std::pair<double, double> cross_validate(
        std::vector<sample_type> &samples,
        std::vector<double> &labels,
        size_t folds,
        size_t num_repetitions) {

    dlib::svm_c_linear_dcd_trainer<kernel_type> trainer;

    std::vector<double> C{1};
    for (auto i : {1, 3, 5, 7, 9, 11}) {
        C.push_back(std::pow(2., -1*i));
        C.push_back(std::pow(2., i));
    }

    size_t num_samples = samples.size();
    size_t num_in_test = num_samples / folds;
    size_t num_in_val = num_samples / folds;
    size_t num_in_train = num_samples - num_in_test - num_in_val;
    std::vector<sample_type> samples_test;
    std::vector<sample_type> samples_val;
    std::vector<sample_type> samples_train;
    std::vector<double> labels_test;
    std::vector<double> labels_val;
    std::vector<double> labels_train;
    samples_test.reserve(num_in_test);
    samples_val.reserve(num_in_val);
    samples_train.reserve(num_in_train);
    labels_test.reserve(num_in_test);
    labels_val.reserve(num_in_val);
    labels_train.reserve(num_in_train);

    std::vector<double> test_accuracies;
    test_accuracies.reserve(folds * num_repetitions);

    for (size_t i = 1; i <= num_repetitions; ++i) {
        dlib::randomize_samples(samples, labels);

        size_t next_test_idx = 0;
        size_t next_idx;

        for (size_t k=1; k <= folds; ++k) {
            samples_test.clear();
            samples_val.clear();
            samples_train.clear();
            labels_test.clear();
            labels_val.clear();
            labels_train.clear();

            for (size_t cnt = 0; cnt < num_in_test; ++cnt) {
                samples_test.push_back(samples[next_test_idx]);
                labels_test.push_back(labels[next_test_idx]);
                next_test_idx = (next_test_idx + 1) % num_samples;
            }

            next_idx = next_test_idx;
            for (size_t cnt = 0; cnt < num_in_val; ++cnt) {
                samples_val.push_back(samples[next_idx]);
                labels_val.push_back(labels[next_idx]);
                next_idx = (next_idx + 1) % num_samples;
            }
            for (size_t cnt = 0; cnt < num_in_train; ++cnt) {
                samples_train.push_back(samples[next_idx]);
                labels_train.push_back(labels[next_idx]);
                next_idx = (next_idx + 1) % num_samples;
            }

            double best_val_acc = 0;
            double best_c = C[0];

            for (auto c : C) {
                trainer.set_c(c);
                auto dec_func = trainer.train(samples_train, labels_train);

                // compute accuracy on validation data
                double count = 0.;
                for (size_t l = 0; l < num_in_val; ++l) {
                    if (std::signbit(dec_func(samples_val[l]))
                            == std::signbit(labels_val[l])) {
                        count += 1.;
                    }
                }
                double val_acc = count / (double) num_in_val;
                if (val_acc > best_val_acc) {
                    best_val_acc = val_acc;
                    best_c = c;
                }
            }

            // train on training + validation data
            samples_train.insert(
                    samples_train.end(),
                    samples_val.begin(),
                    samples_val.end());

            labels_train.insert(
                    labels_train.end(),
                    labels_val.begin(),
                    labels_val.end());

            trainer.set_c(best_c);
            auto dec_func = trainer.train(samples_train, labels_train);

            // compute accuracy on test data
            double count = 0.;
            for (size_t l = 0; l < num_in_test; ++l) {
                if (std::signbit(dec_func(samples_test[l]))
                        == std::signbit(labels_test[l])) {
                    count += 1.;
                }
            }
            double test_acc = count / (double) num_in_test;
            test_accuracies.push_back(test_acc);
            std::cout << "fold " << i << "." << k << ", accuracy: " << test_acc;
            std::cout << ", best C: " << best_c << std::endl;
        }
    }

    double N = test_accuracies.size();
    double mean = std::accumulate(test_accuracies.begin(), test_accuracies.end(), 0.) / N;
    std::vector<double> diff(N);
    for (size_t i = 0; i < N; ++i) {
        diff[i] = test_accuracies[i] - mean;
    }
    double var = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.) / N;
    double stddev = std::sqrt(var);

    return std::make_pair(mean, stddev);
}


int main() {
    std::string dataset_name = "NCI1";
    size_t max_size = 6;
    size_t max_num_patterns = 100;
    size_t wl_height = 10;
    bool exact_gi = true;
    bool apriori = true;
    GEDMethod ged_method = F2;

    std::ofstream log;
    // append instead of overwrite
    log.open(root_dir() / "logs" / "log.txt", std::ios_base::app);

    std::cout << "dataset_name = " << dataset_name << std::endl;
    std::cout << "max_size = " << max_size << " " << std::endl;
    std::cout << "max_num_patterns = " << max_num_patterns << " " << std::endl;
    std::cout << "wl_height = " << wl_height << std::endl;
    std::cout << "exact_gi = " << exact_gi << std::endl;
    std::cout << "apriori = " << apriori << std::endl;
    std::cout << "ged_method = " << ged_method << std::endl;
    log << std::endl << "--------------------" << std::endl;
    log << "dataset_name = " << dataset_name << std::endl;
    log << "max_size = " << max_size << " " << std::endl;
    log << "max_num_patterns = " << max_num_patterns << " " << std::endl;
    log << "wl_height = " << wl_height << std::endl;
    log << "exact_gi = " << exact_gi << std::endl;
    log << "apriori = " << apriori << std::endl;
    log << "ged_method = " << ged_method << std::endl;

    std::unordered_map<GraphId, Graph> dataset;
    read_TUDataset(dataset, dataset_name);
    size_t num_samples = dataset.size();

    std::vector<size_t> t(2);

    PatternGrowth patgrow(
            dataset,
            t,
            max_size,
            max_num_patterns,
            wl_height,
            exact_gi,
            apriori,
            ged_method);

    size_t folds = 10;
    size_t num_repetitions = 10;
    std::cout << "folds = " << folds << std::endl;
    std::cout << "num_repetitions = " << num_repetitions << std::endl;
    std::cout << std::endl;
    log << "folds = " << folds << std::endl;
    log << "num_repetitions = " << num_repetitions << std::endl;
    log << std::endl;

    for (int t_1 = 40; t_1 >= 0; t_1 -= 10) {
        for (int t_0 = t_1; t_0 >= 0; t_0 -= 10) {
            t[0] = std::lround((double) t_0 / 100 * num_samples);
            t[1] = std::lround((double) t_1 / 100 * num_samples);
            std::cout << "t_0 = " << t_0 << ", t_1 = " << t_1 << std::endl;
            log << "t_0 = " << t_0 << ", t_1 = " << t_1 << std::endl;

            fs::path stored_vectors = storage_path(dataset_name, t_0, t_1);

            if (not fs::exists(stored_vectors)) {
                patgrow.t = t;
                // if we find too many patterns, break out of the inner loop
                if (feature_mining(patgrow, stored_vectors)) {
                    break;
                }
            }

            std::vector<sample_type> samples;
            std::vector<double> labels;
            dlib::deserialize(stored_vectors) >> samples >> labels;

            size_t num_features = samples[0].size();
            std::cout << "num_features = " << num_features << std::endl;
            log << "num_features = " << num_features << std::endl;
            if (num_features < 30) {
                continue;
            }

            std::vector<sample_type> binary_samples;
            std::vector<double> labels_copy = labels;

            for (const auto &sample : samples) {
                sample_type binary_sample;
                binary_sample.set_size(num_features, 1);
                for (size_t i = 0; i < num_features; ++i) {
                    if (sample(i) == 0) {
                        binary_sample(i) = 1;
                    } else {
                        binary_sample(i) = 0;
                    }
                }
                binary_samples.push_back(binary_sample);
            }

            std::cout << "Using SGED features" << std::endl;
            log << "Using SGED features" << std::endl;
            std::pair mean_stdev = cross_validate(
                    samples, labels, folds, num_repetitions);

            std::cout << "mean = " << mean_stdev.first << std::endl;
            std::cout << "stdev = " << mean_stdev.second << std::endl;
            std::cout << std::endl;
            log << "mean = " << mean_stdev.first << std::endl;
            log << "stdev = " << mean_stdev.second << std::endl;
            log << std::endl;

            std::cout << "Using binary features" << std::endl;
            log << "Using binary features" << std::endl;
            mean_stdev = cross_validate(
                    binary_samples, labels_copy, folds, num_repetitions);

            std::cout << "mean = " << mean_stdev.first << std::endl;
            std::cout << "stdev = " << mean_stdev.second << std::endl;
            std::cout << std::endl;
            log << "mean = " << mean_stdev.first << std::endl;
            log << "stdev = " << mean_stdev.second << std::endl;
            log << std::endl;
        }
    }

    return 0;
}

