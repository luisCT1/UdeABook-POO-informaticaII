#include "Usuario.h"
#include "AlbumFotos.h"
#include "Mensaje.h"
#include "utilidades.h"

namespace {

bool ponerTexto(char * & destino, const char * origen) {
    char * copia = util::duplicar(origen == nullptr ? "" : origen);
    if (copia == nullptr) {
        return false;
    }
    delete[] destino;
    destino = copia;
    return true;
}

int compararDia(const FechaHora & a, const FechaHora & b) {
    if (a.getAnio() != b.getAnio()) return a.getAnio() < b.getAnio() ? -1 : 1;
    if (a.getMes() != b.getMes()) return a.getMes() < b.getMes() ? -1 : 1;
    if (a.getDia() != b.getDia()) return a.getDia() < b.getDia() ? -1 : 1;
    return 0;
}

}

Usuario::Usuario()
    : nombreUsuario(nullptr), correo(nullptr), contrasena(nullptr), ciudad(nullptr), pais(nullptr),
      indice(0), primerNoLeido(0), valido(false) {
}

Usuario::Usuario(const char * nombreUsuario, const char * contrasena,
                 const FechaHora & fechaNacimiento, const FechaHora & fechaInscripcion, unsigned int indice)
    : nombreUsuario(nullptr), fechaNacimiento(fechaNacimiento), correo(nullptr), contrasena(nullptr),
      ciudad(nullptr), pais(nullptr), fechaInscripcion(fechaInscripcion),
      indice(indice), primerNoLeido(0), valido(true) {
    this->nombreUsuario = util::duplicar(nombreUsuario == nullptr ? "" : nombreUsuario);
    this->contrasena = util::duplicar(contrasena == nullptr ? "" : contrasena);
    if (this->nombreUsuario == nullptr || this->contrasena == nullptr || this->nombreUsuario[0] == '\0') {
        valido = false;
    }
}

Usuario::~Usuario() {
    for (unsigned int i = 0; i < albumes.getTamanio(); i++) {
        delete albumes[i];
    }
    for (unsigned int i = 0; i < mensajesRecibidos.getTamanio(); i++) {
        delete mensajesRecibidos[i];
    }
    delete[] nombreUsuario;
    delete[] correo;
    delete[] contrasena;
    delete[] ciudad;
    delete[] pais;
}

bool Usuario::estaValido() const { return valido; }

const char * Usuario::getNombreUsuario() const { return nombreUsuario; }
const FechaHora & Usuario::getFechaNacimiento() const { return fechaNacimiento; }
const char * Usuario::getCorreo() const { return correo; }
const char * Usuario::getCiudad() const { return ciudad; }
const char * Usuario::getPais() const { return pais; }
const FechaHora & Usuario::getFechaInscripcion() const { return fechaInscripcion; }
unsigned int Usuario::getIndice() const { return indice; }
unsigned int Usuario::getPrimerNoLeido() const { return primerNoLeido; }
ListaDinamica<AlbumFotos *> & Usuario::getAlbumes() { return albumes; }
ListaDinamica<Mensaje *> & Usuario::getMensajesRecibidos() { return mensajesRecibidos; }
ListaDinamica<Mensaje *> & Usuario::getMensajesEnviados() { return mensajesEnviados; }

bool Usuario::setCorreo(const char * valor) { return ponerTexto(correo, valor); }
bool Usuario::setContrasena(const char * valor) { return ponerTexto(contrasena, valor); }
bool Usuario::setCiudad(const char * valor) { return ponerTexto(ciudad, valor); }
bool Usuario::setPais(const char * valor) { return ponerTexto(pais, valor); }
void Usuario::setPrimerNoLeido(unsigned int posicion) { primerNoLeido = posicion; }

bool Usuario::verificarContrasena(const char * intento) const {
    return util::iguales(contrasena, intento);
}

bool Usuario::agregarAlbum(AlbumFotos * album) {
    if (album == nullptr) {
        return false;
    }
    return albumes.agregar(album);
}

unsigned int Usuario::getNumAlbumes() const {
    return albumes.getTamanio();
}

AlbumFotos * Usuario::buscarAlbum(const char * nombre) const {
    for (unsigned int i = 0; i < albumes.getTamanio(); i++) {
        if (albumes[i] != nullptr && util::iguales(albumes[i]->getNombre(), nombre)) {
            return albumes[i];
        }
    }
    return nullptr;
}

AlbumFotos * Usuario::buscarAlbum(unsigned int posicion) const {
    if (posicion >= albumes.getTamanio()) {
        return nullptr;
    }
    return albumes[posicion];
}

bool Usuario::recibirMensaje(Mensaje * mensaje) {
    if (mensaje == nullptr) {
        return false;
    }
    return mensajesRecibidos.agregar(mensaje);
}

bool Usuario::registrarEnviado(Mensaje * mensaje) {
    if (mensaje == nullptr) {
        return false;
    }
    return mensajesEnviados.agregar(mensaje);
}

unsigned int Usuario::contarNoLeidos() const {
    unsigned int n = mensajesRecibidos.getTamanio();
    if (primerNoLeido >= n) {
        return 0;
    }
    return n - primerNoLeido;
}

unsigned int Usuario::marcarTodosLeidos() {
    unsigned int n = mensajesRecibidos.getTamanio();
    for (unsigned int i = primerNoLeido; i < n; i++) {
        if (mensajesRecibidos[i] != nullptr) {
            mensajesRecibidos[i]->setLeido(true);
        }
    }
    primerNoLeido = n;
    return primerNoLeido;
}

bool Usuario::buscarRangoMensajes(const FechaHora & inicio, const FechaHora & fin,
                                  unsigned int & desde, unsigned int & hasta) const {
    unsigned int n = mensajesRecibidos.getTamanio();
    unsigned int lo = 0;
    unsigned int hi = n;
    while (lo < hi) {
        unsigned int medio = lo + (hi - lo) / 2;
        Mensaje * m = mensajesRecibidos[medio];
        if (m != nullptr && compararDia(m->getFechaEnvio(), inicio) < 0) {
            lo = medio + 1;
        } else {
            hi = medio;
        }
    }
    desde = lo;
    lo = 0;
    hi = n;
    while (lo < hi) {
        unsigned int medio = lo + (hi - lo) / 2;
        Mensaje * m = mensajesRecibidos[medio];
        if (m != nullptr && compararDia(m->getFechaEnvio(), fin) <= 0) {
            lo = medio + 1;
        } else {
            hi = medio;
        }
    }
    hasta = lo;
    return desde < hasta;
}

bool Usuario::operator<(const Usuario & otro) const {
    return util::comparar(nombreUsuario, otro.nombreUsuario) < 0;
}

bool Usuario::operator==(const Usuario & otro) const {
    return util::iguales(nombreUsuario, otro.nombreUsuario);
}

unsigned long Usuario::calcularMemoria() const {
    unsigned long total = sizeof(*this);
    total += albumes.calcularMemoria();
    total += mensajesRecibidos.calcularMemoria();
    total += mensajesEnviados.calcularMemoria();
    if (nombreUsuario != nullptr) total += util::longitud(nombreUsuario) + 1;
    if (correo != nullptr) total += util::longitud(correo) + 1;
    if (contrasena != nullptr) total += util::longitud(contrasena) + 1;
    if (ciudad != nullptr) total += util::longitud(ciudad) + 1;
    if (pais != nullptr) total += util::longitud(pais) + 1;
    for (unsigned int i = 0; i < albumes.getTamanio(); i++) {
        if (albumes[i] != nullptr) {
            total += albumes[i]->calcularMemoria();
        }
    }
    for (unsigned int i = 0; i < mensajesRecibidos.getTamanio(); i++) {
        if (mensajesRecibidos[i] != nullptr) {
            total += mensajesRecibidos[i]->calcularMemoria();
        }
    }
    return total;
}
