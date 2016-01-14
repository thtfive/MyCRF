#include "Params.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <vector>
namespace CRF3D
{
	using std::cout;
	using std::endl;
	using std::string;
	vector<Options> Params::defaultConfigs
	{
		{ "shrinkingSize", 's',   "20", "INT"," be optimal before considered for shrinking. (default 20)" },
		{ "freq",     'f', "1",    "INT","use features that occuer no less than INT(default 1)" },
		{"maxiter" ,  'M', "10000","INT","set INT for max iterations in LBFGS routine(default 10k)" },
		{"cost",      'c', "1.0",  "DOUBLE", "set DOUBLE for cost parameter(default 1.0)" },
		{"eta",       'e', "0.0001","DOUBLE","set DOUBLE for termination criterion(default 0.0001)" },
		{"algorithm", 'a', "CRF",   "CRF","select training algorithm" },	
		{ "model file", 'm', "trainmodel.txt",   "string","model file name" },
		{ "learn file", 'l', "trainFile.txt",   "string","learn file name" },
		{ "test file", 't', "testFile.txt",   "string","test file name" },
		{ "template file", 'T', "templateFile.txt",   "string","template file name" },
		{ "result file", 'r', "resultFile.txt",   "string","result file name" },
		{"version",   'v', "False",     "BOOL",  "show the version and exit" },		
		{"help",      'h', "False",     "BOOL",     "show this help and exit" }
	};
    void CRF3D::Params::init_configs(int argc,char **argv)
    {
        for(auto &opt:defaultConfigs)
            allConfigs[opt.shortName]=opt.defaultValue;

        for(int eachArgIdx=0;eachArgIdx<argc;eachArgIdx++)
        {
            if(argv[eachArgIdx][0]!='-')restConfigs.push_back(argv[eachArgIdx]);
            else
            {
				if (argv[eachArgIdx][1] == 'l')
				{
					modeStr = "Train";
				}
				if (argv[eachArgIdx][1] == 't')
				{
					modeStr = "Test";
				}
				if (argv[eachArgIdx][1] == 'h' || argv[eachArgIdx][1]=='v')
				{
					allConfigs[argv[eachArgIdx][1]] = "1";
				}
				else
				{
					++eachArgIdx;
					allConfigs[argv[eachArgIdx - 1][1]] = argv[eachArgIdx];
				}
            }
        }
        freq          = std::atoi(allConfigs['f'].c_str());
        maxiter       = std::atoi(allConfigs['M'].c_str());
        cost          = std::atof(allConfigs['c'].c_str());
        eta           = std::atof(allConfigs['e'].c_str());
        algorithm     = allConfigs['a'];
		trainFileName = allConfigs['l'];		
		modelFileName = allConfigs['m'];
		templateFileName = allConfigs['T'];
		resultFileName = allConfigs['r'];
		testFileName = allConfigs['t'];
        shrinkingSize = std::atoi(allConfigs['s'].c_str());
		bVersion      = false;
		bHelp         = false;
		if (allConfigs['v'] == "1") bVersion = true;
		if (allConfigs['h'] == "1") bHelp = true;
		helpStr = "help information:\n";  //set helpStr here
		for (auto &eachConfig : defaultConfigs)
		{
			helpStr += eachConfig.fullName+" ";
			helpStr += eachConfig.shortName + " ";
			helpStr += eachConfig.defaultValue + " ";
			helpStr += eachConfig.argDescription + "\n";
			helpStr += eachConfig.description + " \n";
		}
		helpStr += "\n"; 
		versionStr = restConfigs[0]+ " CRF3D\n";//set versionStr here
		

		cout << "freq   :" << freq << endl;
		cout << "maxiter:" << maxiter << endl;
		cout << "cost   :" << cost << endl;
		cout << "eta    :" << eta << endl;

		std::ifstream markedBlocksFile("markedBlocks");
		char charBuffer[8096];
		int blockRow = 0;
		while (markedBlocksFile.getline(charBuffer, 8096))
		{
			std::istringstream eachLine(charBuffer);
			int blockCol = 0;
			int flg = 0;
			while (eachLine >> flg)
			{
				if (flg) 
				{
					markedPlaces.insert(std::make_pair(blockRow, blockCol));			
				}
				++blockCol;
			}
			++blockRow;
		}
		blocksNumPerImage = markedPlaces.size();
		cout << "blocksNumPerImage: " << blocksNumPerImage << endl;
    }
}
