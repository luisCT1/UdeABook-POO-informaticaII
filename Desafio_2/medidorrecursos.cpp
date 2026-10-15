#include "MedidorRecursos.h"
#include <iostream>
#include <iomanip>

using namespace std;

static const char * const NOMBRES[NUM_FUNCIONALIDADES] = {
    "Carga de datos",
    "Ingreso y mejores amigos",
    "Enviar solicitud de amistad",
    "Aceptar solicitud de amistad",
    "Visitar perfil de un amigo",
    "Sugerencias de amistad",
    "Enviar mensaje",
    "Leer mensajes nuevos",
    "Buscar mensajes por fechas",
    "Actualizar fotos de amigos"
};


static void formatear(unsigned long long valor, char * destino) {
    char digitos[24];
    int cuantos = 0;
    if (valor == 0) {
        digitos[cuantos++] = '0';
    }
    while (valor > 0) {
        digitos[cuantos++] = (char)('0' + (int)(valor % 10));
        valor /= 10;
    }
    int posicion = 0;
    for (int i = cuantos - 1; i >= 0; i--) {
        destino[posicion++] = digitos[i];
        if (i > 0 && i % 3 == 0) {
            destino[posicion++] = ',';
        }
    }
    destino[posicion] = '\0';
}


static void etiqueta(const char * texto) {
    cout << " " << left << setfill('.') << setw(42) << texto << setfill(' ') << " ";
}
static void imprimirComparacion(unsigned long long real, unsigned long long base) {
    if (real == 0) {
        cout << "  (no hubo conteo que comparar)";
        return;
    }
    unsigned long long decimas = (base * 10) / real;
    if (base >= real) {
        cout << "  ->  " << (decimas / 10) << "." << (decimas % 10) << " veces menos";
    } else {
        unsigned long long inversa = (real * 10) / (base == 0 ? 1 : base);
        cout << "  ->  " << (inversa / 10) << "." << (inversa % 10) << " veces MAS que la linea base";
    }
}

static void linea(const char * texto, unsigned long long valor) {
    char numero[32];
    formatear(valor, numero);
    etiqueta(texto);
    cout << numero;
}

MedidorRecursos * MedidorRecursos::activo = nullptr;

MedidorRecursos::MedidorRecursos()
    : funcionalidadActual(FUNC_CARGA),
    iterFuncionalidad(0), refFuncionalidad(0), extFuncionalidad(0), memoriaLocal(0),
    iterAcumuladas(0), refAcumuladas(0), extAcumulados(0) {
    for (unsigned short i = 0; i < NUM_FUNCIONALIDADES; i++) {
        iterPorFunc[i] = 0;
        refPorFunc[i] = 0;
        extPorFunc[i] = 0;
        ejecuciones[i] = 0;
    }
    activo = this;
}

MedidorRecursos::~MedidorRecursos() {
    if (activo == this) {
        activo = nullptr;
    }
}

MedidorRecursos & MedidorRecursos::obtener() {
    if (activo != nullptr) {
        return *activo;
    }
    static MedidorRecursos respaldo;
    return respaldo;
}

void MedidorRecursos::sumarIteraciones(unsigned long long n) {
    MedidorRecursos & m = obtener();
    m.iterFuncionalidad += n;
    m.iterPorFunc[m.funcionalidadActual] += n;
    if (m.funcionalidadActual != FUNC_CARGA) {
        m.iterAcumuladas += n;
    }
}

void MedidorRecursos::sumarReferencia(unsigned long long n) {
    MedidorRecursos & m = obtener();
    m.refFuncionalidad += n;
    m.refPorFunc[m.funcionalidadActual] += n;
    if (m.funcionalidadActual != FUNC_CARGA) {
        m.refAcumuladas += n;
    }
}

void MedidorRecursos::sumarComun(unsigned long long n) {
    sumarIteraciones(n);
    sumarReferencia(n);
}

void MedidorRecursos::sumarUsoExterno(unsigned long long n) {
    MedidorRecursos & m = obtener();
    m.extFuncionalidad += n;
    m.extPorFunc[m.funcionalidadActual] += n;
    if (m.funcionalidadActual != FUNC_CARGA) {
        m.extAcumulados += n;
    }
}

void MedidorRecursos::sumarMemoriaLocal(unsigned long bytes) {
    obtener().memoriaLocal += bytes;
}

void MedidorRecursos::iniciarFuncionalidad(unsigned short idFuncionalidad) {
    funcionalidadActual = (idFuncionalidad < NUM_FUNCIONALIDADES) ? idFuncionalidad : FUNC_CARGA;
    iterFuncionalidad = 0;
    refFuncionalidad = 0;
    extFuncionalidad = 0;
    memoriaLocal = 0;
}

void MedidorRecursos::finalizarFuncionalidad(unsigned long memoriaEstructuras, unsigned long memoriaReferencia) {
    ejecuciones[funcionalidadActual]++;
    unsigned long long memoriaTotal = (unsigned long long)memoriaEstructuras + memoriaLocal;
    char numero[32];

    cout << "\n...................... REPORTE: " << nombreFuncionalidad(funcionalidadActual) << " .........................\n";
    linea("Iteraciones de esta funcionalidad", iterFuncionalidad);
    cout << "\n";
    linea("Iteraciones acumuladas de la sesion", iterAcumuladas);
    cout << "\n";
    linea("Linea base ingenua (misma tarea)", refFuncionalidad);
    imprimirComparacion(iterFuncionalidad, refFuncionalidad);
    cout << "\n";
    linea("Usos de componentes externos", extFuncionalidad);
    cout << "\n";
    linea("Memoria consumida (bytes)", memoriaTotal);
    formatear(memoriaEstructuras, numero);
    cout << "   (estructuras " << numero << " + locales ";
    formatear(memoriaLocal, numero);
    cout << numero << ")\n";
    linea("Memoria linea base (bytes)", memoriaReferencia);
    imprimirComparacion(memoriaTotal, memoriaReferencia);
    cout << "\n..........................................................\n";
}

void MedidorRecursos::mostrarResumenSesion(unsigned long memoriaEstructuras, unsigned long memoriaReferencia) const {
    char numero[32];
    cout << "\n............. RESUMEN DE LA SESION ...............\n";
    cout << left << setw(32) << " Funcionalidad" << right << setw(7) << "Veces"
         << setw(14) << "Iteraciones" << setw(16) << "Linea base" << setw(10) << "Externos" << "\n";
    for (unsigned short id = 1; id < NUM_FUNCIONALIDADES; id++) {
        if (ejecuciones[id] == 0) {
            continue;
        }
        cout << " " << left << setw(31) << NOMBRES[id] << right << setw(7) << ejecuciones[id];
        formatear(iterPorFunc[id], numero);
        cout << setw(14) << numero;
        formatear(refPorFunc[id], numero);
        cout << setw(16) << numero;
        formatear(extPorFunc[id], numero);
        cout << setw(10) << numero << "\n";
    }
    cout << left;
    linea("TOTAL iteraciones de la sesion", iterAcumuladas);
    cout << "\n";
    linea("TOTAL linea base ingenua", refAcumuladas);
    imprimirComparacion(iterAcumuladas, refAcumuladas);
    cout << "\n";
    linea("Memoria de las estructuras (bytes)", memoriaEstructuras);
    cout << "\n";
    linea("Memoria linea base (bytes)", memoriaReferencia);
    imprimirComparacion(memoriaEstructuras, memoriaReferencia);
    cout << "\n.................................................\n";
}
void MedidorRecursos::mostrarAviso(const char * texto) const {
    cout << "AVISO " << texto << "\n";
}

unsigned long long MedidorRecursos::getIteraciones() const { return iterFuncionalidad; }
unsigned long long MedidorRecursos::getIteracionesAcumuladas() const { return iterAcumuladas; }
unsigned long long MedidorRecursos::getReferencia() const { return refFuncionalidad; }
unsigned long long MedidorRecursos::getUsosExternos() const { return extFuncionalidad; }
unsigned long MedidorRecursos::getMemoriaLocal() const { return memoriaLocal; }
unsigned short MedidorRecursos::getFuncionalidadActual() const { return funcionalidadActual; }

const char * MedidorRecursos::nombreFuncionalidad(unsigned short idFuncionalidad) {
    if (idFuncionalidad < NUM_FUNCIONALIDADES) {
        return NOMBRES[idFuncionalidad];
    }
    return "Desconocida";
}