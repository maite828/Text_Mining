/************************************

METODO: static int Bigrama::carga_mod_idioma(char *filename, long *mod_big)

TAREA: 	Carga un modelo de idioma en ascii del fichero especificado.
	Devuelve -1 si no se ha podido leer correctamente el 
        modelo de idioma. 
	El modelo de idioma debe ser un modelo de idioma grabado 
        por la función graba_mod_idioma.

FUNCIONES LLAMADAS:

HISTORICO MODIFICACIONES:
	23/3/98 (DTT) Creación de la función

**************************************/

#include <stdio.h>
#include <string.h>

#include "Bigrama.h"

int Bigrama::carga_mod_idioma(char *filename, long *mod_big)
{
  FILE *fin;
  long *ptr;
  float version;
  int i;

  if((fin=fopen(filename,"r"))==NULL)
    return -1;

  if((fscanf(fin,"<MODELO DE IDIOMA V%f>\n",&version)!=1)||
     (version!=VERSION_BIG))
    return -1;

  ptr = mod_big;
  for(i=0;i<NUMCOD_BIG*NUMCOD_BIG;i++,ptr++)
  {
    if(fscanf(fin," %ld ",ptr)!=1)
      return -1;
  }

  fclose(fin);
  
  return 0;
}
    
/************************************

METODO: static int Bigrama::carga_mod_idioma(std::ifstream &fbig, long *mod_big)

TAREA: 	Carga un modelo de idioma en ascii del fichero especificado.
	Devuelve -1 si no se ha podido leer correctamente el 
        modelo de idioma. 
	El modelo de idioma debe ser un modelo de idioma grabado 
        por la función graba_mod_idioma.

FUNCIONES LLAMADAS:

HISTORICO MODIFICACIONES:
	23/3/98 (DTT) Creación de la función

**************************************/

#include <fstream>

#include "Bigrama.h"

int Bigrama::carga_mod_idioma(std::ifstream &fbig, long *mod_big)
{
//  FILE *fin;
  long *ptr;
  float version = 0.0;
  int i;
  char cadena[50];


  fbig >> cadena;
  if(strcmp(cadena,"<MODELO"))
    return -1;
  fbig >> cadena;
  if(strcmp(cadena,"DE"))
    return -1;
  fbig >> cadena;
  if(strcmp(cadena,"IDIOMA"))
    return -1;
  fbig >> cadena;
  if(sscanf(cadena,"V%f%s", &version, cadena)!=2)
    return -1;
  if(version!=VERSION_BIG)
    return -1;
  if(strcmp(cadena,">"))
    return -1;
  if(!fbig.good())
    return -1;

  ptr = mod_big;
  for(i=0;i<NUMCOD_BIG*NUMCOD_BIG;i++,ptr++)
  {
    fbig >> *ptr;
    if(!fbig.good())
      return -1;
  }

  return 0;
}

