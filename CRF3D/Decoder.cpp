#include "Decoder.h"
namespace CRF3D
{
	bool Decoder::read()
	{
		featureCap.setParamPtr(&param);//associate featureCap to param
		featureCap.setFeatureIndexPtr(&featureIndex);//associate featureCap to featureIndex	
		featureIndex.readModel(param);
		featureCap.readTestFile(param.getTestFileName()); // read testFile		
		featureCap.readTemplateFile(param.getTemplateFileName()); //read templateFile
		// init taggerVector(include size,featureIndexPtr..) 
		initTaggerVector(featureCap.getDataSetsNum());

		const auto &markedPlaces = param.markedPlaces;
		size_t blocksNumPerImage = markedPlaces.size();

		// build feature
		for (size_t trainSetId = 0; trainSetId < featureCap.getDataSetsNum(); ++trainSetId)
		{
			featureCap.setTrainSetId(trainSetId);
			featureCap.buildFeatureMap();
			featureCap.readAnswers();
			size_t taggerVectorIdx = trainSetId * blocksNumPerImage;
			for (const auto &eachMarkedPlace : markedPlaces)
			{
				Tagger &eachTagger = taggerVector[taggerVectorIdx++];
				eachTagger.read(featureCap, eachMarkedPlace);// to read data and build features
			}
			featureCap.clear();
		}
		return true;
		
	}

	bool Decoder::initTaggerVector(size_t dataSetsNum)
	{
		//resize taggerVector to trainSetNum, set the featureIndexPtr of each Tagger
		size_t taggerVectorSize = dataSetsNum * param.getBlocksNumPerImage();
		taggerVector.resize(taggerVectorSize);
		for (auto &eachTagger : taggerVector)eachTagger.setFeatureIndex(&featureIndex);
		return true;
	}

	bool Decoder::test()
	{
		size_t taggerSize=taggerVector.size();
		for (size_t taggerIdx = 0; taggerIdx < taggerSize; ++taggerIdx)
		{
			taggerVector[taggerIdx].setNodeConfigs();
			taggerVector[taggerIdx].buildLattice();
			taggerVector[taggerIdx].forwardbackward();
			taggerVector[taggerIdx].viterbi();
			taggerVector[taggerIdx].releaseNodeAndPath();
		}
		return true;
	}

	bool Decoder::save()
	{
		std::ofstream fout(param.getResultFileName().c_str());
		size_t taggerSize = taggerVector.size();
		size_t nodesNum = 0;
		size_t correct = 0;
		size_t correctCloudsNum = 0;
		size_t cloudsNum = 0;
		size_t correctSkysNum = 0;
		size_t skysNum = 0;
		size_t imageNum = 4;
		size_t nodesNumForASet[4]{ 0 };
		size_t correctNumForASet[4]{ 0 };
		size_t correctCloudForASet[4]{ 0 };
		size_t cloudsNumForASet[4]{ 0 };
		size_t correctSkyForASet[4]{ 0 };
		size_t SkysNumForASet[4]{ 0 };
		size_t counter = 0;
		for (auto &eachTagger:taggerVector)
		{
			size_t nodeSize = eachTagger.getSizeOfNodes();
			for (size_t tagIdx = 0; tagIdx < nodeSize; ++tagIdx )
			{
				++nodesNum;
				++nodesNumForASet[tagIdx%nodeSize];
				string ansStr = featureIndex.getTag(eachTagger.answer[tagIdx]);
				string resultStr = featureIndex.getTag(eachTagger.result[tagIdx]);
				if (ansStr == "Sky")
				{
					++skysNum;
				}
				else if (ansStr != "Others" && ansStr != "HighLightCloud")
				{
					++cloudsNum;
				}
				if (ansStr == resultStr)
				{
					++correct;
					++correctNumForASet[tagIdx%nodeSize];
					if (ansStr == "Sky")
					{
						++correctSkysNum;
					}
					else if (ansStr != "Others" && ansStr != "HighLightCloud")
					{
						++correctCloudsNum;						
					}
					fout << "answer: " << ansStr <<" result:" << resultStr << endl;
				}
			}
			++counter;
		}
		cout << "Total nodes num:" << nodesNum << endl;
		cout << "Correct rate: "<<(double)correct / (double)nodesNum << endl;
		cout << "Total skys num: " << skysNum << endl;
		cout << "Sky correct rate: " << (double)correctSkysNum / skysNum << endl;
		cout << "Total clouds num: " << cloudsNum << endl;
		cout << "Cloud correct rate: " << (double)correctCloudsNum / cloudsNum << endl;
		fout << "Total nodes num:" << nodesNum << endl;
		fout << "Correct rate: " << (double)correct / (double)nodesNum << endl;
		fout << "Total skys num: " << skysNum << endl;
		fout << "Sky correct rate: " << (double)correctSkysNum / skysNum << endl;
		fout << "Total clouds num: " << cloudsNum << endl;
		fout << "Cloud correct rate: " << (double)correctCloudsNum / cloudsNum << endl;
		for (size_t idx = 0; idx < 4; ++idx)
		{
			cout << "Image " << idx << endl;
			cout << "Correct rate:" << ((double)correctNumForASet[idx] / nodesNumForASet[idx]) << endl;
		}
		for (size_t idx = 0; idx < 4; ++idx)
		{
			fout << "Image " << idx << endl;
			fout << "Correct rate:" << ((double)correctNumForASet[idx] / nodesNumForASet[idx]) << endl;
		}
		return true;
	}
}