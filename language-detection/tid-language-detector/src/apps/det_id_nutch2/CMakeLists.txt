add_executable(det_id_nutch2 det_id_nutch2.cpp)
 
target_link_libraries(det_id_nutch2 det_id)
 
install(TARGETS det_id_nutch2
  # IMPORTANT: Add the det_id_nutch2 executable to the "export-set"
  EXPORT DetectorIdiomaTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin)
