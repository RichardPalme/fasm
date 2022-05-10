#define GUROBI

#include "src/env/ged_env.hpp"
#include "sged_gedlib.h"
//#include <cstdio>
#include <unistd.h> // for dup and close
#include <iostream>

constexpr double eps() { return 1e-8; }

// works only if the graph G is undirected, as gedlib only works for undirected graphs
ged::GEDGraph::GraphID add_graph_to_env(
        Graph &G,
        ged::GEDEnv<size_t, int, int> &env) {

    auto graph_id = env.add_graph();
    for (const auto &kv : G.node_labels) {
        NodeId u = kv.first;
        int label = kv.second;
        env.add_node(graph_id, u, label);
    }
    for (const auto &kv : G.edge_labels) {
        size_t key = kv.first;
        int label = kv.second;
        auto e = G.key2edge(key);
        env.add_edge(graph_id, e.first, e.second, label);
    }
    return graph_id;
}

int sged_gedlib(Graph &H, Graph &G, GEDMethod ged_method) {
    ged::GEDEnv<size_t, int, int> env;

    // insertions have cost 0, deletions / substitutions have cost 1
    env.set_edit_costs(
            ged::Options::EditCosts::CONSTANT,
            {0,1,1,0,1,1});

    switch(ged_method) {
        case F2: env.set_method(ged::Options::GEDMethod::F2); break;
        case RING: env.set_method(ged::Options::GEDMethod::RING); break;
        // env.set_method(ged::Options::GEDMethod::RING, "--led-method GAMMA");
        case IPFP: env.set_method(ged::Options::GEDMethod::IPFP); break;
        case BRANCH_FAST: env.set_method(ged::Options::GEDMethod::BRANCH_FAST); break;
        case REFINE: env.set_method(ged::Options::GEDMethod::REFINE); break;
        case BP_BEAM:
            env.set_method(ged::Options::GEDMethod::BP_BEAM, "--num-orderings 20");
            break;
    }

    auto H_id = add_graph_to_env(H, env);
    auto G_id = add_graph_to_env(G, env);

    env.init(ged::Options::InitType::EAGER_WITHOUT_SHUFFLED_COPIES);
    //env.init(ged::Options::InitType::LAZY_WITHOUT_SHUFFLED_COPIES);

    // redirect stdout so that Gurobi does not tell me about my license...
    int stdout_copy = dup(1);
    close(1);

    env.run_method(H_id, G_id);

    dup2(stdout_copy, 1);
    close(stdout_copy);

    return env.get_upper_bound(H_id, G_id) + eps();
}
