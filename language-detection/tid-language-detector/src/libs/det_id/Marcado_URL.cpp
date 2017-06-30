// METODOS DE LA CLASE: Marcado 
// METODO: MarcaURLs
// UTILIDAD: Permite introducir marcas en el texto que identifiquen 
//           las URLs y las distingan del resto del texto normal. 
//           Esto en algun momento permitira al conversor leerlas de
//           forma mucho mas precisa. Por el momento serviran para que
//           ni el Detector Textual de Idioma, ni el Corrector Ortografico
//           Automatico Multilingue metan la pata con las URLs
//
// MARCAS INTRODUCIDAS: 
//           Justo antes del inicio de la URL: "\url_ini\"
//           Justo despues del final de la URL: "\url_fin\"
//

#include <math.h>
#ifdef WIN32
#include <strstrea>
#else
#include <strstream>
#endif
#include <string.h>

#include "Marcado.h"

char *Marcado::MarcaURLs(const char *entrada)
{
	std::ostrstream salida;

  char *pos_write = (char *)entrada;
  char *inicio_URL, *final_URL;
	
  // Procesar la entrada.
  while(((inicio_URL=strstr(pos_write,"http://"))!=NULL)||
	((inicio_URL=strstr(pos_write,"ftp://"))!=NULL)||
	((inicio_URL=strstr(pos_write,"gopher://"))!=NULL)||
	((inicio_URL=strstr(pos_write,"mailto://"))!=NULL)||
	((inicio_URL=strstr(pos_write,"news://"))!=NULL)||
	((inicio_URL=strstr(pos_write,"nntp://"))!=NULL)||
	((inicio_URL=strstr(pos_write,"telnet://"))!=NULL)||
	((inicio_URL=strstr(pos_write,"wais://"))!=NULL)||
	((inicio_URL=strstr(pos_write,"file://"))!=NULL)||
	((inicio_URL=strstr(pos_write,"www"))!=NULL))
  {
    //Buscamos el final de la URL, es decir, el primer 
    //espacio en blanco que encontremos
    final_URL=inicio_URL;
    while((*final_URL!=' ')&&(*final_URL!='\n')&&(*final_URL!='\0'))
    {
	final_URL++;
    }
	  //Escribimos desde la pos_write hasta el incio de la URL
	  salida.write(pos_write, inicio_URL-pos_write);

	  //Escribimos la marca de comienzo de URL
	  salida << "\\url_ini\\";

	  //Escribimos la URL
	  salida.write(inicio_URL, final_URL-inicio_URL);

	  //Escribimos la marca de final de URL
	  salida << "\\url_fin\\";

	  pos_write = final_URL;
  }

  // Escribimos la parte del buffer que resta por escribir
  salida << pos_write;

  if(salida.pcount()==0)
    salida << ' ';

  // Goyo. Parece que a veces no termina bien los mensajes. Según José,
  // para terminar los strstreams hay que añadir ends.

  salida << std::ends;
  
  return salida.str();

}
char *Marcado::MarcaMails(const char *entrada)
{
	std::ostrstream salida;

  char *pos_write = (char *)entrada;
  char *inicio_URL, *final_URL;
  char *pos_inicio_total = (char *)entrada;
  
  while((inicio_URL=strstr(pos_write,"@"))!=NULL)
  {
    	final_URL=inicio_URL;

    	while((*inicio_URL!=' ')&&(*inicio_URL!='\n')&&(inicio_URL!=pos_inicio_total))
    	{
		inicio_URL--;
    	}
    	while((*final_URL!=' ')&&(*final_URL!='\n')&&(*final_URL!='\0'))
    	{
        	final_URL++;
    	}
          //Escribimos desde la pos_write hasta el incio de la URL
	  if (pos_write != inicio_URL)
	  {
	    inicio_URL++;
	    salida.write(pos_write, inicio_URL-pos_write);
	  }

          //Escribimos la marca de comienzo de URL
          salida << "\\url_ini\\";

          //Escribimos la URL
          salida.write(inicio_URL, final_URL-inicio_URL);

          //Escribimos la marca de final de URL
          salida << "\\url_fin\\";

          pos_write = final_URL;
  }
  salida << pos_write;

  if(salida.pcount()==0)
    salida << ' ';

  // Goyo. Parece que a veces no termina bien los mensajes. Según José,
  // para terminar los strstreams hay que añadir ends.

  salida << std::ends;
  return salida.str();
}

char *Marcado::EliminaMarcado(const char *entrada)
{
	std::ostrstream salida;

  char *pos_write = (char *)entrada;
  char *inicio_eti, *final_eti;
  while((inicio_eti=strstr(pos_write,"\\url_ini\\"))!=NULL)
  {
	final_eti=inicio_eti+9;
        //Escribimos desde la pos_write hasta el incio de la etiqueta 
        salida.write(pos_write, inicio_eti-pos_write);
	pos_write=final_eti;
	if((inicio_eti=strstr(pos_write,"\\url_fin\\"))!=NULL)
	{
		final_eti=inicio_eti+9;
        	//Escribimos desde la pos_write hasta el incio de la etiqueta
        	salida.write(pos_write, inicio_eti-pos_write);
        	pos_write=final_eti;
	}
  }
  salida << pos_write;

  if(salida.pcount()==0)
         salida << ' ';

  // Goyo. Parece que a veces no termina bien los mensajes. Según José,
  // para terminar los strstreams hay que añadir ends.

  salida << std::ends;
  return salida.str();
}
