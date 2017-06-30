// METODO: static void Convierte(ifstream &fbig, ofstream &fbinario)
// DESCRIPCION: Toma un stream de entrada fbig que contiene un fichero del
//              modelo de bigramas en ASCII, y como resultado escribe en
//              el fichero binario fbinario el modelo de bigramas en binario.
//              Esta función se usa por Det_id_interno::Gen_mod_bin para generar
//              el modelo de lenguaje binario.
// VALOR DEVUELTO: 0 si ha ido todo bien, -1 en caso contrario.

#include <fstream>

#include "Bigrama.h"

int Bigrama::Convierte(std::ifstream &fbig, std::ofstream &fbinario)
{
  long mod_idioma[NUMCOD_BIG*NUMCOD_BIG];
  float logprob_mod_idioma[NUMCOD_BIG*NUMCOD_BIG];
  float *ptr = logprob_mod_idioma;
  int i;

  // Carga el modelo de bigramas del fichero
  if(carga_mod_idioma(fbig,mod_idioma)==-1)
    return -1;

  // Lo pasa a logprob
  obten_logprob_mod_idioma(mod_idioma,logprob_mod_idioma);

  // Lo escribe en el fbinario
  for(i=0;i<NUMCOD_BIG*NUMCOD_BIG;i++,ptr++)
  {
    fbinario.write( (char *) ptr, sizeof(float) );
    if(!fbinario.good())
      return -1;
  }

  // Ya está
  return 0;
}


