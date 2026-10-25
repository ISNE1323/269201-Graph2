#ifndef GRAPH_H
#define GRAPH_H

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>

class Node {
public:
	std::string name;
	Node(const std::string& n = "") : name(n) {}
};

class Edge {
public:
	int from, to;
	int weight;
	Edge(int f = 0, int t = 0, int w = 1) : from(f), to(t), weight(w) {}
};

class Graph {
public:
	Graph() {}

	template<int N>
	Graph(const int (&matrix)[N][N]) {
		for (int i = 0; i < N; i++)
			addNode(defaultName(i));
		for (int i = 0; i < N; i++)
			for (int j = 0; j < N; j++)
				if (matrix[i][j] != 0)
					addEdge(i, j, matrix[i][j]);
	}

	void addNode(const std::string& name);
	bool addEdge(int from, int to, int weight = 1);   // false if a node number is wrong

	int nodeCount() const;
	int edgeCount() const;
	int componentCount() const;                      // how many separate pieces the graph has
	std::vector<std::string> isolatedNodes() const;  // nodes with no edge to any other node

	bool isMultigraph() const;    // two or more edges from the same node to the same node?
	bool isPseudograph() const;   // has a loop (an edge from a node to itself)?
	bool isDigraph() const;       // has an edge that can only be walked one way?
	bool isWeighted() const;      // has an edge whose weight is not 1?
	bool isComplete() const;      // is every node joined to every other node?
	bool isDisjointed() const;    // is the graph in 2 or more separate pieces (e.g. isolated nodes)?
	bool isConnected() const;     // is the graph in ONE piece? (the opposite of disjointed)

	Graph minimumSpanningTree() const;   // Kruskal's algorithm (explained near its code below)
	int totalWeight() const;             // sum of all the weights, a two-way edge counted once

	void print(std::ostream& out = std::cout) const;            // nodes and edges
	void printEdges(std::ostream& out = std::cout) const;       // every edge once, e.g.  A - C  (1)
	void printProperties(std::ostream& out = std::cout) const;  // answers to the six questions

private:
	std::vector<Node> nodes;
	std::vector<Edge> edges;

	static std::string defaultName(int i);
	static const char* yesNo(bool b) { return b ? "Yes" : "No"; }
	static bool lighter(const Edge& a, const Edge& b);        // for sorting edges by weight
	static int findRoot(std::vector<int>& parent, int x);     // union-find: the leader of x's group
	bool hasMirror(const Edge& e) const;                      // is there a matching edge going back?
	bool countsOnce(const Edge& e) const;                     // true for one copy of each two-way edge
	std::vector<std::vector<bool> > adjacency() const;   // adj[i][j] is true if an edge i->j exists
};

//  Function bodies

inline std::string Graph::defaultName(int i)
{
	if (i < 26)
		return std::string(1, (char)('A' + i));
	return "N" + std::to_string(i);
}

inline void Graph::addNode(const std::string& name)
{
	nodes.push_back(Node(name));
}

inline bool Graph::addEdge(int from, int to, int weight)
{
	if (from < 0 || from >= nodeCount() || to < 0 || to >= nodeCount())
		return false;
	edges.push_back(Edge(from, to, weight));
	return true;
}

inline int Graph::nodeCount() const { return (int)nodes.size(); }
inline int Graph::edgeCount() const { return (int)edges.size(); }

inline std::vector<std::vector<bool> > Graph::adjacency() const
{
	int n = nodeCount();
	std::vector<std::vector<bool> > adj(n, std::vector<bool>(n, false));
	for (size_t k = 0; k < edges.size(); k++)
		adj[edges[k].from][edges[k].to] = true;
	return adj;
}

// ---- Multigraph ----------------------------------------------------

inline bool Graph::isMultigraph() const
{
	for (size_t a = 0; a < edges.size(); a++)
		for (size_t b = a + 1; b < edges.size(); b++)
			if (edges[a].from == edges[b].from && edges[a].to == edges[b].to)
				return true;
	return false;
}

// ---- Pseudograph ---------------------------------------------------

inline bool Graph::isPseudograph() const
{
	for (size_t k = 0; k < edges.size(); k++)
		if (edges[k].from == edges[k].to)
			return true;
	return false;
}

// ---- Digraph (directed graph) --------------------------------------

inline bool Graph::isDigraph() const
{
	for (size_t a = 0; a < edges.size(); a++) {
		if (edges[a].from == edges[a].to)
			continue;                       // a loop has no direction
		bool hasMirror = false;
		for (size_t b = 0; b < edges.size(); b++) {
			if (edges[b].from == edges[a].to && edges[b].to == edges[a].from
				&& edges[b].weight == edges[a].weight) {
				hasMirror = true;
				break;
			}
		}
		if (!hasMirror)
			return true;
	}
	return false;
}

// ---- Weighted graph ------------------------------------------------

inline bool Graph::isWeighted() const
{
	for (size_t k = 0; k < edges.size(); k++)
		if (edges[k].weight != 1)
			return true;
	return false;
}

// ---- Complete graph ------------------------------------------------

inline bool Graph::isComplete() const
{
	int n = nodeCount();
	std::vector<std::vector<bool> > adj = adjacency();
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++)
			if (i != j && !adj[i][j])
				return false;
	return true;
}

inline std::vector<std::string> Graph::isolatedNodes() const
{
	std::vector<bool> hasEdge(nodes.size(), false);
	for (size_t k = 0; k < edges.size(); k++) {
		if (edges[k].from != edges[k].to) {       // a loop does not connect to another node
			hasEdge[edges[k].from] = true;
			hasEdge[edges[k].to] = true;
		}
	}
	std::vector<std::string> result;
	for (size_t i = 0; i < nodes.size(); i++)
		if (!hasEdge[i])
			result.push_back(nodes[i].name);
	return result;
}

inline int Graph::componentCount() const
{
	int n = nodeCount();
	std::vector<std::vector<bool> > adj = adjacency();
	std::vector<bool> visited(n, false);
	int pieces = 0;

	for (int start = 0; start < n; start++) {
		if (visited[start])
			continue;
		pieces++;
		std::vector<int> toVisit(1, start);
		visited[start] = true;
		while (!toVisit.empty()) {
			int current = toVisit.back();
			toVisit.pop_back();
			for (int next = 0; next < n; next++) {
				if (!visited[next] && (adj[current][next] || adj[next][current])) {
					visited[next] = true;
					toVisit.push_back(next);
				}
			}
		}
	}
	return pieces;
}

inline bool Graph::isDisjointed() const
{
	return componentCount() > 1;
}

inline bool Graph::isConnected() const
{
	return !isDisjointed();
}

//  Graph #2: Minimum Spanning Tree (Kruskal's algorithm)

inline bool Graph::lighter(const Edge& a, const Edge& b)
{
	return a.weight < b.weight;
}

inline int Graph::findRoot(std::vector<int>& parent, int x)
{
	while (parent[x] != x) {
		parent[x] = parent[parent[x]];   // shortcut, so the next search is faster
		x = parent[x];
	}
	return x;
}

inline Graph Graph::minimumSpanningTree() const
{
	Graph tree;
	for (size_t i = 0; i < nodes.size(); i++)
		tree.addNode(nodes[i].name);

	// 1. sort a copy of the edges, lightest first (stable: equal weights keep their order)
	std::vector<Edge> sorted = edges;
	std::stable_sort(sorted.begin(), sorted.end(), lighter);

	// 2. every node starts in a group of its own
	int n = nodeCount();
	std::vector<int> parent(n);
	for (int i = 0; i < n; i++)
		parent[i] = i;

	int taken = 0;
	for (size_t k = 0; k < sorted.size() && taken < n - 1; k++) {
		const Edge& e = sorted[k];
		int groupA = findRoot(parent, e.from);
		int groupB = findRoot(parent, e.to);
		if (groupA != groupB) {                         // not connected yet: take the edge
			parent[groupA] = groupB;                    // merge the two groups
			tree.addEdge(e.from, e.to, e.weight);
			tree.addEdge(e.to, e.from, e.weight);
			taken++;
		}
	}
	return tree;
}

inline bool Graph::hasMirror(const Edge& e) const
{
	if (e.from == e.to)
		return false;                    // a loop has no "other direction"
	for (size_t k = 0; k < edges.size(); k++)
		if (edges[k].from == e.to && edges[k].to == e.from && edges[k].weight == e.weight)
			return true;
	return false;
}

inline bool Graph::countsOnce(const Edge& e) const
{
	if (!hasMirror(e))
		return true;
	return e.from < e.to;
}

inline int Graph::totalWeight() const
{
	int total = 0;
	for (size_t k = 0; k < edges.size(); k++)
		if (countsOnce(edges[k]))
			total += edges[k].weight;
	return total;
}

// Prints each edge once:  "A - C  (1)" for a two-way edge, "A -> C  (1)" for a one-way edge.
inline void Graph::printEdges(std::ostream& out) const
{
	bool any = false;
	for (size_t k = 0; k < edges.size(); k++) {
		const Edge& e = edges[k];
		if (!countsOnce(e))
			continue;
		out << "  " << nodes[e.from].name << (hasMirror(e) ? " - " : " -> ")
			<< nodes[e.to].name << "  (" << e.weight << ")\n";
		any = true;
	}
	if (!any)
		out << "  (no edges)\n";
}

// ---- Printing ------------------------------------------------------
inline void Graph::print(std::ostream& out) const
{
	out << "Nodes (" << nodeCount() << "):";
	for (size_t i = 0; i < nodes.size(); i++)
		out << " " << nodes[i].name;
	out << "\n";
	out << "Edges (" << edgeCount() << "), shown as  node: neighbour(weight) ...\n";
	for (size_t i = 0; i < nodes.size(); i++) {
		out << "  " << nodes[i].name << ":";
		bool any = false;
		for (size_t k = 0; k < edges.size(); k++) {
			if (edges[k].from == (int)i) {
				out << " " << nodes[edges[k].to].name << "(" << edges[k].weight << ")";
				any = true;
			}
		}
		if (!any)
			out << " (none)";
		out << "\n";
	}
}

inline void Graph::printProperties(std::ostream& out) const
{
	out << std::left;
	out << std::setw(28) << "Is it a multigraph?"      << yesNo(isMultigraph())  << "\n";
	out << std::setw(28) << "Is it a pseudograph?"     << yesNo(isPseudograph()) << "\n";
	out << std::setw(28) << "Is it a digraph?"         << yesNo(isDigraph())     << "\n";
	out << std::setw(28) << "Is it a weighted graph?"  << yesNo(isWeighted())    << "\n";
	out << std::setw(28) << "Is it a complete graph?"  << yesNo(isComplete())    << "\n";
	out << std::setw(28) << "Is it a disjointed graph?" << yesNo(isDisjointed()) << "\n";

	std::vector<std::string> isolated = isolatedNodes();
	out << std::setw(28) << "Isolated nodes:";
	if (isolated.empty()) {
		out << "none";
	}
	else {
		for (size_t i = 0; i < isolated.size(); i++)
			out << (i > 0 ? ", " : "") << isolated[i];
	}
	out << "\n";
}

#endif