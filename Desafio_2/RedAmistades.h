#ifndef REDAMISTADES_H
#define REDAMISTADES_H

#include <random>
#include "configuracion.h"

class RedAmistades {
private:
    unsigned long long * bitsAmistad;
    unsigned long long * bitsSolicitud;
    unsigned int * grados;
    unsigned int * pendientes;
    //lista de adyacencia: para recorrer amigos sin barrer la fila de bits
    unsigned int ** listasAmigos;
    unsigned int * capListas;
    unsigned int numUsuarios;
    unsigned int palabrasPorFila;
    std::mt19937 generador;
    bool valido;


    bool leerBit(const unsigned long long * matriz, unsigned int fila, unsigned int columna) const;
    void  escribirBit(unsigned long long * matriz, unsigned int fila, unsigned int columna, bool valor);
    unsigned int contarBits(unsigned long long palabra) const;
    unsigned int extraerIndices(const unsigned long long * matriz, unsigned int fila,
                                unsigned int * salida, unsigned int capacidad) const;
    bool agregarEnLista(unsigned int u, unsigned int v);
    float calcularPeso(unsigned int amigosEnComun) const;
    void  liberar();

public:
    RedAmistades();
    explicit RedAmistades(unsigned int numUsuarios);
    RedAmistades(const RedAmistades & otra);
    ~RedAmistades();

    bool estaValido() const;
    unsigned int getNumUsuarios() const;
    void setSemilla(unsigned int semilla);

    // ---- Amistades ----
    bool conectar(unsigned int a, unsigned int b);
    bool sonAmigos(unsigned int a, unsigned int b) const;
    unsigned int getGrado(unsigned int a) const;
    unsigned int contarEnComun(unsigned int a, unsigned int b) const;
    unsigned int obtenerAmigos(unsigned int a, unsigned int * salida, unsigned int capacidad) const;


    unsigned short enviarSolicitud(unsigned int emisor, unsigned int receptor);
    bool tieneSolicitud(unsigned int emisor, unsigned int receptor) const;
    unsigned int contarSolicitudes(unsigned int receptor) const;
    unsigned int obtenerSolicitudes(unsigned int receptor, unsigned int * emisores, unsigned int capacidad) const;
    bool aceptarSolicitud(unsigned int emisor, unsigned int receptor);
    void marcarSolicitud(unsigned int emisor, unsigned int receptor);


    unsigned short sortearAmigos(unsigned int usuario, unsigned short cuantos,
                                 const unsigned int * excluidos, unsigned short numExcluidos,
                                 unsigned int * salida);

    RedAmistades & operator=(const RedAmistades & otra);

    unsigned long calcularMemoria() const;
    unsigned long calcularMemoriaReferencia() const;
};

#endif
