#include "redsocial.h"
#include "Usuario.h"
#include "AlbumFotos.h"
#include "Foto.h"
#include "Mensaje.h"
#include "RedAmistades.h"
#include "gestorarchivos.h"
#include "MedidorRecursos.h"
#include "utilidades.h"

#include <chrono>
#include <iostream>
#include <new>

namespace {

bool fechaAhora(FechaHora & destino) {
    long long secs = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch()).count();
    secs += DESPLAZAMIENTO_HORARIO;
    long long dias = secs / 86400;
    long long resto = secs % 86400;
    if (resto < 0) {
        resto += 86400;
        dias--;
    }
    int hora = (int)(resto / 3600);
    int minuto = (int)((resto % 3600) / 60);
    int segundo = (int)(resto % 60);
    long long z = dias + 719468;
    long long era = (z >= 0 ? z : z - 146096) / 146097;
    unsigned doe = (unsigned)(z - era * 146097);
    unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    long long y = (long long)yoe + era * 400;
    unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    unsigned mp = (5 * doy + 2) / 153;
    unsigned d = doy - (153 * mp + 2) / 5 + 1;
    unsigned m = mp < 10 ? mp + 3 : mp - 9;
    y += (m <= 2);
    MedidorRecursos::sumarUsoExterno();
    return destino.setFechaHora((short)y, (short)m, (short)d, (short)hora, (short)minuto, (short)segundo);
}

}

RedSocial::RedSocial(const char * rutaDatos, const char * subRutaFija)
    : amistades(nullptr), gestor(nullptr), medidor(nullptr), usuarioActivo(nullptr),
      grupoFotos(nullptr), cantidadGrupo(0), fotosActualizadas(false), valido(true) {
    medidor = new (std::nothrow) MedidorRecursos();
    gestor = new (std::nothrow) GestorArchivos(rutaDatos, subRutaFija);
    grupoFotos = reservar<unsigned int>(TAM_GRUPO_FOTOS);
    if (medidor == nullptr || gestor == nullptr || !gestor->estaValido() || grupoFotos == nullptr) {
        valido = false;
    }
}

RedSocial::~RedSocial() {
    for (unsigned int i = 0; i < usuarios.getTamanio(); i++) {
        delete usuarios[i];
    }
    delete amistades;
    delete gestor;
    delete medidor;
    delete[] grupoFotos;
}

bool RedSocial::estaValido() const { return valido && usuarios.estaValida(); }

bool RedSocial::cargarDatos() {
    if (!valido || gestor == nullptr) {
        return false;
    }
    bool ok = gestor->cargarDatos(*this);
    if (medidor != nullptr) {
        medidor->mostrarAviso(ok ? "carga de datos terminada" : "fallo la carga de datos");
    }
    return ok;
}

bool RedSocial::inicializarRed(unsigned int numUsuarios) {
    delete amistades;
    amistades = new (std::nothrow) RedAmistades(numUsuarios);
    if (amistades == nullptr || !amistades->estaValido()) {
        valido = false;
        return false;
    }
    return true;
}

bool RedSocial::agregarUsuario(Usuario * usuario) {
    return usuarios.agregar(usuario);
}

RedAmistades & RedSocial::getAmistades() {
    if (amistades == nullptr) {
        amistades = new (std::nothrow) RedAmistades();
    }
    return *amistades;
}

unsigned int RedSocial::getNumUsuarios() const { return usuarios.getTamanio(); }

Usuario * RedSocial::getUsuario(unsigned int indice) const {
    if (indice >= usuarios.getTamanio()) {
        return nullptr;
    }
    return usuarios[indice];
}

Usuario * RedSocial::getUsuarioActivo() const { return usuarioActivo; }
GestorArchivos & RedSocial::getGestor() { return *gestor; }
MedidorRecursos & RedSocial::getMedidor() { return *medidor; }

Usuario * RedSocial::buscarUsuario(const char * nombre) const {
    unsigned int n = usuarios.getTamanio();
    MedidorRecursos::sumarReferencia(n);
    unsigned int lo = 0;
    unsigned int hi = n;
    while (lo < hi) {
        MedidorRecursos::sumarIteraciones();
        unsigned int medio = lo + (hi - lo) / 2;
        Usuario * u = usuarios[medio];
        if (u == nullptr) {
            for (unsigned int i = 0; i < n; i++) {
                MedidorRecursos::sumarIteraciones();
                Usuario * candidato = usuarios[i];
                if (candidato != nullptr && iguales(candidato->getNombreUsuario(), nombre)) {
                    return candidato;
                }
            }
            return nullptr;
        }
        int c = comparar(u->getNombreUsuario(), nombre);
        if (c == 0) {
            return u;
        }
        if (c < 0) {
            lo = medio + 1;
        } else {
            hi = medio;
        }
    }
    return nullptr;
}

Usuario * RedSocial::buscarAmigo(const char * nombre) const {
    if (usuarioActivo == nullptr || amistades == nullptr) {
        return nullptr;
    }
    Usuario * otro = buscarUsuario(nombre);
    if (otro == nullptr) {
        return nullptr;
    }
    if (!amistades->sonAmigos(usuarioActivo->getIndice(), otro->getIndice())) {
        return nullptr;
    }
    return otro;
}

bool RedSocial::puedeVerAlbum(const Usuario & duenio, const AlbumFotos & album) const {
    bool esDueno = usuarioActivo == &duenio;
    bool esAmigo = usuarioActivo != nullptr && amistades != nullptr
        && amistades->sonAmigos(usuarioActivo->getIndice(), duenio.getIndice());
    return album.puedeVer(esDueno, esAmigo);
}

unsigned short RedSocial::iniciarSesion(const char * nombre, const char * contrasena) {
    Usuario * usuario = buscarUsuario(nombre);
    if (usuario == nullptr) {
        return RES_NO_EXISTE;
    }
    if (!usuario->verificarContrasena(contrasena)) {
        return RES_CONTRASENA_INCORRECTA;
    }
    usuarioActivo = usuario;
    fotosActualizadas = false;
    cantidadGrupo = 0;
    iniciarFotosAmigos();
    return RES_OK;
}

void RedSocial::cerrarSesion() {
    usuarioActivo = nullptr;
    cantidadGrupo = 0;
    fotosActualizadas = false;
}

bool RedSocial::iniciarFotosAmigos() {
    if (usuarioActivo == nullptr || amistades == nullptr || grupoFotos == nullptr) {
        return false;
    }
    cantidadGrupo = amistades->sortearAmigos(usuarioActivo->getIndice(), TAM_GRUPO_FOTOS,
                                             nullptr, 0, grupoFotos);
    return true;
}

unsigned short RedSocial::actualizarFotosAmigos() {
    if (usuarioActivo == nullptr) {
        return RES_SIN_SESION;
    }
    if (fotosActualizadas) {
        return RES_YA_ACTUALIZO;
    }
    if (amistades == nullptr || grupoFotos == nullptr) {
        return RES_ERROR_MEMORIA;
    }
    unsigned int nuevo[TAM_GRUPO_FOTOS];
    unsigned short n = amistades->sortearAmigos(usuarioActivo->getIndice(), TAM_GRUPO_FOTOS,
                                                grupoFotos, cantidadGrupo, nuevo);
    if (n == 0) {
        return RES_SIN_CANDIDATOS;
    }
    for (unsigned short i = 0; i < n; i++) {
        grupoFotos[i] = nuevo[i];
    }
    cantidadGrupo = n;
    fotosActualizadas = true;
    return RES_OK;
}

unsigned short RedSocial::getCantidadGrupo() const { return cantidadGrupo; }

Usuario * RedSocial::getAmigoDestacado(unsigned short posicion) const {
    if (posicion >= cantidadGrupo || grupoFotos == nullptr) {
        return nullptr;
    }
    return getUsuario(grupoFotos[posicion]);
}

unsigned short RedSocial::enviarSolicitud(const char * nombreDestino) {
    if (usuarioActivo == nullptr) {
        return RES_SIN_SESION;
    }
    Usuario * destino = buscarUsuario(nombreDestino);
    if (destino == nullptr) {
        return RES_NO_EXISTE;
    }
    if (destino == usuarioActivo) {
        return RES_ES_UNO_MISMO;
    }
    unsigned short codigo = amistades->enviarSolicitud(usuarioActivo->getIndice(), destino->getIndice());
    if (codigo == RES_OK && gestor != nullptr) {
        if (!gestor->registrarSolicitud(usuarioActivo->getIndice(), destino->getIndice())) {
            return RES_ERROR_ARCHIVO;
        }
    }
    return codigo;
}

unsigned int RedSocial::listarSolicitudes(unsigned int * emisores, unsigned int capacidad) const {
    if (usuarioActivo == nullptr || amistades == nullptr) {
        return 0;
    }
    return amistades->obtenerSolicitudes(usuarioActivo->getIndice(), emisores, capacidad);
}

unsigned short RedSocial::aceptarSolicitud(unsigned int indiceEmisor) {
    if (usuarioActivo == nullptr) {
        return RES_SIN_SESION;
    }
    if (amistades == nullptr || !amistades->aceptarSolicitud(indiceEmisor, usuarioActivo->getIndice())) {
        return RES_NO_HAY_SOLICITUD;
    }
    if (gestor != nullptr && !gestor->registrarAmistad(indiceEmisor, usuarioActivo->getIndice())) {
        return RES_ERROR_ARCHIVO;
    }
    return RES_OK;
}

unsigned short RedSocial::sugerirAmigos(unsigned short k, unsigned int * indices, unsigned int * comunes) const {
    if (usuarioActivo == nullptr || amistades == nullptr || indices == nullptr || comunes == nullptr || k == 0) {
        return 0;
    }
    if (k > K_MAX_SUGERENCIAS) {
        k = K_MAX_SUGERENCIAS;
    }
    unsigned int n = usuarios.getTamanio();
    unsigned int yo = usuarioActivo->getIndice();
    MedidorRecursos::sumarReferencia(n);
    bool * ya = reservar<bool>(n);
    unsigned int * candidatos = reservar<unsigned int>(n);
    unsigned int * vecinos = reservar<unsigned int>(n);
    if (ya == nullptr || candidatos == nullptr || vecinos == nullptr) {
        delete[] ya;
        delete[] candidatos;
        delete[] vecinos;
        return 0;
    }
    MedidorRecursos::sumarMemoriaLocal((unsigned long)n * (sizeof(bool) + 2 * sizeof(unsigned int)));
    for (unsigned int i = 0; i < n; i++) {
        ya[i] = false;
    }
    ya[yo] = true;
    unsigned int grado = amistades->getGrado(yo);
    unsigned int * amigos = reservar<unsigned int>(grado == 0 ? 1 : grado);
    if (amigos == nullptr) {
        delete[] ya;
        delete[] candidatos;
        delete[] vecinos;
        return 0;
    }
    unsigned int numAmigos = amistades->obtenerAmigos(yo, amigos, grado);
    for (unsigned int i = 0; i < numAmigos; i++) {
        ya[amigos[i]] = true;
    }
    unsigned int nc = 0;
    for (unsigned int i = 0; i < numAmigos; i++) {
        unsigned int g = amistades->getGrado(amigos[i]);
        unsigned int nv = amistades->obtenerAmigos(amigos[i], vecinos, n);
        for (unsigned int j = 0; j < nv && j < g; j++) {
            MedidorRecursos::sumarIteraciones();
            unsigned int c = vecinos[j];
            if (c < n && !ya[c]) {
                ya[c] = true;
                candidatos[nc++] = c;
            }
        }
    }
    unsigned short hallados = 0;
    for (unsigned short t = 0; t < k && nc > 0; t++) {
        unsigned int mejor = 0;
        unsigned int mejorComun = 0;
        bool alguno = false;
        for (unsigned int i = 0; i < nc; i++) {
            MedidorRecursos::sumarIteraciones();
            unsigned int enComun = amistades->contarEnComun(yo, candidatos[i]);
            if (!alguno || enComun > mejorComun) {
                mejor = i;
                mejorComun = enComun;
                alguno = true;
            }
        }
        indices[hallados] = candidatos[mejor];
        comunes[hallados] = mejorComun;
        hallados++;
        candidatos[mejor] = candidatos[nc - 1];
        nc--;
    }
    delete[] amigos;
    delete[] vecinos;
    delete[] candidatos;
    delete[] ya;
    return hallados;
}

unsigned short RedSocial::enviarMensaje(const char * nombreDestino, const char * texto) {
    if (usuarioActivo == nullptr) {
        return RES_SIN_SESION;
    }
    if (!Mensaje::textoValido(texto)) {
        return RES_TEXTO_INVALIDO;
    }
    Usuario * destino = buscarUsuario(nombreDestino);
    if (destino == nullptr) {
        return RES_NO_EXISTE;
    }
    if (destino == usuarioActivo) {
        return RES_ES_UNO_MISMO;
    }
    if (amistades == nullptr || !amistades->sonAmigos(usuarioActivo->getIndice(), destino->getIndice())) {
        return RES_NO_ES_AMIGO;
    }
    FechaHora ahora;
    if (!fechaAhora(ahora)) {
        return RES_ERROR_MEMORIA;
    }
    Mensaje * mensaje = new (std::nothrow) Mensaje(usuarioActivo, destino, texto, ahora);
    if (mensaje == nullptr || !mensaje->estaValido() || !destino->recibirMensaje(mensaje)) {
        delete mensaje;
        return RES_ERROR_MEMORIA;
    }
    usuarioActivo->registrarEnviado(mensaje);
    if (gestor == nullptr || !gestor->registrarMensaje(*mensaje)) {
        return RES_ERROR_ARCHIVO;
    }
    return RES_OK;
}

unsigned short RedSocial::marcarMensajesLeidos() {
    if (usuarioActivo == nullptr) {
        return RES_SIN_SESION;
    }
    unsigned int nuevo = usuarioActivo->marcarTodosLeidos();
    if (gestor != nullptr && !gestor->registrarLecturas(usuarioActivo->getIndice(), nuevo)) {
        return RES_ERROR_ARCHIVO;
    }
    return RES_OK;
}

unsigned short RedSocial::registrarLike(Foto * foto, bool dar) {
    if (usuarioActivo == nullptr) {
        return RES_SIN_SESION;
    }
    if (foto == nullptr) {
        return RES_NO_EXISTE;
    }
    if (dar) {
        ++(*foto);
    } else {
        --(*foto);
    }
    if (gestor != nullptr && !gestor->registrarLikes(foto->getId(), foto->getLikes())) {
        return RES_ERROR_ARCHIVO;
    }
    return RES_OK;
}

void RedSocial::iniciarMedicion(unsigned short idFuncionalidad) {
    if (medidor != nullptr) {
        medidor->iniciarFuncionalidad(idFuncionalidad);
    }
}

void RedSocial::mostrarReporte() {
    if (medidor != nullptr) {
        medidor->finalizarFuncionalidad(calcularMemoria(), calcularMemoriaReferencia());
        //lo que se imprime despues del reporte no pertenece a la funcionalidad
        medidor->iniciarFuncionalidad(FUNC_CARGA);
    }
}

void RedSocial::mostrarResumenSesion() {
    if (medidor != nullptr) {
        medidor->mostrarResumenSesion(calcularMemoria(), calcularMemoriaReferencia());
    }
}

unsigned long RedSocial::calcularMemoria() const {
    unsigned long total = sizeof(*this);
    total += usuarios.calcularMemoria();
    for (unsigned int i = 0; i < usuarios.getTamanio(); i++) {
        if (usuarios[i] != nullptr) {
            total += usuarios[i]->calcularMemoria();
        }
    }
    if (amistades != nullptr) {
        total += amistades->calcularMemoria();
    }
    if (gestor != nullptr) {
        total += gestor->calcularMemoria();
    }
    if (medidor != nullptr) {
        total += sizeof(*medidor);
    }
    if (grupoFotos != nullptr) {
        total += TAM_GRUPO_FOTOS * sizeof(unsigned int);
    }
    return total;
}

unsigned long RedSocial::calcularMemoriaReferencia() const {
    unsigned long total = 0;
    if (amistades != nullptr) {
        total += amistades->calcularMemoriaReferencia();
    }
    for (unsigned int i = 0; i < usuarios.getTamanio(); i++) {
        Usuario * usuario = usuarios[i];
        if (usuario == nullptr) {
            continue;
        }
        total += usuario->getMensajesRecibidos().getTamanio() * MAX_BYTES_MENSAJE;
    }
    return total;
}
