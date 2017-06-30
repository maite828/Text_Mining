/*******************************************

PROGRAMA: entrena_Bigrama mod_bigr_ascii fich_texto

TAREA: Estima o reestima un modelo de idioma basándose en
	un fichero de texto en ese idioma.

	El funcionamiento es como sigue: 

	- Intenta cargar el modelo de bigramas ascii, si no lo encuentra
	  crea un modelo de idioma inicial nulo, en caso contrario
	  reestima el modelo ya existente.

	- Abre el fichero de texto y lee todas y cada una de las
	  palabras. En función de las secuencias de caracteres 
	  presentes en la palabra va reestimando el modelo de bigramas.

	- Cuando llega al final del fichero de texto graba el modelo
	  de bigramas reestimado, también en ascii.

FUNCIONES LLAMADAS
	cargar_mod_idioma
	mod_idioma_nulo
	sig_token
	pasa_a_minuscula
	codigo_letra
	graba_mod_idioma

HISTORICO DE MODIFICACIONES:

	23/3/98 (DTT) Creación del programa 

********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <iostream>

#include "libs/det_id/lista.h"

#include "libs/det_id/Bigrama.h"
#include "libs/det_id/Pares_caracteres.h"
#include "libs/det_id/Pares_secuencias.h"
#include "libs/det_id/Det_id_interno.h"
#include "libs/det_id/detector_id_Privado.h"
#include "libs/det_id/detector_id.h"

main(int argc, char *argv[])
{
  long mod_idioma[NUMCOD_BIG*NUMCOD_BIG];
  char pal[MAXLONGPAL_DET_ID+1];
  char *ptr_texto, *ptr_final;
  int fd;
  long tam;
  char *texto_in;
  char mod_detid[1000];
  unsigned char ch,chant;
  unsigned char *ptrch;

  // Flag que indica si estamos dentro de una URL o no
  int URL = 0;


  // Goyo. Punteros a las cadenas de texto con marcas
  char *texto_marc_url;
  char *texto_marc_email;


  // Objeto detector de idioma
  Det_id_interno det;

  if(argc!=3)
  {
    printf("USO: entrena_mod_idioma mod_idioma fich_texto\n");
    exit(-1);
  }

  /*** Carga el modelo existente o genera uno nulo ***/
  if((Bigrama::carga_mod_idioma(argv[1],mod_idioma))==-1)
  {
    printf("Estimando por primera vez el modelo ""%s""...\n",argv[1]);
    Bigrama::reset_mod_idioma(mod_idioma);
  }
  else
  {
    printf("Reestimando el modelo preexistente ""%s""...\n",argv[1]);
  }

  // Mapea el fichero de texto en memoria
  if((fd = open(argv[2],O_RDONLY))==-1)
  {
    printf("ERROR: No se puede abrir el fichero '%s'.\n",argv[2]);
    exit(-1);
  }
  tam = lseek(fd, 0, SEEK_END);
  texto_in = mmap(0, tam, PROT_READ, MAP_SHARED, fd, 0);
  if(texto_in == MAP_FAILED)
  {
    printf("ERROR: Error al hacer el mapeo del fichero '%s'.\n",argv[2]);
    exit(-1);
  }
  close(fd);

  // Carga el modelo de deteccion con el diccionario nulo
  if(getenv("DETECTOR_ID")!=NULL)
    strcpy(mod_detid,getenv("DETECTOR_ID"));
  else
  {
    printf("ERROR: Variable de entorno DETECTOR_ID no definida.\n");
    exit(-1);
  }
  strcat(mod_detid,"/Solaris/modid_bin.estable/mod_cas");

  if(det.Carga(Det_id_interno::CASTELLANO,mod_detid)!=Det_id_interno::OK)
  {
    printf("ERROR: No se ha podido cargar el modelo de detección '%s'.\n",
           mod_detid);
    exit(-1);
  }

  /*** Lee las palabras y actualiza el modelo ***/

  // Goyo. Intentamos marcar y eliminar las direcciones de e-mail y URLS
 
  texto_marc_url = det.marc.MarcaURLs(texto_in);
  texto_marc_email = det.marc.MarcaMails(texto_marc_url);
 
  delete [] texto_marc_url;


  ptr_texto = texto_marc_email;
  tam = strlen(texto_marc_email);
  ptr_final = texto_marc_email + tam;
  while((ptr_texto=det.Sgte_pal_obtenpal(ptr_texto,ptr_final,pal))!=NULL) 
  {
    //printf("%s\n",pal);
    if(*pal=='\\')
    {
      // TRATAMIENTO DE LAS MARCAS DE TEXTO
      if(!strcmp(pal,"\\url_ini\\"))
      {
        //Entramos en modo URL - No corregirá nada hasta encontrar una url_fin
       URL = 1;
      }
      else if(!strcmp(pal,"\\url_fin\\"))
      {
        //Salimos del modo URL
        URL = 0;
      }
    }
    // Comprobamos si estamos en una zona que no hay que corregir debido
    // al marcado - De momento sólo URLs.
    else if(!URL)
    {

    chant = ' ';
    chant = Bigrama::codifica_letra(chant);
      ptrch = (unsigned char *) pal;
      while((ch=*ptrch)!='\0')
      {
        ch = Bigrama::codifica_letra(ch);
	/* Para no acumular en el caso de dos separadores seguidos */
	if (ch||chant)
          mod_idioma[(unsigned int)(chant)*NUMCOD_BIG+(unsigned int)(ch)]++;
        chant = ch;
        ptrch++;
      }
      ch = ' ';
      ch = Bigrama::codifica_letra(ch);
      if (ch||chant)
        mod_idioma[(unsigned int)(chant)*NUMCOD_BIG+(unsigned int)(ch)]++;
    }
  }

  delete [] texto_marc_email;

  /*** Escribe el modelo reestimado ***/
  if(Bigrama::graba_mod_idioma(mod_idioma,argv[1])==-1)
  {
    printf("ERROR(%s): Error al escribir el nuevo modelo de idioma ""%s""\n",
            argv[0],argv[1]);
    exit(-1);
  }

  // Desmapeamos todo el texto
  if (munmap(texto_in,tam) == -1)
  {
    printf("ERROR: Se ha producido un error al desmapear..\n");
    exit(-1);
  }

  det.Descarga(Det_id_interno::CASTELLANO);
}

