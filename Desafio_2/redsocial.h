#ifndef REDSOCIAL_H
#define REDSOCIAL_H

#include "ListaDinamica.h"
#include "configuracion.h"

class Usuario;
class AlbumFotos;
class Foto;
class RedAmistades;
class GestorArchivos;
class MedidorRecursos;

class RedSocial {
private:
    ListaDinamica<Usuario *> usuarios;
    RedAmistades *    amistades;
    GestorArchivos *  gestor;
    MedidorRecursos * medidor;
    Usuario *         usuarioActivo;
    unsigned int *    grupoFotos;
    unsigned short    cantidadGrupo;
    bool              fotosActualizadas;
    bool              valido;

public:
    RedSocial(const char * rutaDatos, const char * subRutaFija);
    ~RedSocial();
    RedSocial(const RedSocial &) = delete;
    RedSocial & operator=(const RedSocial &) = delete;

    bool estaValido() const;

    bool cargarDatos();

    bool inicializarRed(unsigned int numUsuarios);
    bool agregarUsuario(Usuario * usuario);
    RedAmistades & getAmistades();

    unsigned int getNumUsuarios() const;
    Usuario * getUsuario(unsigned int indice) const;
    Usuario * getUsuarioActivo() const;
    GestorArchivos & getGestor();
    MedidorRecursos & getMedidor();
    Usuario * buscarUsuario(const char * nombre) const;
    Usuario * buscarAmigo(const char * nombre) const;

    bool puedeVerAlbum(const Usuario & duenio, const AlbumFotos & album) const;

    unsigned short iniciarSesion(const char * nombre, const char * contrasena);
    void cerrarSesion();
    bool iniciarFotosAmigos();
    unsigned short actualizarFotosAmigos();
    unsigned short getCantidadGrupo() const;
    Usuario * getAmigoDestacado(unsigned short posicion) const;

    unsigned short enviarSolicitud(const char * nombreDestino);
    unsigned int listarSolicitudes(unsigned int * emisores, unsigned int capacidad) const;
    unsigned short aceptarSolicitud(unsigned int indiceEmisor);

    unsigned short sugerirAmigos(unsigned short k, unsigned int * indices, unsigned int * comunes) const;

    unsigned short enviarMensaje(const char * nombreDestino, const char * texto);
    unsigned short marcarMensajesLeidos();

    unsigned short registrarLike(Foto * foto, bool dar);

    void iniciarMedicion(unsigned short idFuncionalidad);
    void mostrarReporte();
    void mostrarResumenSesion();
    unsigned long calcularMemoria() const;
    unsigned long calcularMemoriaReferencia() const;
};

#endif