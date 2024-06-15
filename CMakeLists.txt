cmake_minimum_required (VERSION 3.8)

if(WIN32)
	set(CMAKE_PREFIX_PATH "C:/dev/vcpkg/installed/x64-windows/share")
	set(CMAKE_TOOLCHAIN_FILE "C:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake")
endif()

project (osync)

find_package(libwebsockets CONFIG REQUIRED)

include_directories("include")

file(GLOB SRC "src/*.cpp")

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME} websockets_shared)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
