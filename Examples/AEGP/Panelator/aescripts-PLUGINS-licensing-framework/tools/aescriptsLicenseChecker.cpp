#include "../include/aescriptsLicensing.h"
#include <string>

#include <iostream>
using namespace std;

/*
This is a tool for internal use/testing only!

The syntax should be:
./aescriptsLicenseChecker effectName

so for example:
./aescriptsLicenseChecker myplugin

if there is a license file and there is a version for it (only for v2+ licenses) then it will say:
0 - myplugin 1.4

or if a license file exists and there is no version (v1 licenses):
0 - myplugin

other return codes/output:
-3 - No license file found
-4 - License file corrupted
-5 - Generic error
-6 - invalid product name
-7 - trial license
-14 - network adapter error

in all other cases, just the error number will be printed
*/

int main(int argc, char* argv[]) {
	if (argc != 2) {
		cout << "aescriptsLicenseChecker v" << AESCRIPTSLICLIB_VERSION_TAG << " (" << AESCRIPTSLICLIB_BUILD_DATE << ")" << endl << endl;
		cout << "usage: aescriptsLicenseChecker [name]" << endl;
		cout << "[name] is the name of the product to be checked" << endl;
		return -1;
	}
	aescriptsLicenseData lData;
	aescriptsInitLicenseData(lData);
	int result = aescriptsGetLicenseDataFromFile(argv[1], -1, lData);

	switch (result) {
		case AESCRIPTSLICLIB_RESULT_OK:
		case AESCRIPTSLICLIB_RESULT_INVALID_FORMAT:
		case AESCRIPTSLICLIB_RESULT_WRONG_MACHINE:
		case AESCRIPTSLICLIB_RESULT_INVALID_TIMELIMITED_LIC_FORMAT:
		case AESCRIPTSLICLIB_RESULT_INVALID_LIC_TYPE:
		case AESCRIPTSLICLIB_RESULT_LIC_VERIFICATION_FAILED:
		case AESCRIPTSLICLIB_RESULT_LIC_BLOCKED:
		case AESCRIPTSLICLIB_RESULT_INVALID_LIC_KEY:
		case AESCRIPTSLICLIB_RESULT_LIC_PERIOD_NOT_STARTED:
		case AESCRIPTSLICLIB_RESULT_LIC_PERIOD_ENDED:
		case AESCRIPTSLICLIB_RESULT_INVALID_FLOATING_LICENSE: {
			// license file was found (but ignore if valid license or not)
			cout << "0 - " << argv[1];
			if (strlen(lData.licenseVersion) > 0) {
				// we have a license version set, so have a v2 license file with effect version included
				cout << " " << lData.productVersion;
			}
			cout << endl;
			break;
		}
		case AESCRIPTSLICLIB_RESULT_NO_LICFILE: {
			cout << "-3 - No license file found" << endl;
			break;
		}
		case AESCRIPTSLICLIB_RESULT_CORRUPTED_LICFILE: {
			cout << "-4 - License file corrupted" << endl;
			break;
		}
		case AESCRIPTSLICLIB_RESULT_GENERIC_ERROR: {
			cout << "-5 - Generic error" << endl;
			break;
		}
		case AESCRIPTSLICLIB_RESULT_INVALID_PRODUCT: {
			cout << "-6 - Invalid product name" << endl;
			break;
		}
		case AESCRIPTSLICLIB_RESULT_TRIAL: {
			cout << "-7 - Trial license" << endl;
			break;
		}
		case AESCRIPTSLICLIB_RESULT_NETWORK_ADAPTER_ERROR: {
			cout << "-14 - Network adapter error" << endl;
			break;
		}
		default: {
			cout << result << " - Unknown" << endl;
			break;
		}
	}
	return result;
}
