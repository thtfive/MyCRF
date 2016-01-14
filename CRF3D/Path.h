#ifndef CRF3D_PATH_H
#define CRF3D_PATH_H
#include "Node.h"
#include <vector>
namespace CRF3D
{
	using std::vector;
	class Node;
	class Path {
	public:
		Node   *rightNode = 0;
		Node   *leftNode = 0;
		vector<int>    *fVector;
		double  cost = 0.0;

		Path() : rightNode(0), leftNode(0), fVector(0), cost(0.0) {}

		// for CRF
		void calcExpectation(double *expected, double, size_t) const;
		void add(Node *_lnode, Node *_rnode);

		void clear() {
			rightNode = leftNode = 0;
			fVector = 0;
			cost = 0.0;
		}
	};
}
#endif // CRF3D_PATH_H
