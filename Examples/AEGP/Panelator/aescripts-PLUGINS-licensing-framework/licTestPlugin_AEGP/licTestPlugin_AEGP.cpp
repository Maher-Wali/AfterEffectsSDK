#include "licTestPlugin_AEGP.h"

#define LIC_PRIVATE_NUM 11111111
#define LIC_PRODUCT_NAME "licTestPlugin_AEGP"
#define LIC_PRODUCT_ID "AETP"
#define LIC_FILENAME "licTestPlugin_AEGP"

#include "../include/aescriptsLicensing.h"
#include "../include/aescriptsLicensing_AdobeHelpers.h"

static AEGP_Command S_AEGP_CMD = 0L;
static AEGP_PluginID S_AEGP_ID = 0L;
static SPBasicSuite* sP = NULL;

static A_Err CommandHook(AEGP_GlobalRefcon plugin_refconPV, AEGP_CommandRefcon refconPV, AEGP_Command command, AEGP_HookPriority hook_priority, A_Boolean already_handledB, A_Boolean* handledPB) {
	AEGP_SuiteHandler suites(sP);
	if (S_AEGP_CMD == command) aescripts::showRegistrationDialog(&suites, aescripts::licenseData);
	return A_Err_NONE;
}

static A_Err UpdateMenuHook(AEGP_GlobalRefcon plugin_refconP, AEGP_UpdateMenuRefcon refconP, AEGP_WindowType active_window) {
	AEGP_SuiteHandler suites(sP);
	suites.CommandSuite1()->AEGP_EnableCommand(S_AEGP_CMD);
	return A_Err_NONE;
}

A_Err EntryPointFunc(struct SPBasicSuite* pica_basicP, A_long major_versionL, A_long minor_versionL, AEGP_PluginID aegp_plugin_id, AEGP_GlobalRefcon* global_refconV) {
	S_AEGP_ID = aegp_plugin_id;
	A_Err err = A_Err_NONE;
	sP = pica_basicP;
	AEGP_SuiteHandler suites(sP);
	ERR(suites.CommandSuite1()->AEGP_GetUniqueCommand(&S_AEGP_CMD));
	ERR(suites.CommandSuite1()->AEGP_InsertMenuCommand(S_AEGP_CMD, "licTestPlugin_AEGP", AEGP_Menu_FILE, AEGP_MENU_INSERT_AT_BOTTOM));
	ERR(suites.RegisterSuite5()->AEGP_RegisterCommandHook(S_AEGP_ID, AEGP_HP_BeforeAE, AEGP_Command_ALL, CommandHook, 0));
	ERR(suites.RegisterSuite5()->AEGP_RegisterUpdateMenuHook(S_AEGP_ID, UpdateMenuHook, NULL));
	return err;
}

