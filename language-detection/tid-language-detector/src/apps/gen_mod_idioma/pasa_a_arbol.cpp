// Fichero     pasa_a_arbol.c
// Programa    pasa_a_arbol
// Autor:      DTT, Enero 2000
// Utilidad:   Dado un fichero con un diccionario en forma de lista de
//             palabras, ordenada de forma creciente, separadas entre sí por
//             un retorno de carro, y pudiendo contener espacios (que se 
//             considerarán formando parte de la palabra!!! - Mucho ojito),
//             y tal que no contenga ningún carácter que no satisfaga la
//             condición TRATA_CAR::Letra (si no es así el programa avisa),
//             genera una versión binaria del diccionario en forma de árbol,
//             que presumiblemente ocupará menos espacio y será más sencilla
//             de acceder.


#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/mman.h>

#include "libs/det_id/trata_car.h"
#include "libs/det_id/Bigrama.h"


// Función principal.
void main(int argc, char *argv[])
{
  FILE *fout;
  int desfichin;
  unsigned_long tam_diccio;
  char *inicio_diccio, *posicion, *final_diccio;
  

  if(argc!=3)
  {
    fprintf(stderr,"USO: pasa_a_arbol lista arbol\n");
    fprintf(stderr,"donde lista es una lista ordenada de palabras\n");
    fprintf(stderr,"y arbol es el fichero que creará conteniendo el árbol.\n");
    exit(-1);
  }

  if((desfichin=open(argv[1],O_RDONLY))==-1)
  {
    fprintf(stderr,"No se puede abrir el fichero '%s'.\n",argv[1]);
    exit(-1);
  }
  tam_diccio = lseek(desfichin, 0, SEEK_END);
  inicio_diccio = mmap(0,tam_diccio,PROT_READ, MAP_SHARED, desfichin, 0);
  if(inicio_diccio = MAP_FAILED)
  {
    fprintf(stderr,"No se pudo realizar el mapeo en memoria.\n");
    exit(-1);
  }
  final_diccio = inicio_diccio + tam_diccio;

  close(desfichin);

  // Esta función se encarga de realizar una comprobación del diccionario
  // a fin de que luego sea más sencillo el tratamiento del diccionario.
  comprueba_diccio(inicio_diccio,final_diccio,0);

  // Esta función se encarga de realizar la codificación del diccionario,
  // pasando de los códigos ASCII a códigos numéricos entre 0 y NUMCOD_BIG
  codifica_diccio(inicio_diccio,final_diccio);

  if((fout=fopen(argv[2],"wb"))==NULL)
  {
    fprintf(stderr,"No se puede crear el fichero '%s'.\n",argv[2]);
    exit(-1);
  }

  // Esta función trata el nivel 1 y se llama recursivamente para tratar
  // los niveles subsiguientes hasta transformar totalmente el fichero lista
  // en un fichero binario que contiene un árbol.
  // El puntero posicion le indica que zona del diccionario debe manejar. 
  // La función devuelve la siguiente zona del diccionario a tratar (con el 
  // mismo nivel), o NULL si no hay más zonas a tratar en este nivel.
  // Cada zona a tratar en un nivel se corresponderá con un nodo de ese nivel,
  // por lo tanto el puntero devuelto es el puntero a la zona correspondiente
  // al siguiente nodo dentro de este mismo nivel.
  // Será necesaria una llamada por cada nodo a situar en cada nivel. En el
  // caso del nivel 1 sólo habrá un nodo, el nodo raíz del árbol, por lo que
  // sólo es necesaria una llamada.

  posicion = trata_nivel(1,inicio_diccio,final_diccio,fout)
  if(posicion != NULL) 
  {
    fprintf(stderr,"Algo ha ido mal con la primera llamada a trata_nivel!\n");
    exit(-1);
  }

  // Una vez que ha retornado esta función todo está hecho. Solo queda
  // cerrar los ficheros y salir.

  fclose(fout);

}

// Función:  comprueba_diccio 
// Utilidad: Hace un repaso de todo el diccionario comprobando sin está
//           correcto para su uso con este programa. En concreto comprueba
//           que:
//           Las palabras estén ordenadas en orden estrictamente creciente
//           Las palabras estén separadas por retornos de carro, por tanto una
//           en cada línea. Si no se cumple esto o hay una línea vacía se da 
//           un error.
//           Los únicos caracteres posibles en el fichero fin sean los que 
//           satisfagan la condicion TRATA_CAR::Letra y el hash (#). La
//           aparición de cualquier otro carácter dará lugar a un error. 
//           Los hashes (#) en principio se consideran como cualquier otro
//           carácter y pueden formar parte de las palabras. Así por ejemplo
//           la línea "tex#mex" formará una única palabra en el diccionario.
//           La utilidad de esto es introducir en el diccionario palabras
//           que contengan símbolos no incluidos en TRATA_CAR::Letra. El 
//           hash (#) actúa como un comodín que va a representar a cualquier
//           carácter que no sea considerado una letra por TRATA_CAR::Letra.
//           Así la entrada anterior podra representar Tex-Mex, Tex@Mex, 
//           Tex Mex, etc.
//           La aceptación de los hashes se puede eliminar si la llamada se
//           hace poniendo comodines a 0. En este caso la aparición de un 
//           hash da lugar a un error.
//           El tamaño máximo de una palabra del diccionario viene determinado
//           por la constante MAXLONGPAL_DIC. Si se supera este valor se da
//           un error.
//           La función sólo retorna si todo estaba correcto, en cualquier
//           otro caso sale del programa.

#define MAXLONGPAL_DIC 20

void comprueba_diccio(char *inicio_diccio, char *final_diccio,int comodines)
{
  char *pal_ant, *fin_pal_ant;
  char *pal, *fin_pal;
  char *ptr;
  unsigned long linea = 1;

  // Inicialización
  pal_ant = inicio_diccio;
  fin_pal_ant = strchr(pal_ant,'\n');
  if(fin_pal_ant==NULL)
  {
    fprintf(stderr,"ERROR(línea %lu): No encuentro el final de línea!!!\n",linea);
    exit(-1);
  }

  // Comprobación de línea excediendo el tamaño del fichero
  if(fin_pal_ant >= fin_diccio)
  {
    fprintf(stderr,"ERROR(línea %lu): Línea no terminada en fichero.\n",
            linea);
    exit(-1);
  }

  *fin_pal_ant = '\0';

  // Comprobación de línea vacía
  if(fin_pal_ant-pal_ant == 1)
  {
    frpintf(stderr,"ERROR(línea %lu): Línea vacía.\n",linea);
    exit(-1);
  }

  // Comprobación de tamaño
  if(fin_pal_ant-pal_ant > MAXLONPAL_DIC)
  {
    fprintf(stderr,"ERROR(línea %lu): Palabra demasiado larga.\n",linea);
    exit(-1);
  }

  // Comprobación de caracteres
  for(ptr=pal_ant;ptr<fin_pal_ant;ptr++)
  {
    if(!TRATA_CAR::Letra(*ptr))
    {
      if(!comodines)||(*ptr!='#'))
      {
	fprintf(stderr,"ERROR(línea %lu -> '%s'): Carácter '%c' no válido.\n",
		linea, pal_ant, *ptr);
	exit(-1);
      }
    }
  }

  linea++;
  pal = fin_pal_ant+1;
  while(pal<final_diccio)
  {
    fin_pal = strchr(pal,'\n');
    if(fin_pal==NULL)
    {
      printf("ERROR(línea %lu): No encuentro el final de línea!!!\n",linea);
      exit(-1);
    }

    // Comprobación de línea excediendo el tamaño del fichero
    if(fin_pal >= fin_diccio)
    {
      fprintf(stderr,"ERROR(línea %lu): Línea no terminada en fichero.\n", 
	      linea);
      exit(-1);
    }

    *fin_pal = '\0';

    // Comprobación de línea vacía
    if(fin_pal-pal == 1)
    {
      frpintf(stderr,"ERROR(línea %lu): Línea vacía.\n",linea);
      exit(-1);
    }
  
    // Comprobación de tamaño
    if(fin_pal-pal > MAXLONPAL_DIC)
    {
      fprintf(stderr,"ERROR(línea %lu): Palabra demasiado larga.\n",linea);
      exit(-1);
    }
  
    // Comprobación de caracteres
    for(ptr=pal;ptr<fin_pal;ptr++)
    {
      if(!TRATA_CAR::Letra(*ptr))
      {
        if(!comodines)||(*ptr!='#'))
        {
	  fprintf(stderr,"ERROR(línea %lu -> '%s'): Carácter '%c' no válido.\n",
		  linea, pal, *ptr);
	  exit(-1);
        }
      }
    }

    // Comprobación de orden
    if((ord=strcmp(pal,pal_ant))==0)
    {
      fprintf(stderr,"ERROR(línea %lu -> '%s'): Línea repetida.\n",linea,pal);
      exit(-1);
    }
    else if(ord < 0)
    {
      fprintf(stderr,"ERROR(línea %lu -> '%s'): Orden incorrecto.\n",linea,pal);
      exit(-1);
    }

    // Todas las comprobaciones OK. Pasamos a la siguiente línea

    // Dejamos esto como estaba;
    *fin_pal_ant = '\n';

    linea++;

    // Obtenemos la siguiente palabra
    pal = fin_pal + 1;
  }
}


// Función: codifica_diccio
// Descripción: Toma el diccionario y lo recorre carácter a carácter
//              transformando cada carácter en el código correspondiente
//              usando para ello la función Bigrama::codifica_letra. 
//              La única excepción es el carácter '\n', que se usa en 
//              el diccionario para separar palabras entre sí, y que se
//              transformará en el valor 255 (el mayor unsigned char 
//              disponible) para marcar el final de cada palabra codificada.
void codifica_diccio(char *inicio_diccio, char *final_diccio)
{
  unsigned char *ptr = (unsigned char *)inicio_diccio;
  while (ptr<final_diccio)
  {
    if(*ptr=='\0')
      *ptr==255
    *ptr=Bigrama::codifica_letra(*ptr);
    ptr++;
  }
}


// Función trata_nivel
// Descripción. Se trata de una función recursiva que genera un nodo en 
// un nivel del diccionario. El nivel se especifica como el número de 
// caracteres tratados hasta el momento. Así el nivel 1 se corresponde 
// con el primer carácter de las palabras, el 1 con el siguiente, etc.
// El nodo a generar viene definido por el puntero posición, que apunta
// inicialmente al principio de la primera palabra a incluir en el nodo.
// El nodo a generar representará a todas las palabras que coincidan hasta
// el carácter p-1 con esa primera palabra.
// La función devolverá una nueva posición, la correspondiente a la primera
// palabra que coincida hasta el carácter p-2 con esa primera palabra, pero 
// que no coincida en el carácter p-1. En ese caso se tiene que llamar otra
// vez a la función con el mismo nivel y con la nueva posición, para que
// genere el siguiente nodo de ese nivel.
// En el caso en que en lugar de esa palabra se encuentre una en la que no
// coinciden los p-2 caracteres primeros, se devolverá NULL para indicar 
// al nivel anterior que se han terminado de procesar los nodos de este nivel,
// y por tanto hay que pasar al siguiente nodo del nivel anterior.

char *trata_nivel(int p, char *posicion, FILE *fout)
{
  // Datos a tener en cuenta a la hora de optimizar el nodo actual

  // Número de palabras que parten del nodo actual
  unsigned long Ncp;
  // Número de subnodos del nodo actual
  unsigned long Scp;
  // Número de bytes que contienen las terminaciones desde el nodo actual
  unsigned long Bcp;

  // Posición inicial de la zona del fichero englobada en este nodo 
  char *pos_ini_nodo = posicion;

  // Tamaños
  unsigned long tam_lista_terminaciones;
  unsigned long tam_lista_letras;
  unsigned long tam_letras = NUMCOD_BIG * 4;

  // Tamaño aproximado del siguiente nivel si usamos un nodo LETRAS o 
  // LISTA_LETRAS
  unsigned long tam_signivel; 

  // Obtiene los datos del nodo actual que necesitamos para su optimización
  calcula_datos_nodo (p, posicion, &Ncp, &Scp, &Bcp);

  // Determina el tipo de nodo que va a usar para representar el nivel

  // Vamos a hacer optimización espacial confiando en que la estructura
  // de árbol permita hacer búsquedas suficientemente rápidas. Por tanto
  // elegiremos el tipo de nodo que vaya a dar lugar a un tamaño menor
  // en la representación de las terminaciones de palabras que quedan.

  // Tamaño utilizando un nodo LISTA_TERMINACIONES - Tamaño a mejorar
  tam_lista_terminaciones = (unsigned long) (4*ceil((double)(2+Ncp+Bcp)/4.0));

  // Tamaño ocupado por el nodo lista_letras
  tam_lista_letras = (unsigned long) (4*ceil((double)(3+Scp)/4.0) + 4*Scp);

  // Tamaño aproximado ocupado por el siguiente nivel si nos decidimos a
  // utilizar un nodo LETRAS o LISTA_LETRAS
  // El cálculo aproximado se base en calcular
  //  - El número de nodos del siguiente nivel (Scp, que ya lo tenemos)
  //  - El número medio de palabras por cada nodo del siguiente nivel
  //    (Ncp/Scp)
  //  - El número medio de caracteres que representa cada nodo del 
  //    siguiente nivel (Bcp-Ncp/Scp).
  //
  // Una vez que tenemos disponibles estos datos suponemos que el siguiente
  // nivel lo realizaremos con nodos LISTA_TERMINACIONES. Calcularemos el 
  // tamaño medio de los nodos LISTA_TERMINACIONES del siguiente nivel 
  // basándonos en los valores medios calculados anteriormente y finalmente
  // multiplicaremos eso por Scp, para obtener el tamaño global

  tam_signivel = Scp * (unsinged long)
		 (4*ceil(
		     (double)(
		       2+
		       (double)Ncp/(double)Scp+
		       (double)(Bcp-Ncp)/(double)Scp
		     )
		     /4.0
		   )
		 );

  // Ahora vemos cuál consume menos memoria.
  if((tam_lista_terminaciones < tam_signivel + tam_lista_letras) &&
     (tam_lista_terminaciones < tam_signivel + tam_letras))
  {
    // Elegido el tipo de nodo lista_terminaciones

    // Escribe el nodo
    escribe_lista_terminaciones_nivel(p,posicion,fout);
  }
  else if(tam_lista_letras < tam_letras)
  {
    // Elegido el tipo de nodo lista_letras 

    // Escribe el nodo como una lista de letras y sus subnodos 
    // correspondientes. Para escribir los subnodos correspondientes
    // esta función llama a su vez a trata_nivel con los niveles
    // subsiguientes.
    escribe_lista_letras_nivel(p,posicion,fout);
  }
  else
  {
    // Elegido el tipo de nodo letras

    // Escribe el nodo como uno de tipo letras y sus subnodos 
    // correspondientes. Para escribir los subnodos correspondientes
    // esta función llama a su vez a trata_nivel con los niveles
    // subsiguientes.
    escribe_letras_nivel(p,posicion,fout);
  }

  // En este punto el descriptor del fichero se deja justo al final ...
}


// Función: calcula_datos_nodo
// Utilidad: Obtiene una serie de datos del nodo que se utilizan para
//           determinar qué tipo de nodo se utiliza para representar este
//           nodo. Esa decisión tratará de optimizar fundamentalmente el
//           espacio que ocupa la representación del subárbol que parte de
//           éste nodo.
//           Los datos calulados son:
//             Ncp - Número total de palabras comprendidas en el subárbol
//             Scp - Número de subnodos del nodo actual.
//             Bcp - Número de bytes que contienen las terminaciones
//                   contenidas en el subárbol actual.

void calcula_datos_nodo (int p,char *posicion, unsigned long *Ncp, 
			  unsigned long *Scp, unsigned long *Bcp)
{
  // Cadena que mantiene los p-1 primeros caracteres (códigos) que 
  // identifican el nodo.
  // Por supuesto, también mantiene los p-2 primeros caracteres que 
  // identifican el nodo padre de éste.
  unsigned char comun[MAXLONGPAL_DIC];

  unsigned char *ptr = posicion;
  unsigned char *ptrcomun = comun;

  // Inicialización
  *Ncp = 0;
  *Scp = 0;
  *Bcp = 0; // Bcp se calulará al final

  // Obtiene la parte común
  if(p<=1)
    *ptrcomun=255; // Esto significa que no hay parte común
  else
  {
    for(i=0;i<p-1;i++)
    {
      *ptrcomun=*ptr;
      ptr++;
      ptrcomun++;
    }
  }

  // Estudiamos la primera palabra en función del carácter en 
  // la posición p
  if(*ptr!=255)
  {
    // Caso normal, la palabra no terminaba en esta posición
    Ncp++;
    Scp++;
  }
  else
  { 
    // Caso curioso: La palabra termina en esta posición, por lo
    // que no requiere un nodo 


