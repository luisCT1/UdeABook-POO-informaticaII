#ifndef UTILIDADES_H
#define UTILIDADES_H

#include <new>
#include <iostream>

//funciones sueltas de texto. no van dentro de una clase

unsigned int longitud(const char * texto);
char * copiar(const char * texto);
bool copiarEn(const char * origen, char * destino, unsigned int capacidad);
bool iguales(const char * a, const char * b);
int comparar(const char * a, const char * b);
bool aNumero(const char * texto, unsigned int desde, unsigned int hasta, unsigned long long & valor);
void escribirNumero(unsigned long long valor, unsigned int ancho, char * destino);
const char * descripcionResultado(unsigned short codigoResultado);

template <class T>
T * reservar(unsigned int cantidad) {
    T * bloque = new (std::nothrow) T[cantidad];
    if (bloque == nullptr) {
        std::cerr << "[ERROR] No hay memoria suficiente para reservar " << cantidad << " elementos.\n";
    }
    return bloque;
}

#endif
