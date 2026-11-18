/*
 * Por que BFS sirve para estos tres casos
 *
 * BFS recorre el arbol por niveles: primero la raiz, luego sus hijos,
 * despues los nietos, y asi. La cola mantiene el orden de izquierda
 * a derecha, por eso nos facilita las siguientes comprobaciones.
 *
 * 1) Ancho maximo:
 *    Antes de recorrer un nivel, el tamano de la cola indica cuantos
 *    nodos hay en ese nivel. Solo guardamos el mayor valor encontrado.
 *    Recorremos O(n) nodos y la cola usa hasta O(w) memoria, donde w
 *    es el ancho maximo.
 *
 * 2) Profundidad minima:
 *    Como avanzamos nivel por nivel, la primera hoja que encontramos
 *    es la mas cercana a la raiz. Podemos detenernos en ese momento;
 *    no hace falta revisar las ramas mas profundas.
 *
 * 3) Arbol completo:
 *    En un arbol completo los niveles estan llenos, salvo tal vez el
 *    ultimo, que se llena de izquierda a derecha. Si aparece un espacio
 *    vacio, no deberia haber mas hijos despues de ese espacio.
 *    Tampoco puede existir un hijo derecho sin uno izquierdo.
 *    BFS permite revisar esas condiciones en orden y cortar apenas
 *    detectamos un problema.
 *
 * Para comprobar el ancho o si es completo, el tiempo es O(n) y la
 * memoria O(w). DFS tambien podria usarse para algunas de estas tareas,
 * pero BFS permite aprovechar directamente el orden por niveles.
 */

#include <iostream>
#include <stack>
#include <queue>

#include <algorithm>
#include <string>
using namespace std;

template<typename Nodo, typename Visitar>
void recorrer_por_niveles(const Nodo* raiz, Visitar visitar) {
    if (raiz == nullptr) return;
    queue<const Nodo*> por_visitar;
    por_visitar.push(raiz);
    int nivel = 0;

    while (!por_visitar.empty()) {
        nivel++;
        size_t nodos_del_nivel = por_visitar.size();
        for (size_t k = 0; k < nodos_del_nivel; k++) {
            const Nodo* nodo_actual = por_visitar.front();
            por_visitar.pop();
            if (!visitar(nodo_actual, nivel, nodos_del_nivel)) return;
            if (nodo_actual->izq) por_visitar.push(nodo_actual->izq);
            if (nodo_actual->der) por_visitar.push(nodo_actual->der);
        }
    }
}

template<typename Nodo>
int altura_arbol(const Nodo* raiz) {
    int altura = 0;
    recorrer_por_niveles(raiz, [&altura](const Nodo*, int nivel, size_t) {
        altura = nivel;
        return true;
    });
    return altura;
}

template<typename Nodo, typename Visitar>
void recorrido_preorden(const Nodo* raiz, Visitar visitar) {
    if (raiz == nullptr) return;
    stack<const Nodo*> pila;
    pila.push(raiz);
    while (!pila.empty()) {
        const Nodo* nodo_actual = pila.top();
        pila.pop();
        visitar(nodo_actual);
        if (nodo_actual->der) pila.push(nodo_actual->der);
        if (nodo_actual->izq) pila.push(nodo_actual->izq);
    }
}

template<typename Nodo>
void liberar_arbol(Nodo* raiz) {
    if (raiz == nullptr) return;
    stack<Nodo*> pila;
    pila.push(raiz);
    while (!pila.empty()) {
        Nodo* nodo_actual = pila.top();
        pila.pop();
        if (nodo_actual->izq) pila.push(nodo_actual->izq);
        if (nodo_actual->der) pila.push(nodo_actual->der);
        delete nodo_actual;
    }
}

template<typename T>
class BST {
public:
    struct Nodo {
        T valor;
        Nodo* izq = nullptr;
        Nodo* der = nullptr;
        explicit Nodo(const T& v) : valor(v) {}
    };

private:
    Nodo* raiz = nullptr;

    Nodo** buscar_enlace(const T& valor) {
        Nodo** enlace = &raiz;
        while (*enlace != nullptr) {
            if (valor < (*enlace)->valor)      enlace = &(*enlace)->izq;
            else if ((*enlace)->valor < valor) enlace = &(*enlace)->der;
            else break;
        }
        return enlace;
    }

public:
    BST() = default;
    BST(const BST&) = delete;
    BST& operator=(const BST&) = delete;
    ~BST() { liberar_arbol(raiz); }

    bool insertar(const T& valor) {
        Nodo** enlace = buscar_enlace(valor);
        if (*enlace != nullptr) return false;
        *enlace = new Nodo(valor);
        return true;
    }

    void imprimir_preorden() const {
        recorrido_preorden(raiz, [](const Nodo* n) { cout << n->valor << ' '; });
        cout << endl;
    }

    int altura() const { return altura_arbol(raiz); }
    const Nodo* get_raiz() const { return raiz; }
};

template<typename Nodo>
bool es_completo(const Nodo* raiz) {
    bool hueco_visto = false;
    bool completo = true;
    recorrer_por_niveles(raiz, [&](const Nodo* n, int, size_t) {
        if ((n->izq == nullptr && n->der != nullptr) ||
            (hueco_visto && (n->izq != nullptr || n->der != nullptr))) {
            completo = false;
            return false;
        }
        if (n->izq == nullptr || n->der == nullptr) hueco_visto = true;
        return true;
    });
    return completo;
}

template<typename Nodo>
size_t ancho_maximo(const Nodo* raiz) {
    size_t ancho = 0;
    recorrer_por_niveles(raiz, [&ancho](const Nodo*, int, size_t nodos_del_nivel) {
        ancho = max(ancho, nodos_del_nivel);
        return true;
    });
    return ancho;
}

template<typename Nodo>
int profundidad_minima(const Nodo* raiz) {
    int profundidad = 0;
    recorrer_por_niveles(raiz, [&profundidad](const Nodo* n, int nivel, size_t) {
        if (n->izq == nullptr && n->der == nullptr) {
            profundidad = nivel;
            return false;
        }
        return true;
    });
    return profundidad;
}

template<typename T>
void analizar(const string& nombre, const BST<T>& arbol) {
    const auto* raiz = arbol.get_raiz();
    cout << nombre << endl;
    cout << "  Pre-order: ";
    arbol.imprimir_preorden();
    cout << "  Completo: " << (es_completo(raiz) ? "SI" : "NO") << endl;
    cout << "  Ancho maximo: " << ancho_maximo(raiz) << endl;
    cout << "  Profundidad minima: " << profundidad_minima(raiz) << endl;
    cout << "  Altura: " << arbol.altura() << "\n" << endl;
}

int main() {
        BST<int> a1;
    for (int x : {50, 30, 70, 20, 40, 60}) a1.insertar(x);
    analizar("Arbol 1 (completo)", a1);

        BST<int> a2;
    for (int x : {50, 30, 70, 20, 80}) a2.insertar(x);
    analizar("Arbol 2 (hueco en el ultimo nivel)", a2);

        BST<int> a3;
    for (int x : {50, 30, 70, 20, 40, 60, 80, 10, 25, 90}) a3.insertar(x);
    analizar("Arbol 3 (no completo)", a3);

    BST<int> a4;
    for (int x : {1, 2, 3, 4}) a4.insertar(x);
    analizar("Arbol 4 (degenerado)", a4);
}
