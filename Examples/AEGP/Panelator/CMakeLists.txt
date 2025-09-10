cmake_minimum_required(VERSION 3.20)
project(PathLink)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform-specific settings
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13")
    enable_language(OBJCXX)
    set(CMAKE_OBJCXX_FLAGS "${CMAKE_OBJCXX_FLAGS} -fobjc-arc")
endif()

# Include directories (relative to AfterEffectsSDK root)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}                      # Panelator root
    ${CMAKE_CURRENT_SOURCE_DIR}/Headers              # if you add local headers
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
    ${CMAKE_CURRENT_SOURCE_DIR}/Win
    ${CMAKE_CURRENT_SOURCE_DIR}/../../SDK/Headers
    ${CMAKE_CURRENT_SOURCE_DIR}/../../SDK/Examples/Headers
    ${CMAKE_CURRENT_SOURCE_DIR}/../../SDK/Util
)

# Collect cross-platform sources
set(SOURCES
    panelator.cpp
    PanelatorUI.cpp
    Panelator_Strings.cpp
    Parser.cpp
    LicensingUtils.cpp
)

# Platform-specific sources
if(WIN32)
    list(APPEND SOURCES
        Win/PanelatorUI_Plat.cpp
    )
elseif(APPLE)
    list(APPEND SOURCES
        PanelatorUI_Mac.mm
    )
endif()

# Create plugin target
if(APPLE)
    # macOS plugin bundle
    add_library(PathLink MODULE ${SOURCES})

    # Bundle properties
    set_target_properties(PathLink PROPERTIES
        BUNDLE TRUE
        BUNDLE_EXTENSION "plugin"
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in
        MACOSX_BUNDLE_BUNDLE_NAME "PathLink"
        MACOSX_BUNDLE_IDENTIFIER "com.yourcompany.pathlink"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
        MACOSX_BUNDLE_BUNDLE_VERSION "1"
    )

    # Link macOS frameworks
    target_link_libraries(PathLink
        "-framework Cocoa"
        "-framework CoreFoundation"
        "-framework Foundation"
    )

    target_compile_definitions(PathLink PRIVATE
        MAC_ENV=1
        kMacOS=1
    )

elseif(WIN32)
    # Windows plugin
    add_library(PathLink SHARED ${SOURCES})
    set_target_properties(PathLink PROPERTIES SUFFIX ".aex")

    target_link_libraries(PathLink shell32 user32)

    target_compile_definitions(PathLink PRIVATE
        WIN_ENV=1
        _WINDOWS=1
    )
endif()
