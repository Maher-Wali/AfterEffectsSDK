cmake_minimum_required(VERSION 3.20)
project(PathLink)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Debug: Print directory information ---
message(STATUS "CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")

# Now CMAKE_SOURCE_DIR should be the repository root
set(PANELATOR_SOURCE_DIR "${CMAKE_SOURCE_DIR}/Examples/AEGP/Panelator")

# Check if key directories exist
set(AE_HEADERS_DIR "${CMAKE_SOURCE_DIR}/Examples/Headers")
if(EXISTS "${AE_HEADERS_DIR}")
    message(STATUS "✓ Found AE Headers directory: ${AE_HEADERS_DIR}")
    # List some key files
    file(GLOB AE_HEADER_FILES "${AE_HEADERS_DIR}/*.h")
    message(STATUS "Sample header files in ${AE_HEADERS_DIR}:")
    foreach(header ${AE_HEADER_FILES})
        get_filename_component(header_name ${header} NAME)
        message(STATUS "  - ${header_name}")
    endforeach()
else()
    message(WARNING "✗ AE Headers directory NOT FOUND: ${AE_HEADERS_DIR}")
endif()

# Check specifically for AEConfig.h
set(AECONFIG_PATH "${AE_HEADERS_DIR}/AEConfig.h")
if(EXISTS "${AECONFIG_PATH}")
    message(STATUS "✓ Found AEConfig.h at: ${AECONFIG_PATH}")
else()
    message(WARNING "✗ AEConfig.h NOT FOUND at: ${AECONFIG_PATH}")
    # Search for it elsewhere
    file(GLOB_RECURSE AECONFIG_SEARCH "${CMAKE_SOURCE_DIR}/**/AEConfig.h")
    if(AECONFIG_SEARCH)
        message(STATUS "Found AEConfig.h at alternative locations:")
        foreach(config_file ${AECONFIG_SEARCH})
            message(STATUS "  - ${config_file}")
        endforeach()
    else()
        message(WARNING "AEConfig.h not found anywhere in the project!")
    endif()
endif()

# --- macOS-specific settings ---
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13")
    enable_language(OBJCXX)
    set(CMAKE_OBJCXX_FLAGS "${CMAKE_OBJCXX_FLAGS} -fobjc-arc")
endif()

# --- Include directories ---
# Note: Order matters! AE SDK headers should come first

# Define all include directories in variables first for easier debugging
set(INCLUDE_DIRS
    # After Effects SDK headers - these must come first
    ${CMAKE_SOURCE_DIR}/Examples/Headers              # Main AE headers (contains AEConfig.h)
    ${CMAKE_SOURCE_DIR}/Examples/Headers/SP           # Smart Pointers
    ${CMAKE_SOURCE_DIR}/Examples/Resources            # Resources  
    ${CMAKE_SOURCE_DIR}/Examples/Util                 # Utilities
    # Project-specific directories
    ${PANELATOR_SOURCE_DIR}                          # Panelator root
    ${PANELATOR_SOURCE_DIR}/Win                      # Parser.h, LicensingUtils.h (cross-platform)
    ${PANELATOR_SOURCE_DIR}/third_party              # JSON, other third-party headers
    ${PANELATOR_SOURCE_DIR}/aescripts-PLUGINS-licensing-framework/include
)

# Platform-specific include directories
if(APPLE)
    list(APPEND INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/Examples/Headers/Mac)
elseif(WIN32)
    list(APPEND INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/Examples/Headers/Win)
endif()

# Debug: Print all include directories and check if they exist
message(STATUS "=== INCLUDE DIRECTORIES DEBUG ===")
foreach(inc_dir ${INCLUDE_DIRS})
    if(EXISTS "${inc_dir}")
        message(STATUS "✓ Include dir exists: ${inc_dir}")
    else()
        message(WARNING "✗ Include dir MISSING: ${inc_dir}")
    endif()
endforeach()

# Apply the include directories
include_directories(${INCLUDE_DIRS})

# --- Cross-platform source files ---
set(SOURCES
    ${PANELATOR_SOURCE_DIR}/panelator.cpp
    ${PANELATOR_SOURCE_DIR}/PanelatorUI.cpp
    ${PANELATOR_SOURCE_DIR}/Panelator_Strings.cpp
    # Include cross-platform sources from Win directory
    ${PANELATOR_SOURCE_DIR}/Win/Parser.cpp
    ${PANELATOR_SOURCE_DIR}/Win/LicensingUtils.cpp
)

# --- Platform-specific sources ---
if(APPLE)
    # Check if PanelatorUI_Mac.mm exists, otherwise use cross-platform version
    if(EXISTS "${PANELATOR_SOURCE_DIR}/Win/PanelatorUI_Mac.mm")
        list(APPEND SOURCES ${PANELATOR_SOURCE_DIR}/Win/PanelatorUI_Mac.mm)
    else()
        # Use the Windows platform file as base for now - you may need to adapt this
        list(APPEND SOURCES ${PANELATOR_SOURCE_DIR}/Win/PanelatorUI_Plat.cpp)
    endif()
elseif(WIN32)
    list(APPEND SOURCES
        ${PANELATOR_SOURCE_DIR}/Win/PanelatorUI_Plat.cpp
    )
endif()

# --- Create plugin target ---
if(APPLE)
    add_library(PathLink MODULE ${SOURCES})
    set_target_properties(PathLink PROPERTIES
        BUNDLE TRUE
        BUNDLE_EXTENSION "plugin"
        MACOSX_BUNDLE_INFO_PLIST ${PANELATOR_SOURCE_DIR}/Info.plist.in
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