#include "Foto.h"
#include "utilidades.h"

#include <new>

Foto::Foto()
    : id(0), tamanio(0), nombreArchivo(nullptr), likes(0), valido(true) {
}

Foto::Foto(unsigned int id, const FechaHora & fechaCaptura, unsigned int tamanio,
           const char * nombreArchivo, unsigned int likes)
    : id(id), fechaCaptura(fechaCaptura), tamanio(tamanio),
      nombreArchivo(nullptr), likes(likes), valido(true) {
    this->nombreArchivo = util::duplicar(nombreArchivo == nullptr ? "" : nombreArchivo);
    if (this->nombreArchivo == nullptr) {
        valido = false;
    }
}

Foto::Foto(const Foto & otra)
    : id(otra.id), fechaCaptura(otra.fechaCaptura), tamanio(otra.tamanio),
      nombreArchivo(nullptr), likes(otra.likes), valido(otra.valido) {
    nombreArchivo = util::duplicar(otra.nombreArchivo == nullptr ? "" : otra.nombreArchivo);
    if (nombreArchivo == nullptr) {
        valido = false;
    }
}

Foto::~Foto() {
    delete[] nombreArchivo;
}

bool Foto::estaValido() const { return valido; }

unsigned int Foto::getId() const { return id; }
const FechaHora & Foto::getFechaCaptura() const { return fechaCaptura; }
unsigned int Foto::getTamanio() const { return tamanio; }
const char * Foto::getNombreArchivo() const { return nombreArchivo; }
unsigned int Foto::getLikes() const { return likes; }

void Foto::setFechaCaptura(const FechaHora & fecha) { fechaCaptura = fecha; }
void Foto::setTamanio(unsigned int bytes) { tamanio = bytes; }

bool Foto::setNombreArchivo(const char * nombre) {
    char * copia = util::duplicar(nombre == nullptr ? "" : nombre);
    if (copia == nullptr) {
        return false;
    }
    delete[] nombreArchivo;
    nombreArchivo = copia;
    return true;
}

void Foto::setLikes(unsigned int cantidad) { likes = cantidad; }

Foto & Foto::operator++() {
    likes++;
    return *this;
}

Foto & Foto::operator--() {
    //quitar like no puede dejarlo en negativo
    if (likes > 0) {
        likes--;
    }
    return *this;
}

Foto & Foto::operator=(const Foto & otra) {
    if (this == &otra) {
        return *this;
    }
    char * copia = util::duplicar(otra.nombreArchivo == nullptr ? "" : otra.nombreArchivo);
    if (copia == nullptr) {
        valido = false;
        return *this;
    }
    delete[] nombreArchivo;
    nombreArchivo = copia;
    id = otra.id;
    fechaCaptura = otra.fechaCaptura;
    tamanio = otra.tamanio;
    likes = otra.likes;
    valido = otra.valido;
    return *this;
}

unsigned long Foto::calcularMemoria() const {
    unsigned long extra = 0;
    if (nombreArchivo != nullptr) {
        extra = util::longitud(nombreArchivo) + 1;
    }
    return sizeof(*this) + extra;
}
