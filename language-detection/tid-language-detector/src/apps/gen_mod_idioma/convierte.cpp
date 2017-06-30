// Fichero : convierte.cc
// Descripcion : Herramienta de conversion de diccionarios.
//      Los convierte del formato ASCII al formato BINARIO.
// Fecha : 26/11/99
// Version : 1.0

#include <iostream>
#include <stdlib.h>
#include "libs/det_id/dicc.h"

void Uso(void)
{
  cout << "convierte <fich. ASCII> <fich. BINARIO> <nsubdicc>" << endl;
  exit(0);
}

int main(int narg, char **args)
{
  if (narg < 4)
    Uso();

  Diccionario().Convierte(args[1], args[2], atol(args[3]));
}

