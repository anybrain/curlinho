project(deeep-tests)

set(CMAKE_CXX_STANDARD 11)

include_directories(../lib/include ../lib/dependencies/include dependencies)

option(TEST_HTTP "Test HTTP" OFF)

if(TEST_METRICS)
    file(GLOB_RECURSE SOURCE_FILES src/metrics/*.cpp)
endif()

if(TEST_STATISTICS)
    file(GLOB_RECURSE SOURCE_FILES src/statistics/*.cpp)
endif()

if(TEST_HTTP)
    file(GLOB_RECURSE SOURCE_FILES src/http/*.cpp)
endif()

message(STATUS "..Building tests          (-DBUILD_TESTS=OFF to disable)\n")
add_executable(${PROJECT_NAME} ${SOURCE_FILES} src/TestCase-00.cpp)

target_link_libraries(${PROJECT_NAME} ${CURLINHO_LIBRARIES})
