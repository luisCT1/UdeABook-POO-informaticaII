#ifndef USUARIO_H
#define USUARIO_H

#include "FechaHora.h"
#include "ListaDinamica.h"

//persona de la red. borra sus albumes y los mensajes que recibio, no los que envio

class AlbumFotos;
class Mensaje;

class Usuario {
private:
    char * nombreUsuario;
    FechaHora fechaNacimiento;
    char * correo;
    char * contrasena;
    char * ciudad;
    char * pais;
    FechaHora fechaInscripcion;
    unsigned int indice;
    unsigned int primerNoLeido;
    ListaDinamica<AlbumFotos *> albumes;
    ListaDinamica<Mensaje *> mensajesRecibidos;
    ListaDinamica<Mensaje *> mensajesEnviados;
    bool valido;

public:
    Usuario();
    Usuario(const char * nombreUsuario, const char * contrasena,
            const FechaHora & fechaNacimiento, const FechaHora & fechaInscripcion, unsigned int indice);
    ~Usuario();
    Usuario(const Usuario &) = delete;
    Usuario & operator=(const Usuario &) = delete;

    bool estaValido() const;

    const char * getNombreUsuario() const;
    const FechaHora & getFechaNacimiento() const;
    const char * getCorreo() const;
    const char * getCiudad() const;
    const char * getPais() const;
    const FechaHora & getFechaInscripcion() const;
    unsigned int getIndice() const;
    unsigned int getPrimerNoLeido() const;
    ListaDinamica<AlbumFotos *> & getAlbumes();
    ListaDinamica<Mensaje *> & getMensajesRecibidos();
    ListaDinamica<Mensaje *> & getMensajesEnviados();

    bool setCorreo(const char * correo);
    bool setContrasena(const char * contrasena);
    bool setCiudad(const char * ciudad);
    bool setPais(const char * pais);
    void setPrimerNoLeido(unsigned int posicion);

    bool verificarContrasena(const char * intento) const;

    bool agregarAlbum(AlbumFotos * album);
    unsigned int getNumAlbumes() const;
    AlbumFotos * buscarAlbum(const char * nombre) const;
    AlbumFotos * buscarAlbum(unsigned int posicion) const;

    bool recibirMensaje(Mensaje * mensaje);
    bool registrarEnviado(Mensaje * mensaje);
    unsigned int contarNoLeidos() const;
    unsigned int marcarTodosLeidos();
    bool buscarRangoMensajes(const FechaHora & inicio, const FechaHora & fin,
                             unsigned int & desde, unsigned int & hasta) const;

    bool operator<(const Usuario & otro) const;
    bool operator==(const Usuario & otro) const;
    unsigned long calcularMemoria() const;
};

#endif
