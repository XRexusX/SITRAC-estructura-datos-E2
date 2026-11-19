#include "ArbolPasajeros.h"

#include <iostream>

// ---------------------------------------------------------------------------
// Funciones auxiliares RECURSIVAS (cada una trabaja sobre un subarbol).
// Estan fuera de la clase para que la clase quede corta y facil de leer.
// ---------------------------------------------------------------------------

// Inserta 'p' en el subarbol 'nodo' y devuelve la nueva raiz de ese subarbol.
// 'insertado' queda en false si el ID ya existia.
static NodoABB* insertarEn(NodoABB* nodo, const Pasajero& p, bool& insertado) {
    if (nodo == nullptr) {  // llegamos a un hueco: aqui va el nodo nuevo
        NodoABB* nuevo = new NodoABB;
        nuevo->dato = p;
        nuevo->izq = nullptr;
        nuevo->der = nullptr;
        insertado = true;
        return nuevo;
    }
    if (p.id < nodo->dato.id)
        nodo->izq = insertarEn(nodo->izq, p, insertado);
    else if (p.id > nodo->dato.id)
        nodo->der = insertarEn(nodo->der, p, insertado);
    else
        insertado = false;  // ID repetido: no se inserta
    return nodo;
}

// Elimina el ID del subarbol 'nodo' y devuelve la nueva raiz de ese subarbol.
static NodoABB* eliminarEn(NodoABB* nodo, int id, bool& eliminado) {
    if (nodo == nullptr) return nullptr;  // no existe
    if (id < nodo->dato.id) {
        nodo->izq = eliminarEn(nodo->izq, id, eliminado);
    } else if (id > nodo->dato.id) {
        nodo->der = eliminarEn(nodo->der, id, eliminado);
    } else {
        eliminado = true;
        // Caso 1 y 2: sin hijo izquierdo -> lo reemplaza su hijo derecho (o nada)
        if (nodo->izq == nullptr) {
            NodoABB* hijo = nodo->der;
            delete nodo;
            return hijo;
        }
        // Caso 2: sin hijo derecho -> lo reemplaza su hijo izquierdo
        if (nodo->der == nullptr) {
            NodoABB* hijo = nodo->izq;
            delete nodo;
            return hijo;
        }
        // Caso 3: dos hijos. Se copia el SUCESOR (el menor ID del lado derecho)
        // en este nodo y luego se borra ese sucesor, que tiene a lo sumo un hijo.
        NodoABB* sucesor = nodo->der;
        while (sucesor->izq != nullptr) sucesor = sucesor->izq;
        nodo->dato = sucesor->dato;
        bool borradoSucesor = false;
        nodo->der = eliminarEn(nodo->der, nodo->dato.id, borradoSucesor);
    }
    return nodo;
}

static void imprimirNodo(const NodoABB* n, bool& primero) {
    if (!primero) std::cout << ", ";
    std::cout << n->dato.id << " " << n->dato.nombre;
    primero = false;
}

static void inordenEn(const NodoABB* n, bool& primero) {
    if (n == nullptr) return;
    inordenEn(n->izq, primero);
    imprimirNodo(n, primero);
    inordenEn(n->der, primero);
}

static void preordenEn(const NodoABB* n, bool& primero) {
    if (n == nullptr) return;
    imprimirNodo(n, primero);
    preordenEn(n->izq, primero);
    preordenEn(n->der, primero);
}

static void postordenEn(const NodoABB* n, bool& primero) {
    if (n == nullptr) return;
    postordenEn(n->izq, primero);
    postordenEn(n->der, primero);
    imprimirNodo(n, primero);
}

static int alturaEn(const NodoABB* n) {
    if (n == nullptr) return 0;
    int hi = alturaEn(n->izq);
    int hd = alturaEn(n->der);
    return 1 + (hi > hd ? hi : hd);
}

static void liberar(NodoABB* n) {  // postorden: primero los hijos, luego el nodo
    if (n == nullptr) return;
    liberar(n->izq);
    liberar(n->der);
    delete n;
}

// ---------------------------------------------------------------------------
// Metodos de la clase
// ---------------------------------------------------------------------------

ArbolPasajeros::ArbolPasajeros() : raiz(nullptr), cantidad(0) {}

ArbolPasajeros::~ArbolPasajeros() { vaciar(); }

void ArbolPasajeros::vaciar() {
    liberar(raiz);
    raiz = nullptr;
    cantidad = 0;
}

void ArbolPasajeros::intercambiar(ArbolPasajeros& otro) {
    NodoABB* r = raiz;
    raiz = otro.raiz;
    otro.raiz = r;
    int n = cantidad;
    cantidad = otro.cantidad;
    otro.cantidad = n;
}

bool ArbolPasajeros::insertar(const Pasajero& p) {
    bool insertado = false;
    raiz = insertarEn(raiz, p, insertado);
    if (insertado) ++cantidad;
    return insertado;
}

// Busqueda iterativa: baja por el arbol comparando el ID.
const Pasajero* ArbolPasajeros::buscar(int id) const {
    const NodoABB* actual = raiz;
    while (actual != nullptr) {
        if (id == actual->dato.id) return &actual->dato;
        actual = (id < actual->dato.id) ? actual->izq : actual->der;
    }
    return nullptr;
}

bool ArbolPasajeros::actualizar(int id, const std::string& nombre, int edad) {
    NodoABB* actual = raiz;
    while (actual != nullptr) {
        if (id == actual->dato.id) {
            actual->dato.nombre = nombre;
            actual->dato.edad = edad;
            return true;
        }
        actual = (id < actual->dato.id) ? actual->izq : actual->der;
    }
    return false;
}

bool ArbolPasajeros::eliminar(int id) {
    bool eliminado = false;
    raiz = eliminarEn(raiz, id, eliminado);
    if (eliminado) --cantidad;
    return eliminado;
}

void ArbolPasajeros::inorden() const {
    bool primero = true;
    inordenEn(raiz, primero);
    if (primero) std::cout << "(arbol vacio)";
    std::cout << "\n";
}

void ArbolPasajeros::preorden() const {
    bool primero = true;
    preordenEn(raiz, primero);
    if (primero) std::cout << "(arbol vacio)";
    std::cout << "\n";
}

void ArbolPasajeros::postorden() const {
    bool primero = true;
    postordenEn(raiz, primero);
    if (primero) std::cout << "(arbol vacio)";
    std::cout << "\n";
}

int ArbolPasajeros::altura() const { return alturaEn(raiz); }
