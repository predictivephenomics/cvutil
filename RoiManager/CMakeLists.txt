# RoiManager CMakeLists.txt

cmake_minimum_required(VERSION 3.28)

# Ensure this project is only built as part of the full solution
if(NOT DEFINED CVUTIL_PROJECT)
    message(FATAL_ERROR "This project should only be built as part of the full solution. Please use the solution-level CMakeLists.txt.")
endif()

# Define the source files for RoiManager
set(SOURCES
    RoiManager.cpp
)

# Define the header files
set(HEADERS
    include/RoiManager.h
    resource.h
)

# Include the resource file only if it's WIN32 and MSVC
if(WIN32)
    list(APPEND SOURCES RoiManager.rc)
endif()

# Create the shared library target
add_library(RoiManager SHARED ${SOURCES} ${HEADERS})

# Qt6 and OpenCV include directories are handled globally, so no need to repeat
# target_include_directories(RoiManager
#     PRIVATE
#         ${CMAKE_SOURCE_DIR}/include
#         ${OpenCV_INCLUDE_DIRS}
# )
target_include_directories(RoiManager
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/cvutil>
    PRIVATE
        ${OpenCV_INCLUDE_DIRS}
)

# Link Qt6 and OpenCV libraries, along with other dependencies
target_link_libraries(RoiManager
    PRIVATE
        ${OpenCV_LIBS}
        Qt6::Core
        Qt6::Widgets
        Qt6::Charts
        Qt6::Gui
        Qt6::OpenGL
)

target_compile_definitions(RoiManager PRIVATE ROIMANAGER_SOURCE)

set(PUBLIC_HEADERS
    include/RoiManager.h
)

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/library/bin)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/library/include)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/library/lib)

# Set the output directories for the built targets
set_target_properties(cvutil PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/library/bin/
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/library/bin/
    
    LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/library/lib/
    LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/library/lib/
    
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/library/lib/
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/library/lib/
)

# Post-build events to copy DLL, lib, and headers to appropriate directories
add_custom_command(TARGET RoiManager POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/include/RoiManager.h ${CMAKE_BINARY_DIR}/library/include/
)

# # Install the RoiManager library for Debug configuration
# install(TARGETS RoiManager
#     EXPORT cvutilTargets
#     CONFIGURATIONS Debug Release
#     RUNTIME DESTINATION $<CONFIG>/bin
#     LIBRARY DESTINATION $<CONFIG>/lib/cvutil
#     ARCHIVE DESTINATION $<CONFIG>/lib/cvutil
#     PUBLIC_HEADER DESTINATION $<CONFIG>/include/cvutil
# )

# # Install public headers
# install(FILES ${PUBLIC_HEADERS}
#     CONFIGURATIONS Debug
#     DESTINATION Debug/include/cvutil
# )

# install(FILES ${PUBLIC_HEADERS}
#     CONFIGURATIONS Release
#     DESTINATION Release/include/cvutil
# )

# # Install PDB files
# install(FILES $<TARGET_PDB_FILE:RoiManager> CONFIGURATIONS Debug DESTINATION Debug/bin)

# Install the RoiManager library for Debug configuration
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    install(TARGETS RoiManager
        EXPORT cvutilTargets
        CONFIGURATIONS Debug Release
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib/cvutil
        ARCHIVE DESTINATION lib/cvutil
        PUBLIC_HEADER DESTINATION include/cvutil
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    install(TARGETS RoiManager
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
if (MSVC)
    install(FILES $<TARGET_PDB_FILE:RoiManager> CONFIGURATIONS Debug DESTINATION bin COMPONENT development)
endif()

message(STATUS "Dependency path: ${DEP_PATH}")

# For the RoiManager target
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_custom_command(TARGET RoiManager POST_BUILD
    COMMAND ${CMAKE_COMMAND}
        -D TARGET_FILE=$<TARGET_FILE:RoiManager>
        -D INSTALL_BIN=${CMAKE_INSTALL_PREFIX}/$<IF:$<BOOL:${WIN32}>,bin,lib/cvutil>
        -D SEARCH_DIRS=${DEP_PATH}
        -D BUILD_CONFIG=$<CONFIG>
        -D BINARY_DIR=${CMAKE_BINARY_DIR}
        -P "${CMAKE_CURRENT_SOURCE_DIR}/../CMake/find_runtime_deps.cmake"
    COMMENT "Copying runtime dependencies for RoiManager"
    )
endif()
