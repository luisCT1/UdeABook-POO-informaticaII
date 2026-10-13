TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        albumfotos.cpp \
        fechahora.cpp \
        fotos.cpp \
        gestorarchivos.cpp \
        main.cpp \
        medidorrecursos.cpp \
        mensajes.cpp \
        redamistades.cpp \
        redsocial.cpp \
        usuario.cpp \
        utilidades.cpp

HEADERS += \
    Configuracion.h \
    albumfotos.h \
    fechahora.h \
    fotos.h \
    gestorarchivos.h \
    listadinamica.h \
    medidorrecursos.h \
    mensajes.h \
    redamistades.h \
    redsocial.h \
    usuario.h \
    utilidades.h
