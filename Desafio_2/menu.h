#ifndef MENU_H
#define MENU_H

class RedSocial;

//el menu solo llama a RedSocial

const char * seleccionarDataset();
bool pantallaDeIngreso(RedSocial & red);
void mostrarPantallaInicio(RedSocial & red);
void ejecutarSesion(RedSocial & red);
void opcionAdministrarAmistades(RedSocial & red);
void opcionVisitarPerfil(RedSocial & red);
void opcionSugerencias(RedSocial & red);
void opcionMensajes(RedSocial & red);
void opcionActualizarFotos(RedSocial & red);

#endif
