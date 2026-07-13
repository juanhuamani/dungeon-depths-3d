cmake_minimum_required(VERSION 3.16)
project(dungeon-depths-3d LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(glm REQUIRED)

add_library(glad STATIC vendor/glad/src/gl.c)
target_include_directories(glad PUBLIC vendor/glad/include)

add_library(stb_image STATIC vendor/stb_image_impl.cpp)
target_include_directories(stb_image PUBLIC vendor)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/vendor
    ${CMAKE_SOURCE_DIR}/vendor/glad/include
)

find_package(Threads REQUIRED)

target_link_libraries(${PROJECT_NAME}
    OpenGL::GL
    glfw
    glm::glm
    glad
    stb_image
    Threads::Threads
    dl
    m
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)
