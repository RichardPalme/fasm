#ifndef _CROSS_VALIDATION_H
#define _CROSS_VALIDATION_H

#include <dlib/svm.h>

typedef dlib::matrix<double, 0, 1> sample_type;
typedef dlib::linear_kernel<sample_type> kernel_type;

std::pair<double, double> cross_validate(
        std::vector<sample_type> &samples,
        std::vector<double> &labels,
        size_t folds,
        size_t num_repetitions);

#endif
