#include "menu.h"
#include "redsocial.h"
#include "gestorarchivos.h"
#include "Usuario.h"
#include "AlbumFotos.h"
#include "Foto.h"
#include "Mensaje.h"
#include "MedidorRecursos.h"
#include "utilidades.h"
#include "configuracion.h"

#include <fstream>
#include <iostream>

namespace {

bool entradaCerrada = false;

void leerTexto(const char * aviso, char * buffer, unsigned int cap) {
    std::cout << aviso;
    MedidorRecursos::sumarMemoriaLocal(cap);
    if (std::cin.getline(buffer, cap)) {
        return;
    }
    if (std::cin.eof()) {
        entradaCerrada = true;
    } else {
        std::cin.clear();
        std::cin.ignore(1000000, '\n');
    }
    buffer[0] = '\0';
}

void decir(unsigned short codigo) {
    std::cout << descripcionResultado(codigo) << "\n";
}

void pausar() {
    char entra[8];
    leerTexto("enter para seguir: ", entra, 8);
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
    std::cout << "\nfotos de amigos:\n";
    if (n == 0) {
        std::cout << "  no hay amigos para mostrar\n";
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
        std::cout << "  " << (i + 1) << ". " << amigo->getNombreUsuario() << "\n";
        std::cout << "     " << ruta << "\n";
    }
}

void cerrarFuncionalidad(RedSocial & red) {
    std::cout << "\n--- recursos usados ---\n";
    red.mostrarReporte();
    pausar();
}

}

const char * buscarCarpeta(const char * relativa) {
    static char ruta[512];
    char intento[512];
    copiarEn(relativa, intento, 512);
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
    copiarEn(relativa, ruta, 512);
    return ruta;
}

const char * seleccionarDataset() {
    std::cout << "\nque base de datos quieres?\n";
    std::cout << "  1. pequenio   (100 usuarios)\n";
    std::cout << "  2. mediano    (1000 usuarios)\n";
    std::cout << "  3. grande     (5000 usuarios)\n";
    char op[8];
    leerTexto("> ", op, 8);
    if (op[0] == '1') {
        return buscarCarpeta("datos/pequenio");
    }
    if (op[0] == '3') {
        return buscarCarpeta("datos/grande");
    }
    //enter o 2 = mediano, que es el de la entrega
    return buscarCarpeta("datos/mediano");
}

bool pantallaDeIngreso(RedSocial & red) {
    std::cout << "\n";
    std::cout << " ========================================\n";
    std::cout << "\n";
    std::cout << "              UdeABook\n";
    std::cout << "\n";
    std::cout << " ========================================\n";
    std::cout << "         red social de la udea\n\n";

    red.iniciarMedicion(FUNC_INGRESO);
    while (true) {
        char nombre[64];
        char clave[64];
        leerTexto("usuario: ", nombre, 64);
        if (nombre[0] == '\0') {
            red.mostrarReporte();
            return false;
        }
        leerTexto("contrasena: ", clave, 64);
        unsigned short codigo = red.iniciarSesion(nombre, clave);
        if (codigo == RES_OK) {
            mostrarPantallaInicio(red);
            cerrarFuncionalidad(red);
            return true;
        }
        decir(codigo);
    }
}

void mostrarPantallaInicio(RedSocial & red) {
    Usuario * activo = red.getUsuarioActivo();
    std::cout << "\n";
    std::cout << " ========================================\n";
    std::cout << "              UdeABook\n";
    std::cout << " ========================================\n";
    std::cout << "hola " << (activo == nullptr ? "?" : activo->getNombreUsuario()) << "\n";
    if (activo != nullptr) {
        std::cout << "mensajes nuevos: " << activo->contarNoLeidos() << "\n";
    }
    mostrarAmigosDestacados(red);
    std::cout << "\nque quieres hacer?\n";
    std::cout << "  1. amigos y solicitudes\n";
    std::cout << "  2. ver perfil de un amigo\n";
    std::cout << "  3. personas que quizas conozcas\n";
    std::cout << "  4. mensajes\n";
    std::cout << "  n. otras fotos de amigos\n";
    std::cout << "  s. salir\n";
}

void opcionAmigos(RedSocial & red) {
    std::cout << "\namigos y solicitudes\n";
    std::cout << "  a. agregar amigo\n";
    std::cout << "  b. solicitudes pendientes\n";
    std::cout << "  s. volver\n";
    char op[8];
    leerTexto("> ", op, 8);

    if (op[0] == 'a' || op[0] == 'A') {
        red.iniciarMedicion(FUNC_ENVIAR_SOLICITUD);
        char nombre[64];
        leerTexto("a quien le envias la solicitud? ", nombre, 64);
        decir(red.enviarSolicitud(nombre));
        cerrarFuncionalidad(red);
        return;
    }
    if (op[0] != 'b' && op[0] != 'B') {
        return;
    }

    red.iniciarMedicion(FUNC_ACEPTAR_SOLICITUD);
    unsigned int emisores[128];
    unsigned int n = red.listarSolicitudes(emisores, 128);
    if (n == 0) {
        std::cout << "no tienes solicitudes\n";
        cerrarFuncionalidad(red);
        return;
    }
    std::cout << "te enviaron solicitud:\n";
    for (unsigned int i = 0; i < n; i++) {
        Usuario * emisor = red.getUsuario(emisores[i]);
        std::cout << "  " << (i + 1) << ". "
                  << (emisor == nullptr ? "?" : emisor->getNombreUsuario()) << "\n";
    }
    char num[16];
    leerTexto("numero a aceptar (enter cancela): ", num, 16);
    unsigned long long elegido = 0;
    if (num[0] != '\0' && aNumero(num, 0, longitud(num), elegido) && elegido >= 1 && elegido <= n) {
        decir(red.aceptarSolicitud(emisores[elegido - 1]));
    }
    cerrarFuncionalidad(red);
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
        cerrarFuncionalidad(red);
        return;
    }

    std::cout << "\n" << visitado->getNombreUsuario() << "\n";
    std::cout << (visitado->getCiudad() == nullptr ? "-" : visitado->getCiudad())
              << ", " << (visitado->getPais() == nullptr ? "-" : visitado->getPais()) << "\n";

    unsigned int visibles[64];
    unsigned int nv = 0;
    std::cout << "\nalbumes:\n";
    for (unsigned int i = 0; i < visitado->getNumAlbumes() && nv < 64; i++) {
        AlbumFotos * album = visitado->buscarAlbum(i);
        if (album != nullptr && red.puedeVerAlbum(*visitado, *album)) {
            visibles[nv++] = i;
            std::cout << "  " << nv << ". " << album->getNombre()
                      << " (" << album->getPrivacidad() << ") "
                      << album->getNumFotos() << " fotos\n";
        }
    }
    if (nv == 0) {
        std::cout << "no hay albumes que puedas ver\n";
        cerrarFuncionalidad(red);
        return;
    }

    char num[16];
    leerTexto("cual album? (enter vuelve): ", num, 16);
    unsigned long long elegido = 0;
    if (num[0] == '\0' || !aNumero(num, 0, longitud(num), elegido) || elegido < 1 || elegido > nv) {
        cerrarFuncionalidad(red);
        return;
    }

    AlbumFotos * album = visitado->buscarAlbum(visibles[elegido - 1]);
    int pos = 0;
    bool seguir = album != nullptr && album->getNumFotos() > 0;
    if (!seguir) {
        std::cout << "ese album esta vacio\n";
    } else {
        std::cout << "n siguiente | p anterior | l me gusta | q quitar | s salir\n";
    }
    while (seguir) {
        Foto * foto = album->getFotoCircular(pos);
        char ruta[256];
        if (foto != nullptr && !red.getGestor().armarRutaFoto(*visitado, *album, *foto, ruta, 256)) {
            copiarEn("?", ruta, 256);
        }
        if (foto != nullptr) {
            std::cout << "\n" << foto->getNombreArchivo() << ".png\n";
            std::cout << foto->getFechaCaptura() << "\n";
            std::cout << "likes: " << foto->getLikes() << "\n";
            std::cout << ruta << "\n";
        }
        char mov[8];
        leerTexto("> ", mov, 8);
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
    cerrarFuncionalidad(red);
}

void opcionSugerencias(RedSocial & red) {
    red.iniciarMedicion(FUNC_SUGERENCIAS);
    char num[16];
    leerTexto("cuantas personas quieres ver? (1 a 20): ", num, 16);
    unsigned long long k = 0;
    if (!aNumero(num, 0, longitud(num), k) || k < K_MIN_SUGERENCIAS || k > K_MAX_SUGERENCIAS) {
        std::cout << "tiene que ser entre 1 y 20\n";
        cerrarFuncionalidad(red);
        return;
    }
    unsigned int indices[K_MAX_SUGERENCIAS];
    unsigned int comunes[K_MAX_SUGERENCIAS];
    unsigned short n = red.sugerirAmigos((unsigned short)k, indices, comunes);
    if (n == 0) {
        std::cout << "no hay sugerencias por ahora\n";
        cerrarFuncionalidad(red);
        return;
    }
    std::cout << "\npersonas que quizas conozcas:\n";
    for (unsigned short i = 0; i < n; i++) {
        Usuario * u = red.getUsuario(indices[i]);
        std::cout << "  " << (i + 1) << ". "
                  << (u == nullptr ? "?" : u->getNombreUsuario())
                  << "  (" << comunes[i] << " amigos en comun)\n";
    }
    leerTexto("enviar solicitud a cual? (enter no): ", num, 16);
    unsigned long long elegido = 0;
    if (num[0] != '\0' && aNumero(num, 0, longitud(num), elegido) && elegido >= 1 && elegido <= n) {
        Usuario * u = red.getUsuario(indices[elegido - 1]);
        if (u != nullptr) {
            decir(red.enviarSolicitud(u->getNombreUsuario()));
        }
    }
    cerrarFuncionalidad(red);
}

void opcionMensajes(RedSocial & red) {
    std::cout << "\nmensajes\n";
    std::cout << "  a. escribir a un amigo\n";
    std::cout << "  b. leer los nuevos\n";
    std::cout << "  c. buscar por fechas\n";
    std::cout << "  s. volver\n";
    char op[8];
    leerTexto("> ", op, 8);
    Usuario * activo = red.getUsuarioActivo();
    if (activo == nullptr) {
        return;
    }

    if (op[0] == 'a' || op[0] == 'A') {
        red.iniciarMedicion(FUNC_ENVIAR_MENSAJE);
        char nombre[64];
        char texto[MAX_BYTES_MENSAJE + 1];
        leerTexto("para quien: ", nombre, 64);
        leerTexto("mensaje: ", texto, MAX_BYTES_MENSAJE + 1);
        decir(red.enviarMensaje(nombre, texto));
        cerrarFuncionalidad(red);
        return;
    }

    if (op[0] == 'b' || op[0] == 'B') {
        red.iniciarMedicion(FUNC_LEER_NUEVOS);
        unsigned int desde = activo->getPrimerNoLeido();
        unsigned int n = activo->getMensajesRecibidos().getTamanio();
        if (desde >= n) {
            std::cout << "no tienes mensajes nuevos\n";
        }
        for (unsigned int i = desde; i < n; i++) {
            Mensaje * m = activo->getMensajesRecibidos()[i];
            if (m == nullptr) {
                continue;
            }
            const char * quien = m->getEmisor() == nullptr ? "?" : m->getEmisor()->getNombreUsuario();
            std::cout << quien << " - " << m->getFechaEnvio() << "\n";
            std::cout << "  " << m->getTexto() << "\n";
        }
        if (desde < n) {
            decir(red.marcarMensajesLeidos());
        }
        cerrarFuncionalidad(red);
        return;
    }

    if (op[0] != 'c' && op[0] != 'C') {
        return;
    }

    red.iniciarMedicion(FUNC_RANGO_MENSAJES);
    char inicioTxt[16];
    char finTxt[16];
    std::cout << "escribe las fechas asi: AAAAMMDD\n";
    leerTexto("desde: ", inicioTxt, 16);
    leerTexto("hasta: ", finTxt, 16);
    unsigned long long ai = 0;
    unsigned long long mi = 0;
    unsigned long long di = 0;
    unsigned long long af = 0;
    unsigned long long mf = 0;
    unsigned long long df = 0;
    bool ok = longitud(inicioTxt) == 8 && longitud(finTxt) == 8
        && aNumero(inicioTxt, 0, 4, ai) && aNumero(inicioTxt, 4, 6, mi) && aNumero(inicioTxt, 6, 8, di)
        && aNumero(finTxt, 0, 4, af) && aNumero(finTxt, 4, 6, mf) && aNumero(finTxt, 6, 8, df);
    FechaHora inicio;
    FechaHora fin;
    bool fechasBien = ok && inicio.setFechaHora((short)ai, (short)mi, (short)di) && fin.setFechaHora((short)af, (short)mf, (short)df);
    unsigned int desde = 0;
    unsigned int hasta = 0;
    if (!fechasBien) {
        std::cout << "fecha invalida: escribe AAAAMMDD con una fecha real, por ejemplo 20260930\n";
    } else if (!activo->buscarRangoMensajes(inicio, fin, desde, hasta)) {
        std::cout << "no hay mensajes en esas fechas\n";
    } else {
        for (unsigned int i = desde; i < hasta; i++) {
            Mensaje * m = activo->getMensajesRecibidos()[i];
            if (m != nullptr) {
                const char * quien = m->getEmisor() == nullptr ? "?" : m->getEmisor()->getNombreUsuario();
                std::cout << quien << " - " << m->getFechaEnvio() << "\n";
                std::cout << "  " << m->getTexto() << "\n";
            }
        }
    }
    cerrarFuncionalidad(red);
}

void opcionActualizarFotos(RedSocial & red) {
    red.iniciarMedicion(FUNC_ACTUALIZAR_FOTOS);
    unsigned short codigo = red.actualizarFotosAmigos();
    decir(codigo);
    if (codigo == RES_OK) {
        mostrarAmigosDestacados(red);
    }
    cerrarFuncionalidad(red);
}

void ejecutarSesion(RedSocial & red) {
    bool seguir = true;
    while (seguir && !entradaCerrada) {
        mostrarPantallaInicio(red);
        char op[8];
        leerTexto("> ", op, 8);
        if (op[0] == 's' || op[0] == 'S') {
            seguir = false;
        } else if (op[0] == '1') {
            opcionAmigos(red);
        } else if (op[0] == '2') {
            opcionVisitarPerfil(red);
        } else if (op[0] == '3') {
            opcionSugerencias(red);
        } else if (op[0] == '4') {
            opcionMensajes(red);
        } else if (op[0] == 'n' || op[0] == 'N') {
            opcionActualizarFotos(red);
        } else {
            std::cout << "opcion invalida\n";
        }
    }
    red.cerrarSesion();
    std::cout << "\nresumen de la sesion\n";
    red.mostrarResumenSesion();
}
