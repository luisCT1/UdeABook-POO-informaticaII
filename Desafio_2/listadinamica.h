#ifndef LISTADINAMICA_H
#define LISTADINAMICA_H

//arreglo que crece solo. guarda punteros, pero no borra los objetos

template <class T>
class ListaDinamica {
private:
    T * elementos;
    unsigned int tamanio;
    unsigned int capacidad;
    bool valida;
    bool ampliar();

public:
    ListaDinamica();
    explicit ListaDinamica(unsigned int capacidadInicial);
    ListaDinamica(const ListaDinamica & otra);
    ~ListaDinamica();

    bool estaValida() const;
    unsigned int getTamanio() const;
    unsigned int getCapacidad() const;
    bool estaVacia() const;

    bool agregar(const T & elemento);
    bool asegurarCapacidad(unsigned int minima);

    T & operator[](unsigned int posicion);
    const T & operator[](unsigned int posicion) const;
    ListaDinamica & operator=(const ListaDinamica & otra);

    unsigned long calcularMemoria() const;
};

#include "ListaDinamica.tpp"

#endif
