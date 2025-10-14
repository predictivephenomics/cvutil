# Common settings regardless of environment
set(CPACK_DEBIAN_PACKAGE_SECTION "libs")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")

# CPack configuration for DEB packaging
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME "cvutil")
set(CPACK_PACKAGE_VENDOR "Oak Ridge National Laboratory")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Computer Vision UTILity Toolkit")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_CONTACT "seethepallia@ornl.gov")

# DEB-specific configuration
set(CPACK_DEBIAN_PACKAGE_SECTION "science")
set(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT) # Use standard naming convention

# # Bundle all runtime dependencies - reduce external dependencies to minimum
# set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS OFF) # Don't auto-generate dependencies

# # Only set minimal dependencies needed for the package to function
# set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.27), libstdc++6 (>= 8.0)")

# # This gets the path to the directory containing the Qt6Charts shared library
# get_target_property(Qt6ChartsLibLocation Qt6::Charts LOCATION)

# # Extract the directory
# get_filename_component(Qt6ChartsLibDir "${Qt6ChartsLibLocation}" DIRECTORY)

# Enable automatic dependency detection
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

# set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS_PRIVATE_DIRS 
#     "${CMAKE_INSTALL_PREFIX}/lib/cvutil")

# # Exclude bundled libraries from dependency generation
# set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS_PRIVATE_DIRS 
#     "${CMAKE_INSTALL_PREFIX}/lib/cvutil;${Qt6ChartsLibDir}")

# Explicitly set runtime package dependencies.
# Current minimum requirements for Debian 12 (bookworm) and later.
# The list of libraries are Qt6 Core, Widgets, Gui, OpenGL, Charts modules.
# And the OpenCV libraries that CVUtil depends on.
set(CPACK_DEBIAN_RUNTIME_PACKAGE_DEPENDS 
    "libc6 (>= 2.34), libstdc++6 (>= 11), libqt6core6 (>= 6.4.0), libqt6widgets6 (>= 6.4.0), libqt6gui6 (>= 6.4.0), libqt6opengl6 (>= 6.4.0), libqt6charts6 (>= 6.4.0), libopencv-core406t64, libopencv-imgproc406t64, libopencv-highgui406t64, libopencv-videoio406t64")

set(CPACK_DEBIAN_DEVELOPMENT_PACKAGE_DEPENDS 
    "libc6-dev (>= 2.34), libstdc++-dev (>= 11), libqt6core6-dev (>= 6.4.0), libqt6widgets6-dev (>= 6.4.0), libqt6gui6-dev (>= 6.4.0), libqt6opengl6-dev (>= 6.4.0), libqt6charts6-dev (>= 6.4.0), libopencv-dev (>= 4.5)")

# Set installation directory for bundled libraries (follows FHS standard)
set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")

# Create component-based installation for proper Debian packaging
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_COMPONENTS_ALL runtime development)

# Runtime component (shared libraries)
set(CPACK_COMPONENT_RUNTIME_DISPLAY_NAME "CVUtil Runtime Libraries")
set(CPACK_COMPONENT_RUNTIME_DESCRIPTION "Shared libraries for CVUtil")
set(CPACK_DEBIAN_RUNTIME_PACKAGE_NAME "libcvutil")
set(CPACK_DEBIAN_RUNTIME_PACKAGE_SECTION "libs")

# Development component (headers, static libraries, CMake configs)
set(CPACK_COMPONENT_DEVELOPMENT_DISPLAY_NAME "CVUtil Development Files")
set(CPACK_COMPONENT_DEVELOPMENT_DESCRIPTION "Development files for CVUtil")
set(CPACK_DEBIAN_DEVELOPMENT_PACKAGE_NAME "libcvutil-dev")
set(CPACK_DEBIAN_DEVELOPMENT_PACKAGE_SECTION "libdevel")
set(CPACK_DEBIAN_DEVELOPMENT_PACKAGE_DEPENDS "libcvutil (= ${CPACK_PACKAGE_VERSION})")

# Handle copyright and changelog files required by Debian
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/debian/copyright" 
               "${CMAKE_CURRENT_BINARY_DIR}/copyright" COPYONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/copyright"
        DESTINATION "share/doc/cvutil")

# Create Debian package metadata files
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/debian/control.in"
    "${CMAKE_CURRENT_BINARY_DIR}/debian/control"
    @ONLY
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/debian/compat"
    "${CMAKE_CURRENT_BINARY_DIR}/debian/compat"
    COPYONLY
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/debian/control"
    "${CMAKE_CURRENT_BINARY_DIR}/debian/compat"
    DESTINATION "share/doc/${CPACK_PACKAGE_NAME}/DEBIAN"
)

include(CPack)
