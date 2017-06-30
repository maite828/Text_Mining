/*************************************************

   FICHERO: Alinear.cc

   FUNCION:  int Det_id_interno::Alinear(char *cad_in, char *cad_out, char *sucesos)

   PROGRAMADA POR: Doroteo Torre Toledano.

   TAREA QUE REALIZA: Hace un alineamiento DTW entre dos cadenas de caracteres.
      Inicialmente se basa en pesos fijos, pero supongo que acabaré
      extendiéndolo a una matriz de confusión de caracteres totalmente libre.
      
   PARAMETROS:
      cad_in, cad_out: Son las cadenas que se comparan. cad_in se supone la
         inicial, y se supone que cad_out se obtiene a partir de cad_in con
         una secuencia de Aciertos, Sustituciones, Borrados e Inserciones.

      sucesos: Es la cadena en la que se indica la secuencia de Aciertos (A)
         Sustituciones (S), Borrados (B), e Inserciones (I) que han ocurrido
         para obtener cad_out a partir de cad_in. Esta cadena puede tener 
         una longitud igual a la suma de las longitudes de las dos cadenas
         de entrada.

   SALIDA: 
      Devuelve el coste del alineamiento, un valor mayor cuanto más dispares
      sean las cadenas.

   HISTÓRICO DE MODIFICACIONES:

      25/2/98 - Creación de la función.

**************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Det_id_interno.h"

/* Pesos que se van a emplear en el alineamiento. */
#define COSTE_ACI 0
#define COSTE_SUS 10
#define COSTE_BOR 7
#define COSTE_INS 7

int Det_id_interno::Alinear(char *cad_in, char *cad_out, char *sucesos)
{
   struct GRID
   {
      int coste;
      char dir;
   };
   struct GRID grid[MAXLONGPAL_DET_ID+1][MAXLONGPAL_DET_ID+1];
   char sucesos_inv[MAXLONGPAL_DET_ID*2];
   char *ptr_in, *ptr_out, *ptr_suc;
   char *ptr_sal;
   struct GRID *ptr_fila, *ptr_fila_ant;
   int i,j;
   int n,m;
   int V,H,D;
   int acu,coste;

   /* Obtiene el tamaño de la matriz a rellenar */
   m = strlen(cad_in);  /* N columnas-1 */
   n = strlen(cad_out); /* N filas - 1  */

   /* Rellena la primera columna */
   ptr_fila = &(grid[0][0]);
   acu = 0;
   for(j=0;j<=n;j++,ptr_fila++,acu+=COSTE_INS)
   {
      ptr_fila->coste = acu; 
      ptr_fila->dir = 'V';
   }
   
   /* Ahora Rellena el resto */
   ptr_in = cad_in;
   for(i=1;i<=m;i++)
   {
      /* Inicializa la columna */
      ptr_out = cad_out;

      ptr_fila = &(grid[i][0]);
      ptr_fila_ant = &(grid[i-1][0]);
      ptr_fila->coste = ptr_fila_ant->coste + COSTE_BOR;
      ptr_fila->dir = 'H';

      /* Rellena el resto de elementos de la columna */
      for(j=1;j<=n;j++)
      {
         V = ptr_fila->coste + COSTE_INS;
         ptr_fila++;
         if(*ptr_in == *ptr_out)
            D = ptr_fila_ant->coste + COSTE_ACI;
         else
            D = ptr_fila_ant->coste + COSTE_SUS;
         ptr_fila_ant++;
         H = ptr_fila_ant->coste + COSTE_BOR;
   
         if((D<=V)&&(D<=H))
         {
            ptr_fila->coste = D;
            ptr_fila->dir = 'D';
         }
         else if (V<=H)
         {
            ptr_fila->coste = V;
            ptr_fila->dir = 'V';
         }
         else 
         {
            ptr_fila->coste = H;
            ptr_fila->dir = 'H';
         }
      
         ptr_out++;
      }
      ptr_in++;
   }

   /* Ahora hace el backtracking para detectar los sucesos */
   ptr_suc = sucesos_inv;
   i = m;
   j = n;
   coste = grid[i][j].coste;
   acu = coste;
   while((i!=0)||(j!=0))
   {
      switch(grid[i][j].dir)
      {
         case 'D': /* ACIERTO O SUSTITUCION */
         
                  if(grid[i][j].coste-grid[i-1][j-1].coste==COSTE_ACI)
                  {
                     /* ACIERTO */
                     *ptr_suc = 'A';
                     i--;
                     j--;
                  }
                  else 
                  {
                     /* SUSTITUCION */
                     *ptr_suc   = 'S';
                     i--;
                     j--;
                  }
                  break;

         case 'H': /* BORRADO */
                  *ptr_suc = 'B';
                  i--;
                  break;

         case 'V': /* INSERCIÓN */
                  *ptr_suc = 'I';
                  j--;
                  break;
         
         default: /* NUNCA DEBERÍA LLEGAR AQUÍ */
                  fprintf(stderr,"ERROR INTERNO (alinear.c).\n");
                  fprintf(stderr,"Alineando: cad_in = \"%s\", cad_out = \"%s\"\n",
                           cad_in,cad_out);
                  fprintf(stderr,"Situación: i = %d, j = %d\n",i,j);
                  fprintf(stderr,"GRID: j|_i\n");
                  for(j=0;j<=n;j++)
                  {
                     for(i=0;i<=m;i++)
                      {
                        fprintf(stderr,"%3d ",grid[i][j]);
                     }
                     fprintf(stderr,"\n");
                  }
                  exit(-1);
                  break;
      }
   ptr_suc++;
   }
   
   /* Ahora inverte la cadena de sucesos y la deja en la definitva */
   ptr_sal = sucesos;
	 ptr_suc--;
   while(ptr_suc>=sucesos_inv)
   {
      *ptr_sal = *ptr_suc;
      ptr_sal++;
      ptr_suc--;
   }
   *ptr_sal = '\0';

   return(coste);
   
}
