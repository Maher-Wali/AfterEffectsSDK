#include <Types.r>  // Standard macOS types (CarbonCore)

resource 'PiPL' (16000, "AEPathLink", purgeable) {
    {
        Kind { AEGP },
        Name { "AEPathLink" },
        Category { "General Plugin" },
        Version { 0x30000 },  // 3.0.0
        CodeMacIntel64 { "EntryPointFunc" },
        CodeMacARM64 { "EntryPointFunc" },
        AE_PiPL_Version { 2, 0 },
        AE_Effect_Global_OutFlags { 0x02000000 }  // Panel flag
    }
};