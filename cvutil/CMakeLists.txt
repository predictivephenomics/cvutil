# cvutil CMakeLists.txt

cmake_minimum_required(VERSION 3.28)

# project(cvutil)

if(NOT DEFINED CVUTIL_PROJECT)
    message(FATAL_ERROR "This project should only be built as part of the full solution. Please use the solution-level CMakeLists.txt.")
endif()

# Set sources and headers for cvutil
set(SOURCES
    cvutil_bwdist.cpp
    cvutil_bwskel.cpp
    cvutil_bwthin.cpp
    cvutil_core.cpp
    cvutil_figure.cpp
    cvutil_linesim.cpp
    cvutil_videowriter.cpp
    main.cpp
    cvutil_matlab_interface.cpp
    MainWindow/BatchProcessor.cpp
    MainWindow/FeatureExtractorThread.cpp
    MainWindow/GraphicsScene.cpp
    MainWindow/InteractiveExtractorThread.cpp
    MainWindow/logger.cpp
    MainWindow/MainWindow.cpp
    MainWindow/helper_functions.cpp
    MainWindow/MaterialStyle.cpp
    Profiler.cpp
    resources.qrc
)

set(HEADERS
    cvutil.h
    cvutil_bwdist.h
    cvutil_bwskel.h
    cvutil_bwthin.h
    cvutil_core.h
    cvutil_figure.h
    cvutil_linesim.h
    cvutil_templates.h
    cvutil_matlab_interface.h
    cvutil_types.h
    cvutil_videowriter.h
    demo.h
    figure.h
    main.h
    MainWindow/BatchProcessor.h
    MainWindow/FeatureExtractorThread.h
    MainWindow/GraphicsScene.h
    MainWindow/InteractiveExtractorThread.h
    MainWindow/logger.h
    MainWindow/MainWindow.h
    MainWindow/helper_functions.h
    MainWindow/MaterialStyle.h
    profiler.h
    resource.h
    stdproto.h
    video.h
)

# Include the resource file only if it's WIN32 and MSVC
if(WIN32)
    list(APPEND SOURCES cvutil.rc)
endif()


# # Determine if we are in Debug mode
# if(CMAKE_BUILD_TYPE STREQUAL "Debug")
#     set(LIBRARY_SUFFIX "d")  # "d" for Debug libraries
# else()
#     set(LIBRARY_SUFFIX "")
# endif()

# Define the target as a shared library
add_library(cvutil SHARED ${SOURCES} ${HEADERS})

# Automatically collect .qrc files and process them
# file(GLOB RESOURCE_FILES "*.qrc")
# qt6_add_resources(cvutil "RESOURCE_FILES" ${RESOURCE_FILES})

# Set the QT_RESOURCE_ALIAS property for the resources.qrc file
# set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/resources.qrc PROPERTIES QT_RESOURCE_ALIAS "resources.qrc")

# Enable auto-rcc to process .qrc files (e.g., resources.qrc)
# qt_add_resources(cvutil "resources" PREFIX "/icons" FILES resources.qrc)

# Add the generated resource .cpp file to the target
# target_sources(cvutil PRIVATE ${RESOURCE_FILES})

# Include directories (Qt and OpenCV paths are already handled at the solution level)
# target_include_directories(cvutil
#     PRIVATE
#         ${CMAKE_BINARY_DIR}/library/include
#         ${OpenCV_INCLUDE_DIRS}
#         ${Qt6Core_INCLUDE_DIRS}
#         ${Qt6Widgets_INCLUDE_DIRS}
#         ${Qt6Charts_INCLUDE_DIRS}
#         ${Qt6Gui_INCLUDE_DIRS}
# )
target_include_directories(cvutil
    PUBLIC
        $<INSTALL_INTERFACE:include/cvutil>
    PRIVATE
        ${OpenCV_INCLUDE_DIRS}
)

# Link libraries (cvutil depends on PluginManager and RoiManager)
target_link_libraries(cvutil
    PRIVATE
        cvutil_compiler_flags
        ${OpenCV_LIBS}
        Qt6::Core
        Qt6::Widgets
        Qt6::Charts
        Qt6::Gui
        Qt6::OpenGL
        PluginManager  # These are handled at the solution level, no need to add subdirectories here
        RoiManager
)

target_compile_definitions(cvutil PRIVATE CVUTIL_SOURCE)

set(PUBLIC_HEADERS
    cvutil.h
    cvutil_core.h
    cvutil_matlab_interface.h
    cvutil_templates.h
    figure.h
    profiler.h
    stdproto.h
    video.h
)

# file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/library/bin)
# file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/library/include)
# file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/library/lib)

# # Set the output directories for the shared library (bin for runtime, lib for static and import libraries)
# set_target_properties(cvutil PROPERTIES
#     RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/library/bin/
#     RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/library/bin/
    
#     LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/library/lib/
#     LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/library/lib/
    
#     ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/library/lib/
#     ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/library/lib/
# )

# # Post-build events headers to appropriate directory
# add_custom_command(TARGET cvutil POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         ${CMAKE_CURRENT_SOURCE_DIR}/cvutil.h ${CMAKE_BINARY_DIR}/library/include/
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         ${CMAKE_CURRENT_SOURCE_DIR}/cvutil_core.h ${CMAKE_BINARY_DIR}/library/include/
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         ${CMAKE_CURRENT_SOURCE_DIR}/cvutil_matlab_interface.h ${CMAKE_BINARY_DIR}/library/include/
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         ${CMAKE_CURRENT_SOURCE_DIR}/cvutil_templates.h ${CMAKE_BINARY_DIR}/library/include/
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         ${CMAKE_CURRENT_SOURCE_DIR}/figure.h ${CMAKE_BINARY_DIR}/library/include/
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         ${CMAKE_CURRENT_SOURCE_DIR}/profiler.h ${CMAKE_BINARY_DIR}/library/include/
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         ${CMAKE_CURRENT_SOURCE_DIR}/stdproto.h ${CMAKE_BINARY_DIR}/library/include/
#     COMMAND ${CMAKE_COMMAND} -E copy_if_different
#         ${CMAKE_CURRENT_SOURCE_DIR}/video.h ${CMAKE_BINARY_DIR}/library/include/
# )

# # Install the cvutil library
# install(TARGETS cvutil
#     EXPORT cvutilTargets
#     CONFIGURATIONS Debug Release
#     RUNTIME DESTINATION $<CONFIG>/bin
#     LIBRARY DESTINATION $<CONFIG>/lib/cvutil
#     ARCHIVE DESTINATION $<CONFIG>/lib/cvutil
#     PUBLIC_HEADER DESTINATION $<CONFIG>/include/cvutil
# )

# # Install public headers
# install(FILES ${PUBLIC_HEADERS}
#     CONFIGURATIONS Debug Release
#     DESTINATION $<CONFIG>/include/cvutil
# )

# # Install PDB files
# install(FILES $<TARGET_PDB_FILE:cvutil> CONFIGURATIONS Debug DESTINATION Debug/bin)

# Install the cvutil library
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    install(TARGETS cvutil
        EXPORT cvutilTargets
        CONFIGURATIONS Debug Release
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib/cvutil
        ARCHIVE DESTINATION lib/cvutil
        PUBLIC_HEADER DESTINATION include/cvutil
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    install(TARGETS cvutil
        EXPORT cvutilTargets
        CONFIGURATIONS Debug Release
        RUNTIME 
            COMPONENT runtime
            DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY 
            COMPONENT runtime
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE 
            COMPONENT development
            DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER 
            COMPONENT development
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cvutil
    )
endif()

# Install public headers
install(FILES ${PUBLIC_HEADERS}
    CONFIGURATIONS Debug Release
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cvutil
    COMPONENT development
)

# Install PDB files
if(MSVC)
    install(FILES $<TARGET_PDB_FILE:cvutil> CONFIGURATIONS Debug DESTINATION bin COMPONENT development)
endif()

# For the cvutil target
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    # Build search directories list for runtime dependency detection
    set(RUNTIME_SEARCH_DIRS ${DEP_PATH})
    add_custom_command(TARGET cvutil POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        -D TARGET_FILE=$<TARGET_FILE:cvutil>
        -D INSTALL_BIN=${CMAKE_INSTALL_PREFIX}/$<IF:$<BOOL:${WIN32}>,bin,lib/cvutil>
        -D SEARCH_DIRS="${RUNTIME_SEARCH_DIRS}"
        -D BUILD_CONFIG=$<CONFIG>
        -D BINARY_DIR=${CMAKE_BINARY_DIR}
        -P "${CMAKE_CURRENT_SOURCE_DIR}/../CMake/find_runtime_deps.cmake"
    COMMENT "Copying runtime dependencies for cvutil"
    )
endif()

