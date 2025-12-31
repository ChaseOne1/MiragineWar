cmake_minimum_required(VERSION 3.26)

project(MiragineWar VERSION 0.1.0 LANGUAGES CXX)
configure_file(
    "${CMAKE_SOURCE_DIR}/src/app/AppMetadata.hpp.in"
    "${CMAKE_SOURCE_DIR}/src/app/AppMetadata.hpp"
)
#------------------------------------------------------------------
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
#should be changed to a bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS src/*.cpp src/*.cc)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
#------------------------------------------------------------------
#3rdlibs
add_subdirectory(3rdlibs/SDL3 EXCLUDE_FROM_ALL)

add_subdirectory(3rdlibs/SDL3_image EXCLUDE_FROM_ALL)

set(SDLTTF_VENDORED ON)
set(SDLTTF_PLUTOSVG OFF)
add_subdirectory(3rdlibs/SDL3_ttf EXCLUDE_FROM_ALL)
target_compile_options(harfbuzz PRIVATE -Wa,-mbig-obj) # for SDL_ttf's dependency on mingw

add_subdirectory(3rdlibs/toml++ EXCLUDE_FROM_ALL)

add_subdirectory(3rdlibs/entt EXCLUDE_FROM_ALL)

set(mathfu_build_benchmarks OFF CACHE BOOL "")
set(mathfu_build_tests OFF CACHE BOOL "")
cmake_policy(PUSH)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
add_subdirectory(3rdlibs/mathfu EXCLUDE_FROM_ALL)
mathfu_configure_flags(${PROJECT_NAME})
cmake_policy(POP)

set(SLIKENET_ENABLE_STATIC OFF)
add_subdirectory(3rdlibs/SLikeNet EXCLUDE_FROM_ALL)

set(LUA_ROOT "" CACHE PATH "lua root path")
add_subdirectory(3rdlibs/sol2 EXCLUDE_FROM_ALL)

add_subdirectory(3rdlibs/mirrow EXCLUDE_FROM_ALL)

add_subdirectory(3rdlibs/MiragineWarProtocol EXCLUDE_FROM_ALL)

#------------------------------------------------------------------
#compile flags
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3 
                                              SDL3_image::SDL3_image
                                              SDL3_ttf::SDL3_ttf
                                              tomlplusplus::tomlplusplus
                                              EnTT::EnTT
                                              SLikeNetDLL
                                              MWProtocol::MWProtocol
                                              sol2::sol2
                                              lua
                                              Mirrow::Srefl)
#target_link_options(${PROJECT_NAME} PRIVATE -mwindows) #disable console window
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch.hpp)
target_include_directories(${PROJECT_NAME} PRIVATE src ${LUA_ROOT}/include)
target_link_directories(${PROJECT_NAME} PRIVATE ${LUA_ROOT}/lib)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_options(${PROJECT_NAME} PRIVATE -g -Wa,-mbig-obj -fno-rtti)
