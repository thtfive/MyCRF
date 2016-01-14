#ifndef CRF3D_FEATURECAPTURE_H
#define CRF3D_FEATURECAPTURE_H
#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <cstdlib>
#include "cv.h"
#include "highgui.h"
#include "Tagger.h"
#include "Params.h"

namespace CRF3D
{
	using std::cout;
	using std::cin;
	using std::endl;
	using std::pair;
	using std::map;
	using std::set;
	using std::tuple;
	using std::string;
	using std::vector;
	class Tagger;
	class Params;
	class FeatureCapture
	{
	private:
		cv::Mat imgMat;		//used to store an image, from OpenCV
		int blockHeight = 4;//the height in pixels of a block
		int blockWidth = 4 ;	//the width in pixels of a block		
		int rows = 0;			//the total number of rows of an image
		int cols = 0;			//the total number of cols of an image
		int blockRowSize = 0;
		int blockColSize = 0;
		int blockSize = 0;
		int featureSize = 2;
		size_t dataSetsNum = 0;

		vector<string>                        imageAddresses;
		vector<vector<string>>                imageNames;	
		
		size_t nodesNum;
		size_t dataSetId;
		Params *paramPtr;
		FeatureIndex *featureIndexPtr;
		string imageNameEnding;
		//vector <string>& imageNames;
	public:

		map<tuple<int, int, int, int>, string>featureMap;

		vector<vector<vector<string>>> answersForASet;

		size_t  getNodeNums() { return nodesNum; }

		size_t  getDataSetsNum(){ return dataSetsNum; }

		void buildFeatureMap();

		void readTags();

		void readAnswers();

		void setParamPtr(Params *param_ptr) { paramPtr = param_ptr; }

		void setFeatureIndexPtr(FeatureIndex *featureidx) { featureIndexPtr = featureidx; }

		void setTrainSetId(size_t trainid) 
		{
			dataSetId = trainid; 
			nodesNum = imageNames[trainid].size();
		}

		void initAnswersForASet()
		{
			for (auto &eachAnswer : answersForASet)
			{
				eachAnswer.resize(blockRowSize);
				for (auto &eachAnsRow : eachAnswer)
					eachAnsRow.resize(blockColSize);
			}
		}

		void setTestSetId(size_t testid)
		{
			dataSetId = testid;
			nodesNum = imageNames[testid].size();
		}

		size_t  getSetSize() { return nodesNum; }

		size_t  getTrainSetId() { return dataSetId; }

		vector<string>& getImageNames(){ return imageNames[dataSetId]; }
		
		void getFeature1(int imIdx);

		void getFeature2(int imIdx);

		size_t getSetsNum() { return imageNames.size(); }

		bool readTrainFile(const string &trainFileName);

		bool readTemplateFile(const string &modelFileName);

		bool readTestFile(const string & testFileName);
	
		uchar * getPerPtrEachBlock(int blockRowIdx, int blockColIdx, int idxInBlock)
		{
			int row = blockRowIdx * blockHeight + idxInBlock / blockWidth;
			int col = blockColIdx * blockWidth + idxInBlock % blockWidth;
			return imgMat.data + imgMat.step[0] * row + imgMat.step[1] * col;
		}

		uchar & getBlue(int blockRowIdx, int blockColIdx, int idxInBlock)
		{
			return *(getPerPtrEachBlock(blockRowIdx, blockColIdx, idxInBlock));
		}

		uchar & getGreen(int blockRowIdx, int blockColIdx, int idxInBlock)
		{
			return *(getPerPtrEachBlock(blockRowIdx, blockColIdx, idxInBlock)+imgMat.elemSize1());
		}

		uchar & getRed(int blockRowIdx, int blockColIdx, int idxInBlock)
		{
			return *(getPerPtrEachBlock(blockRowIdx, blockColIdx, idxInBlock)+imgMat.elemSize1() * 2);
		}

		void initRowColInfo()
		{
			blockRowSize = rows / blockHeight;
			blockColSize = cols / blockWidth;
			blockSize = blockHeight * blockWidth;
		}

		void readImage(const string &imageName)
		{
			imgMat=cv::imread(imageName.c_str());
		}

		pair<int,int> getBlockIdxPair(int row,int col)
		{
			return{ row / blockHeight,col / blockWidth };
		}

		int get_feature_id(int row, int col)
		{
			return (row % blockHeight) * blockWidth + (col % blockWidth);
		}

		void getFeature(Tagger &tagger, int imageIdx);
		
		void clear() 
		{
			featureMap.clear();
		}

		void clearall()
		{
			featureMap.clear();
		}
	};
}
#endif  //CRF3D_FEATURECAPTURE_H
