// PROGRAMA: gen_Mod_idioma
//
// DESCRIPCION: Genera el modelo de idioma binario a utilizar por el detector
//              de idioma a partir del diccionario en ASCII para dicho idioma
//              y del modelo de idioma en ASCII generado por entrena_Bigrama.
//              El diccionario en ASCII es simplemente una lista de palabras
//              pertenecientes al idioma (en principio usamos unas 100000 por
//              idioma).
//              El programa produce como salida un fichero binario con una 
//              cabecera, la versi�n binaria del diccionario, ya convertido en
//              tabla hash, y la versi�n binaria del modelo de bigramas, ya
//              transformado a logaritmos de probabilidades para que los
//              c�lculos sean m�s r�pidos.
//
// USO: gen_Mod_idioma idi dicc.ascii [N_ent_tabla_hash] mod_bigr.ascii mod_idi
// donde:
//       idi es uno de los idiomas usados: castellano catal�n gallego euskera
//                                         ingl�s franc�s alem�n brasileiro
//       dicc.ascii es una lista con las palabras del idioma, una en cada l�nea
//       N_ent_tabla_hash es un par�metro opcional que permite especificar el
//           n�mero de entradas de la tabla hash usada para contener el
//           diccionario.
//       mod_bigr.ascii es un modelo de bigramas en ascii. Este fichero es 
//           generado por el programa entrena_Bigrama
//       mod_idi es el modelo de idioma binario a generar.
//
// 16 Diciembre 1999 - Doroteo Torre Toledano

#include <stdio.h>
#include <string.h>
#include <fstream>
#include <stdlib.h>

#include "libs/det_id/Bigrama.h"
#include "libs/det_id/dicc.h"
#include "libs/det_id/Det_id_interno.h"

// Funci�n para transformar de cadena ascii a idioma
Det_id_interno::Idioma_detid ascii2Idioma(char *idioma)
{
  if(!strcmp(idioma, "cas"))
    return Det_id_interno::CASTELLANO;
  else if(!strcmp(idioma, "cat"))
    return Det_id_interno::CATALAN;
  else if(!strcmp(idioma, "gal"))
    return Det_id_interno::GALLEGO;
  else if(!strcmp(idioma, "eus"))
    return Det_id_interno::EUSKERA;
  else if(!strcmp(idioma, "ing"))
    return Det_id_interno::INGLES;
  else if(!strcmp(idioma, "fra"))
    return Det_id_interno::FRANCES;
  else if(!strcmp(idioma, "ale"))
    return Det_id_interno::ALEMAN;
  else if(!strcmp(idioma, "bra"))
    return Det_id_interno::BRASILENO;
  else if(!strcmp(idioma, "ita"))
    return Det_id_interno::ITALIANO;
  else
    return Det_id_interno::IDIOMA_NULO;
}

// Funci�n para mostrar el uso del programa
void USO()
{
  printf("USO: gen_Mod_idioma idi dicc.ascii mod_bigr.ascii mod_idi\n");
  printf(" donde:\n");
  printf("       idi es uno de los c�digos de idioma: cas cat gal eus ing fra ale bra\n");
  printf("       dicc.ascii es una lista con las palabras del idioma, una en cada l�nea\n");
  printf("       mod_bigr.ascii es un modelo de bigramas en ascii. Este fichero es \n");
  printf("           generado por el programa entrena_Bigrama.\n");
  printf("       mod_idi es el modelo de idioma binario a generar.\n");
}


// Main
int main(int argc, char *argv[])
{
  char dic_ascii[1000];
  char mod_bigr_ascii[1000];
  char mod_idi[1000];
  Det_id_interno::Idioma_detid idi;
  long N_ent = -1;
  int narg=1;

  // Obtiene los argumentos
  if((argc!=6)&&(argc!=5))
  {
    USO();
    exit(-1);
  }

  idi = ascii2Idioma(argv[narg]);
  if(idi==Det_id_interno::IDIOMA_NULO)
  {
    printf(">>>IDIOMA MAL ESPECIFICADO<<<\n\n");
    USO();
    exit(-1);
  }
 
  narg++;
  strcpy(dic_ascii,argv[narg]);

  narg++;

/* -- Esto todav�a no est� soportado --
  if(argc==6)
  {
    if((sscanf(argv[narg],"%ld",&N_ent)!=1)||(N_ent<1))
    {
      printf(">>>NUMERO DE ENTRADAS MAL ESPECIFICADO<<<\n\n");
      USO();
      exit(-1);
    }
    narg++;
  }
*/

  if(argc==6)
  {
    printf(">>>N_ent_hash TODAV�A NO EST� SOPORTADO: Om�telo, por favor<<<\n");
    USO();
    exit(-1);
  }

  strcpy(mod_bigr_ascii,argv[narg]);
  narg++;
  strcpy(mod_idi,argv[narg]);

  // Llama a la funci�n de la clase Det_id_interno que hace la 
  // conversi�n de formatos.
  Det_id_interno().Gen_mod_bin(idi,dic_ascii,mod_bigr_ascii,mod_idi);

} 
