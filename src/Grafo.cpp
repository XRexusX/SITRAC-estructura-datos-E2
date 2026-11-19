#include "Grafo.h"

#include <cmath>
#include <iostream>

#include "Utilidades.h"

Grafo::Grafo() : cabeza(nullptr), cantidad(0) {}

Grafo::~Grafo() { vaciar(); }

void Grafo::vaciar() {
    while (cabeza != nullptr) {
        Ciudad* borrar = cabeza;
        cabeza = cabeza->sig;
        while (borrar->aristas != nullptr) {  // primero sus aristas
            Arista* a = borrar->aristas;
            borrar->aristas = a->sig;
            delete a;
        }
        delete borrar;  // despues la ciudad
    }
    cantidad = 0;
}

void Grafo::intercambiar(Grafo& otro) {
    Ciudad* c = cabeza;
    cabeza = otro.cabeza;
    otro.cabeza = c;
    int n = cantidad;
    cantidad = otro.cantidad;
    otro.cantidad = n;
}

// ---------------------------------------------------------------- auxiliares

Ciudad* Grafo::buscarMutable(const std::string& nombre) const {
    for (Ciudad* c = cabeza; c != nullptr; c = c->sig)
        if (util::iguales(c->nombre, nombre)) return c;
    return nullptr;
}

const Ciudad* Grafo::buscar(const std::string& nombre) const {
    return buscarMutable(nombre);
}

Arista* Grafo::buscarArista(const Ciudad* desde, const Ciudad* hacia) {
    for (Arista* a = desde->aristas; a != nullptr; a = a->sig)
        if (a->destino == hacia) return a;
    return nullptr;
}

// Agrega la arista AL FINAL de la lista, asi los vecinos salen en el orden en
// que se crearon las conexiones (los recorridos BFS/DFS son reproducibles).
void Grafo::insertarArista(Ciudad* desde, Ciudad* hacia, double distancia) {
    Arista* nueva = new Arista;
    nueva->destino = hacia;
    nueva->distancia = distancia;
    nueva->sig = nullptr;
    if (desde->aristas == nullptr) {
        desde->aristas = nueva;
    } else {
        Arista* ultima = desde->aristas;
        while (ultima->sig != nullptr) ultima = ultima->sig;
        ultima->sig = nueva;
    }
}

bool Grafo::quitarArista(Ciudad* desde, const Ciudad* hacia) {
    Arista* previa = nullptr;
    Arista* actual = desde->aristas;
    while (actual != nullptr && actual->destino != hacia) {
        previa = actual;
        actual = actual->sig;
    }
    if (actual == nullptr) return false;  // no habia tal arista
    if (previa == nullptr)
        desde->aristas = actual->sig;  // era la primera
    else
        previa->sig = actual->sig;     // saltarla
    delete actual;
    return true;
}

int Grafo::grado(const Ciudad* c) {
    int g = 0;
    for (const Arista* a = c->aristas; a != nullptr; a = a->sig) ++g;
    return g;
}

Ciudad** Grafo::numerar() {
    Ciudad** todas = new Ciudad*[cantidad > 0 ? cantidad : 1];
    int i = 0;
    for (Ciudad* c = cabeza; c != nullptr; c = c->sig, ++i) {
        c->indice = i;
        todas[i] = c;
    }
    return todas;
}

// ------------------------------------------------------------------ ciudades

bool Grafo::agregarCiudad(const std::string& nombre) {
    Ciudad* ultima = nullptr;
    for (Ciudad* c = cabeza; c != nullptr; c = c->sig) {
        if (util::iguales(c->nombre, nombre)) return false;  // duplicada
        ultima = c;
    }
    Ciudad* nueva = new Ciudad;
    nueva->nombre = nombre;
    nueva->aristas = nullptr;
    nueva->sig = nullptr;
    nueva->indice = 0;
    if (ultima == nullptr)
        cabeza = nueva;
    else
        ultima->sig = nueva;
    ++cantidad;
    return true;
}

bool Grafo::eliminarCiudad(const std::string& nombre, int& conexionesEliminadas) {
    conexionesEliminadas = 0;
    Ciudad* previa = nullptr;
    Ciudad* objetivo = cabeza;
    while (objetivo != nullptr && !util::iguales(objetivo->nombre, nombre)) {
        previa = objetivo;
        objetivo = objetivo->sig;
    }
    if (objetivo == nullptr) return false;

    // 1) En las DEMAS ciudades, quitar toda arista que apunte a 'objetivo'.
    //    Si no se hiciera, quedarian punteros colgantes a memoria liberada.
    for (Ciudad* c = cabeza; c != nullptr; c = c->sig)
        if (c != objetivo && quitarArista(c, objetivo)) ++conexionesEliminadas;

    // 2) Liberar las aristas propias de 'objetivo'.
    while (objetivo->aristas != nullptr) {
        Arista* a = objetivo->aristas;
        objetivo->aristas = a->sig;
        delete a;
    }

    // 3) Sacar la ciudad de la lista y liberarla.
    if (previa == nullptr)
        cabeza = objetivo->sig;
    else
        previa->sig = objetivo->sig;
    delete objetivo;
    --cantidad;
    return true;
}

void Grafo::listarCiudades() const {
    if (cabeza == nullptr) {
        std::cout << "  (no hay ciudades registradas)\n";
        return;
    }
    for (const Ciudad* c = cabeza; c != nullptr; c = c->sig)
        std::cout << "  - " << c->nombre << " (" << grado(c) << " conexiones)\n";
    std::cout << "  Total: " << cantidad << "\n";
}

// --------------------------------------------------------------- conexiones

ResultadoRuta Grafo::agregarConexion(const std::string& a, const std::string& b,
                                     double distancia) {
    Ciudad* ca = buscarMutable(a);
    Ciudad* cb = buscarMutable(b);
    if (ca == nullptr || cb == nullptr) return RUTA_CIUDAD_INEXISTENTE;
    if (ca == cb) return RUTA_MISMA_CIUDAD;
    if (!std::isfinite(distancia) || distancia <= 0) return RUTA_DISTANCIA_INVALIDA;
    if (buscarArista(ca, cb) != nullptr) return RUTA_YA_EXISTE;
    insertarArista(ca, cb, distancia);  // se guarda en los dos sentidos
    insertarArista(cb, ca, distancia);
    return RUTA_OK;
}

ResultadoRuta Grafo::eliminarConexion(const std::string& a, const std::string& b) {
    Ciudad* ca = buscarMutable(a);
    Ciudad* cb = buscarMutable(b);
    if (ca == nullptr || cb == nullptr) return RUTA_CIUDAD_INEXISTENTE;
    if (!quitarArista(ca, cb)) return RUTA_NO_EXISTE;
    quitarArista(cb, ca);
    return RUTA_OK;
}

bool Grafo::listarVecinos(const std::string& nombre) const {
    const Ciudad* c = buscar(nombre);
    if (c == nullptr) return false;
    if (c->aristas == nullptr) {
        std::cout << "  " << c->nombre << " no tiene conexiones.\n";
        return true;
    }
    std::cout << "  Vecinos de " << c->nombre << ":\n";
    for (const Arista* a = c->aristas; a != nullptr; a = a->sig)
        std::cout << "    -> " << a->destino->nombre << " ("
                  << util::numeroATexto(a->distancia) << " km)\n";
    return true;
}

void Grafo::listarConexiones() const {
    int mostradas = 0;
    for (const Ciudad* c = cabeza; c != nullptr; c = c->sig) {
        for (const Arista* a = c->aristas; a != nullptr; a = a->sig) {
            // Cada conexion esta guardada dos veces: se imprime solo en un sentido.
            if (util::minusculas(c->nombre) < util::minusculas(a->destino->nombre)) {
                std::cout << "  " << c->nombre << " <-> " << a->destino->nombre << " : "
                          << util::numeroATexto(a->distancia) << " km\n";
                ++mostradas;
            }
        }
    }
    if (mostradas == 0) std::cout << "  (no hay conexiones registradas)\n";
}

int Grafo::cantidadConexiones() const {
    int suma = 0;
    for (const Ciudad* c = cabeza; c != nullptr; c = c->sig) suma += grado(c);
    return suma / 2;  // cada conexion se conto dos veces
}

double Grafo::kilometrosTotales() const {
    double suma = 0;
    for (const Ciudad* c = cabeza; c != nullptr; c = c->sig)
        for (const Arista* a = c->aristas; a != nullptr; a = a->sig) suma += a->distancia;
    return suma / 2;
}

// ---------------------------------------------------------------------- BFS
// Recorrido por NIVELES. Se usa una cola (arreglo + dos posiciones) y un
// arreglo 'visitada'. Costo: O(V + E).
bool Grafo::recorridoBFS(const std::string& inicio) {
    Ciudad* origen = buscarMutable(inicio);
    if (origen == nullptr) return false;

    Ciudad** todas = numerar();
    bool* visitada = new bool[cantidad];
    int* nivel = new int[cantidad];
    int* cola = new int[cantidad];  // guarda indices de ciudades
    for (int i = 0; i < cantidad; ++i) visitada[i] = false;
    int frente = 0, fin = 0;

    std::cout << "  BFS desde " << origen->nombre << " (orden de visita por niveles):\n";
    visitada[origen->indice] = true;
    nivel[origen->indice] = 0;
    cola[fin++] = origen->indice;  // encolar el inicio

    while (frente < fin) {
        int u = cola[frente++];  // atender el frente de la cola
        std::cout << "    nivel " << nivel[u] << ": " << todas[u]->nombre << "\n";
        for (Arista* a = todas[u]->aristas; a != nullptr; a = a->sig) {
            int v = a->destino->indice;
            if (!visitada[v]) {  // solo se encola lo que nunca se ha visto
                visitada[v] = true;
                nivel[v] = nivel[u] + 1;
                cola[fin++] = v;
            }
        }
    }
    std::cout << "  Ciudades alcanzadas: " << fin << " de " << cantidad << "\n";

    delete[] todas;
    delete[] visitada;
    delete[] nivel;
    delete[] cola;
    return true;
}

// ---------------------------------------------------------------------- DFS
// Visita una ciudad y se mete lo mas profundo posible por el primer vecino no
// visitado antes de volver (la pila es la RECURSION). Costo: O(V + E).
static void dfsDesde(Ciudad* u, bool* visitada, int profundidad, int& contador) {
    visitada[u->indice] = true;
    ++contador;
    std::cout << "    " << std::string(profundidad * 2, ' ') << u->nombre << "\n";
    for (Arista* a = u->aristas; a != nullptr; a = a->sig)
        if (!visitada[a->destino->indice])
            dfsDesde(a->destino, visitada, profundidad + 1, contador);
}

bool Grafo::recorridoDFS(const std::string& inicio) {
    Ciudad* origen = buscarMutable(inicio);
    if (origen == nullptr) return false;

    Ciudad** todas = numerar();
    bool* visitada = new bool[cantidad];
    for (int i = 0; i < cantidad; ++i) visitada[i] = false;
    int contador = 0;

    std::cout << "  DFS desde " << origen->nombre
              << " (orden de visita; la sangria indica la profundidad):\n";
    dfsDesde(origen, visitada, 0, contador);
    std::cout << "  Ciudades alcanzadas: " << contador << " de " << cantidad << "\n";

    delete[] todas;
    delete[] visitada;
    return true;
}

// ----------------------------------------------------------------- Dijkstra
// Camino de menor distancia. Requiere distancias NO negativas (aqui siempre
// son > 0). Costo con arreglos: O(V^2).
bool Grafo::rutaMasCorta(const std::string& origen, const std::string& destino,
                         double& total, std::string& camino) {
    Ciudad* co = buscarMutable(origen);
    Ciudad* cd = buscarMutable(destino);
    if (co == nullptr || cd == nullptr) return false;

    Ciudad** todas = numerar();
    double* dist = new double[cantidad];    // mejor distancia conocida desde el origen
    int* previo = new int[cantidad];        // de que ciudad se llego (para armar el camino)
    bool* definitiva = new bool[cantidad];  // ya se sabe que su distancia es la minima
    const double INFINITO = 1e300;

    for (int i = 0; i < cantidad; ++i) {
        dist[i] = INFINITO;
        previo[i] = -1;
        definitiva[i] = false;
    }
    dist[co->indice] = 0;

    for (int paso = 0; paso < cantidad; ++paso) {
        // Elegir la ciudad NO definitiva con menor distancia conocida.
        int u = -1;
        for (int i = 0; i < cantidad; ++i)
            if (!definitiva[i] && dist[i] < INFINITO && (u == -1 || dist[i] < dist[u])) u = i;
        if (u == -1) break;  // lo que queda es inalcanzable
        definitiva[u] = true;

        // Relajar: se llega mas barato a un vecino pasando por 'u'?
        for (Arista* a = todas[u]->aristas; a != nullptr; a = a->sig) {
            int v = a->destino->indice;
            if (!definitiva[v] && dist[u] + a->distancia < dist[v]) {
                dist[v] = dist[u] + a->distancia;
                previo[v] = u;
            }
        }
    }

    bool hayCamino = dist[cd->indice] < INFINITO;
    if (hayCamino) {
        total = dist[cd->indice];
        // Reconstruir el camino yendo hacia atras desde el destino.
        int* pila = new int[cantidad];
        int tope = 0;
        for (int x = cd->indice; x != -1; x = previo[x]) pila[tope++] = x;
        camino.clear();
        for (int i = tope - 1; i >= 0; --i) {  // sacar de la pila = orden origen->destino
            camino += todas[pila[i]]->nombre;
            if (i > 0) camino += " -> ";
        }
        delete[] pila;
    }
    delete[] todas;
    delete[] dist;
    delete[] previo;
    delete[] definitiva;
    return hayCamino;
}
