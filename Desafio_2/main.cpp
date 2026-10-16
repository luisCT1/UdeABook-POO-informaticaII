#include "menu.h"
#include "redsocial.h"

#include <iostream>

int main() {
    //la carga va por debajo, no sale como opcion del menu
    const char * ruta = seleccionarDataset();
    RedSocial red(ruta, "x");
    if (!red.estaValido() || !red.cargarDatos()) {
        std::cout << "no se pudieron cargar los datos\n";
        return 1;
    }
    if (!pantallaDeIngreso(red)) {
        return 0;
    }
    ejecutarSesion(red);
    return 0;
}
