#ifndef AESCRIPTSLICENSING_H
#define AESCRIPTSLICENSING_H

#pragma once

#define AESCRIPTSLICLIB_VERSION 41
#define AESCRIPTSLICLIB_BUILD_DATE "20250424"
#define AESCRIPTSLICLIB_VERSION_FULL 414
#define AESCRIPTSLICLIB_VERSION_TAG "AESCRIPTSLICLIB 4.1.4"

#ifdef _WIN32
	#ifndef _CRT_SECURE_NO_WARNINGS
		#define _CRT_SECURE_NO_WARNINGS
	#endif

	#ifdef AESCRIPTSLICLIB_BUILD
		#define AESCRIPTSLICLIB_DLLINTERFACE __declspec(dllexport)
	#else
		#define AESCRIPTSLICLIB_DLLINTERFACE
	#endif
	#define AESCRIPTSLICLIB_CALLCONV __stdcall

#else
	#ifdef AESCRIPTSLICLIB_BUILD
		#define AESCRIPTSLICLIB_DLLINTERFACE __attribute__((visibility("default")))
	#else
		#define AESCRIPTSLICLIB_DLLINTERFACE
	#endif
	#define AESCRIPTSLICLIB_CALLCONV

#endif
#define AESCRIPTSLICLIB_INTERFACE AESCRIPTSLICLIB_DLLINTERFACE int AESCRIPTSLICLIB_CALLCONV

#if !defined(_MSC_VER) || (_MSC_VER >= 1900)
#include <stdbool.h>
#endif

// these are the common result codes used as return values for every library function call below
// numbers not in this list are either deprecated or are used internally only and are not exposed to the calling functions
enum AESCRIPTSLICLIB_RESULT_CODES {
	AESCRIPTSLICLIB_RESULT_OK = 0,
	AESCRIPTSLICLIB_RESULT_INVALID_FORMAT = -1,
	AESCRIPTSLICLIB_RESULT_WRONG_MACHINE = -2,
	AESCRIPTSLICLIB_RESULT_NO_LICFILE = -3,
	AESCRIPTSLICLIB_RESULT_CORRUPTED_LICFILE = -4,
	AESCRIPTSLICLIB_RESULT_GENERIC_ERROR = -5,
	AESCRIPTSLICLIB_RESULT_INVALID_PRODUCT = -6,
	AESCRIPTSLICLIB_RESULT_TRIAL = -7,

	AESCRIPTSLICLIB_RESULT_INVALID_TIMELIMITED_LIC_FORMAT = -8,
	AESCRIPTSLICLIB_RESULT_INVALID_LIC_TYPE = -9,
	AESCRIPTSLICLIB_RESULT_LIC_VERIFICATION_FAILED = -10,
	AESCRIPTSLICLIB_RESULT_LIC_BLOCKED = -11,
	AESCRIPTSLICLIB_RESULT_INVALID_LIC_KEY = -12,

	AESCRIPTSLICLIB_RESULT_NETWORK_ADAPTER_ERROR = -14,
	AESCRIPTSLICLIB_RESULT_LIC_PERIOD_NOT_STARTED = -15,
	AESCRIPTSLICLIB_RESULT_LIC_PERIOD_ENDED = -16,

	AESCRIPTSLICLIB_RESULT_INVALID_FLOATING_LICENSE = -20,
	AESCRIPTSLICLIB_RESULT_NO_CONNECTION_TO_SERVER = -21,
	AESCRIPTSLICLIB_RESULT_FLOATING_LIC_REQUIRES_SERVER = -22,
	AESCRIPTSLICLIB_RESULT_NO_SLOTS_ON_SERVER = -23,
	AESCRIPTSLICLIB_RESULT_NO_LICENSE_ON_SERVER = -24,
	AESCRIPTSLICLIB_RESULT_NO_LEASE_ON_SERVER = -25,
	AESCRIPTSLICLIB_RESULT_CLIENT_DENIED_BY_SERVER = -26,

	AESCRIPTSLICLIB_RESULT_ONLINE_INVALID = -30,
	AESCRIPTSLICLIB_RESULT_ONLINE_EXPIRED = -31,
	AESCRIPTSLICLIB_RESULT_ONLINE_MAX_ACTIVATIONS = -32,
	AESCRIPTSLICLIB_RESULT_ONLINE_NO_CONNECTION = -33,
	AESCRIPTSLICLIB_RESULT_ONLINE_INVALID_PAYLOAD = -34,
	AESCRIPTSLICLIB_RESULT_ONLINE_NO_DEVICEID = -35,
	AESCRIPTSLICLIB_RESULT_ONLINE_NO_RECORD = -36,
	AESCRIPTSLICLIB_RESULT_ONLINE_INVALID_REQUEST = -37,
	AESCRIPTSLICLIB_RESULT_ONLINE_INVALID_RESPONSE = -38,
	AESCRIPTSLICLIB_RESULT_ONLINE_INVALID_SUBSCRIPTION = -39,

	AESCRIPTSLICLIB_RESULT_OFFLINE_INVALID_CODE = -40
};

// this function converts a result code into a string with more information
static const char* resultCodeToMessage(int resultCode) {
	switch (resultCode) {
		case AESCRIPTSLICLIB_RESULT_OK: return "Valid license";
		case AESCRIPTSLICLIB_RESULT_INVALID_FORMAT: return "The license has an invalid format";
		case AESCRIPTSLICLIB_RESULT_WRONG_MACHINE: return "The license belongs to a different machine";
		case AESCRIPTSLICLIB_RESULT_NO_LICFILE: return "License file not found";
		case AESCRIPTSLICLIB_RESULT_CORRUPTED_LICFILE: return "License file is corrupted";
		case AESCRIPTSLICLIB_RESULT_GENERIC_ERROR: return "Generic error";
		case AESCRIPTSLICLIB_RESULT_INVALID_PRODUCT: return "This license is for a different product"; // "There is an issue with the product header";
		case AESCRIPTSLICLIB_RESULT_TRIAL: return "Trial mode";

		case AESCRIPTSLICLIB_RESULT_INVALID_TIMELIMITED_LIC_FORMAT: return "The time-limited license has an invalid format";
		case AESCRIPTSLICLIB_RESULT_INVALID_LIC_TYPE: return "The license type is not recognized";
		case AESCRIPTSLICLIB_RESULT_LIC_VERIFICATION_FAILED: return "Verification of the license format failed";
		case AESCRIPTSLICLIB_RESULT_LIC_BLOCKED: return "The license is no longer valid";
		case AESCRIPTSLICLIB_RESULT_INVALID_LIC_KEY: return "The license key is invalid";

		case AESCRIPTSLICLIB_RESULT_NETWORK_ADAPTER_ERROR: return "Error reading network adapter data";

		case AESCRIPTSLICLIB_RESULT_LIC_PERIOD_NOT_STARTED: return "License period has not started yet";
		case AESCRIPTSLICLIB_RESULT_LIC_PERIOD_ENDED: return "License period has ended";

		case AESCRIPTSLICLIB_RESULT_INVALID_FLOATING_LICENSE: return "Invalid floating license - Please check that the license code was entered correctly in the server config";
		case AESCRIPTSLICLIB_RESULT_NO_CONNECTION_TO_SERVER: return "Cannot connect to the floating license server";
		case AESCRIPTSLICLIB_RESULT_FLOATING_LIC_REQUIRES_SERVER: return "Floating licenses can only be used with the floating license server";
		case AESCRIPTSLICLIB_RESULT_NO_SLOTS_ON_SERVER: return "There are no more free slots for this product on the floating license server";
		case AESCRIPTSLICLIB_RESULT_NO_LICENSE_ON_SERVER: return "The license cannot be found on the floating license server";
		case AESCRIPTSLICLIB_RESULT_NO_LEASE_ON_SERVER: return "The license you are trying to deactivate is not found on the floating license server";
		case AESCRIPTSLICLIB_RESULT_CLIENT_DENIED_BY_SERVER: return "Your client IP is denied by the floating license server";

		case AESCRIPTSLICLIB_RESULT_ONLINE_INVALID: return "Online activation: the license is invalid";
		case AESCRIPTSLICLIB_RESULT_ONLINE_EXPIRED: return "Online activation: the license is no longer valid";
		case AESCRIPTSLICLIB_RESULT_ONLINE_MAX_ACTIVATIONS: return "Online activation: maximum number of activations reached - Please deactivate an existing activation before activating this one";
		case AESCRIPTSLICLIB_RESULT_ONLINE_NO_CONNECTION: return "Cannot connect to online activation server";
		case AESCRIPTSLICLIB_RESULT_ONLINE_INVALID_PAYLOAD: return "Online activation: invalid payload";
		case AESCRIPTSLICLIB_RESULT_ONLINE_NO_DEVICEID: return "Online activation: missing device ID - This activation needs to be deactivated in your aescripts.com account, please click Get Help for more details";
		case AESCRIPTSLICLIB_RESULT_ONLINE_NO_RECORD: return "Online activation: no matching activation found - Please click Get Help for assistance";
		case AESCRIPTSLICLIB_RESULT_ONLINE_INVALID_REQUEST: return "Online activation: missing parameters - Please click Get Help for assistance";
		case AESCRIPTSLICLIB_RESULT_ONLINE_INVALID_RESPONSE: return "Online activation: invalid response from server - This activation needs to be deactivated in your aescripts.com account, please click Get Help for more details";
		case AESCRIPTSLICLIB_RESULT_ONLINE_INVALID_SUBSCRIPTION: return "Online activation: subscription is not valid";
		case AESCRIPTSLICLIB_RESULT_OFFLINE_INVALID_CODE: return "Invalid offline activation code";
	}
	return "Unknown error";
};

#define AESCRIPTSLICLIB_URL_LICENSE_CODE_FAQ "https://aescripts.com/knowledgebase/index/view/faq/license-code-faq/"
#define AESCRIPTSLICLIB_URL_FLOATING_LICENSE_FAQ "https://aescripts.com/knowledgebase/index/view/faq/floating-license-faq/"
#define AESCRIPTSLICLIB_URL_LICENSE_ACTIVATION_MANAGEMENT "https://aescripts.com/knowledgebase/index/view/faq/license-activation-management/"
#define AESCRIPTSLICLIB_URL_SUBSCRIPTION_FAQ "https://aescripts.com/knowledgebase/index/view/faq/product-subscription-faq/"
#define AESCRIPTSLICLIB_URL_CONTACT "https://aescripts.com/contact/?direct=1&subject="

#define AESCRIPTSLICLIB_URL_CONTACT_NO_LICFILE AESCRIPTSLICLIB_URL_CONTACT "License%20file%20not%20found%20-3"
#define AESCRIPTSLICLIB_URL_CONTACT_CORRUPTED_LICFILE AESCRIPTSLICLIB_URL_CONTACT "License%20file%20is%20corrupted%20-4"
#define AESCRIPTSLICLIB_URL_CONTACT_GENERIC_ERROR AESCRIPTSLICLIB_URL_CONTACT "Generic%20license%20error%20-5"
#define AESCRIPTSLICLIB_URL_CONTACT_INVALID_PRODUCT AESCRIPTSLICLIB_URL_CONTACT "Product%20header%20is%20not%20valid%20-6"
#define AESCRIPTSLICLIB_URL_CONTACT_LIC_BLOCKED AESCRIPTSLICLIB_URL_CONTACT "License%20is%20no%20longer%20valid%20-11"
#define AESCRIPTSLICLIB_URL_CONTACT_INVALID_LIC_KEY AESCRIPTSLICLIB_URL_CONTACT "License%20key%20is%20invalid%20-12"
#define AESCRIPTSLICLIB_URL_CONTACT_NETWORK_ADAPTER_ERROR AESCRIPTSLICLIB_URL_CONTACT "Could%20not%20find%20a%20network%20adapter%20on%20this%20machine%20-14"
#define AESCRIPTSLICLIB_URL_CONTACT_ONLINE_INVALID_PAYLOAD AESCRIPTSLICLIB_URL_CONTACT "invalid%20activation%20payload%20-34"
#define AESCRIPTSLICLIB_URL_CONTACT_ONLINE_NO_RECORD AESCRIPTSLICLIB_URL_CONTACT "no%20matching%20activation%20id%20-36"
#define AESCRIPTSLICLIB_URL_CONTACT_ONLINE_INVALID_REQUEST AESCRIPTSLICLIB_URL_CONTACT "activation%20payload%20missing%20parameters%20-37"

// returns a URL associated with a certain result code
static const char* getHelpURL(int resultCode) {
	switch (resultCode) {
		case AESCRIPTSLICLIB_RESULT_INVALID_FLOATING_LICENSE:
		case AESCRIPTSLICLIB_RESULT_NO_CONNECTION_TO_SERVER:
		case AESCRIPTSLICLIB_RESULT_FLOATING_LIC_REQUIRES_SERVER:
		case AESCRIPTSLICLIB_RESULT_NO_SLOTS_ON_SERVER:
		case AESCRIPTSLICLIB_RESULT_NO_LICENSE_ON_SERVER:
		case AESCRIPTSLICLIB_RESULT_NO_LEASE_ON_SERVER:
		case AESCRIPTSLICLIB_RESULT_CLIENT_DENIED_BY_SERVER:
			{
				return AESCRIPTSLICLIB_URL_FLOATING_LICENSE_FAQ;
				break;
			}
		case AESCRIPTSLICLIB_RESULT_ONLINE_MAX_ACTIVATIONS:
		case AESCRIPTSLICLIB_RESULT_ONLINE_NO_CONNECTION:
		case AESCRIPTSLICLIB_RESULT_ONLINE_NO_DEVICEID:
		case AESCRIPTSLICLIB_RESULT_OFFLINE_INVALID_CODE:
		case AESCRIPTSLICLIB_RESULT_ONLINE_INVALID_RESPONSE:
			{
				return AESCRIPTSLICLIB_URL_LICENSE_ACTIVATION_MANAGEMENT;
				break;
			}
		case AESCRIPTSLICLIB_RESULT_ONLINE_INVALID_SUBSCRIPTION:
			{
				return AESCRIPTSLICLIB_URL_SUBSCRIPTION_FAQ;
				break;
			}
		case AESCRIPTSLICLIB_RESULT_NO_LICFILE: return AESCRIPTSLICLIB_URL_CONTACT_NO_LICFILE;
		case AESCRIPTSLICLIB_RESULT_CORRUPTED_LICFILE: return AESCRIPTSLICLIB_URL_CONTACT_CORRUPTED_LICFILE;
		case AESCRIPTSLICLIB_RESULT_GENERIC_ERROR: return AESCRIPTSLICLIB_URL_CONTACT_GENERIC_ERROR;
		case AESCRIPTSLICLIB_RESULT_INVALID_PRODUCT: return AESCRIPTSLICLIB_URL_CONTACT_INVALID_PRODUCT;
		case AESCRIPTSLICLIB_RESULT_LIC_BLOCKED: return AESCRIPTSLICLIB_URL_CONTACT_LIC_BLOCKED;
		case AESCRIPTSLICLIB_RESULT_INVALID_LIC_KEY: return AESCRIPTSLICLIB_URL_CONTACT_INVALID_LIC_KEY;
		case AESCRIPTSLICLIB_RESULT_NETWORK_ADAPTER_ERROR: return AESCRIPTSLICLIB_URL_CONTACT_NETWORK_ADAPTER_ERROR;
		case AESCRIPTSLICLIB_RESULT_ONLINE_INVALID_PAYLOAD: return AESCRIPTSLICLIB_URL_CONTACT_ONLINE_INVALID_PAYLOAD;
		case AESCRIPTSLICLIB_RESULT_ONLINE_NO_RECORD: return AESCRIPTSLICLIB_URL_CONTACT_ONLINE_NO_RECORD;
		case AESCRIPTSLICLIB_RESULT_ONLINE_INVALID_REQUEST: return AESCRIPTSLICLIB_URL_CONTACT_ONLINE_INVALID_REQUEST;
	}
	return AESCRIPTSLICLIB_URL_LICENSE_CODE_FAQ;
}

typedef char aescriptsLicString[128];
typedef char aescriptsMachineString[60];
typedef char aescriptsVerString[11];
typedef char aescriptsLicVerString[6];
typedef char aescriptsLicenseFileString[1024];

// this is a class that holds the decoded user license information
struct aescriptsLicenseData {
	char firstName[30]; // user's first name
	char lastName[30]; // user's last name
	int nofUsers; // number of licensed users
	char licType[10]; // license type (SUL, BTA, EDU, REN, FLT, SUB*, FSU)
	char productID[30]; // product ID
	char serial[64]; // license serial string

	aescriptsLicString license; // complete license string
	aescriptsVerString productVersion; // product version
	aescriptsLicVerString licenseVersion; // version of the license file

	int firstStartTimestamp; // timestamp of first start
	int numberOfDaysSinceFirstStart; // number of days since first start
	bool registered; // true if license is valid
	int overused; // same license active in the network? -1: unknown, 0: no, 1: yes
	bool renderOK; // is rendering allowed with this license/state?

	char startDate[11]; // time-limited licenses: start date in format YYYY-MM-DD
	char endDate[11]; // time-limited licenses: end date in format YYYY-MM-DD

	int lastActivationTryTimestamp; // last time the product was tried to be activated online
	int lastActivationTimestamp; // last time the product was successfully activated online
	int daysUntilNextActivation; // days until another online activation is necessary
	int numberOfActivations; // number of successful activations

	int lastResultCode; // result code of the the last library function call
};

#ifdef __cplusplus
extern "C"
{
#endif

// this function initializes/clears an aescriptsLicenseData object
AESCRIPTSLICLIB_INTERFACE aescriptsInitLicenseData(aescriptsLicenseData& _lData);

// load and decode the content of the license file from disk but don't validate it
// the privNum can be set to -1 to load the content without validation, but then the return value will be meaningless, only the lData values will be set
AESCRIPTSLICLIB_INTERFACE aescriptsGetLicenseDataFromFile(const char* _licenseFileName, long long _privNum, aescriptsLicenseData& _lData);

// load the raw content of the license file from disk
AESCRIPTSLICLIB_INTERFACE aescriptsGetRawLicenseDataFromFile(const char* _licenseFileName, aescriptsLicenseFileString& data);

// load license file from disk and use it immediately (in case of floating licenses, implicitly request a lease from the floating server)
AESCRIPTSLICLIB_INTERFACE aescriptsLoadLicenseFromFile(const char* _licenseFileName, const char* _productID, long long _privNum, aescriptsLicenseData& _lData, bool _createTrialLicense, bool _renderOnlyMode);

// save license to disk
AESCRIPTSLICLIB_INTERFACE aescriptsSaveLicenseToFile(const char* _licenseFileName, const char* _productID, aescriptsLicString _lic, aescriptsVerString _productVersion);

// validate license string (usually not needed to be called directly, used implicitly by loadLicenseFromFile)
AESCRIPTSLICLIB_INTERFACE aescriptsValidateLicense(aescriptsLicString _license, const char* _productID, long long _privNum, aescriptsLicenseData& _lData, bool _renderOnlyMode);

// get machine ID (usually not needed to be called directly)
AESCRIPTSLICLIB_INTERFACE aescriptsGetMachineId(aescriptsMachineString& _result);

// dynamically add data for a blocked serial
AESCRIPTSLICLIB_INTERFACE aescriptsAddBlockedSerial(long long _privNum, int _d1, int _d2, int _d3, int _d4);

// get overuse state (usually not needed to be called directly, used implicitly by validateLicense)
AESCRIPTSLICLIB_INTERFACE aescriptsCheckOveruse(aescriptsLicenseData& _lData);

// get server and port for (optional) licensing server
AESCRIPTSLICLIB_INTERFACE aescriptsGetLicenseServerConfig(aescriptsLicString& _cServer, aescriptsLicString& _cPort, aescriptsLicString& _cBackupServer, aescriptsLicString& _cBackupPort);

// returns licensing library version as integer (e.g. 40 meaning its version 4.0)
AESCRIPTSLICLIB_INTERFACE aescriptsGetLicLibVersion();

// returns licensing library version as integer (e.g. 40 meaning its version 4.0) and detailed build version information in the buildInfo parameter
AESCRIPTSLICLIB_INTERFACE aescriptsGetLicLibBuildVersion(aescriptsLicString& buildInfo);

// request a floating license from the license server (usually not needed to be called directly, used implicitly by loadLicenseFromFile)
AESCRIPTSLICLIB_INTERFACE aescriptsRequestLicenseFromLicenseServer(const char* _productID, aescriptsVerString _productVersion, long long _privNum, aescriptsLicString& _license);

// drop a floating license from the license server (should be done at unloading/exit of the plugin/application)
AESCRIPTSLICLIB_INTERFACE aescriptsDropLicenseFromLicenseServer(const char* _productID, long long _privNum);

// this function is only used for special cases by developers, as instructed by the licensing library support team
AESCRIPTSLICLIB_INTERFACE aescriptsConfigure(int token, int value);

// simple alias for convenience
#define getLicenseServerPort aescriptsGetLicenseServerConfig
#define addBlacklistedSerial aescriptsAddBlockedSerial

#ifdef __cplusplus
} //#ifdef  __cplusplus
#endif

#ifdef __cplusplus
#ifndef AESCRIPTSLICLIB_BUILD

// C++ wrapper interface
namespace aescripts {
	static aescriptsLicenseData LicenseData;
	typedef char licString[128];
	typedef char machineString[60];
	typedef char verString[11];
	typedef char licVerString[6];
	static int initLicenseData(aescriptsLicenseData& _lData) { return aescriptsInitLicenseData(_lData); }
	static int getLicenseDataFromFile(const char* _licenseFileName, long long _privNum, aescriptsLicenseData& _lData) { return aescriptsGetLicenseDataFromFile(_licenseFileName, _privNum, _lData); }
	static int getRawLicenseDataFromFile(const char* _licenseFileName, aescriptsLicenseFileString& data) { return aescriptsGetRawLicenseDataFromFile(_licenseFileName, data); }
	static int loadLicenseFromFile(const char* _licenseFileName, const char* _productID, long long _privNum, aescriptsLicenseData& _lData, bool _createTrialLicense = true, bool _renderOnlyMode = false) { return aescriptsLoadLicenseFromFile(_licenseFileName, _productID, _privNum, _lData, _createTrialLicense, _renderOnlyMode); }
	static int saveLicenseToFile(const char* _licenseFileName, const char* _productID, aescriptsLicString _lic, aescriptsVerString _productVersion) { return aescriptsSaveLicenseToFile(_licenseFileName, _productID, _lic, _productVersion); }
	static int validateLicense(aescriptsLicString _license, const char* _productID, long long _privNum, aescriptsLicenseData& _lData, bool _renderOnlyMode= false) { return aescriptsValidateLicense(_license, _productID, _privNum, _lData, _renderOnlyMode); }
	static int getMachineId(aescriptsMachineString& _result) { return aescriptsGetMachineId(_result); }
	static int addBlockedSerial(long long _privNum, int _d1, int _d2, int _d3, int _d4) { return aescriptsAddBlockedSerial(_privNum, _d1, _d2, _d3, _d4); }
	static int checkOveruse(aescriptsLicenseData& _lData) { return aescriptsCheckOveruse(_lData); }
	static int getLicenseServerConfig(aescriptsLicString& _cServer, aescriptsLicString& _cPort, aescriptsLicString& _cBackupServer, aescriptsLicString& _cBackupPort) { return aescriptsGetLicenseServerConfig(_cServer, _cPort, _cBackupServer, _cBackupPort); }
	static int getLicLibVersion() { return aescriptsGetLicLibVersion(); }
	static int getLicLibBuildVersion(aescriptsLicString& buildInfo) { return aescriptsGetLicLibBuildVersion(buildInfo); }
	static int requestLicenseFromLicenseServer(const char* _productID, aescriptsVerString _productVersion, long long _privNum, aescriptsLicString& _license) { return aescriptsRequestLicenseFromLicenseServer(_productID, _productVersion, _privNum, _license); }
	static int dropLicenseFromLicenseServer(const char* _productID, long long _privNum) { return aescriptsDropLicenseFromLicenseServer(_productID, _privNum); }
	static int configure(int token, int value) { return aescriptsConfigure(token, value); }
};
#endif

#endif

#define AESCRIPTS_CONFIGURE_DISABLE_ONLINE_ACTIVATION 4

#endif // AESCRIPTSLICENSING_H
