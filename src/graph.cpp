#include "graph.h"
#include <algorithm> // for std::minmax(u, v)
#include <cstdint> // for cast to int32_t
#include <iostream> // for debugging


// The key is used as an index for the unordered_map edge_labels
// Only works for NodeIds up to 2^32 - 1
size_t Graph::edge2key(NodeId u, NodeId v) {
    if (m_directed) {
        return u << 32 | (uint32_t) v;
    } else {
        auto e = std::minmax(u, v);
        return e.first << 32 | (uint32_t) e.second;
    }
}

std::pair<NodeId, NodeId> Graph::key2edge(size_t key) {
    NodeId v = (uint32_t) key;
    NodeId u = key >> 32;
    return std::make_pair(u, v);
}

NodeId Graph::add_node(int label, NodeId u) {
    if (u == DEFAULT()) {
        u = m_next_NodeId;
    }

    outgoing[u] = NodeSet(); // are the parentheses correct?
    if (m_directed) {
        incoming[u] = NodeSet(); // are the parentheses correct?
    }
    node_labels[u] = label;

    m_next_NodeId = u+1;
    return u;
}

void Graph::delete_isolated_node(NodeId u) {
    outgoing.erase(u);
    if (m_directed) {
        incoming.erase(u);
    }
    node_labels.erase(u);
}

void Graph::add_edge(NodeId u, NodeId v, int label) {
    outgoing[u].insert(v);
    if (m_directed) {
        incoming[v].insert(u);
    } else {
        outgoing[v].insert(u);
    }
    edge_labels[edge2key(u,v)] = label;
}

// only for undirected graphs
void Graph::add_single_edge(NodeId u, NodeId v, int label) {
    outgoing[u].insert(v);
    edge_labels[edge2key(u,v)] = label;
}

void Graph::delete_edge(NodeId u, NodeId v) {
    outgoing[u].erase(v);
    if (m_directed) {
        incoming[v].erase(u);
    } else {
        outgoing[v].erase(u);
    }
    edge_labels.erase(edge2key(u,v));
}

size_t Graph::num_nodes() {
    return node_labels.size();
}

size_t Graph::num_edges() {
    return edge_labels.size();
}

void Graph::_visit(NodeId u, std::unordered_set<NodeId> &visited) {
    visited.insert(u);
    for (const NodeId &v : outgoing[u]) {
        auto it = visited.find(v);
        // if v has not been visited before
        if (it == visited.end()) {
            _visit(v, visited);
        }
    }
    // Note: we are looking at the underlying undirected graph
    if (m_directed) {
        for (const NodeId &v : incoming[u]) {
            auto it = visited.find(v);
            // if v has not been visited before
            if (it == visited.end()) {
                _visit(v, visited);
            }
        }
    }
}

// Note that is_connected() returns true for the empty graph.
bool Graph::is_connected() {
    if (num_nodes() == 0) {
        return true;
    }

    std::unordered_set<NodeId> visited;

    // get any valid NodeId
    NodeId u;
    for (const auto &kv : node_labels) {
        u = kv.first;
        break;
    }

    _visit(u, visited);
    return visited.size() == num_nodes();
}
