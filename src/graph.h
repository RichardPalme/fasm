#ifndef _GRAPH_H
#define _GRAPH_H

#include <unordered_map>
#include <unordered_set>
#include <utility> // for std::pair
#include <limits>

typedef size_t NodeId;
typedef std::unordered_set<NodeId> NodeSet;

// invalid label or node that should be ignored.
constexpr int NONE() {return std::numeric_limits<int>::max();}

// use the default NodeId when adding a node
constexpr NodeId DEFAULT() {return std::numeric_limits<size_t>::max();}

class Graph {
public:
    std::unordered_map<NodeId, NodeSet> outgoing;
    // Note: incoming is empty for undirected graphs
    std::unordered_map<NodeId, NodeSet> incoming;
    std::unordered_map<NodeId, int> node_labels;
    // Note: the keys for edge_labels are given by edge2key(u, v)
    std::unordered_map<size_t, int> edge_labels;

    bool m_directed;
    int m_class_label;

    Graph(bool m_directed=false, int m_class_label=0):
        m_directed{m_directed}, m_class_label{m_class_label}, m_next_NodeId{0}
    {}

    // returns a key to be used by edge_labels
    size_t edge2key(NodeId u, NodeId v);
    std::pair<NodeId, NodeId> key2edge(size_t key);

    NodeId add_node(int label, NodeId u = DEFAULT());
    void delete_isolated_node(NodeId u);
    void add_edge(NodeId u, NodeId v, int label);
    void delete_edge(NodeId u, NodeId v);
    // only for undirected graphs
    void add_single_edge(NodeId u, NodeId v, int label);

    size_t num_nodes(void);
    size_t num_edges(void);

    // returns true if the graph is (weakly) connected.
    bool is_connected(void);

private:
    NodeId m_next_NodeId;

    // private helper function for is_connected()
    void _visit(NodeId u, std::unordered_set<NodeId> &visited);
};

#endif
