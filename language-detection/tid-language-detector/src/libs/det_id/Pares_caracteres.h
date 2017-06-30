//////////////////////////////////////////////////////////
//
// Clase Pares_caracteres
//
// Mantiene una tabla de pares de caracteres. 
// La tabla de pares de caracteres admitirá dos representaciones,
// una binaria, que se usa en memoria y en los ficheros binarios
// que la contengan, y otra ascii que se usará para crear la tabla
// de pares de caracteres.
// La clase proporciona un método Convierte_a_binario que pasa de
// la representación en ASCII a la binaria. No hay conversión a la
// inversa.
//
// 29/2/2000 DTT - Creación de la clase
//
//////////////////////////////////////////////////////////

#ifndef Pares_caracteres_h
#define Pares_caracteres_h

class Pares_caracteres 
{
  private :

  // Posición en memoria donde está la lista de pares de caracteres
  char *lista_bin;
  
  public:

  // Constructor
  // Inicializa la tabla a partir de la imagen binaria ya cargada en memoria
  Pares_caracteres(char *imagen_en_memoria);

  // Destructor
  // No hace nada
  ~Pares_caracteres(void);

  // Método para obtener el carácter asociado a otro por la tabla
  // Devuelve 0 si no está (Luego no se puede usar el 0 como caracter asociado
  // a otro)
  char Asociado(char ch);

  // Método para obtener el carácter asociado a otro por la tabla
  // Devuelve 0 si no está (Luego no se puede usar el 0 como caracter asociado
  // a otro)
  // Este método utiliza una referencia a un puntero a carácter que permite
  // encontrar TODOS los caracteres asociados a uno dado.
  // Para ello se debe usar un puntero sig_seek que se inicialice a NULL.
  // Cada vez que se llame a esta función devolverá el siguiente carácter
  // asociado y actualizará el puntero, que servirá en la próxima llamada
  // para determinar el siguiente carácter asociado. Cuando no haya más
  // caracteres asociados devolverá 0 y en sig_seek devolverá un puntero 
  // tal que sucesivas llamadas usando dicho puntero siempre hagan que se 
  // devuelva un 0.
  // El puntero sig_seek no debe utilizarse ni modificarse salvo para
  // inicializarlo a NULL.
  char Asociado(char ch, char *&sig_seek);


  // Método para transformar la representación en ASCII de la tabla de
  // caracteres (un fichero con líneas de dos caracteres en las que el primer
  // caracter es el caracter a buscar y el segundo es el caracter asociado)
  // en la representación interna, que consiste en un primer unsigned char
  // que indica el número de caracteres en la tabla, seguido de los pares de
  // caracteres en sí.
  // Esta función se supone que es sólo de desarrollo, por lo que puede dar
  // errores por pantalla.
  // Además, es estática: No necesita un objeto para funcionar.
  static void Convierte_a_binario(const char *fich_ascii, 
                                  const char *fich_binario);

};

#endif // Pares_caracteres_h
 
