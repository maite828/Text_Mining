/////////////////////////////////////////////////////////////////////////
//
// Fichero: Transformaciones_may_min.cc
// Descripción: Contiene las funciones Det_id_interno::Transforma_a_min 
//              y Det_id_interno::Restaura_may que se encargan en el 
//              corrector ortográfico multilingüe, respectivamente, de 
//              transformar una palabra obtenida del texto con la función 
//              Sgte_pal en una palabra en minúsculas (dando también 
//              información de los cambios que se han efectuado, o lo 
//              que es lo mismo, de las letras que se deben pasar despues
//              a mayúsculas), y de restaurar las mayúsculas que sean
//              necesarias después de la corrección ortográfica (utilizando
//              para ello la información dada por Transforma_a_min).
//
// 2/3/2000 DTT
// 3/3/2000 DTT Además incluyo la función Patron_maymin que permite obtener,
//              a partir del la cadena cambios_maymin que devuelve la función
//              Transforma_a_min, una cadena de dos caracteres con el patrón
//              de mayúsculas/minúsculas así:
//                mm - Toda la palabra en minúsculas
//                Mm - En minúsculas salvo la primera letra
//                mM - En mayúsculas salvo la primera letra
//                MM - Toda la palabra en mayúsculas
//                mX - Empieza en minúscula y el resto es una mezcla.
//                MX - Empieza en mayúscula y el resto es una mezcla.
//              Estas cadenas se buscarán en la tabla de pares de secuencias
//              parsec_maymin, y sólo se corregirá la palabra si la secuencia
//              asociada al patrón devuelto por esta función es "CORR".
//
/////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>

#include "Det_id_interno.h"
#include "trata_car.h"

// Transforma una palabra a minúscula e indica los cambios efectuados.
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

// Restaura las mayúsculas en la palabra corregida, teniendo en cuenta
// como era la palabra original una vez pasada a minúsculas, los cambios
// de mayúscula a minúscula que se realizaron para obtener dicha palabra
// original en minúsculas, y cómo es la palabra corregida en minúsculas.
void Det_id_interno::Restaura_may(char *cad_orig_min, 
                                   char *cambios_may_min,
                                   char *cad_corr_min,
                                   char *cad_corr_may)
{
  char cambios_corr[2*MAXLONGPAL_DET_ID];

  // Alineamos las palabras original y corregida en minúsculas para ver los
  // cambios que se han producido
  Alinear(cad_orig_min,cad_corr_min,cambios_corr);

  // Ahora recorremos la palabra corregida en minúsculas viendo qué hacer con
  // cada uno de sus caracteres, si es necesario pasarlo a mayúsculas o no

  char *p_cambios_corr = cambios_corr;
  char *p_cambios_may_min = cambios_may_min;
  char *p_corr_min = cad_corr_min;
  char *p_corr_may = cad_corr_may;

  while(*p_cambios_corr!='\0')
  {
    if(*p_cambios_corr=='B')
    {
      // El carácter de entrada se eliminó. 
      // No existe el carácter correspondiente en la salida.
      p_cambios_corr++;
      p_cambios_may_min++;
    }
    else if(*p_cambios_corr=='I')
    {
      // El carácter es una inserción, tenemos que decidir si lo 
      // escribimos con mayúscula o minúscula. Irá en mayúscula sólo 
      // cuando la inserción se produzca entre dos mayúsculas 
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
      /* El carácter es una sustitución o un acierto. Se pone mayúscula */
      /* o minúscula según estuviese en la entrada. */
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

// Obtiene el patrón de mayúsculas y minúsculas
char *Det_id_interno::Patron_maymin(char *cambios_maymin)
{
  const char *ptr=cambios_maymin;
  // Debe ser static para que no se borre al salir de la función.
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
