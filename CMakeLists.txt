cmake_minimum_required(VERSION 3.1)
project(curlinho VERSION 0.0.1 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 11)

###### Options you can change! ##################
option(BUILD_STATIC_LIB "Build static lib" OFF)
option(BUILD_RELEASE_DEBUG "Build release with debug info" OFF)
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
set(CURLINHO_OUTPUT_DIR ${OUTPUT_DIR})
set(CURLINHO_PLATFORM ${PLATFORM})
##################################################

## Enable warnings.
if (CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Werror -pedantic -Wshadow -Wno-variadic-macros")
endif ()

if (MSVC)
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4 /WX" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()
endif()

## Find source files for each platform.
file(GLOB_RECURSE SOURCE_FILES src/*.cpp include/*.h)
include_directories(${CURLINHO_INCLUDE_DIRS})

## Build STATIC library.
message(STATUS "Building curlinho static lib")
add_library(${CURLINHO_LIBRARIES} STATIC ${SOURCE_FILES})
set(CURLINHO_LIBRARY_DIRS ${CURLINHO_LIBRARY_DIRS}
        ${CURLINHO_OUTPUT_DIR}/lib/$<TARGET_LINKER_FILE_NAME:${CURLINHO_LIBRARIES}>
        CACHE INTERNAL "curlinho lib dirs" FORCE)
add_definitions(-DCURL_STATICLIB)

if(CMAKE_SYSTEM_NAME STREQUAL Android)
    target_link_libraries(${CURLINHO_LIBRARIES}
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/ANDROID/${ANDROID_ABI}/libcurl.a
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/ANDROID/${ANDROID_ABI}/libnghttp2.a
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/ANDROID/${ANDROID_ABI}/libssl.a
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/ANDROID/${ANDROID_ABI}/libcrypto.a
        ${CURL_LIBRARIES})

        set(CURLINHO_LIBRARY_DIRS ${CURLINHO_LIBRARY_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/ANDROID/${ANDROID_ABI}/libcurl.a
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/ANDROID/${ANDROID_ABI}/libnghttp2.a
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/ANDROID/${ANDROID_ABI}/libssl.a
        ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/ANDROID/${ANDROID_ABI}/libcrypto.a
        CACHE INTERNAL "curlinho lib dirs" FORCE)
elseif(APPLE)
    if(CMAKE_SYSTEM_NAME STREQUAL iOS)
        if(${IS_SIMULATOR})
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mios-simulator-version-min=6.0")
        endif()

        find_library(CORE_MOTION CoreMotion)
        find_library(CORE_MOTION CoreMotion)
        find_library(UI_KIT UIKit)
        target_link_libraries(${CURLINHO_LIBRARIES}
            ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/IOS/${PLATFORM}/libcrypto.a
            ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/IOS/${PLATFORM}/libssl.a
            ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/IOS/${PLATFORM}/libcurl.a
            ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/IOS/${PLATFORM}/libnghttp2.a
            z # libz
            ${UI_KIT} ${CORE_MOTION})

        set(CURLINHO_LIBRARY_DIRS ${CURLINHO_LIBRARY_DIRS}
            ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/IOS/${PLATFORM}/libcrypto.a
            ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/IOS/${PLATFORM}/libssl.a
            ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/IOS/${PLATFORM}/libcurl.a
            ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/IOS/${PLATFORM}/libnghttp2.a
            ${UI_KIT} ${CORE_MOTION}
            CACHE INTERNAL "curlinho lib dirs" FORCE)

    else()
        if(CMAKE_OSX_ARCHITECTURES STREQUAL "arm64")
            target_link_libraries(${CURLINHO_LIBRARIES}
                    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/OSX/arm64/libcrypto.a
                    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/OSX/arm64/libssl.a
                    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/OSX/arm64/libcurl.a
                    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/OSX/arm64/libnghttp2.a
                    ${CURL_LIBRARIES})

            set(CURLINHO_LIBRARY_DIRS ${CURLINHO_LIBRARY_DIRS}
                    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/OSX/arm64/libcrypto.a
                    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/OSX/arm64/libssl.a
                    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/OSX/arm64/libcurl.a
                    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/OSX/arm64/libnghttp2.a
                    CACHE INTERNAL "curlinho lib dirs" FORCE)
        else()
            target_link_libraries(${CURLINHO_LIBRARIES}
                    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/OSX/libcrypto.a
                    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/OSX/libssl.a
                    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/OSX/libcurl.a
                    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/OSX/libnghttp2.a
                    ${CURL_LIBRARIES})

            set(CURLINHO_LIBRARY_DIRS ${CURLINHO_LIBRARY_DIRS}
                    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/OSX/libcrypto.a
                    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/OSX/libssl.a
                    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/OSX/libcurl.a
                    ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/OSX/libnghttp2.a
                    CACHE INTERNAL "curlinho lib dirs" FORCE)
        endif()
    endif()
elseif(MSVC)
    if(BUILD_STATIC_LIB)
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D_ITERATOR_DEBUG_LEVEL=2")
        if(BUILD_RELEASE_DEBUG)
            set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /D_ITERATOR_DEBUG_LEVEL=2")
        else()
            set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /D_ITERATOR_DEBUG_LEVEL=0")
        endif()
        message(STATUS "Building curlinho with MT flag")
    else()
        set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd /D_ITERATOR_DEBUG_LEVEL=2")
        if(BUILD_RELEASE_DEBUG)
            set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MDd /D_ITERATOR_DEBUG_LEVEL=2")
        else()
            set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /D_ITERATOR_DEBUG_LEVEL=0")
        endif()
        message(STATUS "Building curlinho with MD flag")
    endif()

    add_definitions(-DUNICODE -D_UNICODE -DASSEMBLYDEBUG -DCURL_STATICLIB)
    target_link_libraries(${CURLINHO_LIBRARIES} ws2_32.lib wldap32.lib crypt32.lib normaliz.lib)
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
        target_link_libraries(${CURLINHO_LIBRARIES}
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x86/libcurl.lib
				${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x86/nghttp2.lib
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x86/zlib.lib
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x86/libcrypto.lib
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x86/libssl.lib
                )
        set(CURLINHO_LIBRARY_DIRS ${CURLINHO_LIBRARY_DIRS}
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x86/libcurl.lib
				${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x86/nghttp2.lib
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x86/zlib.lib
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x86/libcrypto.lib
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x86/libssl.lib
                CACHE INTERNAL "curlinho lib dirs" FORCE)
        set(CURLINHO_BINARY_DIRS ${CURLINHO_BINARY_DIRS}
                CACHE INTERNAL "curlinho binary dirs" FORCE)
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
        target_link_libraries(${CURLINHO_LIBRARIES}
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x64/libcurl.lib
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x64/nghttp2.lib
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x64/zlib.lib
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x64/libcrypto.lib
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x64/libssl.lib
                )
        set(CURLINHO_LIBRARY_DIRS ${CURLINHO_LIBRARY_DIRS}
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x64/libcurl.lib
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x64/nghttp2.lib
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x64/zlib.lib
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x64/libcrypto.lib
                ${CMAKE_CURRENT_SOURCE_DIR}/dependencies/libs/MSVC/x64/libssl.lib
                CACHE INTERNAL "curlinho lib dirs" FORCE)
        set(CURLINHO_BINARY_DIRS ${CURLINHO_BINARY_DIRS}
                CACHE INTERNAL "curlinho binary dirs" FORCE)
    endif()
elseif(UNIX AND NOT APPLE)
    add_definitions(-D_LARGEFILE64_SOURCE)
    target_link_libraries (${CURLINHO_LIBRARIES} curl)
endif()

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

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif(BUILD_EXAMPLES)
