#include "pattern_growth.h"
#include "gi_nauty.h"
#include "gi_weisfeiler_leman.h"
#include "sged_gedlib.h"
#include <algorithm> // for std::sort
#include <set>
#include <iostream>

void PatternGrowth::extract_label_alphabet() {
    vertex_labels.clear();
    edge_labels.clear();

    std::set<int> vlabels;
    std::set<int> elabels;
    for (const auto &kv : database) {
        const Graph &G = kv.second;
        std::transform(
                G.node_labels.begin(),
                G.node_labels.end(),
                std::inserter(vlabels, vlabels.end()),
                [](const auto &keyvalue) {return keyvalue.second;});
        std::transform(
                G.edge_labels.begin(),
                G.edge_labels.end(),
                std::inserter(elabels, elabels.end()),
                [](const auto &keyvalue) {return keyvalue.second;});
    }
    std::copy(vlabels.begin(), vlabels.end(), std::back_inserter(vertex_labels));
    std::copy(elabels.begin(), elabels.end(), std::back_inserter(edge_labels));
    std::sort(vertex_labels.begin(), vertex_labels.end());
    std::sort(edge_labels.begin(), edge_labels.end());
}

std::string PatternGrowth::graph_hash(Graph &P) {
    if (exact_gi) {
        return gi_nauty(P);
    } else {
        return gi_weisfeiler_leman(P, wl_height);
    }
}

void PatternGrowth::pattern_growth() {
    output.clear();
    F.clear();
    C.clear();
    pattern_count = 0;
    sged_count = 0;
    const size_t n = database.size();

    std::vector<TransactionData> support_set(n);
    size_t i = 0;
    for (auto & kv : database) {
        support_set[i].tid = kv.first;
        support_set[i].sged = 0;
        support_set[i].lb = 0;
        i += 1;
    }

    Graph P;
    std::string P_hash = graph_hash(P);
    C.insert(P_hash);
    F.insert(P_hash);

    _pattern_growth(P, support_set);
}


void PatternGrowth::_pattern_growth(
        Graph &P,
        std::vector<TransactionData> &support_set) {

    if (P.num_edges() >= max_size) {
        return;
    }

    if (P.num_nodes() == 0) {
        for (int alpha: vertex_labels) {
            Graph H = P;
            H.add_node(alpha);

            // local copy so that support_set does not get changed
            std::vector<TransactionData> S = support_set;
            _update_naive_lb(P, H, S, alpha, NONE());
            _inner_loop(H, S);
        }
    }

    for (const auto &kv : P.node_labels) { // for all NodeIds u
        NodeId u = kv.first;
        for (const auto &kv : P.node_labels) { // for all NodeIds v
            NodeId v = kv.first;
            // only consider those pairs u, v with u < v
            if (u >= v) {
                continue;
            }

            // if {u,v} is an edge in P then continue. I.e. only consider those
            // pairs u, v s.t. {u, v} is not an edge in P
            if(std::find(P.outgoing[u].begin(), P.outgoing[u].end(), v)
                    != P.outgoing[u].end()) {
                continue;
            }

            for (int beta: edge_labels) {
                Graph H = P;
                H.add_edge(u, v, beta);

                std::vector<TransactionData> S = support_set;
                _update_naive_lb(P, H, S, NONE(), beta);
                _inner_loop(H, S);
            }
        }

        for (int alpha: vertex_labels) {
            for (int beta: edge_labels) {
                Graph H = P;
                NodeId new_node = H.add_node(alpha);
                H.add_edge(u, new_node, beta);

                std::vector<TransactionData> S = support_set;
                _update_naive_lb(P, H, S, alpha, beta);
                _inner_loop(H, S);
            }
        }
    }
}


void PatternGrowth::_update_naive_lb(
        Graph &P,
        Graph &H,
        std::vector<TransactionData> &S,
        int alpha,
        int beta) {

    // if a node was added to P
    if (P.num_nodes() < H.num_nodes()) {
        // count how often alpha appears in H.node_labels
        size_t H_label_count = std::count_if(
            H.node_labels.begin(),
            H.node_labels.end(),
            [=](const auto &kv){return kv.second == alpha;});

        for (size_t i = 0; i < S.size(); ++i) {
            size_t tid = S[i].tid;

            size_t G_label_count = std::count_if(
                database[tid].node_labels.begin(),
                database[tid].node_labels.end(),
                [=](const auto &kv){return kv.second == alpha;});

            if (H_label_count > G_label_count) {
                S[i].lb += 1;
            }
        }
    }

    // if an edge was added to P
    if (P.num_edges() < H.num_edges()) {
        size_t H_label_count = std::count_if(
            H.edge_labels.begin(),
            H.edge_labels.end(),
            [=](const auto &kv){return kv.second == beta;});

        for (size_t i = 0; i < S.size(); ++i) {
            size_t tid = S[i].tid;

            size_t G_label_count = std::count_if(
                database[tid].edge_labels.begin(),
                database[tid].edge_labels.end(),
                [=](const auto &kv){return kv.second == beta;});

            if (H_label_count > G_label_count) {
                S[i].lb += 1;
            }
        }
    }
}


void PatternGrowth::_inner_loop(
        Graph &H,
        std::vector<TransactionData> &S) {

    std::string H_hash = graph_hash(H);

    if (apriori) {
        if (C.find(H_hash) == C.end() && is_strong_candidate(H)) {
            C.insert(H_hash);

            if (is_interesting(H, S)) {
                F.insert(H_hash);
                output.push_back(H);
                pattern_count += 1;
                std::cout << "pattern " << pattern_count << " has size ";
                std::cout << H.num_edges() << std::endl;

                _pattern_growth(H, S);
            }
        }
    } else {
        if (C.find(H_hash) == C.end()) {
            C.insert(H_hash);

            if (is_interesting(H, S)) {
                output.push_back(H);
                pattern_count += 1;
                std::cout << "pattern " << pattern_count << " has size ";
                std::cout << H.num_edges() << std::endl;

                if (output.size() > max_num_patterns) {
                    exit(EXIT_FAILURE);
                }

                _pattern_growth(H, S);
            }
        }
    }
}


bool PatternGrowth::is_strong_candidate(Graph &H) {
    for (const auto &kv : H.edge_labels) {
        size_t key = kv.first;
        auto e = H.key2edge(key);
        NodeId u = e.first;
        NodeId v = e.second;

        // if u and v have exactly one incident edge, then the predecessors
        // of H are two isolated vertices
        if (H.outgoing[u].size() == 1 && H.outgoing[v].size() == 1) {
            Graph P;
            NodeId new_node = P.add_node(H.node_labels[u]);
            std::string P_hash = graph_hash(P);
            // if P_hash is not in F, then H is not a strong candidate
            if (F.find(P_hash) == F.end()) {
                return false;
            }

            P.node_labels[new_node] = H.node_labels[v];
            P_hash = graph_hash(P);
            if (F.find(P_hash) == F.end()) {
                return false;
            }

        // if only u has exactly one incident edge, then deleting the edge + u
        // yields a predecessor
        } else if (H.outgoing[u].size() == 1) {
            Graph P = H;
            P.delete_edge(u, v);
            P.delete_isolated_node(u);

            std::string P_hash = graph_hash(P);
            if (F.find(P_hash) == F.end()) {
                return false;
            }

        // if only v has exactly one incident edge, ...
        } else if (H.outgoing[v].size() == 1) {
            Graph P = H;
            P.delete_edge(u, v);
            P.delete_isolated_node(v);

            std::string P_hash = graph_hash(P);
            if (F.find(P_hash) == F.end()) {
                return false;
            }

        // else, deleting only the edge yields a predecessor, but only if
        // the resulting graph is connected!
        } else {
            Graph P = H;
            P.delete_edge(u, v);

            if (P.is_connected()) {
                std::string P_hash = graph_hash(P);
                if (F.find(P_hash) == F.end()) {
                    return false;
                }
            }
        }
    }
    return true;
}

bool PatternGrowth::is_interesting(
        Graph &H,
        std::vector<TransactionData> &S) {

    const size_t N = t.size() - 1; // max index of t

    // update the sged values w.r.t. lb
    for (size_t i = 0; i < S.size(); ++i) {
        S[i].sged = std::max(S[i].sged, S[i].lb);
    }

    // upper bounds for t (u is not an upper bound iff H is not interesting)
    std::vector<size_t> u(N+1, 0);
    // initialize u
    for (size_t i = 0; i < S.size(); ++i) {
        for (size_t j = S[i].sged; j <= N; ++j) {
            u[j] += 1;
        }
    }

    // test if u contains upper bounds for t
    for (size_t j = 0; j <= N; ++j) {
        if (u[j] < t[j]) {
            return false;
        }
    }

    // sort S w.r.t. sged
    std::sort(S.begin(), S.end(),
        [](auto &left, auto &right) {return left.sged < right.sged;});

    // update sged
    for (size_t i = 0; i < S.size(); ++i) {
        GraphId tid = S[i].tid;
        size_t sged = S[i].sged;
        S[i].sged = sged_gedlib(H, database[tid], ged_method);
        for (size_t j = sged; j < S[i].sged && j <= N; ++j) {
            u[j] -= 1;
            if (u[j] < t[j]) {
                return false;
            }
        }
    }

    // sort S w.r.t. sged
    std::sort(S.begin(), S.end(),
        [](auto &left, auto &right) {return left.sged < right.sged;});

    // remove elements from S that have sged-value > N
    size_t i = 0;
    while (i < S.size() && S[i].sged <= N) {
        i += 1;
    }
    S.resize(i);

    return true;
}
