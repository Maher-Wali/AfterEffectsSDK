#include "../include/aescriptsLicensing.h"
#include <string>
#include <stdlib.h>
#include <iostream>
#include <time.h>

using namespace std;

int main(int argc, char* argv[]) {
	if (argc != 3) {
		cout << "usage: aescriptsValidator [name] [privnum]" << endl;
		cout << "[name] is the name of the product to be validated" << endl;
		cout << "[privnum] is the private number of the product" << endl;
		return -1;
	}
	aescriptsLicenseData lData;
	aescriptsInitLicenseData(lData);

	aescripts::configure(AESCRIPTS_CONFIGURE_DISABLE_ONLINE_ACTIVATION, 1);

	lData.nofUsers = 0;
	string s = argv[2];
	int privNum = atoi(s.c_str());
	int result = aescripts::loadLicenseFromFile(argv[1], "", privNum, lData, false);
	string status = string(resultCodeToMessage(result));
	cout << "status: " << status << "\n";
	if (result == 0) {
		cout << "first name: '" << lData.firstName << "'\n";
		cout << "last name: '" << lData.lastName << "'\n";
		cout << "number of user licenses: " << lData.nofUsers << "\n";
		cout << "license type: '" << lData.licType << "'\n";
		cout << "product ID: '" << lData.productID << "'\n";
		cout << "product version: '" << lData.productVersion << "'\n";
		cout << "serial: '" << lData.serial << "'\n";
	}
	if (result == -7) {
		cout << "days since trial start: " << lData.numberOfDaysSinceFirstStart << "\n";
	}
	return result;
}

