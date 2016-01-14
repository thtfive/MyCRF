#include <iostream>
#include <vector>
#include <iterator>
#include <cmath>
#include <string>
#include <sstream>
#include <tuple>
#include <set>
#include "Params.h"
#include "Common.h"
#include "Tagger.h"
#include "FeatureCapture.h"
#include "FeatureIndex.h"
#include "Path.h"

namespace CRF3D {
	using std::string;
	using std::map;
	using std::set;
	using std::tuple;
	using std::pair;
	using std::make_pair;
	 
	void Tagger::insertAnswer(const string & answerStr)
	{
		size_t nAnswer = featureIndexPtr->getAnswerInt(answerStr);
		answer.push_back(nAnswer);		
	}

	bool Tagger::read(FeatureCapture &featureCap,const pair<int,int> &blockPlace)
	{	
		size_t nodesNum = featureCap.getSetSize(); //get the size of a data set, which means the number of nodes of a set
		setSizeOfNodes(nodesNum);                  // reset the size number of nodes
		setAnswersFromFC(featureCap, blockPlace); // get answers for a tagger		
		buildFeature(featureCap,blockPlace);       // build features 
		return true;
	}

	void Tagger::setAnswersFromFC(FeatureCapture &featureCap,const pair<int,int>&blockPlace)
	{
		size_t nodesNum = featureCap.getNodeNums();
		for (size_t nodeIdx = 0; nodeIdx < nodesNum; ++nodeIdx)
		{
			string ansStr = featureCap.answersForASet[nodeIdx][blockPlace.first][blockPlace.second];
			int ansInt = featureIndexPtr->getAnswerInt(ansStr);
			if (ansInt == -1)
			{
				ansInt = 0;
				//ansInt = featureIndexPtr->getSizeOfTags();
				//featureIndexPtr->insertAnTagger(ansStr);
			}
			answer[nodeIdx] = ansInt;
		}
	}

	bool EncoderTagger::buildFeature(FeatureCapture &featureCap,const pair<int,int>& blockPlace)// build features
	{
		size_t nodeSize = featureCap.getNodeNums();
		auto &featureMap = featureCap.featureMap;
		auto &featureDict = featureIndexPtr->featureDict;
		auto &featureCache = featureIndexPtr->featureCache;
		
		setFeatureCacheBeginIdx(featureCache.size());//set the begin idx of this tagger	
		size_t tagsNum = featureIndexPtr->getSizeOfTags();

		int realImIdx = 0;
		int realRow = 0;
		int realCol = 0;

		// uniGramTempls
		for (size_t eachImIdx = 0; eachImIdx < nodeSize; ++eachImIdx)
		{
			vector<int> featuresIdVec;
			for (auto &eachTemp : featureIndexPtr->unigramTempls)
			{
				std::istringstream stringIn(eachTemp);
				string featureStr = eachTemp;
				string flgStr;
				int imgIdx = 0, rowIdx = 0, colIdx = 0, fIdx = 0;
				size_t featureId = 0;
				while (stringIn >> flgStr)
				{
					if (flgStr == "#")break;
					stringIn >> imgIdx >> rowIdx >> colIdx >> fIdx;
					realImIdx = eachImIdx + imgIdx;
					realRow = blockPlace.first + rowIdx;
					realCol = blockPlace.second + colIdx;
					auto fMIter = featureMap.begin();
					if (fMIter = featureMap.find(make_tuple(realImIdx, realRow, realCol, fIdx)),fMIter != featureMap.end())
					{
						featureStr += " ";
						featureStr+=(*fMIter).second;
					}
					featureStr += " %";
				}
				featureStr += " #";
				auto fDIter = featureDict.find(featureStr);// search if there is an identical feature
				if (fDIter == featureDict.end())
				{
					size_t maxFeatureId = featureIndexPtr->getMaxId();
					featureDict.insert(make_pair(featureStr,make_pair(maxFeatureId,1)));
					featureId = maxFeatureId;//set featureId with the maxFeatureId
					maxFeatureId += tagsNum;
					featureIndexPtr->setMaxId(maxFeatureId);//update the maxId of class featureIndex
				}
				else
				{
					featureId = (*fDIter).second.first;
					++(*fDIter).second.second;
				}
				featuresIdVec.push_back(featureId);
			}
			featureCache.push_back(featuresIdVec);
		}

		//bigramTempls
		for (size_t eachImIdx = 1; eachImIdx < nodeSize; ++eachImIdx)
		{
			vector<int> featuresIdVec;
			for (auto &eachTemp : featureIndexPtr->bigramTempls)
			{
				std::istringstream stringIn(eachTemp);
				string featureStr = eachTemp;
				string flgStr;
				int imgIdx = 0, rowIdx = 0, colIdx = 0, fIdx = 0;
				size_t featureId = 0;
				while (stringIn >> flgStr)
				{
					if (flgStr == "#")break;
					stringIn >> imgIdx >> rowIdx >> colIdx >> fIdx;
					realImIdx = eachImIdx + imgIdx;
					realRow = blockPlace.first + rowIdx;
					realCol = blockPlace.second + colIdx;
					auto fMIter = featureMap.begin();
					if (fMIter = featureMap.find(make_tuple(realImIdx, realRow, realCol, fIdx)), fMIter != featureMap.end())
					{
						featureStr += " ";
						featureStr += (*fMIter).second;
					}
					featureStr += " %";
				}
				featureStr += " #";
				auto fDIter = featureDict.find(featureStr);// search if there is an identical feature
				if (fDIter == featureDict.end())
				{
					size_t maxFeatureId = featureIndexPtr->getMaxId();
					featureDict.insert(make_pair(featureStr, make_pair(maxFeatureId, 1)));
					featureId = maxFeatureId;//set featureId with the maxFeatureId
					maxFeatureId += tagsNum * tagsNum;
					featureIndexPtr->setMaxId(maxFeatureId);//update the maxId of class featureIndex
				}
				else
				{
					featureId = (*fDIter).second.first;
					++(*fDIter).second.second;
				}
				featuresIdVec.push_back(featureId);
			}
			featureCache.push_back(featuresIdVec);
		}
		return true;
	}

	void Tagger::setSizeOfNodes(size_t sizeOfX)
	{
		sizeOfNodes = sizeOfX;
		nodeFeatures.resize(sizeOfX);
		nodes.resize(sizeOfX);
		size_t sizeOfTagSet = featureIndexPtr->getSizeOfTags();
		answer.resize(sizeOfX);
		result.resize(sizeOfX);
	}

	void Tagger::setNodeConfigs()
	{
		sizeOfTags = featureIndexPtr->getSizeOfTags();
		for (size_t idx = 0; idx < sizeOfNodes; ++idx)
		{
			nodeFeatures[idx].resize(sizeOfTags);
			nodes[idx].resize(sizeOfTags);
		}
	}


	double Tagger::calcGradient(double *expected) // calculate the gradience
	{
		double s = 0.0;
		buildLattice();
		forwardbackward();
		//begin
		size_t nodesize = getSizeOfNodes();
		size_t tagsize = featureIndexPtr->getSizeOfTags();
		for (size_t eachNodeIdx = 0; eachNodeIdx < nodesize; ++eachNodeIdx)
			for (size_t eachTagIdx = 0; eachTagIdx < tagsize; ++eachTagIdx)
				getNode(eachNodeIdx, eachTagIdx)->calcExpectation(expected, Z_, tagsize);

		for (size_t eachNodeIdx = 0; eachNodeIdx < nodesize; ++eachNodeIdx)
		{
			for (auto &featureId : *(getNode(eachNodeIdx, answer[eachNodeIdx])->fVector))
				--expected[featureId + answer[eachNodeIdx]];
			s += getNode(eachNodeIdx, answer[eachNodeIdx])->cost;//unigram cost

			const vector<Path*> &lpath = getNode(eachNodeIdx, answer[eachNodeIdx])->leftPath;
			for (auto it = lpath.begin(); it != lpath.end(); ++it)
			{
				if ((*it)->leftNode->y == answer[(*it)->leftNode->x])
				{
					for (auto &featureId : *((*it)->fVector))
						--expected[featureId + (*it)->leftNode->y * tagsize + (*it)->rightNode->y];
					s += (*it)->cost;  // bigram cost
					break;
				}
			}
		}
	
		viterbi();  // call for eval()
		releaseNodeAndPath();
		return Z_ - s;
	}

	void Tagger::viterbi()
	{
		size_t nodesize = getSizeOfNodes();
		size_t tagsize = featureIndexPtr->getSizeOfTags();
		for (size_t eachNodeIdx = 0; eachNodeIdx < nodesize; ++eachNodeIdx) 
		{
			for (size_t eachTagIdx = 0; eachTagIdx < tagsize; ++eachTagIdx)
			{
				double bestc = -1e37;
				Node *best = 0;
				const std::vector<Path *> &lpath = getNode(eachNodeIdx,eachTagIdx)->leftPath;
				for (auto it = lpath.begin(); it != lpath.end(); ++it) 
				{
					double cost = (*it)->leftNode->bestCost + (*it)->cost +getNode(eachNodeIdx,eachTagIdx)->cost;
					if (cost > bestc) 
					{
						bestc = cost;
						best = (*it)->leftNode;
					}
				}
				getNode(eachNodeIdx, eachTagIdx)->prev = best;
				getNode(eachNodeIdx, eachTagIdx)->bestCost = best ? bestc : getNode(eachNodeIdx, eachTagIdx)->cost;
			}
		}
		double bestc = -1e37;
		Node *bestnode = 0;
		size_t s = nodesize - 1;
		for (size_t eachTagIdx = 0; eachTagIdx < tagsize; ++eachTagIdx) {
			if (bestc <  getNode(s, eachTagIdx)->bestCost) {
				bestnode = getNode(s, eachTagIdx);
				bestc = getNode(s, eachTagIdx)->bestCost;
			}
		}

		for (Node *nodePtr = bestnode; nodePtr; nodePtr = nodePtr->prev)
			result[nodePtr->x] = nodePtr->y;

		cost_ = -getNode(nodesize-1,result[nodesize - 1])->bestCost;
	}

	void Tagger::buildLattice()
	{
		featureIndexPtr->rebuildFeature(*this); //
		size_t tagsNum = featureIndexPtr->getSizeOfTags();
		for (size_t eachNodeIdx = 0; eachNodeIdx < getSizeOfNodes(); ++eachNodeIdx)
		{
			for (size_t eachTagIdx = 0; eachTagIdx < tagsNum; ++eachTagIdx)
			{
				featureIndexPtr->calcCost(getNode(eachNodeIdx,eachTagIdx));
				const vector<Path *> &lpath = getNode(eachNodeIdx, eachTagIdx)->leftPath;
				for (auto it = lpath.begin(); it != lpath.end(); ++it)
					featureIndexPtr->calcCost(*it);
			}
		}		
	}

	void Tagger::releaseNodeAndPath()
	{
		size_t ySetSize = featureIndexPtr->getSizeOfTags();
		for (size_t eachNodeIdx = 0; eachNodeIdx < getSizeOfNodes(); ++eachNodeIdx)
		{// tagger->size() return the nodes number of taggers
			for (unsigned short eachTagIdx = 0; eachTagIdx < ySetSize; ++eachTagIdx)
			{
				if (eachNodeIdx)
				{
					for (auto &eachPathPtr : getNode(eachNodeIdx, eachTagIdx)->leftPath)delete eachPathPtr;
				}
				delete getNode(eachNodeIdx, eachTagIdx);				
			}
		}
	}

	void Tagger::forwardbackward()
	{
		if (empty()) return;
		size_t tagsNum = featureIndexPtr->getSizeOfTags();
		size_t nodesize = getSizeOfNodes();
		for (size_t eachNodeIdx = 0; eachNodeIdx < nodesize; ++eachNodeIdx)
			for (size_t eachTagIdx = 0; eachTagIdx < tagsNum; ++eachTagIdx)
				getNode(eachNodeIdx,eachTagIdx)->calcAlpha();

		for (int eachNodeIdx = nodesize-1; eachNodeIdx >= 0; --eachNodeIdx)
			for (size_t eachTagIdx = 0; eachTagIdx < tagsNum; ++eachTagIdx)	
				getNode(eachNodeIdx, eachTagIdx)->calcBeta();

		Z_ = 0.0;
		for (size_t eachTagIdx = 0; eachTagIdx < tagsNum; ++eachTagIdx)
			Z_ = logsumexp(Z_, getNode(0, eachTagIdx)->beta, eachTagIdx == 0);
	
		return;
	}

	int Tagger::eval()
	{
		int errnum = 0;
		size_t nodesize = getSizeOfNodes();
		for (size_t eachNodeIdx = 0; eachNodeIdx < nodesize; ++eachNodeIdx)
			if (answer[eachNodeIdx] != result[eachNodeIdx]) ++errnum;
		return errnum;
	}

	bool DecoderTagger::buildFeature(FeatureCapture &featureCap,const pair<int,int> &blockPlace)
	{
		size_t trainSetId = featureCap.getTrainSetId();
		size_t nodeSize = featureCap.getNodeNums();
		auto &featureMap = featureCap.featureMap;
		auto &featureDict = featureIndexPtr->featureDict;
		auto &featureCache = featureIndexPtr->featureCache;

		setFeatureCacheBeginIdx(featureCache.size());//set the begin idx of this tagger	
		size_t tagsNum = featureIndexPtr->getSizeOfTags();

		int realImIdx = 0;
		int realRow = 0;
		int realCol = 0;

		// uniGramTempls
		for (size_t eachImIdx = 0; eachImIdx < nodeSize; ++eachImIdx)
		{
			vector<int> featuresIdVec;
			for (auto &eachTemp : featureIndexPtr->unigramTempls)
			{
				std::istringstream stringIn(eachTemp);
				string featureStr = eachTemp;
				string flgStr;
				int imgIdx = 0, rowIdx = 0, colIdx = 0, fIdx = 0;
				size_t featureId = 0;
				while (stringIn >> flgStr)
				{
					if (flgStr == "#")break;
					stringIn >> imgIdx >> rowIdx >> colIdx >> fIdx;
					realImIdx = eachImIdx + imgIdx;
					realRow = blockPlace.first + rowIdx;
					realCol = blockPlace.second + colIdx;
					auto fMIter = featureMap.find(make_tuple(realImIdx, realRow, realCol, fIdx));
					if (fMIter != featureMap.end())
					{
						featureStr += " ";
						featureStr += (*fMIter).second;
					}
					featureStr += " %";
				}
				featureStr += " #";
				auto fDIter = featureDict.find(featureStr);// search if there is an identical feature
				if (fDIter != featureDict.end())
				{
					featureId = (*fDIter).second.first;		
					featuresIdVec.push_back(featureId);
				}
			}
			featureCache.push_back(featuresIdVec);
		}

		//bigramTempls
		for (size_t eachImIdx = 1; eachImIdx < nodeSize; ++eachImIdx)
		{
			vector<int> featuresIdVec;
			for (auto &eachTemp : featureIndexPtr->bigramTempls)
			{
				std::istringstream stringIn(eachTemp);
				string featureStr = eachTemp;
				string flgStr;
				int imgIdx = 0, rowIdx = 0, colIdx = 0, fIdx = 0;
				size_t featureId = 0;
				while (stringIn >> flgStr)
				{
					if (flgStr == "#")break;
					stringIn >> imgIdx >> rowIdx >> colIdx >> fIdx;
					realImIdx = eachImIdx + imgIdx;
					realRow = blockPlace.first + rowIdx;
					realCol = blockPlace.second + colIdx;
					auto fMIter = featureMap.begin();
					if (fMIter = featureMap.find(make_tuple(realImIdx, realRow, realCol, fIdx)), fMIter != featureMap.end())
					{
						featureStr += " ";
						featureStr += (*fMIter).second;
					}
					featureStr += " %";
				}
				featureStr += " #";
				auto fDIter = featureDict.find(featureStr);// search if there is an identical feature
				if (fDIter != featureDict.end())
				{
					featureId = (*fDIter).second.first;
					featuresIdVec.push_back(featureId);
				}				
			}
			featureCache.push_back(featuresIdVec);
		}
		return true;
	}
}
