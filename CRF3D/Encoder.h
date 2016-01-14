#ifndef CRF3D_ENCODER_H
#define CRF3D_ENCODER_H
#include <vector>
#include <set>
#include "Params.h"
#include "FeatureIndex.h"
#include "Tagger.h"
#include "FeatureCapture.h"

namespace CRF3D
{
	using std::vector;
    class Tagger;
	class EncoderFeatureIndex;
    class Encoder
    {
	public:
		Encoder(Params &para):param(para){}
		~Encoder();
		EncoderFeatureIndex   featureIndex;
		FeatureCapture        featureCap;
		vector<EncoderTagger> taggerVector;
		Params				 &param;
		bool getParams(int argc,char **argv);
		bool initTaggerVector(size_t dataSetsNum);
		bool read();
		bool save();
        bool learn();
		bool runCRF();		
    };
    bool CRF3D_Train(Params &para);	
}

#endif // CRF3D_ENCODER_H
