// Grafo.h - Grafo NO dirigido y ponderado con LISTA DE ADYACENCIA propia.
//
//   Ciudad  = vertice. Las ciudades forman una lista enlazada.
//   Arista  = conexion. Cada ciudad tiene su propia lista enlazada de aristas
//             (a que ciudad llega y a cuantos km).
//
// Una conexion A-B se guarda DOS veces (A->B y B->A) porque el grafo no tiene
// direccion: asi se consultan los vecinos desde cualquiera de los dos extremos.
#ifndef GRAFO_H
#define GRAFO_H

#include <string>

struct Ciudad;

struct Arista {
    Ciudad* destino;
    double distancia;
    Arista* sig;
};

struct Ciudad {
    std::string nombre;
    Arista* aristas;  // lista de vecinos
    Ciudad* sig;      // siguiente ciudad de la lista
    int indice;       // numero temporal (0..n-1) que usan BFS, DFS y Dijkstra
};

enum ResultadoRuta {
    RUTA_OK,
    RUTA_CIUDAD_INEXISTENTE,
    RUTA_MISMA_CIUDAD,
    RUTA_DISTANCIA_INVALIDA,
    RUTA_YA_EXISTE,
    RUTA_NO_EXISTE
};

class Grafo {
public:
    Grafo();
    ~Grafo();
    Grafo(const Grafo&) = delete;
    Grafo& operator=(const Grafo&) = delete;

    // ---- Ciudades ----
    bool agregarCiudad(const std::string& nombre);  // false si ya existe
    // Elimina la ciudad y TODAS las conexiones que llegan a ella.
    // 'conexionesEliminadas' dice cuantas se quitaron. false si no existe.
    bool eliminarCiudad(const std::string& nombre, int& conexionesEliminadas);
    const Ciudad* buscar(const std::string& nombre) const;
    void listarCiudades() const;

    // ---- Conexiones ----
    ResultadoRuta agregarConexion(const std::string& a, const std::string& b, double distancia);
    ResultadoRuta eliminarConexion(const std::string& a, const std::string& b);
    bool listarVecinos(const std::string& nombre) const;  // false si la ciudad no existe
    void listarConexiones() const;

    // ---- Algoritmos (imprimen su resultado). Todos devuelven false si la
    // ciudad pedida no existe (o, en Dijkstra, si no hay camino). ----
    bool recorridoBFS(const std::string& inicio);   // por niveles, usa una cola
    bool recorridoDFS(const std::string& inicio);   // en profundidad, usa recursion
    // Dijkstra: camino de menor distancia. Llena 'total' (km) y 'camino' (texto).
    bool rutaMasCorta(const std::string& origen, const std::string& destino,
                      double& total, std::string& camino);

    // ---- Datos para reportes ----
    int cantidadCiudades() const { return cantidad; }
    int cantidadConexiones() const;
    double kilometrosTotales() const;
    static int grado(const Ciudad* c);  // cuantos vecinos tiene

    void vaciar();
    void intercambiar(Grafo& otro);
    const Ciudad* primera() const { return cabeza; }

private:
    Ciudad* buscarMutable(const std::string& nombre) const;
    static Arista* buscarArista(const Ciudad* desde, const Ciudad* hacia);
    static void insertarArista(Ciudad* desde, Ciudad* hacia, double distancia);
    static bool quitarArista(Ciudad* desde, const Ciudad* hacia);
    Ciudad** numerar();  // asigna indice 0..n-1 y devuelve un arreglo (hay que hacer delete[])

    Ciudad* cabeza;
    int cantidad;
};

#endif
