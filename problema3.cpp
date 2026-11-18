/*
 * Por que use DFS para buscar el camino
 *
 * Necesitamos encontrar un camino desde la raiz hasta una hoja cuya
 * suma sea K. Con DFS podemos ir siguiendo una rama completa y revisar
 * la suma cuando llegamos a una hoja.
 *
 * En la pila guardamos el nodo y la suma que llevamos hasta ese punto.
 * Asi, cuando pasamos a otra rama, cada estado ya tiene su propia suma
 * y no tenemos que volver a calcular todo el camino.
 *
 * Tambien es mas comodo para la memoria: la pila y el camino que
 * guardamos ocupan O(h), donde h es la altura del arbol. Con BFS
 * podriamos guardar muchos nodos del mismo nivel y tendriamos que
 * recuperar el camino de cada uno.
 *
 * En el peor caso revisamos todos los nodos, asi que el tiempo es
 * O(n). Si encontramos una hoja con la suma pedida, paramos ahi.
 */

#include <iostream>
#include <stack>
#include <queue>

#include <vector>
using namespace std;

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

    const Nodo* get_raiz() const { return raiz; }
};

template<typename Nodo, typename T>
bool camino_con_suma(const Nodo* raiz, const T& K, vector<T>& camino) {
    camino.clear();
    if (raiz == nullptr) return false;

    struct Estado {
        const Nodo* nodo;
        T suma;
        size_t profundidad;
    };

    stack<Estado> por_visitar;
    por_visitar.push({raiz, raiz->valor, 0});

    while (!por_visitar.empty()) {
        Estado actual = por_visitar.top();
        por_visitar.pop();

        while (camino.size() > actual.profundidad) camino.pop_back();
        camino.push_back(actual.nodo->valor);

        const Nodo* izq = actual.nodo->izq;
        const Nodo* der = actual.nodo->der;

        if (izq == nullptr && der == nullptr) {
            if (actual.suma == K) return true;
            continue;
        }
        if (der) por_visitar.push({der, actual.suma + der->valor, actual.profundidad + 1});
        if (izq) por_visitar.push({izq, actual.suma + izq->valor, actual.profundidad + 1});
    }

    camino.clear();
    return false;
}

template<typename T>
void imprimir_resultado(const BST<T>& arbol, const T& K) {
    vector<T> camino;
    cout << "K = " << K << ": ";
    if (camino_con_suma(arbol.get_raiz(), K, camino)) {
        for (size_t i = 0; i < camino.size(); i++)
            cout << camino[i] << (i + 1 < camino.size() ? " -> " : "");
        cout << endl;
    } else {
        cout << "No existe camino" << endl;
    }
}

int main() {
        BST<int> arbol;
    for (int x : {50, 30, 70, 20, 40, 60, 80, 10, 45, 90}) arbol.insertar(x);

    cout << "Pre-order del arbol: ";
    arbol.imprimir_preorden();

    imprimir_resultado(arbol, 110);
    imprimir_resultado(arbol, 165);
    imprimir_resultado(arbol, 180);
    imprimir_resultado(arbol, 290);
    imprimir_resultado(arbol, 120);
}
