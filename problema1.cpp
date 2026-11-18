/*
 * Por que un BST puede terminar con complejidad O(n)
 *
 * En un BST, buscar, insertar o eliminar depende de cuantos niveles
 * tengamos que recorrer. Si la altura es h, el costo es O(h).
 *
 * El problema aparece cuando insertamos datos ya ordenados. Por ejemplo,
 * con 1, 2, 3, 4... cada numero nuevo queda a la derecha del anterior.
 * En vez de un arbol bien distribuido, terminamos con una cadena.
 *
 * Esto pasa desde el primer nodo: al insertar el siguiente, siempre es
 * mayor que los anteriores y tiene que llegar hasta el final. Por eso,
 * despues de insertar n valores, la altura llega a n.
 *
 * Insertar el elemento i requiere recorrer i-1 nodos. Si sumamos todas
 * las inserciones, sale 0 + 1 + ... + (n-1) = n(n-1)/2, o sea O(n^2)
 * en total. Una sola busqueda o eliminacion puede costar O(n).
 *
 * Si insertamos de mayor a menor, ocurre lo mismo pero hacia la izquierda.
 * Por eso un BST normal solo tiene O(log n) cuando su altura se mantiene
 * cerca de log n; no lo garantiza por si solo.
 */

#include <iostream>
#include <stack>
#include <queue>

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
void recorrido_inorden(const Nodo* raiz, Visitar visitar) {
    stack<const Nodo*> pila;
    const Nodo* actual = raiz;
    while (actual != nullptr || !pila.empty()) {
        while (actual != nullptr) {
            pila.push(actual);
            actual = actual->izq;
        }
        actual = pila.top();
        pila.pop();
        visitar(actual);
        actual = actual->der;
    }
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

    bool eliminar(const T& valor) {
        Nodo** enlace = buscar_enlace(valor);
        Nodo* nodo = *enlace;
        if (nodo == nullptr) return false;

        if (nodo->izq != nullptr && nodo->der != nullptr) {
            Nodo** enlace_sucesor = &nodo->der;
            while ((*enlace_sucesor)->izq != nullptr)
                enlace_sucesor = &(*enlace_sucesor)->izq;
            nodo->valor = (*enlace_sucesor)->valor;
            enlace = enlace_sucesor;
            nodo = *enlace;
        }

        *enlace = (nodo->izq != nullptr) ? nodo->izq : nodo->der;
        delete nodo;
        return true;
    }

    bool contiene(const T& valor) const {
        const Nodo* actual = raiz;
        while (actual != nullptr) {
            if (valor < actual->valor)      actual = actual->izq;
            else if (actual->valor < valor) actual = actual->der;
            else return true;
        }
        return false;
    }

    void imprimir_inorden() const {
        recorrido_inorden(raiz, [](const Nodo* n) { cout << n->valor << ' '; });
        cout << endl;
    }

    void imprimir_preorden() const {
        recorrido_preorden(raiz, [](const Nodo* n) { cout << n->valor << ' '; });
        cout << endl;
    }

    int altura() const { return altura_arbol(raiz); }
};

int main() {
    BST<int> arbol;
    int datos[] = {50, 30, 70, 20, 40, 60, 80, 10, 25, 35,
                   45, 55, 65, 75, 85, 5, 15, 33, 68, 90};
    for (int x : datos) arbol.insertar(x);

    cout << "In-order inicial (20 elementos): ";
    arbol.imprimir_inorden();
    cout << "Altura inicial: " << arbol.altura() << endl;

    arbol.eliminar(30);
    arbol.eliminar(90);

    cout << "In-order tras eliminar 30 y 90: ";
    arbol.imprimir_inorden();
    cout << "Altura final: " << arbol.altura() << endl;

    BST<int> degenerado;
    for (int i = 1; i <= 20; i++) degenerado.insertar(i);

    cout << "\nBST con 1..20 insertados en orden ascendente" << endl;
    cout << "Pre-order: ";
    degenerado.imprimir_preorden();
    cout << "Altura: " << degenerado.altura() << " (= n, es una cadena)" << endl;
}
