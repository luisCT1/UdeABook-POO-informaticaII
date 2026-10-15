//la plantilla va en este archivo porque si se deja en un .cpp las otras clases no la ven

template <class T>
ListaDinamica<T>::ListaDinamica()
    : elementos(nullptr), tamanio(0), capacidad(0), valida(true) {
}

template <class T>
ListaDinamica<T>::ListaDinamica(unsigned int capacidadInicial)
    : elementos(nullptr), tamanio(0), capacidad(0), valida(true) {
    if (capacidadInicial == 0) {
        return;
    }
    elementos = reservar<T>(capacidadInicial);
    if (elementos == nullptr) {
        valida = false;
        return;
    }
    capacidad = capacidadInicial;
}

template <class T>
ListaDinamica<T>::ListaDinamica(const ListaDinamica & otra)
    : elementos(nullptr), tamanio(0), capacidad(0), valida(true) {
    if (otra.capacidad == 0) {
        return;
    }
    elementos = reservar<T>(otra.capacidad);
    if (elementos == nullptr) {
        valida = false;
        return;
    }
    for (unsigned int i = 0; i < otra.tamanio; i++) {
        MedidorRecursos::sumarComun();
        elementos[i] = otra.elementos[i];
    }
    tamanio = otra.tamanio;
    capacidad = otra.capacidad;
    valida = otra.valida;
}

template <class T>
ListaDinamica<T>::~ListaDinamica() {
    delete[] elementos;
}

template <class T>
bool ListaDinamica<T>::estaValida() const {
    return valida;
}

template <class T>
unsigned int ListaDinamica<T>::getTamanio() const {
    return tamanio;
}

template <class T>
unsigned int ListaDinamica<T>::getCapacidad() const {
    return capacidad;
}

template <class T>
bool ListaDinamica<T>::estaVacia() const {
    return tamanio == 0;
}

template <class T>
bool ListaDinamica<T>::asegurarCapacidad(unsigned int minima) {
    if (minima <= capacidad) {
        return true;
    }
    T * nuevo = reservar<T>(minima);
    if (nuevo == nullptr) {
        valida = false;
        return false;
    }
    for (unsigned int i = 0; i < tamanio; i++) {
        MedidorRecursos::sumarIteraciones();
        nuevo[i] = elementos[i];
    }
    delete[] elementos;
    elementos = nuevo;
    capacidad = minima;
    return true;
}

template <class T>
bool ListaDinamica<T>::ampliar() {
    unsigned int nueva = (capacidad == 0) ? 2u : capacidad * 2u;
    if (nueva < capacidad) {
        return false;
    }
    return asegurarCapacidad(nueva);
}

template <class T>
bool ListaDinamica<T>::agregar(const T & elemento) {
    //crecer de uno en uno copiaria todos los elementos en cada agregar
    MedidorRecursos::sumarReferencia(tamanio);
    if (tamanio == capacidad) {
        if (!ampliar()) {
            return false;
        }
    }
    elementos[tamanio] = elemento;
    tamanio++;
    return true;
}

template <class T>
T & ListaDinamica<T>::operator[](unsigned int posicion) {
    return elementos[posicion];
}

template <class T>
const T & ListaDinamica<T>::operator[](unsigned int posicion) const {
    return elementos[posicion];
}

template <class T>
ListaDinamica<T> & ListaDinamica<T>::operator=(const ListaDinamica & otra) {
    if (this == &otra) {
        return *this;
    }
    T * nuevo = nullptr;
    if (otra.capacidad > 0) {
        nuevo = reservar<T>(otra.capacidad);
        if (nuevo == nullptr) {
            valida = false;
            return *this;
        }
        for (unsigned int i = 0; i < otra.tamanio; i++) {
            MedidorRecursos::sumarComun();
            nuevo[i] = otra.elementos[i];
        }
    }
    delete[] elementos;
    elementos = nuevo;
    tamanio = otra.tamanio;
    capacidad = otra.capacidad;
    valida = otra.valida;
    return *this;
}

template <class T>
unsigned long ListaDinamica<T>::calcularMemoria() const {
    return sizeof(*this) + (unsigned long)capacidad * sizeof(T);
}
