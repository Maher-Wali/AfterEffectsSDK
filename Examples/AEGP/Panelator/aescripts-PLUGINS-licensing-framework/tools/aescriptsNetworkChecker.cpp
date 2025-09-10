#include "../include/aescriptsLicensing.h"

#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

int main(int argc, char* argv[]) {
	aescripts::machineString m;
	aescripts::getMachineId(m);
	cout << "aescriptsNetworkChecker" << endl << endl;
	cout << "Found the following ethernet network adapters on this system:" << endl;
	for (int c = 0; c < 10; c++) {
		stringstream ss;
		for (size_t i = 0; i < 6; i++) {
			ss << hex << setw(2) << setfill('0') << static_cast<unsigned int>(static_cast<unsigned char>(m[6*c+i]));
			if (i != 5) ss << ":";
		}
		if (ss.str() != "00:00:00:00:00:00") {
			cout << dec << "(" << c << ") " << ss.str() << endl;
		}
	}
	cout << "Press enter to exit..." << endl;
	cin.get();
	return 0;
}