cmake_minimum_required(VERSION 3.28)

# Set the preferred compilers in order of priority (must be before project())
if(NOT DEFINED CMAKE_CXX_COMPILER)
    find_program(CLANG_COMPILER NAMES clang++ clang)
    find_program(GCC_COMPILER NAMES g++ gcc)
    find_program(MSVC_COMPILER NAMES cl)

    if(CLANG_COMPILER)
        set(CMAKE_CXX_COMPILER ${CLANG_COMPILER} CACHE FILEPATH "C++ compiler")
    elseif(GCC_COMPILER)
        set(CMAKE_CXX_COMPILER ${GCC_COMPILER} CACHE FILEPATH "C++ compiler")
    elseif(MSVC_COMPILER)
        set(CMAKE_CXX_COMPILER ${MSVC_COMPILER} CACHE FILEPATH "C++ compiler")
    else()
        message(FATAL_ERROR "No suitable compiler found")
    endif()
endif()

# Define the project
project(cvutil VERSION "2.5.0" DESCRIPTION "Computer Vision UTILity Toolkit" LANGUAGES CXX)

set(CVUTIL_PROJECT "cvutil" CACHE STRING "CVUTIL project")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options to enable or disable AVX-512 and AVX2/FMA support
option(ENABLE_AVX512 "Enable AVX-512 support" OFF) # To be enabled on x86_64 server processors
option(ENABLE_AVX2_FMA "Enable AVX2 and FMA support" ON) # Default support for x86_64 desktop processors

# Detect the OS
if(WIN32)
    message(STATUS "Configuring for Windows")
elseif(UNIX)
    message(STATUS "Configuring for Unix/Linux")
else()
    message(FATAL_ERROR "Unsupported OS")
endif()

message(STATUS "Using ${CMAKE_CXX_COMPILER_ID} as the C++ compiler")

# Create an interface library for compiler flags (modern CMake approach)
add_library(cvutil_compiler_flags INTERFACE)

    # MSVC-specific flags
target_compile_options(cvutil_compiler_flags INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:
        $<$<CONFIG:Debug>:/MDd /Zi /Ob0 /Od /RTC1 /fsanitize=address>
        $<$<CONFIG:Release>:/MD /Ox>
        /fp:precise /fp:strict
        $<$<BOOL:${ENABLE_AVX512}>:/arch:AVX512>
        $<$<BOOL:${ENABLE_AVX2_FMA}>:/arch:AVX2>
    >
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        $<$<CONFIG:Debug>:-g -O0 -fsanitize=address>
        $<$<CONFIG:Release>:-O3>
        -fno-fast-math -frounding-math
        $<$<BOOL:${ENABLE_AVX512}>:-mavx512f>
        $<$<BOOL:${ENABLE_AVX2_FMA}>:-mavx2 -mfma>
    >
)

target_compile_definitions(cvutil_compiler_flags INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:
        $<$<CONFIG:Debug>:DEBUG;_CRT_SECURE_NO_WARNINGS>
        $<$<CONFIG:Release>:NDEBUG>
    >
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        $<$<CONFIG:Debug>:DEBUG;_DEBUG>
        $<$<CONFIG:Release>:NDEBUG>
    >
)

target_link_options(cvutil_compiler_flags INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:
        $<$<CONFIG:Debug>:/DEBUG /INCREMENTAL> # /INFERASANLIBS:NO
        $<$<CONFIG:Release>:/OPT:REF /OPT:ICF>
    >
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
        $<$<CONFIG:Debug>:-fsanitize=address>
    >
)

# # Find and link the ASAN runtime library for Debug builds with MSVC
# if(MSVC)
#     # Find the ASAN import library
#     get_filename_component(COMPILER_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)
#     file(GLOB ASAN_LIB "${COMPILER_DIR}/clang_rt.asan_dynamic-x86_64.lib")
#     if(ASAN_LIB)
#         target_link_libraries(cvutil_compiler_flags INTERFACE
#             $<$<CONFIG:Debug>:${ASAN_LIB}>
#         )
#         message(STATUS "Found ASAN import library: ${ASAN_LIB}")
#     else()
#         message(WARNING "ASAN import library not found in ${COMPILER_DIR}")
#     endif()
# endif()

# Find OpenCV
find_package(OpenCV REQUIRED)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Charts Gui Widgets OpenGL)
qt_standard_project_setup()

message(STATUS "Found CMAKE_INSTALL_BINDIR: ${CMAKE_INSTALL_BINDIR}")
message(STATUS "Found CMAKE_INSTALL_LIBDIR: ${CMAKE_INSTALL_LIBDIR}")

# Set the Debug postfix
set(CMAKE_DEBUG_POSTFIX "d")

# Enable automatic MOC and RCC for all projects
set(CMAKE_AUTOMOC ON)    # Enable automatic MOC
set(CMAKE_AUTORCC ON)    # Enable automatic RCC
set(CMAKE_AUTOUIC ON)    # Enable automatic UIC

# Add subdirectories for each project
add_subdirectory(cvutil)
add_subdirectory(PluginManager)
add_subdirectory(RoiManager)

# Install OpenCV and Qt6 runtime shared libraries
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    foreach(opencv_lib ${OpenCV_LIBS})
        install(FILES $<TARGET_FILE:${opencv_lib}> CONFIGURATIONS Debug Release DESTINATION bin)
    endforeach()

    foreach(qt_lib Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Charts Qt6::OpenGL Qt6::OpenGLWidgets)
        install(FILES $<TARGET_FILE:${qt_lib}> CONFIGURATIONS Debug Release DESTINATION bin)
    endforeach()
    
    if(MSVC)
        # Configurable path for MSVC LLVM bin directory
        set(MSVC_LLVM_BIN_PATH "" CACHE PATH "Path to MSVC LLVM bin directory (e.g., C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/Llvm/x64/bin)")
        
        # Get the compiler directory and construct paths dynamically
        get_filename_component(COMPILER_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)
        
        # Build list of search paths for ASAN DLL
        set(ASAN_SEARCH_PATHS
            "${COMPILER_DIR}"
            "${COMPILER_DIR}/../../../bin"
            "${COMPILER_DIR}/../../bin"
        )
        
        # Add user-specified path if provided
        if(MSVC_LLVM_BIN_PATH)
            list(APPEND ASAN_SEARCH_PATHS "${MSVC_LLVM_BIN_PATH}")
        endif()
        
        # Add paths based on VSINSTALLDIR environment variable if available
        if(DEFINED ENV{VSINSTALLDIR})
            list(APPEND ASAN_SEARCH_PATHS "$ENV{VSINSTALLDIR}/VC/Tools/Llvm/x64/bin")
        endif()
        
        # Add common VS installation paths as fallback (supporting different editions)
        list(APPEND ASAN_SEARCH_PATHS
            "C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/Llvm/x64/bin"
            "C:/Program Files/Microsoft Visual Studio/2022/Professional/VC/Tools/Llvm/x64/bin"
            "C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/Llvm/x64/bin"
        )
        
        # Find and install ASAN DLL for Debug builds
        find_file(ASAN_DLL "clang_rt.asan_dynamic-x86_64.dll" 
                  PATHS ${ASAN_SEARCH_PATHS}
                  NO_DEFAULT_PATH)
        if(ASAN_DLL)
            install(FILES ${ASAN_DLL} CONFIGURATIONS Debug DESTINATION bin)
        endif()
    endif()
    
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Set RPATH to include our bundled libraries
    # set(CMAKE_INSTALL_RPATH "\$ORIGIN/../lib/cvutil;\$ORIGIN/../lib;/usr/lib/x86_64-linux-gnu;/lib/x86_64-linux-gnu")
    set(CMAKE_INSTALL_RPATH "/usr/lib/x86_64-linux-gnu;/lib/x86_64-linux-gnu")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    
    # In cvutil/CMakeLists.txt
    set_target_properties(cvutil PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )
    
    set_target_properties(PluginManager PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )
    
    set_target_properties(RoiManager PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
    )
    
    # foreach(opencv_lib ${OpenCV_LIBS})
    #     install(FILES $<TARGET_FILE:${opencv_lib}> CONFIGURATIONS Debug Release DESTINATION lib/cvutil COMPONENT runtime)
    # endforeach()

    # foreach(qt_lib Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Charts Qt6::OpenGL Qt6::OpenGLWidgets)
    #     install(FILES $<TARGET_FILE:${qt_lib}> CONFIGURATIONS Debug Release DESTINATION lib/cvutil COMPONENT runtime)
    # endforeach()
endif()

# Install PDB files for OpenCV libraries
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    foreach(opencv_lib ${OpenCV_LIBS})
        get_target_property(pdb_path ${opencv_lib} IMPORTED_PDB_LOCATION_DEBUG)
        if(NOT pdb_path)
            get_target_property(pdb_path ${opencv_lib} IMPORTED_LOCATION_DEBUG)
            if(pdb_path)
                string(REPLACE ".dll" ".pdb" pdb_path ${pdb_path})
            endif()
        endif()
        if(pdb_path AND EXISTS ${pdb_path})
            install(FILES ${pdb_path} CONFIGURATIONS Debug DESTINATION bin)
        endif()
    endforeach()

    # Install PDB files for Qt6 libraries
    foreach(qt_lib Qt6::Core Qt6::Widgets Qt6::Gui Qt6::Charts Qt6::OpenGL Qt6::OpenGLWidgets)
        get_target_property(pdb_path ${qt_lib} IMPORTED_PDB_LOCATION_DEBUG)
        if (NOT pdb_path)
            get_target_property(pdb_path ${qt_lib} IMPORTED_LOCATION_DEBUG)
            if(pdb_path)
                string(REPLACE ".dll" ".pdb" pdb_path ${pdb_path})
            endif()
        endif()
        if(pdb_path AND EXISTS ${pdb_path})
            install(FILES ${pdb_path} CONFIGURATIONS Debug DESTINATION bin)
        endif()
    endforeach()
endif()

# Install Qt6 support plugins
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    foreach(qt_plugin Qt6::QWindowsIntegrationPlugin)
        install(FILES $<TARGET_FILE:${qt_plugin}> CONFIGURATIONS Debug Release DESTINATION bin/platforms)
        # Copy pdb files for the plugins
        get_target_property(pdb_path ${qt_plugin} IMPORTED_PDB_LOCATION_DEBUG)
        if(NOT pdb_path)
            get_target_property(pdb_path ${qt_plugin} IMPORTED_LOCATION_DEBUG)
            if(pdb_path)
                string(REPLACE ".dll" ".pdb" pdb_path ${pdb_path})
            endif()
        endif()
        if(pdb_path AND EXISTS ${pdb_path})
            install(FILES ${pdb_path} CONFIGURATIONS Debug DESTINATION bin/platforms)
        endif()
    endforeach()
    
    foreach(qt_plugin Qt6::QModernWindowsStylePlugin)
        install(FILES $<TARGET_FILE:${qt_plugin}> CONFIGURATIONS Debug Release DESTINATION bin/styles)
        # Copy pdb files for the plugins
        get_target_property(pdb_path ${qt_plugin} IMPORTED_PDB_LOCATION_DEBUG)
        if(NOT pdb_path)
            get_target_property(pdb_path ${qt_plugin} IMPORTED_LOCATION_DEBUG)
            if(pdb_path)
                string(REPLACE ".dll" ".pdb" pdb_path ${pdb_path})
            endif()
        endif()
        if(pdb_path AND EXISTS ${pdb_path})
            install(FILES ${pdb_path} CONFIGURATIONS Debug DESTINATION bin/styles)
        endif()
    endforeach()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # # Qt6 platform plugins for Linux
    # if(TARGET Qt6::QXcbIntegrationPlugin)
    #     install(FILES $<TARGET_FILE:Qt6::QXcbIntegrationPlugin>
    #             CONFIGURATIONS Debug Release
    #             DESTINATION lib/cvutil/platforms
    #             COMPONENT runtime
    #             )
    # else()
    #     message(STATUS "Qt6::QXcbIntegrationPlugin not found")
    # endif()
    
    # # Qt6 style plugins for Linux
    # # Qt6 theme plugins for Linux (if available)
    # if(TARGET Qt6::QGtk3ThemePlugin)
    #     install(FILES $<TARGET_FILE:Qt6::QGtk3ThemePlugin>
    #             CONFIGURATIONS Debug Release
    #             DESTINATION lib/cvutil/platformthemes
    #             COMPONENT runtime
    #             )
    # else()
    #     message(STATUS "Qt6::QGtk3ThemePlugin not found")
    # endif()
    
endif()

# Set the include install directory
set(PACKAGE_INCLUDE_INSTALL_DIR "include/cvutil")

# Add to the root CMakeLists.txt
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    install(FILES "${CMAKE_BINARY_DIR}/cvutil_runtime_dependencies-$<CONFIG>.cmake"
            DESTINATION lib/cmake/cvutil
            COMPONENT development)
endif()

# Install the cvutilConfig.cmake and cvutilConfigVersion.cmake files
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/cvutilConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_file(cvutilConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/cvutilConfig.cmake" @ONLY
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/cvutilConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/cvutilConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cvutil
    COMPONENT development
)

install(EXPORT cvutilTargets
    FILE cvutilTargets.cmake
    NAMESPACE cvutil::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cvutil
    COMPONENT development
)

# Add to your root CMakeLists.txt
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/CMake/copy_runtime_dependencies.cmake
  ${CMAKE_BINARY_DIR}/copy_runtime_dependencies.cmake
  COPYONLY
)

install(FILES 
  ${CMAKE_BINARY_DIR}/copy_runtime_dependencies.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cvutil
  COMPONENT development
)

# Packaging configuration
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Check if we are using a conda env or not
    if (DEFINED ENV{CONDA_PREFIX})
        message(STATUS "Using conda environment: $ENV{CONDA_PREFIX}")
        # Do not package if inside a conda environment
    else()
        message(STATUS "Not using conda environment, use CPack to create a Debian package.")
        include(CMake/PackageLinux.cmake)
    endif()
endif()


