#ifndef CONFIGURACION_H
#define CONFIGURACION_H

//numeros que se repiten en varias clases, para no dejarlos sueltos en el codigo

namespace cfg {

const unsigned short NUM_ARCHIVOS = 6;
const unsigned short ARCHIVO_USUARIOS = 0;
const unsigned short ARCHIVO_MULTIMEDIA = 1;
const unsigned short ARCHIVO_AMISTADES = 2;
const unsigned short ARCHIVO_SOLICITUDES = 3;
const unsigned short ARCHIVO_MENSAJES = 4;
const unsigned short ARCHIVO_CAMBIOS = 5;

const char PRIVACIDAD_PUBLICA = 'P';
const char PRIVACIDAD_PRIVADA = 'R';
const char PRIVACIDAD_AMIGOS = 'A';

const unsigned int MAX_BYTES_MENSAJE = 512;

const float PESO_BASE = 1.0f;
const float PESO_MEDIO = 2.5f;
const float PESO_ALTO = 3.5f;

const unsigned short NUM_FUNCIONALIDADES = 9;

const unsigned short RES_OK = 0;
const unsigned short RES_NO_EXISTE = 1;
const unsigned short RES_CLAVE_MALA = 2;
const unsigned short RES_ES_UNO_MISMO = 3;
const unsigned short RES_YA_SON_AMIGOS = 4;
const unsigned short RES_ELLOS_YA_ENVIARON = 5;
const unsigned short RES_SOLICITUD_REPETIDA = 6;
const unsigned short RES_YA_ACTUALIZO = 7;
const unsigned short RES_NO_ES_AMIGO = 8;
const unsigned short RES_MENSAJE_INVALIDO = 9;
const unsigned short RES_SIN_SESION = 10;
const unsigned short RES_ERROR = 11;

inline const char * nombreArchivo(unsigned short id) {
    static const char * nombres[] = {
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

}

const unsigned short FUNC_INGRESO = 0;
const unsigned short FUNC_ACTUALIZAR_FOTOS = 1;
const unsigned short FUNC_ENVIAR_SOLICITUD = 2;
const unsigned short FUNC_ACEPTAR_SOLICITUD = 3;
const unsigned short FUNC_SUGERENCIAS = 4;
const unsigned short FUNC_ENVIAR_MENSAJE = 5;
const unsigned short FUNC_LEER_NUEVOS = 6;
const unsigned short FUNC_RANGO_MENSAJES = 7;
const unsigned short FUNC_PERFIL_AMIGO = 8;

#endif
