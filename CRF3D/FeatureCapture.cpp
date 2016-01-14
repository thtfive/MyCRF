#include <fstream>
#include <sstream>
#include <tuple>
#include <set>
#include <map>
#include "FeatureCapture.h"
#include "FeatureIndex.h"
#include "Tagger.h"
#include "Params.h"
#include "Common.h"
namespace CRF3D
{
	using std::map;
	using std::set;
	using std::pair;
	using std::tuple;

	void FeatureCapture::buildFeatureMap()
	{
		for (size_t eachImageIdx = 0; eachImageIdx < getNodeNums(); ++eachImageIdx)
		{
			string imageNameAbs = imageAddresses[dataSetId] + imageNames[dataSetId][eachImageIdx]
				+ imageNameEnding;
			readImage(imageNameAbs);
			getFeature1(eachImageIdx);
			getFeature2(eachImageIdx);
		}
	}

	void FeatureCapture::readTags()
	{
		auto &markedPlaces=paramPtr->markedPlaces;
		for (size_t setIdx = 0; setIdx < dataSetsNum; ++setIdx)
		{
			for (size_t eachImageIdx = 0; eachImageIdx < imageNames[setIdx].size(); ++eachImageIdx)
			{
				string ansFileName = imageAddresses[setIdx] + imageNames[setIdx][eachImageIdx]
					+ ".txt";
				std::ifstream ansFile(ansFileName.c_str());
				for (int bRowId = 0; bRowId < blockRowSize; ++bRowId)
				{
					for (int bColId = 0; bColId < blockColSize; ++bColId)
					{
						string tagStr;
						ansFile >> tagStr;
						if (markedPlaces.count({bRowId,bColId}))
						{
							featureIndexPtr->insertAnTagger(tagStr);
						}						
					}
				}
			}
		}
	}

	void FeatureCapture::readAnswers()// read answers from corresponding folder, store in answersForASet
	{
		answersForASet.resize(nodesNum);
		initAnswersForASet();
		for (size_t eachImageIdx = 0; eachImageIdx < getNodeNums(); ++eachImageIdx)
		{
			string ansFileName = imageAddresses[dataSetId] + imageNames[dataSetId][eachImageIdx]
				+ ".txt";
			std::ifstream ansFile(ansFileName.c_str());
			for (int bRowId = 0; bRowId < blockRowSize; ++bRowId)
			{
				for (int bColId = 0; bColId < blockColSize; ++bColId)
				{
					string ansStr;
					ansFile >> ansStr;
					answersForASet[eachImageIdx][bRowId][bColId] = ansStr;
				}
			}
		}
	}
	
	void FeatureCapture::getFeature1(int imIdx)
	{
		auto &markedPlaces = paramPtr->markedPlaces;
		for (auto &blockPlace: markedPlaces)
		{
			int blockRow = blockPlace.first;
			int blockCol = blockPlace.second;
			int red = 0, blue = 0;
			for (int eachPixel = 0; eachPixel < blockSize; ++eachPixel)
			{
				red += getRed(blockRow, blockCol, eachPixel);
				blue += getBlue(blockRow, blockCol, eachPixel);
			}
			if (!blue)blue = 1;
			double rate = (double)red / blue * 100.0;
			int cloudMode = 0;
			if (rate > 80)
			{
				if (rate <= 85)
				{
					cloudMode = 1;
				}
				else
				{
					if (rate <= 90)
					{
						cloudMode = 2;
					}
					else
					{
						if (rate <= 95)
						{
							cloudMode = 3;
						}
						else
						{
							if (rate <= 100)
							{
								cloudMode = 4;
							}
							else
							{
								if (rate <= 105)
								{
									cloudMode = 5;
								}
								else
								{
									if (rate <= 110)
									{
										cloudMode = 6;
									}
									else
									{
										cloudMode = 7;
									}
								}
							}
						}
					}
				}
			}
			switch (cloudMode)
			{	
			case 0:
				featureMap.insert(std::make_pair(make_tuple(imIdx, blockRow, blockCol, 1), "0"));
				break;
			case 1:
				featureMap.insert(std::make_pair(make_tuple(imIdx, blockRow, blockCol, 1), "1"));
				break;
			case 2:
				featureMap.insert(std::make_pair(make_tuple(imIdx, blockRow, blockCol, 1), "2"));
				break;
			case 3:
				featureMap.insert(std::make_pair(make_tuple(imIdx, blockRow, blockCol, 1), "3"));
				break;
			case 4:
				featureMap.insert(std::make_pair(make_tuple(imIdx, blockRow, blockCol, 1), "4"));
				break;
			case 5:
				featureMap.insert(std::make_pair(make_tuple(imIdx, blockRow, blockCol, 1), "5"));
				break;
			case 6:
				featureMap.insert(std::make_pair(make_tuple(imIdx, blockRow, blockCol, 1), "6"));
				break;
			case 7:
				featureMap.insert(std::make_pair(make_tuple(imIdx, blockRow, blockCol, 1), "7"));
				break;
			}
		}
	}

	void FeatureCapture::getFeature2(int imIdx)
	{
		auto &markedPlaces = paramPtr->markedPlaces;
		for (auto &blockPlace : markedPlaces)
		{
			int blockRow = blockPlace.first;
			int blockCol = blockPlace.second;
			int red = 0, green = 0, blue = 0;
			for (int eachPixel = 0; eachPixel < blockSize; ++eachPixel)
			{
				red += getRed(blockRow, blockCol, eachPixel);
				green += getGreen(blockRow, blockCol, eachPixel);
				blue += getBlue(blockRow, blockCol, eachPixel);
			}		
			red /= 16;
			green /= 16;
			blue /= 16;
			switch((red*red+green*green+blue*blue)/40000)
			{
			case 0:
				featureMap.insert(std::make_pair(make_tuple(imIdx, blockRow, blockCol, 2), "0"));
				break;
			case 1:
				featureMap.insert(std::make_pair(make_tuple(imIdx, blockRow, blockCol, 2), "1"));				
				break;
			case 2:
				featureMap.insert(std::make_pair(make_tuple(imIdx, blockRow, blockCol, 2), "2"));
				break;
			case 3:
				featureMap.insert(std::make_pair(make_tuple(imIdx, blockRow, blockCol, 2), "3"));
				break;
			case 4:
				featureMap.insert(std::make_pair(make_tuple(imIdx, blockRow, blockCol, 2), "4"));
				break;
			}
		}
	}


	bool FeatureCapture::readTrainFile(const string &trainFileName)
	{
		std::ifstream fileIn(trainFileName.c_str());
		fileIn >> dataSetsNum;
		fileIn >> imageNameEnding;
		string sizeTypeStr;
		fileIn >> sizeTypeStr;
		fileIn >> blockHeight >> blockWidth;
		fileIn >> sizeTypeStr;
		fileIn >> rows >> cols;
		initRowColInfo();

		// read tag information
		// the code below is used for read tag str
		string tagStr;
		int tagNum;
		fileIn >> tagStr; 
		fileIn >> tagNum;// the number of tag
		for (int tagIdx = 0; tagIdx < tagNum; ++tagIdx)
		{
			fileIn >> tagStr;
			featureIndexPtr->insertAnTagger(tagStr);
		}				

		imageAddresses.resize(dataSetsNum);// init blockRowSize,blockColSize
		imageNames.resize(dataSetsNum);// resize the iamgeNames to trainSetNum
				
		//read the addresses of training sets, one address for each set
		for (size_t trainSetIdx = 0; trainSetIdx < dataSetsNum; ++trainSetIdx)
			fileIn >> imageAddresses[trainSetIdx];

		// read the information of the places of the sun at different images, and tags' values 
		string nameStr;
		string answerStr;
		for (size_t trainSetIdx = 0; trainSetIdx < dataSetsNum; ++trainSetIdx)
		{
			while (fileIn >> nameStr, nameStr != "endOfSet")
			{
				imageNames[trainSetIdx].push_back(nameStr);
			}
		}
		return true;
	}

	bool FeatureCapture::readTemplateFile(const string & templateFileName)
	{
		std::ifstream fileIn(templateFileName.c_str());
		char charBuffer[8096];
		while (fileIn.getline(charBuffer, 8096))
		{
			string templateStr = charBuffer;
			if (templateStr == "End")break;
			if (templateStr.size() && templateStr[0] == 'U')
				featureIndexPtr->unigramTempls.push_back(templateStr);
			if (templateStr.size() && templateStr[0] == 'B')
				featureIndexPtr->bigramTempls.push_back(templateStr);
		}
		return true;
	}

	bool FeatureCapture::readTestFile(const string &testFileName)
	{
		std::ifstream fileIn(testFileName.c_str());
		fileIn >> dataSetsNum;
		fileIn >> imageNameEnding;
		string sizeTypeStr;
		fileIn >> sizeTypeStr;
		fileIn >> blockHeight >> blockWidth;
		fileIn >> sizeTypeStr;
		fileIn >> rows >> cols;
		initRowColInfo();

		// read tag information
		string tagStr;
		int tagsNum;
		fileIn >> tagStr;
		fileIn >> tagsNum;// the number of tag
		for (int tagIdx = 0; tagIdx < tagsNum; ++tagIdx)
		{
			fileIn >> tagStr;
			if (!featureIndexPtr->checkAnTagger(tagStr))
			{
				cout << "Warning!  Tag:" << tagStr << " has not been in training model!\n";
			}
		}

		imageAddresses.resize(dataSetsNum);// init blockRowSize,blockColSize
		imageNames.resize(dataSetsNum);// resize the iamgeNames to trainSetNum

		//read the addresses of training sets, one address for each set
		for (size_t testSetIdx = 0; testSetIdx < dataSetsNum; ++testSetIdx)
			fileIn >> imageAddresses[testSetIdx];

		// read the information of the places of the sun at different images, and tags' values 
		string nameStr;
		for (size_t testSetIdx = 0; testSetIdx < dataSetsNum; ++testSetIdx)
		{
			while (fileIn >> nameStr, nameStr != "endOfSet")
			{
				imageNames[testSetIdx].push_back(nameStr);
			}
		}
		return true;
	}

	void FeatureCapture::getFeature(Tagger &tagger, int imageIdx)
	{
		//	cout << imgMat.rows << endl;
		//	cout << imgMat.cols << endl;
		//refNodeFeatures.resize(nodeNums);
		///for (auto &tmp : refNodeFeatures)tmp.resize(featureSize);
		int maxBlockId = 0;
		std::map<int, pair<int, int>>blockRB;
		string *nodeStrPtr;

		int featureIdx = blockSize;
		for (int id = 0; id < maxBlockId; ++id)
		{
			auto &blockRed = blockRB[id].first;
			auto &blockBlue = blockRB[id].second;
			blockRed /= blockSize;
			blockBlue /= blockSize;
			if (!blockBlue)blockBlue = 1;
			int blockRBRate = (int)(100.0 * blockRed / blockBlue);
			nodeStrPtr = new string;
			
		//	refNodeFeatures[id][featureIdx] = nodeStrPtr;
		}
		//		cout << maxid << endl;
		//		cv::imshow("adjustedimage",imgMat);
		blockRB.clear();
	}
}