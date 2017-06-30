/////////////////////////////////////////////////////////////////////////
//
// Fichero: Transformaciones_may_min.cc
// Descripci�n: Contiene las funciones Det_id_interno::Transforma_a_min 
//              y Det_id_interno::Restaura_may que se encargan en el 
//              corrector ortogr�fico multiling�e, respectivamente, de 
//              transformar una palabra obtenida del texto con la funci�n 
//              Sgte_pal en una palabra en min�sculas (dando tambi�n 
//              informaci�n de los cambios que se han efectuado, o lo 
//              que es lo mismo, de las letras que se deben pasar despues
//              a may�sculas), y de restaurar las may�sculas que sean
//              necesarias despu�s de la correcci�n ortogr�fica (utilizando
//              para ello la informaci�n dada por Transforma_a_min).
//
// 2/3/2000 DTT
// 3/3/2000 DTT Adem�s incluyo la funci�n Patron_maymin que permite obtener,
//              a partir del la cadena cambios_maymin que devuelve la funci�n
//              Transforma_a_min, una cadena de dos caracteres con el patr�n
//              de may�sculas/min�sculas as�:
//                mm - Toda la palabra en min�sculas
//                Mm - En min�sculas salvo la primera letra
//                mM - En may�sculas salvo la primera letra
//                MM - Toda la palabra en may�sculas
//                mX - Empieza en min�scula y el resto es una mezcla.
//                MX - Empieza en may�scula y el resto es una mezcla.
//              Estas cadenas se buscar�n en la tabla de pares de secuencias
//              parsec_maymin, y s�lo se corregir� la palabra si la secuencia
//              asociada al patr�n devuelto por esta funci�n es "CORR".
//
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>

#include "Det_id_interno.h"
#include "trata_car.h"

// Transforma una palabra a min�scula e indica los cambios efectuados.
void Det_id_interno::Transforma_a_min(char *cad_orig_may, 
                                       char *cad_orig_min, 
                                       char *cambios_may_min)
{

  char *ptr_may = cad_orig_may;
  char *ptr_min = cad_orig_min;
  char *ptr_cambios = cambios_may_min;

  while (*ptr_may!='\0')
  {
    if((*ptr_min=TRATA_CAR::AMinusculas(*ptr_may))!=*ptr_may)
      *ptr_cambios='1';
    else
      *ptr_cambios='0';
    ptr_may++;
    ptr_min++;
    ptr_cambios++;
  }
  *ptr_min='\0';
  *ptr_cambios='\0';
}

// Restaura las may�sculas en la palabra corregida, teniendo en cuenta
// como era la palabra original una vez pasada a min�sculas, los cambios
// de may�scula a min�scula que se realizaron para obtener dicha palabra
// original en min�sculas, y c�mo es la palabra corregida en min�sculas.
void Det_id_interno::Restaura_may(char *cad_orig_min, 
                                   char *cambios_may_min,
                                   char *cad_corr_min,
                                   char *cad_corr_may)
{
  char cambios_corr[2*MAXLONGPAL_DET_ID];

  // Alineamos las palabras original y corregida en min�sculas para ver los
  // cambios que se han producido
  Alinear(cad_orig_min,cad_corr_min,cambios_corr);

  // Ahora recorremos la palabra corregida en min�sculas viendo qu� hacer con
  // cada uno de sus caracteres, si es necesario pasarlo a may�sculas o no

  char *p_cambios_corr = cambios_corr;
  char *p_cambios_may_min = cambios_may_min;
  char *p_corr_min = cad_corr_min;
  char *p_corr_may = cad_corr_may;

  while(*p_cambios_corr!='\0')
  {
    if(*p_cambios_corr=='B')
    {
      // El car�cter de entrada se elimin�. 
      // No existe el car�cter correspondiente en la salida.
      p_cambios_corr++;
      p_cambios_may_min++;
    }
    else if(*p_cambios_corr=='I')
    {
      // El car�cter es una inserci�n, tenemos que decidir si lo 
      // escribimos con may�scula o min�scula. Ir� en may�scula s�lo 
      // cuando la inserci�n se produzca entre dos may�sculas 
      if(((p_cambios_may_min-1<cambios_may_min)||
          (*(p_cambios_may_min-1)=='1'))&&
         ((*p_cambios_may_min=='\0')||
          (*(p_cambios_may_min)=='1')))
        *p_corr_may = TRATA_CAR::AMayusculas(*p_corr_min);
      else
        *p_corr_may = *p_corr_min;
	
      p_cambios_corr++;
      p_corr_may++;
      p_corr_min++;
    }
    else
    {
      /* El car�cter es una sustituci�n o un acierto. Se pone may�scula */
      /* o min�scula seg�n estuviese en la entrada. */
      if(*p_cambios_may_min=='1')
        *p_corr_may = TRATA_CAR::AMayusculas(*p_corr_min);
      else
        *p_corr_may = *p_corr_min;
      p_cambios_corr++;
      p_cambios_may_min++;
      p_corr_may++;
      p_corr_min++;
    }
  }
  *p_corr_may = '\0';
}

// Obtiene el patr�n de may�sculas y min�sculas
char *Det_id_interno::Patron_maymin(char *cambios_maymin)
{
  const char *ptr=cambios_maymin;
  // Debe ser static para que no se borre al salir de la funci�n.
  static char patron[3]="--";
  char p1;

  if(*ptr=='1')
    patron[0]='M';
  else
    patron[0]='m';
 
  ptr++; 
  if(*ptr=='1')
    p1='M';
  else
    p1='m';

  ptr++;
  while((*ptr!='\0')&&(p1!='X'))
  {
    if(((p1=='M')&&(*ptr=='0'))||((p1=='m')&&(*ptr=='1')))
      p1='X';
    ptr++;
  }
  patron[1]=p1;
  return patron;
}
