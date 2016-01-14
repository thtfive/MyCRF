#ifndef CRF3D_PARAMS_H
#define CRF3D_PARAMS_H
#include <vector>
#include <map>
#include <string>
#include <set>
namespace CRF3D
{
    using std::string;
    using std::map;
    using std::vector;
	using std::set;
	using std::pair;
    struct Options
    {
        string fullName;
        char   shortName;
        string defaultValue;
        string argDescription;
        string description;
		Options(const string &fn,const char &sn,const string &dv,const string &ad,const string &de):
			fullName(fn),shortName(sn),defaultValue(dv),argDescription(ad),description(de){}
    };

	class Params
	{
	private:
		static vector<Options>   defaultConfigs;    //默认的配置信息
		map<char, string>        allConfigs;        //用于存储配置信息
		vector<string>           restConfigs;       //用于存储剩余的输入参数
		size_t      freq = 0;
		size_t      maxiter = 0;
		size_t      blocksNumPerImage = 0;
		double      cost = 1.0;
		double      eta;
		string      algorithm;
		size_t      shrinkingSize;
		string      versionStr;
		bool        bVersion;
		bool        bHelp;
		string      helpStr;
		string      trainFileName;
		string      testFileName;
		string      modelFileName;		
		string      resultFileName;
		string      imageNameEnding;
		string      templateFileName;
		string      modeStr;
	public:
		set<pair<int, int>>markedPlaces;

		double getCost() { return cost; }
		double getEta() { return eta; }
		size_t getBlocksNumPerImage() { return blocksNumPerImage; }
		string getTemplateFileName() { return templateFileName; }
		string getTrainFileName() { return trainFileName; }
		string getTestFileName() { return testFileName; }
		string getModelFileName() { return modelFileName; }
		string getResultFileName() { return resultFileName; }
		string getModeStr() { return modeStr; }		
		string getImageNameEnding() { return imageNameEnding; }
		string getAlgorithm() { return algorithm; }
		size_t getMaxIter() { return maxiter; }
		size_t getFreq() { return freq; }		
		size_t getShrinkingSize() { return shrinkingSize; }		
		string getHelpStr() { return helpStr; }
		string getVersion() { return versionStr; }

		bool   ifHelp() { return bHelp; }
		bool   ifVersion() { return bVersion; }
		void   setVersion(string versionStrIn) { versionStr = versionStrIn; }

		void   init_configs(int argc, char **argv);		//parameters initialization        
    };
}
#endif // PARAMS_H_INCLUDED
