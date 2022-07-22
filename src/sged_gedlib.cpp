#define GUROBI

#include "src/env/ged_env.hpp"
#include "src/edit_costs/edit_costs.hpp"
#include "sged_gedlib.h"
//#include <cstdio>
#include <unistd.h> // for dup and close
#include <iostream>

constexpr double eps() { return 1e-8; }

//enum NodeNames {C = 0, N = 1, O = 2, F = 3, I = 4, Cl = 5, Br = 6, L1 = 7, L2 = 8, L3 = 9, L4 = 10, L5 = 11};
//enum NodeNames {In = 0, Na = 4, Cl = 6, Br = 8, F = 9, K = 10, Cu = 11, Zn = 12, I = 13, Ba = 14, Sn = 15, Pb = 16, Ca = 17, L1 = 18, L2 = 19, L3 = 20, L4 = 21, L5 = 22, L6 = 23, L7 = 24, L8 = 25, L9 = 26};

#if 0
// These are the edit costs for MUTAG (only the substitution costs)
const double cost_matrix[12][12] {
    {0,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,1,1,1,1,1,1,1,1,1,1},
    {1,1,0,1,1,1,1,1,1,1,1,1},
    {1,1,1,0,1,1,1,1,1,1,1,1},
    {1,1,1,1,0,1,1,1,1,1,1,1},
    {1,1,1,1,1,0,1,1,1,1,1,1},
    {1,1,1,1,1,1,0,1,1,1,1,1},
    {1,1,1,1,1,0,0,1,1,1,1,1},
    {1,1,1,1,0,0,0,1,1,1,1,1},
    {1,1,1,0,0,0,0,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1,1,1}
};
#endif

const double max_cost = 1.;

template<class UserNodeLabel, class UserEdgeLabel>
class MatrixCosts : public ged::EditCosts<UserNodeLabel, UserEdgeLabel> {
public:
    std::vector<std::vector<double>> cost_matrix;

	virtual ~MatrixCosts();
	MatrixCosts(std::vector<std::vector<double>> &cost_matrix);
	virtual double node_ins_cost_fun(const UserNodeLabel & node_label) const final;
	virtual double node_del_cost_fun(const UserNodeLabel & node_label) const final;
	virtual double node_rel_cost_fun(const UserNodeLabel & node_label_1, const UserNodeLabel & node_label_2) const final;
	virtual double edge_ins_cost_fun(const UserEdgeLabel & edge_label) const final;
	virtual double edge_del_cost_fun(const UserEdgeLabel & edge_label) const final;
	virtual double edge_rel_cost_fun(const UserEdgeLabel & edge_label_1, const UserEdgeLabel & edge_label_2) const final;
};

template<>
MatrixCosts<int, int>::
~MatrixCosts() {}

template<>
MatrixCosts<int, int>::
MatrixCosts(std::vector<std::vector<double>> &cost_matrix) : cost_matrix{cost_matrix} {}

template<>
double
MatrixCosts<int, int>::
node_ins_cost_fun(const int & node_label) const {
	return 0;
}

template<>
double
MatrixCosts<int, int>::
node_del_cost_fun(const int & node_label) const {
	return max_cost;
}

template<>
double
MatrixCosts<int, int>::
node_rel_cost_fun(const int & a, const int & b) const {
    return cost_matrix[a][b];
#if 0
	if (a == b) {
		return 0;
	}
    if (a==L1)
        if (b==Cl || b==Br)
            return 1;
    if (a==L2)
        if (b==Cl || b==Br || b==I)
            return 2;
    if (a==L3)
        if (b==Cl || b==Br || b==I || b==F)
            return 3;
    if (a==L4) {
        if (b==Cl || b==Br || b==I || b==F || b==N) {
            return 4;
        }
    }
    if (a==L5) {
        if (b==Cl || b==Br || b==I || b==F || b==N || b==O) {
            return 5;
        }
    }
    return max_cost;
#endif
}

template<>
double
MatrixCosts<int, int>::
edge_ins_cost_fun(const int & edge_label) const {
	return 0;
}

template<>
double
MatrixCosts<int, int>::
edge_del_cost_fun(const int & edge_label) const {
	return max_cost;
}

template<>
double
MatrixCosts<int, int>::
edge_rel_cost_fun(const int & edge_label_1, const int & edge_label_2) const {
	if (edge_label_1 != edge_label_2) {
		return max_cost;
	}
	return 0;
}


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

int sged_gedlib(Graph &H, Graph &G, GEDMethod ged_method, std::vector<std::vector<double>> cost_matrix) {
    ged::GEDEnv<size_t, int, int> env;

#if 0
    // insertions have cost 0, deletions / substitutions have cost 1
    env.set_edit_costs(
            ged::Options::EditCosts::CONSTANT,
            {0,1,1,0,1,1});
#endif

#if 1
    MatrixCosts<int, int> Costs1(cost_matrix);
    MatrixCosts<int, int> *edit_costs;
    edit_costs = &Costs1;
    env.set_edit_costs(edit_costs);
#endif

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
