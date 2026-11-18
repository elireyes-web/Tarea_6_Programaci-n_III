/*
 * Por que un Red-Black Tree mantiene altura O(log n)
 *
 * A diferencia del BST normal, este arbol se va acomodando con cambios
 * de color y rotaciones. Lo importante es que no deja crecer una rama
 * demasiado en comparacion con las demas.
 *
 * La regla de que un nodo rojo no puede tener hijos rojos evita que
 * aparezcan muchos rojos seguidos. Ademas, todos los caminos desde un
 * nodo hasta sus hojas nulas tienen la misma cantidad de nodos negros.
 *
 * Si llamamos bh a esa cantidad de negros, un subarbol necesita por
 * lo menos 2^bh - 1 nodos internos. Se entiende por induccion: con bh=0
 * tenemos la hoja nula, y al aumentar la altura negra se necesitan
 * dos subarboles con la cantidad minima correspondiente.
 *
 * Como en un camino no puede haber dos rojos consecutivos, al menos
 * la mitad de sus nodos son negros. Asi, si h es la altura, se cumple:
 *
 *     n >= 2^(h/2) - 1
 *     h <= 2 * log2(n + 1)
 *
 * Entonces la altura es O(log n). Insertar primero recorre esa altura,
 * y despues corrige los colores o hace rotaciones. La correccion puede
 * subir varios niveles, pero solo realiza hasta dos rotaciones.
 * En total, la insercion sigue siendo O(log n).
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

    void imprimir_preorden() const {
        recorrido_preorden(raiz, [](const Nodo* n) { cout << n->valor << ' '; });
        cout << endl;
    }

    int altura() const { return altura_arbol(raiz); }
};

template<typename T>
class RedBlackTree {
public:
    struct Nodo {
        T valor;
        bool rojo = true;
        Nodo* izq = nullptr;
        Nodo* der = nullptr;
        Nodo* padre = nullptr;
        Nodo(const T& v, Nodo* p) : valor(v), padre(p) {}
    };

private:
    Nodo* raiz = nullptr;

    static bool es_rojo(const Nodo* n) { return n != nullptr && n->rojo; }

    void reemplazar_en_padre(Nodo* viejo, Nodo* nuevo) {
        nuevo->padre = viejo->padre;
        if (viejo->padre == nullptr)          raiz = nuevo;
        else if (viejo == viejo->padre->izq)  viejo->padre->izq = nuevo;
        else                                  viejo->padre->der = nuevo;
        viejo->padre = nuevo;
    }

        void rotar_izquierda(Nodo* x) {
        Nodo* y = x->der;
        x->der = y->izq;
        if (y->izq) y->izq->padre = x;
        reemplazar_en_padre(x, y);
        y->izq = x;
    }

    void rotar_derecha(Nodo* x) {
        Nodo* y = x->izq;
        x->izq = y->der;
        if (y->der) y->der->padre = x;
        reemplazar_en_padre(x, y);
        y->der = x;
    }

    void corregir_insercion(Nodo* z) {
        while (es_rojo(z->padre)) {
            Nodo* padre = z->padre;
            Nodo* abuelo = padre->padre;
            bool padre_es_izq = (padre == abuelo->izq);
            Nodo* tio = padre_es_izq ? abuelo->der : abuelo->izq;

            if (es_rojo(tio)) {
                padre->rojo = false;
                tio->rojo = false;
                abuelo->rojo = true;
                z = abuelo;
                continue;
            }

            if (padre_es_izq) {
                if (z == padre->der) {
                    rotar_izquierda(padre);
                    padre = z;
                }
                rotar_derecha(abuelo);
            } else {
                if (z == padre->izq) {
                    rotar_derecha(padre);
                    padre = z;
                }
                rotar_izquierda(abuelo);
            }
            padre->rojo = false;
            abuelo->rojo = true;
            break;
        }
        raiz->rojo = false;
    }

public:
    RedBlackTree() = default;
    RedBlackTree(const RedBlackTree&) = delete;
    RedBlackTree& operator=(const RedBlackTree&) = delete;
    ~RedBlackTree() { liberar_arbol(raiz); }

    bool insertar(const T& valor) {
        Nodo* padre = nullptr;
        Nodo** enlace = &raiz;
        while (*enlace != nullptr) {
            padre = *enlace;
            if (valor < padre->valor)      enlace = &padre->izq;
            else if (padre->valor < valor) enlace = &padre->der;
            else return false;
        }
        Nodo* nuevo = new Nodo(valor, padre);
        *enlace = nuevo;
        corregir_insercion(nuevo);
        return true;
    }

    bool verificar_propiedades() const {
        if (raiz == nullptr) return true;

        if (raiz->rojo) {
            cout << "Falla P2: la raiz es roja" << endl;
            return false;
        }

        stack<pair<const Nodo*, int>> pila;
        pila.push({raiz, 1});
        int altura_negra = -1;

        while (!pila.empty()) {
            auto [nodo, negros] = pila.top();
            pila.pop();

            if (nodo->rojo && (es_rojo(nodo->izq) || es_rojo(nodo->der))) {
                cout << "Falla P3: nodo rojo " << nodo->valor << " con hijo rojo" << endl;
                return false;
            }

            for (const Nodo* hijo : {nodo->izq, nodo->der}) {
                if (hijo == nullptr) {
                    if (altura_negra == -1) altura_negra = negros;
                    else if (negros != altura_negra) {
                        cout << "Falla P4: caminos con distinta cantidad de negros" << endl;
                        return false;
                    }
                } else {
                    if (hijo->padre != nodo) {
                        cout << "Falla estructural: puntero padre incorrecto en "
                             << hijo->valor << endl;
                        return false;
                    }
                    pila.push({hijo, negros + (hijo->rojo ? 0 : 1)});
                }
            }
        }

        bool ordenado = true;
        const T* anterior = nullptr;
        recorrido_inorden(raiz, [&](const Nodo* n) {
            if (anterior != nullptr && !(*anterior < n->valor)) ordenado = false;
            anterior = &n->valor;
        });
        if (!ordenado) {
            cout << "Falla orden BST" << endl;
            return false;
        }
        return true;
    }

    int altura() const { return altura_arbol(raiz); }

    void imprimir_preorden() const {
        recorrido_preorden(raiz, [](const Nodo* n) {
            cout << n->valor << (n->rojo ? "(R) " : "(N) ");
        });
        cout << endl;
    }
};

int main() {
    const int N = 20;
    BST<int> bst;
    RedBlackTree<int> rbt;

    for (int i = 1; i <= N; i++) {
        bst.insertar(i);
        rbt.insertar(i);
        if (!rbt.verificar_propiedades()) {
            cout << "RB-Tree invalido tras insertar " << i << endl;
            return 1;
        }
    }

    cout << "Insertando 1.." << N << " en orden ascendente\n" << endl;

    cout << "BST - Pre-order: ";
    bst.imprimir_preorden();
    cout << "BST - Altura: " << bst.altura() << "\n" << endl;

    cout << "Red-Black Tree - Pre-order: ";
    rbt.imprimir_preorden();
    cout << "Red-Black Tree - Altura: " << rbt.altura() << endl;
    cout << "Red-Black Tree - Propiedades validas: "
         << (rbt.verificar_propiedades() ? "SI" : "NO") << endl;

    cout << "\nCota teorica RB: h <= 2*log2(n+1) = 2*log2(" << N + 1 << ") ~ 8.78" << endl;
}
