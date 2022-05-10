#include "cross_validation.h"

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
