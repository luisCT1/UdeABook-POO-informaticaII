#include "gestorarchivos.h"
#include "redsocial.h"
#include "RedAmistades.h"
#include "Usuario.h"
#include "AlbumFotos.h"
#include "Foto.h"
#include "Mensaje.h"
#include "MedidorRecursos.h"
#include "utilidades.h"

#include <fstream>
#include <iostream>
#include <new>

namespace {

const char * nombreArchivoDatos(unsigned short id) {
    static const char * nombres[NUM_ARCHIVOS] = {
        "usuarios.txt",
        "multimedia.txt",
        "amistades.txt",
        "solicitudes.txt",
        "mensajes.txt",
        "cambios.txt"
    };
    if (id >= NUM_ARCHIVOS) {
        return "";
    }
    return nombres[id];
}

bool tomarCampo(const char * linea, unsigned int & i, char * destino, unsigned int cap) {
    if (linea == nullptr || destino == nullptr || cap == 0) {
        return false;
    }
    unsigned int d = 0;
    while (linea[i] != '\0' && linea[i] != SEPARADOR) {
        if (d + 1 < cap) {
            destino[d++] = linea[i];
        }
        i++;
    }
    destino[d] = '\0';
    if (linea[i] == SEPARADOR) {
        i++;
        return true;
    }
    return linea[i] == '\0';
}

bool leerIndice(const char * texto, unsigned int & valor) {
    if (texto == nullptr || texto[0] == '\0') {
        return false;
    }
    unsigned long long n = 0;
    if (!aNumero(texto, 0, longitud(texto), n)) {
        return false;
    }
    valor = (unsigned int)n;
    return true;
}

bool nombreRepetido(RedSocial & red, const char * nombre) {
    for (unsigned int i = 0; i < red.getNumUsuarios(); i++) {
        MedidorRecursos::sumarIteraciones();
        Usuario * u = red.getUsuario(i);
        if (u != nullptr && iguales(u->getNombreUsuario(), nombre)) {
            return true;
        }
    }
    return false;
}

Foto * buscarFoto(RedSocial & red, unsigned int id) {
    for (unsigned int u = 0; u < red.getNumUsuarios(); u++) {
        Usuario * usuario = red.getUsuario(u);
        if (usuario == nullptr) {
            continue;
        }
        for (unsigned int a = 0; a < usuario->getNumAlbumes(); a++) {
            AlbumFotos * album = usuario->buscarAlbum(a);
            if (album == nullptr) {
                continue;
            }
            for (unsigned int f = 0; f < album->getNumFotos(); f++) {
                MedidorRecursos::sumarIteraciones();
                Foto * foto = album->getFoto(f);
                if (foto != nullptr && foto->getId() == id) {
                    return foto;
                }
            }
        }
    }
    return nullptr;
}

bool mensajeIgual(const Mensaje * mensaje, Usuario * emisor, const FechaHora & fecha, const char * texto) {
    if (mensaje == nullptr || mensaje->getEmisor() != emisor) {
        return false;
    }
    if (!(mensaje->getFechaEnvio() == fecha)) {
        return false;
    }
    return iguales(mensaje->getTexto(), texto);
}

}

GestorArchivos::GestorArchivos()
    : rutaDatos(nullptr), subRutaFija(nullptr), mostrarMensajes(true), valido(false) {
    for (unsigned short i = 0; i < NUM_ARCHIVOS; i++) {
        registros[i] = 0;
    }
}

GestorArchivos::GestorArchivos(const char * ruta, const char * sub)
    : rutaDatos(nullptr), subRutaFija(nullptr), mostrarMensajes(true), valido(true) {
    for (unsigned short i = 0; i < NUM_ARCHIVOS; i++) {
        registros[i] = 0;
    }
    if (!setRutaDatos(ruta) || !setSubRutaFija(sub == nullptr || sub[0] == '\0' ? "x" : sub)) {
        valido = false;
    }
}

GestorArchivos::~GestorArchivos() {
    delete[] rutaDatos;
    delete[] subRutaFija;
}

bool GestorArchivos::estaValido() const { return valido; }
const char * GestorArchivos::getRutaDatos() const { return rutaDatos; }
const char * GestorArchivos::getSubRutaFija() const { return subRutaFija; }
void GestorArchivos::setMostrarMensajes(bool mostrar) { mostrarMensajes = mostrar; }

unsigned int GestorArchivos::getRegistros(unsigned short idArchivo) const {
    if (idArchivo >= NUM_ARCHIVOS) {
        return 0;
    }
    return registros[idArchivo];
}

bool GestorArchivos::setRutaDatos(const char * ruta) {
    if (ruta == nullptr || ruta[0] == '\0') {
        return false;
    }
    unsigned int n = longitud(ruta);
    char ultimo = ruta[n - 1];
    bool falta = ultimo != '/' && ultimo != '\\';
    char * copia = reservar<char>(n + (falta ? 2u : 1u));
    if (copia == nullptr) {
        return false;
    }
    for (unsigned int i = 0; i < n; i++) {
        copia[i] = ruta[i];
    }
    if (falta) {
        copia[n++] = '/';
    }
    copia[n] = '\0';
    delete[] rutaDatos;
    rutaDatos = copia;
    return true;
}

bool GestorArchivos::setSubRutaFija(const char * subRuta) {
    char * copia = copiar(subRuta == nullptr || subRuta[0] == '\0' ? "x" : subRuta);
    if (copia == nullptr) {
        return false;
    }
    delete[] subRutaFija;
    subRutaFija = copia;
    return true;
}

bool GestorArchivos::pegar(char * destino, unsigned int capacidad, unsigned int & usado, const char * parte) const {
    unsigned int n = longitud(parte);
    if (usado + n + 1 > capacidad) {
        return false;
    }
    for (unsigned int i = 0; i < n; i++) {
        destino[usado++] = parte[i];
    }
    destino[usado] = '\0';
    return true;
}

bool GestorArchivos::pegarNumero(char * destino, unsigned int capacidad, unsigned int & usado, unsigned long long valor) const {
    char tmp[24];
    unsigned int n = 0;
    if (valor == 0) {
        tmp[n++] = '0';
    }
    while (valor > 0 && n < 24) {
        tmp[n++] = (char)('0' + (int)(valor % 10));
        valor /= 10;
    }
    if (usado + n + 1 > capacidad) {
        return false;
    }
    for (unsigned int i = n; i > 0; i--) {
        destino[usado++] = tmp[i - 1];
    }
    destino[usado] = '\0';
    return true;
}

bool GestorArchivos::armarRutaArchivo(unsigned short idArchivo, char * destino, unsigned int capacidad) const {
    if (destino == nullptr || rutaDatos == nullptr) {
        return false;
    }
    unsigned int usado = 0;
    destino[0] = '\0';
    return pegar(destino, capacidad, usado, rutaDatos)
        && pegar(destino, capacidad, usado, nombreArchivoDatos(idArchivo));
}

bool GestorArchivos::leerLinea(std::ifstream & archivo, char * buffer, unsigned int capacidad) const {
    if (buffer == nullptr || capacidad == 0) {
        return false;
    }
    unsigned int i = 0;
    bool algo = false;
    while (true) {
        int c = archivo.get();
        if (!archivo || c == EOF) {
            buffer[i] = '\0';
            return algo;
        }
        algo = true;
        if (c == '\r') {
            continue;
        }
        if (c == '\n') {
            buffer[i] = '\0';
            MedidorRecursos::sumarUsoExterno();
            return true;
        }
        if (i + 1 < capacidad) {
            buffer[i++] = (char)c;
        }
    }
}

bool GestorArchivos::anexarLinea(unsigned short idArchivo, const char * linea, unsigned int largo) {
    char ruta[1024];
    if (!armarRutaArchivo(idArchivo, ruta, 1024)) {
        return false;
    }
    std::ofstream salida(ruta, std::ios::app);
    if (!salida) {
        return false;
    }
    salida.write(linea, largo);
    salida << '\n';
    salida.flush();
    if (!salida) {
        return false;
    }
    MedidorRecursos::sumarUsoExterno();
    if (idArchivo < NUM_ARCHIVOS) {
        registros[idArchivo]++;
    }
    if (mostrarMensajes) {
        avisarActualizacion(idArchivo);
    }
    return true;
}

void GestorArchivos::avisarActualizacion(unsigned short idArchivo) const {
    std::cout << "AVISO se agrego una linea a " << nombreArchivoDatos(idArchivo) << "\n";
}

bool GestorArchivos::cargarUsuarios(RedSocial & red) {
    char ruta[1024];
    if (!armarRutaArchivo(ARCH_USUARIOS, ruta, 1024)) {
        return false;
    }
    std::ifstream archivo(ruta);
    if (!archivo) {
        std::cout << "AVISO no se pudo abrir " << ruta << "\n";
        return false;
    }
    char linea[MAX_LINEA];
    while (leerLinea(archivo, linea, MAX_LINEA)) {
        registros[ARCH_USUARIOS]++;
        unsigned int indice = red.getNumUsuarios();
        if (linea[0] == '\0') {
            red.agregarUsuario(nullptr);
            continue;
        }
        unsigned int i = 0;
        char nombre[64];
        char nacimiento[16];
        char correo[80];
        char clave[64];
        char ciudad[64];
        char pais[64];
        char inscripcion[16];
        if (!tomarCampo(linea, i, nombre, 64) || !tomarCampo(linea, i, nacimiento, 16)
            || !tomarCampo(linea, i, correo, 80) || !tomarCampo(linea, i, clave, 64)
            || !tomarCampo(linea, i, ciudad, 64) || !tomarCampo(linea, i, pais, 64)
            || !tomarCampo(linea, i, inscripcion, 16) || nombreRepetido(red, nombre)) {
            red.agregarUsuario(nullptr);
            continue;
        }
        FechaHora fechaNacimiento;
        FechaHora fechaInscripcion;
        if (!fechaNacimiento.desdeTexto14(nacimiento) || !fechaInscripcion.desdeTexto14(inscripcion)) {
            red.agregarUsuario(nullptr);
            continue;
        }
        Usuario * usuario = new (std::nothrow) Usuario(nombre, clave, fechaNacimiento, fechaInscripcion, indice);
        if (usuario == nullptr || !usuario->estaValido() || !usuario->setCorreo(correo)
            || !usuario->setCiudad(ciudad) || !usuario->setPais(pais)) {
            delete usuario;
            red.agregarUsuario(nullptr);
            continue;
        }
        if (!red.agregarUsuario(usuario)) {
            delete usuario;
            return false;
        }
    }
    return red.inicializarRed(red.getNumUsuarios());
}

bool GestorArchivos::cargarMultimedia(RedSocial & red) {
    char ruta[1024];
    if (!armarRutaArchivo(ARCH_MULTIMEDIA, ruta, 1024)) {
        return false;
    }
    std::ifstream archivo(ruta);
    if (!archivo) {
        std::cout << "AVISO no se pudo abrir " << ruta << "\n";
        return false;
    }
    AlbumFotos * album = nullptr;
    unsigned int idFoto = 0;
    char linea[MAX_LINEA];
    while (leerLinea(archivo, linea, MAX_LINEA)) {
        if (linea[0] == '\0') {
            continue;
        }
        registros[ARCH_MULTIMEDIA]++;
        unsigned int i = 2;
        if (linea[0] == 'A' && linea[1] == SEPARADOR) {
            album = nullptr;
            char indiceTxt[16];
            char nombre[64];
            char privacidad[4];
            unsigned int indice = 0;
            if (!tomarCampo(linea, i, indiceTxt, 16) || !tomarCampo(linea, i, nombre, 64)
                || !tomarCampo(linea, i, privacidad, 4) || !leerIndice(indiceTxt, indice)) {
                continue;
            }
            Usuario * dueno = red.getUsuario(indice);
            if (dueno == nullptr) {
                continue;
            }
            AlbumFotos * nuevo = new (std::nothrow) AlbumFotos(nombre, privacidad[0]);
            if (nuevo == nullptr || !nuevo->estaValido() || !dueno->agregarAlbum(nuevo)) {
                delete nuevo;
                continue;
            }
            album = nuevo;
        } else if (linea[0] == 'F' && linea[1] == SEPARADOR) {
            char fechaTxt[16];
            char bytesTxt[16];
            char nombre[64];
            unsigned int bytes = 0;
            if (!tomarCampo(linea, i, fechaTxt, 16) || !tomarCampo(linea, i, bytesTxt, 16)
                || !tomarCampo(linea, i, nombre, 64) || !leerIndice(bytesTxt, bytes)) {
                idFoto++;
                continue;
            }
            FechaHora fecha;
            if (album != nullptr && fecha.desdeTexto14(fechaTxt)) {
                Foto * foto = new (std::nothrow) Foto(idFoto, fecha, bytes, nombre, 0);
                if (foto == nullptr || !foto->estaValido() || !album->agregarFoto(foto)) {
                    delete foto;
                }
            }
            idFoto++;
        }
    }
    return true;
}

bool GestorArchivos::cargarAmistades(RedSocial & red) {
    char ruta[1024];
    if (!armarRutaArchivo(ARCH_AMISTADES, ruta, 1024)) {
        return false;
    }
    std::ifstream archivo(ruta);
    if (!archivo) {
        std::cout << "AVISO no se pudo abrir " << ruta << "\n";
        return false;
    }
    char linea[MAX_LINEA];
    while (leerLinea(archivo, linea, MAX_LINEA)) {
        if (linea[0] == '\0') {
            continue;
        }
        registros[ARCH_AMISTADES]++;
        unsigned int i = 0;
        char aTxt[16];
        char bTxt[16];
        unsigned int a = 0;
        unsigned int b = 0;
        if (!tomarCampo(linea, i, aTxt, 16) || !tomarCampo(linea, i, bTxt, 16)
            || !leerIndice(aTxt, a) || !leerIndice(bTxt, b)) {
            continue;
        }
        red.getAmistades().conectar(a, b);
    }
    return true;
}

bool GestorArchivos::cargarSolicitudes(RedSocial & red) {
    char ruta[1024];
    if (!armarRutaArchivo(ARCH_SOLICITUDES, ruta, 1024)) {
        return false;
    }
    std::ifstream archivo(ruta);
    if (!archivo) {
        std::cout << "AVISO no se pudo abrir " << ruta << "\n";
        return false;
    }
    char linea[MAX_LINEA];
    while (leerLinea(archivo, linea, MAX_LINEA)) {
        if (linea[0] == '\0') {
            continue;
        }
        registros[ARCH_SOLICITUDES]++;
        unsigned int i = 0;
        char aTxt[16];
        char bTxt[16];
        unsigned int emisor = 0;
        unsigned int receptor = 0;
        if (!tomarCampo(linea, i, aTxt, 16) || !tomarCampo(linea, i, bTxt, 16)
            || !leerIndice(aTxt, emisor) || !leerIndice(bTxt, receptor)) {
            continue;
        }
        if (red.getAmistades().sonAmigos(emisor, receptor)) {
            continue;
        }
        if (red.getAmistades().tieneSolicitud(emisor, receptor)
            || red.getAmistades().tieneSolicitud(receptor, emisor)) {
            continue;
        }
        red.getAmistades().marcarSolicitud(emisor, receptor);
    }
    return true;
}

bool GestorArchivos::cargarMensajes(RedSocial & red) {
    char ruta[1024];
    if (!armarRutaArchivo(ARCH_MENSAJES, ruta, 1024)) {
        return false;
    }
    std::ifstream archivo(ruta);
    if (!archivo) {
        std::cout << "AVISO no se pudo abrir " << ruta << "\n";
        return false;
    }
    char linea[MAX_LINEA];
    while (leerLinea(archivo, linea, MAX_LINEA)) {
        if (linea[0] == '\0') {
            continue;
        }
        registros[ARCH_MENSAJES]++;
        unsigned int i = 0;
        char emisorTxt[16];
        char receptorTxt[16];
        char fechaTxt[16];
        char largoTxt[16];
        unsigned int indiceEmisor = 0;
        unsigned int indiceReceptor = 0;
        unsigned int largo = 0;
        if (!tomarCampo(linea, i, emisorTxt, 16) || !tomarCampo(linea, i, receptorTxt, 16)
            || !tomarCampo(linea, i, fechaTxt, 16) || !tomarCampo(linea, i, largoTxt, 16)
            || !leerIndice(emisorTxt, indiceEmisor) || !leerIndice(receptorTxt, indiceReceptor)
            || !leerIndice(largoTxt, largo) || largo > MAX_BYTES_MENSAJE) {
            continue;
        }
        if (longitud(linea + i) < largo) {
            continue;
        }
        char texto[MAX_BYTES_MENSAJE + 1];
        for (unsigned int k = 0; k < largo; k++) {
            texto[k] = linea[i + k];
        }
        texto[largo] = '\0';
        Usuario * emisor = red.getUsuario(indiceEmisor);
        Usuario * receptor = red.getUsuario(indiceReceptor);
        FechaHora fecha;
        if (emisor == nullptr || receptor == nullptr || !fecha.desdeTexto14(fechaTxt)) {
            continue;
        }
        bool repetido = false;
        ListaDinamica<Mensaje *> & bandeja = receptor->getMensajesRecibidos();
        for (unsigned int m = 0; m < bandeja.getTamanio(); m++) {
            MedidorRecursos::sumarIteraciones();
            if (mensajeIgual(bandeja[m], emisor, fecha, texto)) {
                repetido = true;
            }
        }
        if (repetido) {
            continue;
        }
        Mensaje * mensaje = new (std::nothrow) Mensaje(emisor, receptor, texto, fecha);
        if (mensaje == nullptr || !mensaje->estaValido() || !receptor->recibirMensaje(mensaje)) {
            delete mensaje;
            continue;
        }
        emisor->registrarEnviado(mensaje);
    }
    return true;
}

bool GestorArchivos::cargarCambios(RedSocial & red) {
    char ruta[1024];
    if (!armarRutaArchivo(ARCH_CAMBIOS, ruta, 1024)) {
        return false;
    }
    std::ifstream archivo(ruta);
    if (!archivo) {
        std::cout << "AVISO no se pudo abrir " << ruta << "\n";
        return false;
    }
    char linea[MAX_LINEA];
    while (leerLinea(archivo, linea, MAX_LINEA)) {
        if (linea[0] == '\0') {
            continue;
        }
        registros[ARCH_CAMBIOS]++;
        unsigned int i = 2;
        if ((linea[0] != 'L' && linea[0] != 'K') || linea[1] != SEPARADOR) {
            continue;
        }
        char aTxt[16];
        char bTxt[16];
        unsigned int a = 0;
        unsigned int b = 0;
        if (!tomarCampo(linea, i, aTxt, 16) || !tomarCampo(linea, i, bTxt, 16)
            || !leerIndice(aTxt, a) || !leerIndice(bTxt, b)) {
            continue;
        }
        if (linea[0] == 'L') {
            Usuario * usuario = red.getUsuario(a);
            if (usuario == nullptr) {
                continue;
            }
            unsigned int n = usuario->getMensajesRecibidos().getTamanio();
            if (b > n) {
                b = n;
            }
            usuario->setPrimerNoLeido(b);
            for (unsigned int m = 0; m < n; m++) {
                MedidorRecursos::sumarIteraciones();
                Mensaje * mensaje = usuario->getMensajesRecibidos()[m];
                if (mensaje != nullptr) {
                    mensaje->setLeido(m < b);
                }
            }
        } else {
            Foto * foto = buscarFoto(red, a);
            if (foto != nullptr) {
                foto->setLikes(b);
            }
        }
    }
    return true;
}

bool GestorArchivos::cargarDatos(RedSocial & red) {
    if (!valido) {
        return false;
    }
    if (!cargarUsuarios(red)) {
        return false;
    }
    if (!cargarMultimedia(red) || !cargarAmistades(red) || !cargarSolicitudes(red)
        || !cargarMensajes(red) || !cargarCambios(red)) {
        return false;
    }
    return true;
}

bool GestorArchivos::registrarMensaje(const Mensaje & mensaje) {
    if (mensaje.getEmisor() == nullptr || mensaje.getReceptor() == nullptr || mensaje.getTexto() == nullptr) {
        return false;
    }
    char fecha[16];
    mensaje.getFechaEnvio().aTexto14(fecha);
    char linea[MAX_LINEA];
    unsigned int usado = 0;
    linea[0] = '\0';
    if (!pegarNumero(linea, MAX_LINEA, usado, mensaje.getEmisor()->getIndice())
        || !pegar(linea, MAX_LINEA, usado, "|")
        || !pegarNumero(linea, MAX_LINEA, usado, mensaje.getReceptor()->getIndice())
        || !pegar(linea, MAX_LINEA, usado, "|")
        || !pegar(linea, MAX_LINEA, usado, fecha)
        || !pegar(linea, MAX_LINEA, usado, "|")
        || !pegarNumero(linea, MAX_LINEA, usado, longitud(mensaje.getTexto()))
        || !pegar(linea, MAX_LINEA, usado, "|")
        || !pegar(linea, MAX_LINEA, usado, mensaje.getTexto())) {
        return false;
    }
    return anexarLinea(ARCH_MENSAJES, linea, usado);
}

bool GestorArchivos::registrarSolicitud(unsigned int indiceEmisor, unsigned int indiceReceptor) {
    char linea[64];
    unsigned int usado = 0;
    linea[0] = '\0';
    if (!pegarNumero(linea, 64, usado, indiceEmisor) || !pegar(linea, 64, usado, "|")
        || !pegarNumero(linea, 64, usado, indiceReceptor)) {
        return false;
    }
    return anexarLinea(ARCH_SOLICITUDES, linea, usado);
}

bool GestorArchivos::registrarAmistad(unsigned int indiceA, unsigned int indiceB) {
    if (indiceA > indiceB) {
        unsigned int tmp = indiceA;
        indiceA = indiceB;
        indiceB = tmp;
    }
    char linea[64];
    unsigned int usado = 0;
    linea[0] = '\0';
    if (!pegarNumero(linea, 64, usado, indiceA) || !pegar(linea, 64, usado, "|")
        || !pegarNumero(linea, 64, usado, indiceB)) {
        return false;
    }
    return anexarLinea(ARCH_AMISTADES, linea, usado);
}

bool GestorArchivos::registrarLecturas(unsigned int indiceUsuario, unsigned int primerNoLeido) {
    char linea[64];
    unsigned int usado = 0;
    linea[0] = '\0';
    if (!pegar(linea, 64, usado, "L|")
        || !pegarNumero(linea, 64, usado, indiceUsuario)
        || !pegar(linea, 64, usado, "|")
        || !pegarNumero(linea, 64, usado, primerNoLeido)) {
        return false;
    }
    return anexarLinea(ARCH_CAMBIOS, linea, usado);
}

bool GestorArchivos::registrarLikes(unsigned int idFoto, unsigned int likes) {
    char linea[64];
    unsigned int usado = 0;
    linea[0] = '\0';
    if (!pegar(linea, 64, usado, "K|")
        || !pegarNumero(linea, 64, usado, idFoto)
        || !pegar(linea, 64, usado, "|")
        || !pegarNumero(linea, 64, usado, likes)) {
        return false;
    }
    return anexarLinea(ARCH_CAMBIOS, linea, usado);
}

bool GestorArchivos::armarRutaFoto(const Usuario & duenio, const AlbumFotos & album, const Foto & foto,
                                   char * destino, unsigned int capacidad) const {
    if (destino == nullptr || subRutaFija == nullptr) {
        return false;
    }
    unsigned int usado = 0;
    destino[0] = '\0';
    return pegar(destino, capacidad, usado, "/users/")
        && pegar(destino, capacidad, usado, subRutaFija)
        && pegar(destino, capacidad, usado, "/")
        && pegar(destino, capacidad, usado, duenio.getNombreUsuario())
        && pegar(destino, capacidad, usado, "/albums/")
        && pegar(destino, capacidad, usado, album.getNombre())
        && pegar(destino, capacidad, usado, "/")
        && pegar(destino, capacidad, usado, foto.getNombreArchivo())
        && pegar(destino, capacidad, usado, ".png");
}

bool GestorArchivos::armarRutaPerfil(const Usuario & duenio, char * destino, unsigned int capacidad) const {
    if (destino == nullptr || subRutaFija == nullptr) {
        return false;
    }
    unsigned int usado = 0;
    destino[0] = '\0';
    return pegar(destino, capacidad, usado, "/users/")
        && pegar(destino, capacidad, usado, subRutaFija)
        && pegar(destino, capacidad, usado, "/")
        && pegar(destino, capacidad, usado, duenio.getNombreUsuario())
        && pegar(destino, capacidad, usado, "/albums/profile.png");
}

unsigned long GestorArchivos::calcularMemoria() const {
    unsigned long total = sizeof(*this);
    if (rutaDatos != nullptr) {
        total += longitud(rutaDatos) + 1;
    }
    if (subRutaFija != nullptr) {
        total += longitud(subRutaFija) + 1;
    }
    return total;
}
