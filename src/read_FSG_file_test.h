#ifndef _READ_FSG_FILE_TEST_H
#define _READ_FSG_FILE_TEST_H

#include <cassert>
#include <iostream>
#include "read_FSG_file.h"

void read_FSG_file_test() {
    std::vector<Graph> dataset;
    std::string filename = "unit_test_data.fp";
    read_FSG_file(dataset, filename);

    assert(dataset.size() == 15);
}

#endif
