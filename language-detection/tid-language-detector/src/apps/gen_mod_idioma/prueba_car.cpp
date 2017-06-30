// Prueba del trtamiento de los caracteres.

#include <iostream>

#include "libs/det_id/trata_car.h"

int main (void)
{
  cout << "num c ABCPDMXLdmx" << endl;
  for (unsigned char i = 0; i <= 0xff; i++)
  {
    cout << (int)i << " " << i << " ";
    cout << TRATA_CAR::Ascii(i) << TRATA_CAR::Blanco(i);
    cout << TRATA_CAR::Ctrl(i) << TRATA_CAR::Puntuacion(i);
    cout << TRATA_CAR::Digito(i) << TRATA_CAR::Mayuscula(i);
    cout << TRATA_CAR::Minuscula(i) << TRATA_CAR::Letra(i);
    cout << TRATA_CAR::Delimitador(i) << TRATA_CAR::AMayusculas(i);
    cout << TRATA_CAR::AMinusculas(i);
    cout << endl;
  }
}

