#ifndef FOTO_H
#define FOTO_H

#include "FechaHora.h"

//una foto de un album. la ruta completa se arma despues, aca solo va el nombre

class Foto {
private:
    unsigned int id;
    FechaHora fechaCaptura;
    unsigned int tamanio;
    char * nombreArchivo;
    unsigned int likes;
    bool valido;

public:
    Foto();
    Foto(unsigned int id, const FechaHora & fechaCaptura, unsigned int tamanio,
         const char * nombreArchivo, unsigned int likes);
    Foto(const Foto & otra);
    ~Foto();

    bool estaValido() const;

    unsigned int getId() const;
    const FechaHora & getFechaCaptura() const;
    unsigned int getTamanio() const;
    const char * getNombreArchivo() const;
    unsigned int getLikes() const;

    void setFechaCaptura(const FechaHora & fecha);
    void setTamanio(unsigned int bytes);
    bool setNombreArchivo(const char * nombre);
    void setLikes(unsigned int cantidad);

    Foto & operator++();
    Foto & operator--();
    Foto & operator=(const Foto & otra);

    unsigned long calcularMemoria() const;
};

#endif
