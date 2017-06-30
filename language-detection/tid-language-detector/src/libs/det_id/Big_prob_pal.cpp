/********************************************

METODO: double prob_pal(const char *pal)

TAREA: 	Esta función calcula la prob de que una palabra pertenezca al
	idioma que representa el objeto de clase Bigrama en cuestión.
	Para ello se basa en el modelo de bigramas asociado al objeto, y
	que está apuntado por la variable privada dir_big.
	En concreto la probabilidad se calcula como el valor medio de 
	las probabilidades de aparición en el idioma de cada uno de los
	bigramas que forman la palabra.

FUNCIONES LLAMADAS:
	Bigrama::codifica_letra

HISTORICO DE MODIFICACIONES:
	
	17/1/2000 (DTT) Creación de la función.

*********************************************/

#include <stdio.h>

#include "Bigrama.h"

double Bigrama::prob_pal(const char *pal)
{
  unsigned char ch, chant;
  unsigned char *ptrch = (unsigned char *)pal;
  double logprob;
  float *logprob_mod_idioma = (float *)dir_big;
  unsigned int nbig=0;

  chant = ' ';
  chant = codifica_letra(chant);
  logprob = 0.0;
  while((ch=*ptrch)!='\0')
  {
    ch = codifica_letra(ch);
    /* Elimina los casos de varios separadores juntos */
    if (ch||chant)
    {
      logprob+=logprob_mod_idioma[(unsigned int)(chant)*NUMCOD_BIG+
    				  (unsigned int)(ch)];
      nbig++;
    }
    chant = ch;
    ptrch++;
  }
  ch = ' ';
  ch = codifica_letra(ch);
  /* Elimina el caso de varios separadores juntos */
  if (ch||chant)
  {
    logprob+=logprob_mod_idioma[(unsigned int)(chant)*NUMCOD_BIG+
                                (unsigned int)(ch)];
    nbig++;
  }
  return pow(10, logprob/(double) nbig);
}
