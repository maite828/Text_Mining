
// Fichero : lista.h
// Descripcion :  Template C++ para listas genericas.
// Autor :
// Fecha : 18/11/99
//

#ifndef lista_h
#define lista_h

#ifndef NULL
#define NULL 0
#endif

// Template : Nodo
// Descripcion : Nodo de una lista generica.
template <class C>
  class Nodo
  {
    private :
    C valor;
    Nodo<C> *sgte;

    public :
    // Constructores y destructores.
    Nodo(const C &);
    Nodo(const Nodo<C> &);
    ~Nodo(void);

    // Operadores.
    C &Valor(void);
    Nodo<C> * &Sgte(void);
  };

// Template : Lista<C>
// Descripcion : Lista generica.
template <class C>
  class Lista
  {
    private :
    Nodo<C> *primero;

    public :
    // Posicion de un elemento dentro de la lista.
    typedef unsigned int Posicion;

    
//////////////////////////////////////////////////
// Metodos de la clase Lista.
//////////////////////////////////////////////////

// Metodo : Lista(void)
// Descripcion : Constructor por defecto de la lista.
// Parametros : Ninguno.
  Lista(void)
  : primero(NULL)
  {
  }

// Metodo : Lista(const Lista<C> &)
// Descripcion : Constructor copia.
// Parametros : Lista.
  Lista(const Lista<C> &l)
  : primero(NULL)
  {
    *this = l;
  }

// Metodo : ~Lista(void)
// Descripcion : Destructor de una lista.
// Parametros : Ninguno.
  ~Lista(void)
  {
    Libera();
  }

// Metodo : operator=
// Descripcion : operador de asignacion.
// Parametros : Lista a copiar.
  const Lista<C> &operator=(const Lista<C> &l)
  {
    // Primero borrar la lista actual.
    Libera();

    // Despues copiar.
    if (l == NULL)
    {   // Lista vacia.
      primero = NULL;
    }
    else
    {
      Nodo<C> *porigen, *pdestino;

      primero = new Nodo<C>(*(l.primero));
      porigen = l.primero->Sgte();
      pdestino = primero;

      while (porigen != NULL)
      {
        pdestino->Sgte() = new Nodo<C>(*porigen);
        pdestino = pdestino->Sgte();
        porigen = porigen->Sgte();
      }
    }
  }

// Metodo : operator+=
// Descripcion : Annade un elemento a la lista.
// parametros : Un elemento a introducir en la lista.
  void operator+=(const C &v)
  {
    if (primero == NULL)
    {  // Lista vacia.
      primero = new Nodo<C>(v);
    }
    else
    {
      Nodo<C> *ultimo = primero;

      while (ultimo->Sgte() != NULL)
        ultimo = ultimo->Sgte();

      ultimo->Sgte() = new Nodo<C>(v);
    }
  }

// Metodo : operator-=
// Descripcion : Elimina el elemento que esta en la posicion indicada.
//               Las posiciones van de 0 al tamanno-1.
// Parametros : Posicion a eliminar.
  void operator-=(Posicion pos)
  {
    if (primero == NULL)
    {  // Lista vacia.
       // ERROR.
    }
    else
    {  // Lista no vacia.
      if (pos == 0)
      {  // Eliminar el primero. Existe porque la lista no esta vacia.
        Nodo<C> *paux = primero;
  
        primero = primero->Sgte();
        delete paux;
      }
      else
      {  // Eliminar un elemento que no es el primero.
        Nodo<C> *panterior = NULL, *peliminar = primero;
        Posicion i;

        for (i = 0; i < pos; i++)
        {
          if (peliminar->Sgte() != NULL)
          {
            panterior = peliminar;
            peliminar = peliminar->Sgte();
          }
          else
          {   // No hay suficientes elementos.
            break;
          }
        }

        if (i == pos)
        {
          panterior->Sgte() = peliminar->Sgte();
          delete peliminar;
        }
        else
        {  // Pretende borrar un elemento que no esta en la lista.
          // ERROR.
        }
      }  // fin pos == 0
    }  // fin lista vacia.
  }

// Metodo : operator[]
// Descripcion : Operador de acceso a los elementos de la lista.
// Parametros : Posicion del elemento al que queremos acceder.
//              Los elementos se numeran desde 0 hasta tamanno-1.
  C &operator[](Posicion pos)
  {
    static C dummy;

    if (primero == NULL)
    {  // La lista esta vacia.
      // ERROR.
      return dummy;
    }
    else
    {  // La lista no esta vacia.
      Nodo<C> *paux = primero;
      Posicion i;

      for (i = 0; i < pos; i++)
        if (paux->Sgte() != NULL)
          paux = paux->Sgte();
        else
          break;  // No hay suficientes elementos.

      if (pos == i)
        return paux->Valor();
      else
        return dummy;  // ERROR. No existe elemento en esa posicion.
    }
  }

// Metodo : Tamanno
// Descripcion : Da el numero de elementos en la lista.
// Parametros : Ninguno.
  Posicion Tamanno(void)
  {
    Nodo<C> *paux = primero;
    Posicion n = 0;

    while (paux != NULL)
    {
      n++;
      paux = paux->Sgte();
    }

    return n;
  }

// Metodo : Busca
// Descripcion : Busca un elemento en la lista que sea igual que el dado.
//         Indica si le ha encontrado o no.
//         Usa el oprator== de la clase elemento.
// Parametros : Elemento a comparar.
  int Busca(const C &e)
  {
    Posicion p;

    for (p = 0; p < Tamanno(); p++)
      if (operator[](p) == e)
        return (0 == 0);

    return (0 == 1);
  }

// Metodo : Libera
// Descripcion : Libera toda la memoria asignada a la lista.
// Parametros : Ninguno.
  void Libera(void)
  {
    // Libero todos los nodos.
    Nodo<C> *paux;

    while (primero != NULL)
    {
      paux = primero;
      primero = primero->Sgte();
      delete paux;
    }
  }
  };

/////////////////////////////////////////////////
// Metodos de la clase Nodo.
/////////////////////////////////////////////////

// Metodo : Nodo(const C &)
// Descripcion : Constructor con valor.
// Parametros : Valor a almacenar en el nodo.
template <class C>
  Nodo<C>::Nodo(const C &v)
  : valor(v), sgte(NULL)
  {
  }

// Metodo : Nodo(const Nodo<C> &)
// Descripcion : Constructor copia. Crea un nodo con una copia del valor 
//         y el campo sgte a NULL
// Parametros : Nodo a copiar.
template <class C>
  Nodo<C>::Nodo(const Nodo<C> &n)
  : valor(n.valor), sgte(NULL)
  {
  }

// Metodo : ~Nodo
// Descripcion : Destructor del nodo.
// Parametros : Ninguno.
template <class C>
  Nodo<C>::~Nodo(void)
  {
  }

// Metodo : Valor
// Descripcion : Acceso lectura/escritura al valor almacenado en el nodo.
// Parametros : Ninguno.
template <class C>
  inline C &Nodo<C>::Valor(void)
  {
    return valor;
  }

// Metodo : Sgte
// Descripcion : Acceso lectura/escritura al puntero al siguiente nodo.
// Parametros : Ninguno.
template <class C>
  inline Nodo<C> * &Nodo<C>::Sgte(void)
  {
    return sgte;
  }

#endif //lista_h
