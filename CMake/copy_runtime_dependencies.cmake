# copy_runtime_dependencies.cmake
# Script to copy runtime dependencies post-build

# Include the dependencies file
include("${CMAKE_CURRENT_LIST_DIR}/cvutil_runtime_dependencies-${CONFIG}.cmake" OPTIONAL)

if(DEFINED CVUTIL_RUNTIME_DEPENDENCIES)
  # Remove duplicate entries from the list
  list(REMOVE_DUPLICATES CVUTIL_RUNTIME_DEPENDENCIES)
  
  foreach(dep IN LISTS CVUTIL_RUNTIME_DEPENDENCIES)
    if(EXISTS "${dep}")
      file(COPY "${dep}" DESTINATION "${TARGET_BIN_DIR}")
      message(STATUS "Copied cvutil dependency: ${dep} to ${TARGET_BIN_DIR}")
    else()
      message(WARNING "cvutil dependency not found: ${dep}")
    endif()
  endforeach()
else()
  message(STATUS "No runtime dependencies defined for ${CONFIG} configuration")
endif()
