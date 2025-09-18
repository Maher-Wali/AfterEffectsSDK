#include "AEConfig.h"
#include "AE_General.r"

resource 'PiPL' (16000, "AEPathLink", purgeable) {
    {
        Kind { AEGP },
        Name { "AEPathLink" },
        Category { "General Plugin" },
        Version { 196608 },
        CodeMacIntel64 { "EntryPointFunc" },
        CodeMacARM64 { "EntryPointFunc" },
        Creator 'PLTK',
        AE_PiPL_Version { 2, 0 },
        AE_Effect_Spec_Version { 13, 8 },
        AE_Effect_Version { 1 },
        AE_Effect_Info_Flags { 0 },
        AE_Effect_Global_OutFlags { 0 },
        AE_Effect_Global_OutFlags_2 { 0 }
    }
};