// Fichero : detecta.cc
// Descripcion : Deteccion de idiomas.
// Fecha : 28/11/99
// Version : 1.0

#include <strstream>
#include <fstream>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "libs/det_id/dicc.h"
#include "libs/det_id/mod_idioma.h"

typedef int Idioma;
typedef double Confianza;

// Numero de diccionarios activos.
//int nDicc = 6;
#define nDicc 2
Diccionario dicc[] = { "castellano.bin", "ingles.bin"};
//Diccionario dicc[] = { "castellano.bin", "cas1.bin", "cas2.bin", "ingles.bin", "ing1.bin", "ing2.bin"};

int es_permitido(const char c)
{
  static const char especiales_cas[] = "·¡È…ÌÕÛ”˙⁄¸‹Ò—";

  if ( ( ('a' <= c) && (c <= 'z') ) ||
       ( ('A' <= c) && (c <= 'Z') ) ||
       (strchr(especiales_cas, c) != 0) )
  {
    return (0 == 0);
  }
  else
  {
    return (0 == 1);
  }

  // return !ispunct(c);
}

void Filtra(const char *pal, char *palf)
{
  int is,id;

  is = id = 0;
  while (pal[is] != '\0')
  {
    if (es_permitido(pal[is]))
    {
      palf[id] = pal[is];
      id++;
    }
    is++;
  }
  palf[id] = '\0';
}

Idioma Detecta(const char *texto, Confianza &c)
{
  int i;
  unsigned short apariciones[nDicc];
  int nPal;

  for (i = 0; i < nDicc; i++)
  {
    apariciones[i] = 0;
  }

  // Comprobar todos los idiomas.
  for (i = 0; i < nDicc; i++)
  {
    // Palabra.
    static char pal[60];
    static char palf[60];
    // Stream auxiliar.
    istrstream stexto(texto);

    nPal = 0;

    pal[0] = '\0';
    stexto >> pal;
    while (pal[0] != '\0')
    {
      Filtra(pal, palf);
      if (palf[0] != '\0')
      {
      nPal++;
      if (dicc[i].Busca(palf))
        apariciones[i]++;
      }

      pal[0] = '\0';
      stexto >> pal;
    }
  }

  // Expresemonos.
  cout << "# palabras " << nPal << endl;
  for (i = 0; i < nDicc; i++)
    cout << "Apariciones[" << i  << "] = " << apariciones[i] << endl;

  getchar();

  // Calculo de la confianza.
  // Encontrar el los dos mayores y sus indices.
  unsigned short pri = 0, seg = 0;
  int ipri = -1, iseg = -1;

  switch (nDicc)
  {
    case 0 :
      ipri = iseg = -1;
      pri = seg = 0;
      break;

    case 1 :
      ipri = 0;
      iseg = -1;
      pri = apariciones[0];
      seg = 0;
      break;

    default :
      for (i = 0; i < nDicc; i++)
        if (pri < apariciones[i])
        {
          seg = pri;
          pri = apariciones[i];
          iseg = ipri;
          ipri = i;
        }
        else
        {
          if (seg < apariciones[i])
          {
            seg = apariciones[i];
            iseg = i;
          }
        }
      break;
  }

  // Como una diferencia relativa entre los 2 primeros.
  if (pri == 0)
    c = 0;
  else
    c = (Confianza)(pri - seg)/pri;

  // Devolver el idioma del mejor.
  return ipri;
}

void Uso(void)
{
  cout << "detecta <fichero_frases>" << endl;
}

int main(int narg, char **args)
{
  if (narg < 2)
  {
    Uso();
    exit(2);
  }

  char frase[250];
  char aux;
  ifstream f(args[1]);
  Idioma idioma;
  Confianza conf;
  float prob;
  int i;

  if (!f)
    exit(1);

  // bigramas.
  char *nmods[nDicc] = { "mod_esp", "mod_cat" };
  long *mods[nDicc];
  for (i = 0; i < nDicc; i++)
   if ( (mods[i] = carga_mod_idioma(nmods[i])) == 0)
     exit(2);

  f.get(frase, 250);
  while (!f.eof())
  {
    cout << "FRASE   " << frase << endl;

    // diccionario.
    idioma = Detecta(frase, conf);
    cout << "Idioma " << idioma << "  confianza " << conf << endl;

    // bigramas.
    for (i = 0; i < nDicc; i++)
    {
      prob = logprob_pal_idioma(obten_logprob_mod_idioma(mods[i]),
                           frase);
      cout << "probabilidad bigrama[" << i << "] " << prob << endl;
    } 
    
    f.ignore();
    f.get(frase, 250);
  }
}

