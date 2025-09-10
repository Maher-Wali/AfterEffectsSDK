#ifndef PARSER_H
#define PARSER_H

#include "AEConfig.h"
#include "entry.h"
#include "AE_Effect.h"
#include "AE_Macros.h"
#include "Param_Utils.h"
#include "AE_GeneralPlug.h"
#include "AEGP_SuiteHandler.h"
#include "Smart_Utils.h"
#include "A.h"
#include "AE_EffectCB.h"
#include "AE_EffectCBSuites.h"
#include "AE_EffectUI.h"
#include "AE_EffectSuites.h"
#include "AE_EffectPixelFormat.h"
#include <vector>
#include <string>


namespace Parser {
    // Structures to hold extracted data
    struct Point {
        struct Vector2 {
            float x, y;
        };

        Vector2 anchor;
        bool hasLeftHandle;
        bool hasRightHandle;
        Vector2 leftHandle;
        Vector2 rightHandle;
    };

    struct Fill {
        std::string color;  // Hex color string
        float opacity;
        bool enabled;
    };

    struct Stroke {
        std::string color;  // Hex color string
        float width;
        float opacity;
        bool enabled;
    };

    struct Transform {
        float tx, ty;       // Translation
        float rotation;     // Rotation in degrees
        float scaleX, scaleY; // Scale factors
    };

    struct ShapeObject {
        bool hasPath;
        std::vector<Point> points;
        bool isClosed;
        Fill fill;
        Stroke stroke;
        Transform transform;
        std::string layerName;
        int layerIndex;
    };

    struct ExportData {
        bool hasAnyObjects;
        std::vector<ShapeObject> objects;
    };

    std::string checkSelection(AEGP_SuiteHandler& suiteHandler, AEGP_PluginID pluginID);
    std::string getSelectedPathsFillColor(AEGP_SuiteHandler& suiteHandler, AEGP_PluginID pluginID);
    std::string ColorToHex(const AEGP_ColorVal& color);
    
}

#endif // PARSER_H