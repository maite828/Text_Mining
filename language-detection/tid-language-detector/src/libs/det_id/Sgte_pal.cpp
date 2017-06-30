// Fichero: Sgte_pal.cc
// Descripción: Contiene las funciones que realizan la segmentación del 
//              texto en palabras en el corrector ortográfico. Esta 
//              segmentación es dependiente del idioma, por lo que se 
//              deberá pasar a esta función un puntero a un objeto de 
//              clase Info_id que le permita a la función acceder a 
//              elementos dependientes del idioma.
//              
//              La función lee el texto, a partir de la posición actual
//              y lee una palabra, que devuelve, devolviendo un puntero
//              a la nueva posición del texto a estudiar. Una palabra se
//              define como una secuencia de caracteres considerados en 
//              la tabla parcar_segmpal, o de secuencias de caracteres
//              consideradas en la tabla parsec_segmpal. En ambos casos
//              los caracteres o las secuencias encontradas son sustituidas
//              por su carácter o secuencia asociada según la tabla.
// Fecha: 29/2/2000 DTT
// 
// 5/4/2000 DTT - A partir de hoy estas funciones se usan también para
//                segmentar el texto en palabras en el detector de idioma.
//                Como aquí la segmentación es independiente del idioma, 
//                el objeto de la clase Info_id va a contener un par de 
//                punteros a unos objetos parcar_segmpal y parsec_segmpal
//                que se crean en el constructor del Det_id_interno y se 
//                inicializan para que el comportamiento de estas funciones
//                sea exactamente igual al que era hasta entonces.
//                Para conseguir eso es necesario hacer varias cosas:  
//                 - Incluir estas funciones en una clase particular que
//                   llamaremos Segmenta_pal y que se encargará de segmentar
//                   un texto en palabras.
//                 - Definir en esa clase una clase Info_segmpal que contenga
//                   las cuatro listas que se manejan para realizar la 
//                   segmentación en palabras.
//                 - Hacer que estas funciones reciban un objeto Info_segmpal
//                   en lugar de un objeto Info_id. De esta forma se podrán
//                   independizar estas funciones de la aplicación (de si es
//                   llamada por el detector de idioma o por el corrector)
//                 - Modificar el Corrector_ortog para que se adapte a esta
//                   nueva interfaz.
//                Todo esto es demasiado complicado, lo dejo de momento.
// 

// 5/4/2000 DTT - A partir de hoy o mañana estas funciones serán capaces
//                de manejar también las marcas de texto que manejaremos,
//                por ejemplo, para indicar la presencia de URLs, y que en
//                un futuro cercano pensamos utilizar para indicar el 
//                idioma en el que está escrito un pedazo de texto.


// NOTA IMPORTANTE: Aunque en principio configurando las tablas parcar_segmpal
// y parsec_segmpal podría realizarse aquí un paso de mayúsculas a minúsculas
// de forma automática y configurable para el idioma NO LO VAMOS A HACER. 
// En lugar de eso la función Sgte_pal realizará unicamente la SEGMENTACIÓN 
// en palabras y la TRANSFORMACIÓN de algunos caracteres y secuencias, que
// pueden ser dependientes del idioma, en caracteres válidos en el idioma
// (un ejemplo de ello puede ser la secuencia &iacute; que se podría
// transformar en í.
// El paso de mayúsculas a minúsculas y la restauración de las mayúsculas
// una vez corregida la palabra debe realizarse fuera de esta función.

// OTRA NOTA: La tabla parcar_validos, que hemos tenido que hacer distinta
// de la tabla parcar_segmpal debe tener en su parte de caracteres
// asociados todos los caracteres que pueden formar parte de una palabra 
// en el idioma. Esto no tiene ninguna relación con este fichero, pero como
// es una nota de una versión anterior la dejo.

// Se le pasa en texto un puntero a la posición del texto desde la que se
// quiere empezar a buscar la palabra. La palabra encontrada la devuelve 
// en palabra (si encontrase una palabra mayor que MAXLONGPAL_DET_ID, la 
// descarta y sigue buscando). Como resultado devuelve la posición del
// buffer inmediatamente posterior a la palabra devuelta, esta es la 
// posición en la que habrá que empezar a buscar a continuación.
// info_id es una referencia al objeto Info_id que guarda la información 
// dependiente del idioma.
// Cuando no encuentra más palabras devuelve una cadena vacía.
// Además, se le debe pasar a esta función un ostrstream en el que se 
// vaya poniendo la salida de la corrección ortográfica, para que de este
// modo las partes que no son devueltas como palabras se pasen a dicho 
// ostrstream
#ifdef WIN32
#include <strstrea>
#else
#include <strstream>
#endif

#include "Det_id_interno.h"

// LLama a la función Sgte_pal_1 hasta que esta encuentra una palabra
// válida.
char *Det_id_interno::Sgte_pal(const char *inicio, char *palabra, 
                                Info_id &info_id, std::ostrstream &sal) 
{
  int valida=0;
  const char *ptr_texto = inicio;

  do
  {
    ptr_texto = Sgte_pal_1(ptr_texto, palabra, info_id, sal, valida);
  } while ((ptr_texto!=NULL)&&(valida==0));

  return (char *) ptr_texto;
}

// Versión llamada por Sgte_pal. En válida devuelve 0 si la palabra
// estudiada no era válida y hay que seguir buscano. Devuelve 1 cuando
// la palabra es válida y hay que devolverla.  
char *Det_id_interno::Sgte_pal_1(const char *inicio, char *palabra, 
                                  Info_id &info_id, std::ostrstream &sal,
                                  int &valida) 
{
  const char *ptr_texto = inicio;
  const char *inicio_temporal_palabra;
  char *ptr_pal = palabra;
  int nCar = 0;
  char char_trad;
  char *cad_trad;
  char *cad_markup;
  char *posicion;
  char **sig_pos=&posicion;
  static int escaped = 0;

  // Protección
  if (inicio == NULL)
  {
    *palabra = '\0';
    return NULL;
  }

  // Buscamos un inicio posible de palabra
  while (( *ptr_texto!= '\0')&&
         ((char_trad=info_id.parcar_segmpal->Asociado(*ptr_texto))==0)&&
         ((cad_trad =info_id.parsec_segmpal->Asociado((char *)ptr_texto, 
                                                      sig_pos))==NULL)&&
         ((escaped)||
          ((cad_markup=info_id.parsec_markup->Asociado((char *)ptr_texto,
                                                       sig_pos))==NULL)))
  {
    if(*ptr_texto=='\\')
      escaped = !escaped;
    sal << *ptr_texto;
    ptr_texto++;
  }

  if (*ptr_texto == '\0')
  {
    *palabra = '\0';
    return NULL;
  }

  // Lee la palabra completa
  inicio_temporal_palabra = ptr_texto;

  if(char_trad!=0)
  {
    *ptr_pal = char_trad;
    ptr_pal++;
    ptr_texto++;
    nCar++;
  }
  else if(cad_trad!=NULL)
  {
    strcpy(ptr_pal,cad_trad);
    ptr_pal+=strlen(cad_trad);
    ptr_texto = *sig_pos;
    nCar+=strlen(cad_trad);
  }
  else if(cad_markup!=NULL)
  {
    strcpy(ptr_pal,cad_markup);
    ptr_texto = *sig_pos;
    valida = 1;
    return (char *) ptr_texto;
  }
  
  while((*ptr_texto != '\0') &&
        (((char_trad=info_id.parcar_segmpal->Asociado(*ptr_texto))!=0)||
         ((cad_trad =info_id.parsec_segmpal->Asociado((char *)ptr_texto, sig_pos))!=NULL))&&
        (nCar < MAXLONGPAL_DET_ID))
  {
    if(char_trad!=0)
    {
      *ptr_pal = char_trad;
      ptr_pal++;
      ptr_texto++;
      nCar++;
    }
    else if(cad_trad!=NULL)
    {
      strcpy(ptr_pal,cad_trad);
      ptr_pal+=strlen(cad_trad);
      ptr_texto = *sig_pos;
      nCar+=strlen(cad_trad);
    }
  }

  if(nCar >= MAXLONGPAL_DET_ID)
  {
    // Hemos encontrado una palabra mayor de MAXLONGPAL_DET_ID
    // La vamos a descartar completamente, lo que hemos leído hasta
    // ahora y lo que sigue hasta el siguiente separador.
    // En el buffer de salida copiaremos lo que había originariamente
    // en el buffer de entrada, desde el inicio de esta palabra fallida

    // Volvemos a lo que creíamos el inicio de una palabra
    ptr_texto = inicio_temporal_palabra;

    while((*ptr_texto != '\0') &&
          (((char_trad=info_id.parcar_segmpal->Asociado(*ptr_texto))!=0)||
           ((cad_trad =info_id.parsec_segmpal->Asociado((char *)ptr_texto, sig_pos))!=NULL)))
    {
      sal << *ptr_texto;
      ptr_texto++;
    }
    
    // Ahora estamos situados en el siguiente carácter a una "palabra larga",
    // para buscar la siguiente palabra válida hacemos una llamada recursiva
    // return Det_id_interno::Sgte_pal(ptr_texto, palabra, info_id, sal);

    // En lugar de la llamada recursiva, lo que hacemos ahora es devolver la
    // posición actual e indicar que la palabra no es válida
    valida = 0;
    return (char *) ptr_texto;
  }

  // Hemos encontrado y tenemos una palabra normal, la devolvemos indicando 
  // que es una palabra válida.
  *ptr_pal = '\0';
  valida = 1;
  return (char *) ptr_texto;
}

// Este método de Sgte_pal se utiliza cuando en lugar de tener el texto
// como cadena ASCIIZ lo tenemos como un buffer en memoria con un puntero
// que apunta al primer carácter y otro que apunta a la posición siguiente
// al último carácter.

// LLama a la función Sgte_pal_1 hasta que esta encuentra una palabra
// válida.
char *Det_id_interno::Sgte_pal(const char *inicio_texto, 
                                const char *final_texto,
                                char *palabra, 
                                Info_id &info_id, std::ostrstream &sal) 
{
  int valida=0;
  const char *ptr_texto = inicio_texto;

  do
  {
    ptr_texto = Sgte_pal_1(ptr_texto, final_texto, palabra, 
                           info_id, sal, valida);
  } while ((ptr_texto!=NULL)&&(valida==0));

  return (char *) ptr_texto;
}

// Versión llamada por Sgte_pal. En válida devuelve 0 si la palabra
// estudiada no era válida y hay que seguir buscano. Devuelve 1 cuando
// la palabra es válida y hay que devolverla.  
char *Det_id_interno::Sgte_pal_1(const char *inicio_texto,  
                                  const char *final_texto,
                                  char *palabra, 
                                  Info_id &info_id, std::ostrstream &sal,
                                  int &valida) 
{
  const char *ptr_texto = inicio_texto;
  const char *inicio_temporal_palabra;
  char *ptr_pal = palabra;
  int nCar = 0;
  char char_trad;
  char *cad_trad;
  char *cad_markup;
  char *posicion;
  char **sig_pos=&posicion;
  static int escaped = 0;

  // Protección
  if (inicio_texto == NULL)
  {
    *palabra = '\0';
    return NULL;
  }

  // Buscamos un inicio posible de palabra
  while (( ptr_texto<final_texto)&&
         ((char_trad=info_id.parcar_segmpal->Asociado(*ptr_texto))==0)&&
         ((cad_trad =info_id.parsec_segmpal->Asociado((char *)ptr_texto, 
                                                      sig_pos))==NULL)&&
         ((escaped)||
          ((cad_markup=info_id.parsec_markup->Asociado((char *)ptr_texto,
                                                       sig_pos))==NULL)))
  {
    if(*ptr_texto=='\\')
      escaped = !escaped;
    sal << *ptr_texto;
    ptr_texto++;
  }

  if (ptr_texto == final_texto)
  {
    *palabra = '\0';
    return NULL;
  }

  // Lee la palabra completa
  inicio_temporal_palabra = ptr_texto;

  if(char_trad!=0)
  {
    *ptr_pal = char_trad;
    ptr_pal++;
    ptr_texto++;
    nCar++;
  }
  else if(cad_trad!=NULL)
  {
    strcpy(ptr_pal,cad_trad);
    ptr_pal+=strlen(cad_trad);
    ptr_texto = *sig_pos;
    nCar+=strlen(cad_trad);
  }
  else if(cad_markup!=NULL)
  {
    strcpy(ptr_pal,cad_markup);
    ptr_texto = *sig_pos;
    valida = 1;
    return (char *) ptr_texto;
  }
  
  while((ptr_texto < final_texto) &&
        (((char_trad=info_id.parcar_segmpal->Asociado(*ptr_texto))!=0)||
         ((cad_trad =info_id.parsec_segmpal->Asociado((char *)ptr_texto, sig_pos))!=NULL))&&
        (nCar < MAXLONGPAL_DET_ID))
  {
    if(char_trad!=0)
    {
      *ptr_pal = char_trad;
      ptr_pal++;
      ptr_texto++;
      nCar++;
    }
    else if(cad_trad!=NULL)
    {
      strcpy(ptr_pal,cad_trad);
      ptr_pal+=strlen(cad_trad);
      ptr_texto = *sig_pos;
      nCar+=strlen(cad_trad);
    }
  }

  if(nCar >= MAXLONGPAL_DET_ID)
  {
    // Hemos encontrado una palabra mayor de MAXLONGPAL_DET_ID
    // La vamos a descartar completamente, lo que hemos leído hasta
    // ahora y lo que sigue hasta el siguiente separador.
    // En el buffer de salida copiaremos lo que había originariamente
    // en el buffer de entrada, desde el inicio de esta palabra fallida

    // Volvemos a lo que creíamos el inicio de una palabra
    ptr_texto = inicio_temporal_palabra;

    while((ptr_texto < final_texto) &&
          (((char_trad=info_id.parcar_segmpal->Asociado(*ptr_texto))!=0)||
           ((cad_trad =info_id.parsec_segmpal->Asociado((char *)ptr_texto, sig_pos))!=NULL)))
    {
      sal << *ptr_texto;
      ptr_texto++;
    }
    
    // Ahora estamos situados en el siguiente carácter a una "palabra larga",
    // para buscar la siguiente palabra válida hacemos una llamada recursiva
    // return Det_id_interno::Sgte_pal(ptr_texto, final_texto, palabra, 
    //                                  info_id, sal);
    // En lugar de la llamada recursiva, lo que hacemos ahora es devolver la
    // posición actual e indicar que la palabra no es válida
    valida = 0;
    return (char *) ptr_texto;
  }

  // Hemos encontrado y tenemos una palabra normal, la devolvemos:
  *ptr_pal = '\0';
  valida = 1;
  return (char *) ptr_texto;
}

// Esta función es la versión especializada para el obten_palabras.
// Lo que hace de especial es que no recibe un ostrstream de salida,
// directamente tira todo lo que no es una palabra, y devuelve sólo
// las palabras válidas, no solo de acuerdo a los criterios de 
// segmentación normales, sino que además deben cumplir los siguientes
// requisitos:
// 
//   - Que tengan sólo caracteres válidos en el idioma.
//   - Que sean de longitud >= MINLONGPAL_DET_ID.
//   - Que su patrón de mayúsculas/minúsculas sea de los que se van a
//     corregir.
//
// Además devuelve la palabra pasada a minúsculas.
// Y por si fueran pocas modificaciones, también supone que sólo hay un
// modelo de idioma cargado, y siempre toma el modelo de idioma en la 
// primera posición de la lista.
//
// Una última diferencia. El texto no es una cadena ASCIIZ, sino un 
// buffer que empieza en inicio y termina en final-1

// LLama a la función Sgte_pal_obtenpal_1 hasta que esta encuentra una palabra
// válida.
char *Det_id_interno::Sgte_pal_obtenpal(const char *inicio_texto, 
                                         const char *final_texto,
                                         char *palabra)
{
  int valida=0;
  const char *ptr_texto = inicio_texto;

  do
  {
    ptr_texto = Sgte_pal_obtenpal_1(ptr_texto, final_texto, palabra, valida);
  } while ((ptr_texto!=NULL)&&(valida==0));

  return (char *) ptr_texto;
}

// Versión llamada por Sgte_pal_obtenpal. En válida devuelve 0 si la palabra
// estudiada no era válida y hay que seguir buscano. Devuelve 1 cuando
// la palabra es válida y hay que devolverla.  
char *Det_id_interno::Sgte_pal_obtenpal_1(const char *inicio, 
                                           const char *final,
                                           char *palabra,
                                           int &valida) 
{
  // Toma el primer modelo de corrección disponible.
  Info_id info_id = linfo_id[0];
  const char *ptr_texto = inicio;
  char *ptr_pal = palabra;
  char *ptrch;
  int nCar = 0;
  char char_trad;
  char *cad_trad;
  char *cad_markup;
  char *posicion;
  char **sig_pos=&posicion;
  char pal_min[MAXLONGPAL_DET_ID];
  char cambios[MAXLONGPAL_DET_ID];
  char *patron;
  static int escaped = 0;

  // Protección
  if (inicio == NULL)
  {
    *palabra = '\0';
    return NULL;
  }

  // Buscamos un inicio posible de palabra
  while ((ptr_texto<final)&&
         ((char_trad=info_id.parcar_segmpal->Asociado(*ptr_texto))==0)&&
         ((cad_trad =info_id.parsec_segmpal->Asociado((char *)ptr_texto, 
                                                      sig_pos))==NULL)&&
         ((escaped)||
          ((cad_markup=info_id.parsec_markup->Asociado((char *)ptr_texto,
                                                       sig_pos))==NULL)))
  {
    if(*ptr_texto=='\\')
      escaped = !escaped;
    ptr_texto++;
  }

  if (ptr_texto==final)
  {
    *palabra = '\0';
    return NULL;
  }

  // Lee la palabra completa

  if(char_trad!=0)
  {
    *ptr_pal = char_trad;
    ptr_pal++;
    ptr_texto++;
    nCar++;
  }
  else if(cad_trad!=NULL)
  {
    strcpy(ptr_pal,cad_trad);
    ptr_pal+=strlen(cad_trad);
    ptr_texto = *sig_pos;
    nCar+=strlen(cad_trad);
  }
  else if(cad_markup!=NULL)
  {
    strcpy(ptr_pal,cad_markup);
    ptr_texto = *sig_pos;
    valida = 1;
    return (char *) ptr_texto;
  }
  
  while((ptr_texto < final) &&
        (((char_trad=info_id.parcar_segmpal->Asociado(*ptr_texto))!=0)||
         ((cad_trad =info_id.parsec_segmpal->Asociado((char *)ptr_texto, sig_pos))!=NULL))&&
        (nCar < MAXLONGPAL_DET_ID))
  {
    if(char_trad!=0)
    {
      *ptr_pal = char_trad;
      ptr_pal++;
      ptr_texto++;
      nCar++;
    }
    else if(cad_trad!=NULL)
    {
      strcpy(ptr_pal,cad_trad);
      ptr_pal+=strlen(cad_trad);
      ptr_texto = *sig_pos;
      nCar+=strlen(cad_trad);
    }
  }

  if(nCar >= MAXLONGPAL_DET_ID)
  {
    // Hemos encontrado una palabra mayor de MAXLONGPAL_DET_ID
    // La vamos a descartar completamente, lo que hemos leído hasta
    // ahora y lo que sigue hasta el siguiente separador.

    while((ptr_texto < final) &&
          (((char_trad=info_id.parcar_segmpal->Asociado(*ptr_texto))!=0)||
           ((cad_trad =info_id.parsec_segmpal->Asociado((char *)ptr_texto, sig_pos))!=NULL)))
    {
      ptr_texto++;
    }
    
    // Ahora estamos situados en el siguiente carácter a una "palabra larga",
    // para buscar la siguiente palabra válida hacemos una llamada recursiva
    // return Sgte_pal_obtenpal(ptr_texto, final, palabra);
    // En lugar de la llamada recursiva, lo que hacemos ahora es devolver la
    // posición actual e indicar que la palabra no es válida
    valida = 0;
    return (char *) ptr_texto;
  }

  // Hemos encontrado y tenemos una palabra normal, hacemos las comprobaciones
  // antes de devolverla.
  *ptr_pal = '\0';

  // Comprobamos la longitud
  if(strlen(palabra)<MINLONGPAL_DET_ID)
  {
    // Si es demasiado corta buscamos otra
    // return Sgte_pal_obtenpal(ptr_texto, final, palabra);
    // En lugar de la llamada recursiva, lo que hacemos ahora es devolver la
    // posición actual e indicar que la palabra no es válida
    valida = 0;
    return (char *) ptr_texto;
  }

  // Pasamos la palabra a minúscula
  Transforma_a_min(palabra,pal_min,cambios);

  // Obtiene el patrón de mayúsculas/minúsculas
  patron = Patron_maymin(cambios);

  // Comprueba que el patron sea de los válidos
  if(((ptrch=info_id.parsec_maymin->Asociado(patron))!=NULL)&&
     (strcmp(ptrch,"CORR")))
  {
    // Si no es una palabra de las que haya que corregir buscamos la siguiente
    // return Sgte_pal_obtenpal(ptr_texto, final, palabra);
    // En lugar de la llamada recursiva, lo que hacemos ahora es devolver la
    // posición actual e indicar que la palabra no es válida
    valida = 0;
    return (char *) ptr_texto;
  }

  // Comprobamos que tenga sólo caracteres válidos en el idioma
  ptrch = pal_min;
  while(*ptrch!='\0')
  {
    if(strchr((char *)info_id.letras_validas,*ptrch)==NULL)
    {
      // Si hay un carácter inválido buscamos la siguiente palabra
      // return Sgte_pal_obtenpal(ptr_texto, final, palabra);
      // En lugar de la llamada recursiva, lo que hacemos ahora es devolver la
      // posición actual e indicar que la palabra no es válida
      valida = 0;
      return (char *) ptr_texto;
    }
    ptrch++;
  }

  // La palabra a devolver es la palabra en minúsculas
  strcpy(palabra,pal_min);
  valida = 1;
  return (char *) ptr_texto;
}


 
