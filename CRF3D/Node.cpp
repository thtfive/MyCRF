#include <vector>
#include <string>
#include <cstring>
#include <map>
#include <iostream>
#include "Node.h"
namespace CRF3D {
	using std::cout;
	using std::endl;
    void Node::calcAlpha() {
        alpha = 0.0;
		for (auto it = leftPath.begin(); it != leftPath.end(); ++it)
			alpha = logsumexp(alpha, (*it)->cost + (*it)->leftNode->alpha, (it == leftPath.begin()));
		alpha += cost;
    }

    void Node::calcBeta() {
	  beta = 0.0;
	  for (auto it = rightPath.begin(); it != rightPath.end(); ++it)
		  beta = logsumexp(beta,(*it)->cost + (*it)->rightNode->beta,(it == rightPath.begin()));
	  beta += cost;
    }

    void Node::calcExpectation(double *expected, double Z, size_t tagsize) const
    {
		const double c = std::exp(alpha + beta - cost - Z);
		for(const auto &featureId: *fVector) expected[featureId + y] += c;
		for (auto it = leftPath.begin(); it != leftPath.end(); ++it)
			(*it)->calcExpectation(expected, Z, tagsize);
    }
}
