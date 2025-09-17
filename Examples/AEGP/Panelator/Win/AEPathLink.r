#include "AE_General.r"

resource 'PiPL' (16000)
{
    {
        Name        { "AEPathLink" },          // Display name
        category    { "General Plugin" },      // Menu category
        MfgID       { 'dnik' },               // 4-char manufacturer ID
        TypeID      { 'xgEA' },               // 4-char plugin type ID (unique per plugin)
        EntryPoint  { "EntryPointFunc" },     // Function exported by plugin
        Version     { 1 }                      // Plugin version
    }
};