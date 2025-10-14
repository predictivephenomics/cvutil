cmake_minimum_required(VERSION 3.28)

# Define the project
project(cvutil VERSION "2.5.0" DESCRIPTION "Computer Vision UTILity Toolkit" LANGUAGES CXX)

set(CVUTIL_PROJECT "cvutil" CACHE STRING "CVUTIL project")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the preferred compilers in order of priority
if(NOT CMAKE_CXX_COMPILER)
    find_program(CLANG_COMPILER NAMES clang++ clang)
    find_program(GCC_COMPILER NAMES g++ gcc)
    find_program(MSVC_COMPILER NAMES cl)

    if(CLANG_COMPILER)
        set(CMAKE_CXX_COMPILER ${CLANG_COMPILER})
    elseif(GCC_COMPILER)
        set(CMAKE_CXX_COMPILER ${GCC_COMPILER})
    elseif(MSVC_COMPILER)
        set(CMAKE_CXX_COMPILER ${MSVC_COMPILER})
    else()
        message(FATAL_ERROR "No suitable compiler found")
    endif()
endif()

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

# Set global CMake build options
if(MSVC) # OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    # MSVC-specific flags
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd /Zi /Ob0 /Od /RTC1 /DDEBUG /D_CRT_SECURE_NO_WARNINGS")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG /INCREMENTAL")
    
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD /Ox /DNDEBUG")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF")
    
    # Enable AVX-512 and AVX2/FMA support
    if(ENABLE_AVX512)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX512")
    endif()
    if(ENABLE_AVX2_FMA)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")
    endif()
    
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /fp:precise /fp:strict")
    
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUG -D_DEBUG -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
    
    # Enable AVX-512 and AVX2/FMA support
    if(ENABLE_AVX512)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx512")
    endif()
    if(ENABLE_AVX2_FMA)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -mfma")
    endif()
    
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-fast-math -frounding-math")
    
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -DDEBUG -D_DEBUG -O0")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -DNDEBUG")
    
    # Enable AVX-512 and AVX2/FMA support
    if(ENABLE_AVX512)
        message(STATUS "Enabling AVX-512 support")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx512")
    endif()
    if(ENABLE_AVX2_FMA)
        message(STATUS "Enabling AVX2 and FMA support")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -mfma")
    endif()
    
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-fast-math -frounding-math")
    
else()
    message(FATAL_ERROR "Unsupported compiler")
endif()

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


