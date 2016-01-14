#ifndef CRF3D_TAGGER_H
#define CRF3D_TAGGER_H

#include <fstream>
#include <iostream>
#include <vector>
#include <set>

namespace CRF3D
{
	using std::vector;
	using std::string;
	using std::pair;
	using std::set;
    class FeatureIndex;
	class FeatureCapture;
	class Node;
	class Path;
	class Params;
    class Tagger
    {
    private:        
		vector<vector<string *>> nodeFeatures;             //store features of a node
		vector<vector<Node *>>   nodes;       
		size_t                   nodeSizePerImage = 0;
		size_t                   featureSizePerBlock = 0;
		size_t                   sizeOfTags = 0;
		size_t                   sizeOfNodes = 0;
		double                   cost_ = 0.0;
		double                   Z_ = 0.0;
		size_t                   featureCacheBeginIdx = 0;
		
    public:
		vector<int>              answer;				   //store the ground truth
		vector<int>              result;                   //store the computed result
		FeatureIndex            *featureIndexPtr = 0;
		void resize_nodeFeatures(int nodeNums,int featureNums)
		{
			nodeSizePerImage = nodeNums;
			featureSizePerBlock = featureNums; 
		}
		void   insertAnswer(const string &answerStr);
		bool   read(FeatureCapture &featureCap,const pair<int, int>& blockPlace);
		void setAnswersFromFC(FeatureCapture & featureCap, const pair<int, int>& blockPlace);
		virtual bool  buildFeature(FeatureCapture &featureCap, const pair<int, int>& blockPlace) { return true; }
		size_t getSizeOfNodes(){ return sizeOfNodes; }
		void   setFeatureCacheBeginIdx(size_t fCacheBeginIdx) { featureCacheBeginIdx = fCacheBeginIdx; }
		size_t getFeatureCacheBeginIdx(){ return featureCacheBeginIdx; }
		void   setSizeOfNodes(size_t sizeOfX);
		void   setNodeConfigs();
		void   setFeatureIndex(FeatureIndex * feature) { featureIndexPtr = feature; }
		bool   empty(){ return nodeFeatures.empty(); }
		void   setNode(Node *node, size_t nodeId, size_t tagId) { nodes[nodeId][tagId] = node; }
		Node*  getNode(size_t nodeId, size_t tagId) { return nodes[nodeId][tagId]; }
		double calcGradient(double *expected);		
		void   viterbi();
		void   buildLattice();
		void   releaseNodeAndPath();
		void   forwardbackward();
		int    eval();
    };

	class EncoderTagger :public Tagger
	{
	public:
		virtual bool buildFeature(FeatureCapture &featureCap, const pair<int, int>& blockPlace);
	};

	class DecoderTagger :public Tagger
	{
	public:
		virtual bool buildFeature(FeatureCapture &featureCap, const pair<int, int>& blockPlace);
		bool   initNBest()
		{
			return true;
		}
	};
}

#endif // CRF3D_TAGGER_H
