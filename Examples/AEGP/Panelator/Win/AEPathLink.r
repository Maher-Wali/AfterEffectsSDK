#include "AE_General.r"

resource 'PiPL' (16000) {
    {
        Name { "AEPathLink" },
        Category { "General Plugin" },
        AE_Effect_Match_Name { "MIB8" },
        AE_Effect_Version { 1 },
        AE_Effect_EntryPoint { "EntryPointFunc" }
    }
};