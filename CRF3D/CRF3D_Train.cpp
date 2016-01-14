#include <iostream>
#include "Encoder.h"
#include "Params.h"
namespace CRF3D
{
	using std::cout;
	using std::endl;
    bool CRF3D_Train(Params &para)
    {
		Encoder encoder(para);
		
		cout << "Reading data..." << endl;
        encoder.read();
		cout << "It contains " << encoder.featureCap.getSetsNum() <<" sets" << endl;
		cout << "Nodes number: " << encoder.featureIndex.getSizeOfX() << endl;
		cout << "Features number: " << encoder.featureIndex.getSizeOfFeatures() << " features" << endl;

		cout << "Training..." << endl;
        encoder.learn();
		
		cout << "Saving model..." << endl;
		encoder.save();
		
		cout << "Done!\n" << endl;

        return true;
    }
}
