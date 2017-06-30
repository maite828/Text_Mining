// Fichero : Det_id_interno.h
// Descripcion : Detector de idiomas.
// Version : 1.0
// Fecha : 9/12/1999

#ifndef Det_id_interno_h
#define Det_id_interno_h

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

#ifndef DllExport
#ifdef WIN32
#define DllExport _declspec(dllexport)
#else
#define DllExport
#endif
#endif

#define FICH_MAPEADO 

// Clase  : Det_id_interno
// Descripcion : Clase que permite configurar varios idiomas para 
//     realizar la deteccion de a cual de ellos pertenece un trozo de texto.
class Det_id_interno
{
  private :

// Longitud de un unsigned long
#define SIZE_ENT 4

// Máximo número de caracteres por palabra válida para detección.
// Conviene dejarlo bastante ajustado para evitar secuencias de caracteres
// sin sentido pero no muy largas que aparecían en el texto de entrenamiento
// de los bigramas (por ejemplo imágenes codificadas como texto, al estilo de
// las de los ficheros postscript).
#define MAXLONGPAL_DET_ID 20 

// Mínimo número de caracteres por palabra válida para detección.
// En general aceptaremos cualquier palabra por pequeña que sea.
#define MINLONGPAL_DET_ID 1

// Número máximo de palabras a estudiar antes de decidir el idioma.
#define MAXNUMPAL_DET_ID 200

  // Versión
#define Version 2   // Debe coincidir con la del fichero
#define Subversion 0 // Puede no coincidir con la del fichero.

#define Tabla_id 0x5AA55AA5  // Primer long del modelo de idioma.


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
  enum Idioma_detid
  {
    IDIOMA_NULO, //Usado para el caso de que se llame a Detecta sin haber
		 //cargado ningún idioma.
    CASTELLANO, CATALAN, GALLEGO, EUSKERA,
    INGLES, FRANCES, ALEMAN, BRASILENO, ITALIANO
  };

  // Confianza.
  // Normalizada entre 0 y 100.
  typedef int Confianza;

  Marcado marc;

  // Constructores y destructores.
  DllExport  Det_id_interno(void);
  DllExport  ~Det_id_interno(void);

  // Administracion de idiomas.
  Error Carga(Idioma_detid, const char *fich_mod_idioma);
  Error Descarga(Idioma_detid);

  // Deteccion.
  DllExport  Idioma_detid Detecta(const char *, Confianza &);

  // Generación del modelo de idioma binario.
  DllExport  Error Gen_mod_bin(Idioma_detid idi, const char *dic_ascii, const char *big_ascii,
		    const char *mod_id_bin);

  //Métodos útiles para acceder e identifiacr el Idioma
  Idioma_detid cad2idi_detid(char *cad);
  char *idi2cad_detid(Idioma_detid idioma);

/* ----- No la vamos a usar más -----
  // Separación de la entrada (para ser utilizada en el entrenamiento del 
  // modelo de bigramas)
  static int Sgte_pal(std::ifstream &ftexto, char *pal);
   ----- No la vamos a usar más ----- */

  // Segmentación de la entrada (para obten_palabras)
  // Supone que sólo hay un modelo de idioma cargado y toma el que
  // está en la primera posición de la lista.
  char *Sgte_pal_obtenpal(const char *texto, const char *fin_texto,
                          char *palabra);

  private :

  // Unsigned int de identificación de tablas
  // Esta palabra debe aparecer al principio de
  // todos los modelos de idioma manejados por 
  // el detector de idioma.

  // Agentes relacionados con los idiomas.
  // Clase que mantiene toda la información relacionada
  // con un idioma.

  class Info_id // En realidad es una struct, pero la ponemos como una class
  {
    public:
    Idioma_detid idioma;         // Identificador del idioma a que pertenece

#ifdef WIN32
#ifdef FICH_MAPEADO
	LPVOID inicio_tabla; // Es necesario para liberar la memoria
	HANDLE fmod_idi;
#else
	char *inicio_tabla;
	int fmod_idi;
#endif
#else
	void * inicio_tabla; // Es necesario para liberar la memoria
#endif
    
    unsigned long tam;    // También necesario para liberar memoria
    Pares_caracteres *parcar_segmpal; // Puntero a un objeto Pares_caracteres
                                      // Caracteres válidos en el idioma
                                      // en cuestión. Esta lista de pares de
                                      // caracteres es la que se va a usar para
                                      // determinar que sustituciones e
                                      // inserciones probar. Sólo debe contener
                                      // minúsculas.
                                      // El carácter asociado debe ser él mismo.
    Pares_caracteres *parcar_validos; // Puntero a un objeto Pares_caracteres
                                      // Caracteres válidos en el idioma
                                      // en cuestión. Esta lista de pares de
                                      // caracteres es la que se va a usar para
                                      // determinar que sustituciones e
                                      // inserciones probar. Sólo debe contener
                                      // minúsculas.
                                      // El carácter asociado debe ser él mismo.
    Pares_secuencias *parsec_segmpal; // Puntero a un objeto Pares_secuencias
                                      // Secuencias de caracteres válidas
                                      // dentro de una palabra en el idioma en
                                      // cuestión. La secuencia se sustituye
                                      // por la secuencia asociada.
                                      // Se utiliza para segmentar en palabras.
    Pares_secuencias *parsec_markup;  // Puntero a un objeto Pares_secuencias
                                      // Contiene los códigos de marcado que
                                      // serán detectados por las funciones
                                      // de segmentación en palabras. Estos
                                      // códigos en lugar de definirse en una
                                      // tabla se definen en el fichero
                                      // img_parsec_markup.h. La función de
                                      // carga del modelo de idioma se encarga
                                      // de inicializar este objeto.
    Pares_secuencias *parsec_maymin;  // Puntero a un objeto Pares_secuencias
                                      // Indica las palabras a corregir en
                                      // función del patrón de may/min. Se
                                      // puede especificar que se corrija
                                      // "CORR" o no "NOCORR" (o cualquier
                                      // otra cosa) en función de una
                                      // combinación de si el primer carácter
                                      // era mayúscula 'M' o minúscula 'm' y
                                      // de si el resto eran mayúsculas 'M',
                                      // minúsculas 'm' o una combinación 'X'

    char letras_validas[256];
                           // Array ASCIIZ con las letras que aparecen
                           // asociadas en parcar_validos (en otras palabras,
                           // vienen a ser las letras consideradas válidas en
                           // cada idioma, en la representación interna que
                           // usa el corrector (minúsculas).
                           // Este array se calcula durante la carga del
                           // modelo de idioma a partir de parcar_validos

    Diccionario *dic;  // Es un puntero al objeto de clase Diccionario
		       // asociado a este idioma.
    Bigrama *big;      // Es un puntero al objeto de clase Bigrama
		       // asociado a este idioma.
  };

  Lista<Info_id> linfo_id; // Lista con un objeto Info_id por cada idioma
		           // Cargado.

/* ----- No la vamos a usar más -----
  // Separacion de la entrada.
  char *Sgte_pal(const char *, char *);
   ----- No la vamos a usar más ----- */

  // Separación de la entrada. Función llamada por Sgte_pal_obtenpal.
  char *Sgte_pal_obtenpal_1(const char *texto, const char *fin_texto,
                            char *palabra, int &valida);
 
  // Separacion de la entrada. - Entrada=cadena ASCIIZ empezando en texto
  char *Sgte_pal(const char *texto, char *palabra, Info_id &info_id, 
				 std::ostrstream &sal);
 
  // Separacion de la entrada. - Entrada=cadena ASCIIZ empezando en texto
  // Función llamada por Sgte_pal
  char *Sgte_pal_1(const char *texto, char *palabra, Info_id &info_id,
                   std::ostrstream &sal, int &valida);
 
  // Separacion de la entrada. - Entrada=buffer en memoria, empieza en
  // inicio_texto y termina en final_texto-1.
  char *Sgte_pal(const char *inicio_texto, const char *final_texto,
                 char *palabra, Info_id &info_id, std::ostrstream &sal);
 
  // Separacion de la entrada. - Entrada=buffer en memoria, empieza en
  // inicio_texto y termina en final_texto-1.
  // Función llamada por Sgte_pal
  char *Sgte_pal_1(const char *inicio_texto, const char *final_texto,
                   char *palabra, Info_id &info_id, std::ostrstream &sal,
                   int &valida);

  // Transformación a minúsculas.
  void Transforma_a_min(char *cad_orig_may,
                                         char *cad_orig_min,
                                         char *cambios_may_min);
 
  // Restauración de las mayúsculas.
  void Restaura_may(char *cad_orig_min,
                    char *cambios_may_min,
                    char *cad_corr_min,
                    char *cad_corr_may);
 
  // Obtención del patrón de mayúsculas/minúsculas para decidir si corregir
  char *Patron_maymin(char *cambios_maymin);

  // Alineacion de palabras original y corregida.
  int Alinear(char *cad_in, char *cad_out, char *sucesos);


};


#endif // Det_id_interno_h
