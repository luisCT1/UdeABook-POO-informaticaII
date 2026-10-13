#include "Mensaje.h"
#include "configuracion.h"
#include "utilidades.h"

Mensaje::Mensaje()
    : texto(nullptr), leido(false), emisor(nullptr), receptor(nullptr), valido(true) {
}

Mensaje::Mensaje(Usuario * emisor, Usuario * receptor, const char * texto, const FechaHora & fechaEnvio)
    : texto(nullptr), fechaEnvio(fechaEnvio), leido(false),
      emisor(emisor), receptor(receptor), valido(true) {
    this->texto = util::duplicar(texto == nullptr ? "" : texto);
    if (this->texto == nullptr || !textoValido(this->texto)) {
        valido = false;
    }
}

Mensaje::Mensaje(const Mensaje & otra)
    : texto(nullptr), fechaEnvio(otra.fechaEnvio), leido(otra.leido),
      emisor(otra.emisor), receptor(otra.receptor), valido(otra.valido) {
    texto = util::duplicar(otra.texto == nullptr ? "" : otra.texto);
    if (texto == nullptr) {
        valido = false;
    }
}

Mensaje::~Mensaje() {
    delete[] texto;
}

bool Mensaje::estaValido() const { return valido; }

bool Mensaje::textoValido(const char * texto) {
    if (texto == nullptr || texto[0] == '\0') {
        return false;
    }
    //una enie o una tilde ocupan 2 bytes, por eso el tope es en bytes y no en letras
    return util::longitud(texto) <= cfg::MAX_BYTES_MENSAJE;
}

const char * Mensaje::getTexto() const { return texto; }
const FechaHora & Mensaje::getFechaEnvio() const { return fechaEnvio; }
bool Mensaje::isLeido() const { return leido; }
Usuario * Mensaje::getEmisor() const { return emisor; }
Usuario * Mensaje::getReceptor() const { return receptor; }
void Mensaje::setLeido(bool valor) { leido = valor; }

bool Mensaje::operator<(const Mensaje & otro) const {
    return fechaEnvio < otro.fechaEnvio;
}

Mensaje & Mensaje::operator=(const Mensaje & otro) {
    if (this == &otro) {
        return *this;
    }
    char * copia = util::duplicar(otro.texto == nullptr ? "" : otro.texto);
    if (copia == nullptr) {
        valido = false;
        return *this;
    }
    delete[] texto;
    texto = copia;
    fechaEnvio = otro.fechaEnvio;
    leido = otro.leido;
    emisor = otro.emisor;
    receptor = otro.receptor;
    valido = otro.valido;
    return *this;
}

unsigned long Mensaje::calcularMemoria() const {
    unsigned long extra = 0;
    if (texto != nullptr) {
        extra = util::longitud(texto) + 1;
    }
    return sizeof(*this) + extra;
}
