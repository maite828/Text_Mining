//////////////////////////////////////////////////////////
//
// Clase Pares_secuencias
//
// Mantiene una tabla de pares de secuencias de caracteres de longitud
// variable. 
// La tabla de pares de secuencias de caracteres admitir� dos 
// representaciones, una binaria, que se usa en memoria y en los 
// ficheros binarios que la contengan, y otra ascii que se usar� 
// para crear la tabla de pares de secuencias de caracteres.
// La clase proporciona un m�todo Convierte_a_binario que pasa de
// la representaci�n en ASCII a la binaria. No hay conversi�n a la
// inversa.
//
// 29/2/2000 DTT - Creaci�n de la clase
//
//////////////////////////////////////////////////////////

#ifndef Pares_secuencias_h
#define Pares_secuencias_h

class Pares_secuencias 
{
  private :

  // Posici�n en memoria donde est� la lista de pares de secuencias de 
  // caracteres
  char *lista_bin;
  
  public:

  // Constructor
  // Inicializa la tabla a partir de la imagen binaria ya cargada en memoria
  Pares_secuencias(char *imagen_en_memoria);

  // Destructor
  // No hace nada
  ~Pares_secuencias(void);

  // M�todo para obtener la secuencia de caracteres asociada a otra en la 
  // tabla. Devuelve NULL si la secuencia de caracteres no est� en la tabla.
  // Ojo! Se devuelve un puntero a la cadena almacenada en la tabla. NO HAY
  // QUE TOCAR DICHA CADENA!!!
  char *Asociado(char *cadena);

  // M�todo para obtener la secuencia de caracteres asociada a otra en la 
  // tabla. Devuelve NULL si la secuencia de caracteres no est� en la tabla.
  // Ojo! Se devuelve un puntero a la cadena almacenada en la tabla. NO HAY
  // QUE TOCAR DICHA CADENA!!!
  // Este m�todo utiliza una referencia a un puntero a car�cter que permite
  // encontrar TODAS las secuencias de caracteres asociadas a una dada.
  // Para ello se debe usar un puntero sig_seek que se inicialice a NULL.
  // Cada vez que se llame a esta funci�n devolver� la siguiente secuencia
  // asociada y actualizar� el puntero, que servir� en la pr�xima llamada
  // para determinar la siguiente secuencia asociada. Cuando no haya m�s
  // secuencias asociadas devolver� NULL y en sig_seek devolver� un puntero tal
  // que sucesivas llamadas usando dicho puntero siempre hagan que se
  // devuelva un NULL.
  // El puntero sig_seek no debe utilizarse ni modificarse salvo para
  // inicializarlo a NULL.
  char *Asociado(char *cadena, char *&sig_seek);

  // M�todo adaptado especialmente a las necesidades del corrector ortogr�fico
  // La adaptaci�n consiste en que devuelve la cadena traducida, termine o no
  // termine la cadena que se le pasa en el mismo instante en que termina la
  // cadena encontrada en la tabla. Otra especializaci�n es que devuelve un
  // puntero al caracter siguiente al que ha coincidido con la cadena
  // encontrada en la tabla.
  // M�todo para obtener la secuencia de caracteres asociada a otra por la 
  // tabla
  // Devuelve NULL si no est�
  char *Asociado(char *cadena, char **sig_pos);

  // M�todo adaptado especialmente a las necesidades del corrector ortogr�fico
  // La adaptaci�n consiste en que devuelve la cadena traducida, termine o no
  // termine la cadena que se le pasa, en el mismo instante en que termina la
  // cadena encontrada en la tabla. Otra especializaci�n es que devuelve un
  // puntero al caracter siguiente al que ha coincidido con la cadena
  // encontrada en la tabla.
  // M�todo para obtener TODAS las secuencias de caracteres asociadas a una
  // por la tabla.
  // Devuelve NULL si no est�.
  //
  // Este m�todo utiliza una referencia a un puntero a car�cter que permite
  // encontrar TODAS las secuencias asociadas a una secuencia de caracteres
  // dada. Para ello se debe usar un puntero sig_seek que se inicialice a NULL
  // Cada vez que se llame a esta funci�n devolver� la siguiente secuencia
  // asociada, la siguiente posici�n en la cadena, tras la secuencia a la que
  // corresponde la secuencia asociada (sig_pos) y actualizar� el puntero
  // sig_seek, que servir� en la pr�xima llamada para determinar la siguiente
  // cadena asociada. Cuando no haya m�s cadenas asociadas devolver� NULL y
  // en sig_seek devolver� un puntero tal que sucesivas llamadas usando dicho
  // puntero hagan que se devuelva NULL.
  // El puntero sig_seek no debe utilizarse ni modificarse salvo para
  // inicializarlo a NULL.
  char *Asociado(char *cadena, char **sig_pos, char *&sig_seek);

  // M�todo adaptado especialmente a las necesidades del corrector ortogr�fico
  // La adaptaci�n consiste en que devuelve la cadena traducida, termine o no
  // termine la cadena que se le pasa, en el mismo instante en que termina la
  // cadena encontrada en la tabla. Otra especializaci�n es que devuelve un
  // puntero al caracter siguiente al que ha coincidido con la cadena
  // encontrada en la tabla.
  // M�todo para obtener TODAS las secuencias de caracteres asociadas a una
  // por la tabla.
  // Devuelve NULL si no est�.
  //
  // Este m�todo utiliza una referencia a un puntero a car�cter que permite
  // encontrar TODAS las secuencias asociadas a una secuencia de caracteres
  // dada. Para ello se debe usar un puntero sig_seek que se inicialice a NULL
  // Cada vez que se llame a esta funci�n devolver� la siguiente secuencia
  // asociada, la siguiente posici�n en la cadena, tras la secuencia a la que
  // corresponde la secuencia asociada (sig_pos) y actualizar� el puntero
  // sig_seek, que servir� en la pr�xima llamada para determinar la siguiente
  // cadena asociada. Cuando no haya m�s cadenas asociadas devolver� NULL y
  // en sig_seek devolver� un puntero tal que sucesivas llamadas usando dicho
  // puntero hagan que se devuelva NULL.
  // El puntero sig_seek no debe utilizarse ni modificarse salvo para
  // inicializarlo a NULL.
  //
  // Este m�todo permite obtener la asociaci�n inversa, es decir, que cadena
  // se busca entre las cadenas asocidas. Si se encuentra se devuelve la cadena
  // original. Si no se devuelve NULL. El sig_pos y el sig_seek se utilizan
  // igual que en la otra.
  char *AsociadoInv(char *cadena, char **sig_pos,
                                      char *&sig_seek);

  // M�todo para transformar la representaci�n en ASCII de la tabla de
  // cadenas de caracteres (un fichero en el que una cadena y su cadena
  // asociada se recogen en una l�nea y separadas por el car�cter | los 
  // blancos, salvo el retorno de carro, se incluyen tambi�n en las cadenas
  // de la tabla. La primera cadena es la cadena a buscar, la segunda es 
  // la cadena asociada) en la representaci�n interna, que consiste en varias
  // estructuras con la forma:
  //  UNSIGNED CHAR *sig (offset al siguiente par)
  //  char *cadena_busqueda (ASCIIZ)
  //  char *cadena_asociada (ASCIIZ) 
  // Esta funci�n se supone que es s�lo de desarrollo, por lo que puede dar
  // errores por pantalla.
  // Adem�s, es est�tica: No necesita un objeto para funcionar.
  static void Convierte_a_binario(const char *fich_ascii, 
                                  const char *fich_binario);

};

#endif // Pares_secuencias_h
 
