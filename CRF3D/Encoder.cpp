#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <set>
#include "Encoder.h"
#include "Params.h"
#include "FeatureIndex.h"
#include "Lbfgs.h"

namespace CRF3D
{
	using std::cout;
	using std::endl;
	using std::string;
	using std::vector;
	using std::set;
	using std::pair;
    class Tagger;
	bool Encoder::getParams(int argc,char** argv)
	{
		param.init_configs(argc, argv);
		bool a = param.ifHelp();
		bool b = param.ifVersion();
		if (param.ifHelp() || param.ifVersion())
		{
			if (param.ifHelp())cout << param.getHelpStr() << "\n";
			if (param.ifVersion())cout << param.getVersion() << "\n";
			return false;
		}
		return true;
	}

	bool Encoder::initTaggerVector(size_t dataSetsNum)
	{
		//resize taggerVector to trainSetNum, set the featureIndexPtr of each Tagger
		size_t taggerVectorSize = dataSetsNum * param.getBlocksNumPerImage();
		taggerVector.resize(taggerVectorSize);
		for (auto &eachTagger : taggerVector)eachTagger.setFeatureIndex(&featureIndex);
		return true;
	}

	bool Encoder::read()
	{
		featureCap.setParamPtr(&param);//associate featureCap to param
		featureCap.setFeatureIndexPtr(&featureIndex);//associate featureCap to featureIndex
		featureCap.readTrainFile(param.getTrainFileName()); // read trainFile	
		featureCap.readTemplateFile(param.getTemplateFileName()); //read templateFile
		// init taggerVector(include size,featureIndexPtr..) 
		initTaggerVector(featureCap.getDataSetsNum());
		featureCap.readTags();
		
		const auto &markedPlaces = param.markedPlaces;
		size_t blocksNumPerImage = markedPlaces.size();	
		size_t dataSetsNum = featureCap.getDataSetsNum(); // the number of data sets 
		// build feature
		for (size_t trainSetId = 0; trainSetId < dataSetsNum; ++trainSetId)
		{
			featureCap.setTrainSetId(trainSetId);
			featureCap.buildFeatureMap();// build featureMap for each set

			featureCap.readAnswers(); // read answers for each set
			size_t taggerVectorIdx = trainSetId*blocksNumPerImage;
			for (const auto &eachMarkedPlace : markedPlaces)
			{
				Tagger &eachTagger = taggerVector[taggerVectorIdx++];
				eachTagger.read(featureCap, eachMarkedPlace);// to read data and build features
			}
			featureCap.clear();
		}
		return true;
	}

	bool Encoder::save()
	{		
		featureIndex.save(param);
		return true;
	}

	bool Encoder::runCRF()
	{
		featureIndex.initAlpha();
		size_t featureNum = featureIndex.getSizeOfFeatures();
		
		vector<double>expected(featureNum);
		LBFGS lbfgs;
		size_t totalNodesNum = 0;
		
		for (size_t eachTaggerIdx = 0; eachTaggerIdx < taggerVector.size(); ++eachTaggerIdx)
		{
			taggerVector[eachTaggerIdx].setNodeConfigs();
			totalNodesNum += taggerVector[eachTaggerIdx].getSizeOfNodes();
		}
		featureIndex.setSizeOfX(totalNodesNum);
		
		double preObj = 1e+37;
		int convergeTimes = 0;
		auto &alpha = featureIndex.getAlpha();

		auto &fm=featureIndex.featureDict;
		auto &fc = featureIndex.featureCache;
		
		for (size_t itrTimes = 0; itrTimes < param.getMaxIter(); ++itrTimes)
		{
			//for each iteration
			//run begins
			double obj = 0.0;
			size_t errNum = 0;
			std::fill(expected.begin(), expected.end(), 0.0);
			for (size_t eachTaggerIdx = 0; eachTaggerIdx < taggerVector.size(); ++eachTaggerIdx)
			{
				obj += taggerVector[eachTaggerIdx].calcGradient(&expected[0]);
				size_t error = taggerVector[eachTaggerIdx].eval();
				errNum += error;
			}

			size_t nonZeroNum = 0;
			for (size_t eachAlphaIdx = 0; eachAlphaIdx < featureNum ; ++eachAlphaIdx) 
			{
				obj += (alpha[eachAlphaIdx] * alpha[eachAlphaIdx] / (2.0 * param.getCost()));
				expected[eachAlphaIdx] += alpha[eachAlphaIdx]/param.getCost();
				if (alpha[eachAlphaIdx] != 0.0) ++nonZeroNum;
			}
			double diff = itrTimes == 0 ? 1 : abs((preObj - obj) / obj);
			preObj = obj;
			if (diff < param.getEta()) convergeTimes++; 
			else convergeTimes = 0;
			if (convergeTimes >= 3) break;

			cout.precision(6);
			cout << "iter="  << itrTimes
				 << " errorNum="  <<errNum
				 << " obj="  << obj
				 << " diff=" <<diff << endl;

			if (lbfgs.optimize(featureNum, &alpha[0],obj, &expected[0], false, param.getCost()) <= 0)
				return false;
		}
		return true;
	}

    bool Encoder::learn()
    {
		runCRF();
        return true;
    }

	Encoder::~Encoder()
	{
	}
}


