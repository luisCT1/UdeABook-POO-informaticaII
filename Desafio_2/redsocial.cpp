#include "RedSocial.h"
#include "Usuario.h"
#include "AlbumFotos.h"
#include "Fotos.h"
#include "Mensajes.h"
#include "FechaHora.h"
#include "RedAmistades.h"
#include "GestorArchivos.h"
#include "MedidorRecursos.h"
#include "utilidades.h"
#include "configuracion.h"
#include <chrono>

using namespace std;

static FechaHora ahoraLocal() {
    MedidorRecursos::sumarUsoExterno();
    long long segundos = chrono::duration_cast<chrono::seconds>(chrono::system_clock::now().time_since_epoch()).count();
    segundos += DESPLAZAMIENTO_HORARIO;
    long long dias = segundos / 86400;
    long long resto = segundos % 86400;
    if (resto < 0) {
        resto += 86400;
        dias--;
    }
    long long z = dias + 719468;
    long long era = (z >= 0 ? z : z - 146096) / 146097;
    long long doe = z - era * 146097;
    long long yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    long long anio = yoe + era * 400;
    long long doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    long long mp = (5 * doy + 2) / 153;
    long long dia = doy - (153 * mp + 2) / 5 + 1;
    long long mes = (mp < 10) ? mp + 3 : mp - 9;
    if (mes <= 2) {
        anio++;
    }
    return FechaHora((short)anio, (short)mes, (short)dia,
                     (short)(resto / 3600), (short)((resto % 3600) / 60), (short)(resto % 60));
}

RedSocial::RedSocial(const char * rutaDatos, const char * subRutaFija)
    : amistades(nullptr), gestor(nullptr), medidor(nullptr), usuarioActivo(nullptr), grupoFotos(nullptr),
    cantidadGrupo(0), fotosActualizadas(false), valido(true) {
    medidor = new (nothrow) MedidorRecursos();
    gestor = new (nothrow) GestorArchivos(rutaDatos, subRutaFija);
    grupoFotos = reservar<unsigned int>(TAM_GRUPO_FOTOS);
    if (medidor == nullptr || gestor == nullptr || !gestor->estaValido() || grupoFotos == nullptr) {
        valido = false;
    }
}

RedSocial::~RedSocial() {
    for (unsigned int i = 0; i < usuarios.gettamaño(); i++) {
        delete usuarios[i];
    }
    delete amistades;
    delete gestor;
    delete medidor;
    delete[] grupoFotos;
}

bool RedSocial::estaValido() const { return valido; }

bool RedSocial::cargarDatos() {
    if (!valido) {
        return false;
    }
    medidor->iniciarFuncionalidad(FUNC_CARGA);
    return gestor->cargarDatos(*this);
}

bool RedSocial::inicializarRed(unsigned int numUsuarios) {
    RedAmistades * nueva = new (nothrow) RedAmistades(numUsuarios);
    if (nueva == nullptr || !nueva->estaValido()) {
        delete nueva;
        return false;
    }
    delete amistades;
    amistades = nueva;
    return true;
}

bool RedSocial::agregarUsuario(Usuario * usuario) {
    if (usuario == nullptr) {
        return false;
    }
    return usuarios.agregar(usuario);
}

RedAmistades & RedSocial::getAmistades() { return *amistades; }

unsigned int RedSocial::getNumUsuarios() const { return usuarios.gettamaño(); }

Usuario * RedSocial::getUsuario(unsigned int indice) const {
    return (indice < usuarios.gettamaño()) ? usuarios[indice] : nullptr;
}

Usuario * RedSocial::getUsuarioActivo() const { return usuarioActivo; }
GestorArchivos & RedSocial::getGestor() { return *gestor; }
MedidorRecursos & RedSocial::getMedidor() { return *medidor; }

Usuario * RedSocial::buscarUsuario(const char * nombre) const {
    unsigned int bajo = 0;
    unsigned int alto = usuarios.gettamaño();
    Usuario * hallado = nullptr;
    unsigned int posicion = 0;
    while (bajo < alto) {
        unsigned int medio = bajo + (alto - bajo) / 2;
        int orden = comparar(nombre, usuarios[medio]->getNombreUsuario());
        if (orden == 0) {
            hallado = usuarios[medio];
            posicion = medio;
            break;
        }
        if (orden < 0) {
            alto = medio;
        } else {
            bajo = medio + 1;
        }
    }
    MedidorRecursos::sumarReferencia(hallado != nullptr ? posicion + 1 : usuarios.gettamaño());
    return hallado;
}

Usuario * RedSocial::buscarAmigo(const char * nombre) const {
    if (usuarioActivo == nullptr || amistades == nullptr) {
        return nullptr;
    }
    Usuario * candidato = buscarUsuario(nombre);
    if (candidato == nullptr || !amistades->sonAmigos(usuarioActivo->getIndice(), candidato->getIndice())) {
        return nullptr;
    }
    return candidato;
}

bool RedSocial::puedeVerAlbum(const Usuario & duenio, const AlbumFotos & album) const {
    bool esDueno = false;
    bool esAmigo = false;
    if (usuarioActivo != nullptr) {
        esDueno = (usuarioActivo->getIndice() == duenio.getIndice());
        esAmigo = (amistades != nullptr) && amistades->sonAmigos(usuarioActivo->getIndice(), duenio.getIndice());
    }
    return album.puedeVer(esDueno, esAmigo);
}

unsigned short RedSocial::iniciarSesion(const char * nombre, const char * contrasena) {
    Usuario * u = buscarUsuario(nombre);
    if (u == nullptr) {
        return RES_NO_EXISTE;
    }
    if (!u->verificarContrasena(contrasena)) {
        return RES_CONTRASENA_INCORRECTA;
    }
    usuarioActivo = u;
    fotosActualizadas = false;
    iniciarFotosAmigos();
    MedidorRecursos::sumarMemoriaLocal(sizeof(nombre) + sizeof(contrasena) + sizeof(u));
    return RES_OK;
}

void RedSocial::cerrarSesion() {
    usuarioActivo = nullptr;
    cantidadGrupo = 0;
    fotosActualizadas = false;
}

bool RedSocial::iniciarFotosAmigos() {
    cantidadGrupo = 0;
    if (usuarioActivo == nullptr || amistades == nullptr) {
        return false;
    }
    cantidadGrupo = amistades->sortearAmigos(usuarioActivo->getIndice(), TAM_GRUPO_FOTOS, nullptr, 0, grupoFotos);
    return cantidadGrupo > 0;
}

unsigned short RedSocial::actualizarFotosAmigos() {
    if (usuarioActivo == nullptr || amistades == nullptr) {
        return RES_SIN_SESION;
    }
    if (fotosActualizadas) {
        return RES_YA_ACTUALIZO;
    }
    unsigned int nuevos[TAM_GRUPO_FOTOS];
    unsigned short cuantos = amistades->sortearAmigos(usuarioActivo->getIndice(), TAM_GRUPO_FOTOS,
                                                      grupoFotos, cantidadGrupo, nuevos);
    if (cuantos == 0) {
        return RES_SIN_CANDIDATOS;
    }
    for (unsigned short i = 0; i < cuantos; i++) {
        grupoFotos[i] = nuevos[i];
    }
    MedidorRecursos::sumarIteraciones(cuantos);
    MedidorRecursos::sumarMemoriaLocal(sizeof(nuevos) + sizeof(cuantos));
    cantidadGrupo = cuantos;
    fotosActualizadas = true;
    return RES_OK;
}

unsigned short RedSocial::getCantidadGrupo() const { return cantidadGrupo; }

Usuario * RedSocial::getAmigoDestacado(unsigned short posicion) const {
    if (posicion >= cantidadGrupo) {
        return nullptr;
    }
    return usuarios[grupoFotos[posicion]];
}

unsigned short RedSocial::enviarSolicitud(const char * nombreDestino) {
    if (usuarioActivo == nullptr || amistades == nullptr) {
        return RES_SIN_SESION;
    }
    Usuario * destino = buscarUsuario(nombreDestino);
    if (destino == nullptr) {
        return RES_NO_EXISTE;
    }
    unsigned int yo = usuarioActivo->getIndice();
    unsigned int el = destino->getIndice();
    unsigned short resultado = amistades->enviarSolicitud(yo, el);
    MedidorRecursos::sumarMemoriaLocal(sizeof(nombreDestino) + sizeof(destino) + sizeof(yo) + sizeof(el) + sizeof(resultado));
    if (resultado != RES_OK) {
        return resultado;
    }
    if (!gestor->registrarSolicitud(yo, el)) {
        return RES_ERROR_ARCHIVO;
    }
    return RES_OK;
}

unsigned int RedSocial::listarSolicitudes(unsigned int * emisores, unsigned int capacidad) const {
    if (usuarioActivo == nullptr || amistades == nullptr || emisores == nullptr) {
        return 0;
    }
    return amistades->obtenerSolicitudes(usuarioActivo->getIndice(), emisores, capacidad);
}

unsigned short RedSocial::aceptarSolicitud(unsigned int indiceEmisor) {
    if (usuarioActivo == nullptr || amistades == nullptr) {
        return RES_SIN_SESION;
    }
    if (indiceEmisor >= usuarios.gettamaño()) {
        return RES_NO_EXISTE;
    }
    unsigned int yo = usuarioActivo->getIndice();
    if (!amistades->tieneSolicitud(indiceEmisor, yo)) {
        return RES_NO_HAY_SOLICITUD;
    }
    MedidorRecursos::sumarReferencia(amistades->contarSolicitudes(yo));
    if (!amistades->aceptarSolicitud(indiceEmisor, yo)) {
        return RES_YA_SON_AMIGOS;
    }
    MedidorRecursos::sumarMemoriaLocal(sizeof(indiceEmisor) + sizeof(yo));
    if (!gestor->registrarAmistad(indiceEmisor, yo)) {
        return RES_ERROR_ARCHIVO;
    }
    return RES_OK;
}
unsigned short RedSocial::sugerirAmigos(unsigned short k, unsigned int * indices, unsigned int * comunes) const {
    if (usuarioActivo == nullptr || amistades == nullptr || indices == nullptr || comunes == nullptr) {
        return 0;
    }
    if (k < K_MIN_SUGERENCIAS) {
        k = K_MIN_SUGERENCIAS;
    }
    if (k > K_MAX_SUGERENCIAS) {
        k = K_MAX_SUGERENCIAS;
    }
    unsigned int yo = usuarioActivo->getIndice();
    unsigned int total = usuarios.gettamaño();
    unsigned short encontrados = 0;
    unsigned long long vueltas = 0;
    for (unsigned int c = 0; c < total; c++) {
        vueltas++;
        if (c == yo || amistades->sonAmigos(yo, c) || amistades->tieneSolicitud(yo, c) || amistades->tieneSolicitud(c, yo)) {
            continue;
        }
        unsigned int enComun = amistades->contarEnComun(yo, c);
        if (enComun == 0) {
            continue;
        }
        if (encontrados == k && enComun <= comunes[k - 1]) {
            continue;
        }
        unsigned short pos = (encontrados < k) ? encontrados : (unsigned short)(k - 1);
        while (pos > 0 && comunes[pos - 1] < enComun) {
            comunes[pos] = comunes[pos - 1];
            indices[pos] = indices[pos - 1];
            pos--;
            vueltas++;
        }
        comunes[pos] = enComun;
        indices[pos] = c;
        if (encontrados < k) {
            encontrados++;
        }
    }
    MedidorRecursos::sumarIteraciones(vueltas);
    MedidorRecursos::sumarMemoriaLocal(sizeof(k) + sizeof(indices) + sizeof(comunes) + sizeof(yo) + sizeof(total)
                                       + sizeof(encontrados) + sizeof(vueltas));
    return encontrados;
}

unsigned short RedSocial::enviarMensaje(const char * nombreDestino, const char * texto) {
    if (usuarioActivo == nullptr || amistades == nullptr) {
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
    if (!amistades->sonAmigos(usuarioActivo->getIndice(), destino->getIndice())) {
        return RES_NO_ES_AMIGO;
    }
    Mensaje * mensaje = new (nothrow) Mensaje(usuarioActivo, destino, texto, ahoraLocal());
    if (mensaje == nullptr || !mensaje->estaValido()) {
        delete mensaje;
        return RES_ERROR_MEMORIA;
    }
    if (!destino->recibirMensaje(mensaje)) {
        delete mensaje;
        return RES_ERROR_MEMORIA;
    }
    usuarioActivo->registrarEnviado(mensaje);
    MedidorRecursos::sumarMemoriaLocal(sizeof(nombreDestino) + sizeof(texto) + sizeof(destino) + sizeof(mensaje));
    if (!gestor->registrarMensaje(*mensaje)) {
        return RES_ERROR_ARCHIVO;
    }
    return RES_OK;
}

unsigned short RedSocial::marcarMensajesLeidos() {
    if (usuarioActivo == nullptr) {
        return RES_SIN_SESION;
    }
    if (usuarioActivo->contarNoLeidos() == 0) {
        return RES_OK;
    }
    unsigned int nuevoPrimerNoLeido = usuarioActivo->marcarTodosLeidos();
    MedidorRecursos::sumarMemoriaLocal(sizeof(nuevoPrimerNoLeido));
    if (!gestor->registrarLecturas(usuarioActivo->getIndice(), nuevoPrimerNoLeido)) {
        return RES_ERROR_ARCHIVO;
    }
    return RES_OK;
}

unsigned short RedSocial::registrarLike(Foto * foto, bool dar) {
    if (foto == nullptr) {
        return RES_NO_EXISTE;
    }
    if (!dar && foto->getLikes() == 0) {
        return RES_OK;
    }
    if (dar) {
        ++(*foto);
    } else {
        --(*foto);
    }
    MedidorRecursos::sumarMemoriaLocal(sizeof(foto) + sizeof(dar));
    if (!gestor->registrarLikes(foto->getId(), foto->getLikes())) {
        return RES_ERROR_ARCHIVO;
    }
    return RES_OK;
}

void RedSocial::iniciarMedicion(unsigned short idFuncionalidad) {
    medidor->iniciarFuncionalidad(idFuncionalidad);
}

void RedSocial::mostrarReporte() {
    medidor->finalizarFuncionalidad(calcularMemoria(), calcularMemoriaReferencia());
}

void RedSocial::mostrarResumenSesion() {
    medidor->mostrarResumenSesion(calcularMemoria(), calcularMemoriaReferencia());
}

unsigned long RedSocial::calcularMemoria() const {
    unsigned long total = sizeof(*this) + usuarios.calcularMemoria() - sizeof(usuarios);
    for (unsigned int i = 0; i < usuarios.gettamaño(); i++) {
        total += usuarios[i]->calcularMemoria();
    }
    if (amistades != nullptr) {
        total += amistades->calcularMemoria();
    }
    if (gestor != nullptr) {
        total += gestor->calcularMemoria();
    }
    if (medidor != nullptr) {
        total += sizeof(MedidorRecursos);
    }
    total += TAM_GRUPO_FOTOS * sizeof(unsigned int);
    return total;
}

unsigned long RedSocial::calcularMemoriaReferencia() const {
    unsigned long real = calcularMemoria();
    if (amistades == nullptr) {
        return real;
    }
    unsigned long conBool = sizeof(RedAmistades) + 2UL * amistades->getNumUsuarios() * sizeof(unsigned int)
                            + amistades->calcularMemoriaReferencia();
    return real - amistades->calcularMemoria() + conBool;
}
