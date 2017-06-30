// PROGRAMA: test_Detector_id
// DESCRIPCION: Programa de Test y Ejemplo sobre el uso del Detector de Idioma

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <strstream>

#include <sys/types.h>
#include <sys/mman.h>


#include "libs/det_id/detector_id.h"

Detector_id::Idioma cad2idi(char *cad)
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

char *idi2cad(Detector_id::Idioma idioma)
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

int main(int argc, char *argv[])
{
  Detector_id det;
  long tam;
  int ftexto;
  char *ptexto;
  Detector_id::Idioma idioma;
  int conf;
  int i;

  // Comprueba los argumentos
  if(argc < 4) 
  {
    cout << "USO: test_Detector_id fich_texto {idioma mod_idioma}+" << endl;
    cout << " Idiomas : CASTELLANO, CATALAN, GALLEGO, EUSKERA, INGLES, FRANCES, ALEMAN, BRASILEÑO, ITALIANO" << endl;
    exit(-1);
  }

  if((ftexto = open(argv[1], O_RDONLY))==-1)
  {
    cout << "No puedo abrir el fichero " << argv[1] << endl;
    exit(-1);
  }
  tam = lseek(ftexto, 0, SEEK_END);
  ptexto = mmap(0, tam, PROT_READ, MAP_SHARED, ftexto, 0);
  if(ptexto == MAP_FAILED)
  {
    cout << "Error al mapear el fichero de texto" << endl;
    exit(-1);
  }
  close(ftexto);

  // Carga de los modelos de idioma.
  for (i = 2; i < argc; i += 2)
  {
    // Como tomamos los argumentos de 2 en 2, comprobamos que hay 2.
    if (i+1 >= argc)
    {
      cout << "Ultimo idioma no correcto." << endl;
      continue;
    }

    if(det.Carga(cad2idi(argv[i]), argv[i+1]) != Detector_id::OK)
    {
      cout << "Error al cargar el modelo de idioma " << argv[i] << endl;
      exit(-1);
    }
  }

  // Llamamos a la función del detector de idioma que detecta el idioma.
  idioma = det.Detecta(ptexto, &conf);

  // Escribimos el idioma detectado
  cout << ">>> Detectado " << idi2cad(idioma) << " con Confianza " << (int) conf << endl;

  // Desmapeamos el fichero de texto
  if(munmap(ptexto,tam) == -1)
  {
    cout << "Error al demapear" << endl;
    exit(-1);
  }

  // NOTA: En general sería buena cosa descargar los modelos de idioma
  // con las siguientes líneas, pero ne es necesario ya que el destructor
  // del Detector_id ya descarga todos los modelos que hubiese cargados.
  for (i = 2; i < argc; i += 2)
  {
    if (i+1 >= argc)
      continue;

    if (det.Descarga(cad2idi(argv[i]))!=Detector_id::OK)
    {
      cout << "Error al descargar el modelo de idioma " << argv[i] << endl;
    }
  }
}

