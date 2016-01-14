#include <iostream>
#include <string>
#include <cstring>
#include "Encoder.h"
#include "Decoder.h"
#include "Params.h"
#include "cv.h"
#include "highgui.h"

using namespace std;
int main(int argc,char **argv)
{
	argc = 9;
    argv=new char*[20];
	argv[0] = new char[20];
	argv[1] = new char[20];
	argv[2] = new char[20];
	argv[3] = new char[20];
	argv[4] = new char[20];
	argv[5] = new char[20];
	argv[6] = new char[20];
	argv[7] = new char[20];
	argv[8] = new char[20];
	argv[9] = new char[20];
	argv[10] = new char[20];
	strcpy(argv[0], "crf_learn");
	strcpy(argv[1], "-T");
	strcpy(argv[2], "template.txt");
	strcpy(argv[3], "-t");
	strcpy(argv[4], "testfile.txt");
	strcpy(argv[5], "-m");
	strcpy(argv[6], "trainmodel.txt");
	strcpy(argv[7], "-M");
	strcpy(argv[8], "10");
	strcpy(argv[9], "-h");
	strcpy(argv[10], "-v");

	cout << "Initilization params...\n";
	CRF3D::Params param;
	param.init_configs(argc, argv);
	
	if (param.getModeStr() == "Train")
	{
		cout << "Train Program " << endl;
		CRF3D::CRF3D_Train(param);//Training function
	}
	else if(param.getModeStr() == "Test")
	{
		cout << "Test Program " << endl;
		CRF3D::CRF3D_Test(param);
	}
	else
	{
		cout << "Error, Invalid Command" << endl;
	}

	do
	{
		cout << "Press any key to exit!";
	}while (!cin.get());
    
	return 0;
}
