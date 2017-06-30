add_executable(det_id_analytics det_id_analytics.cpp)
 
target_link_libraries(det_id_analytics det_id)
 
install(TARGETS det_id_analytics
  # IMPORTANT: Add the det_id_analytics executable to the "export-set"
  EXPORT DetectorIdiomaTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin)
