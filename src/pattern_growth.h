#ifndef _PATTERN_GROWTH_H
#define _PATTERN_GROWTH_H

#include "graph.h"
#include "sged_gedlib.h" // for enum GEDMethod
#include <vector>

typedef size_t GraphId;

struct TransactionData {
    GraphId tid;
    size_t sged;
    size_t lb;
};

class PatternGrowth {
public:
    std::unordered_map<GraphId, Graph> &database;
    std::vector<Graph> output;
    std::vector<int> vertex_labels;
    std::vector<int> edge_labels;
    std::vector<size_t> &t;
    size_t max_size; // maximum size of mined patterns (# of edges)
    size_t max_num_patterns; // abort program if there are too many freq. patterns
    size_t wl_height; // weisfeiler-leman height parameter
    bool exact_gi;
    bool apriori;
    GEDMethod ged_method;

    PatternGrowth(
            std::unordered_map<GraphId, Graph> &database,
            std::vector<size_t> &t,
            size_t max_size=10,
            size_t max_num_patterns=1000,
            size_t wl_height=10,
            bool exact_gi=true,
            bool apriori=true,
            GEDMethod ged_method=F2) :
        database{database}, t{t}, max_size{max_size},
        wl_height{wl_height}, exact_gi{exact_gi}, apriori{apriori},
        ged_method{ged_method} {}

    void extract_label_alphabet(void);
    void pattern_growth(void);

private:
    std::unordered_set<std::string> F;
    std::unordered_set<std::string> C;
    size_t pattern_count;
    size_t sged_count;

    std::string graph_hash(Graph &P);

    void _pattern_growth(
        Graph &P,
        std::vector<TransactionData> &support_set);

    void _update_naive_lb(
        Graph &P,
        Graph &H,
        std::vector<TransactionData> &S,
        int alpha,
        int beta);

    void _inner_loop(
        Graph &H,
        std::vector<TransactionData> &S);

    bool is_strong_candidate(Graph &H);

    bool is_interesting(
        Graph &H,
        std::vector<TransactionData> &S);
};

#endif
