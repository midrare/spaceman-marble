# Guidelines:
# - assume project will be included into a larger hierarchy of projects with
#   many other tests
#   - use unique test names (reduce chance of name clash)
#   - parent project should control whether to add tests at all. default
#     behavior: only add tests if there is no parent project
#   - add project-specific LABELS property to each test


# ======================================================================
# ------------------------------ INIT ----------------------------------
# ======================================================================

include_guard()

include(CMakePrintHelpers)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmaux")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../cmake")

find_package(Catch2 "3.7.1" REQUIRED CONFIG)

set_directory_properties(PROPERTIES LABELS "mousedriver")

include_directories("${mousedriver_SOURCE_DIR}/src")
link_libraries(
  Catch2::Catch2WithMain
  Microsoft.GSL::GSL
  $<$<BOOL:${MINGW}>:ws2_32>
  $<$<BOOL:${WIN32}>:dwmapi>
  $<$<BOOL:${WIN32}>:Msimg32>
)

if (NOT DEFINED CMAKE_CXX_VISIBILITY_PRESET)
  set(CMAKE_CXX_VISIBILITY_PRESET hidden)
endif()
if (NOT DEFINED CMAKE_VISIBILITY_INLINES_HIDDEN)
  set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
endif ()

# do not import min() max() macros from windows.h
add_compile_definitions(NOMINMAX)

# make winapi prefer unicode functions
# https://learn.microsoft.com/en-us/windows/win32/intl/conventions-for-function-prototypes
add_compile_definitions(UNICODE)




# ======================================================================
# ------------------------------ TARGETS -------------------------------
# ======================================================================


include(CTest)
include(Catch)

