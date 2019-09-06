project(deeep-examples)

message(STATUS "..Building examples       (-DBUILD_EXAMPLES=OFF to disable)")
if (WIN32)
    add_subdirectory(windows)
elseif (APPLE)
    add_subdirectory(mac)
endif ()
