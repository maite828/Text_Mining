// Fichero:  nodos.h
// Clase:    NODO
// Autor:    DTT Enero 2000
// Utilidad: Es el fichero de cabecera para una clase NODO que �nicamente
//           exporta una serie de funciones para el manejo de los nodos de 
//           un �rbol que se emplea para almacenar diccionarios de forma 
//           compacta a la vez que permite una b�squeda r�pida.
//           Existen una serie de clases especiales de nodo que heredan de
//           esta clase y que permiten realizar el almacenamiento de la 
//           informaci�n de forma lo m�s adecuada posible a cada situaci�n
//           del �rbol.
// Hist�rico de modificaciones:
//           Enero 2000 - Creaci�n del fichero.
//


class NODO
{
  public:

  // Tipo de nodo
  enum Tipo
  {
    ERROR,
    DESCONOCIDO,
    LETRAS,
    LISTA_LETRAS,
    LISTA_CADENAS,
    LISTA_TERMINACIONES
  };

  // Direcci�n donde se encuentra el nodo
  void *Dir;

  // Contructores y destructores.
  NODO(void); // Objeto que no representa a ning�n nodo.
  NODO(void *Dir); // Objeto que representa a un nodo real.

  ~NODO(void);

  // Obtenci�n del tipo de nodo
  Tipo tipo(void);
  Tipo tipo(void *Dir);

  // Copia del nodo a otra posici�n de memoria
  void copia_a(void *NewPos);
  void copia_a(void *OldPos, void *NewPos);

  // Optimizaci�n del nodo
  void optimiza(void);
  void *optimiza(void *Dir);
};