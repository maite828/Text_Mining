// Fichero : dicc.h
// Descripcion : Interfaz del diccionario.
// Fecha : 22/11/99

#ifndef dicc_h
#define dicc_h

#include <stdint.h>
#include <fstream>
#include <sys/mman.h>
#include <string.h>

// Clase : Diccionario.
// Descripcion : Clase que representa un diccionario.
//       Admite dos representaciones externas que se dan a continuacion.
//
//       <FICH_ASCII> := { <cadena> }
//
//       <FICH_BINARIO> := <num_subdicc> { <OFFSET> }(num_subdicc) { <cadena> <OFFSET> }
//       <OFFSET> := OFFSET_NULO | <entero>
//       <cadena> := ASCIIZ
//                Tamanno multiplo del tamanno de un <entero>.
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

  // Entrada en el diccionario.
  class Entrada
  {
    private :
    const char *palabra;
    Offset sgte;

    public :
    // Constructores y destructores.
    Entrada(const char *, const Offset &);
    ~Entrada(void);

    // Acceso.
    const char *Palabra(void);
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
 

  // Direccion del mapeo del fichero que contiene el diccionario.
  void * dir_dicc;
  // Tamanno del fichero.
  size_t tam;

  public :
  // Constructores y destructores.
  Diccionario(int = -1);
  Diccionario(void *);
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
  void Convierte(ifstream &, ofstream &, Entero = 4001);
  
  // Busqueda de una entrada.
  int Busca(const char *) const;

  private :
  // Resolucion de las direcciones de memoria.
  char *Resuelve(const Offset &) const;

  // Filtrado de cadenas.
  void Filtra(const char *, char *) const;
};

#endif  // dicc_h

