#ifndef CRF3D_FEATUREINDEX_H
#define CRF3D_FEATUREINDEX_H
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include <tuple>
#include "Tagger.h"

using std::vector;
using std::string;
using std::cout;
using std::cin;
using std::pair;
using std::map;
using std::tuple;
using std::make_tuple;
namespace CRF3D
{
    class Node;
    class Path;
    class Tagger;
    class FeatureIndex  //该类可以被继承为EncoderFeatureIndex 和 DecoderFeatureIndex
    {
    private:
		size_t              maxId = 0;
        vector<double>      alpha;
		double              costFactor = 1.0;
		size_t              sizeOfX = 0;
        vector<string>      ySet;        
        
    public:
		map<string, pair<size_t, size_t>> featureDict;
		vector<string>					  unigramTempls;
		vector<string>                    bigramTempls;
		vector<vector<int>>	        	  featureCache;
		void insertAnTagger(const string &taggerStr) 
		{ 
			for (auto &eachY : ySet)
			{
				if (eachY == taggerStr)return;
			}
			ySet.push_back(taggerStr); 
		}
		bool checkAnTagger(const string &taggerStr)
		{
			size_t tagsNum = ySet.size();
			for (size_t tagIdx = 0; tagIdx < tagsNum; ++tagIdx)
			{
				if (ySet[tagIdx] == taggerStr)return true;
			}
			return false;
		}
		vector<double>& getAlpha() { return alpha; }
		double getCostFactor() { return costFactor; }
        void initAlpha()
        {
            alpha.resize(getSizeOfFeatures());
            std::fill(alpha.begin(),alpha.end(),0.0);
        }
		void   setAlphaSize(size_t alphaSize) { alpha.resize(alphaSize); }
		void   setAlpha(size_t alphaIdx, double eachAlpha) { alpha[alphaIdx] = eachAlpha; }
		void   setCostFactor(double newCostFactor) { costFactor = newCostFactor; }
		int    getAnswerInt(const string &answerStr);
        size_t getSizeOfX(){return sizeOfX;}
		void   setSizeOfX(size_t xSize) { sizeOfX = xSize; }
		size_t getSizeOfFeatures(){ return maxId; }
		size_t getAlphaSize() { return alpha.size(); }
		size_t getSizeOfTags() { return ySet.size(); }
        size_t getID(const string &key);
		size_t getMaxId() { return maxId; }
		string getTag(size_t tagId) 
		{
			return tagId < ySet.size() ? ySet[tagId] : "Error TagIdx\n";
		}
		void   setTag(size_t tagId, const string &tagStr) { ySet[tagId] = tagStr; }
		void   setMaxId(size_t maxid) { maxId = maxid; }
		void   setSizeOfTags(size_t tagsNum) { ySet.resize(tagsNum); }
		int    getYValue(const string & yName);
		virtual bool   save(Params & param);
		
        void   shrink(int freq);
		void   rebuildFeature(Tagger &tagger);
		void   calcCost(Node *);
		void   calcCost(Path *);
	};

    class EncoderFeatureIndex: public FeatureIndex
    {
    public:
		bool save(Params &param);

    };

    class DecoderFeatureIndex: public FeatureIndex
    {
	public:
		bool save(Params &param);
		bool readModel(Params & param);
    };
}


#endif // CRF3D_FEATUREINDEX_H
