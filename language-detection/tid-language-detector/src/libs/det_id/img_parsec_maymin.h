// Este fichero define un array de caracteres que contiene la imagen en
// memoria del objeto parsec_maymin que define las combinaciones de 
// caracteres en mayúsculas y minúsculas a considerar
 
// Este array y la estructura parsec_maymin se utilizará tanto en el 
// detector de idioma, como en el corrector ortográfico. 

static char img_parsec_maymin[] = 
{

  //
  // Entradas de la tabla
  // 

  (char) 9,                                              // OFFSET A SIGUIENTE
  'm','m',                                               // PATRON MAYMIN
  (char) 0,                                              // CERO SEPARADOR
  'C','O','R','R',                                       // CORR / NOCORR
  (char) 0,                                              // CERO SEPARADOR
 
  (char) 11,                                             // OFFSET A SIGUIENTE
  'm','M',                                               // PATRON MAYMIN
  (char) 0,                                              // CERO SEPARADOR
  'N','O','C','O','R','R',                               // CORR / NOCORR
  (char) 0,                                              // CERO SEPARADOR
 
  (char) 11,                                             // OFFSET A SIGUIENTE
  'M','m',                                               // PATRON MAYMIN
  (char) 0,                                              // CERO SEPARADOR
  'N','O','C','O','R','R',                               // CORR / NOCORR
  (char) 0,                                              // CERO SEPARADOR
 
  (char) 11,                                             // OFFSET A SIGUIENTE
  'M','M',                                               // PATRON MAYMIN
  (char) 0,                                              // CERO SEPARADOR
  'N','O','C','O','R','R',                               // CORR / NOCORR
  (char) 0,                                              // CERO SEPARADOR
 
  (char) 11,                                             // OFFSET A SIGUIENTE
  'm','X',                                               // PATRON MAYMIN
  (char) 0,                                              // CERO SEPARADOR
  'N','O','C','O','R','R',                               // CORR / NOCORR
  (char) 0,                                              // CERO SEPARADOR
 
  (char) 11,                                             // OFFSET A SIGUIENTE
  'M','X',                                               // PATRON MAYMIN
  (char) 0,                                              // CERO SEPARADOR
  'N','O','C','O','R','R',                               // CORR / NOCORR
  (char) 0,                                              // CERO SEPARADOR
 
  //
  // Indicación del final de la tabla
  //

  (char) 0

}; 
