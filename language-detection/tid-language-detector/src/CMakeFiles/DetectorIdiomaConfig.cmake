# - Config file for the DetectorIdioma package
# It defines the following variables
#  DETECTORIDIOMA_INCLUDE_DIRS - include directories for DetectorIdioma
#  DETECTORIDIOMA_LIBRARIES    - libraries to link against
#  DETECTORIDIOMA_EXECUTABLE   - the det_id_analytics executable
 
# Compute paths
get_filename_component(DETECTORIDIOMA_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)
set(DETECTORIDIOMA_INCLUDE_DIRS "${DETECTORIDIOMA_CMAKE_DIR}/../../../libs/include")
 
# Our library dependencies (contains definitions for IMPORTED targets)
if(NOT TARGET det_id AND NOT DetectorIdioma_BINARY_DIR)
  include("${DETECTORIDIOMA_CMAKE_DIR}/DetectorIdiomaTargets.cmake")
endif()
 
# These are IMPORTED targets created by DetectorIdiomaTargets.cmake
set(DETECTORIDIOMA_LIBRARIES det_id)
set(DETECTORIDIOMA_EXECUTABLE det_id_analytics)
