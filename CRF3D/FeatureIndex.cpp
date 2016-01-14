#include <map>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <set>
#include "FeatureIndex.h"
#include "Node.h"
#include "Tagger.h"
#include "Params.h"

namespace CRF3D {
	using std::string;
	using std::endl;
	int FeatureIndex::getAnswerInt(const string & answerStr)
	{
		for (size_t yIdx = 0; yIdx < ySet.size(); ++yIdx)
		{
			if (ySet[yIdx] == answerStr)return yIdx;
		}
		return -1;
	}

    size_t FeatureIndex::getID(const string &key)
    {
        auto it = featureDict.find(key);
        if (it == featureDict.end())
        {
            featureDict.insert(std::make_pair(key, std::make_pair(maxId,1)));
            int n = maxId;
            maxId += ySet.size();
            return n;
        }
        else
        {
            it->second.second++;
            return it->second.first;
        }
        return -1;
    }

	int FeatureIndex::getYValue(const string &yName)
	{
		for (size_t eachY = 0; eachY < ySet.size(); ++eachY)
		{
			if (ySet[eachY] == yName)return eachY;
		}
		auto newYSize = ySet.size() + 1;
		ySet.resize(newYSize);
		ySet.push_back(yName);
		return newYSize - 1;
	}

	bool FeatureIndex::save(Params &param)
	{
		//filename2 += ".txt";
		string modelFileName = param.getModelFileName();
		std::ofstream oFileStream(modelFileName.c_str());

		// header
		oFileStream << "version:         " << param.getVersion() << endl;
		oFileStream << "cost_factor:     " << getCostFactor() << std::endl;
		oFileStream << "features_number: " << getMaxId() << std::endl;
		oFileStream << "node_number:     " << getSizeOfX() << std::endl;
		oFileStream << endl;

		// tags
		size_t tagsNums = getSizeOfTags();
		oFileStream << getSizeOfTags() << endl;
		for (size_t eachTagIdx = 0; eachTagIdx < tagsNums; ++eachTagIdx)
		{
			oFileStream << getTag(eachTagIdx) << endl;
		}
		oFileStream << endl;

		// template
		oFileStream << unigramTempls.size() << endl;
		for (size_t eachUTIdx = 0; eachUTIdx < unigramTempls.size(); ++eachUTIdx)
		{
			oFileStream << unigramTempls[eachUTIdx] << endl;
		}
		oFileStream << bigramTempls.size() << endl;
		for (size_t eachBTIdx = 0; eachBTIdx < bigramTempls.size(); ++eachBTIdx)
		{
			oFileStream << bigramTempls[eachBTIdx] << endl;
		}
		oFileStream << endl;

		oFileStream << featureDict.size() << endl;
		for (auto &fDIter = featureDict.cbegin(); fDIter != featureDict.cend(); ++fDIter)
		{
			oFileStream << std::setw(6) << std::left << fDIter->second.first << " " << fDIter->first << endl;
		}
		oFileStream << std::endl;

		oFileStream.setf(std::ios::fixed, std::ios::floatfield);
		oFileStream.precision(16);
		auto &alpha = getAlpha();
		oFileStream << alpha.size() << endl;
		for (auto &eachAlpha : alpha)
		{
			oFileStream << eachAlpha << endl;
		}
		return true;
	}

	
    void FeatureIndex::shrink(int freq) 
	{

    }

	void FeatureIndex::calcCost(Node *node)
	{
		node->cost = 0.0;                                               
        double c = 0;                                                            
		for (const auto &featureId: *(node->fVector)) c += alpha[featureId + node->y];       
		node->cost = costFactor * c;
	}

	void FeatureIndex::calcCost(Path *path)
	{
		path->cost = 0.0;
		double c = 0.0;
		for (auto &featureId : *(path->fVector))
			c += alpha[featureId + path->leftNode->y * ySet.size() + path->rightNode->y];
		path->cost = costFactor * c;
	}

	void FeatureIndex::rebuildFeature(Tagger &tagger)
	{
		size_t fid = tagger.getFeatureCacheBeginIdx();//get featureCacheBeginIdx for this tagger
		auto sizeYSet = ySet.size();
		for (size_t eachNodeIdx = 0; eachNodeIdx < tagger.getSizeOfNodes(); ++eachNodeIdx) 
		{// tagger->size() return the nodes number of tagger
			vector<int> &f = featureCache[fid++];
			for (size_t eachTagIdx = 0; eachTagIdx < ySet.size(); ++eachTagIdx) 
			{
				Node *nodePtr = new Node;
				nodePtr->x = eachNodeIdx;
				nodePtr->y = eachTagIdx;
				nodePtr->fVector = &f;
				tagger.setNode(nodePtr, eachNodeIdx, eachTagIdx);
			}
		}

		for (size_t eachNodeIdx = 1; eachNodeIdx < tagger.getSizeOfNodes(); ++eachNodeIdx) 
		{
			auto &f= featureCache[fid++];
			for (size_t preTagIdx = 0; preTagIdx < ySet.size(); ++preTagIdx) 
			{
				for (size_t postTagIdx = 0; postTagIdx < ySet.size(); ++postTagIdx) 
				{
					Path *pathPtr = new Path;
					pathPtr->clear();
					pathPtr->add(tagger.getNode(eachNodeIdx-1,preTagIdx),tagger.getNode(eachNodeIdx, postTagIdx));
					pathPtr->fVector = &f;
				}
			}
		}
	}
	bool EncoderFeatureIndex::save(Params & param)
	{
		//filename2 += ".txt";
		string modelFileName = param.getModelFileName();
		std::ofstream oFileStream(modelFileName.c_str());

		// header
		oFileStream << "version:         " << param.getVersion() << endl;
		oFileStream << "cost_factor:     " << getCostFactor() << std::endl;
		oFileStream << "features_number: " << getMaxId() << std::endl;
		oFileStream << "node_number:     " << getSizeOfX() << std::endl;
		oFileStream << endl;

		// tags
		size_t tagsNums = getSizeOfTags();
		oFileStream << getSizeOfTags() << endl;
		for (size_t eachTagIdx = 0; eachTagIdx < tagsNums; ++eachTagIdx)
		{
			oFileStream << getTag(eachTagIdx) << endl;
		}
		oFileStream << endl;

		// template
		oFileStream << unigramTempls.size() << endl;
		for (size_t eachUTIdx = 0; eachUTIdx < unigramTempls.size(); ++eachUTIdx)
		{
			oFileStream << unigramTempls[eachUTIdx] << endl;
		}
		oFileStream << bigramTempls.size() << endl;
		for (size_t eachBTIdx = 0; eachBTIdx < bigramTempls.size(); ++eachBTIdx)
		{
			oFileStream << bigramTempls[eachBTIdx] << endl;
		}
		oFileStream << endl;

		oFileStream << featureDict.size() << endl;
		for (auto &fDIter = featureDict.cbegin(); fDIter != featureDict.cend(); ++fDIter)
		{
			oFileStream << std::setw(6) << std::left << fDIter->second.first << " " << fDIter->first << endl;
		}
		oFileStream << std::endl;

		oFileStream.setf(std::ios::fixed, std::ios::floatfield);
		oFileStream.precision(16);
		auto &alpha = getAlpha();
		oFileStream << alpha.size() << endl;
		for (auto &eachAlpha : alpha)
		{
			oFileStream << eachAlpha << endl;
		}
		return true;
	}

	bool DecoderFeatureIndex::save(Params & param)
	{
		string resultFileName = param.getResultFileName();
		std::ofstream oFileStream(resultFileName.c_str());

		return true;
	}

	bool DecoderFeatureIndex::readModel(Params & param)
	{
		string modelFileName = param.getModelFileName();
		std::ifstream iFileStream(modelFileName.c_str());
		char charBuffer[8096];
		string preStr;
		// header
		string versionStr;
		iFileStream.getline(charBuffer, 8096);
		std::istringstream iStrStream(charBuffer);
		iStrStream >> preStr;
		while (iStrStream >> preStr)
		{
			if (!versionStr.empty())versionStr += " ";
			versionStr += preStr;
		}
		param.setVersion(versionStr);

		double newCostFactor = 1.0;
		iFileStream >> preStr;
		iFileStream >> newCostFactor;
		setCostFactor(newCostFactor);

		iFileStream >> preStr;
		size_t newMaxId;
		iFileStream >> newMaxId;
		setMaxId(newMaxId);

		iFileStream >> preStr;
		size_t modelNodeNum;
		iFileStream >> modelNodeNum;

		// read tags
		size_t tagsNum = 0;
		iFileStream >> tagsNum;
		setSizeOfTags(tagsNum);
		for (size_t eachTagIdx = 0; eachTagIdx < tagsNum; ++eachTagIdx)
		{
			string tagStr;
			iFileStream >> tagStr;
			setTag(eachTagIdx, tagStr);
		}

		// read template
		size_t sizeOfUnigramTempl = 0;
		iFileStream >> sizeOfUnigramTempl;
		iFileStream.get();
		for (size_t eachUTIdx = 0; eachUTIdx < sizeOfUnigramTempl; ++eachUTIdx)
		{
			iFileStream.getline(charBuffer, 8096);
			string uTStr = charBuffer;
			unigramTempls.push_back(uTStr);
		}

		size_t sizeOfBigramTempl = 0;
		iFileStream >> sizeOfBigramTempl;
		iFileStream.get();
		for (size_t eachBTIdx = 0; eachBTIdx < sizeOfBigramTempl; ++eachBTIdx)
		{
			iFileStream.getline(charBuffer, 8096);
			string bTStr = charBuffer;
			bigramTempls.push_back(bTStr);
		}

		size_t sizeOfFeatureDict = 0;
		iFileStream >> sizeOfFeatureDict;
		iFileStream.get();
		for (size_t fDIdx = 0; fDIdx < sizeOfFeatureDict; ++fDIdx)
		{
			size_t fDSecondFirst;
			string strTmp;
			iFileStream >> fDSecondFirst;
			iFileStream.getline(charBuffer, 8096);
			strTmp = charBuffer;
			auto bIdx = strTmp.find_first_not_of(' ');
			string fDFirstStr(strTmp.begin() + bIdx, strTmp.end());
			featureDict.insert(std::make_pair(fDFirstStr, std::make_pair(fDSecondFirst, 0)));
		}

		size_t alphaSize = 0;
		iFileStream >> alphaSize;
		setAlphaSize(alphaSize);
		for (size_t alphaIdx = 0; alphaIdx < alphaSize; ++alphaIdx)
		{
			double eachAlpha = 0.0;
			iFileStream >> eachAlpha;
			setAlpha(alphaIdx, eachAlpha);
		}
		return true;
	}
}
