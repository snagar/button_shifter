cmake_minimum_required(VERSION 3.20)

### https://cmake.org/cmake/help/latest/manual/cmake-commands.7.html
### Example1: https://github.com/krux02/minimal_cmake_example/blob/master/CMakeLists.txt
### Example2: https://github.com/sparker256/imgui4xp/blob/master/src/CMakeLists.txt
### https://cmake.org/cmake/help/latest/command/list.html
### https://cmake.org/cmake/help/latest/command/add_library.html
### https://stackoverflow.com/questions/28597351/how-do-i-add-a-library-path-in-cmake
### https://stackoverflow.com/questions/61029105/cmake-add-library-cannot-create-target-cxx-because-another-target-with-the-s

####### MacOS Related #########
##  https://developer.apple.com/documentation/apple-silicon/porting-your-macos-apps-to-apple-silicon
## https://cmake.org/cmake/help/latest/prop_tgt/ENABLE_EXPORTS.html

project(button_shifter LANGUAGES C CXX VERSION 0.25.10 DESCRIPTION "Button Shifter X-Plane plugin v25.x")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_VERBOSE_MAKEFILE ON)


if (UNIX)
  # Find the operating system name
  execute_process(COMMAND lsb_release -si OUTPUT_VARIABLE OS_NAME OUTPUT_STRIP_TRAILING_WHITESPACE)

  # Find the operating system version ID
  execute_process(COMMAND lsb_release -sr OUTPUT_VARIABLE OS_VERSION_ID OUTPUT_STRIP_TRAILING_WHITESPACE)
endif ()


# Return the date (yyyy-mm-dd)
macro(DATE RESULT)
  if(WIN32)
    execute_process(COMMAND "cmd" " /C date /T" OUTPUT_VARIABLE ${RESULT})
    string(REGEX REPLACE "(..)/(..)/(....).*" "\\3-\\2-\\1" ${RESULT} ${${RESULT}})
  elseif(UNIX)
    execute_process(COMMAND "date" "+%Y-%m-%d" OUTPUT_VARIABLE ${RESULT})
  else()
    message(SEND_ERROR "Unable to detect date")
    set(${RESULT} UNKNOWN)
  endif()
endmacro()

# Return the time (hh:mm:ss)
macro(TIME RESULT)
  if(WIN32)
    execute_process(COMMAND "cmd" " /C echo %TIME%" OUTPUT_VARIABLE ${RESULT})
    string(REGEX REPLACE "(..:..:..),(..)" "\\1" ${RESULT} ${${RESULT}})
  elseif(UNIX)
    execute_process(COMMAND "date" "+%H:%M:%S" OUTPUT_VARIABLE ${RESULT})
  else()
    message(SEND_ERROR "Unable to detect time")
    set(${RESULT} UNKNOWN)
  endif()
endmacro()



if (LINUX)
  set(CMAKE_PREFIX_PATH "/usr/bin")
elseif(APPLE)
  set(CMAKE_OSX_ARCHITECTURES arm64;x86_64) # https://stackoverflow.com/questions/65157483/macos-build-universal-binary-2-with-cmake
  set(CMAKE_MACOSX_RPATH 1)
endif()


string (TOLOWER ${CMAKE_BUILD_TYPE} BUILD_TYPE_LOWERCASE) ## store the build type always as lower case and use it through out the cmake file

include_directories("${CMAKE_SOURCE_DIR}/libs/SDK/CHeaders/XPLM")
include_directories("${CMAKE_SOURCE_DIR}/libs/SDK/CHeaders/Widgets")

if (LINUX)
  # ubuntu 22.04
  include_directories("/usr/include/x86_64-linux-gnu/")
endif()

set(BASE_SRC
    src/plugin.cpp
    src/plugin.h
        src/xshifter.cpp
        src/xshifter.h
        src/utils.cpp
        src/utils.h
        src/xx_share_config.h
        src/state.cpp
        src/state.h
        src/Timer.cpp
        src/Timer.h
)


## Mission-X source files
add_library(${PROJECT_NAME} SHARED
    ${BASE_SRC}
)

#################################
### Compiler Directives
#################################

#if (NOT WIN32)
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)
#endif()

if (APPLE)
  # We will aim to MacOS version 11.0 and up
  target_compile_options(${PROJECT_NAME} PUBLIC -mmacosx-version-min=11.0)
  target_link_libraries(${PROJECT_NAME} -mmacosx-version-min=11.0)
endif ()



# Enable all X-Plane SDK APIs up to the newest version.
add_definitions(-DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1 -DXPLM302=1 -DXPLM303=1 -DXPLM_DEPRECATED)

# Define platform macros.
if (WIN32)
  add_definitions(-DIBM=1 -D_WIN32 -D_CONSOLE -DWIN32 -D_WINDOWS  -D_WINDLL -D_MBCS -D_AFXDLL -DFMT_UNICODE=0) ## IBM=1 the "FMT_UNICODE=0" is needed to solve FMT compilation from CMake, not relevant from MSVC.
elseif(APPLE)
  add_definitions(-DMAC=1 -DAPL=1 -DGL_SILENCE_DEPRECATION) ## APL=1 MAC=1 GL_SILENCE_DEPRECATION=remove mac warning regarding GL deprecation since 10.14
elseif (UNIX)
  add_definitions(-DLIN=1) ## LIN=1
endif()

#if (NOT APPLE)
#  #  add_definitions(-DCPPHTTPLIB_OPENSSL_SUPPORT )  ## deprecated, using MacOS secure library
#  add_definitions(-DGLEW_BUILD) ## hopefully will resolve warning C4273, based on the  libs/imgui4xp/GL/glew.h macro
#endif()

## Enable other Mission-X flags
#add_definitions(-DIMGUI_INCLUDE_IMCONFIG_H -DIMGUI_IMPL_OPENGL_LOADER_GLEW -DMB_DOUBLE_FLOAT)
#add_definitions(-Dglm_shared_EXPORTS -DFT2_BUILD_LIBRARY -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_WARNINGS)
#add_definitions(-DENABLE_EXPERIMENTAL_DISABLE)


if ( ${BUILD_TYPE_LOWERCASE} STREQUAL "debug" )
  message("We are in debug - adding debug definition to compiler !!!!")
  add_definitions(-DDEBUG)
elseif ( ${BUILD_TYPE_LOWERCASE} STREQUAL "release" )
  add_definitions(-DRELEASE)
endif()

# Always use position-independent code and highest optimization level (FPS!).
# Optimisation options
if ( ${BUILD_TYPE_LOWERCASE} STREQUAL "debug" )
  message ("Adding debug optimization -O0")
  add_compile_options(-O0)
  add_compile_options(-g3)
else()
  message ("Adding release optimization -O3")
  add_compile_options(-O3)
  add_compile_options(-g0)
endif()

if ( NOT MSVC )

  # Force-enable exception support. This is most likely redundant, although for C
  # code the default is the opposite. Since we are mixing C++ and C libraries,
  # safer to set it on?
  #    add_compile_options(-fexceptions -fpermissive) # v3.305.3 add permissive flag
  add_compile_options(-fexceptions)
  # On UNIX systems this makes symbols non-exported by default. On Windows this
  # option is simply ignored, since symbol visibility works differently there.
  add_compile_options(-fvisibility=hidden)

  add_compile_options(-c -fmessage-length=0 -Wunused-function -Wno-narrowing)

  ## Add -fpermissive only for CXX compiler and not C
  target_compile_options(${PROJECT_NAME}  PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-fpermissive> )

  # Enable stricter warnings and then disable some we are not interested in.
  #    add_compile_options(-Wall -Wpedantic -Wshadow -Wextra)
  add_compile_options(-Wall -Wpedantic -Wextra)
  add_compile_options(-Wno-unused)

  if (NOT WIN32)
    add_compile_options(-fPIC)
  endif()
endif ()





#################################
### Linker Directives
#################################

## Link Third party libraries
### Specify library search locations.
if (WIN32)
  message(">> Windows Libraries Path")
  list(APPEND CMAKE_LIBRARY_PATH "${CMAKE_SOURCE_DIR}/libs/SDK/Libraries/Win ")
elseif (APPLE)
  message(">> Apple Libraries Path")
  list(APPEND CMAKE_FRAMEWORK_PATH "${CMAKE_SOURCE_DIR}/libs/SDK/Libraries/Mac")
elseif (UNIX)
  message(">> Linux Libraries Path")

  list(APPEND CMAKE_LIBRARY_PATH "/lib/x86_64-linux-gnu") ## v24.06.1  Mint/Ubuntu ub 22.04, mint 21.x
  list(APPEND CMAKE_LIBRARY_PATH "/usr/lib/x86_64-linux-gnu") ## v24.06.1  Mint/Ubuntu ub 22.04, mint 21.x
  list(APPEND CMAKE_LIBRARY_PATH "/usr/local/lib64")
  list(APPEND CMAKE_LIBRARY_PATH "/usr/local/lib")
  list(APPEND CMAKE_LIBRARY_PATH "/usr/lib64")
  list(APPEND CMAKE_LIBRARY_PATH "/usr/lib")
  list(APPEND CMAKE_LIBRARY_PATH "/lib64") ## v24.06.1 Fedora

endif ()


# ==========================
# X-Plane SDK
# ==========================
if (WIN32 OR APPLE)
  if (WIN32)
    add_library(XPLM::XPLM SHARED IMPORTED)
    set_target_properties(XPLM::XPLM PROPERTIES
            IMPORTED_IMPLIB "${CMAKE_SOURCE_DIR}/libs/SDK/Libraries/Win/XPLM_64.lib"
            INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/libs/SDK/CHeaders/XPLM"
    )

    add_library(XPLM::XPWidgets SHARED IMPORTED)
    set_target_properties(XPLM::XPWidgets PROPERTIES
            IMPORTED_IMPLIB "${CMAKE_SOURCE_DIR}/libs/SDK/Libraries/Win/XPWidgets_64.lib"
            INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/libs/SDK/CHeaders/Widgets"
    )
  elseif(APPLE)
    # XPLM framework
    add_library(XPLM::XPLM INTERFACE IMPORTED)
    set_target_properties(XPLM::XPLM PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/libs/SDK/CHeaders/XPLM"
            INTERFACE_LINK_LIBRARIES "${CMAKE_SOURCE_DIR}/libs/SDK/Libraries/Mac/XPLM.framework"
    )

    # XPWidgets framework
    add_library(XPLM::XPWidgets INTERFACE IMPORTED)
    set_target_properties(XPLM::XPWidgets PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/libs/SDK/CHeaders/Widgets"
            INTERFACE_LINK_LIBRARIES "${CMAKE_SOURCE_DIR}/libs/SDK/Libraries/Mac/XPWidgets.framework"
    )
  elseif (UNIX)
    add_library(XPLM::XPLM SHARED IMPORTED)
    set_target_properties(XPLM::XPLM PROPERTIES
            IMPORTED_IMPLIB "${CMAKE_SOURCE_DIR}/libs/SDK/Libraries/Lin/XPLM_64.so"
            INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/libs/SDK/CHeaders/XPLM"
    )

    add_library(XPLM::XPWidgets SHARED IMPORTED)
    set_target_properties(XPLM::XPWidgets PROPERTIES
            IMPORTED_IMPLIB "${CMAKE_SOURCE_DIR}/libs/SDK/Libraries/Lin/XPWidgets_64.so"
            INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_SOURCE_DIR}/libs/SDK/CHeaders/Widgets"
    )
  endif()  # end APPLE

endif()

# Linking by OS
if (WIN32)
  if (MSVC)
    message(">> MSVC linking.")
    target_link_libraries(${PROJECT_NAME} PRIVATE
            XPLM::XPLM
            XPLM::XPWidgets
    )
  else()
    message(">> non-msvc linking.")
    target_link_libraries(${PROJECT_NAME} PRIVATE -static-libgcc -static-libstdc++ -static)
  endif()

elseif (APPLE)
  message(">> Apple linking.")
  target_link_libraries(${PROJECT_NAME} PRIVATE stdc++ ) # or libc++ on macOS/Clang
  target_link_libraries(${PROJECT_NAME} PRIVATE
          "-v -exported_symbols_list ${CMAKE_SOURCE_DIR}/build/${PROJECT_NAME}.sym_mac"
          "-Wl,-rpath,./Resources/plugins/button_shifter/"
          "-Wno-c++11-narrowing"
          "-framework ApplicationServices"
          "-framework Foundation"
          "-framework Security"
          XPLM::XPLM
          XPLM::XPWidgets
  )
elseif (UNIX)
  message(">> Unix linking.")
  target_link_libraries(${PROJECT_NAME} PRIVATE
          "-Wl,--version-script=${CMAKE_SOURCE_DIR}/build/${PROJECT_NAME}.sym"
          "-Wl,-rpath,./Resources/dlls/64"
          "-Wl,-rpath,./Resources/dlls/64/gl"
          "-Wl,-rpath,/usr/local/lib64"
          "-Wl,-rpath,/usr/local/lib"
          "-Wl,-rpath,/usr/lib64"
          "-Wl,-rpath,/usr/lib"
          "-Wl,-rpath,/lib64"
          "-Wl,-rpath,/lib/x86_64-linux-gnu"
          "-Wl,-rpath,/usr/lib/x86_64-linux-gnu"
          "-Wl,-rpath,./Resources/plugins/button_shifter/"
  )
endif()


target_compile_definitions(${PROJECT_NAME} PRIVATE BUTTON_SHIFTER_LIBRARY)

set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "")
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".xpl")

############# DEBUG ##############
message("PROJECT_NAME: " ${PROJECT_NAME})
if (UNIX)
  message("OS_NAME:" ${OS_NAME})
  message("OS_VERSION_ID:" ${OS_VERSION_ID})
endif ()
#message("CMAKE_COMMAND: " ${CMAKE_COMMAND})
#message("CMAKE_BUILD_TYPE: " ${CMAKE_BUILD_TYPE}, "Local BUILD_TYPE_LOWERCASE: " ${BUILD_TYPE_LOWERCASE})
message("CMAKE_LIBRARY_PATH: " ${CMAKE_LIBRARY_PATH} )

message("Button Shifter: CMAKE_CURRENT_BINARY_DIR: " ${CMAKE_CURRENT_BINARY_DIR})




