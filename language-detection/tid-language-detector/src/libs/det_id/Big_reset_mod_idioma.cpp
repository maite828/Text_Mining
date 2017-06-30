/************************************

FUNCION: static void Bigrama::reset_mod_idioma(long *mod_idioma)

TAREA: 	Pone a 0 todos los contadores de apariciones del modelo.

FUNCIONES LLAMADAS:

HISTORICO MODIFICACIONES:
	23/3/98 (DTT) Creación de la función

**************************************/

#include <stdio.h>

#include "Bigrama.h"

void Bigrama::reset_mod_idioma(long *mod_idioma)
{

  long *ptr;
  int i;

  ptr = mod_idioma;
  for(i=0;i<NUMCOD_BIG*NUMCOD_BIG;i++,ptr++)
  {
    *ptr = 0;
  }

}

