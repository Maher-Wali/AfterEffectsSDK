#include <new>
#include "Panelator.h"
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include "Parser.h"
#include "json.hpp"
#include "AE_GeneralPlugPanels.h"
#include "AEGP_SuiteHandler.h"
#include "PanelatorUI.h"
#include "Config.h"
#include "LicensingUtils.h"

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#elif defined(__APPLE__)
#include <CoreFoundation/CoreFoundation.h>
#include <Foundation/Foundation.h>
#include <Cocoa/Cocoa.h>
#include <objc/objc-runtime.h>
#endif

#ifdef _WIN32
#include "PanelatorUI_Plat.h"
#elif defined(__APPLE__)
#include "PanelatorUI_Mac.h"
#endif

using json = nlohmann::json;

// Template specializations for Panel Suite
template <>
const A_char* SuiteTraits<AEGP_PanelSuite1>::i_name = kAEGPPanelSuite;
template <>
const int32_t SuiteTraits<AEGP_PanelSuite1>::i_version = kAEGPPanelSuiteVersion1;


class Panelator {
public:
    SPBasicSuite* i_pica_basicP;
    AEGP_PluginID i_pluginID;
    AEGP_SuiteHandler i_sp;
    AEGP_Command i_command;
    AEGP_Command i_reg_command;
    SuiteHelper<AEGP_PanelSuite1> i_ps;
    const A_u_char* i_match_nameZ;

    static SPAPI A_Err S_CommandHook(
        AEGP_GlobalRefcon plugin_refconP,
        AEGP_CommandRefcon refconP,
        AEGP_Command command,
        AEGP_HookPriority hook_priority,
        A_Boolean already_handledB,
        A_Boolean* handledPB) {
        PT_XTE_START {
            reinterpret_cast<Panelator*>(refconP)->CommandHook(command, hook_priority, already_handledB, handledPB);
        } PT_XTE_CATCH_RETURN_ERR;
    }

    static A_Err S_UpdateMenuHook(
        AEGP_GlobalRefcon plugin_refconP,
        AEGP_UpdateMenuRefcon refconP,
        AEGP_WindowType active_window) {
        PT_XTE_START {
            reinterpret_cast<Panelator*>(plugin_refconP)->UpdateMenuHook(active_window);
        } PT_XTE_CATCH_RETURN_ERR;
    }

    static A_Err S_CreatePanelHook(
        AEGP_GlobalRefcon plugin_refconP,
        AEGP_CreatePanelRefcon refconP,
        AEGP_PlatformViewRef container,
        AEGP_PanelH panelH,
        AEGP_PanelFunctions1* outFunctionTable,
        AEGP_PanelRefcon* outRefcon) {
        PT_XTE_START {
            reinterpret_cast<Panelator*>(plugin_refconP)->CreatePanelHook(container, panelH, outFunctionTable, outRefcon);
        } PT_XTE_CATCH_RETURN_ERR;
    }

    Panelator(SPBasicSuite* pica_basicP, AEGP_PluginID pluginID) :
        i_pica_basicP(pica_basicP),
        i_pluginID(pluginID),
        i_sp(pica_basicP),
        i_ps(pica_basicP),
        i_match_nameZ((A_u_char*)"AEPathLink") {  // Match PiPL Name
        // Register panel in Window menu
        PT_ETX(i_sp.CommandSuite1()->AEGP_GetUniqueCommand(&i_command));
        PT_ETX(i_sp.CommandSuite1()->AEGP_InsertMenuCommand(i_command, "PathLink", AEGP_Menu_WINDOW, AEGP_MENU_INSERT_SORTED));

        PT_ETX(i_sp.RegisterSuite5()->AEGP_RegisterCommandHook(i_pluginID,
            AEGP_HP_BeforeAE,
            i_command,
            &Panelator::S_CommandHook,
            (AEGP_CommandRefcon)(this)));

        // Register menu update hook
        PT_ETX(i_sp.RegisterSuite5()->AEGP_RegisterUpdateMenuHook(i_pluginID,
            &Panelator::S_UpdateMenuHook,
            NULL));

        // Initialize licensing
        LicensingUtils::InitializeLicense(i_sp);

        // Register registration command and menu item
        PT_ETX(i_sp.CommandSuite1()->AEGP_GetUniqueCommand(&i_reg_command));
        PT_ETX(i_sp.CommandSuite1()->AEGP_InsertMenuCommand(i_reg_command, "Register PathLink", AEGP_Menu_WINDOW, AEGP_MENU_INSERT_SORTED));

        // Register hook for registration command (once)
        PT_ETX(i_sp.RegisterSuite5()->AEGP_RegisterCommandHook(i_pluginID,
            AEGP_HP_BeforeAE,
            i_reg_command,
            &Panelator::S_CommandHook,
            (AEGP_CommandRefcon)(this)));

        // Register panel creation hook
        PT_ETX(i_ps->AEGP_RegisterCreatePanelHook(i_pluginID, i_match_nameZ,
            &Panelator::S_CreatePanelHook,
            (AEGP_CreatePanelRefcon)this,
            true));
    }

    void CommandHook(
        AEGP_Command command,
        AEGP_HookPriority hook_priority,
        A_Boolean already_handledB,
        A_Boolean* handledPB) {
        if (command == i_command) {
            PT_ETX(i_ps->AEGP_ToggleVisibility(i_match_nameZ));
            *handledPB = TRUE;
        }
        if (command == i_reg_command) {
            LicensingUtils::CheckLicense(i_sp, i_pluginID, true);
            *handledPB = TRUE;
        }
    }

    void UpdateMenuHook(AEGP_WindowType active_window) {
        PT_ETX(i_sp.CommandSuite1()->AEGP_EnableCommand(i_command));
        A_Boolean out_thumb_is_shownB = FALSE, out_panel_is_frontmostB = FALSE;
        PT_ETX(i_ps->AEGP_IsShown(i_match_nameZ, &out_thumb_is_shownB, &out_panel_is_frontmostB));
        PT_ETX(i_sp.CommandSuite1()->AEGP_CheckMarkMenuCommand(i_command, out_thumb_is_shownB && out_panel_is_frontmostB));
        PT_ETX(i_sp.CommandSuite1()->AEGP_EnableCommand(i_reg_command));
    }

    void CreatePanelHook(
        AEGP_PlatformViewRef container,
        AEGP_PanelH panelH,
        AEGP_PanelFunctions1* outFunctionTable,
        AEGP_PanelRefcon* outRefcon) {
#ifdef _WIN32
        *outRefcon = reinterpret_cast<AEGP_PanelRefcon>(
            new PanelatorUI_Plat(i_pica_basicP, panelH, container, outFunctionTable, i_pluginID));
#elif defined(__APPLE__)
        *outRefcon = reinterpret_cast<AEGP_PanelRefcon>(
            new PanelatorUI_Mac(i_pica_basicP, panelH, container, outFunctionTable, i_pluginID));
#endif
    }

    ~Panelator() {
        aescripts::dropLicenseFromLicenseServer(LIC_PRODUCT_ID, LIC_PRIVATE_NUM);
    }
};

#ifdef __cplusplus
extern "C" {
#endif

A_Err EntryPointFunc(
    struct SPBasicSuite* pica_basicP,
    A_long major_versionL,
    A_long minor_versionL,
    AEGP_PluginID aegp_plugin_id,
    AEGP_GlobalRefcon* global_refconP) {
    PT_XTE_START {
        *global_refconP = (AEGP_GlobalRefcon) new Panelator(pica_basicP, aegp_plugin_id);
    } PT_XTE_CATCH_RETURN_ERR;
}

#ifdef __cplusplus
}
#endif