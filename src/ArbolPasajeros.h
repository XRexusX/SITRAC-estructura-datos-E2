// ArbolPasajeros.h - Arbol Binario de Busqueda (ABB) PROPIO, ordenado por ID.
//
// Regla del ABB: para cada nodo, todo lo que hay a su IZQUIERDA tiene un ID
// menor y todo lo que hay a su DERECHA tiene un ID mayor.
// Por eso buscar es rapido: en cada paso se descarta la mitad del arbol.
//   - Arbol equilibrado:  buscar / insertar / eliminar cuestan O(log n)
//   - Peor caso (IDs que llegan ya ordenados, el arbol parece una lista): O(n)
#ifndef ARBOL_PASAJEROS_H
#define ARBOL_PASAJEROS_H

#include "ListaPasajeros.h"  // aqui esta la definicion de Pasajero

struct NodoABB {
    Pasajero dato;
    NodoABB* izq;  // pasajeros con ID menor
    NodoABB* der;  // pasajeros con ID mayor
};

class ArbolPasajeros {
public:
    ArbolPasajeros();
    ~ArbolPasajeros();
    ArbolPasajeros(const ArbolPasajeros&) = delete;
    ArbolPasajeros& operator=(const ArbolPasajeros&) = delete;

    bool insertar(const Pasajero& p);              // false si el ID ya existe
    const Pasajero* buscar(int id) const;          // nullptr si no existe
    bool actualizar(int id, const std::string& nombre, int edad);
    bool eliminar(int id);                         // false si no existe

    // Recorridos: imprimen "ID nombre, ID nombre, ..."
    void inorden() const;    // izquierda, raiz, derecha  -> sale ORDENADO por ID
    void preorden() const;   // raiz, izquierda, derecha
    void postorden() const;  // izquierda, derecha, raiz

    int altura() const;      // arbol vacio = 0, un solo nodo = 1
    int tamano() const { return cantidad; }
    bool estaVacio() const { return raiz == nullptr; }
    void vaciar();
    void intercambiar(ArbolPasajeros& otro);

private:
    NodoABB* raiz;
    int cantidad;
};

#endif
