#include "../../../Resources/AE_General.r"

// AEPathLink macOS PiPL
resource 'PiPL' (16000)
{
    // Plugin name shown in AE menus
    name = "AEPathLink";

    // Category of the plugin
    category = "General Plugin";

    // Unique 4-character plugin ID (from Windows rc dnik)
    id = 'dnik';

    // Entry point function exported from the plugin
    entry_point = "EntryPointFunc";

    // Plugin version
    version = 1;
};
