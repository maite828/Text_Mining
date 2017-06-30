/*******************************************

PROGRAMA: obten_palabras cod_idi fich_texto

TAREA: Obtiene la lista de palabras que aparecen en un fichero tal y como las
       segmentar�a el corrector ortogr�fico. La principal utilidad de esto es
       facilitar la generaci�n del diccionario del corrector ortogr�fico.

       Es importante darse cuenta de que la segmentaci�n puede ser dependiente
       del idioma, por lo que la segmentaci�n la tiene que hacer un objeto 
       Det_id_interno que cargue el modelo de correcci�n del idioma en el que 
       estemos interesado. En realidad este programa se utiliza para generar
       el diccionario, que es parte del modelo de correcci�n. Sin embargo el
       diccionario no interviene en la segmentaci�n ni en determinar si una
       palabra contiene s�lo caracteres del idioma o no. Por ello en lugar
       de cargar el modelo de correcci�n normal 

          $CORRECTOR_ORTOG/Solaris/modid_bin.estable/mod_$cod_id

       carga un modelo de correcci�n similar PERO con un diccionario vac�o:

          $CORRECTOR_ORTOG/Solaris/modid_bin.estable/mod_$cod_id_dicnull

       Con este modelo de correcci�n va a hacer la segmentaci�n del fichero 
       en palabras y s�lo va a escribir a su salida las palabras que le hayan
       devuelto que adem�s:

          - Tengan s�lo caracteres v�lidos en el idioma.
          - Sean de longitud >= MINLONGPAL_CORR.
          - Su patr�n de may�sculas/min�sculas sea de los que se van a 
            corregir.

       La lista de palabras la escribe en stdout. Por supuesto la escribe 
       pas�ndolo todo a min�sculas.

       Otra utilidad de este programa es analizar y depurar el comportamiento
       de la segmentaci�n.

FUNCIONES LLAMADAS
	Sgte_pal

HISTORICO DE MODIFICACIONES:

	3/03/2000 (DTT) Creaci�n del programa 

********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>


#include <iostream>

#include <lista.h>

#include "libs/det_id/Det_id_interno.h"
#include "libs/det_id/trata_car.h"

Det_id_interno::Idioma cod_idi2Idioma(char *cod_idi)
{
  if(!strcmp(cod_idi,"cas"))
    return Det_id_interno::CASTELLANO;
  else if(!strcmp(cod_idi,"cat"))
    return Det_id_interno::CATALAN;
  else if(!strcmp(cod_idi,"gal"))
    return Det_id_interno::GALLEGO;
  else if(!strcmp(cod_idi,"eus"))
    return Det_id_interno::EUSKERA;
  else if(!strcmp(cod_idi,"ing"))
    return Det_id_interno::INGLES;
  else if(!strcmp(cod_idi,"fra"))
    return Det_id_interno::FRANCES;
  else if(!strcmp(cod_idi,"ale"))
    return Det_id_interno::ALEMAN;
  else if(!strcmp(cod_idi,"bra"))
    return Det_id_interno::BRASILENO;
  else if(!strcmp(cod_idi,"ita"))
    return Det_id_interno::ITALIANO;
  else 
    return Det_id_interno::IDIOMA_NULO;
}

main(int argc, char *argv[])
{
  char pal[MAXLONGPAL_DET_ID+1];
  char *ptr_texto, *ptr_final;
  int fd;
  long tam;
  char *texto_in;
  char mod_corr[1000];
  Det_id_interno::Idioma id;

  // Flag que indica si estamos dentro de una URL o no
  int URL = 0;


  // Objeto detector de idioma
  Det_id_interno det;


  // Goyo. Punteros a las cadenas de texto con marcas
  char *texto_marc_url;
  char *texto_marc_email;


  // Objeto corrector ortogr�fico
  Det_id_interno idioma;

  if(argc!=3)
  {
    printf("USO: obten_palabras cod_idi fich_texto\n");
    exit(-1);
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

  id = cod_idi2Idioma("cas");
  if(id==Det_id_interno::IDIOMA_NULO)
  {
    printf("ERROR: Idioma '%s' no v�lido.\n",argv[1]);
    exit(-1);
  }

  // Carga el modelo de correcci�n con el diccionario nulo
  if(getenv("DETECTOR_ID")!=NULL)
    strcpy(mod_corr,getenv("DETECTOR_ID"));
  else
  {
    printf("ERROR: Variable de entorno DETECTOR_ID no definida.\n");
    exit(-1);
  }
  strcat(mod_corr,"/Solaris/modid_bin.estable/mod_");
  strcat(mod_corr,"cas");
 
 
  if(idioma.Carga(id,mod_corr)!=Det_id_interno::OK)
  {
    printf("ERROR: No se ha podido cargar el modelo de idioma '%s'.\n",
           mod_corr);
    exit(-1);
  }

  // Goyo. Intentamos marcar y eliminar las direcciones de e-mail y URLS

  texto_marc_url = det.marc.MarcaURLs(texto_in);
  texto_marc_email = det.marc.MarcaMails(texto_marc_url);

  delete [] texto_marc_url;


  
  ptr_texto = texto_marc_email;
  tam = strlen(texto_marc_email);
  ptr_final = texto_marc_email + tam;
  while((ptr_texto=idioma.Sgte_pal_obtenpal(ptr_texto,ptr_final,pal))!=NULL)
  {
    // Sgte_pal_obtenpal ya devuelve la siguiente palabra v�lida de acuerdo
    // a las condiciones que se indican al principio del fichero, y en 
    // min�sculas
    // Comprobamos si se trata de una marca de texto
    if(*pal=='\\')
    {
      // TRATAMIENTO DE LAS MARCAS DE TEXTO
      if(!strcmp(pal,"\\url_ini\\"))
      {
        //Entramos en modo URL - No corregir� nada hasta encontrar una url_fin
       URL = 1;
      }
      else if(!strcmp(pal,"\\url_fin\\"))
      {
        //Salimos del modo URL
        URL = 0;
      }
    }
    // Comprobamos si estamos en una zona que no hay que corregir debido
    // al marcado - De momento s�lo URLs.
    else if(!URL)
    {
      printf("%s\n",pal);
    }
  }

  delete [] texto_marc_email;

}

