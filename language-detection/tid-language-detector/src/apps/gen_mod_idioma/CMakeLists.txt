add_executable(gen_mod_idioma gen_mod_idioma.cpp)
 
target_link_libraries(gen_mod_idioma det_id)
 
install(TARGETS gen_mod_idioma
  # IMPORTANT: Add the gen_mod_idioma executable to the "export-set"
  EXPORT DetectorIdiomaTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin)
