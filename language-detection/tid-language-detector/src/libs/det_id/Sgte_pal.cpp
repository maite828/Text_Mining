// Fichero: Sgte_pal.cc
// Descripci�n: Contiene las funciones que realizan la segmentaci�n del 
//              texto en palabras en el corrector ortogr�fico. Esta 
//              segmentaci�n es dependiente del idioma, por lo que se 
//              deber� pasar a esta funci�n un puntero a un objeto de 
//              clase Info_id que le permita a la funci�n acceder a 
//              elementos dependientes del idioma.
//              
//              La funci�n lee el texto, a partir de la posici�n actual
//              y lee una palabra, que devuelve, devolviendo un puntero
//              a la nueva posici�n del texto a estudiar. Una palabra se
//              define como una secuencia de caracteres considerados en 
//              la tabla parcar_segmpal, o de secuencias de caracteres
//              consideradas en la tabla parsec_segmpal. En ambos casos
//              los caracteres o las secuencias encontradas son sustituidas
//              por su car�cter o secuencia asociada seg�n la tabla.
// Fecha: 29/2/2000 DTT
// 
// 5/4/2000 DTT - A partir de hoy estas funciones se usan tambi�n para
//                segmentar el texto en palabras en el detector de idioma.
//                Como aqu� la segmentaci�n es independiente del idioma, 
//                el objeto de la clase Info_id va a contener un par de 
//                punteros a unos objetos parcar_segmpal y parsec_segmpal
//                que se crean en el constructor del Det_id_interno y se 
//                inicializan para que el comportamiento de estas funciones
//                sea exactamente igual al que era hasta entonces.
//                Para conseguir eso es necesario hacer varias cosas:  
//                 - Incluir estas funciones en una clase particular que
//                   llamaremos Segmenta_pal y que se encargar� de segmentar
//                   un texto en palabras.
//                 - Definir en esa clase una clase Info_segmpal que contenga
//                   las cuatro listas que se manejan para realizar la 
//                   segmentaci�n en palabras.
//                 - Hacer que estas funciones reciban un objeto Info_segmpal
//                   en lugar de un objeto Info_id. De esta forma se podr�n
//                   independizar estas funciones de la aplicaci�n (de si es
//                   llamada por el detector de idioma o por el corrector)
//                 - Modificar el Corrector_ortog para que se adapte a esta
//                   nueva interfaz.
//                Todo esto es demasiado complicado, lo dejo de momento.
// 

// 5/4/2000 DTT - A partir de hoy o ma�ana estas funciones ser�n capaces
//                de manejar tambi�n las marcas de texto que manejaremos,
//                por ejemplo, para indicar la presencia de URLs, y que en
//                un futuro cercano pensamos utilizar para indicar el 
//                idioma en el que est� escrito un pedazo de texto.


// NOTA IMPORTANTE: Aunque en principio configurando las tablas parcar_segmpal
// y parsec_segmpal podr�a realizarse aqu� un paso de may�sculas a min�sculas
// de forma autom�tica y configurable para el idioma NO LO VAMOS A HACER. 
// En lugar de eso la funci�n Sgte_pal realizar� unicamente la SEGMENTACI�N 
// en palabras y la TRANSFORMACI�N de algunos caracteres y secuencias, que
// pueden ser dependientes del idioma, en caracteres v�lidos en el idioma
// (un ejemplo de ello puede ser la secuencia &iacute; que se podr�a
// transformar en �.
// El paso de may�sculas a min�sculas y la restauraci�n de las may�sculas
// una vez corregida la palabra debe realizarse fuera de esta funci�n.

// OTRA NOTA: La tabla parcar_validos, que hemos tenido que hacer distinta
// de la tabla parcar_segmpal debe tener en su parte de caracteres
// asociados todos los caracteres que pueden formar parte de una palabra 
// en el idioma. Esto no tiene ninguna relaci�n con este fichero, pero como
// es una nota de una versi�n anterior la dejo.

// Se le pasa en texto un puntero a la posici�n del texto desde la que se
// quiere empezar a buscar la palabra. La palabra encontrada la devuelve 
// en palabra (si encontrase una palabra mayor que MAXLONGPAL_DET_ID, la 
// descarta y sigue buscando). Como resultado devuelve la posici�n del
// buffer inmediatamente posterior a la palabra devuelta, esta es la 
// posici�n en la que habr� que empezar a buscar a continuaci�n.
// info_id es una referencia al objeto Info_id que guarda la informaci�n 
// dependiente del idioma.
// Cuando no encuentra m�s palabras devuelve una cadena vac�a.
// Adem�s, se le debe pasar a esta funci�n un ostrstream en el que se 
// vaya poniendo la salida de la correcci�n ortogr�fica, para que de este
// modo las partes que no son devueltas como palabras se pasen a dicho 
// ostrstream
#ifdef WIN32
#include <strstrea>
#else
#include <strstream>
#endif

#include "Det_id_interno.h"

// LLama a la funci�n Sgte_pal_1 hasta que esta encuentra una palabra
// v�lida.
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

// Versi�n llamada por Sgte_pal. En v�lida devuelve 0 si la palabra
// estudiada no era v�lida y hay que seguir buscano. Devuelve 1 cuando
// la palabra es v�lida y hay que devolverla.  
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

  // Protecci�n
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
    // La vamos a descartar completamente, lo que hemos le�do hasta
    // ahora y lo que sigue hasta el siguiente separador.
    // En el buffer de salida copiaremos lo que hab�a originariamente
    // en el buffer de entrada, desde el inicio de esta palabra fallida

    // Volvemos a lo que cre�amos el inicio de una palabra
    ptr_texto = inicio_temporal_palabra;

    while((*ptr_texto != '\0') &&
          (((char_trad=info_id.parcar_segmpal->Asociado(*ptr_texto))!=0)||
           ((cad_trad =info_id.parsec_segmpal->Asociado((char *)ptr_texto, sig_pos))!=NULL)))
    {
      sal << *ptr_texto;
      ptr_texto++;
    }
    
    // Ahora estamos situados en el siguiente car�cter a una "palabra larga",
    // para buscar la siguiente palabra v�lida hacemos una llamada recursiva
    // return Det_id_interno::Sgte_pal(ptr_texto, palabra, info_id, sal);

    // En lugar de la llamada recursiva, lo que hacemos ahora es devolver la
    // posici�n actual e indicar que la palabra no es v�lida
    valida = 0;
    return (char *) ptr_texto;
  }

  // Hemos encontrado y tenemos una palabra normal, la devolvemos indicando 
  // que es una palabra v�lida.
  *ptr_pal = '\0';
  valida = 1;
  return (char *) ptr_texto;
}

// Este m�todo de Sgte_pal se utiliza cuando en lugar de tener el texto
// como cadena ASCIIZ lo tenemos como un buffer en memoria con un puntero
// que apunta al primer car�cter y otro que apunta a la posici�n siguiente
// al �ltimo car�cter.

// LLama a la funci�n Sgte_pal_1 hasta que esta encuentra una palabra
// v�lida.
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

// Versi�n llamada por Sgte_pal. En v�lida devuelve 0 si la palabra
// estudiada no era v�lida y hay que seguir buscano. Devuelve 1 cuando
// la palabra es v�lida y hay que devolverla.  
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

  // Protecci�n
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
    // La vamos a descartar completamente, lo que hemos le�do hasta
    // ahora y lo que sigue hasta el siguiente separador.
    // En el buffer de salida copiaremos lo que hab�a originariamente
    // en el buffer de entrada, desde el inicio de esta palabra fallida

    // Volvemos a lo que cre�amos el inicio de una palabra
    ptr_texto = inicio_temporal_palabra;

    while((ptr_texto < final_texto) &&
          (((char_trad=info_id.parcar_segmpal->Asociado(*ptr_texto))!=0)||
           ((cad_trad =info_id.parsec_segmpal->Asociado((char *)ptr_texto, sig_pos))!=NULL)))
    {
      sal << *ptr_texto;
      ptr_texto++;
    }
    
    // Ahora estamos situados en el siguiente car�cter a una "palabra larga",
    // para buscar la siguiente palabra v�lida hacemos una llamada recursiva
    // return Det_id_interno::Sgte_pal(ptr_texto, final_texto, palabra, 
    //                                  info_id, sal);
    // En lugar de la llamada recursiva, lo que hacemos ahora es devolver la
    // posici�n actual e indicar que la palabra no es v�lida
    valida = 0;
    return (char *) ptr_texto;
  }

  // Hemos encontrado y tenemos una palabra normal, la devolvemos:
  *ptr_pal = '\0';
  valida = 1;
  return (char *) ptr_texto;
}

// Esta funci�n es la versi�n especializada para el obten_palabras.
// Lo que hace de especial es que no recibe un ostrstream de salida,
// directamente tira todo lo que no es una palabra, y devuelve s�lo
// las palabras v�lidas, no solo de acuerdo a los criterios de 
// segmentaci�n normales, sino que adem�s deben cumplir los siguientes
// requisitos:
// 
//   - Que tengan s�lo caracteres v�lidos en el idioma.
//   - Que sean de longitud >= MINLONGPAL_DET_ID.
//   - Que su patr�n de may�sculas/min�sculas sea de los que se van a
//     corregir.
//
// Adem�s devuelve la palabra pasada a min�sculas.
// Y por si fueran pocas modificaciones, tambi�n supone que s�lo hay un
// modelo de idioma cargado, y siempre toma el modelo de idioma en la 
// primera posici�n de la lista.
//
// Una �ltima diferencia. El texto no es una cadena ASCIIZ, sino un 
// buffer que empieza en inicio y termina en final-1

// LLama a la funci�n Sgte_pal_obtenpal_1 hasta que esta encuentra una palabra
// v�lida.
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

// Versi�n llamada por Sgte_pal_obtenpal. En v�lida devuelve 0 si la palabra
// estudiada no era v�lida y hay que seguir buscano. Devuelve 1 cuando
// la palabra es v�lida y hay que devolverla.  
char *Det_id_interno::Sgte_pal_obtenpal_1(const char *inicio, 
                                           const char *final,
                                           char *palabra,
                                           int &valida) 
{
  // Toma el primer modelo de correcci�n disponible.
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

  // Protecci�n
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
    // La vamos a descartar completamente, lo que hemos le�do hasta
    // ahora y lo que sigue hasta el siguiente separador.

    while((ptr_texto < final) &&
          (((char_trad=info_id.parcar_segmpal->Asociado(*ptr_texto))!=0)||
           ((cad_trad =info_id.parsec_segmpal->Asociado((char *)ptr_texto, sig_pos))!=NULL)))
    {
      ptr_texto++;
    }
    
    // Ahora estamos situados en el siguiente car�cter a una "palabra larga",
    // para buscar la siguiente palabra v�lida hacemos una llamada recursiva
    // return Sgte_pal_obtenpal(ptr_texto, final, palabra);
    // En lugar de la llamada recursiva, lo que hacemos ahora es devolver la
    // posici�n actual e indicar que la palabra no es v�lida
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
    // posici�n actual e indicar que la palabra no es v�lida
    valida = 0;
    return (char *) ptr_texto;
  }

  // Pasamos la palabra a min�scula
  Transforma_a_min(palabra,pal_min,cambios);

  // Obtiene el patr�n de may�sculas/min�sculas
  patron = Patron_maymin(cambios);

  // Comprueba que el patron sea de los v�lidos
  if(((ptrch=info_id.parsec_maymin->Asociado(patron))!=NULL)&&
     (strcmp(ptrch,"CORR")))
  {
    // Si no es una palabra de las que haya que corregir buscamos la siguiente
    // return Sgte_pal_obtenpal(ptr_texto, final, palabra);
    // En lugar de la llamada recursiva, lo que hacemos ahora es devolver la
    // posici�n actual e indicar que la palabra no es v�lida
    valida = 0;
    return (char *) ptr_texto;
  }

  // Comprobamos que tenga s�lo caracteres v�lidos en el idioma
  ptrch = pal_min;
  while(*ptrch!='\0')
  {
    if(strchr((char *)info_id.letras_validas,*ptrch)==NULL)
    {
      // Si hay un car�cter inv�lido buscamos la siguiente palabra
      // return Sgte_pal_obtenpal(ptr_texto, final, palabra);
      // En lugar de la llamada recursiva, lo que hacemos ahora es devolver la
      // posici�n actual e indicar que la palabra no es v�lida
      valida = 0;
      return (char *) ptr_texto;
    }
    ptrch++;
  }

  // La palabra a devolver es la palabra en min�sculas
  strcpy(palabra,pal_min);
  valida = 1;
  return (char *) ptr_texto;
}


 
