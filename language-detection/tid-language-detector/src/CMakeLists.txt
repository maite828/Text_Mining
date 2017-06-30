cmake_minimum_required(VERSION 2.8)
project(DetectorIdioma)
 
set(DETECTORIDIOMA_MAJOR_VERSION 0)
set(DETECTORIDIOMA_MINOR_VERSION 1)
set(DETECTORIDIOMA_PATCH_VERSION 0)
set(DETECTORIDIOMA_VERSION
  ${DETECTORIDIOMA_MAJOR_VERSION}.${DETECTORIDIOMA_MINOR_VERSION}.${DETECTORIDIOMA_PATCH_VERSION})
 
# Baseline compiler flags, any change here will affect all build types
set (CMAKE_CXX_FLAGS "-g -Wno-unknown-pragmas -D_LARGEFILE64_SOURCE -D_GNU_SOURCE -Wno-deprecated ")


# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR ./bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR libs/include CACHE PATH
  "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/CMake/DetectorIdioma)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")
 
# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()
 
# set up include-directories
include_directories(
  "${PROJECT_SOURCE_DIR}"   # to find det_id/det_id.h
  "${PROJECT_BINARY_DIR}")  # to find det_id/config.h
 
# Add sub-directories
add_subdirectory(libs)
add_subdirectory(apps)
 
# The interesting stuff goes here
# ===============================
 
# Add all targets to the build-tree export set
export(TARGETS det_id det_id_analytics gen_mod_idioma
  FILE "${PROJECT_BINARY_DIR}/DetectorIdiomaTargets.cmake")
 
# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE DetectorIdioma)
 
# Create the DetectorIdiomaConfig.cmake and DetectorIdiomaConfigVersion files
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
   "${INSTALL_INCLUDE_DIR}")
# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file(DetectorIdiomaConfig.cmake.in
  "${PROJECT_BINARY_DIR}/DetectorIdiomaConfig.cmake" @ONLY)
# ... for the install tree
set(CONF_INCLUDE_DIRS "\${DETECTORIDIOMA_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(DetectorIdiomaConfig.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/DetectorIdiomaConfig.cmake" @ONLY)
# ... for both
configure_file(DetectorIdiomaConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/DetectorIdiomaConfigVersion.cmake" @ONLY)
 
# Install the DetectorIdiomaConfig.cmake and DetectorIdiomaConfigVersion.cmake
install(FILES
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/DetectorIdiomaConfig.cmake"
  "${PROJECT_BINARY_DIR}/DetectorIdiomaConfigVersion.cmake"
  DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)
 
# Install the export set for use with the install-tree
install(EXPORT DetectorIdiomaTargets DESTINATION
  "${INSTALL_CMAKE_DIR}" COMPONENT dev)
