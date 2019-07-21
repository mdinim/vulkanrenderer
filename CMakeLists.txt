cmake_minimum_required(VERSION 3.8)

set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/Engine)
set(COMMON_COMPILE_OPTIONS -Wall -pedantic -pedantic-errors -Werror -Wextra)
set(COMMON_COMPILE_FEATURES cxx_std_17)

macro(add_common_compiler_options TARGET_NAME)
    target_compile_options(${TARGET_NAME} PRIVATE ${COMMON_COMPILE_OPTIONS}
            $<$<CONFIG:DEBUG>:-O0>
            $<$<CONFIG:RELEASE>:-O3>
#            $<$<BOOL:${CREATE_COVERAGE_REPORT}>:-O0>
#            $<$<AND:$<BOOL:${CREATE_COVERAGE_REPORT}>,$<CXX_COMPILER_ID:GNU>>:--coverage>
#            $<$<AND:$<BOOL:${CREATE_COVERAGE_REPORT}>,$<OR:$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:Clang>>>:-fprofile-instr-generate -fcoverage-mapping>
    )
endmacro()

# Project name
project(VulkanEngine VERSION 0.0.1 LANGUAGES CXX)

include(GNUInstallDirs)

# GLFW Specific settings
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

find_package(Vulkan REQUIRED)

list(APPEND Engine_FILES
        #src/main.cpp

        include/configuration.hpp

        include/Window/IWindow.hpp
        include/Window/IWindowService.hpp

        include/Renderer/IRenderer.hpp
        include/Renderer/VulkanRenderer.hpp

        include/Window/GLFWWindow.hpp
        include/Window/GLFWWindowService.hpp
)

list(APPEND Engine_SRC_FILES
        src/Window/GLFWWindow.cpp
        src/Window/GLFWWindowService.cpp
        src/Renderer/VulkanRenderer.cpp
)

### Target Vulkan::Engine
add_library(Engine ${Engine_FILES} ${Engine_SRC_FILES})

#add_executable(Engine
#              ${Engine_FILES}
#)

add_common_compiler_options(Engine)

set_target_properties(Engine PROPERTIES
        LANGUAGE CXX
        LINKER_LANGUAGE CXX
)

target_compile_features(Engine PUBLIC ${COMMON_COMPILE_FEATURES})

target_include_directories(Engine PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extern/glfw)

target_link_libraries(Engine glfw Vulkan::Vulkan)

install(TARGETS Engine glfw
        EXPORT engine-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Renderer
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Engine
)

install(DIRECTORY
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Window
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Engine
        )

install(EXPORT engine-targets
        FILE
        EngineTargets.cmake
        NAMESPACE
        Engine::
        DESTINATION
        ${CMAKE_INSTALL_LIBDIR}/cmake/Engine
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/EngineConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/EngineConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/EngineConfig.cmake
        INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

export(EXPORT engine-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/EngineTargets.cmake NAMESPACE Engine::)

export(PACKAGE Engine)

configure_file(Engine.pc.in Engine.pc @ONLY)

install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/EngineConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/EngineConfigVersion.cmake
        DESTINATION ${INSTALL_CONFIGDIR}
)

target_include_directories(Engine
    PUBLIC
        $<INSTALL_INTERFACE:include>
        $<INSTALL_INTERFACE:include/Engine>
    PRIVATE
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
