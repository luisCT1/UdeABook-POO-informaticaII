#ifndef LISTADINAMICA_H
#define LISTADINAMICA_H

#include "utilidades.h"
#include "MedidorRecursos.h"

template <class T>
class ListaDinamica {
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

template <class T>
ListaDinamica<T>::ListaDinamica()
    : elementos(nullptr), tamaño(0), capacidad(0), valida(true) {
}

template <class T>
ListaDinamica<T>::ListaDinamica(unsigned int capacidadInicial)
    : elementos(nullptr), tamaño(0), capacidad(0), valida(true) {
    if (capacidadInicial > 0) {
        elementos = reservar<T>(capacidadInicial);
        if (elementos == nullptr) {
            valida = false;
        } else {
            capacidad = capacidadInicial;
        }
    }
}

template <class T>
ListaDinamica<T>::ListaDinamica(const ListaDinamica & otra)
    : elementos(nullptr), tamaño(0), capacidad(0), valida(true) {
    if (otra.tamaño > 0) {
        elementos = reservar<T>(otra.tamaño);
        if (elementos == nullptr) {
            valida = false;
            return;
        }
        for (unsigned int i = 0; i < otra.tamaño; i++) {
            MedidorRecursos::sumarIteraciones();
            elementos[i] = otra.elementos[i];
        }
        tamaño = otra.tamaño;
        capacidad = otra.tamaño;
    }
}

template <class T>
ListaDinamica<T>::~ListaDinamica() {
    delete[] elementos;
}

template <class T>
bool ListaDinamica<T>::estaValida() const { return valida; }

template <class T>
unsigned int ListaDinamica<T>::gettamaño() const { return tamaño; }

template <class T>
unsigned int ListaDinamica<T>::getCapacidad() const { return capacidad; }

template <class T>
bool ListaDinamica<T>::estaVacia() const { return tamaño == 0; }

template <class T>
bool ListaDinamica<T>::asegurarCapacidad(unsigned int minima) {
    if (minima <= capacidad) {
        return true;
    }
    T * nuevo = reservar<T>(minima);
    if (nuevo == nullptr) {
        return false;
    }
    for (unsigned int i = 0; i < tamaño; i++) {
        MedidorRecursos::sumarIteraciones();
        nuevo[i] = elementos[i];
    }
    delete[] elementos;
    elementos = nuevo;
    capacidad = minima;
    return true;
}

template <class T>
bool ListaDinamica<T>::ampliar() {
    if (capacidad > 0x7FFFFFFFu) {
        return false;
    }
    unsigned int nueva = (capacidad == 0) ? 2 : capacidad * 2;
    return asegurarCapacidad(nueva);
}

template <class T>
bool ListaDinamica<T>::agregar(const T & elemento) {
    MedidorRecursos::sumarReferencia(tamaño);
    if (tamaño == capacidad) {
        if (!ampliar()) {
            return false;
        }
    }
    elementos[tamaño] = elemento;
    tamaño++;
    return true;
}

template <class T>
T & ListaDinamica<T>::operator[](unsigned int posicion) {
    return elementos[posicion];
}

template <class T>
const T & ListaDinamica<T>::operator[](unsigned int posicion) const {
    return elementos[posicion];
}


template <class T>
ListaDinamica<T> & ListaDinamica<T>::operator=(const ListaDinamica & otra) {
    if (this == &otra) {
        return *this;
    }
    T * nuevo = nullptr;
    if (otra.tamaño > 0) {
        nuevo = reservar<T>(otra.tamaño);
        if (nuevo == nullptr) {
            return *this;
        }
        for (unsigned int i = 0; i < otra.tamaño; i++) {
            MedidorRecursos::sumarIteraciones();
            nuevo[i] = otra.elementos[i];
        }
    }
    delete[] elementos;
    elementos = nuevo;
    tamaño = otra.tamaño;
    capacidad = otra.tamaño;
    return *this;
}

template <class T>
unsigned long ListaDinamica<T>::calcularMemoria() const {
    return sizeof(*this) + (unsigned long)capacidad * sizeof(T);
}

#endif