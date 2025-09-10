cmake_minimum_required(VERSION 3.20)
project(PathLink)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- macOS-specific settings ---
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13")
    enable_language(OBJCXX)
    set(CMAKE_OBJCXX_FLAGS "${CMAKE_OBJCXX_FLAGS} -fobjc-arc")
endif()

# --- Include directories ---
# Note: Order matters! AE SDK headers should come first
include_directories(
    # After Effects SDK headers - these must come first
    ${CMAKE_SOURCE_DIR}/Examples/Headers              # Main AE headers (contains AEConfig.h)
    ${CMAKE_SOURCE_DIR}/Examples/Headers/SP           # Smart Pointers
    ${CMAKE_SOURCE_DIR}/Examples/Resources            # Resources  
    ${CMAKE_SOURCE_DIR}/Examples/Util                 # Utilities
    # Project-specific directories
    ${CMAKE_CURRENT_SOURCE_DIR}                       # Panelator root
    ${CMAKE_CURRENT_SOURCE_DIR}/Win                   # Parser.h, LicensingUtils.h (cross-platform)
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party           # JSON, other third-party headers
    ${CMAKE_CURRENT_SOURCE_DIR}/aescripts-PLUGINS-licensing-framework/include
)

# Platform-specific include directories
if(APPLE)
    include_directories(
        ${CMAKE_SOURCE_DIR}/Examples/Headers/Mac      # macOS-specific headers
    )
elseif(WIN32)
    include_directories(
        ${CMAKE_SOURCE_DIR}/Examples/Headers/Win      # Windows-specific headers
    )
endif()

# --- Cross-platform source files ---
set(SOURCES
    panelator.cpp
    PanelatorUI.cpp
    Panelator_Strings.cpp
    # Include cross-platform sources from Win directory
    Win/Parser.cpp
    Win/LicensingUtils.cpp
)

# --- Platform-specific sources ---
if(APPLE)
    # Check if PanelatorUI_Mac.mm exists, otherwise create a placeholder or use cross-platform version
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Win/PanelatorUI_Mac.mm")
        list(APPEND SOURCES Win/PanelatorUI_Mac.mm)
    else()
        # Use the Windows platform file as base for now - you may need to adapt this
        list(APPEND SOURCES Win/PanelatorUI_Plat.cpp)
    endif()
elseif(WIN32)
    list(APPEND SOURCES
        Win/PanelatorUI_Plat.cpp
    )
endif()

# --- Create plugin target ---
if(APPLE)
    add_library(PathLink MODULE ${SOURCES})
    set_target_properties(PathLink PROPERTIES
        BUNDLE TRUE
        BUNDLE_EXTENSION "plugin"
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
        MACOSX_BUNDLE_BUNDLE_NAME "PathLink"
        MACOSX_BUNDLE_IDENTIFIER "com.yourcompany.pathlink"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
        MACOSX_BUNDLE_BUNDLE_VERSION "1"
    )
    target_link_libraries(PathLink
        "-framework Cocoa"
        "-framework CoreFoundation"
        "-framework Foundation"
    )
    target_compile_definitions(PathLink PRIVATE
        MAC_ENV=1
        kMacOS=1
        MSWindows=0
    )
    
    # macOS-specific compiler flags
    target_compile_options(PathLink PRIVATE
        -Wno-deprecated-declarations
        -fvisibility=hidden
    )
    
elseif(WIN32)
    add_library(PathLink SHARED ${SOURCES})
    set_target_properties(PathLink PROPERTIES SUFFIX ".aex")
    target_link_libraries(PathLink shell32 user32)
    target_compile_definitions(PathLink PRIVATE
        WIN_ENV=1
        _WINDOWS=1
        MSWindows=1
    )
endif()