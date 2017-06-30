//////////////////////////////////////////////////////////
//
// Clase Pares_secuencias
//
// Mantiene una tabla de pares de secuencias de caracteres de longitud
// variable. 
// La tabla de pares de secuencias de caracteres admitirá dos 
// representaciones, una binaria, que se usa en memoria y en los 
// ficheros binarios que la contengan, y otra ascii que se usará 
// para crear la tabla de pares de secuencias de caracteres.
// La clase proporciona un método Convierte_a_binario que pasa de
// la representación en ASCII a la binaria. No hay conversión a la
// inversa.
//
// 29/2/2000 DTT - Creación de la clase
//
//////////////////////////////////////////////////////////

#ifndef Pares_secuencias_h
#define Pares_secuencias_h

class Pares_secuencias 
{
  private :

  // Posición en memoria donde está la lista de pares de secuencias de 
  // caracteres
  char *lista_bin;
  
  public:

  // Constructor
  // Inicializa la tabla a partir de la imagen binaria ya cargada en memoria
  Pares_secuencias(char *imagen_en_memoria);

  // Destructor
  // No hace nada
  ~Pares_secuencias(void);

  // Método para obtener la secuencia de caracteres asociada a otra en la 
  // tabla. Devuelve NULL si la secuencia de caracteres no está en la tabla.
  // Ojo! Se devuelve un puntero a la cadena almacenada en la tabla. NO HAY
  // QUE TOCAR DICHA CADENA!!!
  char *Asociado(char *cadena);

  // Método para obtener la secuencia de caracteres asociada a otra en la 
  // tabla. Devuelve NULL si la secuencia de caracteres no está en la tabla.
  // Ojo! Se devuelve un puntero a la cadena almacenada en la tabla. NO HAY
  // QUE TOCAR DICHA CADENA!!!
  // Este método utiliza una referencia a un puntero a carácter que permite
  // encontrar TODAS las secuencias de caracteres asociadas a una dada.
  // Para ello se debe usar un puntero sig_seek que se inicialice a NULL.
  // Cada vez que se llame a esta función devolverá la siguiente secuencia
  // asociada y actualizará el puntero, que servirá en la próxima llamada
  // para determinar la siguiente secuencia asociada. Cuando no haya más
  // secuencias asociadas devolverá NULL y en sig_seek devolverá un puntero tal
  // que sucesivas llamadas usando dicho puntero siempre hagan que se
  // devuelva un NULL.
  // El puntero sig_seek no debe utilizarse ni modificarse salvo para
  // inicializarlo a NULL.
  char *Asociado(char *cadena, char *&sig_seek);

  // Método adaptado especialmente a las necesidades del corrector ortográfico
  // La adaptación consiste en que devuelve la cadena traducida, termine o no
  // termine la cadena que se le pasa en el mismo instante en que termina la
  // cadena encontrada en la tabla. Otra especialización es que devuelve un
  // puntero al caracter siguiente al que ha coincidido con la cadena
  // encontrada en la tabla.
  // Método para obtener la secuencia de caracteres asociada a otra por la 
  // tabla
  // Devuelve NULL si no está
  char *Asociado(char *cadena, char **sig_pos);

  // Método adaptado especialmente a las necesidades del corrector ortográfico
  // La adaptación consiste en que devuelve la cadena traducida, termine o no
  // termine la cadena que se le pasa, en el mismo instante en que termina la
  // cadena encontrada en la tabla. Otra especialización es que devuelve un
  // puntero al caracter siguiente al que ha coincidido con la cadena
  // encontrada en la tabla.
  // Método para obtener TODAS las secuencias de caracteres asociadas a una
  // por la tabla.
  // Devuelve NULL si no está.
  //
  // Este método utiliza una referencia a un puntero a carácter que permite
  // encontrar TODAS las secuencias asociadas a una secuencia de caracteres
  // dada. Para ello se debe usar un puntero sig_seek que se inicialice a NULL
  // Cada vez que se llame a esta función devolverá la siguiente secuencia
  // asociada, la siguiente posición en la cadena, tras la secuencia a la que
  // corresponde la secuencia asociada (sig_pos) y actualizará el puntero
  // sig_seek, que servirá en la próxima llamada para determinar la siguiente
  // cadena asociada. Cuando no haya más cadenas asociadas devolverá NULL y
  // en sig_seek devolverá un puntero tal que sucesivas llamadas usando dicho
  // puntero hagan que se devuelva NULL.
  // El puntero sig_seek no debe utilizarse ni modificarse salvo para
  // inicializarlo a NULL.
  char *Asociado(char *cadena, char **sig_pos, char *&sig_seek);

  // Método adaptado especialmente a las necesidades del corrector ortográfico
  // La adaptación consiste en que devuelve la cadena traducida, termine o no
  // termine la cadena que se le pasa, en el mismo instante en que termina la
  // cadena encontrada en la tabla. Otra especialización es que devuelve un
  // puntero al caracter siguiente al que ha coincidido con la cadena
  // encontrada en la tabla.
  // Método para obtener TODAS las secuencias de caracteres asociadas a una
  // por la tabla.
  // Devuelve NULL si no está.
  //
  // Este método utiliza una referencia a un puntero a carácter que permite
  // encontrar TODAS las secuencias asociadas a una secuencia de caracteres
  // dada. Para ello se debe usar un puntero sig_seek que se inicialice a NULL
  // Cada vez que se llame a esta función devolverá la siguiente secuencia
  // asociada, la siguiente posición en la cadena, tras la secuencia a la que
  // corresponde la secuencia asociada (sig_pos) y actualizará el puntero
  // sig_seek, que servirá en la próxima llamada para determinar la siguiente
  // cadena asociada. Cuando no haya más cadenas asociadas devolverá NULL y
  // en sig_seek devolverá un puntero tal que sucesivas llamadas usando dicho
  // puntero hagan que se devuelva NULL.
  // El puntero sig_seek no debe utilizarse ni modificarse salvo para
  // inicializarlo a NULL.
  //
  // Este método permite obtener la asociación inversa, es decir, que cadena
  // se busca entre las cadenas asocidas. Si se encuentra se devuelve la cadena
  // original. Si no se devuelve NULL. El sig_pos y el sig_seek se utilizan
  // igual que en la otra.
  char *AsociadoInv(char *cadena, char **sig_pos,
                                      char *&sig_seek);

  // Método para transformar la representación en ASCII de la tabla de
  // cadenas de caracteres (un fichero en el que una cadena y su cadena
  // asociada se recogen en una línea y separadas por el carácter | los 
  // blancos, salvo el retorno de carro, se incluyen también en las cadenas
  // de la tabla. La primera cadena es la cadena a buscar, la segunda es 
  // la cadena asociada) en la representación interna, que consiste en varias
  // estructuras con la forma:
  //  UNSIGNED CHAR *sig (offset al siguiente par)
  //  char *cadena_busqueda (ASCIIZ)
  //  char *cadena_asociada (ASCIIZ) 
  // Esta función se supone que es sólo de desarrollo, por lo que puede dar
  // errores por pantalla.
  // Además, es estática: No necesita un objeto para funcionar.
  static void Convierte_a_binario(const char *fich_ascii, 
                                  const char *fich_binario);

};

#endif // Pares_secuencias_h
 
