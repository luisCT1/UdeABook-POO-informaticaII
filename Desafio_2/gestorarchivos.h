#ifndef GESTORARCHIVOS_H
#define GESTORARCHIVOS_H

#include <iosfwd>
#include "configuracion.h"

class RedSocial;
class Usuario;
class AlbumFotos;
class Foto;
class Mensaje;

//lee los 6 txt una vez y despues solo agrega lineas al final

class GestorArchivos {
private:
    char * rutaDatos;
    char * subRutaFija;
    unsigned int registros[NUM_ARCHIVOS];
    bool mostrarMensajes;
    bool valido;

    bool cargarUsuarios(RedSocial & red);
    bool cargarMultimedia(RedSocial & red);
    bool cargarAmistades(RedSocial & red);
    bool cargarSolicitudes(RedSocial & red);
    bool cargarMensajes(RedSocial & red);
    bool cargarCambios(RedSocial & red);

    bool armarRutaArchivo(unsigned short idArchivo, char * destino, unsigned int capacidad) const;
    bool leerLinea(std::ifstream & archivo, char * buffer, unsigned int capacidad) const;
    bool anexarLinea(unsigned short idArchivo, const char * linea, unsigned int largo);
    void avisarActualizacion(unsigned short idArchivo) const;
    bool pegar(char * destino, unsigned int capacidad, unsigned int & usado, const char * parte) const;
    bool pegarNumero(char * destino, unsigned int capacidad, unsigned int & usado, unsigned long long valor) const;

public:
    GestorArchivos();
    GestorArchivos(const char * rutaDatos, const char * subRutaFija);
    ~GestorArchivos();
    GestorArchivos(const GestorArchivos &) = delete;
    GestorArchivos & operator=(const GestorArchivos &) = delete;

    bool estaValido() const;
    const char * getRutaDatos() const;
    const char * getSubRutaFija() const;
    bool setRutaDatos(const char * ruta);
    bool setSubRutaFija(const char * subRuta);
    void setMostrarMensajes(bool mostrar);
    unsigned int getRegistros(unsigned short idArchivo) const;

    bool cargarDatos(RedSocial & red);

    bool registrarMensaje(const Mensaje & mensaje);
    bool registrarSolicitud(unsigned int indiceEmisor, unsigned int indiceReceptor);
    bool registrarAmistad(unsigned int indiceA, unsigned int indiceB);
    bool registrarLecturas(unsigned int indiceUsuario, unsigned int primerNoLeido);
    bool registrarLikes(unsigned int idFoto, unsigned int likes);

    bool armarRutaFoto(const Usuario & duenio, const AlbumFotos & album, const Foto & foto,
                       char * destino, unsigned int capacidad) const;
    bool armarRutaPerfil(const Usuario & duenio, char * destino, unsigned int capacidad) const;

    unsigned long calcularMemoria() const;
};

#endif
