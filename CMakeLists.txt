cmake_minimum_required(VERSION 3.20)
project(AEPathLink)
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
    set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0")
    enable_language(OBJCXX)
    set(CMAKE_OBJCXX_FLAGS "${CMAKE_OBJCXX_FLAGS} -fobjc-arc")
    
    # IMPORTANT: Set all .cpp files to be compiled as Objective-C++
    set(CMAKE_CXX_COMPILE_OBJECT "${CMAKE_OBJCXX_COMPILE_OBJECT}")
endif()

if(APPLE)
    # Build universal binary with Intel + Apple Silicon
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "" FORCE)
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
        message(STATUS "✗ Include dir MISSING: ${inc_dir}")
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

list(APPEND SOURCES
	${CMAKE_SOURCE_DIR}/Examples/Util/AEGP_SuiteHandler.cpp
	${CMAKE_SOURCE_DIR}/Examples/Util/AEGP_Utils.cpp
	${CMAKE_SOURCE_DIR}/Examples/Util/MissingSuiteError.cpp
	${CMAKE_SOURCE_DIR}/Examples/Util/Smart_Utils.cpp
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
    # Add SuiteHandler.cpp from AE SDK Util
    list(APPEND SOURCES
        ${CMAKE_SOURCE_DIR}/Examples/Util/AEGP_SuiteHandler.cpp
    )
    add_library(AEPathLink MODULE ${SOURCES})

    # CRITICAL: Set all source files to be compiled as Objective-C++
    set_source_files_properties(${SOURCES} PROPERTIES
        LANGUAGE OBJCXX
        COMPILE_FLAGS "-x objective-c++ -fobjc-arc"
    )

    # Define bundle vars for Info.plist substitution
    set(MACOSX_BUNDLE_EXECUTABLE_NAME "AEPathLink")
    set(MACOSX_BUNDLE_BUNDLE_NAME "AEPathLink")
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0")
    set(MACOSX_BUNDLE_BUNDLE_VERSION "1")
    set(MACOSX_BUNDLE_IDENTIFIER "com.yourcompany.aepathlink")  # Customize as needed

    # Generate Info.plist from template
    configure_file(${PANELATOR_SOURCE_DIR}/Info.plist.in ${CMAKE_CURRENT_BINARY_DIR}/Info.plist @ONLY)

    set_target_properties(AEPathLink PROPERTIES
        BUNDLE TRUE
        BUNDLE_EXTENSION "plugin"
        MACOSX_BUNDLE_INFO_PLIST ${PANELATOR_SOURCE_DIR}/Info.plist
        MACOSX_BUNDLE_BUNDLE_NAME "AEPathLink"
        MACOSX_BUNDLE_IDENTIFIER "com.yourcompany.aepathlink"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0.0"
        MACOSX_BUNDLE_BUNDLE_VERSION "1"
    )

    # Path to aescripts licensing library (adjust if your repo has a different structure)
    set(AESCRIPTS_LIB
        ${PANELATOR_SOURCE_DIR}/aescripts-PLUGINS-licensing-framework/lib/macos/aescriptsLicensing_Release.a
    )

    target_link_libraries(AEPathLink
        "-framework Cocoa"
        "-framework CoreFoundation"
        "-framework IOKit"
        "-lcurl"
        ${AESCRIPTS_LIB}
    )

    target_compile_definitions(AEPathLink PRIVATE
        MAC_ENV=1
        kMacOS=1
        MSWindows=0
        AEGP_PLUGIN=1
        AE_PLUGIN_SUPPORT_PANELS=0
    )

    target_compile_options(AEPathLink PRIVATE
        -Wno-deprecated-declarations
        -fvisibility=hidden
        -Wno-pragma-pack
        -Wno-unknown-pragmas
        -Wno-macro-redefined
        -Wno-nonportable-include-paths
        -Wno-undefined-var-template
        -Wno-null-conversion
        -fobjc-arc
    )

# Define PiPL resource path
set(PIPL_RESOURCE "${PANELATOR_SOURCE_DIR}/Win/AEPathLink.r")

# Verify input files
if(NOT EXISTS "${PIPL_RESOURCE}")
    message(FATAL_ERROR "PiPL resource file not found at: ${PIPL_RESOURCE}")
endif()
set(AE_CONFIG_HEADER "${CMAKE_SOURCE_DIR}/Examples/Headers/AEConfig.h")
set(AE_GENERAL_RESOURCE "${CMAKE_SOURCE_DIR}/Examples/Resources/AE_General.r")
if(NOT EXISTS "${AE_CONFIG_HEADER}")
    message(FATAL_ERROR "AEConfig.h not found at: ${AE_CONFIG_HEADER}")
endif()
if(NOT EXISTS "${AE_GENERAL_RESOURCE}")
    message(FATAL_ERROR "AE_General.r not found at: ${AE_GENERAL_RESOURCE}")
endif()

# --- Compile .r resource file into .rsrc (build-dir then copy into bundle) ---
set(RSRC_INPUT "${CMAKE_SOURCE_DIR}/Examples/Resources/AEPathLink.r")
set(RSRC_BUILD_OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/AEPathLink.rsrc")
set(RSRC_BUNDLE_OUTPUT "$<TARGET_BUNDLE_CONTENT_DIR:AEPathLink>/Resources/AEPathLink.rsrc")

add_custom_command(TARGET AEPathLink POST_BUILD
    # Ensure bundle Resources dir exists
    COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_BUNDLE_CONTENT_DIR:AEPathLink>/Resources"

    # Rez produces the resource file in build dir
    COMMAND xcrun rez "${RSRC_INPUT}" -useDF
        -i "${CMAKE_SOURCE_DIR}/Examples/Headers"
        -i "${CMAKE_SOURCE_DIR}/Examples/Resources"
        -i "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/"
        -o "${RSRC_BUILD_OUTPUT}"

    # Copy from build dir into the bundle
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${RSRC_BUILD_OUTPUT}" "${RSRC_BUNDLE_OUTPUT}"

    # Show attributes and sizes for debug
    COMMAND ${CMAKE_COMMAND} -E echo "RSRC build output: ${RSRC_BUILD_OUTPUT}"
    COMMAND ${CMAKE_COMMAND} -E echo "RSRC bundle output: ${RSRC_BUNDLE_OUTPUT}"
    COMMAND ls -l "${RSRC_BUILD_OUTPUT}"
    COMMAND ${CMAKE_COMMAND} -E size "${RSRC_BUILD_OUTPUT}"

    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Compiling AEPathLink.r into flat AEPathLink.rsrc"
)


# Define bundle vars for Info.plist substitution
set(MACOSX_BUNDLE_EXECUTABLE_NAME "AEPathLink")
set(MACOSX_BUNDLE_BUNDLE_NAME "AEPathLink")
set(MACOSX_BUNDLE_IDENTIFIER "com.example.AEPathLink")
set(MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0")
set(MACOSX_BUNDLE_BUNDLE_VERSION "1")

# Generate Info.plist
configure_file(
    ${PANELATOR_SOURCE_DIR}/Info.plist.in
    ${CMAKE_CURRENT_BINARY_DIR}/Info.plist
    @ONLY
)

# Set compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wno-deprecated-declarations -fvisibility=hidden -Wno-pragma-pack -Wno-unknown-pragmas -Wno-nonportable-include-path -Wno-undefined-var-template -Wno-null-conversion")
set(CMAKE_OBJCXX_FLAGS "${CMAKE_OBJCXX_FLAGS} -fobjc-arc")

# Bundle properties
set_target_properties(AEPathLink PROPERTIES
    BUNDLE TRUE
    BUNDLE_EXTENSION "plugin"
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/Info.plist
    MACOSX_BUNDLE_BUNDLE_NAME "AEPathLink"
    MACOSX_BUNDLE_EXECUTABLE_NAME "AEPathLink"
    MACOSX_BUNDLE_IDENTIFIER "com.example.AEPathLink"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
    MACOSX_BUNDLE_BUNDLE_VERSION "1"
)

# Bundle setup (PkgInfo and .rsrc)
add_custom_command(TARGET AEPathLink POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory $<TARGET_BUNDLE_CONTENT_DIR:AEPathLink>/Resources
    COMMAND ls -l ${CMAKE_CURRENT_BINARY_DIR}/AEPathLink.rsrc || echo "Error: AEPathLink.rsrc missing before copy"
    COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/AEPathLink.rsrc $<TARGET_BUNDLE_CONTENT_DIR:AEPathLink>/Resources/AEPathLink.rsrc || echo "Error: cp failed for AEPathLink.rsrc"
    COMMAND ls -l $<TARGET_BUNDLE_CONTENT_DIR:AEPathLink>/Resources/AEPathLink.rsrc || echo "Error: AEPathLink.rsrc missing after copy"
    COMMAND printf "AEgp????" > ${CMAKE_CURRENT_BINARY_DIR}/PkgInfo_temp
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/PkgInfo_temp $<TARGET_BUNDLE_CONTENT_DIR:AEPathLink>/PkgInfo
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_CURRENT_BINARY_DIR}/PkgInfo_temp
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/AEPathLink.rsrc ${CMAKE_CURRENT_BINARY_DIR}/Info.plist
    COMMENT "Setting up bundle: PkgInfo and .rsrc"
    VERBATIM
)

endif()