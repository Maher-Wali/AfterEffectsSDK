// LicensingUtils.cpp
#include "LicensingUtils.h"
#define _CRT_SECURE_NO_WARNINGS // For safe string functions
#include "AEConfig.h"
#include "AE_GeneralPlug.h"
#include "AE_Macros.h"
#include "AEGP_SuiteHandler.h"
#include "Config.h"
#include "../aescripts-PLUGINS-licensing-framework/include/aescriptsLicensing.h"
#include "../aescripts-PLUGINS-licensing-framework/include/aescriptsLicensing_AdobeHelpers.h"

namespace LicensingUtils {
    void InitializeLicense(AEGP_SuiteHandler& suites) {
        aescripts::initLicenseData(aescripts::licenseData);
        strncpy(aescripts::licenseData.productVersion, PLUGIN_VERSION, sizeof(aescripts::licenseData.productVersion) - 1);
        aescripts::checkLicenseAEGP(&suites, aescripts::licenseData);
    }

    bool CheckLicense(AEGP_SuiteHandler& suites, AEGP_PluginID pluginID, bool showDialog) {
        if (!aescripts::licenseData.registered && showDialog) {
            aescripts::showRegistrationDialog(&suites, aescripts::licenseData, true, PLUGIN_VERSION);
        }

        if (!aescripts::licenseData.registered) {
            suites.UtilitySuite6()->AEGP_ReportInfo(pluginID, "Please register PathLink to use this feature.");
            return false;
        }

        aescripts::checkOveruse(aescripts::licenseData);
        if (aescripts::licenseData.overused > 0) {
            suites.UtilitySuite6()->AEGP_ReportInfo(pluginID, "License overuse detected. Please ensure compliance.");
            return false;
        }

        return true;
    }

    void DropLicense() {
        aescripts::dropLicenseFromLicenseServer(LIC_PRODUCT_ID, LIC_PRIVATE_NUM);
    }
}