# ======================================================================
# ------------------------------ INIT ----------------------------------
# ======================================================================

cmake_minimum_required(VERSION 3.27)

# all vcpkg settings must be set before first project() call
# https://vcpkg.io/en/docs/users/buildsystems/cmake-integration.html#settings-reference

set(mousedriver_DEFAULT_BUILD_TESTS OFF)
if(CMAKE_SOURCE_DIR PATH_EQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(mousedriver_DEFAULT_BUILD_TESTS ON)
endif()

option(mousedriver_BUILD_TESTS "Build tests for mousedriver."
       "${mousedriver_DEFAULT_BUILD_TESTS}")
if(mousedriver_BUILD_TESTS)
  list(APPEND VCPKG_MANIFEST_FEATURES "tests")
endif()


# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()



# ======================================================================
# ----------------------- PROJECT-WIDE SETUP ---------------------------
# ======================================================================

project("mousedriver" VERSION "0.1.0" LANGUAGES C CXX RC)
enable_testing()

if($ENV{CLION_IDE})
  set(CMAKE_COLOR_DIAGNOSTICS ON)
endif()

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


include(CMakePrintHelpers)
include(CTest)
include(GNUInstallDirs)
include(FetchContent)


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmaux")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# NOTE if you add a custom CMAKE_BUILD_TYPE, make sure to update the global
#   property DEBUG_CONFIGURATIONS and MAP_IMPORTED_CONFIG_XXX for
#   locally-created targets




# ======================================================================
# --------------------------- DEPENDENCIES -----------------------------
# ======================================================================

find_package(Microsoft.GSL "4.1.0" REQUIRED CONFIG)
find_package(platform_folders "4.2.0" CONFIG REQUIRED)
find_package(spdlog "1.15.0" REQUIRED CONFIG)
find_package(utf8cpp "4.0.4" REQUIRED CONFIG)
# find_package(wxWidgets "3.2.6" CONFIG REQUIRED)
find_package(hidapi CONFIG REQUIRED)
find_path(SCOPE_GUARD_INCLUDE_DIRS "scope_guard.hpp")
find_path(DBG_MACRO_INCLUDE_DIRS "dbg.h" PATH_SUFFIXES "dbg-macro")




# ======================================================================
# ------------------------  MAIN BUILD TARGETS -------------------------
# ======================================================================

if(PROJECT_SOURCE_DIR PATH_EQUAL PROJECT_BINARY_DIR)
  message(FATAL_ERROR "In-source builds are not allowed.")
endif()

# see: Professional CMake (2022) Ch. 27.1.1
set(CMAKE_INSTALL_DOCDIR "${CMAKE_INSTALL_DATAROOTDIR}/doc/${PROJECT_NAME}")

# see: Professional CMake (2022) Ch. 27.1.2
if(NOT WIN32 AND CMAKE_SOURCE_DIR PATH_EQUAL CMAKE_CURRENT_SOURCE_DIR)
  set(CMAKE_INSTALL_PREFIX "/opt/${PROJECT_NAME}")
endif()

add_subdirectory(src)




# ======================================================================
# ----------------------------- TESTS -----------------------------------
# ======================================================================

if(mousedriver_BUILD_TESTS)
  add_subdirectory(tests)
endif()




# ======================================================================
# ---------------------------- PACKAGING --------------------------------
# ======================================================================

option(mousedriver_BUILD_PACKAGE "Package binaries for mousedriver."
       "${PROJECT_IS_TOP_LEVEL}")
if(mousedriver_BUILD_PACKAGE)
  add_subdirectory(packaging)
endif()


# vim:noet:ft=cmake:
