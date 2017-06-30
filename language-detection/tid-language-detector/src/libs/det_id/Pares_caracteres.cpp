//////////////////////////////////////////////////////////
//
// M�todos de la Clase Pares_caracteres
//
// Mantiene una tabla de pares de caracteres. 
// La tabla de pares de caracteres admitir� dos representaciones,
// una binaria, que se usa en memoria y en los ficheros binarios
// que la contengan, y otra ascii que se usar� para crear la tabla
// de pares de caracteres.
// La clase proporciona un m�todo Convierte_a_binario que pasa de
// la representaci�n en ASCII a la binaria. No hay conversi�n a la
// inversa.
//
// 29/2/2000 DTT - Creaci�n de la clase
//
//////////////////////////////////////////////////////////

#ifndef WIN32
#include <sys/mman.h>
#else 
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Pares_caracteres.h"

// Constructor
// Inicializa la tabla a partir de la imagen binaria ya cargada en memoria
Pares_caracteres::Pares_caracteres(char *imagen_en_memoria)
{
  lista_bin = imagen_en_memoria;
}

// Destructor
// No hace nada
Pares_caracteres::~Pares_caracteres(void)
{
}

// M�todo para obtener el car�cter asociado a otro por la tabla
// Devuelve 0 si no est� (Luego no se puede usar el 0 como caracter asociado
// a otro)
char Pares_caracteres::Asociado(char ch)
{
  int i;
  char *ptr=lista_bin+1;
  // printf("LONG_LISTA: %d\n",(unsigned char) *lista_bin);
  for(i=0;i<(unsigned char) *lista_bin;i++)
  {
    if(ch==*ptr) return (*(ptr+1));
    // printf("Par no v�lido: '%c' - '%c'\n",*ptr,*(ptr+1));
    ptr++;
    ptr++;
  }
  return(0);
}

// M�todo para obtener el car�cter asociado a otro por la tabla
// Devuelve 0 si no est� (Luego no se puede usar el 0 como caracter asociado
// a otro)
// Este m�todo utiliza una referencia a un puntero a car�cter que permite
// encontrar TODOS los caracteres asociados a uno dado. 
// Para ello se debe usar un puntero sig_seek que se inicialice a NULL. 
// Cada vez que se llame a esta funci�n devolver� el siguiente car�cter
// asociado y actualizar� el puntero, que servir� en la pr�xima llamada
// para determinar el siguiente car�cter asociado. Cuando no haya m�s
// caracteres asociados devolver� 0 y en sig_seek devolver� un puntero tal que
// sucesivas llamadas usando dicho puntero siempre hagan que se devuelva
// un 0. 
// El puntero sig_seek no debe utilizarse ni modificarse salvo para 
// inicializarlo a NULL.
char Pares_caracteres::Asociado(char ch, char *&sig_seek)
{
  int i;
  char *ptr;

  if(sig_seek!=NULL)
    ptr=sig_seek;
  else
    ptr=lista_bin+1;
  // printf("LONG_LISTA: %d\n",(unsigned char) *lista_bin);
  for(i=(ptr-lista_bin-1)/2;i<(unsigned char) *lista_bin;i++)
  {
    if(ch==*ptr)
    {
      sig_seek = ptr+2;
      return (*(ptr+1));
    }
    // printf("Par no v�lido: '%c' - '%c'\n",*ptr,*(ptr+1));
    ptr++;
    ptr++;
  }
  sig_seek = ptr;
  return(0);
}

// M�todo para transformar la representaci�n en ASCII de la tabla de
// caracteres (un fichero con l�neas de dos caracteres en las que el primer
// caracter es el caracter a buscar y el segundo es el caracter asociado)
// en la representaci�n interna, que consiste en un primer unsigned char
// que indica el n�mero de caracteres en la tabla, seguido de los pares de
// caracteres en s�.
// Esta funci�n se supone que es s�lo de desarrollo, por lo que puede dar
// errores por pantalla.
// Adem�s, es est�tica: No necesita un objeto para funcionar.
void Pares_caracteres::Convierte_a_binario(const char *fich_ascii, 
                                           const char *fich_binario)
{
  FILE *fin, *fout;
  char ch[512];
  char *ptr;
  unsigned int n=0;
  unsigned char nch;

  if((fin=fopen(fich_ascii,"r"))==NULL)
  {
    fprintf(stderr,"ERROR(Pares_caracteres::Convierte_a_binario): No se puede abrir el fichero '%s'\n",fich_ascii);
    exit(-1);
  }

  if((fout=fopen(fich_binario,"w"))==NULL)
  {
    fprintf(stderr,"ERROR(Pares_caracteres::Convierte_a_binario): No se puede crear el fichero '%s'\n",fich_binario);
    exit(-1);
  }

  // Leemos el fichero de entrada
  ptr=ch;
  while(fscanf(fin,"%c%c\n",ptr,ptr+1)==2)
  {
    fprintf(stderr, "A�adiendo el par '%c' - '%c'\n",*ptr,*(ptr+1));
    ptr++;
    ptr++;
    n++;
    if(n>255)
    {
      fprintf(stderr,"ERROR(Pares_caracteres::Convierte_a_binario): La tabla de caracteres no puede tener m�s de 255 entradas.\n");
      exit(-1);
    }
  }

  // Escribimos el fichero de salida
 
  nch = (unsigned char) n; 
  fwrite(&nch,sizeof(unsigned char),1,fout);
  fwrite(ch,sizeof(char),2*n,fout);

  fclose(fin);
  fclose(fout);
}
  
