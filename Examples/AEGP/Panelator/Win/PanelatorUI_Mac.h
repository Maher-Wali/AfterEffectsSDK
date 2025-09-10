#ifndef PANELATORUI_MAC_H
#define PANELATORUI_MAC_H

#include "../PanelatorUI.h"
#include "AEGP_SuiteHandler.h"

class PanelatorUI_Mac : public PanelatorUI
{
public:
    PanelatorUI_Mac(SPBasicSuite* spbP,
        AEGP_PanelH panelH,
        AEGP_PlatformViewRef platformViewRef,
        AEGP_PanelFunctions1* outFunctionTable,
        AEGP_PluginID pluginID);

    virtual ~PanelatorUI_Mac();

    void InvalidateAll();
    void ExecuteGetFromAI();
    void ExecuteSendToAI();

private:
    SPBasicSuite* i_spbP;
    AEGP_PluginID i_pluginID;
    void* i_panelView; // Holds NSView* using __bridge_retained
};

#endif // PANELATORUI_MAC_H