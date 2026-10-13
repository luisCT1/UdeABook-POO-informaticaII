#ifndef FECHAHORA_H
#define FECHAHORA_H

#include <iosfwd>

//fecha con hora. en el archivo va como 14 digitos: AAAAMMDDhhmmss

class FechaHora {
private:
    short anio, mes, dia, hora, minuto, segundo;

public:
    FechaHora();
    FechaHora(short anio, short mes, short dia);
    FechaHora(short anio, short mes, short dia, short hora, short minuto, short segundo);
    FechaHora(const FechaHora & otra);
    FechaHora & operator=(const FechaHora & otra);

    short getAnio() const;
    short getMes() const;
    short getDia() const;
    short getHora() const;
    short getMinuto() const;
    short getSegundo() const;

    bool setFechaHora(short anio, short mes, short dia);
    bool setFechaHora(short anio, short mes, short dia, short hora, short minuto, short segundo);

    bool esValida() const;
    bool estaEnRango(const FechaHora & inicio, const FechaHora & fin) const;

    bool operator<(const FechaHora & otra) const;
    bool operator==(const FechaHora & otra) const;

    void aTexto14(char * destino) const;
    bool desdeTexto14(const char * texto);

    friend std::ostream & operator<<(std::ostream & salida, const FechaHora & fecha);
};

#endif
