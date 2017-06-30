// Fichero : trata_car.h
// Descripcion : Elementos para tratamiento personalizado de caracteres.
// Version : 1.0
// Fecha : 3/12/1999

#ifndef trata_car_h
#define trata_car_h

// Clase : TRATA_CAR
// Descripcion : Clase que tiene los metodos para tratar los caracteres.
//      Permite clasificar los caracteres segun diversos criterios.
//      Clasifica caracteres codificados como enteros sin signo entre
//      0x00 y 0xff.
//      Se ajusta al ASCII entre 0x00 y 0x7f
//      y al ISO_8859_1 entre 0x80 y 0xff.
//      Tambien permite ciertas conversiones personalizadas.
//      La clase solo proporciona espacio de nombres.
//
//      Tabla de codigo y clase a la que pertenecen.
//      0x00-0x1f	Ctrl
//      0x20-0x20	Blanco
//      0x21-0x2f       Puntuacion
//      0x30-0x39       Digito
//      0x3a-0x40       Puntuacion
//      0x41-0x5a       Mayuscula
//      0x5b-0x60       Puntuacion
//      0x61-0x7a       Minuscula
//      0x7b-0x7e       Puntuacion
//      0x7f-0x7f       Ctrl
//      0x80-0x9f       Ctrl
//      0xa0-0xbf       Puntuacion
//      0xc0-0xdd       Mayuscula
//      0xde-0xf6       Minuscula
//      0xf7-0xf7       Puntuacion
//      0xf8-0xfd       Minuscula
//      0xfe-0xfe       Mayuscula
//      0xff-0xff       Minuscula

#include <string.h>

class TRATA_CAR
{
  public :

  // Metodo : Ascii
  // Descripcion : Indica si es ASCII STANDARD de 7 bits.
  inline static int Ascii(unsigned char c)
  {
    return (c <= 0x7f);
  }

  // Categorias.

  // Metodo : Blanco.
  // Descripcion : Indica si es un caracter conceptuado como blanco.
  inline static int Blanco(unsigned char c)
  {
    // Blancos reconocidos.
    static unsigned char blancos[] = " \f\n\r\t\v";

    if(strchr((const char *)blancos,c)!=NULL)
      return (0 == 0);

    // No es blanco.
    return (0 == 1);
  }

  // Metodo : Ctrl
  // Descripcion : Indica si es un caracter de control, esto es,
  //        si el caracter tiene alguna interpretacion diferente a
  //        presentar un simbolo en la salida.
  inline static int Ctrl(unsigned char c)
  {
    return ( ( (0x00 <= c) && (c <= 0x1f) ) ||
             ( (0x7f <= c) && (c <= 0x9f) ) );
  }

  // Metodo : Puntuacion
  // Descripcion : Indica si es un caracter de puntuacion.
  //       Son caracteres que no forman parte de palabras, pero 
  //       aparecen en las frases y en expresiones.
  inline static int Puntuacion(unsigned char c)
  {
    return ( ( (0x21 <= c) && (c <= 0x2f) ) ||
             ( (0x3a <= c) && (c <= 0x40) ) ||
             ( (0x5b <= c) && (c <= 0x60) ) ||
             ( (0x7b <= c) && (c <= 0x7e) ) ||
             ( (0xa0 <= c) && (c <= 0xbf) ) ||
             (0xf7 == c) || (0xd7 == c)   );
  }

  // Metodo : Digito
  // Descripcion : Indica si es un digito.
  inline static int Digito(unsigned char c)
  {
    return ( (0x30 <= c) && (c <= 0x39) );
  }

  // Metodo : Mayuscula
  // Descripcion : Indica si es una letra mayuscula.
  inline static int Mayuscula(unsigned char c)
  {
    return ( ( (0x41 <= c) && (c <= 0x5a) ) ||
             ( (0xc0 <= c) && (c <= 0xd6) ) ||
             ( (0xd8 <= c) && (c <= 0xdf) ) );
  }

  // Metodo : Minusculas
  // Descripcion : Indica si es una letra minuscula.
  inline static int Minuscula(unsigned char c)
  {
    return ( ( (0x61 <= c) && (c <= 0x7a) ) ||
             ( (0xe0 <= c) && (c <= 0xf6) ) ||
             ( (0xf8 <= c) && (c <= 0xff) ) );
  }

  // Categorias compuestas.

  // Metodo : Letra
  // Descripcion : Indica si es una letra. Cualquier simbolo que 
  //        pueda formar palabras.
  inline static int Letra(unsigned char c)
  {
    return (Mayuscula(c) || Minuscula(c));
  }

  // Metodo : Delimitador
  // Descripcion : Indica si es un delimitador de palabra.
  //       Cualquier simbolo que indica un cambio de palabra en frases 
  //       o expresiones matematicas.
  inline static int Delimitador(unsigned char c)
  {
    return (Ctrl(c) || Blanco(c) || Puntuacion(c));
  }

  // Manipulaciones.

  // Pares minuscula/mayuscula fuera del ASCII STD.
#define PARES "àÀáÁâÂãÃäÄåÅæÆçÇèÈéÉêÊëËìÌíÍîÎïÏðÐñÑòÒóÓôÔõÕöÖøØùÙúÚûÛüÜýÝþÞ"

  // Metodo : AMayusculas
  // Descripcion : Devuelve el caracter convertido a mayusculas.
  //       Si no es posible, devuelve el mismo caracter.
  inline static unsigned char AMayusculas(unsigned char c)
  {
    char *p;
    char *pares = PARES;

    if ((c>=0x61)&&(c<=0x7a))
    {
      // ASCII STD.
      // Conversion estandar.
      return (unsigned char)('A' - 'a' + c);
    }
    else if ((p = strchr(pares, c))!=NULL)
    {
      if (((p-pares)%2)==0)
      {
	// ISO-8859-1
	// Conversión especial
	return (unsigned char)(*(++p));
      }
      else
      {
	// No convertible.
	return c;
      }
    }
    else
    {
      // No convertible.
      return c;
    }
  }

  // Metodo : AMinusculas
  // Descripcion : Devuelve el caracter convertido a minusculas.
  //       Si no es posible, devuelve el mismo caracter.
  inline static unsigned char AMinusculas(unsigned char c)
  {
    char *p;
    char *pares = PARES;

    if ((c>=0x41)&&(c<=0x5a))
    {
      // ASCII STD.
      // Conversion estandar.
      return (unsigned char)('a' - 'A' + c);
    }
    else if((p = strchr(pares, c))!=NULL)
    {
      if (((p-pares)%2)==1)
      {
	// ISO-8859-1
	// Conversión especial
	return (unsigned char)(*(--p));
      }
      else
      {
	// No convertible.
	return c;
      }
    }
    else
    {
      // No convertible.
      return c;
    }
  }
};

#endif //trata_car_h
