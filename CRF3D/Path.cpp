#include "Path.h"
#include "Node.h"
namespace CRF3D
{
	void Path::calcExpectation(double *expected, double Z, size_t tagsize) const {
		const double c = std::exp(leftNode->alpha + cost + rightNode->beta - Z);
		for (const auto &featureId: *fVector)
			expected[featureId + leftNode->y * tagsize + rightNode->y] += c;
	}

	void Path::add(Node *lnode, Node *rnode) {
		leftNode = lnode;
		rightNode = rnode;
		leftNode->rightPath.push_back(this);
		rightNode->leftPath.push_back(this);
	}
}