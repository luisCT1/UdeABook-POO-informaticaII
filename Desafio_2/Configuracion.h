#ifndef CONFIGURACION_H
#define CONFIGURACION_H

const unsigned int   MAX_BYTES_MENSAJE  = 512;
const unsigned short K_MIN_SUGERENCIAS  = 1;
const unsigned short K_MAX_SUGERENCIAS  = 20;
const unsigned short TAM_GRUPO_FOTOS    = 3;

const unsigned int UMBRAL_PESO_MEDIO = 10;
const unsigned int UMBRAL_PESO_ALTO  = 20;
const float PESO_BASE  = 1.0f;
const float PESO_MEDIO = 2.5f;
const float PESO_ALTO  = 3.5f;

const char PRIVACIDAD_PUBLICA = 'P';
const char PRIVACIDAD_PRIVADA = 'R';
const char PRIVACIDAD_AMIGOS  = 'A';

const unsigned int BITS_POR_PALABRA = 64;

const unsigned short NUM_ARCHIVOS      = 6;
const unsigned short ARCH_USUARIOS     = 0;
const unsigned short ARCH_MULTIMEDIA   = 1;
const unsigned short ARCH_AMISTADES    = 2;
const unsigned short ARCH_SOLICITUDES  = 3;
const unsigned short ARCH_MENSAJES     = 4;
const unsigned short ARCH_CAMBIOS      = 5;
const char SEPARADOR = '|';
const unsigned int MAX_LINEA = 1024;

const unsigned short FUNC_CARGA            = 0;
const unsigned short FUNC_INGRESO          = 1;
const unsigned short FUNC_ENVIAR_SOLICITUD = 2;
const unsigned short FUNC_ACEPTAR_SOLICITUD= 3;
const unsigned short FUNC_PERFIL_AMIGO     = 4;
const unsigned short FUNC_SUGERENCIAS      = 5;
const unsigned short FUNC_ENVIAR_MENSAJE   = 6;
const unsigned short FUNC_LEER_NUEVOS      = 7;
const unsigned short FUNC_RANGO_MENSAJES   = 8;
const unsigned short FUNC_ACTUALIZAR_FOTOS = 9;
const unsigned short NUM_FUNCIONALIDADES   = 10;

const unsigned short RES_OK                  = 0;
const unsigned short RES_SIN_SESION          = 1;
const unsigned short RES_NO_EXISTE           = 2;
const unsigned short RES_ES_UNO_MISMO        = 3;
const unsigned short RES_YA_SON_AMIGOS       = 4;
const unsigned short RES_ELLOS_YA_ENVIARON   = 5;
const unsigned short RES_SOLICITUD_REPETIDA  = 6;
const unsigned short RES_NO_ES_AMIGO         = 7;
const unsigned short RES_TEXTO_INVALIDO      = 8;
const unsigned short RES_NO_HAY_SOLICITUD    = 9;
const unsigned short RES_ERROR_MEMORIA       = 10;
const unsigned short RES_ERROR_ARCHIVO       = 11;
const unsigned short RES_YA_ACTUALIZO        = 12;
const unsigned short RES_CONTRASENA_INCORRECTA = 13;
const unsigned short RES_SIN_CANDIDATOS      = 14;

//colombia, utc-5. sirve para fechar los mensajes nuevos
const int DESPLAZAMIENTO_HORARIO = -5 * 3600;

#endif
