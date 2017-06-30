// Fichero : detector_id.cc
// Descripcion : Implementacion de la clade Detector_id
// Version : 1.0
// Fecha : 3/12/99

#ifdef TRAZAS_CC
#include <fstream>
#endif
#ifdef TRAZAS_C
#include <stdio.h>
#endif

#ifndef WIN32
#include <unistd.h>
#include <strstream>
#else 
#include <windows.h>
#include <strstrea>
#endif

#ifndef s25o22mt
#include <iostream>
#else
#include <iostream.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <math.h>


#include "Marcado.h"
#include "Pares_caracteres.h"
#include "Pares_secuencias.h"
#include "Bigrama.h"
#include "dicc.h"
#include "detector_id.h"
#include "Det_id_interno.h"
#include "detector_id_Privado.h"
#include "trata_car.h"

//Imágenes en memoria de tablas hard-coded
#include "img_parcar_validos.h"
#include "img_parcar_segmpal.h"
#include "img_parsec_segmpal.h"
#include "img_parsec_markup.h"
#include "img_parsec_maymin.h"


// Metodo : Detector_id
// Descripcion : Constructor por defecto.
// Parametros : Ninguno.
Detector_id::Detector_id(void)
{
  p_privado = new Detector_id_Privado();
}


// Metodo : ~Detector_id
// Descripcion : Destructor por defecto.
// Parametros : Ninguno.
Detector_id::~Detector_id(void)
{

#ifdef PROBLEMAS_PENDIENTES
  // Al destruirse el objeto Detector_id (debería haber sólo uno)
  // además de borrarse la lista linfo_id es necesario liberara la
  // memoria de los objetos Diccionario y Bigrama creados dinámicamente.
  // Para ello vamos a recorrer la lista linfo_id y descargando los
  // idiomas uno a uno.
  // No es muy eficiente, pero sólo debería hacerse cuando se va a 
  // dejar de usar el detector, por lo que no es muy crítico.

  Lista<Info_id>::Posicion p;

  for (p=0; p<linfo_id.Tamanno(); p++)
    Descarga(linfo_id[p].idioma);

#endif //PROBLEMAS_PENDIENTES

  delete p_privado;

}
 
// Metodo : Carga
// Descripcion : Carga un idioma.
// Parametros : Idioma y fichero con el modelo de lenguaje asociado al idioma.
// Valor devuelto: Código de error de los definidos en la clase Detector_id.
Detector_id::Error Detector_id::Carga(Idioma_detid idi, const char *fich_mod_idioma)
{
  Det_id_interno::Error error;

  Det_id_interno::Idioma_detid idi_interno;

  idi_interno = (Det_id_interno::Idioma_detid) idi;
  error = p_privado->det_id.Carga(idi_interno, fich_mod_idioma);

  if (error == Det_id_interno::OK)
  {
    return OK;
  }
  else if (error == Det_id_interno::VERSION_INCORRECTA)
  {
    return VERSION_INCORRECTA;
  }
  else if (error == Det_id_interno::IDIOMA_INCORRECTO)
  {
    return IDIOMA_INCORRECTO;
  }
  else if (error == Det_id_interno::IDIOMA_YA_CARGADO)
  {
    return IDIOMA_YA_CARGADO;
  }
  else if (error == Det_id_interno::IDIOMA_NO_CARGADO)
  {
    return IDIOMA_NO_CARGADO;
  }
  else if (error == Det_id_interno::MODELO_IDIOMA_NO_VALIDO)
  {
    return MODELO_IDIOMA_NO_VALIDO;
  }
  else if (error == Det_id_interno::ERROR_FICHERO)
  {
    return ERROR_FICHERO;
  }
  else if (error == Det_id_interno::ERROR_MAPEO)
  {
    return ERROR_MAPEO;
  }
  else if (error == Det_id_interno::OK)
  {
	return OK;
  }
  return ERROR_MAPEO;
}

// Metodo : Descarga
// Descripcion : Descarga un idioma.
// Parametros : Idioma
// Valor devuelto: Código de error de los definidos en la clase Detector_id
Detector_id::Error Detector_id::Descarga(Idioma_detid idi)
{
  Det_id_interno::Error error;
  Det_id_interno::Idioma_detid idi_interno;

  idi_interno = (Det_id_interno::Idioma_detid) idi;

  error = p_privado->det_id.Descarga(idi_interno);

  if (error == Det_id_interno::OK)
  {
    return OK;
  }
  else if (error == Det_id_interno::VERSION_INCORRECTA)
  {
    return VERSION_INCORRECTA;
  }
  else if (error == Det_id_interno::IDIOMA_INCORRECTO)
  {
    return IDIOMA_INCORRECTO;
  }
  else if (error == Det_id_interno::IDIOMA_YA_CARGADO)
  {
    return IDIOMA_YA_CARGADO;
  }
  else if (error == Det_id_interno::IDIOMA_NO_CARGADO)
  {
    return IDIOMA_NO_CARGADO;
  }
  else if (error == Det_id_interno::MODELO_IDIOMA_NO_VALIDO)
  {
    return MODELO_IDIOMA_NO_VALIDO;
  }
  else if (error == Det_id_interno::ERROR_FICHERO)
  {
    return ERROR_FICHERO;
  }
  else if (error == Det_id_interno::ERROR_MAPEO)
  {
    return ERROR_MAPEO;
  }
  else if (error == Det_id_interno::OK)
  {
	return OK;
  }
  return ERROR_MAPEO;

}


Detector_id::Idioma_detid Detector_id::Detecta(const char *texto, int *conf)
{
  Det_id_interno::Idioma_detid idioma;

  Det_id_interno::Confianza conf_interna;

  idioma = p_privado->det_id.Detecta(texto, conf_interna);
  *conf = (int) conf_interna;
  return((Detector_id::Idioma_detid)idioma);

}


Detector_id::Idioma_detid cad2idi_detid(char *cad)
{
  if (strcmp(cad, "CASTELLANO") == 0) return Detector_id::CASTELLANO;
  if (strcmp(cad, "CATALAN") == 0) return Detector_id::CATALAN;
  if (strcmp(cad, "GALLEGO") == 0) return Detector_id::GALLEGO;
  if (strcmp(cad, "EUSKERA") == 0) return Detector_id::EUSKERA;
  if (strcmp(cad, "INGLES") == 0) return Detector_id::INGLES;
  if (strcmp(cad, "FRANCES") == 0) return Detector_id::FRANCES;
  if (strcmp(cad, "ALEMAN") == 0) return Detector_id::ALEMAN;
  if (strcmp(cad, "BRASILEÑO") == 0) return Detector_id::BRASILENO;
  if (strcmp(cad, "ITALIANO") == 0) return Detector_id::ITALIANO;
  return Detector_id::IDIOMA_NULO;
}
 
 
char *idi2cad_detid(Detector_id::Idioma_detid idioma)
{
  static char ptrcas[]="CASTELLANO";
  static char ptrcat[]="CATALAN";
  static char ptrgal[]="GALLEGO";
  static char ptreus[]="EUSKERA";
  static char ptring[]="INGLES";
  static char ptrfra[]="FRANCES";
  static char ptrale[]="ALEMAN";
  static char ptrbra[]="BRASILEÑO";
  static char ptrita[]="ITALIANO";
  static char ptrnulo[]="IDIOMA_NULO";
 
  switch (idioma)
  {
    case Detector_id::CASTELLANO:
      return ptrcas;
      break;
    case Detector_id::CATALAN:
      return ptrcat;
      break;
    case Detector_id::GALLEGO:
      return ptrgal;
      break;
    case Detector_id::EUSKERA:
      return ptreus;
      break;
    case Detector_id::INGLES:
      return ptring;
      break;
    case Detector_id::FRANCES:
      return ptrfra;
      break;
    case Detector_id::ALEMAN:
      return ptrale;
      break;
    case Detector_id::BRASILENO:
      return ptrbra;
      break;
    case Detector_id::ITALIANO:
      return ptrita;
      break;
    default:
      return ptrnulo;
      break;
  }
}

extern "C" void Inicializa_mod_det_id_int_c(void **modelo)
{
  *modelo = new Detector_id;
}


extern "C" void Carga_mod_det_id_int_c(char *cad_idioma, const char *fich_mod_idioma, void *modelo)
{
  ((Detector_id *)modelo)->Carga(cad2idi_detid(cad_idioma), fich_mod_idioma);

}

extern "C" void Descarga_mod_det_id_int_c(char *cad_idioma, void *modelo)
{
  ((Detector_id *)modelo)->Descarga(cad2idi_detid(cad_idioma));

}

extern "C" void Detecta_idioma_int_c(const char *texto, char **cad_idioma, int *confianza, void *modelo)
{
  Detector_id::Idioma_detid idioma;
  int *conf = NULL;

  idioma = ((Detector_id *)modelo)->Detecta(texto, conf);
  *cad_idioma = idi2cad_detid(idioma);
  *confianza = *conf;
}

