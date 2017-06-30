// Fichero:  nodos.h
// Clase:    NODO
// Autor:    DTT Enero 2000
// Utilidad: Es el fichero de cabecera para una clase NODO que únicamente
//           exporta una serie de funciones para el manejo de los nodos de 
//           un árbol que se emplea para almacenar diccionarios de forma 
//           compacta a la vez que permite una búsqueda rápida.
//           Existen una serie de clases especiales de nodo que heredan de
//           esta clase y que permiten realizar el almacenamiento de la 
//           información de forma lo más adecuada posible a cada situación
//           del árbol.
// Histórico de modificaciones:
//           Enero 2000 - Creación del fichero.
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

  // Dirección donde se encuentra el nodo
  void *Dir;

  // Contructores y destructores.
  NODO(void); // Objeto que no representa a ningún nodo.
  NODO(void *Dir); // Objeto que representa a un nodo real.

  ~NODO(void);

  // Obtención del tipo de nodo
  Tipo tipo(void);
  Tipo tipo(void *Dir);

  // Copia del nodo a otra posición de memoria
  void copia_a(void *NewPos);
  void copia_a(void *OldPos, void *NewPos);

  // Optimización del nodo
  void optimiza(void);
  void *optimiza(void *Dir);
};