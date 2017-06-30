// Fichero : dicc.cc
// Descripcion : Diccionario.
// Fecha : 22/11/99
// Version : 1.0

#ifndef WIN32
#include <unistd.h>
#else 
#include <windows.h>
#include <io.h>
#endif

#include <string.h>
#ifndef s25o22mt
#include <iostream>
#else
#include <iostream.h>
#endif
#include <ctype.h>
#include <errno.h>

#include "lista.h"
#include "hash.h"
#include "trata_car.h"
#include "dicc.h"

///////////////////////////////////////////////////
// Metodos de la clase Diccionario
///////////////////////////////////////////////////

// Metodo : Diccionario
// Descripcion : Constructor por defecto.
// Parametros : Nombre del fichero que almacena el diccionario.
//        Valor por defecto NULL.
#ifdef WIN32
/*Diccionario::Diccionario(HANDLE fdicc)
: dir_dicc(NULL)
{
	if (fdicc == NULL)
    dir_dicc = NULL;
  else
  {
    if (fdicc != NULL)
    {
      dir_dicc = MapViewOfFile(fdicc,FILE_MAP_READ,0,0,0);
      if (dir_dicc == NULL)
	  {
		CloseHandle(fdicc);
		fdicc = NULL;
		std::cout << "Error al mapear " << errno << std::endl;
	  }
    }
  }
}*/
#else
Diccionario::Diccionario(int fdicc)
: dir_dicc(NULL), tam(0)
{
  if (fdicc == -1)
    dir_dicc = NULL;
  else
  {
    tam = lseek(fdicc, 0, SEEK_END);
    if (fdicc != 0)
    {
      dir_dicc = mmap(0, tam, PROT_READ, MAP_PRIVATE, fdicc, 0);
      if (dir_dicc == MAP_FAILED)
        std::cout << "Error al mapear " << errno << std::endl;
    }
  }
  dir_orig = dir_dicc;
}
#endif


// Metodo : Diccionario
// Descripcion : Constructor con espacio ya reservado.
// Parametros : Direccion de inicio del diccionario ya cargado.
#ifdef WIN32
Diccionario::Diccionario(char* dir, char* dir_tabla)
//: dir_dicc(dir)
{
	dir_dicc = dir;
	dir_orig = dir_tabla;
}
#else
Diccionario::Diccionario(void * dir, void * dir_tabla)
: dir_dicc(dir), tam(0)
{
	dir_orig = dir_tabla;
}
#endif

// Metodo : ~Diccionario
// Descripcion : Destructor del diccionario.
// Parametros : Ninguno.
Diccionario::~Diccionario(void)
{
	/* ------- *
#ifdef WIN32
  if ( dir_orig !=NULL)
  {
#ifdef FICH_MAPEADO
	  UnmapViewOfFile(dir_orig);
	  CloseHandle(fdicc_h);
#else
	  free(dir_orig);
#endif
  }
#else
  if ( (dir_orig != 0) && (tam != 0))
  {
    if (munmap(dir_orig, tam) == -1)
    std::cout << "Error al desmapear " << errno << std::endl;
  }
#endif
  /* ------------------------------ */
}

// Metodo : N_sdicc
// Descripcion : numero de subdiccionarios en los que se ha 
//       clasificado el diccionario.
// Parametros : Ninguno.
Diccionario::Entero Diccionario::N_sdicc(void) const
{
  // El numero de subdiccionarios esta en el offset = 0.
  return *((Entero *)Resuelve(0));
}

// Metodo : Inicio
// Descripcion : obtenemos el offset de la primera palabra del 
//      subdiccionario indicado.
// Parametros : numero del subdiccionario.
//      Va de o a N_sdicc-1.
Diccionario::Offset Diccionario::Inicio(Entero sdicc) const
{
  // El valor buscado esta en el offset = SIZE_ENT + sdicc*SIZE_ENT
  // El valor es un Entero.
	char *pchar = Resuelve(SIZE_ENT + sdicc*SIZE_ENT);
	Offset *poff = (Offset *)pchar;

  return *((Offset *)Resuelve(SIZE_ENT + sdicc*SIZE_ENT));
}

// Metodo : Dame
// Descripcion : obtine la entrada del diccionario que se encuentra en 
//       la posicion dada por el offset.
// Parametros : el offset.
Diccionario::Entrada Diccionario::Dame(const Offset &off) const
{
  unsigned int l = strlen(Resuelve(off))+1;
  // Ajustar la longitud de la cadena a multiplos de SIZE_ENT.
  if ((l%SIZE_ENT) != 0)
    l += SIZE_ENT - (l%SIZE_ENT);

  return Entrada(Resuelve(off), *((float *)Resuelve(off + l)), *((Offset *)Resuelve(off + l + SIZE_ENT)));
}

// Metodo : Resuelve
// Descripcion : Obtiene la direccion de memoria actual, 
//          asociada al offset indicado.
// Parametros : un offset.
char *Diccionario::Resuelve(const Offset &off) const
{
  return (char *)(((char *)dir_dicc)+off);
}

// Metodo : Convierte
// Descripcion : convierte un fichero de formato ASCII a BINARIO.
// Parametros : Nombre del fichero en formato ASCII,
//         Nombre del fichero en formato binario,
//         Numero de subdiccionarios en los que se quiere clasificar.
//            (opcional, valor por defecto 4001)
//            Esta ajustado para unas 100000 entradas en el diccionario.
void Diccionario::Convierte(const char *nascii, const char *nbinario,
        Entero nsdicc)
{
  // Apertura de los ficheros.
	std::ifstream fascii(nascii);
#ifdef WIN32
	std::ofstream fbinario(nbinario,ios::out | ios::binary, filebuf::openprot);
#else
	std::ofstream fbinario(nbinario);
#endif

  // Comprobacion de los ficheros.
  if (!fascii)
  {
	  std::cout << "Error apertura de fichero " << nascii << std::endl;
    return;
  }

  if (!fbinario)
  {
	  std::cout << "Error apertura de fichero " << nbinario << std::endl;
    return;
  }

  // Procedemos a convertir.
  Convierte(fascii, fbinario, nsdicc);
}

// Metodo : Convierte
// Descripcion : Convierte del formato ascii al binario tomando 
//       las entradas de streams.
// Parametros : Un stream de entrada con el formato ASCII.
//       Un stream de salida con el formato BINARIO.
//       El numero de subdiccionarios en los que se va a organizar 
//         el diccionario.
//         Es opcional con un valor por defecto de 4001.
//         Ajustado para diccionarios de unas 100000 entradas.
void Diccionario::Convierte(std::ifstream &fascii, std::ofstream &fbinario, Entero nsdicc)
{
  Hash hash(nsdicc);
  char cad[250], cad_filtrada[250];
  unsigned int i;
  float fre;

#ifdef WIN32
#ifdef FICH_MAPEADO
  fdicc_h = fascii;
#endif
#endif
  // Preparar datos intermedios.
  Lista<Entrada_ASCII> *dicc = new Lista<Entrada_ASCII>[hash.Tamanno()];

  Offset *tams = new Offset[hash.Tamanno()];
  for (i = 0; i < hash.Tamanno(); i++)
    tams[i] = 0;

  double total_frecs = 0;

  // Ajuste de cadenas.
  unsigned int l;
  static const char cdummy[SIZE_ENT] = {'\0', '\0', '\0', '\0'};

  // Procedemos a convertir.
  // Primero leemos.
  fascii >> fre >> cad;
  while (!fascii.eof())
  {
    Entrada_ASCII ent_filtrada;

    total_frecs += (double) fre;
    Filtra(cad, cad_filtrada);
    ent_filtrada.cad = cad_filtrada;
    ent_filtrada.fre = fre;
    if (!dicc[hash(cad_filtrada)].Busca(ent_filtrada))
    {
      dicc[hash(cad_filtrada)] += ent_filtrada;
//      dicc[hash(cad_filtrada)] += Entrada_ASCII(cad_filtrada, fre);

      // Tamanno de la cadena ajustado a multiplos de SIZE_ENT.
      l = strlen(cad_filtrada)+1;
      if ((l%SIZE_ENT) != 0)
        l += SIZE_ENT - (l%SIZE_ENT);
      tams[hash(cad_filtrada)] += l+2*SIZE_ENT; // tamanno de esta Entrada.
    }

    fascii >> fre >> cad;
  }

  // Ahora escribimos.
  // Primero el numero de subdiccionarios.
  Entero aux = hash.Tamanno();

  fbinario.write((char *)&aux, SIZE_ENT);

  // Ahora todos los offset iniciales.
  for (i = 0; i < hash.Tamanno(); i++)
  {
	  std::cout << "hash [" << i << "] = " << dicc[i].Tamanno() << std::endl;
    if (dicc[i].Tamanno() == 0)
    {
      aux = OFFSET_NULO;
      fbinario.write((char *)&aux, SIZE_ENT);
    }
    else
    {
      // Offset inicial del subdiccionario.
      aux = SIZE_ENT + SIZE_ENT*hash.Tamanno();
      for (unsigned int j = 0; j < i; j++)
        aux += tams[j];

      fbinario.write((char *)&aux, SIZE_ENT);
    }
  }

  // Ahora todos los subdiccionarios.
  aux = SIZE_ENT + SIZE_ENT*hash.Tamanno();
  for (i = 0; i < hash.Tamanno(); i++)
  {
    for (unsigned int j = 0; j < dicc[i].Tamanno(); j++)
    {
      // Escribir la cadena.
      l = strlen(dicc[i][j].cad())+1;
      fbinario.write(dicc[i][j].cad(), l);

      // Ajustar el tamanno de la cadena a multiplos de SIZE_ENT.
      if ((l%SIZE_ENT) != 0)
      {
        fbinario.write(cdummy, SIZE_ENT - (l%SIZE_ENT));
        l += SIZE_ENT - (l%SIZE_ENT);
      }

      // Escribir la frecuencia. (Con descuento y limite inferior).
      fre  = (float)(dicc[i][j].fre/total_frecs);
      if (fre < MIN_FREC_DICCIO)
        fre = (float)MIN_FREC_DICCIO;
      fbinario.write( (char *)&fre, SIZE_ENT);

      // Escribir el offset de la siguiente entrada.
      aux += l+2*SIZE_ENT;
      if (j == dicc[i].Tamanno()-1)
      {  // El ultimo debe cerrar la lista.
        Offset off = OFFSET_NULO;
        fbinario.write((char *)&off, SIZE_ENT);
      }
      else
      {
        fbinario.write((char *)&aux, SIZE_ENT);
      }
    }
  }

  // Deletes.
  delete [] dicc;
  delete [] tams;
}

// Metodo : Busca
// Descripcion : Busca la palabra en el diccionario.
// Parametros : Palabra a buscar.
double Diccionario::Busca(const char *pal) const
{
  // La filtrada.
  static char palf[250];
  Filtra(pal, palf);
  Offset off = Inicio(Hash(N_sdicc())(palf));
  while (off != OFFSET_NULO)
  {
    if (strcmp(palf, Dame(off).Palabra()) == 0)
      return ((double) Dame(off).Frec()); // EUREKA.
    off = Dame(off).Sgte();
  }
 
  // Mala suerte.
  return (MIN_FREC_DICCIO);
}

// Metodo : Filtra
// Descripcion : Filtra la cadena de entrada, sustituyendo los caracteres
//      problematicos por un comodin.
// Parametros : puntero a la cadena original,
//      puntero al espacio para la cadena filtrada.
void Diccionario::Filtra(const char *cad, char *cadf) const
{
  static const unsigned char COMODIN = '#';
  unsigned int i;

  // Proteccion.
  if (cad == 0)
  {
	  std::cout << "ERROR llamada a Filtra con puntero nulo." << std::endl;
    return;
  }

  for (i = 0; i < strlen(cad); i++)
  {
    if ( ( (TRATA_CAR::Letra(cad[i])) && 
           (TRATA_CAR::Ascii(cad[i])) ) || 
         (TRATA_CAR::Digito(cad[i])) )
      cadf[i] = TRATA_CAR::AMayusculas(cad[i]);
    else
      cadf[i] = COMODIN;
  }
  cadf[i] = '\0';
}

// Metodo : Test
// Descripcion : Indica el estado del diccionario.
//        Si la respuesta es falso no deberian invocarse otros metodos
//        de Diccionario a excepcion de :
//          - ~Diccionario.
//          - Convierte.
// Parametros : Ninguno.
int Diccionario::Test(void) const
{
  return (dir_dicc != 0);
}

/////////////////////////////////////
// Metodos de la clase Diccionario::Entrada
/////////////////////////////////////

// Metodo : Entrada
// Descripcion : Constructor.
// Parametros : Puntero a la cadena.
//        Offset de la siguiente entrada.
Diccionario::Entrada::Entrada(const char *pal, const float &fre, const Offset &off)
: palabra(pal), frec(fre), sgte(off)
{
  sgte = off;
}

// Metodo : ~Entrada
// Descripcion : Destructor.
// Parametros : Ninguno.
Diccionario::Entrada::~Entrada(void)
{
}

// Metodo : Palabra
// Descripcion : Acceso al campo palabra.
// Parametros : Ninguno.
const char *Diccionario::Entrada::Palabra(void)
{
  return palabra;
}

// Metodo : Frec
// Descripcion : Acceso a la frecuencia de la palabra.
// Parametros : Ninguno.
float Diccionario::Entrada::Frec(void)
{
  return frec;
}

// Metodo : Sgte
// Descripcion : Acceso al campo sgte.
// Parametros : Ninguno.
Diccionario::Offset Diccionario::Entrada::Sgte(void)
{
  return sgte;
}

