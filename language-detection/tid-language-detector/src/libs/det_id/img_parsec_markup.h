// Este fichero define un array de caracteres que contiene la imagen en
// memoria del objeto parsec_markup que define los posibles códigos de 
// marcado utilizados en el proceso de adecuación de los e-mailes
 
// Este array y la estructura parsec_markup se utilizará tanto en el 
// detector de idioma, como en el corrector ortográfico. Para la 
// introducción del marcado no es necesario recurrir a esta estructura,
// es necesaria sobre todo para la lectura del marcado.

static char img_parsec_markup[] = 
{

  //
  // Marcas para indicar el idioma del texto que viene a continuación
  // 

  (char) 13,                                             // OFFSET A SIGUIENTE
  '\\','C','A','S','\\',                                 // CADENA BÚSQUEDA
  (char) 0,                                              // CERO SEPARADOR
  '\\','C','A','S','\\',                                 // CADENA BÚSQUEDA
  (char) 0,                                              // CERO SEPARADOR
 
  (char) 13,                                             // OFFSET A SIGUIENTE
  '\\','C','A','T','\\',                                 // CADENA BÚSQUEDA
  (char) 0,                                              // CERO SEPARADOR
  '\\','C','A','T','\\',                                 // CADENA BÚSQUEDA
  (char) 0,                                              // CERO SEPARADOR
 
  (char) 13,                                             // OFFSET A SIGUIENTE
  '\\','G','A','L','\\',                                 // CADENA BÚSQUEDA
  (char) 0,                                              // CERO SEPARADOR
  '\\','G','A','L','\\',                                 // CADENA BÚSQUEDA
  (char) 0,                                              // CERO SEPARADOR
 
  (char) 13,                                             // OFFSET A SIGUIENTE
  '\\','E','U','S','\\',                                 // CADENA BÚSQUEDA
  (char) 0,                                              // CERO SEPARADOR
  '\\','E','U','S','\\',                                 // CADENA BÚSQUEDA
  (char) 0,                                              // CERO SEPARADOR
 
  (char) 13,                                             // OFFSET A SIGUIENTE
  '\\','I','N','G','\\',                                 // CADENA BÚSQUEDA
  (char) 0,                                              // CERO SEPARADOR
  '\\','I','N','G','\\',                                 // CADENA BÚSQUEDA
  (char) 0,                                              // CERO SEPARADOR
 
  (char) 13,                                             // OFFSET A SIGUIENTE
  '\\','F','R','A','\\',                                 // CADENA BÚSQUEDA
  (char) 0,                                              // CERO SEPARADOR
  '\\','F','R','A','\\',                                 // CADENA BÚSQUEDA
  (char) 0,                                              // CERO SEPARADOR
 
  (char) 13,                                             // OFFSET A SIGUIENTE
  '\\','A','L','E','\\',                                 // CADENA BÚSQUEDA
  (char) 0,                                              // CERO SEPARADOR
  '\\','A','L','E','\\',                                 // CADENA BÚSQUEDA
  (char) 0,                                              // CERO SEPARADOR
 
  (char) 13,                                             // OFFSET A SIGUIENTE
  '\\','B','R','A','\\',                                 // CADENA BÚSQUEDA
  (char) 0,                                              // CERO SEPARADOR
  '\\','B','R','A','\\',                                 // CADENA BÚSQUEDA
  (char) 0,                                              // CERO SEPARADOR

  (char) 13,                                             // OFFSET A SIGUIENTE
  '\\','I','T','A','\\',                                 // CADENA BÚSQUEDA
  (char) 0,                                              // CERO SEPARADOR
  '\\','I','T','A','\\',                                 // CADENA BÚSQUEDA
  (char) 0,                                              // CERO SEPARADOR


  //
  // Marcas para indicar la presencia de una URL completa o no
  //
 
  (char) 21,                                             // OFFSET A SIGUIENTE
  '\\','u','r','l','_','i','n','i','\\',                 // CADENA BÚSQUEDA
  (char) 0,                                              // CERO SEPARADOR
  '\\','u','r','l','_','i','n','i','\\',                 // CADENA BÚSQUEDA
  (char) 0,                                              // CERO SEPARADOR
 
  (char) 21,                                             // OFFSET A SIGUIENTE
  '\\','u','r','l','_','f','i','n','\\',                 // CADENA BÚSQUEDA
  (char) 0,                                              // CERO SEPARADOR
  '\\','u','r','l','_','f','i','n','\\',                 // CADENA BÚSQUEDA
  (char) 0,                                              // CERO SEPARADOR

  //
  // Indicación del final de la tabla
  //

  (char) 0

}; 
