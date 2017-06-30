// Fichero : Bigrama.h
// Descripci�n : Interfaz del modelo de bigramas
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
// Descripci�n: Es una clase que representa un modelo de probabilidad de 
//              bigramas en distintos idiomas, as� como un conjunto de 
//              operaciones con necesarias para su manejo y para la detecci�n
//              de idioma basado en bigramas.
// 
//              El modelo de bigramas admite dos representaciones externas, 
//              una en ascii, y otra en binario. El formato ascii se usa s�lo
//              en el entrenamiento de los modelos de bigramas. El formato que
//              se utiliza posteriormente en la detecci�n de idioma es el 
//              formato binario, que va incluido en el modelo de idioma, junto
//              con otras tablas empleadas en la detecci�n de idioma.
//
//              El formato ascii es como sigue:
//              L�nea 1: "<MODELO DE IDIOMA V2.000000>"
//              L�nea 2: Un conjunto de n�meros enteros organizado como una
//                       matriz cuadrada que da el n�mero de apariciones de 
//                       cada bigrama en el entrenamiento. Estos n�meros est�n
//                       en ascii.
//
//              El formato binario no es m�s que una matriz cuadrada de
//              floats obtenidos como el log10 de la probabilidad de aparici�n
//              de cada uno de los bigramas en el idioma.
//              Esta es tambi�n la representaci�n interna que usa esta clase,
//              un array bidimensional de floats.

class Bigrama
{

// N�mero de c�digos de car�cter v�lidos devueltos por la funci�n 
// Bigrama::codifica_letra. Van de 0 a 58.
#define NUMCOD_BIG 59

// Versi�n del modelo de bigramas
#define VERSION_BIG 2.0

  public: 

  // Constructores y Destructores
#ifdef WIN32
  Bigrama(LPVOID inicio_big);
#else
  Bigrama(void * inicio_big);
#endif

  ~Bigrama();

  // Funci�n para convertir del fomato ascii al binario
  static int Convierte(std::ifstream &fbig, std::ofstream &fbinario);

  // Funci�n para puntuar una palabra
  // Esta funci�n devuelve la suma de los logprob de aparici�n de cada uno
  // de los bigramas que componene la palabra. - Se usaba antes del 17 Ene 2000
  double logprob_pal(const char *pal);

  // Funci�n para puntuar una palabra
  // Esta funci�n devuelve la probabilidad media de aparici�n de los bigramas
  // que componene la palabra. - Se usa desde 17 Ene 2000
  double prob_pal(const char *pal);

  private:

  // Direcci�n donde est� el modelo de bigramas.
#ifdef WIN32
  LPVOID dir_big;
#else
  void * dir_big;
#endif


  public :
  // Funciones Est�ticas 

  // Funci�n para codificaci�n de caracteres.
  static unsigned char codifica_letra(unsigned char ch);

  // Funci�n para cargar un modelo de idioma en representaci�n ASCII
  static int carga_mod_idioma(std::ifstream &fbig, long *mod_idioma);
  static int carga_mod_idioma(char *filename, long *mod_idioma);

  // Funci�n para grabar un modelo de idioma en representaci�n ASCII
  static int graba_mod_idioma(long *mod_idioma, char *filename);

  // Funci�n para resetear un modelo de idioma.
  static void reset_mod_idioma(long *mod_idioma);

  // Funci�n para pasar de la versi�n ascii del modelo de bigramas a la
  // binaria en memoria.
  static void obten_logprob_mod_idioma(long *mod_idioma, 
						float *logprob_mod_idioma);
  
};


#endif // Bigrama_h
