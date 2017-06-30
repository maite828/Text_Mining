// Fichero : hash.h
// Fecha : 25/11/99
// Descripcion : Funciones hash.

#ifndef hash_h
#define hash_h

// Clase : Hash
// Descripcion : Clase que implementa funciones hash.
class Hash
{
  private :
  unsigned short parametro;

  public :
  // Constructores y destructores.
  Hash(unsigned short p)
  : parametro(p)
  {
  }

  ~Hash(void)
  {
  }

  // Operadores.
  unsigned short operator()(const char *cad)
  {
    int i = 0;
    unsigned long suma = 0;

    while (cad[i] != '\0')
    {
      suma += cad[i]<<i;
      i++;
    }
    return ((unsigned short)(suma % parametro));
  }

  // Tamanno del hash.
  unsigned int Tamanno(void)
  {
    return parametro;
  }
};

#endif hash_h

