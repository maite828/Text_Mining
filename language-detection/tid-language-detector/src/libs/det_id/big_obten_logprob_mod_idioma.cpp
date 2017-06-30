/*******************************

METODO: static void Bigrama::obten_logprob_mod_idioma(long *mod_idioma, 
						     float *logprob_mod_idioma)

TAREA: Transforma el modelo de idioma, que pasa de expresar 
	el número de ocurrencias de cada par de caracteres en
        el idioma a expresar la log_prob del par de caracteres
        en el idioma. Para el cálculo de la log_prob se toma
	un umbral inferior de -100, valor que se asigna a los
        casos que no han tenido ninguna ocurrencia.
	Hemos modificado el umbral inferior. En lugar de tomar
	un valor fijo de -100 asignamos a los bigramas que han
	tenido 0 ocurrencias la logprob correspondiente a 0.5
	ocurrencias. De este modo no damos tanta importancia a
	si un bigrama aparece una o ninguna vez en el texto de
	entrenamiento. Además, de esta forma la normalización 
	de la matriz de logprobs sigue siendo correcta porque
	en la suma total de ocurrencias sumamos 0.5 por cada
	bigrama que no ha tenido ninguna ocurrencia.


FUNCIONES LLAMADAS:

HISTÓRICO DE MODIFICACIONES:
	23/3/98 (DTT) Creación de la función

************************************************/
#include <stdio.h>
#include <math.h>

#include "Bigrama.h"

void Bigrama::obten_logprob_mod_idioma(long *mod_idioma,
					      float *logprob_mod_idioma)
{

  //const double LOGPROBUNIF = log10(1.0/(NUMCOD_BIG*NUMCOD_BIG));
  double suma_ocur;
  long *ptrlong;
//  float *retval; 
  float *ptrfloat;
  int i;

  suma_ocur = 0.0;
  ptrlong = mod_idioma;
  for(i=0;i<NUMCOD_BIG*NUMCOD_BIG;i++,ptrlong++)
  {
    if(*ptrlong==0)
      suma_ocur += 0.5;
    else
      suma_ocur += (double) *ptrlong;
  }
  
  ptrfloat = logprob_mod_idioma;
  ptrlong = mod_idioma;
  for(i=0;i<NUMCOD_BIG*NUMCOD_BIG;i++,ptrfloat++,ptrlong++)
  {
    if(*ptrlong<=0)
      *ptrfloat = (float) (log10(0.5/suma_ocur));
    else
      *ptrfloat = (float) (log10((double)*ptrlong/(double)suma_ocur));
   
/*
    if(*ptrfloat< (float) log10(0.5/suma_ocur))
      *ptrfloat = (float) log10(0.5/suma_ocur);
*/
  }
}
