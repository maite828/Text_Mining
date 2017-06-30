// Fichero : Bigrama.h
// Descripción : Interfaz del modelo de bigramas
// Fecha : 14 Dic 1999

#ifndef Bigrama_h
#define Bigrama_h

#ifndef WIN32
#include <sys/mman.h>
#else 
#include <windows.h>
#endif

#include <fstream>
#include <math.h>

// Clase : Bigrama
// Descripción: Es una clase que representa un modelo de probabilidad de 
//              bigramas en distintos idiomas, así como un conjunto de 
//              operaciones con necesarias para su manejo y para la detección
//              de idioma basado en bigramas.
// 
//              El modelo de bigramas admite dos representaciones externas, 
//              una en ascii, y otra en binario. El formato ascii se usa sólo
//              en el entrenamiento de los modelos de bigramas. El formato que
//              se utiliza posteriormente en la detección de idioma es el 
//              formato binario, que va incluido en el modelo de idioma, junto
//              con otras tablas empleadas en la detección de idioma.
//
//              El formato ascii es como sigue:
//              Línea 1: "<MODELO DE IDIOMA V2.000000>"
//              Línea 2: Un conjunto de números enteros organizado como una
//                       matriz cuadrada que da el número de apariciones de 
//                       cada bigrama en el entrenamiento. Estos números están
//                       en ascii.
//
//              El formato binario no es más que una matriz cuadrada de
//              floats obtenidos como el log10 de la probabilidad de aparición
//              de cada uno de los bigramas en el idioma.
//              Esta es también la representación interna que usa esta clase,
//              un array bidimensional de floats.

class Bigrama
{

// Número de códigos de carácter válidos devueltos por la función 
// Bigrama::codifica_letra. Van de 0 a 58.
#define NUMCOD_BIG 59

// Versión del modelo de bigramas
#define VERSION_BIG 2.0

  public: 

  // Constructores y Destructores
#ifdef WIN32
  Bigrama(LPVOID inicio_big);
#else
  Bigrama(void * inicio_big);
#endif

  ~Bigrama();

  // Función para convertir del fomato ascii al binario
  static int Convierte(std::ifstream &fbig, std::ofstream &fbinario);

  // Función para puntuar una palabra
  // Esta función devuelve la suma de los logprob de aparición de cada uno
  // de los bigramas que componene la palabra. - Se usaba antes del 17 Ene 2000
  double logprob_pal(const char *pal);

  // Función para puntuar una palabra
  // Esta función devuelve la probabilidad media de aparición de los bigramas
  // que componene la palabra. - Se usa desde 17 Ene 2000
  double prob_pal(const char *pal);

  private:

  // Dirección donde está el modelo de bigramas.
#ifdef WIN32
  LPVOID dir_big;
#else
  void * dir_big;
#endif


  public :
  // Funciones Estáticas 

  // Función para codificación de caracteres.
  static unsigned char codifica_letra(unsigned char ch);

  // Función para cargar un modelo de idioma en representación ASCII
  static int carga_mod_idioma(std::ifstream &fbig, long *mod_idioma);
  static int carga_mod_idioma(char *filename, long *mod_idioma);

  // Función para grabar un modelo de idioma en representación ASCII
  static int graba_mod_idioma(long *mod_idioma, char *filename);

  // Función para resetear un modelo de idioma.
  static void reset_mod_idioma(long *mod_idioma);

  // Función para pasar de la versión ascii del modelo de bigramas a la
  // binaria en memoria.
  static void obten_logprob_mod_idioma(long *mod_idioma, 
						float *logprob_mod_idioma);
  
};


#endif // Bigrama_h
