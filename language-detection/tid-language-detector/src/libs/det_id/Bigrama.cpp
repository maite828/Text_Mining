
#include "Bigrama.h"

// Contructor de la clase Bigrama
#ifdef WIN32
Bigrama::Bigrama(LPVOID inicio_big)
#else
Bigrama::Bigrama(void * inicio_big)
#endif
{
  dir_big=inicio_big;
}

// Destructor de la clase bigrama
Bigrama::~Bigrama()
{
}



