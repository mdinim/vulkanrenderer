cmake_minimum_required(VERSION 3.8)

include(GNUInstallDirs)

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

# GLFW Specific settings
# set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
# set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
# set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

find_package(Vulkan REQUIRED)
find_package(Core 0.0.1 REQUIRED COMPONENTS FileManager)
find_package(glfw3 REQUIRED)

set(BUILTIN_SHADER_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_DATADIR}/Engine/Shaders)

list(APPEND Engine_FILES
        include/configuration.hpp
        ${CMAKE_CURRENT_BINARY_DIR}/Directories.hpp

        include/Window/IWindow.hpp
        include/Window/IWindowService.hpp

        include/Renderer/IRenderer.hpp
        include/Renderer/Vulkan/Renderer.hpp

        include/Window/GLFWWindow.hpp
        include/Window/GLFWWindowService.hpp
        )

list(APPEND Engine_SRC_FILES
        #TODO remove
        include/Data/Representation.hpp
        src/Data/Representation.cpp

        include/Renderer/Vulkan/Instance.hpp
        src/Renderer/Vulkan/Instance.cpp

        include/Renderer/Vulkan/Surface.hpp
        src/Renderer/Vulkan/Surface.cpp

        include/Renderer/Vulkan/PhysicalDevice.hpp
        src/Renderer/Vulkan/PhysicalDevice.cpp

        include/Renderer/Vulkan/LogicalDevice.hpp
        src/Renderer/Vulkan/LogicalDevice.cpp

        include/Renderer/Vulkan/Utils.hpp
        src/Renderer/Vulkan/Utils.cpp

        include/Renderer/Vulkan/Swapchain.hpp
        src/Renderer/Vulkan/Swapchain.cpp

        include/Renderer/Vulkan/ImageView.hpp
        src/Renderer/Vulkan/ImageView.cpp

        include/Renderer/Vulkan/RenderPass.hpp
        src/Renderer/Vulkan/RenderPass.cpp

        include/Renderer/Vulkan/GraphicsPipeline.hpp
        src/Renderer/Vulkan/GraphicsPipeline.cpp

        include/Renderer/Vulkan/Framebuffer.hpp
        src/Renderer/Vulkan/Framebuffer.cpp

        include/Renderer/Vulkan/CommandPool.hpp
        src/Renderer/Vulkan/CommandPool.cpp

        include/Renderer/Vulkan/Buffers.hpp
        src/Renderer/Vulkan/Buffers.cpp

        include/Renderer/Vulkan/Allocator.hpp
        src/Renderer/Vulkan/Allocator.cpp

        include/Renderer/Vulkan/Chunk.hpp
        src/Renderer/Vulkan/Chunk.cpp

        include/Renderer/Vulkan/Block.hpp
        src/Renderer/Vulkan/Block.cpp

        #endTODO

        src/Window/GLFWWindow.cpp
        src/Window/GLFWWindowService.cpp
        src/Renderer/Vulkan/Renderer.cpp
        )

### Target Vulkan::Engine
add_library(Engine STATIC ${Engine_FILES} ${Engine_SRC_FILES})

#add_executable(Engine
#              ${Engine_FILES}
#)

add_common_compiler_options(Engine)

set_target_properties(Engine PROPERTIES
        LANGUAGE CXX
        LINKER_LANGUAGE CXX
        )

target_compile_features(Engine PUBLIC ${COMMON_COMPILE_FEATURES})

if (APPLE)
    # In case of GCC we have to link the frameworks manually
    if (${CMAKE_COMPILER_IS_GNUCXX})
        macro(add_osx_framework fwname target)
            find_library(FRAMEWORK_${fwname}
                    NAMES ${fwname}
                    PATHS ${CMAKE_OSX_SYSROOT}/System/Library
                    PATH_SUFFIXES Frameworks
                    NO_DEFAULT_PATH)
            if (${FRAMEWORK_${fwname}} STREQUAL FRAMEWORK_${fwname}-NOTFOUND)
                message(FATAL_ERROR ": Framework ${fwname} not found")
            else ()
                target_link_libraries(${target} PUBLIC "${FRAMEWORK_${fwname}}")
                message(STATUS "Framework ${fwname} found at ${FRAMEWORK_${fwname}}")
            endif ()
        endmacro(add_osx_framework)

        add_osx_framework(Cocoa Engine)
        add_osx_framework(IOKit Engine)
        add_osx_framework(CoreFoundation Engine)
        add_osx_framework(CoreVideo Engine)
    endif ()
endif ()

target_link_libraries(Engine PUBLIC Vulkan::Vulkan glfw Core::FileManager)

target_include_directories(Engine
        INTERFACE
            $<INSTALL_INTERFACE:include/Engine>
            $<INSTALL_INTERFACE:include>
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        )

install(TARGETS Engine
        EXPORT engine-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )

install(DIRECTORY include/Renderer  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Engine)
install(DIRECTORY include/Window    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Engine)

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

list(APPEND Shader_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Shaders/shader.frag
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Shaders/shader.vert
)

find_program(GLSLC glslc DOC "GLSL compiler for Vulkan")
if (${GLSLC} STREQUAL "GLSLC-NOTFOUND")
    message(FATAL_ERROR "GLSLC is required to compile the shaders!")
endif()
foreach (shader ${Shader_FILES})
    get_filename_component(filename ${shader} NAME_WE)
    get_filename_component(type ${shader} LAST_EXT)
    string(REPLACE "." "" type ${type})
    set(out_file ${filename}_${type}.spv)

    add_custom_command(TARGET Engine
            POST_BUILD
            COMMAND ${GLSLC} ${shader} -o ${CMAKE_CURRENT_BINARY_DIR}/${out_file}
            DEPENDS ${shader})
    list(APPEND CompiledShader_FILES ${CMAKE_CURRENT_BINARY_DIR}/${out_file})
endforeach ()

install(FILES
        ${CompiledShader_FILES}
    DESTINATION
        ${CMAKE_INSTALL_DATADIR}/Engine/Shaders
)

configure_file(directories.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/directories.hpp)

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

add_executable(test main.cpp)
if(APPLE)
    target_include_directories(test
            PRIVATE
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
endif()
target_link_libraries(test Engine)