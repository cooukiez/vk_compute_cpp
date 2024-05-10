cmake_minimum_required(VERSION 3.8)

project(vcc)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB MAIN_SOURCES *.cpp)
file(GLOB_RECURSE VK_SOURCES vk/*.cpp)
file(GLOB_RECURSE RENDER_SOURCES render/*.cpp)

set(SOURCES)
list(APPEND SOURCES ${MAIN_SOURCES} ${VK_SOURCES} ${RENDER_SOURCES})

add_executable(main ${SOURCES})

find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)
find_package(Stb REQUIRED)

target_link_libraries(main Vulkan::Vulkan)
target_link_libraries(main glm::glm)

target_include_directories(main PRIVATE ${Stb_INCLUDE_DIR})

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    message(STATUS "Detected MinGW compiler.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    message(STATUS "Detected MSVC compiler.")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2")
endif()

add_custom_target(comp.spv
        COMMAND glslangValidator --quiet -e "main" -V ${CMAKE_SOURCE_DIR}/shader.comp -o ${CMAKE_BINARY_DIR}/comp.spv)

add_dependencies(main comp.spv)

file(COPY ${CMAKE_SOURCE_DIR}/textures DESTINATION ${CMAKE_BINARY_DIR})
