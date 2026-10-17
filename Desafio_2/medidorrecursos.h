#ifndef MEDIDORRECURSOS_H
#define MEDIDORRECURSOS_H

#include "configuracion.h"

class MedidorRecursos {
private:
    static MedidorRecursos * activo;
    static bool contando;
    unsigned short funcionalidadActual;
    unsigned long long iterFuncionalidad, refFuncionalidad, extFuncionalidad;
    unsigned long memoriaLocal;
    unsigned long long iterAcumuladas, refAcumuladas, extAcumulados;
    unsigned long long iterPorFunc[NUM_FUNCIONALIDADES];
    unsigned long long refPorFunc[NUM_FUNCIONALIDADES];
    unsigned long long extPorFunc[NUM_FUNCIONALIDADES];
    unsigned int ejecuciones[NUM_FUNCIONALIDADES];

public:
    MedidorRecursos();
    ~MedidorRecursos();
    MedidorRecursos(const MedidorRecursos &) = delete;
    MedidorRecursos & operator=(const MedidorRecursos &) = delete;

    static MedidorRecursos & obtener();

    static void sumarIteraciones(unsigned long long n = 1);
    static void sumarReferencia(unsigned long long n);

    static void sumarComun(unsigned long long n = 1);
    static void sumarUsoExterno(unsigned long long n = 1);
    static void sumarMemoriaLocal(unsigned long bytes);

    static void pausarConteo();
    static void reanudarConteo();

    void iniciarFuncionalidad(unsigned short idFuncionalidad);

    void finalizarFuncionalidad(unsigned long memoriaEstructuras, unsigned long memoriaReferencia);
    void mostrarResumenSesion(unsigned long memoriaEstructuras, unsigned long memoriaReferencia) const;
    void mostrarAviso(const char * texto) const;

    unsigned long long getIteraciones() const;
    unsigned long long getIteracionesAcumuladas() const;
    unsigned long long getReferencia() const;
    unsigned long long getUsosExternos() const;
    unsigned long getMemoriaLocal() const;
    unsigned short getFuncionalidadActual() const;
    static const char * nombreFuncionalidad(unsigned short idFuncionalidad);
};
#endif
