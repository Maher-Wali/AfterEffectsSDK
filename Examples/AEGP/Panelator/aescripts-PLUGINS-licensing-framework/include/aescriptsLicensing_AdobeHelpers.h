#pragma once
#ifdef _WIN32
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#endif

#ifndef AESCRIPTSLICENSING_ADOBEHELPERS_H
#define AESCRIPTSLICENSING_ADOBEHELPERS_H

// This is a helper class for the aescripts licensing framework,
// providing some more high-level functionality specific to After Effects and Premiere Pro as host application
// version 4.1.4
#define REQUIRED_AESCRIPTSLICLIB_VERSION 414

#if (AESCRIPTSLICLIB_VERSION_FULL != REQUIRED_AESCRIPTSLICLIB_VERSION)
#error You are using non-matching versions of aescriptsLicensing.h and aescriptsLicensing_AdobeHelpers.h. Please use both files from the same licensing library download location
#endif

#ifndef TMSGDLG_H
#include "TMsgDlg.h"
#endif

#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <algorithm>

#include <fstream>
#include <time.h>
#ifdef _WIN32
#include <Shlobj.h>
#endif

namespace aescripts {
	
static aescriptsLicenseData licenseData;
static int lastOveruseState = -1;
static bool lc_init = false;

#include "TMsgDlg.jsx.h"

#if defined(AE_OS_MAC) || defined(AE_OS_WIN)

int getLicense(aescriptsLicenseData& lData, AEGP_SuiteHandler* suitesP, bool renderOnlyMode = false, const char* tokenName = "Layout", A_long tokenValue = 1);
PF_Err showRegistrationDialog(AEGP_SuiteHandler* AESuitesP, aescriptsLicenseData& lData, bool useScripting = true, const char* productVersion = NULL);

static std::string intToString(int i) {
	std::stringstream ss;
	std::string s;
	ss << i;
	s = ss.str();
	return s;
}

static std::string strReplace(const std::string s, const std::string search, std::string replace) {
	std::string str = s;
	size_t pos = 0;
	while ((pos = str.find(search, pos)) != std::string::npos) {
		str.replace(pos, search.size(), replace);
		pos += replace.size();
	}
	return str;
}

inline void openOS(const char* cmd) {
	#ifdef AE_OS_WIN
		ShellExecuteA(NULL, "open", cmd, NULL, NULL, SW_SHOWNORMAL);
	#else
		std::string s = "open " + std::string(cmd);
		system(s.c_str());
	#endif
}

int getLicense(aescriptsLicenseData& lData, AEGP_SuiteHandler* suitesP, bool renderOnlyMode, const char* tokenName, A_long tokenValue) {
	if (std::string(lData.licType) == "FLT") return loadLicenseFromFile(LIC_FILENAME, LIC_PRODUCT_ID, LIC_PRIVATE_NUM, lData, false, renderOnlyMode);
	A_Err err = A_Err_NONE;
	AEGP_PersistentBlobH blobH = NULL;
	A_Boolean foundB = FALSE;
	if (suitesP) {
		ERR(suitesP->PersistentDataSuite3()->AEGP_GetApplicationBlob(&blobH));
		ERR(suitesP->PersistentDataSuite3()->AEGP_DoesKeyExist(blobH, LIC_FILENAME, tokenName, &foundB));
	}
	if (foundB) {
		return loadLicenseFromFile(LIC_FILENAME, LIC_PRODUCT_ID, LIC_PRIVATE_NUM, lData, false, renderOnlyMode);
	} else {
		if (suitesP) ERR(suitesP->PersistentDataSuite3()->AEGP_SetLong(blobH, LIC_FILENAME, tokenName, tokenValue));
		return loadLicenseFromFile(LIC_FILENAME, LIC_PRODUCT_ID, LIC_PRIVATE_NUM, lData, true, renderOnlyMode);
	}
}

static std::string getLicenseDataAsString(const aescriptsLicenseData lData) {
	std::string lic = std::string(lData.licType);
	if (!lData.registered) return "Trial License";
	std::string n = intToString(lData.nofUsers);
	std::string regName = std::string(lData.firstName) + " " + std::string(lData.lastName);
	std::string myLicense = (n == "1") ? "Single User" : (n + " User");
	if (lic == "SUL") ;
	else if (lic == "BTA") myLicense += " Beta";
	else if (lic == "EDU") myLicense += " Education";
	else if (lic == "REN") myLicense += " Render-Only";
	else if (lic == "FLT") myLicense += " Floating";
	else if (lic == "SUB") myLicense += " Subscription";
	else if (lic == "ESB") myLicense += " Educational Subscription";
	else myLicense = " Unknown";
	myLicense += " License";
	std::string msg = regName + "\n" + myLicense;
	return msg;
}

inline int checkLicense(AEGP_SuiteHandler* suitesP, aescriptsLicenseData& lData, aescriptsLicString& errorMessage) {
	PF_Boolean bIsRenderEngine = true;
	if (suitesP) {
		try {
			suitesP->AppSuite4()->PF_IsRenderEngine(&bIsRenderEngine);
		} catch(...) {
		}
	}

	std::string errorMsg = "";
	int licResult = aescripts::getLicense(lData, suitesP, bIsRenderEngine?true:false);
	lData.registered = (licResult == 0);
	lData.renderOK = true;

	// check if plugin has a render-only license
	if (strcmp(lData.licType, "REN") == 0) {
		// render-only license, but we are NOT in the renderer!
		if (!bIsRenderEngine) {
			errorMsg = "The plugin '" + std::string(LIC_PRODUCT_NAME) + "' has a render-only license that only works in the command-line renderer!";
			lData.renderOK = false; // set flag to disable rendering
		}

	// check if plugin has a beta-only license
	} else if (strcmp(lData.licType, "BTA") == 0) {

		// only check this if we are NOT in beta mode
		#ifndef LIC_BETA
			errorMsg = "The plugin '" + std::string(LIC_PRODUCT_NAME) + "' uses a beta-only license that is no longer valid in this version!";
			lData.renderOK = false; // set flag to disable rendering
		#endif
	}
	strcpy(errorMessage, errorMsg.substr(0, 127).c_str());
	return licResult;
}

static void strSplit(const std::string& s, const char seperator, std::vector<std::string>& results) {
	results.clear();
	std::string::size_type prev_pos = 0, pos = 0;
	while ((pos = s.find(seperator, pos)) != std::string::npos) {
		std::string substring(s.substr(prev_pos, pos - prev_pos) );
		results.push_back(substring);
		prev_pos = ++pos;
	}
	std::string substring = s.substr(prev_pos, pos - prev_pos);
	results.push_back(substring);
}

static std::string strTrim(const std::string& str, const std::string& whitespace = " \t") {
	const size_t strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::string::npos) return ""; // no content
	const size_t strEnd = str.find_last_not_of(whitespace);
	const size_t strRange = strEnd - strBegin + 1;
	return str.substr(strBegin, strRange);
}

static std::string strSafe(const std::string inp, const std::string charsToRemove) {
	std::string outp;
	for (int i = 0; i < (int)inp.size(); i++) {
		if (inp[i] == 0) continue;
		std::string tmp = inp.substr(i, 1);
		if ((int)tmp.find_first_of(charsToRemove) >= 0) continue;
		outp += inp[i];
	}
	return outp;
}

PF_Err showRegistrationDialog(AEGP_SuiteHandler* AESuitesP, aescriptsLicenseData& lData, bool useScripting, const char* productVersion) {

	enum DLG_STATE {
		DLG_STATE_ERROR = -1,
		DLG_STATE_ACTIVATION = 0,
		DLG_STATE_DEACTIVATION = 1,
		DLG_STATE_ACTIVATION_SUCCESS = 2,
		DLG_STATE_DEACTIVATION_SUCCESS = 3,
		DLG_STATE_EXCHANGE_LICENSE = 4,
		DLG_STATE_DONE = 5
	};
	
	/*
	state			b1			b2			b3
	------------------------------------------------
	error			-			get help	ok
	activation		cancel		get help	activate
	deactivation	deactivate	-			ok
	(de)act succ	-			-			ok
	exchange		cancel		-			ok
	*/

	lastOveruseState = -1;
	PF_Err err = PF_Err_NONE;

	std::string dlgTitle = std::string(LIC_PRODUCT_NAME);
	if (productVersion && strlen(productVersion) > 0) {
		dlgTitle += " v" + std::string(productVersion);
		int l = (int)strlen(productVersion);
		if (l > 10) l = 10;
		strncpy(aescripts::licenseData.productVersion, productVersion, l);
	} else {
		strcpy(aescripts::licenseData.productVersion, "1.0.0");
	}
	dlgTitle += " Registration";

	aescriptsLicString errorMessage;
	memset(errorMessage, 0, 128);

	int dlgState = DLG_STATE_ACTIVATION;
	if (lData.registered) dlgState = DLG_STATE_DEACTIVATION;

	std::string inputText;

	aescriptsLicString cServer, cPort, cBackupServer, cBackupPort;
	memset(cServer, 0, 128);
	memset(cPort, 0, 128);
	memset(cBackupServer, 0, 128);
	memset(cBackupPort, 0, 128);
	aescripts::getLicenseServerConfig(cServer, cPort, cBackupServer, cBackupPort);
	std::string server = std::string(cServer);
	std::string port = std::string(cPort);
	std::string sServerPort;
	if (server != "" && port != "") {
		inputText = "@REMOTE";
		std::string lic = std::string(LIC_PRODUCT_ID) + inputText;
		strcpy(lData.license, lic.substr(0, 127).c_str());
	}

	std::string result, dlgMsg;

	do {
		std::string sUrl = AESCRIPTSLICLIB_URL_LICENSE_CODE_FAQ;
		std::string b1;
		std::string b2;
		std::string b3;
		bool showInput = false;
		if (dlgState == DLG_STATE_ACTIVATION) {
			b1 = "Cancel";
			b2 = "Get Help";
			b3 = "Activate";
			showInput = true;
			dlgMsg = "TRIAL VERSION\nIf you have a license code, please enter it below and click on 'Activate'.";
		} else if (dlgState == DLG_STATE_DEACTIVATION) {
			b1 = "Cancel";
			b2 = "Deactivate";
			b3 = "OK";
			showInput = false;

			aescripts::checkLicense(AESuitesP, lData, errorMessage);
			if (lData.registered && std::string(lData.licType) != "FLT") inputText = "";

			dlgMsg = aescripts::getLicenseDataAsString(lData);
			if (std::string(lData.licType) == "FLT") {
				dlgMsg += " from " + server + ":" + port;
			}
			dlgMsg = "Registered to: " + dlgMsg;
		} else if (dlgState == DLG_STATE_ACTIVATION_SUCCESS) {
			dlgMsg = "Activation successful";
			b1 = "";
			b2 = "";
			b3 = "OK";
			showInput = false;
			dlgState = DLG_STATE_DONE;
		} else if (dlgState == DLG_STATE_DEACTIVATION_SUCCESS) {
			dlgMsg = "Deactivation successful";
			b1 = "";
			b2 = "";
			b3 = "OK";
			showInput = false;
			dlgState = DLG_STATE_DONE;
		} else if (dlgState == DLG_STATE_EXCHANGE_LICENSE) {
			sUrl = "https://license.aescripts.com/exchange/";
			dlgMsg = "This is a temporary serial number that needs to be exchanged for a permanent license.\nWould you like to go to " + sUrl + " ?";

			sUrl += "?serial=" + inputText;
			b1 = "Cancel";
			b2 = "";
			b3 = "OK";
			showInput = false;
		} else {
			dlgMsg = "Error " + intToString(dlgState) + ": " + std::string(resultCodeToMessage(dlgState));
			sUrl = getHelpURL(dlgState);
			b1 = "Cancel";
			b2 = "Get Help";
			b3 = "OK";
			showInput = false;
		}

		int dlgOk = 0;
		int dlgButton = -1;
		if (useScripting) { // if host is After Effects (not Premiere Pro), we can do script registration
			std::string scr = std::string((const char*)&TMsgDlg_jsx[0], TMsgDlg_jsx_len);

			dlgMsg = strReplace(dlgMsg, "\n", "\\n");
			dlgTitle = strSafe(dlgTitle, "\"\r\n\t");
			dlgMsg = strSafe(dlgMsg, "\"\r\n\t");
			b1 = strSafe(b1, "\"\r\n\t");
			b2 = strSafe(b2, "\"\r\n\t");
			b3 = strSafe(b3, "\"\r\n\t");
			inputText = strSafe(inputText, "\"\r\n\t");

			scr = strReplace(scr, "##DLG_TITLE##", dlgTitle);
			scr = strReplace(scr, "##DLG_MSG##", dlgMsg);
			scr = strReplace(scr, "##DLG_BTN1##", b1);
			scr = strReplace(scr, "##DLG_BTN2##", b2);
			scr = strReplace(scr, "##DLG_BTN3##", b3);
			scr = strReplace(scr, "##DLG_SHOWINPUT##", showInput?"1":"0");
			scr = strReplace(scr, "##DLG_INPUT##", inputText);
			AEGP_MemHandle resultMemH = NULL;
			char* prompt = new char[8192];
			try {
				memset(prompt, 0, 8192);
				strcat(prompt, scr.c_str());			
				A_char *resultAC = NULL;
				ERR(AESuitesP->UtilitySuite5()->AEGP_ExecuteScript(NULL, prompt, false, &resultMemH, NULL));
				ERR(AESuitesP->MemorySuite1()->AEGP_LockMemHandle(resultMemH, reinterpret_cast<void**>(&resultAC)));
				result = strSafe(std::string(resultAC), " \"\r\n\t");
				ERR(AESuitesP->MemorySuite1()->AEGP_FreeMemHandle(resultMemH));
			} catch(...) {
			}
			delete [] prompt;
			if (result != "") {
				std::vector<std::string> res;
				strSplit(result, '|', res);
				if (res.size() >= 3) {
					dlgOk = atoi(res[0].c_str());
					dlgButton = atoi(res[1].c_str());
					result = res[2];
				} else {
					dlgOk = 0;
					dlgButton = -1;
					result = "";
				}
			} else {
				dlgOk = 0;
				dlgButton = -1;
				result = "";
			}
		} else {
			CTMsgDlg dlg(450, 180);
			dlgOk = dlg.ShowDialog(dlgTitle.c_str(), dlgMsg.c_str(), b1.c_str(), b2.c_str(), b3.c_str(), showInput, inputText.c_str());
			dlgButton = (int)dlg.lBtnSelected;
			result = strSafe(std::string(dlg.cInputText), " \"\r\n\t");
		}
		
		if (result != "" && result[0] == '^') {
			dlgState = atoi(result.substr(1).c_str());
			result = "";
			continue;
		}

		if (dlgButton < 0) return -1;
		if (dlgState == DLG_STATE_DONE) return 0;

		if (dlgState == DLG_STATE_ACTIVATION) {
			if (dlgButton == 2) { // get help
				openOS(sUrl.c_str());
				continue;
			} else if (dlgButton == 1) { // cancel
				return -1;
			}
			if (result.length() <= 6 || result.length() > 127) {
				dlgState = -1;
				continue;
			}
			aescripts::licString l;
			strcpy(l, std::string(result).substr(0, 127).c_str());
			inputText = result; // keep current entry as default

			if (inputText.length() == 32) {
				int tmpCnt = 0;
				for (int i = 0; i < 32; i++) {
					if (
						(inputText[i] >= 'A' && inputText[i] <= 'Z') ||
						(inputText[i] >= 'a' && inputText[i] <= 'z') ||
						(inputText[i] >= '0' && inputText[i] <= '9') ) tmpCnt++;
				}
				if (inputText[0] >= '0' && inputText[0] <= '9') tmpCnt = 0;
				if (inputText[1] >= '0' && inputText[1] <= '9') tmpCnt = 0;
				if (tmpCnt == 32) {
					dlgState = DLG_STATE_EXCHANGE_LICENSE;
				}
			} 
			if (dlgState != DLG_STATE_EXCHANGE_LICENSE) {
				int valid = 0;
				if (result[0] != '@') {
					valid = aescripts::validateLicense(l, LIC_PRODUCT_ID, LIC_PRIVATE_NUM, lData, false);
				}
				if (valid < 0) {
					if (lData.lastResultCode < 0) {
						dlgState = lData.lastResultCode;
					} else {
						dlgState = DLG_STATE_ERROR;
					}
				} else {
					lData.lastResultCode = aescripts::saveLicenseToFile(LIC_FILENAME, LIC_PRODUCT_ID, l, lData.productVersion);								
					if (lData.lastResultCode < 0) {
						dlgState = lData.lastResultCode;
						lData.registered = false;
					} else {
						int licResult = aescripts::checkLicense(AESuitesP, lData, errorMessage);
						if (lData.registered) {
							dlgState = DLG_STATE_ACTIVATION_SUCCESS;
						} else {
							dlgState = licResult;
						}
					}
				}
			}
		} else if (dlgState == DLG_STATE_DEACTIVATION) {
			if (dlgButton == 2) { // deactivate
				aescripts::licString l = { };
				l[0] = 't'; l[1] = 125; l[2] = '\0';
				aescripts::saveLicenseToFile(LIC_FILENAME, LIC_PRODUCT_ID, l, lData.productVersion);
				aescripts::checkLicense(AESuitesP, lData, errorMessage);
				lData.registered = false;
				lData.nofUsers = 0;
				dlgState = DLG_STATE_DEACTIVATION_SUCCESS;
			} else if (dlgButton == 1) { // cancel
				return -1;
			} else {
				return 0;
			}
		} else if (dlgState == DLG_STATE_EXCHANGE_LICENSE) {
			if (dlgButton == 3) { // open exchange URL
				openOS(sUrl.c_str());
			} else if (dlgButton == 1) { // cancel
				return -1;
			}
			dlgState = DLG_STATE_ACTIVATION;
		} else {
			if (dlgButton == 2) {
				openOS(sUrl.c_str());
			} else if (dlgButton == 1) { // cancel
				return -1;
			}
			dlgState = DLG_STATE_ACTIVATION;
		}
	} while (dlgState != DLG_STATE_DONE);
	aescripts::checkLicense(AESuitesP, lData, errorMessage);
	return 0;
}

inline int checkLicenseAE(PF_Cmd cmd, PF_InData* in_data, PF_OutData* out_data, aescriptsLicenseData& lData) {
	int result = 0;
	if (cmd == PF_Cmd_GLOBAL_SETUP) {
		// initialize license data to empty fields on startup
		aescripts::initLicenseData(lData);
		#ifdef _DEBUG
			aescripts::configure(AESCRIPTS_CONFIGURE_DISABLE_ONLINE_ACTIVATION, 1);
		#endif
	} else if (cmd == PF_Cmd_GLOBAL_SETDOWN) {
		// drop license if it is a floating license
		aescripts::dropLicenseFromLicenseServer(LIC_PRODUCT_ID, LIC_PRIVATE_NUM);
	} else if ((cmd == PF_Cmd_SEQUENCE_SETUP || cmd == PF_Cmd_SEQUENCE_RESETUP)) {
		if (!lc_init) {
			// check for license when a new instance of the plugin is created (SequenceSetup) or an old one is loaded
			lc_init = true;
			AEGP_SuiteHandler suites(in_data->pica_basicP);
			licString errorString;		
			memset(errorString, 0, 128);
			bool doCheck = true;
			if (doCheck) {
				int lResult = aescripts::checkLicense(&suites, lData, errorString);
				lData.lastResultCode = lResult;
				PF_SPRINTF(out_data->return_msg, errorString);
			}

			int lver = aescripts::getLicLibVersion();
			if (lver != AESCRIPTSLICLIB_VERSION) {
				PF_SPRINTF(out_data->return_msg, "Non-matching licensing library versions!");
			}

		}
		checkOveruse(lData);
	} else if (cmd == PF_Cmd_USER_CHANGED_PARAM || cmd == PF_Cmd_UPDATE_PARAMS_UI) {
		checkOveruse(lData);
	} else if (cmd == PF_Cmd_RENDER || cmd == PF_Cmd_SMART_RENDER) {
		
		// if we have a licensed user
		if (lData.nofUsers > 0) {
			// check if we are not allowed to render (because of a beta or render-only license for example)
			if (lData.renderOK == false) return -1;

			// check for license overuse on render
			if (lData.overused != lastOveruseState) {
				if (lData.overused == 1) { // currently overused?
					PF_SPRINTF(out_data->return_msg, "'%s' is already being used by the maximum number of licenses allowed. You need to release a license from another machine to license this one. Until then, this instance will run in trial mode.", 
						LIC_PRODUCT_NAME);
				} else if (lData.overused == 0 && lastOveruseState == 1) { // changing from overused to ok
					PF_SPRINTF(out_data->return_msg, "'%s' has been successfully licensed again.", LIC_PRODUCT_NAME);
				}
				lastOveruseState = lData.overused;
			}
		}
	}
	return result;
}

inline int checkLicenseAEGP(AEGP_SuiteHandler* suitesP, aescriptsLicenseData& lData) {
	if (!lc_init) {
		lc_init = true;
		// initialize license data to empty fields on startup
		memset(&lData, 0, sizeof(lData));
	}
	licString errorString;		
	memset(errorString, 0, 128);
	int result = aescripts::checkLicense(suitesP, lData, errorString);
	checkOveruse(lData);
	return result;
}

static std::string getModulePath(PF_InData* in_data) {
	char mypath[1024] = { 0 };
	A_UTF16Char wPath[AEFX_MAX_PATH];
	PF_GET_PLATFORM_DATA(PF_PlatData_EXE_FILE_PATH_W, wPath);
	A_UTF16Char *wPathP = wPath;
	char *pathP = &mypath[0];
	while (*wPathP) { *pathP++ = (char)*wPathP++; }
	std::string pp = std::string(mypath);
	return pp;
}

void writeLicenseInfoToLogfile(PF_InData *in_data, int cmd) {
	if (!(cmd <= 11 || cmd == PF_Cmd_SMART_RENDER)) return;
#ifdef _WIN32
	char path[1024];
	HRESULT hr;
	hr = SHGetFolderPathA(NULL, CSIDL_DESKTOPDIRECTORY | CSIDL_FLAG_CREATE, NULL, SHGFP_TYPE_CURRENT, path);
	std::string sFile = path;
	sFile += "/" + std::string(LIC_FILENAME) + ".log";
#else
	const char* homedir = getenv("HOME");
	std::string sFile = std::string(homedir) + "/Desktop/" + std::string(LIC_FILENAME) + ".log";
#endif
	std::ofstream log;
	log.open(sFile.c_str(), std::fstream::out | std::fstream::app);

	log << std::endl << "time: " << (int)time(0) << std::endl;
	log << "command: " << cmd << std::endl;
	log << "license: " << aescripts::licenseData.license << std::endl;
	log << "product version: " << aescripts::licenseData.productVersion << std::endl;
	log << "license version: " << aescripts::licenseData.licenseVersion << std::endl;
	log << "registered: " << aescripts::licenseData.registered << std::endl;
	log << "overused: " << aescripts::licenseData.overused << std::endl;
	log << "renderOK: " << aescripts::licenseData.renderOK << std::endl;
	log << "first name: " << aescripts::licenseData.firstName << std::endl;
	log << "last name: " << aescripts::licenseData.lastName << std::endl;
	log << "users: " << aescripts::licenseData.nofUsers << std::endl;
	log << "license type: " << aescripts::licenseData.licType << std::endl;
	log << "product ID: " << aescripts::licenseData.productID << std::endl;
	log << "serial: " << aescripts::licenseData.serial << std::endl;
	log << "first start: " << aescripts::licenseData.firstStartTimestamp << std::endl;
	log << "days since first start: " << aescripts::licenseData.numberOfDaysSinceFirstStart << std::endl;
	log << "start date: " << aescripts::licenseData.startDate << std::endl;
	log << "end date: " << aescripts::licenseData.endDate << std::endl;
	log << "last result: " << aescripts::licenseData.lastResultCode << std::endl;
	log.close();
}

#endif

} // namespace aescripts

#endif // AESCRIPTSLICENSING_ADOBEHELPERS_H
