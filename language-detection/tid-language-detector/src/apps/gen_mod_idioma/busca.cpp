// Fichero : busca.cc
// Descripcion : Herramienta para comprobar la busqueda.
//        Toma un diccionario en formato binario y un fichero con palabras.
//        Para cada palabra indica si la encuentra o no en el diccionario.
// Fecha : 26/11/99
// Version : 1.0

#include <fstream>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#include "libs/det_id/dicc.h"

void Uso(void)
{
  std::cout << "busca <diccionrio> <lista de palabras>" << std::endl;
  exit(0);
}

int main(int narg, char **args)
{
  if (narg < 3)
    Uso();

  int f;
  f = open(args[1], O_RDONLY);
  Diccionario dicc(f);
  ifstream lista(args[2]);
  char cad[200];

  // Comprobacion.
  if (!lista)
  {
    std::cout << "ERROR al abrir el fichero " << args[2] << std::endl;
    exit(1);
  }

  if (!dicc.Test())
  {
    std::cout << "ERROR al abrir el diccionario " << args[1] << std::endl;
    exit(1);
  }

  lista >> cad;
  while (!lista.eof())
  {
    if (!dicc.Busca(cad))
      std::cout << cad << std::endl;
//    std::cout << cad << ((dicc.Busca(cad))?" SI":" NO") << " está." << std::endl;
    lista >> cad;
  }

  close(f);
}

