#configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h" @ONLY)
 
add_library(det_id SHARED 
detector_id.cpp 
detector_id.h 
Alinear.cpp 
big_carga_mod_idioma.cpp 
Big_codifica_letra.cpp 
Big_convierte.cpp 
Big_graba_mod_idioma.cpp 
Big_logprob_pal.cpp 
big_obten_logprob_mod_idioma.cpp 
Big_prob_pal.cpp 
Bigrama.cpp 
Big_reset_mod_idioma.cpp 
detector_id_int_c.h 
det_id_interno.cpp 
dicc.cpp 
gen_cars.c 
Marcado.cpp 
Marcado_URL.cpp 
Pares_caracteres.cpp 
Pares_secuencias.cpp 
Sgte_pal.cpp 
Transformaciones_may_min.cpp)
 
set_target_properties(det_id PROPERTIES
  PUBLIC_HEADER "detector_id.h")
 
install(TARGETS det_id
  # IMPORTANT: Add the det_id library to the "export-set"
  EXPORT FooBarTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/det_id"
    COMPONENT dev)
