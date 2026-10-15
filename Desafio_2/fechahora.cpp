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
    escribirNumero(anio, 4, destino);
    escribirNumero(mes, 2, destino + 4);
    escribirNumero(dia, 2, destino + 6);
    escribirNumero(hora, 2, destino + 8);
    escribirNumero(minuto, 2, destino + 10);
    escribirNumero(segundo, 2, destino + 12);
    destino[14] = '\0';
}

bool FechaHora::desdeTexto14(const char * texto) {
    if (texto == nullptr || longitud(texto) != 14) {
        return false;
    }
    unsigned long long a = 0;
    unsigned long long m = 0;
    unsigned long long d = 0;
    unsigned long long h = 0;
    unsigned long long mi = 0;
    unsigned long long s = 0;
    if (!aNumero(texto, 0, 4, a) || !aNumero(texto, 4, 6, m) || !aNumero(texto, 6, 8, d)
        || !aNumero(texto, 8, 10, h) || !aNumero(texto, 10, 12, mi) || !aNumero(texto, 12, 14, s)) {
        return false;
    }
    return setFechaHora((short)a, (short)m, (short)d, (short)h, (short)mi, (short)s);
}

std::ostream & operator<<(std::ostream & salida, const FechaHora & fecha) {
    char texto[20];
    escribirNumero(fecha.anio, 4, texto);
    texto[4] = '-';
    escribirNumero(fecha.mes, 2, texto + 5);
    texto[7] = '-';
    escribirNumero(fecha.dia, 2, texto + 8);
    texto[10] = ' ';
    escribirNumero(fecha.hora, 2, texto + 11);
    texto[13] = ':';
    escribirNumero(fecha.minuto, 2, texto + 14);
    texto[16] = ':';
    escribirNumero(fecha.segundo, 2, texto + 17);
    texto[19] = '\0';
    salida << texto;
    return salida;
}
