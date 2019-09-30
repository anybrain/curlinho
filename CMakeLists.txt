cmake_minimum_required(VERSION 3.1)
project(curlinho VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)

###### Options you can change! ##################
## Atention: Only build one type of lib (static OR dynamic)
option(BUILD_SHARED_LIB "BUILD_SHARED" OFF)
option(BUILD_STATIC_LIB "BUILD_STATIC" ON)
option(BUILD_EXAMPLES "Build Examples" OFF)
option(BUILD_TESTS "Build Tests" OFF)
#################################################

############ Curlinho variables #################
## use them in your project cmakelists
## see examples cmakelist
set(CURLINHO_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/include
    CACHE INTERNAL "curlinho include dirs" FORCE)

set(CURLINHO_LIBRARIES curlinho CACHE INTERNAL "curlinho library" FORCE)
set(CURLINHO_LIBRARY_DIRS "" CACHE INTERNAL "curlinho lib dirs" FORCE)
set(CURLINHO_BINARY_DIRS "" CACHE INTERNAL "curlinho binary dirs" FORCE)
##################################################

## Enable warnings.
if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -pedantic -Wshadow -Wno-variadic-macros")
endif ()

## Find source files for each platform.
file(GLOB_RECURSE SOURCE_FILES src/*.cpp include/*.h)
include_directories(${CURLINHO_INCLUDE_DIRS})

## Build STATIC or SHARED library.
if (BUILD_SHARED_LIB)
    message(STATUS "..Building dynamically (-DBUILD_SHARED_LIB=OFF to disable)")
    add_library(${CURLINHO_LIBRARIES} SHARED ${SOURCE_FILES})
elseif(BUILD_STATIC_LIB)
    message(STATUS "..Building statically (-DBUILD_STATIC_LIB=OFF to disable)")
    add_library(${CURLINHO_LIBRARIES} STATIC ${SOURCE_FILES})
endif ()
set(CURLINHO_LIBRARY_DIRS
        ${CURLINHO_LIBRARY_DIRS}
        ${CMAKE_CURRENT_BINARY_DIR}/$<TARGET_FILE_NAME:${CURLINHO_LIBRARIES}>
        CACHE INTERNAL "curlinho lib dirs" FORCE)
add_definitions(-DCURL_STATICLIB)
set_target_properties(${CURLINHO_LIBRARIES} PROPERTIES
        DEBUG_POSTFIX -debug)

## include dependencies based on enviornment
IF (APPLE)
    find_package(CURL REQUIRED)
    target_link_libraries(${CURLINHO_LIBRARIES}
            ${CURL_LIBRARIES})
ENDIF()

IF (UNIX AND NOT APPLE)
    add_definitions(-D_LARGEFILE64_SOURCE)
    target_link_libraries (${CURLINHO_LIBRARIES} curl)
ENDIF ()

IF (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -DUNICODE -D_UNICODE)
    target_link_libraries(${CURLINHO_LIBRARIES} ws2_32.lib wldap32.lib crypt32.lib normaliz.lib)
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
        target_link_libraries(${CURLINHO_LIBRARIES}
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x86/libcurl.lib
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x86/libssl.lib
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x86/libcrypto.lib
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x86/nghttp2_static.lib
                )
        set(CURLINHO_LIBRARY_DIRS
                ${CURLINHO_LIBRARY_DIRS}
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x86/libcurl.lib
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x86/libssl.lib
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x86/libcrypto.lib
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x86/nghttp2_static.lib
                CACHE INTERNAL "curlinho lib dirs" FORCE)
    endif()
ENDIF()

## Library installation directory.
if (NOT DEFINED CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR lib)
endif (NOT DEFINED CMAKE_INSTALL_LIBDIR)
set(LIBDIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})

install(
        DIRECTORY "include"
        DESTINATION "include"
        FILES_MATCHING PATTERN "*.h"
)
install(
        TARGETS ${CURLINHO_LIBRARIES}
        LIBRARY DESTINATION "${LIBDIR}"
        ARCHIVE DESTINATION "${LIBDIR}"
        RUNTIME DESTINATION "bin"
)

####### BUILD TESTS ##########
if(BUILD_TESTS)
    #TODO: Add tests
    #add_subdirectory(test)
endif(BUILD_TESTS)

####### BUILD EXAMPLES #######
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif(BUILD_EXAMPLES)
