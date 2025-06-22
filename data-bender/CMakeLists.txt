cmake_minimum_required(VERSION 3.16)
project(DataBender VERSION 1.0.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Platform detection
if(APPLE)
    set(PLATFORM_APPLE TRUE)
elseif(WIN32)
    set(PLATFORM_WINDOWS TRUE)
elseif(UNIX AND NOT APPLE)
    set(PLATFORM_LINUX TRUE)
endif()

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Source files
set(CORE_SOURCES
    core/DataBenderEngine.cpp
)

set(VCV_SOURCES
    vcv/DataBenderModule.cpp
    vcv/plugin.cpp
)

set(CORE_HEADERS
    core/DataBenderEngine.hpp
)

set(VCV_HEADERS
    vcv/DataBenderModule.hpp
    vcv/plugin.hpp
)

# Core DSP library (platform-agnostic)
add_library(DataBenderCore STATIC ${CORE_SOURCES} ${CORE_HEADERS})

target_include_directories(DataBenderCore PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/core>
    $<INSTALL_INTERFACE:include/DataBender>
)

# VCV Rack specific configuration
if(DEFINED RACK_DIR)
    # Include Rack's CMake configuration
    add_subdirectory(${RACK_DIR} ${CMAKE_BINARY_DIR}/rack)
    
    # Create the VCV plugin library
    add_library(DataBenderVCV SHARED ${VCV_SOURCES} ${VCV_HEADERS})
    
    # Link against Rack and core library
    target_link_libraries(DataBenderVCV rack DataBenderCore)
    
    # Include directories for VCV
    target_include_directories(DataBenderVCV PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/core
        ${CMAKE_CURRENT_SOURCE_DIR}/vcv
    )
    
    # Set output directory
    set_target_properties(DataBenderVCV PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${RACK_DIR}/plugins/DataBender
    )
    
    # Copy resources
    file(COPY res DESTINATION ${RACK_DIR}/plugins/DataBender)
    file(COPY plugin.json DESTINATION ${RACK_DIR}/plugins/DataBender)
endif()

# Installation
install(TARGETS DataBenderCore
    EXPORT DataBenderTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(FILES ${CORE_HEADERS}
    DESTINATION include/DataBender
)

install(EXPORT DataBenderTargets
    FILE DataBenderTargets.cmake
    NAMESPACE DataBender::
    DESTINATION lib/cmake/DataBender
)

# Package configuration
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    DataBenderConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/DataBenderConfigVersion.cmake
    DESTINATION lib/cmake/DataBender
) 