// Sistema.h - Reune las 5 estructuras del programa y mantiene su coherencia.
//
// REGLA DE ORO (invariante): la LISTA y el ABB siempre contienen exactamente
// los mismos pasajeros. Por eso nunca se agrega o quita un pasajero tocando la
// lista o el arbol directamente: se usan los metodos de esta clase.
#ifndef SISTEMA_H
#define SISTEMA_H

#include "ArbolPasajeros.h"
#include "ColaSolicitudes.h"
#include "Grafo.h"
#include "Historial.h"
#include "ListaPasajeros.h"

enum ResultadoDeshacer { DESHACER_OK, DESHACER_VACIO, DESHACER_IMPOSIBLE };

struct Sistema {
    ListaPasajeros pasajeros;   // lista enlazada (CRUD de pasajeros)
    ArbolPasajeros arbol;       // ABB por ID (busqueda rapida y recorridos)
    Grafo grafo;                // ciudades y conexiones
    ColaSolicitudes cola;       // solicitudes de viaje (FIFO)
    Historial historial;        // viajes atendidos (pila LIFO)

    // Pasajeros: mantienen lista + arbol sincronizados.
    bool agregarPasajero(const Pasajero& p);                 // false si el ID ya existe
    bool actualizarPasajero(int id, const std::string& nombre, int edad);
    // Elimina de lista y arbol, y retira sus solicitudes de la cola.
    bool quitarPasajero(int id, int& solicitudesRetiradas);  // false si no existe

    // Deshacer: saca el ultimo viaje del historial y lo devuelve al FRENTE de la cola.
    ResultadoDeshacer deshacerUltimaAtencion(Viaje& viaje);
};

#endif
