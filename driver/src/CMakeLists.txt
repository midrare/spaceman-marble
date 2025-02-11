# ======================================================================
# ------------------------------ INIT ----------------------------------
# ======================================================================

include_guard()

include(CMakePrintHelpers)
include(GNUInstallDirs)

include(AddCompilerFlags)
include(AddMissingDLLs)
include(ExportCompileCommands)
include(ExportCompileFlags)
include(FileMisc)

export_compile_commands()
export_compile_flags()



# ======================================================================
# ------------------------------ TARGETS -------------------------------
# ======================================================================

add_executable(
  mousedriver_main WIN32 MACOSX_BUNDLE
  main.cpp
)
add_executable(mousedriver::main ALIAS mousedriver_main)

# RELEASE update the version info here
set_target_properties(
  mousedriver_main
  PROPERTIES
    CXX_STANDARD 20
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
    OUTPUT_NAME "mousedriver"
    TITLE "mousedriver"
    VERSION "0.1.0"
    FILE_PATH_COMPONENT "mousedriver"
    SOVERSION 1
    BUILD_RPATH_USE_ORIGIN ON
    MACOSX_RPATH ON
    MACOSX_BUNDLE ON
    MACOSX_BUNDLE_ICON_FILE "app.icns"
    MACOSX_BUNDLE_BUNDLE_NAME "mousedriver"
    MACOSX_BUNDLE_BUNDLE_VERSION "${BUILD_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "0.1.0"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.midrare.mousedriver"
    MACOSX_BUNDLE_COPYRIGHT "© midrare"
    OSX_DEPLOYMENT_TARGET "12.0"
)

target_include_directories(
  mousedriver_main
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
         $<INSTALL_INTERFACE:include/mousedriver-$<TARGET_PROPERTY:VERSION>>)

target_link_libraries(
  mousedriver_main
  PRIVATE
    $<$<BOOL:${MINGW}>:ws2_32>
    $<$<BOOL:${WIN32}>:dwmapi>
    $<$<BOOL:${WIN32}>:Msimg32>
    hidapi::hidapi hidapi::winapi hidapi::include
    Microsoft.GSL::GSL
    sago::platform_folders
    # wx::core
    # wx::base
  )

target_include_directories(mousedriver_main PRIVATE ${SCOPE_GUARD_INCLUDE_DIRS})
target_include_directories(mousedriver_main PRIVATE ${DBG_MACRO_INCLUDE_DIRS})

add_compiler_flags_from_file(TARGET mousedriver_main)

# do not import min() max() macros from windows.h
target_compile_definitions(mousedriver_main PRIVATE NOMINMAX)

# make winapi prefer unicode functions
# https://learn.microsoft.com/en-us/windows/win32/intl/conventions-for-function-prototypes
target_compile_definitions(mousedriver_main PRIVATE UNICODE)

# disable https://github.com/sharkdp/dbg-macro if not building in debug mode
get_property(DEBUG_CONFIGS GLOBAL PROPERTY DEBUG_CONFIGURATIONS)
list(APPEND DEBUG_CONFIGS "Debug" "RelWithDebInfo")
list(JOIN DEBUG_CONFIGS "," DEBUG_CONFIGS_COMMA)
target_compile_definitions(mousedriver_main PRIVATE
  "$<$<NOT:$<CONFIG:${DEBUG_CONFIGS_COMMA}>>:DBG_MACRO_DISABLE>")

configure_version_file(TARGET mousedriver_main FILE "version.cpp.in")




# ======================================================================
# ---------------------------- DEPLOYMENT ------------------------------
# ======================================================================

install(
  TARGETS mousedriver_main
  RUNTIME ARCHIVE LIBRARY FRAMEWORK BUNDLE PUBLIC_HEADER RESOURCE
  DESTINATION "$<TARGET_FILE_DIR:mousedriver_main>"
)

install(FILES $<TARGET_RUNTIME_DLLS:mousedriver_main> TYPE BIN)
install(TARGETS mousedriver_main BUNDLE DESTINATION .)

# add_missing_dlls_from_library(
#   LIBRARY_TARGET wx::core
#   RELEASE_DLLS
#     "jpeg62.dll" "tiff.dll" "liblzma.dll" "libpng16.dll"
#     "pcre2-16.dll" "zlib1.dll"
#   DEBUG_DLLS
#     "jpeg62.dll" "tiffd.dll" "liblzma.dll" "libpng16d.dll"
#     "pcre2-16d.dll" "zlibd1.dll"
# )
