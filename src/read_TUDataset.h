#ifndef _READ_TUDATASET_H
#define _READ_TUDATASET_H

#include <string>
#include "graph.h"
#include "pattern_growth.h"

void read_TUDataset(
    std::unordered_map<GraphId, Graph> &dataset,
    std::string dataset_name);

#endif
