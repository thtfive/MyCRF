#include "Decoder.h"
#include "Params.h"
namespace CRF3D
{
	using std::cout;
	using std::endl;
	bool CRF3D_Test(Params &param)
	{
		Decoder decoder(param);
		
		cout << "Reading data..." << endl;
		decoder.read();

		cout << "Testing..." << endl;
		decoder.test();

		cout << "Saving..." << endl;
		decoder.save();

		cout << "Done!" << endl;

		return true;
	}
}