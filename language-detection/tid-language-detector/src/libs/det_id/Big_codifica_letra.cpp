/*******************************************

METODO: static unsigned char Bigrama::codifica_letra(unsigned char ch)

TAREA: Codifica el car�cter ch a un valor lineal de forma que
	ese valor se utilice luego para acceder a un array que
        empiece por 0 (el valor asignado al espacio) y que sea
	continuo para no desaprovechar mucho espacio.

	Devuelve el valor codificado. El valor 0 se utiliza para
	codificar todos aquellos caracteres que no representan 
	letras v�lidas en una palabra (seg�n el modelo de bigramas).

HIST�RICO DE MODIFICACIONES:

	23/3/98 (DTT) Creaci�n de la funci�n.
        4/12/99 (DTT) Versi�n para DETECTOR_ID V1.0
        28/2/00 (DTT) Versi�n para DETECTOR_ID V2.0 (incluye � del alem�n)

********************************************/
#include <stdio.h>
#include <string.h>

#include "Bigrama.h"

unsigned char Bigrama::codifica_letra(unsigned char ch)
{ 
  static unsigned char PARES_BIG[]= "  AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz����������������������������������������������������������������";
  unsigned char *ptr;

  if((ptr= (unsigned char *) strchr((const char *)PARES_BIG,ch))!=NULL)
    return (unsigned char) ((unsigned int) (ptr-PARES_BIG) / 2);
  else
    return 0;
}
  
  
