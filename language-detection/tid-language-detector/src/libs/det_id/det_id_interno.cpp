// Fichero : Det_id_interno.cc
// Descripcion : Implementacion de la clade Detector_id
// Version : 1.0
// Fecha : 3/12/99

#ifdef TRAZAS_CC
#include <fstream>
#endif
#ifdef TRAZAS_C
#include <stdio.h>
#endif

#ifndef WIN32
#include <unistd.h>
#include <strstream>
#else
#include <strstrea> 
#include <windows.h>
#include <io.h>
#include <direct.h>
#endif

#ifndef s25o22mt
#include <iostream>
#else
#include <iostream.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <math.h>

#include "Marcado.h"
#include "Pares_caracteres.h"
#include "Pares_secuencias.h"
#include "Bigrama.h"
#include "dicc.h"
#include "Det_id_interno.h"
#include "trata_car.h"

//Imágenes en memoria de tablas hard-coded
#include "img_parcar_validos.h"
#include "img_parcar_segmpal.h"
#include "img_parsec_segmpal.h"
#include "img_parsec_markup.h"
#include "img_parsec_maymin.h"


// Metodo : Det_id_interno
// Descripcion : Constructor por defecto.
// Parametros : Ninguno.
Det_id_interno::Det_id_interno(void)
{
}


// Metodo : ~Det_id_interno
// Descripcion : Destructor por defecto.
// Parametros : Ninguno.
Det_id_interno::~Det_id_interno(void)
{
  // Al destruirse el objeto Det_id_interno (debería haber sólo uno)
  // además de borrarse la lista linfo_id es necesario liberara la
  // memoria de los objetos Diccionario y Bigrama creados dinámicamente.
  // Para ello vamos a recorrer la lista linfo_id y descargando los
  // idiomas uno a uno.
  // No es muy eficiente, pero sólo debería hacerse cuando se va a 
  // dejar de usar el detector, por lo que no es muy crítico.

  Lista<Info_id>::Posicion p;

  for (p=0; p<linfo_id.Tamanno(); p++)
    Descarga(linfo_id[p].idioma);

}
 
// Metodo : Carga
// Descripcion : Carga un idioma.
// Parametros : Idioma y fichero con el modelo de lenguaje asociado al idioma.
// Valor devuelto: Código de error de los definidos en la clase Det_id_interno.
Det_id_interno::Error Det_id_interno::Carga(Idioma_detid idi, const char *fich_mod_idioma)
{
//#ifdef WIN32
//  HANDLE fmod_idi;	
//#else
#ifndef WIN32
  int fmod_idi;
#endif

  Info_id info_id;

  // Mapea el modelo de idioma a la dirección dir_mod_idi y cierra el fichero.
#ifdef WIN32


  char buffer[_MAX_PATH];

   /* Get the current working directory: */
   if( _getcwd( buffer, _MAX_PATH ) == NULL )
      ;
   else
      ;
#ifdef FICH_MAPEADO
  if ((info_id.fmod_idi = OpenFileMapping(FILE_MAP_READ, FALSE, fich_mod_idioma)) == NULL)
#else
  if ((info_id.fmod_idi = _open(fich_mod_idioma, O_RDONLY | O_BINARY)) == -1)
#endif
#else
	  fprintf(stderr, "Trying to open mod:'%s'\n", fich_mod_idioma);
  if((fmod_idi = open(fich_mod_idioma, O_RDONLY))==-1)
#endif
  {
#ifdef WIN32
	  DWORD ret = GetLastError();
#else
	  perror(fich_mod_idioma);
#endif
    return ERROR_FICHERO;
  }


#ifdef WIN32
#ifdef FICH_MAPEADO
  info_id.inicio_tabla = MapViewOfFile(info_id.fmod_idi, FILE_MAP_READ, 0, 0, 0);
    if (info_id.inicio_tabla == NULL)
  {
	  CloseHandle(info_id.fmod_idi);
	  info_id.fmod_idi = NULL;
	  return ERROR_MAPEO;
  }
#else
  struct stat fbloque;
  int n_leidos = 0;

  //Cargámos el fichero, leyéndolo en memoria
  if (stat(fich_mod_idioma, &fbloque) == -1)
  {
	  return ERROR_MAPEO;
  }

  //Comprobamos que no está vacío
  if (fbloque.st_size == 0)
  {
	  return ERROR_MAPEO;
  }

  if ((info_id.inicio_tabla = (char *)malloc(fbloque.st_size + 1)) == NULL)
  {
	  return ERROR_MAPEO;
  }

  n_leidos = read(info_id.fmod_idi, info_id.inicio_tabla, fbloque.st_size);
#endif

#else
#ifdef FICH_MAPEADO
  info_id.tam = lseek(fmod_idi, 0, SEEK_END);
  
  info_id.inicio_tabla = (char *)mmap(0, info_id.tam, PROT_READ, 
			      MAP_SHARED, fmod_idi, 0);
  if (info_id.inicio_tabla == MAP_FAILED)
    return ERROR_MAPEO;
#else

  struct stat fbloque;
  int n_leidos = 0;

  //Cargámos el fichero, leyéndolo en memoria
  if (stat(fich_mod_idioma, &fbloque) == -1)
  {
	  return ERROR_MAPEO;
  }

  //Comprobamos que no está vacío
  if (fbloque.st_size == 0)
  {
	  return ERROR_MAPEO;
  }

  if ((info_id.inicio_tabla = (char *)malloc(fbloque.st_size + 1)) == NULL)
  {
	  return ERROR_MAPEO;
  }

  n_leidos = read(info_id.fmod_idi, info_id.inicio_tabla, fbloque.st_size);

#endif

  close(fmod_idi);
#endif



  // Comprueba la cabecera del fichero
  unsigned int *ptr = (unsigned int *) info_id.inicio_tabla;
  if (*ptr != Tabla_id)
  {
	  fprintf(stderr, "Mod no valido:%p != %p\n", *ptr, Tabla_id);
    return MODELO_IDIOMA_NO_VALIDO;
  }
  ptr++;
  if (*ptr != Version)
    return VERSION_INCORRECTA;
  ptr++;
  // Vamos a dejar que sean compatibles las tablas con mismo
  // número de versión, aunque tengan distinto número de 
  // subversión.
  //if (*ptr != Subversion)
  //  return VERSION_INCORRECTA;
  ptr++;
  if ((Idioma_detid) *ptr != idi)
    return IDIOMA_INCORRECTO;
  ptr++;

  // Pone el idioma en info_id
  info_id.idioma = idi;

  // Comprobamos que no esté ya cargado este idioma.
  Lista<Info_id>::Posicion p;
  for (p=0; p<linfo_id.Tamanno(); p++)
    if(linfo_id[p].idioma == idi)
      return IDIOMA_YA_CARGADO;

  // Las versiones 1.X del detector manejan dos tablas, la 
  // de diccionario y la de bigramas.
  if (*ptr != 2)
    return MODELO_IDIOMA_NO_VALIDO;
  ptr++;

  // Iniciamos en Info_id las tablas hard-coded

  // Tabla de caracteres válidos en palabra del detector (img_parcar_validos.h)
  info_id.parcar_validos = new Pares_caracteres(img_parcar_validos);
  
  // Tabla de caracteres válidos en palabra del detector (img_parcar_segmpal.h)
  info_id.parcar_segmpal = new Pares_caracteres(img_parcar_segmpal);
  
  // Tabla de cadenas válidas en palabra del detector (img_parsec_segmpal.h)
  info_id.parsec_segmpal = new Pares_secuencias(img_parsec_segmpal);

  // Tabla de marcas de texto posibles (fichero img_parsec_markup.h)
  info_id.parsec_markup = new Pares_secuencias(img_parsec_markup);

  // Tabla de tipos de combinaciones may/min (fichero img_parsec_maymin.h)
  info_id.parsec_maymin = new Pares_secuencias(img_parsec_maymin);
  
  // Obtenemos la dirección de inicio del diccionario y 
  // creamos con ella un objeto Diccionario
 
#ifdef WIN32
  info_id.dic = new Diccionario(((char *)info_id.inicio_tabla + *ptr), info_id.inicio_tabla);
#else
  info_id.dic = new Diccionario(info_id.inicio_tabla + *ptr, info_id.inicio_tabla);
#endif
  ptr++;

  // Obtenemos la dirección de inicio del modelo de bigramas
  // y creamos con ella un objeto Bigrama
#ifdef WIN32
  //info_id.big = new Bigrama( (LPVOID) (info_id.inicio_tabla + *ptr) );
  info_id.big = new Bigrama( (unsigned long *) ((char *)info_id.inicio_tabla + *ptr) );
#else
  info_id.big = new Bigrama( (void *) (info_id.inicio_tabla + *ptr) ); 
#endif

  // Obtenemos la cadena con los caracteres considerados válidos en el
  // idioma (en su representación interna)
  // NOTA: Esta cadena contiene SOLO caracteres en minúscula!!!
  // Se usa para probar sustituciones e inserciones, así que no tiene sentido
  // probar con mayúsculas y minúsculas porque el diccionario sólo contiene
  // minúsculas. En el detector de idioma no se usa. Pero la tenemos de todos
  // modos
  unsigned int i;
  for (i=0;i<256;i++)
    info_id.letras_validas[i]=0;
  for (i=0;i<256;i++)
  {
    if((unsigned char) info_id.parcar_validos->Asociado((unsigned char) i) >
0)
    {
      // printf("Ponemos %u a 1\n",(unsigned char)info_id.parcar_validos->Asociado((char) i));
      info_id.letras_validas[(unsigned char) TRATA_CAR::AMinusculas((unsigned char)info_id.parcar_validos->Asociado((char) i))]=1;
    }
  }
  int j=0;
  for (i=0;i<256;i++)
  {
    if(info_id.letras_validas[i])
    {
      info_id.letras_validas[j]=(unsigned char) i;
      j++;
    }
  }
  info_id.letras_validas[j]='\0';

  // Metemos el objeto info_id así creado en la lista que 
  // contiene las informaciones de los idiomas manejados
  linfo_id += info_id;

  // Todo ha ido bien.
  return OK;
}

// Metodo : Descarga
// Descripcion : Descarga un idioma.
// Parametros : Idioma
// Valor devuelto: Código de error de los definidos en la clase Det_id_interno
Det_id_interno::Error Det_id_interno::Descarga(Idioma_detid idi)
{
  // Buscamos su posición
  Lista<Info_id>::Posicion p;
  for (p=0; ((p<linfo_id.Tamanno())&&(linfo_id[p].idioma!=idi)); p++)
  if(p==linfo_id.Tamanno())
    return IDIOMA_NO_CARGADO;
 
  // Eliminamos el objeto parcar_segmpal
  delete linfo_id[p].parcar_segmpal;
 
  // Eliminamos el objeto parcar_validos
  delete linfo_id[p].parcar_validos;
 
  // Eliminamos el objeto parsec_segmpal
  delete linfo_id[p].parsec_segmpal;
 
  // Eliminamos el objeto parsec_markup
  delete linfo_id[p].parsec_markup;

  // Eliminamos el objeto parsec_maymin
  delete linfo_id[p].parsec_maymin;

  // Eliminamos el objeto Diccionario
  delete linfo_id[p].dic;

  // Eliminamos el objeto Bigrama
  delete linfo_id[p].big;

  // Desmapeamos todo el modelo de idioma
#ifdef WIN32
#ifdef FICH_MAPEADO
  UnmapViewOfFile(linfo_id[p].inicio_tabla);
  CloseHandle(linfo_id[p].fmod_idi);
#else
  if (linfo_id[p].inicio_tabla != NULL)
  {
	free(linfo_id[p].inicio_tabla);
	close(linfo_id[p].fmod_idi);
	linfo_id[p].inicio_tabla = NULL;
  }
#endif
#else
  if (munmap(linfo_id[p].inicio_tabla,linfo_id[p].tam) == -1)
    return ERROR_MAPEO;
#endif

  // Eliminamos este info_id de la lista
  linfo_id -= p;

  // Todo ha ido bien
  return OK;
}


// Método : Gen_mod_bin
// Descripcion : Genera el modelo binario a usar en el detector de idioma.
//               El modelo binario incluye un diccionario, un modelo de
//               probabilidades de bigramas, así como una cabecera con 
//               número de versión e idioma.
Det_id_interno::Error Det_id_interno::Gen_mod_bin( Idioma_detid idi, 
			                     const char *dic_ascii,
				             const char *big_ascii,
				             const char *mod_id_bin)
{
  long offset_big;
  unsigned long aux;


  // abre los ficheros
#ifdef WIN32
  std::ofstream fbinario(mod_id_bin, ios::out | ios::binary, filebuf::openprot);
#else
  std::ofstream fbinario(mod_id_bin);
#endif
  std::ifstream fdic(dic_ascii);
  std::ifstream fbig(big_ascii);

  if ((!fbinario) || (!fdic) || (!fbig))
    return ERROR_FICHERO;
  
  // escribimos la cabecera
  aux = (unsigned long) Tabla_id;
  fbinario.write((char *) &aux, SIZE_ENT);
  aux = (unsigned long) Version;
  fbinario.write((char *) &aux, SIZE_ENT);
  aux = (unsigned long) Subversion;
  fbinario.write((char *) &aux, SIZE_ENT);
  aux = (unsigned long) idi;
  fbinario.write((char *) &aux, SIZE_ENT);
  aux = 2; // Tenemos dos tablas
  fbinario.write((char *) &aux, SIZE_ENT);
  aux = 0x1C; // El offset de la primera tabla es fijo;
  fbinario.write((char *) &aux, SIZE_ENT);
  aux = 0; // El offset de la segunda tabla es desconocido todavía. 
	   // De momento escribimos un 0 y luego lo rellenaremos.
  offset_big = fbinario.tellp(); // Nos apuntamos la posición.
  fbinario.write((char *) &aux, SIZE_ENT);

  // LLamamos al método convierte de la clase diccionario para que 
  // genere la versión binaria del diccionario.
  Diccionario dic(NULL, NULL);
  dic.Convierte(fdic, fbinario);

  // Tomamos el offset de inicio de la segunda tabla
  aux = (unsigned long) fbinario.tellp();

  // Llamamos al método convierte de la clase bigrama para que
  // genere la versión binaria del modelo de bigramas.
  Bigrama::Convierte(fbig, fbinario);

  // Pone el offset de la tabla de bigramas en su sitio
  fbinario.seekp(offset_big);
  fbinario.write((char *) &aux, SIZE_ENT);

  // Cerramos los ficheros
  //fbinario.close();
  //fdic.close();
  //fbig.close();
  return OK;
}


/* ----- Ahora se usan las funciones Sgte_pal del corrector -----

// Metodo : Sgte_pal
// Descripcion : Toma un puntero a carácter que le indica dónde empezar a 
//               buscar y devuelve la primera palabra encontrada, así como
//               otro puntero a carácter con la posición donde se debe 
//               empezar a buscar la siguiente palabra.

char * Det_id_interno::Sgte_pal(const char *inicio, char *pal)
{
  const char *ptr_texto = inicio;
  char *ptr_pal = pal;
  int nCar = 0;

  // Protección
  if (inicio == NULL)
  {
    *pal = '\0';
    return NULL;
  }

  // Buscamos inicio de palabra
  while ((*ptr_texto != '\0') && (!TRATA_CAR::Letra(*ptr_texto)))
    ptr_texto++;
  
  if (*ptr_texto == '\0')
  {
    *pal = '\0';
    return NULL;
  }

  // Lee la palabra completa

  *ptr_pal = TRATA_CAR::AMinusculas(*ptr_texto);
  ptr_pal++;
  ptr_texto++;
  nCar++;

  while((*ptr_texto != '\0') && (TRATA_CAR::Letra(*ptr_texto)) &&
	(nCar < MAXLONGPAL_DET_ID))
  {
    *ptr_pal = TRATA_CAR::AMinusculas(*ptr_texto);
    ptr_pal++;
    ptr_texto++;
    nCar++;
  }

  if (nCar >= MAXLONGPAL_DET_ID)
  {
    // Hemos encontrado una palabra mayor de MAXLONGPAL_DET_ID
    // La vamos a descartar completamente, lo que hemos leído hasta
    // ahora y lo que sigue hasta el siguiente separador.

    while ((*ptr_texto != '\0') && (TRATA_CAR::Letra(*ptr_texto)))
      ptr_texto++;
    
    // Hace una llamada recursiva para obtener la siguiente palabra
    return Det_id_interno::Sgte_pal(ptr_texto, pal);
  }

  // Hemos encontrado y tenemos una palabra normal
  *ptr_pal = '\0';
  return (char *) ptr_texto;
}

// Metodo : Sgte_pal
// Descripcion : Toma un stream del que leer y devuelve la primera palabra 
//               encontrada, así como 0 si ha encontrado una palabra y -1 si
//               no ha conseguido leer ninguna.

int Det_id_interno::Sgte_pal(std::ifstream &ftexto, char *pal)
{
  char ch;
  char *ptr_pal = pal;
  int nCar = 0;

  // Protección
  if ((!ftexto)||(!ftexto.good()))
  {
    *pal = '\0';
    return -1;
  }

  // Buscamos inicio de palabra
  do 
  {
    ftexto.get(ch);
  } while ( ftexto.good() && (!TRATA_CAR::Letra(ch)));
  
  if (!ftexto.good())
  {
    *pal = '\0';
    return -1;
  }

  // Lee la palabra completa

  *ptr_pal = TRATA_CAR::AMinusculas(ch);
  ptr_pal++;
  nCar++;
  ftexto.get(ch);

  while( ftexto.good() && (TRATA_CAR::Letra(ch)) &&
	(nCar < MAXLONGPAL_DET_ID))
  {
    *ptr_pal = TRATA_CAR::AMinusculas(ch);
    ptr_pal++;
    nCar++;
    ftexto.get(ch);
  }

  if (nCar >= MAXLONGPAL_DET_ID)
  {
    // Hemos encontrado una palabra mayor de MAXLONGPAL_DET_ID
    // La vamos a descartar completamente, lo que hemos leído hasta
    // ahora y lo que sigue hasta el siguiente separador.

    do
    {
      ftexto.get(ch);
    } while ( ftexto.good() && (TRATA_CAR::Letra(ch)));
    
    // Hace una llamada recursiva para obtener la siguiente palabra
    return Det_id_interno::Sgte_pal(ftexto, pal);
  }

  // Hemos encontrado y tenemos una palabra normal
  *ptr_pal = '\0';
  return 0;
}

  ----- Ahora se usan las funciones Sgte_pal del corrector ----- */

// Metodo : Detecta
// Descripcion : Detecta el idioma al que pertenece el texto recibido.
//         Detecta entre los idiomas cargados.
//         Indica una confianza en la respuesta.
Det_id_interno::Idioma_detid Det_id_interno::Detecta(const char *texto, Confianza &conf)
{
  // Variables para manejar los idiomas
  Lista<Info_id>::Posicion nIdi = linfo_id.Tamanno();
  Lista<Info_id>::Posicion i;

  // Goyo. Punteros a las cadenas de texto con marcas
  char *texto_marc_url;
  char *texto_marc_email;

#ifdef TRAZAS_CC
  //Nombre del fichero de trazas
  char fich_trazas[256];
 
  //Fichero de trazas
  std::ofstream f_trazas;
 
  // Flag de trazas
  int flag_trazas = 0;

  //Identificador de mensaje (su longitud)
  int id_mens;
#endif
#ifdef TRAZAS_C
  //Nombre del fichero de trazas
  char fich_trazas[256];
 
  //Fichero de trazas
  FILE *f_trazas;
 
  // Flag de trazas
  int flag_trazas = 0;

  //Identificador de mensaje (su longitud)
  int id_mens;
#endif
 

  // ostrstream que tiraremos. Sirve para poder llamar a Sgte_pal
  std::ostrstream sal_nula;

  // Flag que indica si estamos dentro de una URL o no
  int URL = 0;

  // Protección para un número de idiomas bajo
  if (nIdi<1)
    return IDIOMA_NULO;
  else if (nIdi<2)
  {
    conf = 0;
    return linfo_id[0].idioma;
  }

  // Número de palabras del texto
  unsigned long nPal = 0;

  // Acumuladores para el manejo de Diccionarios.
  double *apariciones = new double [nIdi];

  // Variables en las que obtener datos más agregados sobre diccionarios
  Lista<Info_id>::Posicion mejor_dic, segundo_dic;

  // Acumuladores para el manejo de Bigramas.
  double *punt_bigramas = new double [nIdi];

  // Variables para obtener datos más agregados sobre bigramas
  Lista<Info_id>::Posicion mejor_big, segundo_big;

  // Inicialización de acumuladores. 
  for (i = 0; i< nIdi; i++)
  {
    apariciones[i] = 0.0;
    punt_bigramas[i] = 0.0;
  }

#ifdef TRAZAS_CC
  //Preparamos un fichero de trazas
  sprintf(fich_trazas, "/tmp/Detid_%d.log", getpid());
  f_trazas.open(fich_trazas, std::ofstream::out | std::ofstream::app);
  if (f_trazas)
  {
    flag_trazas = 1;
  }
 
  if (flag_trazas)
  {
    id_mens = strlen(texto);
    f_trazas << "Orig_" << id_mens << ": " << texto << endl;
    f_trazas.close();
    flag_trazas = 0;
  }
#endif
#ifdef TRAZAS_C
  //Preparamos un fichero de trazas
  sprintf(fich_trazas, "/tmp/Detid_%d.log", getpid());
  f_trazas = fopen(fich_trazas, "a+");
  if (f_trazas)
  {
    flag_trazas = 1;
  }
 
  if (flag_trazas)
  {
    id_mens = strlen(texto);
    fprintf(f_trazas, "Orig_%d: %s\n", id_mens, texto);
    fclose(f_trazas);
    flag_trazas = 0;
  }
#endif


  // Goyo. Procesamos el texto para marcar las direcciones de e-mail y URLs
  texto_marc_url = marc.MarcaURLs(texto);
  texto_marc_email = marc.MarcaMails(texto_marc_url);

  delete [] texto_marc_url;


  // Recorremos las palabras del texto
  // const char *ptr_texto = texto;
  char *ptr_texto = texto_marc_email;
  char pal[MAXLONGPAL_DET_ID + 1];

  // Da igual el info_id que le pasemos porque la información de segmentación
  // es la misma en todos
  ptr_texto = Sgte_pal(ptr_texto, (char *)pal, linfo_id[0], sal_nula);
  while( (*pal!='\0') 
#ifdef LIMITA_PAL
         && (nPal < MAXNUMPAL_DET_ID)
#endif
       )
  {
    // Comprobamos si se trata de una marca de texto
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
 
      // En cualquier caso la marca de texto queda invariable
      sal_nula << pal;
 
      // y se pasa a la siguiente palabra
      ptr_texto = Sgte_pal(ptr_texto,(char *)pal,linfo_id[0],sal_nula);
    }
    // Comprobamos si estamos en una zona que no hay que corregir debido
    // al marcado - De momento sólo URLs.
    else if(URL)
    {
      sal_nula << pal;
      ptr_texto = Sgte_pal(ptr_texto,(char *)pal,linfo_id[0],sal_nula);
    }
    else
    {
      nPal++;

      // Recorremos los idiomas
      for (i=0; i<nIdi; i++)
      {
        // Actualiza las apariciones en el diccionario
        apariciones[i]+=log10(linfo_id[i].dic->Busca(pal));
  
        // Actualiza la puntuación con bigramas
        punt_bigramas[i]+=log10(linfo_id[i].big->prob_pal(pal));
  
        // DEBUG
        //cout << "Palabra: " << pal << " pDIC= " << linfo_id[i].dic->Busca(pal)
        //     << " pBIG= " << linfo_id[i].big->prob_pal(pal) << endl;
      }
      //cout << endl;
  
      // Obtiene la siguiente palabra
      ptr_texto = Sgte_pal(ptr_texto,(char *)pal,linfo_id[0],sal_nula);
    }
  }

  delete [] texto_marc_email;

  // Calcula los valores promedio
  for (i=0; i<nIdi; i++)
  {
    apariciones[i]=pow(10,apariciones[i]/(double)nPal); 
    punt_bigramas[i]=pow(10,punt_bigramas[i]/(double)nPal);
  }

  // Obtiene para cada idioma los valores elaborados

  if (apariciones[0] >= apariciones[1])
  {
    mejor_dic = 0;
    segundo_dic = 1;
  }
  else
  {
    mejor_dic = 1;
    segundo_dic = 0;
  }

  if (punt_bigramas[0] >= punt_bigramas[1])
  {
    mejor_big = 0;
    segundo_big = 1;
  }
  else
  {
    mejor_big = 1;
    segundo_big = 0;
  }
   
  for (i=2; i<nIdi; i++)
  {
    if(apariciones[i]>apariciones[mejor_dic])
    {
      segundo_dic = mejor_dic;
      mejor_dic = i;
    }
    else if (apariciones[i]>apariciones[segundo_dic])
      segundo_dic = i;

    if(punt_bigramas[i]>punt_bigramas[mejor_big])
    {
      segundo_big = mejor_big;
      mejor_big = i;
    }
    else if (punt_bigramas[i]>punt_bigramas[segundo_big])
      segundo_big = i;
  }

  double conf_dic = (double) ((apariciones[mejor_dic] - apariciones[segundo_dic])) /
	            (double) (apariciones[mejor_dic]) * 100.0;

  double conf_big = (punt_bigramas[mejor_big] - punt_bigramas[segundo_big]) /
	            (punt_bigramas[mejor_big]) * 100.0;

  Det_id_interno::Idioma_detid decision;

  if(mejor_dic==mejor_big)
  {
    conf = (int)(conf_dic + conf_big);
    decision = linfo_id[mejor_dic].idioma;
  }
  else if(conf_dic >= conf_big)
  {
    conf = (int)(conf_dic - conf_big);
    decision = linfo_id[mejor_dic].idioma;
  }
  else
  {
    conf = (int)(conf_big - conf_dic);
    decision = linfo_id[mejor_big].idioma;
  }

  // Ajusta la confianza
  if(conf>100) 
    conf = 100;
  if(conf<0)
    conf = 0;

#ifdef TEST
  // Presentamos los resultados
  cout << "Resultados Según el Diccionario" << endl << "-------------------------------" << endl;
  cout << "1º idioma diccionario " << linfo_id[mejor_dic].idioma << " con probabilidad media " << apariciones[mejor_dic] << endl;
  cout << "2º idioma diccionario " << linfo_id[segundo_dic].idioma << " con probabilidad " << apariciones[segundo_dic] << endl;
  cout << "Vector confusiones diccionario: ";
  for(i=0;i<nIdi;i++)
    cout << apariciones[i] << " ";
  cout << endl;

  cout << "Resultados Según los Bigramas" << endl << "-----------------------------" << endl;
  cout << "1º idioma bigrama " << linfo_id[mejor_big].idioma << " con probabilidad media " << punt_bigramas[mejor_big] << endl;
  cout << "2º idioma bigrama " << linfo_id[segundo_big].idioma << " con probabilidad media " << punt_bigramas[segundo_big] << endl;
  cout << "Vector confusiones bigrama: ";
  for(i=0;i<nIdi;i++)
    cout << punt_bigramas[i] << " ";
  cout << endl;

  cout << "--------------" << endl;
  cout << "| CONFIANZAS |" << endl;
  cout << "--------------" << endl;

  cout << "N. palabras = " << nPal << endl;

  cout << "Conf_dic = " << (apariciones[mejor_dic] - apariciones[segundo_dic])
       << "/" << apariciones[mejor_dic] << " * 100 = " << conf_dic << endl;

  cout << "Conf_big = "<<(punt_bigramas[mejor_dic] - punt_bigramas[segundo_dic])
       << "/" << punt_bigramas[mejor_dic] << " * 100 = " << conf_big << endl;

  cout << "CONFIANZA FINAL = " << (int) conf << endl;

  cout << "------------" << endl << endl;

#endif // TEST

  delete [] apariciones;
  delete [] punt_bigramas;

  // El idioma siempre es el del diccionario.
  return decision;
}



#ifdef PROBLEMAS_PORT

Det_id_interno::Idioma_detid cad2idi_detid(char *cad)
{
  if (strcmp(cad, "CASTELLANO") == 0) return Det_id_interno::CASTELLANO;
  if (strcmp(cad, "CATALAN") == 0) return Det_id_interno::CATALAN;
  if (strcmp(cad, "GALLEGO") == 0) return Det_id_interno::GALLEGO;
  if (strcmp(cad, "EUSKERA") == 0) return Det_id_interno::EUSKERA;
  if (strcmp(cad, "INGLES") == 0) return Det_id_interno::INGLES;
  if (strcmp(cad, "FRANCES") == 0) return Det_id_interno::FRANCES;
  if (strcmp(cad, "ALEMAN") == 0) return Det_id_interno::ALEMAN;
  if (strcmp(cad, "BRASILEÑO") == 0) return Det_id_interno::BRASILENO;
  if (strcmp(cad, "ITALIANO") == 0) return Det_id_interno::ITALIANO;
  return Det_id_interno::IDIOMA_NULO;
}
 
 
char *idi2cad_detid(Det_id_interno::Idioma_detid idioma)
{
  static char ptrcas[]="CASTELLANO";
  static char ptrcat[]="CATALAN";
  static char ptrgal[]="GALLEGO";
  static char ptreus[]="EUSKERA";
  static char ptring[]="INGLES";
  static char ptrfra[]="FRANCES";
  static char ptrale[]="ALEMAN";
  static char ptrbra[]="BRASILEÑO";
  static char ptrita[]="ITALIANO";
  static char ptrnulo[]="IDIOMA_NULO";
 
  switch (idioma)
  {
    case Det_id_interno::CASTELLANO:
      return ptrcas;
      break;
    case Det_id_interno::CATALAN:
      return ptrcat;
      break;
    case Det_id_interno::GALLEGO:
      return ptrgal;
      break;
    case Det_id_interno::EUSKERA:
      return ptreus;
      break;
    case Det_id_interno::INGLES:
      return ptring;
      break;
    case Det_id_interno::FRANCES:
      return ptrfra;
      break;
    case Det_id_interno::ALEMAN:
      return ptrale;
      break;
    case Det_id_interno::BRASILENO:
      return ptrbra;
      break;
    case Det_id_interno::ITALIANO:
      return ptrita;
      break;
    default:
      return ptrnulo;
      break;
  }
}

#endif //PROBLEMAS_PORT
