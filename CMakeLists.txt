# cmake --build . --config Release

cmake_minimum_required(VERSION 3.16)
project(rtklog)

set(CMAKE_CXX_STANDARD 17)

add_executable(rtklog
    rtklog.cpp
    )

if(WIN32)
    target_link_libraries(rtklog Winmm.lib)
else()
    target_link_libraries(rtklog m)
endif()