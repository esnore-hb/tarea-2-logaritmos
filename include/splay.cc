#include <algorithm>
#include <cstdint>
#include <vector>

/*
===============================================================================
ESTRUCTURAS DE DATOS
===============================================================================
*/

/**
 * @struct Nodo
 * @brief Representa un nodo dentro del Splay Tree.
 * * @member valor  - Valor (clave) almacenado en el nodo
 * @member height - Altura del subárbol cuya raíz es este nodo
 * @member izq    - Puntero al hijo izquierdo
 * @member der    - Puntero al hijo derecho
 */
struct Nodo {
  uint32_t valor, height;
  Nodo *izq;
  Nodo *der;

  Nodo(uint32_t v) {
    valor = v;
    height = 1; // un nodo recien creado es una hoja, por ende es de altura 1
    izq = nullptr;
    der = nullptr;
  }
};

/*
===============================================================================
FUNCIONES AUXILIARES Y ROTACIONES
===============================================================================
*/

/**
 * @function altura
 * @brief Obtiene la altura de un nodo de forma segura.
 * * @param  nodo Puntero al nodo a consultar
 * @return uint32_t Altura del nodo
 */
uint32_t altura(Nodo *nodo) {
  if (nodo == nullptr)
    return 0;
  return nodo->height;
}

/**
 * @function actualizar_altura
 * @brief Recalcula la altura de un nodo basándose en sus hijos.
 * * @param  nodo Puntero al nodo que se desea actualizar
 */
void actualizar_altura(Nodo *nodo) {
  if (nodo != nullptr) {
    nodo->height = 1 + std::max(altura(nodo->izq), altura(nodo->der));
  }
}

int balance_factor(Nodo *nodo) {
  if (nodo == nullptr)
    return 0;
  return altura(nodo->izq) - altura(nodo->der);
}

/**
 * @function zig
 * @brief Realiza una rotación simple a la derecha.
 * * @param  y Puntero a la raíz original
 * @return Nodo* Puntero a la nueva raíz
 */
Nodo *zig(Nodo *y) {
  Nodo *x = y->izq;
  Nodo *T2 = x->der;
  x->der = y;
  y->izq = T2;
  actualizar_altura(y);
  actualizar_altura(x);
  return x;
}

/**
 * @function zag
 * @brief Realiza una rotación simple a la izquierda.
 * * @param  x Puntero a la raíz original
 * @return Nodo* Puntero a la nueva raíz
 */
Nodo *zag(Nodo *x) {
  Nodo *y = x->der;
  Nodo *T2 = y->izq;
  y->izq = x;
  x->der = T2;
  actualizar_altura(x);
  actualizar_altura(y);
  return y;
}

/**
 * @function zig_zag
 * @brief Realiza una rotación doble Izquierda-Derecha.
 */
Nodo *zig_zag(Nodo *z) {
  Nodo *y = z->izq;
  z->izq = zag(y);
  return zig(z);
}

/**
 * @function zag_zig
 * @brief Realiza una rotación doble Derecha-Izquierda.
 */
Nodo *zag_zig(Nodo *z) {
  Nodo *y = z->der;
  z->der = zig(y); 
  return zag(z);
}

/**
 * @function zag_zag
 * @brief Realiza rotación Zag-Zag (izquierda-izquierda).
 */
Nodo *zag_zag(Nodo *z) { return zag(zag(z)); }

/**
 * @function zig_zig
 * @brief Realiza rotación Zig-Zig (derecha-derecha).
 */
Nodo *zig_zig(Nodo *z) { return zig(zig(z)); }

/*
===============================================================================
CLASE PRINCIPAL: SPLAY TREE
===============================================================================
*/

/**
 * @class SplayTree
 * @brief Implementación de un Splay Tree.
 * * Garantiza tiempo amortizado O(log n) moviendo los elementos accedidos
 * recientemente hacia la raíz mediante la operación splay.
 */
class SplayTree {
private:
  Nodo *raiz;
  std::vector<Nodo *> camino;
  size_t n_; // cantidad de claves distintas almacenadas

  /**
   * @function splay
   * @brief Sube el nodo con valor x (o el más cercano) a la raíz del árbol.
   * * Algoritmo:
   * 1. Busca el nodo y guarda el camino recorrido.
   * 2. Itera desde el nodo objetivo hacia arriba aplicando rotaciones.
   * 3. Aplica Zig o Zag si es hijo directo de la raíz.
   * 4. Aplica Zig-Zig, Zag-Zag, Zig-Zag o Zag-Zig si tiene abuelo.
   * * @param  root Raíz actual del árbol
   * @param  x Valor objetivo
   * @return Nodo* Nueva raíz del árbol tras las rotaciones
   */
  Nodo *splay(Nodo *root, uint32_t x) {
    if (root == nullptr)
      return nullptr;

    // Paso 1: Encontrar el nodo y guardar el camino
    camino.clear();
    for (Nodo *cur = root; cur != nullptr;) {
      camino.push_back(cur);
      if (cur->valor == x)
        break;
      cur = (x < cur->valor) ? cur->izq : cur->der;
    }

    // Paso 2: Subir el nodo objetivo a través de rotaciones
    int top = (int)camino.size() - 1; 
    while (top > 0) {
      if (top == 1) {
        // Caso 1: El padre es la raíz (Rotación Simple)
        Nodo *p = camino[0];
        Nodo *t = camino[1];
        camino[0] = (p->izq == t) ? zig(p) : zag(p);
        top = 0;
      } else {
        // Caso 2: El nodo tiene abuelo (Rotaciones Dobles)
        Nodo *g = camino[top - 2]; // Abuelo
        Nodo *p = camino[top - 1]; // Padre
        Nodo *t = camino[top];     // Objetivo
        
        bool p_izq = (g->izq == p);
        bool t_izq = (p->izq == t);

        Nodo *sub; 
        if (p_izq && t_izq)
          sub = zig_zig(g); // Zig-Zig
        else if (!p_izq && !t_izq)
          sub = zag_zag(g); // Zag-Zag
        else if (p_izq && !t_izq)
          sub = zig_zag(g); // Zig-Zag
        else
          sub = zag_zig(g); // Zag-Zig

        if (top - 2 == 0) {
          camino[0] = sub; // Subárbol es la nueva raíz global
          top = 0;
        } else {
          // Paso 3: Reconectar con el bisabuelo
          Nodo *bis = camino[top - 3];
          if (bis->izq == g)
            bis->izq = sub;
          else
            bis->der = sub;
          camino[top - 2] = sub; 
          top -= 2;
        }
      }
    }
    return camino[0];
  }

  /**
   * @function destruir
   * @brief Libera la memoria de todos los nodos de forma ITERATIVA.
   * * Se usa una pila explícita (en el heap) en lugar de recursión para evitar
   * desbordar la pila de llamadas: tras una secuencia de accesos un Splay Tree
   * puede degenerar en un camino de profundidad ~n (p. ej. n = 2^25), lo que
   * haría stack-overflow con un destructor recursivo.
   * * @param  nodo Raíz del subárbol a destruir
   */
  void destruir(Nodo *nodo) {
    std::vector<Nodo *> pila;
    if (nodo != nullptr)
      pila.push_back(nodo);
    while (!pila.empty()) {
      Nodo *cur = pila.back();
      pila.pop_back();
      if (cur->izq != nullptr)
        pila.push_back(cur->izq);
      if (cur->der != nullptr)
        pila.push_back(cur->der);
      delete cur;
    }
  }

public:
  SplayTree() : raiz(nullptr), n_(0) {}
  ~SplayTree() { destruir(raiz); }

  /**
   * @function size
   * @brief Retorna la cantidad de claves distintas almacenadas en el árbol.
   * * @return size_t Número de nodos del árbol
   */
  size_t size() const { return n_; }

  /**
   * @function search
   * @brief Busca un elemento y realiza splay sobre el nodo accedido.
   * * @param  x Elemento a buscar
   * @return bool True si el elemento existe, False si no
   */
  bool search(uint32_t x) {
    if (raiz == nullptr)
      return false;
    raiz = splay(raiz, x);
    return raiz->valor == x;
  }

  /**
   * @function insert
   * @brief Inserta un nuevo elemento y realiza splay sobre él.
   * * Si el elemento ya existe, simplemente realiza splay sin insertar duplicados.
   * * @param  x Elemento a insertar
   */
  void insert(uint32_t x) {
    if (raiz == nullptr) {
      raiz = new Nodo(x);
      n_++;
      return;
    }

    // Sube a la raíz el nodo más cercano a x
    raiz = splay(raiz, x);
    if (raiz->valor == x)
      return; // El elemento ya existe

    // Realizar inserción en la raíz particionando el árbol original
    Nodo *nuevo = new Nodo(x);
    if (x < raiz->valor) {
      nuevo->izq = raiz->izq;
      nuevo->der = raiz;
      raiz->izq = nullptr;
    } else {
      nuevo->der = raiz->der;
      nuevo->izq = raiz;
      raiz->der = nullptr;
    }
    
    actualizar_altura(raiz);
    actualizar_altura(nuevo);
    raiz = nuevo;
    n_++;
  }
};
