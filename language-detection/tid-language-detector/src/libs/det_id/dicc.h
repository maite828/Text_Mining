// Fichero : dicc.h
// Descripcion : Interfaz del diccionario.
// Fecha : 22/11/99

#ifndef dicc_h
#define dicc_h


#ifndef WIN32
#include <sys/mman.h>
#else 
#include <windows.h>
#endif

#include <stdint.h>
#include <string.h>
#include <fstream>

// Clase : Diccionario.
// Descripcion : Clase que representa un diccionario.
//       Admite dos representaciones externas que se dan a continuacion.
//
//       <FICH_ASCII> := { <frec> <cadena> }
//
//       <FICH_BINARIO> := <num_subdicc> { <OFFSET> }(num_subdicc) { <cadena> <frec> <OFFSET> }
//       <OFFSET> := OFFSET_NULO | <entero>
//       <cadena> := ASCIIZ
//                Tamanno multiplo del tamanno de un <entero>.
//       <frec> := <entero>
//       <num_subdicc> := <entero>
//       <entero> := entero sin signo de 4 bytes de longitud.
//
class Diccionario
{
  public :
  // Tipo para interpretar el fichero binario.
  // Debe tener 4 bytes e interpretarse como un entero sin signo.
  typedef uint32_t Entero;
#define SIZE_ENT 4

  // Offset dentro del fichero. (Formato externo binario.)
  typedef Entero Offset;
#define OFFSET_NULO 0

  // Suelo de probabilidad de aparición de una palabra en el diccionario.
#define MIN_FREC_DICCIO 5.0e-7

  // Entrada en el diccionario.
  class Entrada
  {
    private :
    const char *palabra;
    float frec;
    Offset sgte;

    public :
    // Constructores y destructores.
    Entrada(const char *, const float &, const Offset &);
    ~Entrada(void);

    // Acceso.
    const char *Palabra(void);
    float Frec(void);
    Offset Sgte(void);
  };

  class Cadena
  {
    private :
    char *cad;
 
    public :
    // Constructores y destructores.
    Cadena(void)
    : cad(0)
    {
    }
 
    Cadena(const char *c)
    {
      if (c == 0)
        cad = 0;
      else
      {
        cad = new char[strlen(c)+1];
        strcpy(cad, c);
      }
    }
 
    Cadena (const Cadena &c)
    : cad(0)
    {
      *this = c;
    }
 
    ~Cadena(void)
    {
      if (cad != 0)
        delete [] cad;
    }
 
    // Operador de asignacion.
    const Cadena &operator=(const Cadena &c)
    {
      if (cad != 0)
        delete [] cad;
 
      if (c.cad == 0)
        cad = 0;
      else
      {
        cad = new char[strlen(c.cad)+1];
        strcpy(cad, c.cad);
      }
 
      return c;
    }
 
    // Operador de igualdad.
    int operator==(const Cadena &c)
    {
      return (strcmp(cad, c.cad) == 0);
    }
 
    // Operador de acceso en lectura.
    const char *operator()(void)
    {
      return cad;
    }
  };

  class Entrada_ASCII
  {
    public:

    //Diccionario::Cadena cad;
	Cadena cad;
    float fre;

    int operator==(const Entrada_ASCII &op)
    {
      return((cad==op.cad)&&(fre==op.fre));
    }
  };
 

  // Direccion del mapeo del fichero que contiene el diccionario.
#ifdef WIN32
#ifdef FICH_MAPEADO
  LPVOID dir_dicc;
  LPVOID dir_orig;
  HANDLE fdicc_h;
#else
  char *dir_dicc;
  char *dir_orig;
#endif
#else
  void * dir_dicc;
  void * dir_orig;
#endif

  // Tamanno del fichero.
  size_t tam;

  public :
  // Constructores y destructores.


#ifdef WIN32
  Diccionario(char*, char*);
  //Diccionario(HANDLE);
#else
  Diccionario(int = -1);
  Diccionario(void * dir_dicc, void * dir_orig);
#endif

  ~Diccionario(void);

  // Acceso.
  // Numero de subdiccionarios en que se clasifica.
  Entero N_sdicc(void) const;
  // Offset de inicio del subdiccionario indicado.
  Offset Inicio(Entero) const;
  // Obtencion de la entrada que se encuentra en el offset indicado.
  Entrada Dame(const Offset &) const;

  // Test.
  int Test(void) const;

  // Conversion de formatos externos.
  void Convierte(const char *, const char *, Entero = 4001);
  void Convierte(std::ifstream &, std::ofstream &, Entero = 4001);
  
  // Busqueda de una entrada.
  double Busca(const char *) const;

  private :
  // Resolucion de las direcciones de memoria.
  char *Resuelve(const Offset &) const;

  // Filtrado de cadenas.
  void Filtra(const char *, char *) const;
};

#endif  // dicc_h

