//////////////////////////////////////////////////////////
//
// M�todos de la Clase Pares_secuencias
//
// Mantiene una tabla de pares de secuencias de caracteres de longitud
// variable.
// La tabla de pares de secuencias de caracteres admitir� dos
// representaciones, una binaria, que se usa en memoria y en los
// ficheros binarios que la contengan, y otra ascii que se usar�
// para crear la tabla de pares de secuencias de caracteres.
// La clase proporciona un m�todo Convierte_a_binario que pasa de
// la representaci�n en ASCII a la binaria. No hay conversi�n a la
// inversa.
//
// 29/2/2000 DTT - Creaci�n de la clase
//
//////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#include <windows.h>
#else
#include <sys/mman.h>
#endif

#include <string.h>

#include "Pares_secuencias.h"


// Constructor
// Inicializa la tabla a partir de la imagen binaria ya cargada en memoria
Pares_secuencias::Pares_secuencias(char *imagen_en_memoria)
{
  lista_bin = imagen_en_memoria;
}

// Destructor
// No hace nada
Pares_secuencias::~Pares_secuencias(void)
{
}

// M�todo para obtener la secuencia de caracteres asociada a otra por la tabla
// Devuelve NULL si no est� 
char *Pares_secuencias::Asociado(char *cadena)
{
  unsigned char *ptr=(unsigned char *)lista_bin;
  unsigned char *next;
  while(*ptr!=0)
  {
    next=ptr+*ptr;
    ptr++;
    unsigned char *ptrcad=(unsigned char *)cadena;
    while((*ptr==*ptrcad)&&(*ptr!=0)&&(*ptrcad!=0))
    {
      ptr++;
      ptrcad++;
    }
    if(!*ptr && !*ptrcad)
    {
      ptr++;
      return (char *)ptr;
    }
    ptr=next;
  }
  // El �ltimo elemento de la tabla tiene un offset 0 y
  // no tiene cadenas asociada.

  return(NULL);
}

// M�todo para obtener la secuencia de caracteres asociada a otra por la tabla
// Devuelve NULL si no est� 
// Este m�todo utiliza una referencia a un puntero a car�cter que permite
// encontrar TODAS las secuencias de caracteres asociadas a una dada.
// Para ello se debe usar un puntero sig_seek que se inicialice a NULL.
// Cada vez que se llame a esta funci�n devolver� la siguiente secuencia
// asociada y actualizar� el puntero, que servir� en la pr�xima llamada 
// para determinar la siguiente secuencia asociada. Cuando no haya m�s
// secuencias asociadas devolver� NULL y en sig_seek devolver� un puntero tal
// que sucesivas llamadas usando dicho puntero siempre hagan que se
// devuelva un NULL.
// El puntero sig_seek no debe utilizarse ni modificarse salvo para
// inicializarlo a NULL.
char *Pares_secuencias::Asociado(char *cadena, char *&sig_seek)
{
  unsigned char *ptr;
  unsigned char *next;

  if(sig_seek==NULL)
    ptr=(unsigned char *)lista_bin;
  else
    ptr=(unsigned char *)sig_seek;
  while(*ptr!=0)
  {
    next=ptr+*ptr;
    ptr++;
    unsigned char *ptrcad=(unsigned char *)cadena;
    while((*ptr==*ptrcad)&&(*ptr!=0)&&(*ptrcad!=0))
    {
      ptr++;
      ptrcad++;
    }
    if(!*ptr && !*ptrcad)
    {
      sig_seek = (char *)next;
      ptr++;
      return (char *)ptr;
    }
    ptr=next;
  }
  // El �ltimo elemento de la tabla tiene un offset 0 y
  // no tiene cadenas asociada.

  sig_seek = (char *)ptr;
  return(NULL);
}

// M�todo adaptado especialmente a las necesidades del corrector ortogr�fico
// La adaptaci�n consiste en que devuelve la cadena traducida, termine o no
// termine la cadena que se le pasa, en el mismo instante en que termina la
// cadena encontrada en la tabla. Otra especializaci�n es que devuelve un
// puntero al caracter siguiente al que ha coincidido con la cadena 
// encontrada en la tabla.
// M�todo para obtener la secuencia de caracteres asociada a otra por la tabla
// Devuelve NULL si no est� 
char *Pares_secuencias::Asociado(char *cadena, char **sig_pos)
{
  unsigned char *ptr=(unsigned char *)lista_bin;
  unsigned char *next;
  while(*ptr!=0)
  {
    next=ptr+*ptr;
    ptr++;
    unsigned char *ptrcad=(unsigned char *)cadena;
    while((*ptr==*ptrcad)&&(*ptr!=0))
    {
      ptr++;
      ptrcad++;
    }
    if(!*ptr)
    {
      ptr++;
      *sig_pos = (char *) ptrcad;
      return (char *)ptr;
    }
    ptr=next;
  }

  // El �ltimo elemento de la tabla tiene un offset 0 y
  // no tiene cadenas asociada.

  *sig_pos = cadena;
  return(NULL);
}

// M�todo adaptado especialmente a las necesidades del corrector ortogr�fico
// La adaptaci�n consiste en que devuelve la cadena traducida, termine o no
// termine la cadena que se le pasa, en el mismo instante en que termina la
// cadena encontrada en la tabla. Otra especializaci�n es que devuelve un
// puntero al caracter siguiente al que ha coincidido con la cadena 
// encontrada en la tabla.
// M�todo para obtener TODAS las secuencias de caracteres asociadas a una
// por la tabla.
// Devuelve NULL si no est�.
//
// Este m�todo utiliza una referencia a un puntero a car�cter que permite
// encontrar TODAS las secuencias asociadas a una secuencia de caracteres
// dada. Para ello se debe usar un puntero sig_seek que se inicialice a NULL
// Cada vez que se llame a esta funci�n devolver� la siguiente secuencia
// asociada, la siguiente posici�n en la cadena, tras la secuencia a la que
// corresponde la secuencia asociada (sig_pos) y actualizar� el puntero 
// sig_seek, que servir� en la pr�xima llamada para determinar la siguiente
// cadena asociada. Cuando no haya m�s cadenas asociadas devolver� NULL y
// en sig_seek devolver� un puntero tal que sucesivas llamadas usando dicho
// puntero hagan que se devuelva NULL.
// El puntero sig_seek no debe utilizarse ni modificarse salvo para 
// inicializarlo a NULL.
char *Pares_secuencias::Asociado(char *cadena, char **sig_pos, char *&sig_seek)
{
  unsigned char *ptr;
  unsigned char *next;

  if(sig_seek!=NULL)
    ptr=(unsigned char *)sig_seek;
  else
    ptr=(unsigned char *)lista_bin;
  while(*ptr!=0)
  {
    next=ptr+*ptr;
    ptr++;
    unsigned char *ptrcad=(unsigned char *)cadena;
    while((*ptr==*ptrcad)&&(*ptr!=0))
    {
      ptr++;
      ptrcad++;
    }
    if(!*ptr)
    {
      sig_seek = (char *) next;
      *sig_pos = (char *) ptrcad;
      ptr++;
      return (char *)ptr;
    }
    ptr=next;
  }

  // El �ltimo elemento de la tabla tiene un offset 0 y
  // no tiene cadenas asociada.

  *sig_pos = cadena;
  sig_seek = (char *)ptr;
  return(NULL);
}

// M�todo adaptado especialmente a las necesidades del corrector ortogr�fico
// La adaptaci�n consiste en que devuelve la cadena traducida, termine o no
// termine la cadena que se le pasa, en el mismo instante en que termina la
// cadena encontrada en la tabla. Otra especializaci�n es que devuelve un
// puntero al caracter siguiente al que ha coincidido con la cadena 
// encontrada en la tabla.
// M�todo para obtener TODAS las secuencias de caracteres asociadas a una
// por la tabla.
// Devuelve NULL si no est�.
//
// Este m�todo utiliza una referencia a un puntero a car�cter que permite
// encontrar TODAS las secuencias asociadas a una secuencia de caracteres
// dada. Para ello se debe usar un puntero sig_seek que se inicialice a NULL
// Cada vez que se llame a esta funci�n devolver� la siguiente secuencia
// asociada, la siguiente posici�n en la cadena, tras la secuencia a la que
// corresponde la secuencia asociada (sig_pos) y actualizar� el puntero 
// sig_seek, que servir� en la pr�xima llamada para determinar la siguiente
// cadena asociada. Cuando no haya m�s cadenas asociadas devolver� NULL y
// en sig_seek devolver� un puntero tal que sucesivas llamadas usando dicho
// puntero hagan que se devuelva NULL.
// El puntero sig_seek no debe utilizarse ni modificarse salvo para 
// inicializarlo a NULL.
//
// Este m�todo permite obtener la asociaci�n inversa, es decir, que cadena
// se busca entre las cadenas asocidas. Si se encuentra se devuelve la cadena
// original. Si no se devuelve NULL. El sig_pos y el sig_seek se utilizan
// igual que en la otra.
char *Pares_secuencias::AsociadoInv(char *cadena, char **sig_pos, 
                                    char *&sig_seek)
{
  unsigned char *ptr;
  unsigned char *ptr2;
  unsigned char *next;

  if(sig_seek!=NULL)
    ptr=(unsigned char *)sig_seek;
  else
    ptr=(unsigned char *)lista_bin;
  while(*ptr!=0)
  {
    next=ptr+*ptr;
    ptr++;
    ptr2=ptr+strlen((const char *)ptr)+1;
    unsigned char *ptrcad=(unsigned char *)cadena;
    while((*ptr2==*ptrcad)&&(*ptr2!=0))
    {
      ptr2++;
      ptrcad++;
    }
    if(!*ptr2)
    {
      sig_seek = (char *) next;
      *sig_pos = (char *) ptrcad;
      return (char *)ptr;
    }
    ptr=next;
  }

  // El �ltimo elemento de la tabla tiene un offset 0 y
  // no tiene cadenas asociada.

  *sig_pos = cadena;
  sig_seek = (char *)ptr;
  return(NULL);
}

// M�todo para transformar la representaci�n en ASCII de la tabla de
// secuencias de caracteres (un fichero con l�neas que contienen dos cadenas
// separadaso por | y en el que se considera que las cadenas incluyen
// cualquier car�cter, incluso espacios en blanco, salvo | y retorno de carro,
// y en el que la primera cadena es la cadena a buscar y la segunda la cadena
// asocidada) en la representaci�n interna, que consiste, para cada par de 
// cadenas en un primer unsigned char que indica el offset hasta el siguiente
// par de cadenas, seguido de dos cadenas ASCIIZ, que son la cadena a buscar y
// la cadena asociada.
// Esta funci�n se supone que es s�lo de desarrollo, por lo que puede dar
// errores por pantalla.
// Adem�s, es est�tica: No necesita un objeto para funcionar.
void Pares_secuencias::Convierte_a_binario(const char *fich_ascii, 
                                           const char *fich_binario)
{
  FILE *fin, *fout;
  char cad[254];
  char *ptr;
  int n,nbytes;
  unsigned char offset;

  if((fin=fopen(fich_ascii,"r"))==NULL)
  {
    fprintf(stderr,"ERROR(Pares_secuencias::Convierte_a_binario): No se puede abrir el fichero '%s'\n",fich_ascii);
    exit(-1);
  }

  if((fout=fopen(fich_binario,"w"))==NULL)
  {
    fprintf(stderr,"ERROR(Pares_secuencias::Convierte_a_binario): No se puede crear el fichero '%s'\n",fich_binario);
    exit(-1);
  }

  // Leemos el fichero de entrada y escribimos en el de salida
  ptr=cad;
  n = 0;
  while(fscanf(fin,"%253[^\n]\n",cad)==1)
  {
    if((ptr=strchr(cad,'|'))==NULL)
    {
      fprintf(stderr, "ERROR(Pares_secuencias::Convierte_a_binario): Falta el separador '|', o bien la l�nea es demasiado larga (m�s de 253 caracteres) - l�nea %d\n",n);
      exit(-1);
    }
    nbytes = strlen(cad)+1;
    *ptr = '\0';
    offset = (unsigned char)(nbytes + 1);
  
    fprintf(stderr, "A�adiendo el par '%s' - '%s'\n",cad,ptr+1);
 
    fwrite(&offset,sizeof(unsigned char),1,fout);
    fwrite(cad,sizeof(char),nbytes,fout);
  }

  // Escribe el final de la tabla
  offset = 0;
  fwrite(&offset,sizeof(unsigned char),1,fout);

  fclose(fin);
  fclose(fout);
}
  
