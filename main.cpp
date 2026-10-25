#include <iostream>
#include <string>
#include "graph.h"
using namespace std;

int passed = 0, failed = 0;

void check(const string& what, bool ok)
{
	if (ok) {
		cout << "  [PASS] " << what << endl;
		passed++;
	}
	else {
		cout << "  [FAIL] " << what << endl;
		failed++;
	}
}

bool isSpanningTree(const Graph& tree, int n)
{
	return tree.edgeCount() == 2 * (n - 1) && tree.isConnected();
}

int main()
{
	int sample[9][9] = {
		//  A  B  C  D  E  F  G  H  I
		{   0, 2, 1, 7, 0, 0, 0, 0, 0 },   // A
		{   2, 0, 5, 5, 0, 0, 0, 0, 0 },   // B
		{   1, 5, 0, 4, 0, 9, 0, 0, 0 },   // C
		{   7, 5, 4, 0, 8, 0, 0, 0, 0 },   // D
		{   0, 0, 0, 8, 0, 3, 7, 0, 0 },   // E
		{   0, 0, 9, 0, 3, 0, 5, 10, 0 },  // F
		{   0, 0, 0, 0, 7, 5, 0, 11, 6 },  // G
		{   0, 0, 0, 0, 0, 10, 11, 0, 3 }, // H
		{   0, 0, 0, 0, 0, 0, 6, 3, 0 }    // I
	};

	Graph g(sample);

	cout << "===== The original graph =====" << endl;
	g.printEdges();
	cout << "Total weight of all edges: " << g.totalWeight() << endl;

	Graph mst = g.minimumSpanningTree();

	cout << endl << "===== Minimum spanning tree (Kruskal) =====" << endl;
	mst.printEdges();
	cout << "Number of edges: " << mst.edgeCount() / 2
		<< "  (a tree with " << mst.nodeCount() << " nodes needs " << mst.nodeCount() - 1 << ")" << endl;
	cout << "Total weight of the tree: " << mst.totalWeight() << endl;
	cout << "All nodes joined in one piece: " << (mst.isConnected() ? "Yes" : "No") << endl;

	cout << endl << "===== Self-check =====" << endl;

	check("sample graph: the tree weighs 32", mst.totalWeight() == 32);
	check("sample graph: the result is a spanning tree", isSpanningTree(mst, 9));

	int triangle[3][3] = { {0,1,3}, {1,0,2}, {3,2,0} };
	Graph t1 = Graph(triangle).minimumSpanningTree();
	check("triangle: total weight is 1 + 2 = 3", t1.totalWeight() == 3);
	check("triangle: the heaviest edge (3) is left out", isSpanningTree(t1, 3));

	int allOnes[4][4] = { {0,1,1,1}, {1,0,1,1}, {1,1,0,1}, {1,1,1,0} };
	Graph t2 = Graph(allOnes).minimumSpanningTree();
	check("4 nodes, every edge weighs 1: tree has 3 edges, total 3",
		isSpanningTree(t2, 4) && t2.totalWeight() == 3);

	int line[3][3] = { {0,4,0}, {4,0,6}, {0,6,0} };
	Graph t3 = Graph(line).minimumSpanningTree();
	check("graph that is already a tree: nothing is removed",
		isSpanningTree(t3, 3) && t3.totalWeight() == 10);

	int withLoop[3][3] = { {9,1,3}, {1,0,2}, {3,2,0} };
	Graph t4 = Graph(withLoop).minimumSpanningTree();
	check("loop on A is ignored", isSpanningTree(t4, 3) && t4.totalWeight() == 3);

	int oneWay[2][2] = { {0,2}, {3,0} };
	Graph t5 = Graph(oneWay).minimumSpanningTree();
	check("A->B = 2, B->A = 3: the cheaper edge (2) is used",
		isSpanningTree(t5, 2) && t5.totalWeight() == 2);

	int twoPieces[4][4] = { {0,1,0,0}, {1,0,0,0}, {0,0,0,2}, {0,0,2,0} };
	Graph g6(twoPieces);
	Graph t6 = g6.minimumSpanningTree();
	check("two separate pieces: one tree per piece (2 edges, total 3)",
		!g6.isConnected() && t6.edgeCount() == 4 && t6.totalWeight() == 3);

	int single[1][1] = { {0} };
	Graph t7 = Graph(single).minimumSpanningTree();
	check("single node: tree has no edges", t7.edgeCount() == 0 && t7.totalWeight() == 0);

	cout << endl << "Passed: " << passed << "   Failed: " << failed << endl;
	if (failed == 0)
		cout << "All checks passed!" << endl;

	return failed == 0 ? 0 : 1;
}