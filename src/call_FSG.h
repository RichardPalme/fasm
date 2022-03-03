#ifndef _CALL_FSG_H
#define _CALL_FSG_H

#include "graph.h"
#include "pattern_growth.h" // for GraphId
#include <unordered_map>
#include <string>

void write_FSG_file(
        std::unordered_map<GraphId, Graph> &dataset,
        std::string &filename);

void call_FSG(std::string filename, double support, size_t maxsize=10);

#endif
