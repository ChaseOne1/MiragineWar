cmake_minimum_required(VERSION 3.26)

project(MiragineWar VERSION 0.1.0 LANGUAGES CXX)
configure_file(
    "${CMAKE_SOURCE_DIR}/src/app/AppMetadata.hpp.in"
    "${CMAKE_SOURCE_DIR}/src/app/AppMetadata.hpp"
)

#------------------------------------------------------------------
#should be changed to a bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/../MiragineWar-Game)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

#------------------------------------------------------------------
#3rdlibs
add_subdirectory(3rdlibs/SDL3 EXCLUDE_FROM_ALL)
add_subdirectory(3rdlibs/SDL3_image EXCLUDE_FROM_ALL)
add_subdirectory(3rdlibs/toml++ EXCLUDE_FROM_ALL)
add_subdirectory(3rdlibs/entt EXCLUDE_FROM_ALL)
set(mathfu_build_benchmarks OFF CACHE BOOL "")
set(mathfu_build_tests OFF CACHE BOOL "")
add_subdirectory(3rdlibs/mathfu EXCLUDE_FROM_ALL)
mathfu_configure_flags(${PROJECT_NAME})
add_subdirectory(tools EXCLUDE_FROM_ALL)

#------------------------------------------------------------------
#compile flags
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 
                                              SDL3_image::SDL3_image
                                              tomlplusplus::tomlplusplus
                                              EnTT::EnTT)

#target_link_options(${PROJECT_NAME} PRIVATE -mwindows) #disable console window
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch.hpp)
target_include_directories(${PROJECT_NAME} PRIVATE src)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_options(${PROJECT_NAME} PRIVATE -g -Og -fno-rtti)
