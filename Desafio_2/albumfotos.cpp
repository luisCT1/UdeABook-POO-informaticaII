#include "AlbumFotos.h"
#include "Foto.h"
#include "configuracion.h"
#include "utilidades.h"
#include "MedidorRecursos.h"

AlbumFotos::AlbumFotos()
    : nombre(nullptr), privacidad(PRIVACIDAD_PUBLICA), valido(true) {
}

AlbumFotos::AlbumFotos(const char * nombre, char privacidad)
    : nombre(nullptr), privacidad(PRIVACIDAD_PUBLICA), valido(true) {
    this->nombre = copiar(nombre == nullptr ? "" : nombre);
    if (this->nombre == nullptr) {
        valido = false;
    }
    setPrivacidad(privacidad);
}

AlbumFotos::~AlbumFotos() {
    for (unsigned int i = 0; i < fotos.getTamanio(); i++) {
        MedidorRecursos::sumarIteraciones();
        delete fotos[i];
    }
    delete[] nombre;
}

bool AlbumFotos::estaValido() const { return valido && fotos.estaValida(); }
const char * AlbumFotos::getNombre() const { return nombre; }
char AlbumFotos::getPrivacidad() const { return privacidad; }
unsigned int AlbumFotos::getNumFotos() const { return fotos.getTamanio(); }

bool AlbumFotos::setNombre(const char * nombreNuevo) {
    char * copia = copiar(nombreNuevo == nullptr ? "" : nombreNuevo);
    if (copia == nullptr) {
        return false;
    }
    delete[] nombre;
    nombre = copia;
    return true;
}

void AlbumFotos::setPrivacidad(char valor) {
    if (valor == PRIVACIDAD_PUBLICA || valor == PRIVACIDAD_PRIVADA || valor == PRIVACIDAD_AMIGOS) {
        privacidad = valor;
    }
}

bool AlbumFotos::agregarFoto(Foto * foto) {
    if (foto == nullptr) {
        return false;
    }
    if (!fotos.agregar(foto)) {
        return false;
    }
    return true;
}

Foto * AlbumFotos::getFoto(unsigned int posicion) const {
    if (posicion >= fotos.getTamanio()) {
        return nullptr;
    }
    return fotos[posicion];
}

Foto * AlbumFotos::getFotoCircular(int posicion) const {
    int n = (int)fotos.getTamanio();
    if (n == 0) {
        return nullptr;
    }
    int i = posicion % n;
    if (i < 0) {
        i += n;
    }

    MedidorRecursos::sumarReferencia((unsigned long long)i + 1);
    return fotos[(unsigned int)i];
}

bool AlbumFotos::puedeVer(bool esDueno, bool esAmigo) const {
    if (privacidad == PRIVACIDAD_PUBLICA) {
        return true;
    }
    if (privacidad == PRIVACIDAD_PRIVADA) {
        return esDueno;
    }
    if (privacidad == PRIVACIDAD_AMIGOS) {
        return esDueno || esAmigo;
    }
    return false;
}

unsigned long AlbumFotos::calcularMemoria() const {
    unsigned long total = sizeof(*this) + fotos.calcularMemoria() - sizeof(fotos);
    if (nombre != nullptr) {
        total += longitud(nombre) + 1;
    }
    for (unsigned int i = 0; i < fotos.getTamanio(); i++) {
        if (fotos[i] != nullptr) {
            total += fotos[i]->calcularMemoria();
        }
    }
    return total;
}
