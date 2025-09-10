cmake_minimum_required(VERSION 3.20)
project(PathLink)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# macOS-specific settings
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13")
    enable_language(OBJCXX)
    set(CMAKE_OBJCXX_FLAGS "${CMAKE_OBJCXX_FLAGS} -fobjc-arc")
endif()

# --- Include directories ---
set(SDK_HEADERS
    ${CMAKE_SOURCE_DIR}/Examples/Headers
    ${CMAKE_SOURCE_DIR}/Examples/Headers/SP
    ${CMAKE_SOURCE_DIR}/Examples/Headers/Win
    ${CMAKE_SOURCE_DIR}/Examples/Resources
    ${CMAKE_SOURCE_DIR}/Examples/Util

)

# Debug: print all include directories
message("Panelator source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
message("SDK include directories:")
foreach(dir ${SDK_HEADERS})
    message("  ${dir}")
endforeach()

# Optional: check which headers are visible
foreach(dir ${SDK_HEADERS})
    file(GLOB HEADER_FILES "${dir}/*.h")
    message("Headers in ${dir}: ${HEADER_FILES}")
endforeach()

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}                       # Panelator root
    ${CMAKE_CURRENT_SOURCE_DIR}/Win                   # Parser.h, LicensingUtils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party           # JSON etc.
    ${SDK_HEADERS}                                    # AE SDK headers
    ${CMAKE_CURRENT_SOURCE_DIR}/aescripts-PLUGINS-licensing-framework/include
)

# --- Cross-platform sources ---
set(SOURCES
    panelator.cpp
    PanelatorUI.cpp
    Panelator_Strings.cpp
)

# Platform-specific sources
if(APPLE)
    list(APPEND SOURCES
        Win/Parser.cpp
        Win/LicensingUtils.cpp
        Win/PanelatorUI_Mac.mm
    )
elseif(WIN32)
    list(APPEND SOURCES
        Win/Parser.cpp
        Win/LicensingUtils.cpp
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
    )
elseif(WIN32)
    add_library(PathLink SHARED ${SOURCES})
    set_target_properties(PathLink PROPERTIES SUFFIX ".aex")

    target_link_libraries(PathLink shell32 user32)

    target_compile_definitions(PathLink PRIVATE
        WIN_ENV=1
        _WINDOWS=1
    )
endif()
