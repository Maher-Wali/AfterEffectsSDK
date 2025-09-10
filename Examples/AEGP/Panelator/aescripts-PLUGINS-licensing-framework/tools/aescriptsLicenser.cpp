#include "../include/aescriptsLicensing.h"

#include <iostream>
using namespace std;

int main(int argc, char* argv[]) {
	if (argc < 3) {
		cout << "usage: aescriptsLicenser [name] [license] [version]" << endl;
		cout << "[name] is the name of the product to be licensed" << endl;
		cout << "[license] is the license string for the product" << endl;
		cout << "[version] is the optional version string for the product (format x.y.z)" << endl;
		cout << "if you want to unlicense a product, enter '-' for [license]" << endl;
		return -1;
	}

	aescripts::configure(AESCRIPTS_CONFIGURE_DISABLE_ONLINE_ACTIVATION, 1);
	aescripts::licString license;
	aescripts::verString version;

	license[0] = '\0';
	version[0] = '\0';
	if (argc > 2) strcpy(license, argv[2]);
	if (argc > 3) strcpy(version, argv[3]); else strcpy(version, "1.0.0");
	int result = aescripts::saveLicenseToFile(argv[1], "", license, version);
	cout << "result: " << result << endl;
	return result;
}