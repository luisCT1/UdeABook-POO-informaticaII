#include "Usuario.h"
#include "AlbumFotos.h"
#include "Foto.h"
#include "Mensaje.h"
#include "configuracion.h"
#include "utilidades.h"

#include <iostream>

namespace {

void leerTexto(const char * aviso, char * buffer, unsigned int cap) {
    std::cout << aviso;
    if (!std::cin.getline(buffer, cap)) {
        std::cin.clear();
        buffer[0] = '\0';
    }
}

void mostrarUsuario(const Usuario & usuario) {
    std::cout << usuario.getNombreUsuario() << std::endl;
    std::cout << "nacio: " << usuario.getFechaNacimiento() << std::endl;
    std::cout << "ciudad: " << usuario.getCiudad() << ", " << usuario.getPais() << std::endl;
    std::cout << "correo: " << usuario.getCorreo() << std::endl;
    std::cout << "albumes: " << usuario.getNumAlbumes() << std::endl;
    std::cout << "mensajes sin leer: " << usuario.contarNoLeidos() << std::endl;
}

void mostrarAlbumes(Usuario & dueno, bool esDueno, bool esAmigo) {
    if (dueno.getNumAlbumes() == 0) {
        std::cout << "no tiene albumes" << std::endl;
        return;
    }
    for (unsigned int i = 0; i < dueno.getNumAlbumes(); i++) {
        AlbumFotos * album = dueno.buscarAlbum(i);
        if (album == nullptr) {
            continue;
        }
        std::cout << i << ". " << album->getNombre() << " (" << album->getPrivacidad() << ")";
        if (!album->puedeVer(esDueno, esAmigo)) {
            std::cout << " no se puede ver" << std::endl;
            continue;
        }
        std::cout << std::endl;
        for (unsigned int f = 0; f < album->getNumFotos(); f++) {
            Foto * foto = album->getFoto(f);
            std::cout << "   " << foto->getNombreArchivo() << ".png  likes: " << foto->getLikes() << std::endl;
        }
    }
}

void mostrarMensajes(Usuario & usuario) {
    unsigned int n = usuario.getMensajesRecibidos().getTamanio();
    if (n == 0) {
        std::cout << "bandeja vacia" << std::endl;
        return;
    }
    for (unsigned int i = 0; i < n; i++) {
        Mensaje * m = usuario.getMensajesRecibidos()[i];
        if (m == nullptr || m->getEmisor() == nullptr) {
            continue;
        }
        std::cout << m->getFechaEnvio() << " " << m->getEmisor()->getNombreUsuario();
        if (!m->isLeido()) {
            std::cout << " (nuevo)";
        }
        std::cout << ": " << m->getTexto() << std::endl;
    }
}

}

int main() {
    FechaHora nacimientoAna(2000, 1, 15);
    FechaHora nacimientoBruno(1999, 5, 5);
    FechaHora inscripcion(2024, 1, 2);
    Usuario ana("ana", "clave123", nacimientoAna, inscripcion, 0);
    Usuario bruno("bruno", "clave123", nacimientoBruno, inscripcion, 1);
    ana.setCorreo("ana@mail.com");
    ana.setCiudad("Medellin");
    ana.setPais("Colombia");
    bruno.setCorreo("bruno@mail.com");
    bruno.setCiudad("Bogota");
    bruno.setPais("Colombia");

    AlbumFotos * viaje = new AlbumFotos("viaje", cfg::PRIVACIDAD_AMIGOS);
    AlbumFotos * secreto = new AlbumFotos("secreto", cfg::PRIVACIDAD_PRIVADA);
    viaje->agregarFoto(new Foto(0, FechaHora(2026, 8, 15, 12, 0, 0), 1000, "playa1", 2));
    viaje->agregarFoto(new Foto(1, FechaHora(2026, 8, 16, 12, 0, 0), 1200, "playa2", 0));
    secreto->agregarFoto(new Foto(2, FechaHora(2026, 1, 1, 10, 0, 0), 500, "oculto", 0));
    ana.agregarAlbum(viaje);
    ana.agregarAlbum(secreto);

    Mensaje * hola = new Mensaje(&bruno, &ana, "hola ana", FechaHora(2026, 8, 1, 12, 0, 0));
    Mensaje * luego = new Mensaje(&bruno, &ana, "nos vemos", FechaHora(2026, 8, 20, 18, 30, 0));
    ana.recibirMensaje(hola);
    ana.recibirMensaje(luego);
    bruno.registrarEnviado(hola);
    bruno.registrarEnviado(luego);
    ana.setPrimerNoLeido(0);

    std::cout << "prueba de las clases de datos" << std::endl;
    std::cout << "ana / clave123" << std::endl;

    char nombre[64];
    char clave[64];
    leerTexto("usuario: ", nombre, 64);
    leerTexto("contrasena: ", clave, 64);
    if (!util::iguales(nombre, "ana") || !ana.verificarContrasena(clave)) {
        std::cout << "solo esta de prueba el usuario ana" << std::endl;
        return 0;
    }

    bool seguir = true;
    while (seguir) {
        std::cout << std::endl;
        std::cout << "1. ver perfil" << std::endl;
        std::cout << "2. ver albumes" << std::endl;
        std::cout << "3. dar like a la primera foto" << std::endl;
        std::cout << "4. ver mensajes" << std::endl;
        std::cout << "5. buscar mensajes de agosto 2026" << std::endl;
        std::cout << "6. marcar leidos" << std::endl;
        std::cout << "s. salir" << std::endl;
        char op[8];
        leerTexto("> ", op, 8);
        if (op[0] == 's' || op[0] == 'S') {
            seguir = false;
        } else if (op[0] == '1') {
            mostrarUsuario(ana);
        } else if (op[0] == '2') {
            mostrarAlbumes(ana, true, true);
        } else if (op[0] == '3') {
            Foto * foto = viaje->getFoto(0);
            ++(*foto);
            std::cout << foto->getNombreArchivo() << " ahora tiene " << foto->getLikes() << " likes" << std::endl;
        } else if (op[0] == '4') {
            mostrarMensajes(ana);
        } else if (op[0] == '5') {
            FechaHora inicio(2026, 8, 1);
            FechaHora fin(2026, 8, 31);
            unsigned int desde = 0;
            unsigned int hasta = 0;
            if (!ana.buscarRangoMensajes(inicio, fin, desde, hasta)) {
                std::cout << "no hay mensajes en agosto" << std::endl;
            } else {
                for (unsigned int i = desde; i < hasta; i++) {
                    Mensaje * m = ana.getMensajesRecibidos()[i];
                    std::cout << m->getFechaEnvio() << " " << m->getTexto() << std::endl;
                }
            }
        } else if (op[0] == '6') {
            ana.marcarTodosLeidos();
            std::cout << "sin leer: " << ana.contarNoLeidos() << std::endl;
        }
    }
    return 0;
}
