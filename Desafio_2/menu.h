#ifndef MENU_H
#define MENU_H

class RedSocial;

//el menu solo llama a RedSocial

//resuelve la ruta de la carpeta de datos (sube carpetas si hace falta)
const char * buscarCarpetaDatos(const char * relativa);
bool pantallaDeIngreso(RedSocial & red);
void mostrarPantallaInicio(RedSocial & red);
//true = volver a ingresar con otro usuario; false = salir del programa
bool ejecutarSesion(RedSocial & red);
void opcionVisitarPerfil(RedSocial & red);
void opcionSugerencias(RedSocial & red);
void opcionMensajes(RedSocial & red);
void opcionActualizarFotos(RedSocial & red);

#endif
