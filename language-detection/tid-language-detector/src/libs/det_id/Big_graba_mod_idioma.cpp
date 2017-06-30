/************************************

METODO: static int Bigrama::graba_mod_idioma(long *mod_idioma, char *filename)

TAREA:	Graba el modelo de idioma ASCII que se encuentra apuntado por 
	el puntero mod_idioma a un fichero ASCII con nombre filename

	El modelo de idioma grabado se puede cargar con la 
	función carga_mod_idioma.

	Devuelve 0 si todo ha ido bien y -1 en caso contrario.

FUNCIONES LLAMADAS:

HISTORICO MODIFICACIONES:
	23/3/98 (DTT) Creación de la función

**************************************/

#include <stdio.h>

#include "Bigrama.h"

int Bigrama::graba_mod_idioma(long *mod_idioma, char *filename)
{

  FILE *fout;
  long *ptr;
  int i;

  if((fout=fopen(filename,"w"))==NULL)
    return -1;

  if(fprintf(fout,"<MODELO DE IDIOMA V%f>\n",VERSION_BIG)<0)
    return -1;

  ptr = mod_idioma;
  for(i=0;i<NUMCOD_BIG*NUMCOD_BIG;i++,ptr++)
  {
    if(fprintf(fout," %ld ",*ptr)<0)
      return -1;
  }
 
  fclose(fout); 

  return 0;
}


  

	
