cmake_minimum_required(VERSION 3.20)
project(AEPathLink)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- macOS-specific settings ---
if(APPLE)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "12.0")
    enable_language(OBJCXX)
    set(CMAKE_OBJCXX_FLAGS "${CMAKE_OBJCXX_FLAGS} -fobjc-arc")
    set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
endif()

# --- Include directories ---
set(INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/Examples/Headers
    ${CMAKE_SOURCE_DIR}/Examples/Resources
    ${CMAKE_SOURCE_DIR}/Examples/AE_Effect
)
include_directories(${INCLUDE_DIRS})

# --- Preprocessor definitions ---
if(APPLE)
    add_definitions(-DMAC_ENV=1 -DMACH)
else()
    add_definitions(-DWIN_ENV=1)
endif()

# --- Source files ---
if(APPLE)
    set(SOURCES
        ${CMAKE_SOURCE_DIR}/Examples/AE_Effect/AEPathLink.cpp
        ${CMAKE_SOURCE_DIR}/Examples/AE_Effect/AEPathLinkUI.cpp
    )
else()
    set(SOURCES
        ${CMAKE_SOURCE_DIR}/Examples/AE_Effect/AEPathLink.cpp
        ${CMAKE_SOURCE_DIR}/Examples/AE_Effect/AEPathLinkUI_Win.cpp
    )
endif()

# --- Info.plist ---
set(PRODUCT_NAME "AEPathLink")
set(AE_MAJOR_VERSION 1)
set(AE_MINOR_VERSION 0)
set(AE_REVISION_VERSION 0)
set(BUILD_NUMBER 1)
set(COPYRIGHT_STRING "© 2025 Your Company")
set(AE_BUNDLE_PREFIX "com.yourcompany")

configure_file(
    "${CMAKE_SOURCE_DIR}/Examples/Resources/Info.plist"
    ${CMAKE_BINARY_DIR}/Info.plist
    @ONLY
)

# --- Add the plugin target ---
if(APPLE)
    add_library(AEPathLink MODULE ${SOURCES})

    set_source_files_properties(${SOURCES} PROPERTIES LANGUAGE OBJCXX)

    set(AE_CONFIGURATION "$<IF:$<CONFIG:Debug>,debug,release>")
    set_target_properties(AEPathLink PROPERTIES
        BUNDLE TRUE
        BUNDLE_EXTENSION "plugin"
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_BINARY_DIR}/Info.plist
        MACOSX_BUNDLE_BUNDLE_NAME "${PRODUCT_NAME}"
        MACOSX_BUNDLE_IDENTIFIER "${AE_BUNDLE_PREFIX}.plugins.${PRODUCT_NAME}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${AE_MAJOR_VERSION}.${AE_MINOR_VERSION}.${AE_REVISION_VERSION}"
        MACOSX_BUNDLE_BUNDLE_VERSION "${BUILD_NUMBER}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/output/mac/${AE_CONFIGURATION}"
        LIBRARY_OUTPUT_NAME "AEPathLink"
        PREFIX ""
        SUFFIX ""
        XCODE_ATTRIBUTE_INFOPLIST_PREPROCESS "YES"
        XCODE_ATTRIBUTE_WRAPPER_EXTENSION "plugin"
        XCODE_ATTRIBUTE_MACH_O_TYPE "mh_bundle"
        XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "${AE_BUNDLE_PREFIX}.plugins.${PRODUCT_NAME}"
    )

    # --- Link frameworks ---
    target_link_libraries(AEPathLink
        "-framework Cocoa"
        "-framework QuartzCore"
        "-framework CoreFoundation"
        "-framework Foundation"
        "-framework ApplicationServices"
    )

    # --- Compiler options ---
    target_compile_options(AEPathLink PRIVATE
        -fobjc-arc
        -x objective-c++
        -std=c++17
        -Wno-deprecated-declarations
    )

    # --- Compile .r resource file into .rsrc (then copy into bundle) ---
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

    # --- Create PkgInfo for legacy bundle recognition ---
    add_custom_command(TARGET AEPathLink POST_BUILD
        COMMAND printf "AEgp????" > "$<TARGET_BUNDLE_CONTENT_DIR:AEPathLink>/PkgInfo"
        COMMENT "Creating PkgInfo (AEgp????) for AE compatibility"
    )

    # --- Verify PkgInfo (debugging) ---
    add_custom_command(TARGET AEPathLink POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "=== Verifying PkgInfo ==="
        COMMAND ls -la "$<TARGET_BUNDLE_CONTENT_DIR:AEPathLink>/PkgInfo"
        COMMAND stat -f%z "$<TARGET_BUNDLE_CONTENT_DIR:AEPathLink>/PkgInfo"  # Should be 8 bytes
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Debug: Check PkgInfo size"
    )

endif()
