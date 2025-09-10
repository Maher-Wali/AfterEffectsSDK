// LicensingUtils.h
#pragma once

#include "config.h"
#include "AEGP_SuiteHandler.h"
#include "C:\Adobe\AfterEffectsSDK\Examples\AEGP\Panelator\aescripts-PLUGINS-licensing-framework\include\aescriptsLicensing.h" 

namespace LicensingUtils {
    void InitializeLicense(AEGP_SuiteHandler& suites);
    bool CheckLicense(AEGP_SuiteHandler& suites, AEGP_PluginID pluginID, bool showDialog = false);
    void DropLicense();
}