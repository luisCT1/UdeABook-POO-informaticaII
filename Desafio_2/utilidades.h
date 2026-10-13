#ifndef UTILIDADES_H
#define UTILIDADES_H

//cadenas con char* hechas a mano, sin strlen ni strcpy

namespace util {

unsigned int longitud(const char * texto);
bool copiar(char * destino, unsigned int capacidad, const char * origen);
int comparar(const char * a, const char * b);
bool iguales(const char * a, const char * b);
char * duplicar(const char * origen);
bool soloDigitos(const char * texto, unsigned int cantidad);
bool aEntero(const char * texto, unsigned int & valor);
const char * descripcionResultado(unsigned short codigo);

}

#endif
