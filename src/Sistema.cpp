#include "Sistema.h"

bool Sistema::agregarPasajero(const Pasajero& p) {
    if (!pasajeros.insertar(p)) return false;  // ID duplicado: no se toca nada
    arbol.insertar(p);                         // misma informacion en el arbol
    return true;
}

bool Sistema::actualizarPasajero(int id, const std::string& nombre, int edad) {
    if (!pasajeros.actualizar(id, nombre, edad)) return false;
    arbol.actualizar(id, nombre, edad);
    return true;
}

bool Sistema::quitarPasajero(int id, int& solicitudesRetiradas) {
    solicitudesRetiradas = 0;
    if (!pasajeros.eliminar(id)) return false;  // no existe: no se toca nada
    arbol.eliminar(id);
    // Sus solicitudes pendientes quedarian apuntando a un pasajero que ya no existe.
    solicitudesRetiradas = cola.eliminarDePasajero(id);
    return true;
}

ResultadoDeshacer Sistema::deshacerUltimaAtencion(Viaje& viaje) {
    if (!historial.verTope(viaje)) return DESHACER_VACIO;

    // Solo se puede devolver a la cola si el pasajero y las ciudades siguen existiendo.
    if (pasajeros.buscar(viaje.idPasajero) == nullptr || grafo.buscar(viaje.origen) == nullptr ||
        grafo.buscar(viaje.destino) == nullptr)
        return DESHACER_IMPOSIBLE;

    Viaje sacado;
    historial.desapilar(sacado);  // pop del historial

    Solicitud s;
    s.idPasajero = sacado.idPasajero;
    s.origen = sacado.origen;
    s.destino = sacado.destino;
    cola.encolarAlFrente(s);      // vuelve a ser la proxima en atenderse
    return DESHACER_OK;
}
