#include "menu.h"
#include "redsocial.h"
#include "gestorarchivos.h"
#include "Usuario.h"
#include "AlbumFotos.h"
#include "Foto.h"
#include "Mensaje.h"
#include "utilidades.h"
#include "configuracion.h"

#include <fstream>
#include <iostream>

namespace {

void leerTexto(const char * aviso, char * buffer, unsigned int cap) {
    std::cout << aviso;
    if (!std::cin.getline(buffer, cap)) {
        std::cin.clear();
        buffer[0] = '\0';
    }
}

void decir(unsigned short codigo) {
    std::cout << descripcionResultado(codigo) << "\n";
}

bool existeDataset(const char * carpeta) {
    char ruta[1024];
    unsigned int n = longitud(carpeta);
    if (n + 14 >= 1024) {
        return false;
    }
    unsigned int i = 0;
    for (; i < n; i++) {
        ruta[i] = carpeta[i];
    }
    if (n > 0 && carpeta[n - 1] != '/' && carpeta[n - 1] != '\\') {
        ruta[i++] = '/';
    }
    const char * archivo = "usuarios.txt";
    for (unsigned int k = 0; archivo[k] != '\0'; k++) {
        ruta[i++] = archivo[k];
    }
    ruta[i] = '\0';
    std::ifstream prueba(ruta);
    return (bool)prueba;
}

void mostrarAmigosDestacados(RedSocial & red) {
    unsigned short n = red.getCantidadGrupo();
    if (n == 0) {
        std::cout << "no hay fotos de amigos para mostrar\n";
        return;
    }
    for (unsigned short i = 0; i < n; i++) {
        Usuario * amigo = red.getAmigoDestacado(i);
        if (amigo == nullptr) {
            continue;
        }
        char ruta[256];
        if (!red.getGestor().armarRutaPerfil(*amigo, ruta, 256)) {
            copiarEn("?", ruta, 256);
        }
        std::cout << (i + 1) << ". " << amigo->getNombreUsuario() << "  " << ruta << "\n";
    }
}

}

const char * seleccionarDataset() {
    static char ruta[512];
    leerTexto("carpeta de datos (enter = datos/conjunto): ", ruta, 512);
    if (ruta[0] != '\0') {
        return ruta;
    }
    //qt deja el programa dentro de build, asi que se sube hasta encontrar datos
    char intento[512];
    copiarEn("datos/conjunto", intento, 512);
    for (int sube = 0; sube < 6; sube++) {
        if (existeDataset(intento)) {
            copiarEn(intento, ruta, 512);
            return ruta;
        }
        char siguiente[512];
        unsigned int n = 0;
        const char * prefijo = "../";
        while (prefijo[n] != '\0' && n + 1 < 512) {
            siguiente[n] = prefijo[n];
            n++;
        }
        unsigned int m = longitud(intento);
        if (n + m + 1 > 512) {
            break;
        }
        for (unsigned int i = 0; i < m; i++) {
            siguiente[n + i] = intento[i];
        }
        siguiente[n + m] = '\0';
        copiarEn(siguiente, intento, 512);
    }
    copiarEn("datos/conjunto", ruta, 512);
    return ruta;
}

bool pantallaDeIngreso(RedSocial & red) {
    red.iniciarMedicion(FUNC_INGRESO);
    while (true) {
        char nombre[64];
        char clave[64];
        leerTexto("usuario (enter cancela): ", nombre, 64);
        if (nombre[0] == '\0') {
            red.mostrarReporte();
            return false;
        }
        leerTexto("contrasena: ", clave, 64);
        unsigned short codigo = red.iniciarSesion(nombre, clave);
        if (codigo == RES_OK) {
            mostrarPantallaInicio(red);
            red.mostrarReporte();
            return true;
        }
        decir(codigo);
    }
}

void mostrarPantallaInicio(RedSocial & red) {
    Usuario * activo = red.getUsuarioActivo();
    std::cout << "\nhola " << (activo == nullptr ? "" : activo->getNombreUsuario()) << "\n";
    std::cout << "fotos de amigos:\n";
    mostrarAmigosDestacados(red);
    std::cout << "1 amistades\n";
    std::cout << "2 visitar perfil\n";
    std::cout << "3 sugerencias\n";
    std::cout << "4 mensajes\n";
    std::cout << "n cambiar fotos de amigos\n";
    std::cout << "s salir\n";
}

void opcionAdministrarAmistades(RedSocial & red) {
    char op[8];
    leerTexto("a enviar solicitud, b aceptar, s volver: ", op, 8);
    if (op[0] == 'a' || op[0] == 'A') {
        red.iniciarMedicion(FUNC_ENVIAR_SOLICITUD);
        char nombre[64];
        leerTexto("usuario destino: ", nombre, 64);
        decir(red.enviarSolicitud(nombre));
        red.mostrarReporte();
        return;
    }
    if (op[0] != 'b' && op[0] != 'B') {
        return;
    }
    red.iniciarMedicion(FUNC_ACEPTAR_SOLICITUD);
    unsigned int emisores[64];
    unsigned int n = red.listarSolicitudes(emisores, 64);
    if (n == 0) {
        std::cout << "no hay solicitudes\n";
        red.mostrarReporte();
        return;
    }
    for (unsigned int i = 0; i < n; i++) {
        Usuario * emisor = red.getUsuario(emisores[i]);
        std::cout << (i + 1) << ". " << (emisor == nullptr ? "?" : emisor->getNombreUsuario()) << "\n";
    }
    char num[16];
    leerTexto("numero a aceptar (enter cancela): ", num, 16);
    unsigned long long elegido = 0;
    if (num[0] != '\0' && aNumero(num, 0, longitud(num), elegido) && elegido >= 1 && elegido <= n) {
        decir(red.aceptarSolicitud(emisores[elegido - 1]));
    }
    red.mostrarReporte();
}

void opcionVisitarPerfil(RedSocial & red) {
    red.iniciarMedicion(FUNC_PERFIL_AMIGO);
    char nombre[64];
    leerTexto("amigo a visitar: ", nombre, 64);
    Usuario * activo = red.getUsuarioActivo();
    Usuario * visitado = nullptr;
    if (activo != nullptr && iguales(activo->getNombreUsuario(), nombre)) {
        visitado = activo;
    } else {
        visitado = red.buscarAmigo(nombre);
    }
    if (visitado == nullptr) {
        decir(red.buscarUsuario(nombre) == nullptr ? RES_NO_EXISTE : RES_NO_ES_AMIGO);
        red.mostrarReporte();
        return;
    }
    std::cout << visitado->getNombreUsuario() << "  " << visitado->getCiudad() << ", " << visitado->getPais() << "\n";
    unsigned int visibles[64];
    unsigned int nv = 0;
    for (unsigned int i = 0; i < visitado->getNumAlbumes() && nv < 64; i++) {
        AlbumFotos * album = visitado->buscarAlbum(i);
        if (album != nullptr && red.puedeVerAlbum(*visitado, *album)) {
            visibles[nv++] = i;
            std::cout << nv << ". " << album->getNombre() << " (" << album->getPrivacidad() << ")\n";
        }
    }
    if (nv == 0) {
        std::cout << "no hay albumes visibles\n";
        red.mostrarReporte();
        return;
    }
    char num[16];
    leerTexto("album (enter vuelve): ", num, 16);
    unsigned long long elegido = 0;
    if (num[0] == '\0' || !aNumero(num, 0, longitud(num), elegido) || elegido < 1 || elegido > nv) {
        red.mostrarReporte();
        return;
    }
    AlbumFotos * album = visitado->buscarAlbum(visibles[elegido - 1]);
    int pos = 0;
    bool seguir = album != nullptr && album->getNumFotos() > 0;
    if (!seguir) {
        std::cout << "el album no tiene fotos\n";
    }
    while (seguir) {
        Foto * foto = album->getFotoCircular(pos);
        char ruta[256];
        if (foto != nullptr && !red.getGestor().armarRutaFoto(*visitado, *album, *foto, ruta, 256)) {
            copiarEn("?", ruta, 256);
        }
        if (foto != nullptr) {
            std::cout << foto->getNombreArchivo() << "  likes " << foto->getLikes() << "  " << ruta << "\n";
        }
        char mov[8];
        leerTexto("n siguiente, p previa, l like, q quitar, s volver: ", mov, 8);
        if (mov[0] == 's' || mov[0] == 'S' || mov[0] == '\0') {
            seguir = false;
        } else if (mov[0] == 'n' || mov[0] == 'N') {
            pos++;
        } else if (mov[0] == 'p' || mov[0] == 'P') {
            pos--;
        } else if ((mov[0] == 'l' || mov[0] == 'L' || mov[0] == 'q' || mov[0] == 'Q') && foto != nullptr) {
            decir(red.registrarLike(foto, mov[0] == 'l' || mov[0] == 'L'));
        }
    }
    red.mostrarReporte();
}

void opcionSugerencias(RedSocial & red) {
    red.iniciarMedicion(FUNC_SUGERENCIAS);
    char num[16];
    leerTexto("cuantas sugerencias (1 a 20): ", num, 16);
    unsigned long long k = 0;
    if (!aNumero(num, 0, longitud(num), k) || k < K_MIN_SUGERENCIAS || k > K_MAX_SUGERENCIAS) {
        std::cout << "k tiene que estar entre 1 y 20\n";
        red.mostrarReporte();
        return;
    }
    unsigned int indices[K_MAX_SUGERENCIAS];
    unsigned int comunes[K_MAX_SUGERENCIAS];
    unsigned short n = red.sugerirAmigos((unsigned short)k, indices, comunes);
    if (n == 0) {
        std::cout << "no hay sugerencias\n";
        red.mostrarReporte();
        return;
    }
    for (unsigned short i = 0; i < n; i++) {
        Usuario * u = red.getUsuario(indices[i]);
        std::cout << (i + 1) << ". " << (u == nullptr ? "?" : u->getNombreUsuario())
                  << "  en comun " << comunes[i] << "\n";
    }
    leerTexto("numero para enviar solicitud (enter no): ", num, 16);
    unsigned long long elegido = 0;
    if (num[0] != '\0' && aNumero(num, 0, longitud(num), elegido) && elegido >= 1 && elegido <= n) {
        Usuario * u = red.getUsuario(indices[elegido - 1]);
        if (u != nullptr) {
            decir(red.enviarSolicitud(u->getNombreUsuario()));
        }
    }
    red.mostrarReporte();
}

void opcionMensajes(RedSocial & red) {
    char op[8];
    leerTexto("a enviar, b leer nuevos, c buscar fechas, s volver: ", op, 8);
    Usuario * activo = red.getUsuarioActivo();
    if (activo == nullptr) {
        return;
    }
    if (op[0] == 'a' || op[0] == 'A') {
        red.iniciarMedicion(FUNC_ENVIAR_MENSAJE);
        char nombre[64];
        char texto[MAX_BYTES_MENSAJE + 1];
        leerTexto("amigo: ", nombre, 64);
        leerTexto("mensaje: ", texto, MAX_BYTES_MENSAJE + 1);
        decir(red.enviarMensaje(nombre, texto));
        red.mostrarReporte();
        return;
    }
    if (op[0] == 'b' || op[0] == 'B') {
        red.iniciarMedicion(FUNC_LEER_NUEVOS);
        unsigned int desde = activo->getPrimerNoLeido();
        unsigned int n = activo->getMensajesRecibidos().getTamanio();
        if (desde >= n) {
            std::cout << "no hay mensajes nuevos\n";
        }
        for (unsigned int i = desde; i < n; i++) {
            Mensaje * m = activo->getMensajesRecibidos()[i];
            if (m == nullptr) {
                continue;
            }
            const char * quien = m->getEmisor() == nullptr ? "?" : m->getEmisor()->getNombreUsuario();
            std::cout << quien << "  " << m->getFechaEnvio() << "  " << m->getTexto() << "\n";
        }
        char marca[8];
        leerTexto("marcar leidos? s/n: ", marca, 8);
        if (marca[0] == 's' || marca[0] == 'S') {
            decir(red.marcarMensajesLeidos());
        }
        red.mostrarReporte();
        return;
    }
    if (op[0] != 'c' && op[0] != 'C') {
        return;
    }
    red.iniciarMedicion(FUNC_RANGO_MENSAJES);
    char inicioTxt[16];
    char finTxt[16];
    leerTexto("desde AAAAMMDD: ", inicioTxt, 16);
    leerTexto("hasta AAAAMMDD: ", finTxt, 16);
    unsigned long long ai = 0;
    unsigned long long mi = 0;
    unsigned long long di = 0;
    unsigned long long af = 0;
    unsigned long long mf = 0;
    unsigned long long df = 0;
    bool ok = longitud(inicioTxt) == 8 && longitud(finTxt) == 8
        && aNumero(inicioTxt, 0, 4, ai) && aNumero(inicioTxt, 4, 6, mi) && aNumero(inicioTxt, 6, 8, di)
        && aNumero(finTxt, 0, 4, af) && aNumero(finTxt, 4, 6, mf) && aNumero(finTxt, 6, 8, df);
    FechaHora inicio((short)ai, (short)mi, (short)di);
    FechaHora fin((short)af, (short)mf, (short)df);
    unsigned int desde = 0;
    unsigned int hasta = 0;
    if (!ok || !activo->buscarRangoMensajes(inicio, fin, desde, hasta)) {
        std::cout << "no hay mensajes en ese rango\n";
    } else {
        for (unsigned int i = desde; i < hasta; i++) {
            Mensaje * m = activo->getMensajesRecibidos()[i];
            if (m != nullptr) {
                std::cout << m->getFechaEnvio() << "  " << m->getTexto() << "\n";
            }
        }
    }
    red.mostrarReporte();
}

void opcionActualizarFotos(RedSocial & red) {
    red.iniciarMedicion(FUNC_ACTUALIZAR_FOTOS);
    unsigned short codigo = red.actualizarFotosAmigos();
    decir(codigo);
    if (codigo == RES_OK) {
        mostrarAmigosDestacados(red);
    }
    red.mostrarReporte();
}

void ejecutarSesion(RedSocial & red) {
    bool seguir = true;
    while (seguir) {
        mostrarPantallaInicio(red);
        char op[8];
        leerTexto("> ", op, 8);
        if (op[0] == 's' || op[0] == 'S') {
            seguir = false;
        } else if (op[0] == '1') {
            opcionAdministrarAmistades(red);
        } else if (op[0] == '2') {
            opcionVisitarPerfil(red);
        } else if (op[0] == '3') {
            opcionSugerencias(red);
        } else if (op[0] == '4') {
            opcionMensajes(red);
        } else if (op[0] == 'n' || op[0] == 'N') {
            opcionActualizarFotos(red);
        }
    }
    red.cerrarSesion();
    red.mostrarResumenSesion();
}
