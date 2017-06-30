/********************************************

METODO: double logprob_pal(const char *pal)

TAREA: 	Esta función calcula la logprob de que una palabra pertenezca al
	idioma que representa el objeto de clase Bigrama en cuestión.
	Para ello se basa en el modelo de lenguaje asociado al objeto, y
	que está apuntado por la variable privada dir_big.

FUNCIONES LLAMADAS:
	Bigrama::codifica_letra

HISTORICO DE MODIFICACIONES:
	
	23/3/98 (DTT) Creación de la función.

*********************************************/

#include <stdio.h>

#include "Bigrama.h"

double Bigrama::logprob_pal(const char *pal)
{
  unsigned char ch, chant;
  unsigned char *ptrch = (unsigned char *)pal;
  double logprob;
  float *logprob_mod_idioma = (float *)dir_big;

  chant = ' ';
  chant = codifica_letra(chant);
  logprob = 0.0;
  while((ch=*ptrch)!='\0')
  {
    ch = codifica_letra(ch);
    /* Elimina los casos de varios separadores juntos */
    if (ch||chant)
      logprob+=logprob_mod_idioma[(unsigned int)(chant)*NUMCOD_BIG+
    				  (unsigned int)(ch)];
    chant = ch;
    ptrch++;
  }
  ch = ' ';
  ch = codifica_letra(ch);
  /* Elimina el caso de varios separadores juntos */
  if (ch||chant)
    logprob+=logprob_mod_idioma[(unsigned int)(chant)*NUMCOD_BIG+
                                (unsigned int)(ch)];
  return logprob;
}
