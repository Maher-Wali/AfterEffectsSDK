#include "PanelatorUI_Mac.h"
#include <Cocoa/Cocoa.h>
#include <objc/objc-runtime.h>
#include <string>
#include "AEConfig.h"
#include "AE_GeneralPlug.h"
#include "AE_Macros.h"
#include "AEGP_SuiteHandler.h"
#include "Config.h"
#include "LicensingUtils.h"

// Button tags for identification
const int BtnGetFromAI = 1001;
const int BtnSendToAI = 1002;

// Objective-C class interface for the panel view
@interface PanelatorView : NSView {
    PanelatorUI_Mac* cppInstance;
}
- (instancetype)initWithFrame:(NSRect)frameRect cppInstance:(PanelatorUI_Mac*)instance;
- (void)getButtonClicked:(id)sender;
- (void)sendButtonClicked:(id)sender;
@end

@implementation PanelatorView

- (instancetype)initWithFrame:(NSRect)frameRect cppInstance:(PanelatorUI_Mac*)instance {
    self = [super initWithFrame:frameRect];
    if (self) {
        cppInstance = instance;
        [self setupUI];
    }
    return self;
}

- (void)setupUI {
    // Create Get button
    NSButton* getButton = [[NSButton alloc] initWithFrame:NSMakeRect(10, 10, 80, 30)];
    [getButton setButtonType:NSButtonTypeMomentaryPushIn]; // standard button
    [getButton setBezelStyle:NSBezelStyleRounded];         // rounded bezel
    [getButton setTitle:@"Get"];                           // regular title
    [getButton setFont:[NSFont systemFontOfSize:12]];      // text size
    [getButton setWantsLayer:YES];
    getButton.layer.backgroundColor = [NSColor colorWithRed:0.3 green:0.3 blue:0.3 alpha:1.0].CGColor; // gray
    [getButton setAttributedTitle:[[NSAttributedString alloc] 
        initWithString:@"Get" 
        attributes:@{NSForegroundColorAttributeName: [NSColor whiteColor]}]];
    
    [getButton setTarget:self];
    [getButton setAction:@selector(getButtonClicked:)];
    
    // Create Send button
    NSButton* sendButton = [[NSButton alloc] initWithFrame:NSMakeRect(100, 10, 80, 30)];
    [sendButton setTitle:@"Send"];
    [sendButton setButtonType:(NSButtonType)0];
    [sendButton setTarget:self];
    [sendButton setAction:@selector(sendButtonClicked:)];
    [sendButton setTag:BtnSendToAI];
    
    // Style the Send button
    [sendButton setWantsLayer:YES];
    sendButton.layer.backgroundColor = [NSColor colorWithRed:0.35 green:0.35 blue:0.35 alpha:1.0].CGColor;
    sendButton.layer.cornerRadius = 4.0;
    [sendButton setAttributedTitle:[[NSAttributedString alloc] 
        initWithString:@"Send" 
        attributes:@{
            NSForegroundColorAttributeName: [NSColor whiteColor],
            NSFontAttributeName: [NSFont systemFontOfSize:12]
        }]];
    
    // Add buttons to view
    [self addSubview:getButton];
    [self addSubview:sendButton];
    
    // Set background color to match AE panel
    [self setWantsLayer:YES];
    self.layer.backgroundColor = [NSColor colorWithRed:0.2 green:0.2 blue:0.2 alpha:1.0].CGColor;
}

- (void)getButtonClicked:(id)sender {
    if (cppInstance) {
        cppInstance->ExecuteGetFromAI();
    }
}

- (void)sendButtonClicked:(id)sender {
    if (cppInstance) {
        cppInstance->ExecuteSendToAI();
    }
}

- (void)drawRect:(NSRect)dirtyRect {
    [super drawRect:dirtyRect];
    
    // Fill background with panel color
    [[NSColor colorWithRed:0.2 green:0.2 blue:0.2 alpha:1.0] setFill];
    NSRectFill(dirtyRect);
}

- (BOOL)isFlipped {
    return YES; // Use top-left coordinate system like Windows
}

@end

PanelatorUI_Mac::PanelatorUI_Mac(SPBasicSuite* spbP, AEGP_PanelH panelH,
    AEGP_PlatformViewRef platformViewRef,
    AEGP_PanelFunctions1* outFunctionTable,
    AEGP_PluginID pluginID)
    : PanelatorUI(spbP, panelH, platformViewRef, outFunctionTable),
    i_spbP(spbP),
    i_pluginID(pluginID),
    i_panelView(nullptr)
{
    // Create our custom view
    NSView* containerView = platformViewRef;
    NSRect frame = [containerView bounds];
    
    PanelatorView* panelView = [[PanelatorView alloc] initWithFrame:frame cppInstance:this];
    i_panelView = (__bridge_retained void*)panelView;
    
    [containerView addSubview:panelView];
    
    // Set autoresizing mask to fill container
    [panelView setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];
}

PanelatorUI_Mac::~PanelatorUI_Mac() {
    if (i_panelView) {
        PanelatorView* panelView = (__bridge_transfer PanelatorView*)i_panelView;
        [panelView removeFromSuperview];
        i_panelView = nullptr;
    }
}

void PanelatorUI_Mac::InvalidateAll() {
    if (i_panelView) {
        PanelatorView* panelView = (__bridge PanelatorView*)i_panelView;
        [panelView setNeedsDisplay:YES];
    }
}

void PanelatorUI_Mac::ExecuteGetFromAI() {
    AEGP_SuiteHandler suites(i_spbP);
    if (!LicensingUtils::CheckLicense(suites, i_pluginID, true)) {
        return;
    }
    
    std::string jsxScript;
    jsxScript += "(function() {\n";
    jsxScript += "  var jsonFile = new File(Folder.temp.fsName + '/ai2ae.json');\n";
    jsxScript += "  if (!jsonFile.exists) {\n";
    jsxScript += "    alert('JSON file not found: ' + jsonFile.fsName);\n";
    jsxScript += "    return;\n";
    jsxScript += "  }\n";
    jsxScript += "  if (!jsonFile.open('r')) {\n";
    jsxScript += "    alert('Failed to open JSON file: ' + jsonFile.error);\n";
    jsxScript += "    return;\n";
    jsxScript += "  }\n";
    jsxScript += "  var jsonString = jsonFile.read();\n";
    jsxScript += "  jsonFile.close();\n";
    jsxScript += "  var data;\n";
    jsxScript += "  try {\n";
    jsxScript += "    data = eval('(' + jsonString + ')');\n";
    jsxScript += "  } catch (e) {\n";
    jsxScript += "    alert('Error parsing JSON: ' + e.toString());\n";
    jsxScript += "    return;\n";
    jsxScript += "  }\n";
    jsxScript += "  var comp = app.project.activeItem;\n";
    jsxScript += "  if (!comp || !(comp instanceof CompItem)) {\n";
    jsxScript += "    alert('Please select or open a composition first.');\n";
    jsxScript += "    return;\n";
    jsxScript += "  }\n";
    jsxScript += "  if (!data.hasAnyObjects) {\n";
    jsxScript += "    alert('No valid object data found in JSON.');\n";
    jsxScript += "    return;\n";
    jsxScript += "  }\n";
    jsxScript += "  function hexToRgb(hex) {\n";
    jsxScript += "    if (!hex) return [0, 0, 0];\n";
    jsxScript += "    hex = hex.replace(/^#/, '');\n";
    jsxScript += "    if (hex.length === 3) {\n";
    jsxScript += "      hex = hex.split('').map(function(h) { return h + h; }).join('');\n";
    jsxScript += "    }\n";
    jsxScript += "    var bigint = parseInt(hex, 16);\n";
    jsxScript += "    var r = ((bigint >> 16) & 255) / 255;\n";
    jsxScript += "    var g = ((bigint >> 8) & 255) / 255;\n";
    jsxScript += "    var b = (bigint & 255) / 255;\n";
    jsxScript += "    return [r, g, b];\n";
    jsxScript += "  }\n";
    jsxScript += "  function findClosestLabelColor(hexColor) {\n";
    jsxScript += "    var labelColors = [\n";
    jsxScript += "      {index: 0, name: 'None', rgb: [0, 0, 0]},\n";
    jsxScript += "      {index: 1, name: 'Red', rgb: [1, 0.2, 0.2]},\n";
    jsxScript += "      {index: 2, name: 'Yellow', rgb: [1, 1, 0.2]},\n";
    jsxScript += "      {index: 3, name: 'Aqua', rgb: [0.2, 1, 1]},\n";
    jsxScript += "      {index: 4, name: 'Pink', rgb: [1, 0.4, 1]},\n";
    jsxScript += "      {index: 5, name: 'Lavender', rgb: [0.6, 0.4, 1]},\n";
    jsxScript += "      {index: 6, name: 'Peach', rgb: [1, 0.6, 0.4]},\n";
    jsxScript += "      {index: 7, name: 'Sea Foam', rgb: [0.4, 1, 0.6]},\n";
    jsxScript += "      {index: 8, name: 'Blue', rgb: [0.2, 0.6, 1]},\n";
    jsxScript += "      {index: 9, name: 'Green', rgb: [0.4, 1, 0.4]},\n";
    jsxScript += "      {index: 10, name: 'Purple', rgb: [0.6, 0.2, 1]},\n";
    jsxScript += "      {index: 11, name: 'Orange', rgb: [1, 0.6, 0.2]},\n";
    jsxScript += "      {index: 12, name: 'Brown', rgb: [0.6, 0.4, 0.2]},\n";
    jsxScript += "      {index: 13, name: 'Fuchsia', rgb: [1, 0.2, 0.6]},\n";
    jsxScript += "      {index: 14, name: 'Cyan', rgb: [0.2, 1, 0.8]},\n";
    jsxScript += "      {index: 15, name: 'Sandstone', rgb: [0.8, 0.6, 0.4]},\n";
    jsxScript += "      {index: 16, name: 'Dark Green', rgb: [0.2, 0.6, 0.2]}\n";
    jsxScript += "    ];\n";
    jsxScript += "    var inputRgb = hexToRgb(hexColor);\n";
    jsxScript += "    var minDistance = Infinity;\n";
    jsxScript += "    var closestIndex = 0;\n";
    jsxScript += "    for (var i = 0; i < labelColors.length; i++) {\n";
    jsxScript += "      var labelRgb = labelColors[i].rgb;\n";
    jsxScript += "      var distance = Math.sqrt(\n";
    jsxScript += "        Math.pow(inputRgb[0] - labelRgb[0], 2) +\n";
    jsxScript += "        Math.pow(inputRgb[1] - labelRgb[1], 2) +\n";
    jsxScript += "        Math.pow(inputRgb[2] - labelRgb[2], 2)\n";
    jsxScript += "      );\n";
    jsxScript += "      if (distance < minDistance) {\n";
    jsxScript += "        minDistance = distance;\n";
    jsxScript += "        closestIndex = i;\n";
    jsxScript += "      }\n";
    jsxScript += "    }\n";
    jsxScript += "    return closestIndex;\n";
    jsxScript += "  }\n";
    jsxScript += "  function getSelectedLayers() {\n";
    jsxScript += "    var selectedLayers = [];\n";
    jsxScript += "    for (var i = 1; i <= comp.numLayers; i++) {\n";
    jsxScript += "      if (comp.layer(i).selected) {\n";
    jsxScript += "        selectedLayers.push(comp.layer(i));\n";
    jsxScript += "      }\n";
    jsxScript += "    }\n";
    jsxScript += "    return selectedLayers;\n";
    jsxScript += "  }\n";
    jsxScript += "  var gradientData = [];\n";
    jsxScript += "  function simpleJSONStringify(obj, indent) {\n";
    jsxScript += "    indent = indent || 0;\n";
    jsxScript += "    var spaces = '';\n";
    jsxScript += "    for (var i = 0; i < indent; i++) spaces += '  ';\n";
    jsxScript += "    if (obj === null) return 'null';\n";
    jsxScript += "    if (typeof obj === 'string') return '\"' + obj.replace(/\"/g, '\\\\\"') + '\"';\n";
    jsxScript += "    if (typeof obj === 'number' || typeof obj === 'boolean') return obj.toString();\n";
    jsxScript += "    if (obj instanceof Array) {\n";
    jsxScript += "      var result = '[\\n';\n";
    jsxScript += "      for (var i = 0; i < obj.length; i++) {\n";
    jsxScript += "        result += spaces + '  ' + simpleJSONStringify(obj[i], indent + 1);\n";
    jsxScript += "        if (i < obj.length - 1) result += ',';\n";
    jsxScript += "        result += '\\n';\n";
    jsxScript += "      }\n";
    jsxScript += "      result += spaces + ']';\n";
    jsxScript += "      return result;\n";
    jsxScript += "    }\n";
    jsxScript += "    if (typeof obj === 'object') {\n";
    jsxScript += "      var result = '{\\n';\n";
    jsxScript += "      var keys = [];\n";
    jsxScript += "      for (var key in obj) {\n";
    jsxScript += "        if (obj.hasOwnProperty(key)) keys.push(key);\n";
    jsxScript += "      }\n";
    jsxScript += "      for (var i = 0; i < keys.length; i++) {\n";
    jsxScript += "        var key = keys[i];\n";
    jsxScript += "        result += spaces + '  \"' + key + '\": ' + simpleJSONStringify(obj[key], indent + 1);\n";
    jsxScript += "        if (i < keys.length - 1) result += ',';\n";
    jsxScript += "        result += '\\n';\n";
    jsxScript += "      }\n";
    jsxScript += "      result += spaces + '}';\n";
    jsxScript += "      return result;\n";
    jsxScript += "    }\n";
    jsxScript += "    return 'undefined';\n";
    jsxScript += "  }\n";
    jsxScript += "  function simpleJSONStringifyGradient(obj) {\n";
    jsxScript += "      var str = '{';\n";
    jsxScript += "      var parts = [];\n";
    jsxScript += "      for (var key in obj) {\n";
    jsxScript += "          if (key === 'stops') {\n";
    jsxScript += "              var stopsArray = obj.stops;\n";
    jsxScript += "              var stopsStr = '[';\n";
    jsxScript += "              if (stopsArray && typeof stopsArray === 'object' && stopsArray.length !== undefined) {\n";
    jsxScript += "                  var stopStrings = [];\n";
    jsxScript += "                  for (var i = 0; i < stopsArray.length; i++) {\n";
    jsxScript += "                      var stop = stopsArray[i];\n";
    jsxScript += "                      if (stop && stop.color !== undefined && stop.position !== undefined) {\n";
    jsxScript += "                          stopStrings.push('{' +\n";
    jsxScript += "                              \"\\\"color\\\":\\\"\" + stop.color + \"\\\",\" +\n";
    jsxScript += "                              \"\\\"position\\\":\" + stop.position +\n";
    jsxScript += "                          '}');\n";
    jsxScript += "                      }\n";
    jsxScript += "                  }\n";
    jsxScript += "                  stopsStr += stopStrings.join(',');\n";
    jsxScript += "              }\n";
    jsxScript += "              stopsStr += ']';\n";
    jsxScript += "              parts.push('\"' + key + '\":' + stopsStr);\n";
    jsxScript += "          } else {\n";
    jsxScript += "              parts.push('\"' + key + '\":\"' + obj[key] + '\"');\n";
    jsxScript += "          }\n";
    jsxScript += "      }\n";
    jsxScript += "      str += parts.join(',') + '}';\n";
    jsxScript += "      return str;\n";
    jsxScript += "  }\n";
    jsxScript += "  var pathCount = 0;\n";
    jsxScript += "  var textCount = 0;\n";
    jsxScript += "  var selectedLayers = getSelectedLayers();\n";
    jsxScript += "  var selectedLength = selectedLayers.length;\n";
    jsxScript += "  if (data.layers && data.layers.length > 0) {\n";
    jsxScript += "    var sortedLayers = data.layers.slice();\n";
    jsxScript += "    sortedLayers.sort(function(a, b) {\n";
    jsxScript += "      var zIndexA = a.layerZIndex !== undefined ? a.layerZIndex : 0;\n";
    jsxScript += "      var zIndexB = b.layerZIndex !== undefined ? b.layerZIndex : 0;\n";
    jsxScript += "      return zIndexA - zIndexB;\n";
    jsxScript += "    });\n";
    jsxScript += "    if (selectedLength === 0) {\n";
    jsxScript += "      for (var i = 0; i < sortedLayers.length; i++) {\n";
    jsxScript += "        var layer = sortedLayers[i];\n";
    jsxScript += "        if (layer.paths && layer.paths.length > 0) {\n";
    jsxScript += "          var shapeLayer = comp.layers.addShape();\n";
    jsxScript += "          shapeLayer.name = layer.layerName || ('Layer ' + (i + 1));\n";
    jsxScript += "          if (layer.layerColor) {\n";
    jsxScript += "            var labelIndex = findClosestLabelColor(layer.layerColor);\n";
    jsxScript += "            shapeLayer.label = labelIndex;\n";
    jsxScript += "          }\n";
    jsxScript += "          var contents = shapeLayer.property('ADBE Root Vectors Group');\n";
    jsxScript += "          for (var j = 0; j < layer.paths.length; j++) {\n";
    jsxScript += "            var object = layer.paths[j];\n";
    jsxScript += "            if (object.type === 'path' && object.hasPath) {\n";
    jsxScript += "              var shapeGroup = contents.addProperty('ADBE Vector Group');\n";
    jsxScript += "              shapeGroup.name = 'Shape ' + (j+1);\n";
    jsxScript += "              var contentsGroup = shapeGroup.property('ADBE Vectors Group');\n";
    jsxScript += "              var pathGroup = contentsGroup.addProperty('ADBE Vector Shape - Group');\n";
    jsxScript += "              var pathProperty = pathGroup.property('ADBE Vector Shape');\n";
    jsxScript += "              var shape = new Shape();\n";
    jsxScript += "              var vertices = [];\n";
    jsxScript += "              var inTangents = [];\n";
    jsxScript += "              var outTangents = [];\n";
    jsxScript += "              for (var k = 0; k < object.points.length; k++) {\n";
    jsxScript += "                var point = object.points[k];\n";
    jsxScript += "                vertices.push([point.anchor.x, -point.anchor.y]);\n";
    jsxScript += "                if (point.hasLeftHandle) {\n";
    jsxScript += "                  inTangents.push([point.leftHandle.x - point.anchor.x, -(point.leftHandle.y - point.anchor.y)]);\n";
    jsxScript += "                } else {\n";
    jsxScript += "                  inTangents.push([0, 0]);\n";
    jsxScript += "                }\n";
    jsxScript += "                if (point.hasRightHandle) {\n";
    jsxScript += "                  outTangents.push([point.rightHandle.x - point.anchor.x, -(point.rightHandle.y - point.anchor.y)]);\n";
    jsxScript += "                } else {\n";
    jsxScript += "                  outTangents.push([0, 0]);\n";
    jsxScript += "                }\n";
    jsxScript += "              }\n";
    jsxScript += "              shape.vertices = vertices;\n";
    jsxScript += "              shape.inTangents = inTangents;\n";
    jsxScript += "              shape.outTangents = outTangents;\n";
    jsxScript += "              shape.closed = object.isClosed;\n";
    jsxScript += "              pathProperty.setValue(shape);\n";
    jsxScript += "              if (object.fill) {\n";
    jsxScript += "                if (object.fill.fillType === 'linear_gradient' || object.fill.fillType === 'radial_gradient') {\n";
    jsxScript += "                  var fillGradient = contentsGroup.addProperty('ADBE Vector Graphic - G-Fill');\n";
    jsxScript += "                  fillGradient.name = 'Gradient Fill ' + (j+1);\n";
    jsxScript += "                  shapeLayer.name += j === 0 ? '[GRADIENT]' : '';\n";
    jsxScript += "                  var gradientStops = (object.fill.gradient && object.fill.gradient.stops) ? object.fill.gradient.stops : [];\n";
    jsxScript += "                  var gradientInfo = {\n";
    jsxScript += "                      layerIndex: shapeLayer.index,\n";
    jsxScript += "                      layerName: shapeLayer.name,\n";
    jsxScript += "                      fillType: object.fill.fillType,\n";
    jsxScript += "                      gradient: {\n";
    jsxScript += "                          type: object.fill.fillType,\n";
    jsxScript += "                          angle: object.fill.gradient ? (object.fill.gradient.angle || 0) : 0,\n";
    jsxScript += "                          startPoint: object.fill.gradient && object.fill.gradient.startPoint ? \n";
    jsxScript += "                              {\n";
    jsxScript += "                                  x: object.fill.gradient.startPoint.x || 0,\n";
    jsxScript += "                                  y: object.fill.gradient.startPoint.y || 0\n";
    jsxScript += "                              } : {x: 0, y: 0},\n";
    jsxScript += "                          endPoint: object.fill.gradient && object.fill.gradient.endPoint ?\n";
    jsxScript += "                              {\n";
    jsxScript += "                                  x: object.fill.gradient.endPoint.x || 100,\n";
    jsxScript += "                                  y: object.fill.gradient.endPoint.y || 0\n";
    jsxScript += "                              } : {x: 100, y: 0},\n";
    jsxScript += "                          aspectRatio: object.fill.gradient ? (object.fill.gradient.aspectRatio || 1.0) : 1.0,\n";
    jsxScript += "                          stops: []\n";
    jsxScript += "                      }\n";
    jsxScript += "                  };\n";
    jsxScript += "                  if (object.fill.gradient && object.fill.gradient.stops) {\n";
    jsxScript += "                      for (var s = 0; s < object.fill.gradient.stops.length; s++) {\n";
    jsxScript += "                          var stop = object.fill.gradient.stops[s];\n";
    jsxScript += "                          if (stop) {\n";
    jsxScript += "                              gradientInfo.gradient.stops.push({\n";
    jsxScript += "                                  color: stop.color || '#000000',\n";
    jsxScript += "                                  position: stop.position !== undefined ? stop.position : 0,\n";
    jsxScript += "                                  opacity: stop.opacity !== undefined ? stop.opacity : 1.0\n";
    jsxScript += "                              });\n";
    jsxScript += "                          }\n";
    jsxScript += "                      }\n";
    jsxScript += "                      gradientInfo.gradient.stops.sort(function(a, b) {\n";
    jsxScript += "                          return a.position - b.position;\n";
    jsxScript += "                      });\n";
    jsxScript += "                      gradientData.push(gradientInfo);\n";
    jsxScript += "                  }\n";
    jsxScript += "                } else if (object.fill.color) {\n";
    jsxScript += "                  var fill = contentsGroup.addProperty('ADBE Vector Graphic - Fill');\n";
    jsxScript += "                  if (!fill) {\n";
    jsxScript += "                      alert('fill is null');\n";
    jsxScript += "                  }\n";
    jsxScript += "                  var fillColor = hexToRgb(object.fill.color);\n";
    jsxScript += "                  fill.property('ADBE Vector Fill Color').setValue(fillColor);\n";
    jsxScript += "                }\n";
    jsxScript += "              }\n";
    jsxScript += "              if (object.stroke && object.stroke.color) {\n";
    jsxScript += "                var stroke = contentsGroup.addProperty('ADBE Vector Graphic - Stroke');\n";
    jsxScript += "                var strokeColor = hexToRgb(object.stroke.color);\n";
    jsxScript += "                stroke.property('ADBE Vector Stroke Color').setValue(strokeColor);\n";
    jsxScript += "                if (object.stroke.width) {\n";
    jsxScript += "                  stroke.property('ADBE Vector Stroke Width').setValue(object.stroke.width);\n";
    jsxScript += "                }\n";
    jsxScript += "                if (object.stroke.cap !== undefined) {\n";
    jsxScript += "                  var lineCapMap = {0: 1, 1: 2, 2: 3}; // 0: Butt, 1: Round, 2: Square\n";
    jsxScript += "                  var lineCap = lineCapMap[object.stroke.cap] || 1;\n";
    jsxScript += "                  stroke.property('ADBE Vector Stroke Line Cap').setValue(lineCap);\n";
    jsxScript += "                }\n";
    jsxScript += "                if (object.stroke.join !== undefined) {\n";
    jsxScript += "                  var lineJoinMap = {0: 1, 1: 2, 2: 3}; // 0: Miter, 1: Round, 2: Bevel\n";
    jsxScript += "                  var lineJoin = lineJoinMap[object.stroke.join] || 1;\n";
    jsxScript += "                  stroke.property('ADBE Vector Stroke Line Join').setValue(lineJoin);\n";
    jsxScript += "                }\n";
    jsxScript += "              }\n";
    jsxScript += "              if (object.opacity !== undefined && object.opacity !== null) {\n";
    jsxScript += "                var shapeTransform = shapeGroup.property('ADBE Vector Transform Group');\n";
    jsxScript += "                var shapeOpacity = shapeTransform.property('ADBE Vector Group Opacity');\n";
    jsxScript += "                shapeOpacity.setValue(object.opacity * 100);\n";
    jsxScript += "              }\n";
    jsxScript += "              if (object.transform) {\n";
    jsxScript += "                var shapeTransform = shapeGroup.property('ADBE Vector Transform Group');\n";
    jsxScript += "                var groupPosition = shapeTransform.property('ADBE Vector Position');\n";
    jsxScript += "                groupPosition.setValue([object.transform.tx, object.transform.ty]);\n";
    jsxScript += "              }\n";
    jsxScript += "              pathCount++;\n";
    jsxScript += "            }\n";
    jsxScript += "          }\n";
    jsxScript += "        }\n";
    jsxScript += "      }\n";
    jsxScript += "    } else {\n";
    jsxScript += "      for (var sl = 0; sl < selectedLength; sl++) {\n";
    jsxScript += "        var shapeLayer = selectedLayers[sl];\n";
    jsxScript += "        if (shapeLayer('ADBE Root Vectors Group') === null) {\n";
    jsxScript += "          alert('Selected layer \"' + shapeLayer.name + '\" is not a shape layer. Skipping.');\n";
    jsxScript += "          continue;\n";
    jsxScript += "        }\n";
    jsxScript += "        var contents = shapeLayer.property('ADBE Root Vectors Group');\n";
    jsxScript += "        for (var i = 0; i < sortedLayers.length; i++) {\n";
    jsxScript += "          var layer = sortedLayers[i];\n";
    jsxScript += "          if (layer.paths && layer.paths.length > 0) {\n";
    jsxScript += "            for (var j = 0; j < layer.paths.length; j++) {\n";
    jsxScript += "              var object = layer.paths[j];\n";
    jsxScript += "              if (object.type === 'path' && object.hasPath) {\n";
    jsxScript += "                var shapeGroup = contents.addProperty('ADBE Vector Group');\n";
    jsxScript += "                shapeGroup.name = (layer.layerName || 'Layer ' + (i + 1)) + ' Shape ' + (j+1);\n";
    jsxScript += "                var contentsGroup = shapeGroup.property('ADBE Vectors Group');\n";
    jsxScript += "                var pathGroup = contentsGroup.addProperty('ADBE Vector Shape - Group');\n";
    jsxScript += "                var pathProperty = pathGroup.property('ADBE Vector Shape');\n";
    jsxScript += "                var shape = new Shape();\n";
    jsxScript += "                var vertices = [];\n";
    jsxScript += "                var inTangents = [];\n";
    jsxScript += "                var outTangents = [];\n";
    jsxScript += "                for (var k = 0; k < object.points.length; k++) {\n";
    jsxScript += "                  var point = object.points[k];\n";
    jsxScript += "                  vertices.push([point.anchor.x, -point.anchor.y]);\n";
    jsxScript += "                  if (point.hasLeftHandle) {\n";
    jsxScript += "                    inTangents.push([point.leftHandle.x - point.anchor.x, -(point.leftHandle.y - point.anchor.y)]);\n";
    jsxScript += "                  } else {\n";
    jsxScript += "                    inTangents.push([0, 0]);\n";
    jsxScript += "                  }\n";
    jsxScript += "                  if (point.hasRightHandle) {\n";
    jsxScript += "                    outTangents.push([point.rightHandle.x - point.anchor.x, -(point.rightHandle.y - point.anchor.y)]);\n";
    jsxScript += "                  } else {\n";
    jsxScript += "                    outTangents.push([0, 0]);\n";
    jsxScript += "                  }\n";
    jsxScript += "                }\n";
    jsxScript += "                shape.vertices = vertices;\n";
    jsxScript += "                shape.inTangents = inTangents;\n";
    jsxScript += "                shape.outTangents = outTangents;\n";
    jsxScript += "                shape.closed = object.isClosed;\n";
    jsxScript += "                pathProperty.setValue(shape);\n";
    jsxScript += "                if (object.fill) {\n";
    jsxScript += "                  if (object.fill.fillType === 'linear_gradient' || object.fill.fillType === 'radial_gradient') {\n";
    jsxScript += "                    var fillGradient = contentsGroup.addProperty('ADBE Vector Graphic - G-Fill');\n";
    jsxScript += "                    fillGradient.name = 'Gradient Fill ' + (j+1);\n";
    jsxScript += "                    shapeLayer.name += j === 0 ? '[GRADIENT]' : '';\n";
    jsxScript += "                    var gradientStops = (object.fill.gradient && object.fill.gradient.stops) ? object.fill.gradient.stops : [];\n";
    jsxScript += "                    var gradientInfo = {\n";
    jsxScript += "                        layerIndex: shapeLayer.index,\n";
    jsxScript += "                        layerName: shapeLayer.name,\n";
    jsxScript += "                        fillType: object.fill.fillType,\n";
    jsxScript += "                        gradient: {\n";
    jsxScript += "                            type: object.fill.fillType,\n";
    jsxScript += "                            angle: object.fill.gradient ? (object.fill.gradient.angle || 0) : 0,\n";
    jsxScript += "                            startPoint: object.fill.gradient && object.fill.gradient.startPoint ? \n";
    jsxScript += "                                {\n";
    jsxScript += "                                    x: object.fill.gradient.startPoint.x || 0,\n";
    jsxScript += "                                    y: object.fill.gradient.startPoint.y || 0\n";
    jsxScript += "                                } : {x: 0, y: 0},\n";
    jsxScript += "                            endPoint: object.fill.gradient && object.fill.gradient.endPoint ?\n";
    jsxScript += "                                {\n";
    jsxScript += "                                    x: object.fill.gradient.endPoint.x || 100,\n";
    jsxScript += "                                    y: object.fill.gradient.endPoint.y || 0\n";
    jsxScript += "                                } : {x: 100, y: 0},\n";
    jsxScript += "                            aspectRatio: object.fill.gradient ? (object.fill.gradient.aspectRatio || 1.0) : 1.0,\n";
    jsxScript += "                            stops: []\n";
    jsxScript += "                        }\n";
    jsxScript += "                    };\n";
    jsxScript += "                    if (object.fill.gradient && object.fill.gradient.stops) {\n";
    jsxScript += "                        for (var s = 0; s < object.fill.gradient.stops.length; s++) {\n";
    jsxScript += "                            var stop = object.fill.gradient.stops[s];\n";
    jsxScript += "                            if (stop) {\n";
    jsxScript += "                                gradientInfo.gradient.stops.push({\n";
    jsxScript += "                                    color: stop.color || '#000000',\n";
    jsxScript += "                                    position: stop.position !== undefined ? stop.position : 0,\n";
    jsxScript += "                                    opacity: stop.opacity !== undefined ? stop.opacity : 1.0\n";
    jsxScript += "                                });\n";
    jsxScript += "                            }\n";
    jsxScript += "                        }\n";
    jsxScript += "                        gradientInfo.gradient.stops.sort(function(a, b) {\n";
    jsxScript += "                            return a.position - b.position;\n";
    jsxScript += "                        });\n";
    jsxScript += "                        gradientData.push(gradientInfo);\n";
    jsxScript += "                    }\n";
    jsxScript += "                  } else if (object.fill.color) {\n";
    jsxScript += "                    var fill = contentsGroup.addProperty('ADBE Vector Graphic - Fill');\n";
    jsxScript += "                    if (!fill) {\n";
    jsxScript += "                        alert('fill is null');\n";
    jsxScript += "                    }\n";
    jsxScript += "                    var fillColor = hexToRgb(object.fill.color);\n";
    jsxScript += "                    fill.property('ADBE Vector Fill Color').setValue(fillColor);\n";
    jsxScript += "                  }\n";
    jsxScript += "                }\n";
    jsxScript += "                if (object.stroke && object.stroke.color) {\n";
    jsxScript += "                  var stroke = contentsGroup.addProperty('ADBE Vector Graphic - Stroke');\n";
    jsxScript += "                  var strokeColor = hexToRgb(object.stroke.color);\n";
    jsxScript += "                  stroke.property('ADBE Vector Stroke Color').setValue(strokeColor);\n";
    jsxScript += "                  if (object.stroke.width) {\n";
    jsxScript += "                    stroke.property('ADBE Vector Stroke Width').setValue(object.stroke.width);\n";
    jsxScript += "                  }\n";
    jsxScript += "                  if (object.stroke.cap !== undefined) {\n";
    jsxScript += "                    var lineCapMap = {0: 1, 1: 2, 2: 3}; // 0: Butt, 1: Round, 2: Square\n";
    jsxScript += "                    var lineCap = lineCapMap[object.stroke.cap] || 1;\n";
    jsxScript += "                    stroke.property('ADBE Vector Stroke Line Cap').setValue(lineCap);\n";
    jsxScript += "                  }\n";
    jsxScript += "                  if (object.stroke.join !== undefined) {\n";
    jsxScript += "                    var lineJoinMap = {0: 1, 1: 2, 2: 3}; // 0: Miter, 1: Round, 2: Bevel\n";
    jsxScript += "                    var lineJoin = lineJoinMap[object.stroke.join] || 1;\n";
    jsxScript += "                    stroke.property('ADBE Vector Stroke Line Join').setValue(lineJoin);\n";
    jsxScript += "                  }\n";
    jsxScript += "                }\n";
    jsxScript += "                if (object.opacity !== undefined && object.opacity !== null) {\n";
    jsxScript += "                  var shapeTransform = shapeGroup.property('ADBE Vector Transform Group');\n";
    jsxScript += "                  var shapeOpacity = shapeTransform.property('ADBE Vector Group Opacity');\n";
    jsxScript += "                  shapeOpacity.setValue(object.opacity * 100);\n";
    jsxScript += "                }\n";
    jsxScript += "                if (object.transform) {\n";
    jsxScript += "                  var shapeTransform = shapeGroup.property('ADBE Vector Transform Group');\n";
    jsxScript += "                  var groupPosition = shapeTransform.property('ADBE Vector Position');\n";
    jsxScript += "                  groupPosition.setValue([object.transform.tx, object.transform.ty]);\n";
    jsxScript += "                }\n";
    jsxScript += "                pathCount++;\n";
    jsxScript += "              }\n";
    jsxScript += "            }\n";
    jsxScript += "          }\n";
    jsxScript += "        }\n";
    jsxScript += "      }\n";
    jsxScript += "    }\n";
    jsxScript += "  }\n";
    jsxScript += "  if (gradientData.length > 0) {\n";
    jsxScript += "    try {\n";
    jsxScript += "      var gradientFile = new File(Folder.temp.fsName + '/aegr.json');\n";
    jsxScript += "      if (gradientFile.open('w')) {\n";
    jsxScript += "        var outputData = {\n";
    jsxScript += "          timestamp: new Date().getTime(),\n";
    jsxScript += "          compName: comp.name,\n";
    jsxScript += "          compWidth: comp.width,\n";
    jsxScript += "          compHeight: comp.height,\n";
    jsxScript += "          totalGradients: gradientData.length,\n";
    jsxScript += "          gradients: gradientData\n";
    jsxScript += "        };\n";
    jsxScript += "        var jsonGradient = simpleJSONStringify(outputData);\n";
    jsxScript += "        gradientFile.write(jsonGradient);\n";
    jsxScript += "        gradientFile.close();\n";
    jsxScript += "      }\n";
    jsxScript += "    } catch (e) {\n";
    jsxScript += "      alert('Warning: Could not save gradient data for processing: ' + e.toString());\n";
    jsxScript += "    }\n";
    jsxScript += "  }\n";
    jsxScript += "  var objectsArray = data.objects || [];\n";
    jsxScript += "  for (var i = objectsArray.length - 1; i >= 0; i--) {\n";
    jsxScript += "    var object = objectsArray[i];\n";
    jsxScript += "    if (object.type === 'text' && object.content) {\n";
    jsxScript += "      var textLayer = comp.layers.addText(object.content);\n";
    jsxScript += "      textLayer.name = 'Text ' + (textCount + 1);\n";
    jsxScript += "      var textProp = textLayer.property('ADBE Text Properties').property('ADBE Text Document');\n";
    jsxScript += "      var textDoc = textProp.value;\n";
    jsxScript += "      textDoc.fontSize = 24;\n";
    jsxScript += "      textDoc.fillColor = [0, 0, 0];\n";
    jsxScript += "      textDoc.font = 'Arial-Regular';\n";
    jsxScript += "      textDoc.justification = ParagraphJustification.LEFT_JUSTIFY;\n";
    jsxScript += "      textDoc.tracking = 0;\n";
    jsxScript += "      textDoc.leading = 28.8;\n";
    jsxScript += "      textProp.setValue(textDoc);\n";
    jsxScript += "      var textTransform = textLayer.property('ADBE Transform Group');\n";
    jsxScript += "      var textPosition = textTransform.property('ADBE Position');\n";
    jsxScript += "      textPosition.setValue([comp.width / 2, comp.height / 2]);\n";
    jsxScript += "      if (object.opacity !== undefined && object.opacity !== null) {\n";
    jsxScript += "        var textOpacity = textTransform.property('ADBE Opacity');\n";
    jsxScript += "        textOpacity.setValue(object.opacity);\n";
    jsxScript += "      }\n";
    jsxScript += "      textCount++;\n";
    jsxScript += "    }\n";
    jsxScript += "  }\n";
    jsxScript += "})();";
    
    A_Err err = suites.UtilitySuite6()->AEGP_ExecuteScript(
        0,
        jsxScript.c_str(),
        TRUE,
        NULL,
        NULL
    );
    
    if (err != A_Err_NONE) {
        // Show error dialog using Cocoa
        NSAlert* alert = [[NSAlert alloc] init];
        [alert setMessageText:@"Error"];
        [alert setInformativeText:@"Failed to execute JSX script for Get from AI."];
        [alert setAlertStyle:NSAlertStyleCritical];
        [alert addButtonWithTitle:@"OK"];
        [alert runModal];
    }
}

void PanelatorUI_Mac::ExecuteSendToAI() {
    AEGP_SuiteHandler suites(i_spbP);
    if (!LicensingUtils::CheckLicense(suites, i_pluginID, true)) {
        return;
    }
    
    std::string jsxScript;
    jsxScript += "(function() {\n";
    jsxScript += "  var comp = app.project.activeItem;\n";
    jsxScript += "  if (!comp || !(comp instanceof CompItem)) {\n";
    jsxScript += "    alert('Please select or open a composition first.');\n";
    jsxScript += "    return;\n";
    jsxScript += "  }\n";
    jsxScript += "  var selectedLayers = comp.selectedLayers;\n";
    jsxScript += "  if (selectedLayers.length === 0) {\n";
    jsxScript += "    alert('Please select at least one layer.');\n";
    jsxScript += "    return;\n";
    jsxScript += "  }\n";
    jsxScript += "  function rgbToHex(r, g, b) {\n";
    jsxScript += "    function componentToHex(c) {\n";
    jsxScript += "      var hex = Math.round(c * 255).toString(16);\n";
    jsxScript += "      return hex.length == 1 ? '0' + hex : hex;\n";
    jsxScript += "    }\n";
    jsxScript += "    return '#' + componentToHex(r) + componentToHex(g) + componentToHex(b);\n";
    jsxScript += "  }\n";
    jsxScript += "  function simpleJSONStringify(obj, indent) {\n";
    jsxScript += "    indent = indent || 0;\n";
    jsxScript += "    var spaces = '';\n";
    jsxScript += "    for (var i = 0; i < indent; i++) spaces += '  ';\n";
    jsxScript += "    if (obj === null) return 'null';\n";
    jsxScript += "    if (typeof obj === 'string') return '\"' + obj.replace(/\"/g, '\\\\\"') + '\"';\n";
    jsxScript += "    if (typeof obj === 'number' || typeof obj === 'boolean') return obj.toString();\n";
    jsxScript += "    if (obj instanceof Array) {\n";
    jsxScript += "      var result = '[\\n';\n";
    jsxScript += "      for (var i = 0; i < obj.length; i++) {\n";
    jsxScript += "        result += spaces + '  ' + simpleJSONStringify(obj[i], indent + 1);\n";
    jsxScript += "        if (i < obj.length - 1) result += ',';\n";
    jsxScript += "        result += '\\n';\n";
    jsxScript += "      }\n";
    jsxScript += "      result += spaces + ']';\n";
    jsxScript += "      return result;\n";
    jsxScript += "    }\n";
    jsxScript += "    if (typeof obj === 'object') {\n";
    jsxScript += "      var result = '{\\n';\n";
    jsxScript += "      var keys = [];\n";
    jsxScript += "      for (var key in obj) {\n";
    jsxScript += "        if (obj.hasOwnProperty(key)) keys.push(key);\n";
    jsxScript += "      }\n";
    jsxScript += "      for (var i = 0; i < keys.length; i++) {\n";
    jsxScript += "        var key = keys[i];\n";
    jsxScript += "        result += spaces + '  \"' + key + '\": ' + simpleJSONStringify(obj[key], indent + 1);\n";
    jsxScript += "        if (i < keys.length - 1) result += ',';\n";
    jsxScript += "        result += '\\n';\n";
    jsxScript += "      }\n";
    jsxScript += "      result += spaces + '}';\n";
    jsxScript += "      return result;\n";
    jsxScript += "    }\n";
    jsxScript += "    return 'undefined';\n";
    jsxScript += "  }\n";
    jsxScript += "  function getLayerFillColor(layer) {\n";
    jsxScript += "    if (layer.matchName !== 'ADBE Vector Layer') {\n";
    jsxScript += "      return null;\n";
    jsxScript += "    }\n";
    jsxScript += "    var layerInfo = {\n";
    jsxScript += "      name: layer.name,\n";
    jsxScript += "      index: layer.index,\n";
    jsxScript += "      opacity: null,\n";
    jsxScript += "      fillColor: null,\n";
    jsxScript += "      hasPath: false,\n";
    jsxScript += "      strokeColor: null,\n";
    jsxScript += "      strokeWidth: null,\n";
    jsxScript += "      paths: []\n";
    jsxScript += "    };\n";
    jsxScript += "    var transform = layer.property('ADBE Transform Group');\n";
    jsxScript += "    if (transform) {\n";
    jsxScript += "      var opacityProp = transform.property('ADBE Opacity');\n";
    jsxScript += "      if (opacityProp) {\n";
    jsxScript += "        layerInfo.opacity = opacityProp.value;\n";
    jsxScript += "      }\n";
    jsxScript += "    }\n";
    jsxScript += "    var contents = layer.property('ADBE Root Vectors Group');\n";
    jsxScript += "    if (!contents) {\n";
    jsxScript += "      return layerInfo;\n";
    jsxScript += "    }\n";
    jsxScript += "    function searchForPathAndFill(group, depth) {\n";
    jsxScript += "      depth = depth || 0;\n";
    jsxScript += "      var indent = '';\n";
    jsxScript += "      for (var d = 0; d < depth; d++) indent += '  ';\n";
    jsxScript += "      for (var i = 1; i <= group.numProperties; i++) {\n";
    jsxScript += "        var prop = group.property(i);\n";
    jsxScript += "        if (!prop) continue;\n";
    jsxScript += "        if (prop.matchName === 'ADBE Vector Shape - Group') {\n";
    jsxScript += "          var shapePath = prop.property('ADBE Vector Shape');\n";
    jsxScript += "          if (shapePath) {\n";
    jsxScript += "            layerInfo.hasPath = true;\n";
    jsxScript += "            var shape = shapePath.value;\n";
    jsxScript += "            var pathData = {\n";
    jsxScript += "              closed: shape.closed,\n";
    jsxScript += "              vertices: [],\n";
    jsxScript += "              inTangents: [],\n";
    jsxScript += "              outTangents: []\n";
    jsxScript += "            };\n";
    jsxScript += "            for (var j = 0; j < shape.vertices.length; j++) {\n";
    jsxScript += "              pathData.vertices.push([shape.vertices[j][0], shape.vertices[j][1]]);\n";
    jsxScript += "              pathData.inTangents.push([shape.inTangents[j][0], shape.inTangents[j][1]]);\n";
    jsxScript += "              pathData.outTangents.push([shape.outTangents[j][0], shape.outTangents[j][1]]);\n";
    jsxScript += "            }\n";
    jsxScript += "            if (!layerInfo.paths) {\n";
    jsxScript += "              layerInfo.paths = [];\n";
    jsxScript += "            }\n";
    jsxScript += "            layerInfo.paths.push(pathData);\n";
    jsxScript += "          }\n";
    jsxScript += "        }\n";
    jsxScript += "        else if (prop.matchName === 'ADBE Vector Shape - Rect') {\n";
    jsxScript += "          layerInfo.hasPath = true;\n";
    jsxScript += "          var rectSize = prop.property ? prop.property('ADBE Vector Rect Size') : null;\n";
    jsxScript += "          var rectPos = prop.property ? prop.property('ADBE Vector Rect Position') : null;\n";
    jsxScript += "          if (rectSize && rectPos) {\n";
    jsxScript += "            var size = rectSize.value;\n";
    jsxScript += "            var pos = rectPos.value;\n";
    jsxScript += "            var pathData = {\n";
    jsxScript += "              type: 'rectangle',\n";
    jsxScript += "              size: [size[0], size[1]],\n";
    jsxScript += "              position: [pos[0], pos[1]]\n";
    jsxScript += "            };\n";
    jsxScript += "            if (!layerInfo.paths) {\n";
    jsxScript += "              layerInfo.paths = [];\n";
    jsxScript += "            }\n";
    jsxScript += "            layerInfo.paths.push(pathData);\n";
    jsxScript += "          }\n";
    jsxScript += "        }\n";
    jsxScript += "        else if (prop.matchName === 'ADBE Vector Shape - Ellipse') {\n";
    jsxScript += "          layerInfo.hasPath = true;\n";
    jsxScript += "          var ellipseSize = prop.property ? prop.property('ADBE Vector Ellipse Size') : null;\n";
    jsxScript += "          var ellipsePos = prop.property ? prop.property('ADBE Vector Ellipse Position') : null;\n";
    jsxScript += "          if (ellipseSize && ellipsePos) {\n";
    jsxScript += "            var size = ellipseSize.value;\n";
    jsxScript += "            var pos = ellipsePos.value;\n";
    jsxScript += "            var pathData = {\n";
    jsxScript += "              type: 'ellipse',\n";
    jsxScript += "              size: [size[0], size[1]],\n";
    jsxScript += "              position: [pos[0], pos[1]]\n";
    jsxScript += "            };\n";
    jsxScript += "            if (!layerInfo.paths) {\n";
    jsxScript += "              layerInfo.paths = [];\n";
    jsxScript += "            }\n";
    jsxScript += "            layerInfo.paths.push(pathData);\n";
    jsxScript += "          }\n";
    jsxScript += "        }\n";
    jsxScript += "        else if (prop.matchName === 'ADBE Vector Shape - Star') {\n";
    jsxScript += "          layerInfo.hasPath = true;\n";
    jsxScript += "          var starPoints = prop.property ? prop.property('ADBE Vector Star Points') : null;\n";
    jsxScript += "          var starPos = prop.property ? prop.property('ADBE Vector Star Position') : null;\n";
    jsxScript += "          if (starPoints && starPos) {\n";
    jsxScript += "            var points = starPoints.value;\n";
    jsxScript += "            var pos = starPos.value;\n";
    jsxScript += "            var pathData = {\n";
    jsxScript += "              type: 'star',\n";
    jsxScript += "              points: points,\n";
    jsxScript += "              position: [pos[0], pos[1]]\n";
    jsxScript += "            };\n";
    jsxScript += "            if (!layerInfo.paths) {\n";
    jsxScript += "              layerInfo.paths = [];\n";
    jsxScript += "            }\n";
    jsxScript += "            layerInfo.paths.push(pathData);\n";
    jsxScript += "          }\n";
    jsxScript += "        }\n";
    jsxScript += "        else if (prop.matchName === 'ADBE Vector Graphic - Fill') {\n";
    jsxScript += "          var fillColor = prop.property('ADBE Vector Fill Color');\n";
    jsxScript += "          if (fillColor) {\n";
    jsxScript += "            var color = fillColor.value;\n";
    jsxScript += "            layerInfo.fillColor = rgbToHex(color[0], color[1], color[2]);\n";
    jsxScript += "          }\n";
    jsxScript += "        }\n";
    jsxScript += "        else if (prop.matchName === 'ADBE Vector Graphic - Stroke') {\n";
    jsxScript += "          var strokeColor = prop.property('ADBE Vector Stroke Color');\n";
    jsxScript += "          var strokeWidth = prop.property('ADBE Vector Stroke Width');\n";
    jsxScript += "          if (strokeColor) {\n";
    jsxScript += "            var color = strokeColor.value;\n";
    jsxScript += "            layerInfo.strokeColor = rgbToHex(color[0], color[1], color[2]);\n";
    jsxScript += "          }\n";
    jsxScript += "          if (strokeWidth) {\n";
    jsxScript += "            layerInfo.strokeWidth = strokeWidth.value;\n";
    jsxScript += "          }\n";
    jsxScript += "        }\n";
    jsxScript += "        else if (prop.matchName === 'ADBE Vector Group' || prop.matchName === 'ADBE Vectors Group') {\n";
    jsxScript += "          searchForPathAndFill(prop, depth + 1);\n";
    jsxScript += "        }\n";
    jsxScript += "      }\n";
    jsxScript += "    }\n";
    jsxScript += "    searchForPathAndFill(contents, 0);\n";
    jsxScript += "    return layerInfo;\n";
    jsxScript += "  }\n";
    jsxScript += "  var results = [];\n";
    jsxScript += "  var pathLayerCount = 0;\n";
    jsxScript += "  for (var i = 0; i < selectedLayers.length; i++) {\n";
    jsxScript += "    var layerInfo = getLayerFillColor(selectedLayers[i]);\n";
    jsxScript += "    if (layerInfo) {\n";
    jsxScript += "      results.push(layerInfo);\n";
    jsxScript += "      if (layerInfo.hasPath) pathLayerCount++;\n";
    jsxScript += "    }\n";
    jsxScript += "  }\n";
    jsxScript += "  if (results.length === 0) {\n";
    jsxScript += "    alert('No shape layers found in selection.');\n";
    jsxScript += "    return;\n";
    jsxScript += "  }\n";
    jsxScript += "  results.sort(function(a,b) {\n";
    jsxScript += "    return a.index - b.index;\n";
    jsxScript += "  });\n";
    jsxScript += "  var now = new Date();\n";
    jsxScript += "  var timestamp = now.getFullYear() + '-' + \n";
    jsxScript += "    (now.getMonth() + 1) + '-' + \n";
    jsxScript += "    now.getDate() + ' ' + \n";
    jsxScript += "    now.getHours() + ':' + \n";
    jsxScript += "    now.getMinutes() + ':' + \n";
    jsxScript += "    now.getSeconds();\n";
    jsxScript += "  var exportData = {\n";
    jsxScript += "    timestamp: timestamp,\n";
    jsxScript += "    totalSelected: selectedLayers.length,\n";
    jsxScript += "    shapeLayersFound: results.length,\n";
    jsxScript += "    pathLayersFound: pathLayerCount,\n";
    jsxScript += "    layers: results\n";
    jsxScript += "  };\n";
    jsxScript += "  var jsonString = simpleJSONStringify(exportData);\n";
    jsxScript += "  var outputFile = new File(Folder.temp.fsName + '/ae2ai.json');\n";
    jsxScript += "  if (!outputFile.open('w')) {\n";
    jsxScript += "    alert('Failed to create output file: ' + outputFile.error);\n";
    jsxScript += "    return;\n";
    jsxScript += "  }\n";
    jsxScript += "  outputFile.write(jsonString);\n";
    jsxScript += "  outputFile.close();\n";
    //jsxScript += "  alert('Successfully exported layer opacity and colors from ' + results.length + ' shape layers (' + pathLayerCount + ' with paths) to ae2ai.json');\n";
    jsxScript += "})();\n";
    
    A_Err err = suites.UtilitySuite6()->AEGP_ExecuteScript(
        0,
        jsxScript.c_str(),
        TRUE,
        NULL,
        NULL
    );
    
    if (err != A_Err_NONE) {
        // Show error dialog using Cocoa
        NSAlert* alert = [[NSAlert alloc] init];
        [alert setMessageText:@"Error"];
        [alert setInformativeText:@"Failed to execute JSX script for Send to AI."];
        [alert setAlertStyle:NSAlertStyleCritical];
        [alert addButtonWithTitle:@"OK"];
        [alert runModal];
    }
}