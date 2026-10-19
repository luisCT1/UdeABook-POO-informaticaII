#include "menu.h"
#include "redsocial.h"

#include <iostream>

int main() {
    //la carga va por debajo, no sale como opcion del menu
    //para otra base: cambia "datos/mediano" por "datos/pequenio" o "datos/grande"
    const char * ruta = buscarCarpetaDatos("datos/mediano");
    RedSocial red(ruta, "x");
    if (!red.estaValido() || !red.cargarDatos()) {
        std::cout << "no se pudieron cargar los datos\n";
        return 1;
    }
    std::cout << "carga de datos exitosa\n";
    while (true) {
        if (!pantallaDeIngreso(red)) {
            return 0;
        }
        if (!ejecutarSesion(red)) {
            return 0;
        }
    }
}
