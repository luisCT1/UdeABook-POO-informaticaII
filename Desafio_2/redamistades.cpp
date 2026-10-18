#include "RedAmistades.h"
#include "MedidorRecursos.h"
#include "utilidades.h"
#include <chrono>
#include <new>

using namespace std;

bool RedAmistades::leerBit(const unsigned long long * matriz, unsigned int fila, unsigned int columna) const {
    unsigned long long palabra = matriz[(unsigned long long)fila * palabrasPorFila + columna / BITS_POR_PALABRA];
    return ((palabra >> (columna % BITS_POR_PALABRA)) & 1ULL) != 0;
}

void RedAmistades::escribirBit(unsigned long long * matriz, unsigned int fila, unsigned int columna, bool valor) {
    unsigned long long * palabra = &matriz[(unsigned long long)fila * palabrasPorFila + columna / BITS_POR_PALABRA];
    unsigned long long mascara = 1ULL << (columna % BITS_POR_PALABRA);
    if (valor) {
        *palabra |= mascara;
    } else {
        *palabra &= ~mascara;
    }
}

unsigned int RedAmistades::contarBits(unsigned long long palabra) const {
    unsigned int cuantos = 0;
    while (palabra != 0) {
        palabra &= (palabra - 1);
        cuantos++;
    }
    MedidorRecursos::sumarIteraciones(cuantos);
    return cuantos;
}

unsigned int RedAmistades::extraerIndices(const unsigned long long * matriz, unsigned int fila,
                                          unsigned int * salida, unsigned int capacidad) const {
    unsigned int encontrados = 0;
    unsigned long long vueltas = 0;
    const unsigned long long * inicio = &matriz[(unsigned long long)fila * palabrasPorFila];
    for (unsigned int p = 0; p < palabrasPorFila && encontrados < capacidad; p++) {
        vueltas++;
        unsigned long long palabra = inicio[p];
        unsigned int posicion = 0;
        while (palabra != 0 && encontrados < capacidad) {
            vueltas++;
            if ((palabra & 1ULL) != 0) {
                salida[encontrados++] = p * BITS_POR_PALABRA + posicion;
            }
            palabra >>= 1;
            posicion++;
        }
    }
    MedidorRecursos::sumarIteraciones(vueltas);
    return encontrados;
}

float RedAmistades::calcularPeso(unsigned int amigosEnComun) const {
    if (amigosEnComun <= UMBRAL_PESO_MEDIO) {
        return PESO_BASE;
    }
    if (amigosEnComun <= UMBRAL_PESO_ALTO) {
        return PESO_MEDIO;
    }
    return PESO_ALTO;
}

bool RedAmistades::agregarEnLista(unsigned int u, unsigned int v) {
    unsigned int usados = grados[u];
    if (listasAmigos[u] == nullptr) {
        capListas[u] = 4;
        listasAmigos[u] = reservar<unsigned int>(capListas[u]);
        if (listasAmigos[u] == nullptr) {
            return false;
        }
    }
    if (usados >= capListas[u]) {
        unsigned int nuevaCap = capListas[u] * 2;
        if (nuevaCap < usados + 1) {
            nuevaCap = usados + 1;
        }
        unsigned int * nuevo = reservar<unsigned int>(nuevaCap);
        if (nuevo == nullptr) {
            return false;
        }
        for (unsigned int i = 0; i < usados; i++) {
            nuevo[i] = listasAmigos[u][i];
        }
        delete[] listasAmigos[u];
        listasAmigos[u] = nuevo;
        capListas[u] = nuevaCap;
    }
    listasAmigos[u][usados] = v;
    return true;
}

void RedAmistades::liberar() {
    if (listasAmigos != nullptr) {
        for (unsigned int i = 0; i < numUsuarios; i++) {
            delete[] listasAmigos[i];
        }
    }
    delete[] listasAmigos;
    delete[] capListas;
    delete[] bitsAmistad;
    delete[] bitsSolicitud;
    delete[] grados;
    delete[] pendientes;
    listasAmigos = nullptr;
    capListas = nullptr;
    bitsAmistad = nullptr;
    bitsSolicitud = nullptr;
    grados = nullptr;
    pendientes = nullptr;
}

RedAmistades::RedAmistades()
    : bitsAmistad(nullptr), bitsSolicitud(nullptr), grados(nullptr), pendientes(nullptr),
      listasAmigos(nullptr), capListas(nullptr),
      numUsuarios(0), palabrasPorFila(0), valido(true) {
    generador.seed((unsigned int)chrono::steady_clock::now().time_since_epoch().count());
}

RedAmistades::RedAmistades(unsigned int n)
    : bitsAmistad(nullptr), bitsSolicitud(nullptr), grados(nullptr), pendientes(nullptr),
      listasAmigos(nullptr), capListas(nullptr),
      numUsuarios(n), palabrasPorFila((n + BITS_POR_PALABRA - 1) / BITS_POR_PALABRA), valido(true) {
    generador.seed((unsigned int)chrono::steady_clock::now().time_since_epoch().count());
    if (n == 0) {
        return;
    }
    unsigned long long total = (unsigned long long)n * palabrasPorFila;
    if (total > 0xFFFFFFFFULL) {
        std::cerr << "ERROR, La red de amistades es demasiado grande para " << n << " usuarios.\n";
        numUsuarios = 0;
        palabrasPorFila = 0;
        valido = false;
        return;
    }
    bitsAmistad = reservar<unsigned long long>((unsigned int)total);
    bitsSolicitud = reservar<unsigned long long>((unsigned int)total);
    grados = reservar<unsigned int>(n);
    pendientes = reservar<unsigned int>(n);
    listasAmigos = new (std::nothrow) unsigned int *[n];
    capListas = reservar<unsigned int>(n);
    if (bitsAmistad == nullptr || bitsSolicitud == nullptr || grados == nullptr
        || pendientes == nullptr || listasAmigos == nullptr || capListas == nullptr) {
        liberar();
        numUsuarios = 0;
        palabrasPorFila = 0;
        valido = false;
        return;
    }
    for (unsigned long long i = 0; i < total; i++) {
        bitsAmistad[i] = 0;
        bitsSolicitud[i] = 0;
    }
    for (unsigned int i = 0; i < n; i++) {
        grados[i] = 0;
        pendientes[i] = 0;
        listasAmigos[i] = nullptr;
        capListas[i] = 0;
    }
    MedidorRecursos::sumarIteraciones(total + n);
}

RedAmistades::RedAmistades(const RedAmistades & otra)
    : bitsAmistad(nullptr), bitsSolicitud(nullptr), grados(nullptr), pendientes(nullptr),
      listasAmigos(nullptr), capListas(nullptr),
      numUsuarios(0), palabrasPorFila(0), generador(otra.generador), valido(true) {
    if (!otra.valido) {
        valido = false;
        return;
    }
    if (otra.numUsuarios == 0) {
        return;
    }
    *this = otra;
}

RedAmistades::~RedAmistades() {
    liberar();
}

RedAmistades & RedAmistades::operator=(const RedAmistades & otra) {
    if (this == &otra || !otra.valido) {
        return *this;
    }
    unsigned long long * nuevasAmistad = nullptr;
    unsigned long long * nuevasSolicitud = nullptr;
    unsigned int * nuevosGrados = nullptr;
    unsigned int * nuevosPendientes = nullptr;
    unsigned int ** nuevasListas = nullptr;
    unsigned int * nuevasCaps = nullptr;
    unsigned long long total = (unsigned long long)otra.numUsuarios * otra.palabrasPorFila;
    if (otra.numUsuarios > 0) {
        nuevasAmistad = reservar<unsigned long long>((unsigned int)total);
        nuevasSolicitud = reservar<unsigned long long>((unsigned int)total);
        nuevosGrados = reservar<unsigned int>(otra.numUsuarios);
        nuevosPendientes = reservar<unsigned int>(otra.numUsuarios);
        nuevasListas = new (std::nothrow) unsigned int *[otra.numUsuarios];
        nuevasCaps = reservar<unsigned int>(otra.numUsuarios);
        if (nuevasAmistad == nullptr || nuevasSolicitud == nullptr || nuevosGrados == nullptr
            || nuevosPendientes == nullptr || nuevasListas == nullptr || nuevasCaps == nullptr) {
            delete[] nuevasAmistad;
            delete[] nuevasSolicitud;
            delete[] nuevosGrados;
            delete[] nuevosPendientes;
            delete[] nuevasListas;
            delete[] nuevasCaps;
            return *this;
        }
        for (unsigned int i = 0; i < otra.numUsuarios; i++) {
            nuevasListas[i] = nullptr;
            nuevasCaps[i] = 0;
        }
        for (unsigned long long i = 0; i < total; i++) {
            nuevasAmistad[i] = otra.bitsAmistad[i];
            nuevasSolicitud[i] = otra.bitsSolicitud[i];
        }
        for (unsigned int i = 0; i < otra.numUsuarios; i++) {
            nuevosGrados[i] = otra.grados[i];
            nuevosPendientes[i] = otra.pendientes[i];
            nuevasCaps[i] = otra.capListas[i];
            if (otra.listasAmigos[i] != nullptr && otra.grados[i] > 0) {
                nuevasListas[i] = reservar<unsigned int>(otra.capListas[i] > 0 ? otra.capListas[i] : otra.grados[i]);
                if (nuevasListas[i] == nullptr) {
                    for (unsigned int j = 0; j < i; j++) {
                        delete[] nuevasListas[j];
                    }
                    delete[] nuevasAmistad;
                    delete[] nuevasSolicitud;
                    delete[] nuevosGrados;
                    delete[] nuevosPendientes;
                    delete[] nuevasListas;
                    delete[] nuevasCaps;
                    return *this;
                }
                for (unsigned int j = 0; j < otra.grados[i]; j++) {
                    nuevasListas[i][j] = otra.listasAmigos[i][j];
                }
            }
        }
        MedidorRecursos::sumarIteraciones(total + otra.numUsuarios);
    }
    liberar();
    bitsAmistad = nuevasAmistad;
    bitsSolicitud = nuevasSolicitud;
    grados = nuevosGrados;
    pendientes = nuevosPendientes;
    listasAmigos = nuevasListas;
    capListas = nuevasCaps;
    numUsuarios = otra.numUsuarios;
    palabrasPorFila = otra.palabrasPorFila;
    generador = otra.generador;
    valido = true;
    return *this;
}

bool RedAmistades::estaValido() const { return valido; }
unsigned int RedAmistades::getNumUsuarios() const { return numUsuarios; }
void RedAmistades::setSemilla(unsigned int semilla) { generador.seed(semilla); }

bool RedAmistades::conectar(unsigned int a, unsigned int b) {
    if (a >= numUsuarios || b >= numUsuarios || a == b) {
        return false;
    }
    if (leerBit(bitsAmistad, a, b)) {
        return false;
    }
    if (!agregarEnLista(a, b) || !agregarEnLista(b, a)) {
        return false;
    }
    escribirBit(bitsAmistad, a, b, true);
    escribirBit(bitsAmistad, b, a, true);
    grados[a]++;
    grados[b]++;
    return true;
}

bool RedAmistades::sonAmigos(unsigned int a, unsigned int b) const {
    if (a >= numUsuarios || b >= numUsuarios) {
        return false;
    }
    return leerBit(bitsAmistad, a, b);
}

unsigned int RedAmistades::getGrado(unsigned int a) const {
    return (a < numUsuarios) ? grados[a] : 0;
}

unsigned int RedAmistades::contarEnComun(unsigned int a, unsigned int b) const {
    if (a >= numUsuarios || b >= numUsuarios) {
        return 0;
    }
    const unsigned long long * filaA = &bitsAmistad[(unsigned long long)a * palabrasPorFila];
    const unsigned long long * filaB = &bitsAmistad[(unsigned long long)b * palabrasPorFila];
    unsigned int total = 0;
    for (unsigned int p = 0; p < palabrasPorFila; p++) {
        total += contarBits(filaA[p] & filaB[p]);
    }
    MedidorRecursos::sumarIteraciones(palabrasPorFila);
    MedidorRecursos::sumarReferencia(numUsuarios);
    return total;
}

unsigned int RedAmistades::obtenerAmigos(unsigned int a, unsigned int * salida, unsigned int capacidad) const {
    if (a >= numUsuarios || salida == nullptr || capacidad == 0) {
        return 0;
    }
    unsigned int n = grados[a];
    if (n > capacidad) {
        n = capacidad;
    }
    MedidorRecursos::sumarReferencia(n);
    MedidorRecursos::sumarIteraciones(n);
    if (listasAmigos[a] == nullptr) {
        return 0;
    }
    for (unsigned int i = 0; i < n; i++) {
        salida[i] = listasAmigos[a][i];
    }
    return n;
}

unsigned short RedAmistades::enviarSolicitud(unsigned int emisor, unsigned int receptor) {
    if (emisor >= numUsuarios || receptor >= numUsuarios) {
        return RES_NO_EXISTE;
    }
    if (emisor == receptor) {
        return RES_ES_UNO_MISMO;
    }
    MedidorRecursos::sumarReferencia((unsigned long long)grados[emisor] + pendientes[emisor] + pendientes[receptor]);
    if (leerBit(bitsAmistad, emisor, receptor)) {
        return RES_YA_SON_AMIGOS;
    }
    if (leerBit(bitsSolicitud, emisor, receptor)) {
        return RES_ELLOS_YA_ENVIARON;
    }
    if (leerBit(bitsSolicitud, receptor, emisor)) {
        return RES_SOLICITUD_REPETIDA;
    }
    escribirBit(bitsSolicitud, receptor, emisor, true);
    pendientes[receptor]++;
    return RES_OK;
}

bool RedAmistades::tieneSolicitud(unsigned int emisor, unsigned int receptor) const {
    if (emisor >= numUsuarios || receptor >= numUsuarios) {
        return false;
    }
    return leerBit(bitsSolicitud, receptor, emisor);
}

unsigned int RedAmistades::contarSolicitudes(unsigned int receptor) const {
    return (receptor < numUsuarios) ? pendientes[receptor] : 0;
}

unsigned int RedAmistades::obtenerSolicitudes(unsigned int receptor, unsigned int * emisores, unsigned int capacidad) const {
    if (receptor >= numUsuarios) {
        return 0;
    }
    MedidorRecursos::sumarReferencia(pendientes[receptor]);
    return extraerIndices(bitsSolicitud, receptor, emisores, capacidad);
}

bool RedAmistades::aceptarSolicitud(unsigned int emisor, unsigned int receptor) {
    if (!tieneSolicitud(emisor, receptor)) {
        return false;
    }
    escribirBit(bitsSolicitud, receptor, emisor, false);
    pendientes[receptor]--;
    return conectar(emisor, receptor);
}
void RedAmistades::marcarSolicitud(unsigned int emisor, unsigned int receptor) {
    if (emisor >= numUsuarios || receptor >= numUsuarios || emisor == receptor) {
        return;
    }
    if (!leerBit(bitsSolicitud, receptor, emisor)) {
        escribirBit(bitsSolicitud, receptor, emisor, true);
        pendientes[receptor]++;
    }
}

unsigned short RedAmistades::sortearAmigos(unsigned int usuario, unsigned short cuantos,
                                           const unsigned int * excluidos, unsigned short numExcluidos,
                                           unsigned int * salida) {
    if (usuario >= numUsuarios || cuantos == 0 || grados[usuario] == 0) {
        return 0;
    }
    unsigned int totalAmigos = grados[usuario];
    unsigned int * amigos = reservar<unsigned int>(totalAmigos);
    float * pesos = reservar<float>(totalAmigos);
    if (amigos == nullptr || pesos == nullptr) {
        delete[] amigos;
        delete[] pesos;
        return 0;
    }

    MedidorRecursos::sumarMemoriaLocal((unsigned long)totalAmigos * (sizeof(unsigned int) + sizeof(float)));

    unsigned int n = obtenerAmigos(usuario, amigos, totalAmigos);
    unsigned long long vueltas = 0;
    for (unsigned int i = 0; i < n; i++) {
        bool excluido = false;
        for (unsigned short e = 0; e < numExcluidos; e++) {
            vueltas++;
            if (excluidos[e] == amigos[i]) {
                excluido = true;
            }
        }
        pesos[i] = excluido ? 0.0f : calcularPeso(contarEnComun(usuario, amigos[i]));
        vueltas++;
    }
    unsigned short sorteados = 0;
    for (unsigned short t = 0; t < cuantos; t++) {
        float suma = 0.0f;
        for (unsigned int i = 0; i < n; i++) {
            suma += pesos[i];
            vueltas++;
        }
        if (suma <= 0.0f) {
            break;
        }
        uniform_real_distribution<float> distribucion(0.0f, suma);
        float punto = distribucion(generador);
        MedidorRecursos::sumarUsoExterno();
        float acumulado = 0.0f;
        unsigned int elegido = n;
        for (unsigned int i = 0; i < n; i++) {
            vueltas++;
            if (pesos[i] > 0.0f) {
                acumulado += pesos[i];
                elegido = i;
                if (punto < acumulado) {
                    break;
                }
            }
        }
        salida[sorteados++] = amigos[elegido];
        pesos[elegido] = 0.0f;
    }
    MedidorRecursos::sumarIteraciones(vueltas);
    delete[] amigos;
    delete[] pesos;
    return sorteados;
}

unsigned long RedAmistades::calcularMemoria() const {
    unsigned long long total = (unsigned long long)numUsuarios * palabrasPorFila;
    unsigned long long bytes = sizeof(*this) + 2 * total * sizeof(unsigned long long)
        + 2ULL * numUsuarios * sizeof(unsigned int)
        + (unsigned long long)numUsuarios * sizeof(unsigned int *)
        + (unsigned long long)numUsuarios * sizeof(unsigned int);
    for (unsigned int i = 0; i < numUsuarios; i++) {
        if (listasAmigos != nullptr && listasAmigos[i] != nullptr) {
            bytes += (unsigned long long)capListas[i] * sizeof(unsigned int);
        }
    }
    return (unsigned long)bytes;
}

unsigned long RedAmistades::calcularMemoriaReferencia() const {
    return (unsigned long)(2ULL * numUsuarios * numUsuarios);
}
