#ifndef _H_PANELATOR
#define _H_PANELATOR

#include "AEConfig.h"
#ifdef AE_OS_WIN
#include <windows.h>
#endif
#include "entry.h"
#include "AE_GeneralPlug.h"
#include "AE_GeneralPlugPanels.h"
#include "AE_Macros.h"
#include "AEGP_SuiteHandler.h"
#include "SuiteHelper.h"
#include "String_Utils.h"
#include "AE_EffectSuites.h"
#include "PT_Err.h"
#include <string>

#ifdef WIN32
#include "PanelatorUI_Plat.h"
#elif defined(__APPLE__)

#endif

// Forward declarations
class PanelatorUI;

typedef enum {
    StrID_NONE,
    StrID_Name,
    StrID_Description,
    StrID_GenericError,
    StrID_NUMTYPES
} StrIDType;

// This entry point is exported through the PiPL (.r file)
extern "C" DllExport AEGP_PluginInitFuncPrototype EntryPointFunc;

#endif