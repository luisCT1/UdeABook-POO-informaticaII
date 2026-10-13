#include "FechaHora.h"
#include "utilidades.h"

#include <iostream>

namespace {

bool bisiesto(short anio) {
    if (anio % 400 == 0) {
        return true;
    }
    if (anio % 100 == 0) {
        return false;
    }
    return anio % 4 == 0;
}

short diasDelMes(short anio, short mes) {
    if (mes == 2) {
        return bisiesto(anio) ? 29 : 28;
    }
    if (mes == 4 || mes == 6 || mes == 9 || mes == 11) {
        return 30;
    }
    return 31;
}

bool numerosValidos(short anio, short mes, short dia, short hora, short minuto, short segundo) {
    if (anio < 1 || mes < 1 || mes > 12 || dia < 1) {
        return false;
    }
    if (dia > diasDelMes(anio, mes)) {
        return false;
    }
    if (hora < 0 || hora > 23 || minuto < 0 || minuto > 59 || segundo < 0 || segundo > 59) {
        return false;
    }
    return true;
}

void ponerDigitos(char * destino, int valor, int cantidad) {
    for (int i = cantidad - 1; i >= 0; i--) {
        destino[i] = (char)('0' + (valor % 10));
        valor /= 10;
    }
}

}

FechaHora::FechaHora()
    : anio(1), mes(1), dia(1), hora(0), minuto(0), segundo(0) {
}

FechaHora::FechaHora(short anio, short mes, short dia)
    : anio(1), mes(1), dia(1), hora(0), minuto(0), segundo(0) {
    setFechaHora(anio, mes, dia);
}

FechaHora::FechaHora(short anio, short mes, short dia, short hora, short minuto, short segundo)
    : anio(1), mes(1), dia(1), hora(0), minuto(0), segundo(0) {
    setFechaHora(anio, mes, dia, hora, minuto, segundo);
}

FechaHora::FechaHora(const FechaHora & otra)
    : anio(otra.anio), mes(otra.mes), dia(otra.dia),
      hora(otra.hora), minuto(otra.minuto), segundo(otra.segundo) {
}

FechaHora & FechaHora::operator=(const FechaHora & otra) {
    if (this != &otra) {
        anio = otra.anio;
        mes = otra.mes;
        dia = otra.dia;
        hora = otra.hora;
        minuto = otra.minuto;
        segundo = otra.segundo;
    }
    return *this;
}

short FechaHora::getAnio() const { return anio; }
short FechaHora::getMes() const { return mes; }
short FechaHora::getDia() const { return dia; }
short FechaHora::getHora() const { return hora; }
short FechaHora::getMinuto() const { return minuto; }
short FechaHora::getSegundo() const { return segundo; }

bool FechaHora::setFechaHora(short anioNuevo, short mesNuevo, short diaNuevo) {
    return setFechaHora(anioNuevo, mesNuevo, diaNuevo, 0, 0, 0);
}

bool FechaHora::setFechaHora(short anioNuevo, short mesNuevo, short diaNuevo,
                             short horaNueva, short minutoNuevo, short segundoNuevo) {
    if (!numerosValidos(anioNuevo, mesNuevo, diaNuevo, horaNueva, minutoNuevo, segundoNuevo)) {
        return false;
    }
    anio = anioNuevo;
    mes = mesNuevo;
    dia = diaNuevo;
    hora = horaNueva;
    minuto = minutoNuevo;
    segundo = segundoNuevo;
    return true;
}

bool FechaHora::esValida() const {
    return numerosValidos(anio, mes, dia, hora, minuto, segundo);
}

bool FechaHora::estaEnRango(const FechaHora & inicio, const FechaHora & fin) const {
    //el rango mira el dia, no la hora: el de las 23:59 del ultimo dia si entra
    FechaHora yo(anio, mes, dia);
    FechaHora desde(inicio.anio, inicio.mes, inicio.dia);
    FechaHora hasta(fin.anio, fin.mes, fin.dia);
    if (hasta < desde) {
        return false;
    }
    return !(yo < desde) && !(hasta < yo);
}

bool FechaHora::operator<(const FechaHora & otra) const {
    if (anio != otra.anio) return anio < otra.anio;
    if (mes != otra.mes) return mes < otra.mes;
    if (dia != otra.dia) return dia < otra.dia;
    if (hora != otra.hora) return hora < otra.hora;
    if (minuto != otra.minuto) return minuto < otra.minuto;
    return segundo < otra.segundo;
}

bool FechaHora::operator==(const FechaHora & otra) const {
    return anio == otra.anio && mes == otra.mes && dia == otra.dia
        && hora == otra.hora && minuto == otra.minuto && segundo == otra.segundo;
}

void FechaHora::aTexto14(char * destino) const {
    if (destino == nullptr) {
        return;
    }
    ponerDigitos(destino, anio, 4);
    ponerDigitos(destino + 4, mes, 2);
    ponerDigitos(destino + 6, dia, 2);
    ponerDigitos(destino + 8, hora, 2);
    ponerDigitos(destino + 10, minuto, 2);
    ponerDigitos(destino + 12, segundo, 2);
    destino[14] = '\0';
}

bool FechaHora::desdeTexto14(const char * texto) {
    if (!util::soloDigitos(texto, 14)) {
        return false;
    }
    short a = (short)((texto[0] - '0') * 1000 + (texto[1] - '0') * 100 + (texto[2] - '0') * 10 + (texto[3] - '0'));
    short m = (short)((texto[4] - '0') * 10 + (texto[5] - '0'));
    short d = (short)((texto[6] - '0') * 10 + (texto[7] - '0'));
    short h = (short)((texto[8] - '0') * 10 + (texto[9] - '0'));
    short mi = (short)((texto[10] - '0') * 10 + (texto[11] - '0'));
    short s = (short)((texto[12] - '0') * 10 + (texto[13] - '0'));
    return setFechaHora(a, m, d, h, mi, s);
}

std::ostream & operator<<(std::ostream & salida, const FechaHora & fecha) {
    char texto[20];
    ponerDigitos(texto, fecha.anio, 4);
    texto[4] = '-';
    ponerDigitos(texto + 5, fecha.mes, 2);
    texto[7] = '-';
    ponerDigitos(texto + 8, fecha.dia, 2);
    texto[10] = ' ';
    ponerDigitos(texto + 11, fecha.hora, 2);
    texto[13] = ':';
    ponerDigitos(texto + 14, fecha.minuto, 2);
    texto[16] = ':';
    ponerDigitos(texto + 17, fecha.segundo, 2);
    texto[19] = '\0';
    salida << texto;
    return salida;
}
