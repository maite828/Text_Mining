// CLASE: Marcado
// UTILIDAD: Define una serie de funciones genéricas para facilitar el 
//           marcado de texto, por ejemplo define una función para doblar
//           los posibles caracteres de escape antes de introducir otro
//           tipo de marcas y otra función para eliminar el marcado y el
//           doblado de los caracteres de escape.
//           Es una clase de funciones estáticas.

#ifndef _Marcado_h_
#define _Marcado_h_

class Marcado 
{
  public:
  static char *EliminaMarcado(const char *);
  static char *MarcaURLs(const char *);
  static char *MarcaMails(const char *);
};

#endif // _Marcado_h_
