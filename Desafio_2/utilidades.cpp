#include "utilidades.h"
#include "MedidorRecursos.h"
#include "configuracion.h"

unsigned int longitud(const char * texto) {
    if (texto == nullptr) {
        return 0;
    }
    unsigned int n = 0;
    while (texto[n] != '\0') {
        n++;
    }
    MedidorRecursos::sumarComun(n);
    return n;
}

char * copiar(const char * texto) {
    unsigned int n = longitud(texto);
    char * copia = reservar<char>(n + 1);
    if (copia == nullptr) {
        return nullptr;
    }
    for (unsigned int i = 0; i < n; i++) {
        copia[i] = texto[i];
    }
    copia[n] = '\0';
    MedidorRecursos::sumarComun(n);
    return copia;
}

bool copiarEn(const char * origen, char * destino, unsigned int capacidad) {
    if (destino == nullptr || capacidad == 0) {
        return false;
    }
    unsigned int n = longitud(origen);
    if (n + 1 > capacidad) {
        return false;
    }
    for (unsigned int i = 0; i < n; i++) {
        destino[i] = origen[i];
    }
    destino[n] = '\0';
    MedidorRecursos::sumarComun(n);
    return true;
}

bool iguales(const char * a, const char * b) {
    const char * x = (a != nullptr) ? a : "";
    const char * y = (b != nullptr) ? b : "";
    unsigned int i = 0;
    while (x[i] != '\0' && x[i] == y[i]) {
        i++;
    }
    MedidorRecursos::sumarIteraciones(1);
    return x[i] == y[i];
}

int comparar(const char * a, const char * b) {
    const char * x = (a != nullptr) ? a : "";
    const char * y = (b != nullptr) ? b : "";
    unsigned int i = 0;
    while (x[i] != '\0' && x[i] == y[i]) {
        i++;
    }
    MedidorRecursos::sumarIteraciones(1);
    return (int)(unsigned char)x[i] - (int)(unsigned char)y[i];
}

bool aNumero(const char * texto, unsigned int desde, unsigned int hasta, unsigned long long & valor) {
    if (texto == nullptr || desde >= hasta) {
        return false;
    }
    const unsigned long long MAXIMO = 0xFFFFFFFFFFFFFFFFULL;
    unsigned long long resultado = 0;
    for (unsigned int i = desde; i < hasta; i++) {
        if (texto[i] < '0' || texto[i] > '9') {
            MedidorRecursos::sumarComun(i - desde + 1);
            return false;
        }
        unsigned int digito = (unsigned int)(texto[i] - '0');
        if (resultado > (MAXIMO - digito) / 10) {
            MedidorRecursos::sumarComun(i - desde + 1);
            return false;
        }
        resultado = resultado * 10 + digito;
    }
    MedidorRecursos::sumarComun(hasta - desde);
    valor = resultado;
    return true;
}

void escribirNumero(unsigned long long valor, unsigned int ancho, char * destino) {
    for (unsigned int i = ancho; i > 0; i--) {
        destino[i - 1] = (char)('0' + (int)(valor % 10));
        valor /= 10;
    }
    MedidorRecursos::sumarComun(ancho);
}

const char * descripcionResultado(unsigned short codigoResultado) {
    switch (codigoResultado) {
    case RES_OK:                    return "Operacion realizada correctamente.";
    case RES_SIN_SESION:            return "Debe iniciar sesion primero.";
    case RES_NO_EXISTE:             return "Ese usuario no existe.";
    case RES_ES_UNO_MISMO:          return "No puede hacerlo con su propio usuario.";
    case RES_YA_SON_AMIGOS:         return "Ya son amigos.";
    case RES_ELLOS_YA_ENVIARON:     return "Ese usuario ya le envio una solicitud: acepte esa en vez de enviar otra.";
    case RES_SOLICITUD_REPETIDA:    return "Ya le envio una solicitud a ese usuario y sigue pendiente.";
    case RES_NO_ES_AMIGO:           return "Ese usuario no es su amigo.";
    case RES_TEXTO_INVALIDO:        return "El mensaje no es valido: esta vacio, supera los 512 bytes o tiene saltos de linea.";
    case RES_NO_HAY_SOLICITUD:      return "No hay una solicitud pendiente de ese usuario.";
    case RES_ERROR_MEMORIA:         return "No hay memoria suficiente para completar la operacion.";
    case RES_ERROR_ARCHIVO:         return "Fallo al leer o escribir un archivo de datos.";
    case RES_YA_ACTUALIZO:          return "Ya uso el cambio de fotos: solo se permite una vez.";
    case RES_CONTRASENA_INCORRECTA: return "La contrasena no es correcta.";
    case RES_SIN_CANDIDATOS:        return "No hay amigos distintos para mostrar otro grupo de fotos.";
    default:                        return "Resultado desconocido.";
    }
}
