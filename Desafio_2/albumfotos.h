#ifndef ALBUMFOTOS_H
#define ALBUMFOTOS_H

#include "ListaDinamica.h"

//album con privacidad P, A o R. las fotos las borra el album

class Foto;

class AlbumFotos {
private:
    char * nombre;
    char privacidad;
    ListaDinamica<Foto *> fotos;
    bool valido;

public:
    AlbumFotos();
    AlbumFotos(const char * nombre, char privacidad);
    ~AlbumFotos();
    AlbumFotos(const AlbumFotos &) = delete;
    AlbumFotos & operator=(const AlbumFotos &) = delete;

    bool estaValido() const;
    const char * getNombre() const;
    char getPrivacidad() const;
    unsigned int getNumFotos() const;
    bool setNombre(const char * nombre);
    void setPrivacidad(char privacidad);

    bool agregarFoto(Foto * foto);
    Foto * getFoto(unsigned int posicion) const;
    Foto * getFotoCircular(int posicion) const;
    bool puedeVer(bool esDueno, bool esAmigo) const;
    unsigned long calcularMemoria() const;
};

#endif
