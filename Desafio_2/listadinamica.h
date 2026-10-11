#ifndef LISTADINAMICA_H
#define LISTADINAMICA_H

template <class T>
class ListaDinamica{
    private:
T *          elementos;
unsigned int tamaño;
unsigned int capacidad;
bool         valida;
bool         ampliar();
public:
ListaDinamica();
explicit ListaDinamica(unsigned int capacidadInicial);
ListaDinamica(const ListaDinamica & otra);
~ListaDinamica();

bool estaValida() const;
unsigned int gettamaño() const;
unsigned int getCapacidad() const;
bool estaVacia() const;

bool agregar(const T & elemento);
bool asegurarCapacidad(unsigned int minima);

T &       operator[](unsigned int posicion);
const T & operator[](unsigned int posicion) const;
ListaDinamica & operator=(const ListaDinamica & otra);

unsigned long calcularMemoria() const;

};
#endif // LISTADINAMICA_H
