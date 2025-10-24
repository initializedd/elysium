cmake_minimum_required(VERSION 3.25.0)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_DEBUG_POSTFIX _debug)

project(elysium)

include(FetchContent)

FetchContent_Declare(
    Vulkan-Headers
    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Headers.git
    GIT_TAG        33d7f512583b8de44d1b6384aa1cf482f92e53e9 # v1.4.329
    GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(Vulkan-Headers)

FetchContent_Declare(
    VulkanHpp
    GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Hpp.git
    GIT_TAG        767fce85b7073ca3017f34ae242e01cfbdb7cf3d # v1.4.329
    GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(VulkanHpp)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        7b6aead9fb88b3623e3b3725ebb42670cbe4c579 # v3.4
    GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(glfw)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG        a532f5b1cf27d6a3c099437e6959cf7e398a0a67 # v1.0.2
    GIT_PROGRESS   TRUE
)
set(GLM_ENABLE_CXX_20 ON)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
    OpenAL
    GIT_REPOSITORY https://github.com/kcat/openal-soft.git
    GIT_TAG        dc7d7054a5b4f3bec1dc23a42fd616a0847af948 # v1.24.3
    GIT_PROGRESS   TRUE
)
FetchContent_MakeAvailable(OpenAL)

target_link_libraries(VulkanHpp INTERFACE Vulkan::Headers)

add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(${PROJECT_NAME} PRIVATE Vulkan::Hpp glfw glm::glm OpenAL::OpenAL)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets)

