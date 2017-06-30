//////////////////////////////////////////////////////////
//
// Clase Pares_caracteres
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

#ifndef Pares_caracteres_h
#define Pares_caracteres_h

class Pares_caracteres 
{
  private :

  // Posici�n en memoria donde est� la lista de pares de caracteres
  char *lista_bin;
  
  public:

  // Constructor
  // Inicializa la tabla a partir de la imagen binaria ya cargada en memoria
  Pares_caracteres(char *imagen_en_memoria);

  // Destructor
  // No hace nada
  ~Pares_caracteres(void);

  // M�todo para obtener el car�cter asociado a otro por la tabla
  // Devuelve 0 si no est� (Luego no se puede usar el 0 como caracter asociado
  // a otro)
  char Asociado(char ch);

  // M�todo para obtener el car�cter asociado a otro por la tabla
  // Devuelve 0 si no est� (Luego no se puede usar el 0 como caracter asociado
  // a otro)
  // Este m�todo utiliza una referencia a un puntero a car�cter que permite
  // encontrar TODOS los caracteres asociados a uno dado.
  // Para ello se debe usar un puntero sig_seek que se inicialice a NULL.
  // Cada vez que se llame a esta funci�n devolver� el siguiente car�cter
  // asociado y actualizar� el puntero, que servir� en la pr�xima llamada
  // para determinar el siguiente car�cter asociado. Cuando no haya m�s
  // caracteres asociados devolver� 0 y en sig_seek devolver� un puntero 
  // tal que sucesivas llamadas usando dicho puntero siempre hagan que se 
  // devuelva un 0.
  // El puntero sig_seek no debe utilizarse ni modificarse salvo para
  // inicializarlo a NULL.
  char Asociado(char ch, char *&sig_seek);


  // M�todo para transformar la representaci�n en ASCII de la tabla de
  // caracteres (un fichero con l�neas de dos caracteres en las que el primer
  // caracter es el caracter a buscar y el segundo es el caracter asociado)
  // en la representaci�n interna, que consiste en un primer unsigned char
  // que indica el n�mero de caracteres en la tabla, seguido de los pares de
  // caracteres en s�.
  // Esta funci�n se supone que es s�lo de desarrollo, por lo que puede dar
  // errores por pantalla.
  // Adem�s, es est�tica: No necesita un objeto para funcionar.
  static void Convierte_a_binario(const char *fich_ascii, 
                                  const char *fich_binario);

};

#endif // Pares_caracteres_h
 
