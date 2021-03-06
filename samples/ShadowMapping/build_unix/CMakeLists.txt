cmake_minimum_required(VERSION 2.8)
project(ShadowMapping)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FRAMEWORK_PATH ${ShadowMapping_SOURCE_DIR}/../../../../Framework)
set(PALLEON_PATH ${ShadowMapping_SOURCE_DIR}/../../../)

if(NOT TARGET Framework)
	add_subdirectory(${FRAMEWORK_PATH}/build_unix ${FRAMEWORK_PATH}/build_unix/build)
endif()

if(NOT TARGET Palleon)
	add_subdirectory(${PALLEON_PATH}/build_unix ${PALLEON_PATH}/build_unix/build)
endif()

add_executable(ShadowMapping
	../src/Application.cpp
	../src/TouchFreeCamera.cpp
)

target_include_directories(ShadowMapping PRIVATE ${FRAMEWORK_PATH}/include)
target_include_directories(ShadowMapping PRIVATE ${PALLEON_PATH}/include)

target_link_libraries(ShadowMapping xcb z Palleon Framework)
