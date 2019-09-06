project(mac-sample)

set(CMAKE_CXX_STANDARD 11)

include_directories(${CURLINHO_INCLUDE_DIRS})
message(${CURLINHO_INCLUDE_DIRS})

file(GLOB_RECURSE SOURCE_FILES *.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} ${CURLINHO_LIBRARIES})
