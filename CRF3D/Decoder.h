#ifndef CRF3D_DECODER_H
#define CRF3D_DECODER_H
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
	class DecoderFeatureIndex;
	class Decoder
	{
	public:
		Decoder(Params &para):param(para){ }
		~Decoder(){}
		DecoderFeatureIndex    featureIndex;
		FeatureCapture         featureCap;
		vector<DecoderTagger>  taggerVector;
		Params				  &param;
		bool read();
		bool initTaggerVector(size_t dataSetsNum);
		bool test();
		bool save();
	};
	bool CRF3D_Test(Params &para);
}

#endif // CRF3D_DECODER_H

