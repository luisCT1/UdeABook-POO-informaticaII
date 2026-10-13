#ifndef MENSAJE_H
#define MENSAJE_H

#include "FechaHora.h"

//el que recibe el mensaje es quien lo borra. el emisor solo guarda el puntero

class Usuario;

class Mensaje {
private:
    char * texto;
    FechaHora fechaEnvio;
    bool leido;
    Usuario * emisor;
    Usuario * receptor;
    bool valido;

public:
    Mensaje();
    Mensaje(Usuario * emisor, Usuario * receptor, const char * texto, const FechaHora & fechaEnvio);
    Mensaje(const Mensaje & otra);
    ~Mensaje();

    bool estaValido() const;
    static bool textoValido(const char * texto);

    const char * getTexto() const;
    const FechaHora & getFechaEnvio() const;
    bool isLeido() const;
    Usuario * getEmisor() const;
    Usuario * getReceptor() const;
    void setLeido(bool leido);

    bool operator<(const Mensaje & otro) const;
    Mensaje & operator=(const Mensaje & otro);
    unsigned long calcularMemoria() const;
};

#endif
