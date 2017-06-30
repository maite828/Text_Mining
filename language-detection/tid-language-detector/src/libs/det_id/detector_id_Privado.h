// Fichero : detector_id.h
// Descripcion : Detector de idiomas.
// Version : 1.0
// Fecha : 9/12/1999

#ifndef detector_id_privado_h
#define detector_id_privado_h

#ifdef WIN32
#include <strstrea>
#else
#include <strstream>
#endif
 
#include "lista.h"
#include "trata_car.h"

#include "Marcado.h"
#include "Pares_caracteres.h"
#include "Pares_secuencias.h"
#include "dicc.h"
#include "Bigrama.h"
#include "Det_id_interno.h"


// Para evitar manejar todos los includes:
 
struct Detector_id_Privado
{
  Det_id_interno det_id;
};

#endif // detector_id_privado_h
