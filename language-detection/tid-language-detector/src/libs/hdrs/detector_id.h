// Fichero : detector_id.h
// Descripcion : Detector de idiomas.
// Version : 1.0
// Fecha : 9/12/1999

#ifndef detector_id_h
#define detector_id_h


#ifndef DllExport
#ifdef WIN32
#define DllExport _declspec(dllexport)
#else
#define DllExport
#endif
#endif

// Para evitar manejar todos los includes:
 
class Detector_id_Privado;


// Clase  : Detector_id
// Descripcion : Clase que permite configurar varios idiomas para 
//     realizar la deteccion de a cual de ellos pertenece un trozo de texto.
class Detector_id
{

  private:
    Detector_id_Privado *p_privado;

  public :

  // Tipo de datos para los errores.
  enum Error
  {
    OK,
    VERSION_INCORRECTA,
    IDIOMA_INCORRECTO,
    IDIOMA_YA_CARGADO,
    IDIOMA_NO_CARGADO,
    MODELO_IDIOMA_NO_VALIDO,
    ERROR_FICHERO,
    ERROR_MAPEO
  };

  // Tipos de datos para los parametros.
  // Idioma
  enum DllExport Idioma_detid
  {
    IDIOMA_NULO, //Usado para el caso de que se llame a Detecta sin haber
		 //cargado ningún idioma.
    CASTELLANO, CATALAN, GALLEGO, EUSKERA,
    INGLES, FRANCES, ALEMAN, BRASILENO, ITALIANO
  };

  // Constructores y destructores.
  DllExport  Detector_id(void);
  DllExport  ~Detector_id(void);

  // Administracion de idiomas.
  DllExport  Error Carga(Idioma_detid, const char *fich_mod_idioma);
  DllExport  Error Descarga(Idioma_detid);

  // Deteccion.
  DllExport  Idioma_detid Detecta(const char *, int *);

};


extern "C" void Inicializa_mod_det_id_int_c(void **modelo);
extern "C" void Carga_mod_det_id_int_c(char *cad_idioma, const char *fich_mod_idioma, void *modelo);
extern "C" void Descarga_mod_det_id_int_c(char *cad_idioma, void *modelo);
extern "C" void Detecta_idioma_int_c(const char *texto, char **cad_idioma, int *confianza, void *modelo);

#endif // detector_id_h
