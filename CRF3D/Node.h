#ifndef CRF3D_NODE_H
#define CRF3D_NODE_H

#include <vector>
#include <cmath>
#include <algorithm>
#include "path.h"

const double  LOG2              = 0.69314718055;
const double  MINUS_LOG_EPSILON = 50.0;
inline double logsumexp(double x, double y, bool flg) {
	if (flg) return y;  // init mode
	const double vmin = std::min(x, y);
	const double vmax = std::max(x, y);
	if (vmax > vmin + MINUS_LOG_EPSILON) {
		return vmax;
	}
	else {
		return vmax + std::log(1.0 + std::exp(vmin - vmax));
	}
}
namespace CRF3D {
	using std::vector;
    class Path;
    class Node {
	public:
		unsigned int         x = 0;
		unsigned short int   y = 0;
		double               alpha = 0;
		double               beta = 0.0;
		double               cost = 0.0;
		double               bestCost = 0.0;
		Node                *prev = 0;
		vector<int>         *fVector = 0;
        vector<Path *>       leftPath;
        vector<Path *>       rightPath;

        void calcAlpha();
        void calcBeta();
        void calcExpectation(double *expected, double, size_t) const;

        void clear()
        {
            x = y = 0;
            alpha = beta = cost = 0.0;
            prev = 0;
            fVector = 0;
            leftPath.clear();
            rightPath.clear();
        }
        void shrink()
        {
        }
        Node(): x(0), y(0), alpha(0.0), beta(0.0),
              cost(0.0), bestCost(0.0), prev(0), fVector(0) {}
    };
}
#endif // CRF3D_NODE_H
