#include "gi_nauty.h"
#include "nausparse.h"
#include <vector>
#include <numeric> // for std::iota
#include <algorithm> // for std::sort
#include <limits>

Graph preprocess_graph(Graph &G) {
    Graph H = G;
    for (const auto &kv : G.edge_labels) {
        size_t key = kv.first;
        int label = kv.second;
        auto e = G.key2edge(key);
        NodeId u = e.first;
        NodeId v = e.second;

        H.delete_edge(u, v);
        NodeId new_node = H.add_node(std::numeric_limits<int>::max() - label);
        H.add_edge(u, new_node, 0);
        H.add_edge(v, new_node, 0);
    }
    return H;
}

// only works for undirected graphs
std::string gi_nauty(Graph &G) {
    Graph H = preprocess_graph(G);

    size_t n = H.num_nodes();
    if (n == 0) {
        return "";
    }

    std::vector<NodeId> idx2id(n);
    std::unordered_map<NodeId, size_t> id2idx;

    size_t idx = 0;
    for (const auto &kv : H.node_labels) {
        NodeId u = kv.first;
        idx2id[idx] = u;
        id2idx[u] = idx;
        idx += 1;
    }

    // Declarations
    DYNALLSTAT(int,lab,lab_sz);
    DYNALLSTAT(int,ptn,ptn_sz);
    DYNALLSTAT(int,orbits,orbits_sz);
    static DEFAULTOPTIONS_SPARSEGRAPH(options);
    statsblk stats;

    SG_DECL(sg);    /* Declare and initialize sparse graph structure */
    SG_DECL(cg);

    options.getcanon = TRUE;
    options.defaultptn = FALSE;

    int m = SETWORDSNEEDED(n);
    nauty_check(WORDSIZE,m,n,NAUTYVERSIONID);

    DYNALLOC1(int,lab,lab_sz,n,"malloc");
    DYNALLOC1(int,ptn,ptn_sz,n,"malloc");
    DYNALLOC1(int,orbits,orbits_sz,n,"malloc");

    /* SG_ALLOC makes sure that the v,d,e fields of a sparse graph
    structure point to arrays that are large enough. This only
    works if the structure has been initialised. */
    SG_ALLOC(sg,n,2*H.num_edges(),"malloc");
    SG_ALLOC(cg,n,2*H.num_edges(),"malloc");

    sg.nv = n; /* Number of vertices */
    sg.nde = 2 * H.num_edges(); /* Number of directed edges */

    size_t e_idx = 0;
    for (int i = 0; i < n; ++i) {
        NodeId v = idx2id[i];
        sg.v[i] = e_idx; // indices to sg.e
        sg.d[i] = H.outgoing[v].size();
        for (NodeId w : H.outgoing[v]) {
            sg.e[e_idx] = id2idx[w];
            e_idx += 1;
        }
    }

    std::vector<NodeId> partition(n);
    std::iota(partition.begin(), partition.end(), 0);
    std::sort(
        partition.begin(),
        partition.end(),
        [&](size_t i, size_t j) {
            return H.node_labels[idx2id[i]] < H.node_labels[idx2id[j]];
        });

    std::vector<int> colors(n);
    for (int i = 0; i < n; ++i) {
        lab[i] = partition[i];
        colors[i] = H.node_labels[idx2id[partition[i]]];
    }

    for (int i = 0; i < n-1; ++i) {
        if (colors[i] != colors[i+1]) {
            ptn[i] = 0;
        } else {
            ptn[i] = 1;
        }
    }
    ptn[n-1] = 0;

    // changes both lab and cg.
    // the value of lab on return is the canonical labelling of the graph.
    // Precisely, it lists the vertices of g in the order in which they need to
    // be relabelled to give cg (it maps new indices of cg to old indices of g)
    sparsenauty(&sg, lab, ptn, orbits, &options, &stats, &cg);

    std::string hash;
    for (int i = 0; i < n; ++i) {
        std::string tmp = std::to_string(i) + "," + std::to_string(colors[i]);
        std::vector<int> neighbors(cg.d[i]);
        for (int j = 0; j < cg.d[i]; ++j) {
            neighbors[j] = cg.e[cg.v[i] + j];
        }
        std::sort(neighbors.begin(), neighbors.end());
        for (int j = 0; j < cg.d[i]; ++j) {
            tmp += "," + std::to_string(neighbors[j]);
        }
        tmp += "/";
        hash += tmp;
    }

    DYNFREE(lab, lab_sz);
    DYNFREE(ptn, ptn_sz);
    DYNFREE(orbits, orbits_sz);
    SG_FREE(sg);
    SG_FREE(cg);

    return hash;
}
