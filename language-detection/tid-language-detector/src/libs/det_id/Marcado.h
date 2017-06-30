// CLASE: Marcado
// UTILIDAD: Define una serie de funciones gen�ricas para facilitar el 
//           marcado de texto, por ejemplo define una funci�n para doblar
//           los posibles caracteres de escape antes de introducir otro
//           tipo de marcas y otra funci�n para eliminar el marcado y el
//           doblado de los caracteres de escape.
//           Es una clase de funciones est�ticas.

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
