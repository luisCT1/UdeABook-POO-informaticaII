#include "menu.h"
#include "redsocial.h"

#include <iostream>

int main() {
    const char * ruta = seleccionarDataset();
    RedSocial red(ruta, "x");
    if (!red.estaValido()) {
        std::cout << "no se pudo preparar la red\n";
        return 1;
    }
    if (!red.cargarDatos()) {
        std::cout << "no se pudieron leer los datos de " << ruta << "\n";
        return 1;
    }
    std::cout << "usuarios cargados: " << red.getNumUsuarios() << "\n";
    std::cout << "la clave de la demo es clave123 y el usuario es u0000\n";
    if (!pantallaDeIngreso(red)) {
        return 0;
    }
    ejecutarSesion(red);
    return 0;
}
