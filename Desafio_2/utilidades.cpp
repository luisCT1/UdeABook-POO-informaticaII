#include "utilidades.h"
#include "configuracion.h"

#include <new>

namespace util {

unsigned int longitud(const char * texto) {
    unsigned int n = 0;
    if (texto == nullptr) {
        return 0;
    }
    while (texto[n] != '\0') {
        n++;
    }
    return n;
}

bool copiar(char * destino, unsigned int capacidad, const char * origen) {
    if (destino == nullptr || capacidad == 0) {
        return false;
    }
    if (origen == nullptr) {
        destino[0] = '\0';
        return true;
    }
    unsigned int n = longitud(origen);
    if (n + 1 > capacidad) {
        destino[0] = '\0';
        return false;
    }
    for (unsigned int i = 0; i <= n; i++) {
        destino[i] = origen[i];
    }
    return true;
}

int comparar(const char * a, const char * b) {
    if (a == nullptr) {
        a = "";
    }
    if (b == nullptr) {
        b = "";
    }
    unsigned int i = 0;
    while (a[i] != '\0' && a[i] == b[i]) {
        i++;
    }
    return (int)(unsigned char)a[i] - (int)(unsigned char)b[i];
}

bool iguales(const char * a, const char * b) {
    return comparar(a, b) == 0;
}

char * duplicar(const char * origen) {
    if (origen == nullptr) {
        return nullptr;
    }
    unsigned int n = longitud(origen);
    char * copia = new (std::nothrow) char[n + 1];
    if (copia == nullptr) {
        return nullptr;
    }
    for (unsigned int i = 0; i <= n; i++) {
        copia[i] = origen[i];
    }
    return copia;
}

bool soloDigitos(const char * texto, unsigned int cantidad) {
    if (texto == nullptr) {
        return false;
    }
    for (unsigned int i = 0; i < cantidad; i++) {
        if (texto[i] < '0' || texto[i] > '9') {
            return false;
        }
    }
    return texto[cantidad] == '\0';
}

bool aEntero(const char * texto, unsigned int & valor) {
    if (texto == nullptr || texto[0] == '\0') {
        return false;
    }
    unsigned int n = 0;
    for (unsigned int i = 0; texto[i] != '\0'; i++) {
        if (texto[i] < '0' || texto[i] > '9') {
            return false;
        }
        unsigned int digito = (unsigned int)(texto[i] - '0');
        if (n > 429496729u) {
            return false;
        }
        n = n * 10 + digito;
    }
    valor = n;
    return true;
}

const char * descripcionResultado(unsigned short codigo) {
    if (codigo == cfg::RES_OK) return "listo";
    if (codigo == cfg::RES_NO_EXISTE) return "ese usuario no existe";
    if (codigo == cfg::RES_CLAVE_MALA) return "la contrasena no coincide";
    if (codigo == cfg::RES_ES_UNO_MISMO) return "no te puedes enviar eso a ti mismo";
    if (codigo == cfg::RES_YA_SON_AMIGOS) return "ya son amigos";
    if (codigo == cfg::RES_ELLOS_YA_ENVIARON) return "esa persona ya te envio una solicitud";
    if (codigo == cfg::RES_SOLICITUD_REPETIDA) return "esa solicitud ya estaba";
    if (codigo == cfg::RES_YA_ACTUALIZO) return "las fotos de amigos solo se pueden cambiar una vez";
    if (codigo == cfg::RES_NO_ES_AMIGO) return "solo puedes hacer eso con un amigo";
    if (codigo == cfg::RES_MENSAJE_INVALIDO) return "el mensaje va vacio o se pasa de 512 bytes";
    if (codigo == cfg::RES_SIN_SESION) return "no hay una sesion abierta";
    return "no se pudo completar";
}

}
