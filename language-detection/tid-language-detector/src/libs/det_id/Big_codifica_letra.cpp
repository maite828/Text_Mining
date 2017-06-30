/*******************************************

METODO: static unsigned char Bigrama::codifica_letra(unsigned char ch)

TAREA: Codifica el carácter ch a un valor lineal de forma que
	ese valor se utilice luego para acceder a un array que
        empiece por 0 (el valor asignado al espacio) y que sea
	continuo para no desaprovechar mucho espacio.

	Devuelve el valor codificado. El valor 0 se utiliza para
	codificar todos aquellos caracteres que no representan 
	letras válidas en una palabra (según el modelo de bigramas).

HISTÓRICO DE MODIFICACIONES:

	23/3/98 (DTT) Creación de la función.
        4/12/99 (DTT) Versión para DETECTOR_ID V1.0
        28/2/00 (DTT) Versión para DETECTOR_ID V2.0 (incluye ß del alemán)

********************************************/
#include <stdio.h>
#include <string.h>

#include "Bigrama.h"

unsigned char Bigrama::codifica_letra(unsigned char ch)
{ 
  static unsigned char PARES_BIG[]= "  AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZzÀàÁáÂâÃãÄäÅåÆæÇçÈèÉéÊêËëÌìÍíÎîÏïĞğÑñÒòÓóÔôÕõÖöØøÙùÚúÛûÜüİıŞşÿÿßß";
  unsigned char *ptr;

  if((ptr= (unsigned char *) strchr((const char *)PARES_BIG,ch))!=NULL)
    return (unsigned char) ((unsigned int) (ptr-PARES_BIG) / 2);
  else
    return 0;
}
  
  
