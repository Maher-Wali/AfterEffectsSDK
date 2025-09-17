#include "AE_General.r"

resource 'PiPL' (16000)
{
    {
        Name { "AEPathLink" },
        category { "General Plugin" },
        id { 'dnik' },            // unique plugin ID
        entry_point { "EntryPointFunc" },
        version { 1 }
    }
};