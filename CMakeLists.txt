cmake_minimum_required(VERSION 3.5)
project(cpp_example)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
  set(CMAKE_BUILD_TYPE
      "Release"
      CACHE STRING "" FORCE)
  message(STATUS "Set build type to default: ${CMAKE_BUILD_TYPE}")
else()
  message(STATUS "Your build type: ${CMAKE_BUILD_TYPE}")
endif()
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -ggdb")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -ggdb")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

include_directories(BEFORE ${PROJECT_SOURCE_DIR}/headers/include)
include(${PROJECT_SOURCE_DIR}/headers/include/utils.cmake)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror")

include_directories(BEFORE ${PROJECT_BINARY_DIR} ${PROJECT_SOURCE_DIR}/src)
set(HEADER_ROOT "${PROJECT_SOURCE_DIR}/src/${PROJECT_NAME}")
file(GLOB_RECURSE HEADERS src/**/*.hpp)
file(GLOB_RECURSE SOURCES src/**/*.cpp)
add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})
print_all_linked_libraries(${PROJECT_NAME})
install(
    TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
foreach(file ${HEADERS})
    get_filename_component(dir ${file} DIRECTORY)
    string(
        REGEX
        REPLACE "${HEADER_ROOT}"
                ""
                dir
                ${dir})
    install(FILES ${file} DESTINATION "include/${PROJECT_NAME}/${dir}")
endforeach()

option(BUILD_TESTS "Build tests." ON)
if(BUILD_TESTS)
    file(GLOB SRCS tests/test_*.cpp)
    foreach(src ${SRCS})
        string(
            REGEX
            REPLACE "(^.*/|.cpp$)"
                    ""
                    exe
                    ${src})
        add_executable(${exe} ${src})
        target_link_libraries(${exe} ${PROJECT_NAME})
        install(TARGETS ${exe} RUNTIME DESTINATION bin)
    endforeach(src)
endif()