#include <algorithm>
#include <cstdint>

/*
===============================================================================
ESTRUCTURAS DE DATOS
===============================================================================
*/

/**
 * @struct Nodo
 * @brief Representa un nodo dentro del árbol binario de búsqueda (AVL).
 * * Esta estructura almacena el valor del nodo, su altura actual en el árbol
 * para facilitar el cálculo del factor de balance, y los punteros a sus
 * respectivos hijos izquierdo y derecho.
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
    height = 1; // Un nodo recien creado es una hoja, por ende su altura es 1
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
 * @return uint32_t Altura del nodo (0 si el nodo es nullptr)
 */
uint32_t altura(Nodo *nodo) {
  if (nodo == nullptr)
    return 0;
  return nodo->height;
}

/**
 * @function actualizar_altura
 * @brief Recalcula y actualiza la altura de un nodo basándose en sus hijos.
 * * @param  nodo Puntero al nodo que se desea actualizar
 */
void actualizar_altura(Nodo *nodo) {
  if (nodo != nullptr) {
    nodo->height = 1 + std::max(altura(nodo->izq), altura(nodo->der));
  }
}

/**
 * @function balance_factor
 * @brief Calcula el factor de equilibrio (Balance Factor) de un nodo.
 * * El factor de equilibrio se define como la diferencia entre la altura
 * del subárbol izquierdo y la altura del subárbol derecho.
 * * @param  nodo Puntero al nodo a consultar
 * @return int Diferencia de alturas (BF). Retorna 0 si el nodo es nullptr.
 */
int balance_factor(Nodo *nodo) {
  if (nodo == nullptr)
    return 0;
  return altura(nodo->izq) - altura(nodo->der);
}

/**
 * @function zig
 * @brief Realiza una rotación simple a la derecha.
 * * @param  y Puntero a la raíz original del subárbol
 * @return Nodo* Puntero a la nueva raíz del subárbol tras la rotación
 */
Nodo *zig(Nodo *y) {
  Nodo *x = y->izq;
  Nodo *T2 = x->der;
  
  // Realizar rotación
  x->der = y;
  y->izq = T2;
  
  // Actualizar alturas
  actualizar_altura(y);
  actualizar_altura(x);
  
  return x;
}

/**
 * @function zag
 * @brief Realiza una rotación simple a la izquierda.
 * * @param  x Puntero a la raíz original del subárbol
 * @return Nodo* Puntero a la nueva raíz del subárbol tras la rotación
 */
Nodo *zag(Nodo *x) {
  Nodo *y = x->der;
  Nodo *T2 = y->izq;
  
  // Realizar rotación
  y->izq = x;
  x->der = T2;
  
  // Actualizar alturas
  actualizar_altura(x);
  actualizar_altura(y);
  
  return y;
}

/**
 * @function zig_zag
 * @brief Realiza una rotación doble Izquierda-Derecha (Left-Right Case).
 * * @param  z Puntero a la raíz desbalanceada
 * @return Nodo* Puntero a la nueva raíz del subárbol
 */
Nodo *zig_zag(Nodo *z) {
  Nodo *y = z->izq;
  z->izq = zag(y);
  return zig(z);
}

/**
 * @function zag_zig
 * @brief Realiza una rotación doble Derecha-Izquierda (Right-Left Case).
 * * @param  z Puntero a la raíz desbalanceada
 * @return Nodo* Puntero a la nueva raíz del subárbol
 */
Nodo *zag_zig(Nodo *z) {
  Nodo *y = z->der;
  z->der = zig(y);
  return zag(z);
}

/**
 * @function zag_zag
 * @brief Realiza dos rotaciones simples a la izquierda consecutivas.
 * * @param  z Puntero a la raíz del subárbol
 * @return Nodo* Puntero a la nueva raíz
 */
Nodo *zag_zag(Nodo *z) { 
  return zag(zag(z)); 
}

/**
 * @function zig_zig
 * @brief Realiza dos rotaciones simples a la derecha consecutivas.
 * * @param  z Puntero a la raíz del subárbol
 * @return Nodo* Puntero a la nueva raíz
 */
Nodo *zig_zig(Nodo *z) { 
  return zig(zig(z)); 
}

/*
===============================================================================
CLASE PRINCIPAL: AVL
===============================================================================
*/

/**
 * @class AVL
 * @brief Implementación de un Árbol Binario de Búsqueda Autobalanceable (AVL).
 * * Mantiene el invariante de que la diferencia de alturas entre los hijos
 * de cualquier nodo no es mayor a 1, garantizando tiempos logarítmicos
 * para inserción y búsqueda.
 */
class AVL {
private:
  Nodo *raiz;

  /**
   * @function balancear
   * @brief Restaura el balance del árbol si el factor de balance llega a +-2.
   * * @param  nodo Puntero al nodo que se evaluará y posiblemente rotará
   * @return Nodo* Nueva raíz del subárbol ya balanceado
   */
  Nodo *balancear(Nodo *nodo) {
    int bf = balance_factor(nodo);
    
    // Caso 1: Cargado a la izquierda (Left Case)
    if (bf == 2) { 
      if (balance_factor(nodo->izq) >= 0)
        return zig(nodo);      // Left-Left
      else
        return zig_zag(nodo);  // Left-Right
    }
    
    // Caso 2: Cargado a la derecha (Right Case)
    if (bf == -2) { 
      if (balance_factor(nodo->der) <= 0)
        return zag(nodo);      // Right-Right
      else
        return zag_zig(nodo);  // Right-Left
    }
    
    return nodo;
  }

  /**
   * @function insertar
   * @brief Inserción recursiva clásica de ABB con rebalanceo al retornar.
   * * @param  nodo Raíz actual del subárbol
   * @param  valor Elemento a insertar
   * @return Nodo* Nueva raíz del subárbol modificado
   */
  Nodo *insertar(Nodo *nodo, uint32_t valor) {
    // Paso 1: Inserción normal de ABB
    if (nodo == nullptr)
      return new Nodo(valor);
      
    if (valor < nodo->valor)
      nodo->izq = insertar(nodo->izq, valor);
    else if (valor > nodo->valor)
      nodo->der = insertar(nodo->der, valor);
    else
      return nodo; // No se permiten duplicados

    // Paso 2: Actualizar altura del ancestro actual
    actualizar_altura(nodo);
    
    // Paso 3: Rebalancear el nodo si es necesario
    return balancear(nodo);
  }

  /**
   * @function destruir
   * @brief Libera recursivamente la memoria de todos los nodos en post-orden.
   * * @param  nodo Raíz del subárbol a destruir
   */
  void destruir(Nodo *nodo) {
    if (nodo == nullptr)
      return;
    destruir(nodo->izq);
    destruir(nodo->der);
    delete nodo;
  }

public:
  AVL() : raiz(nullptr) {}
  ~AVL() { destruir(raiz); }

  /**
   * @function insert
   * @brief Método público para insertar un nuevo elemento.
   * * @param  valor Elemento a insertar
   */
  void insert(uint32_t valor) { 
    raiz = insertar(raiz, valor); 
  }

  /**
   * @function search
   * @brief Busca un elemento iterativamente en el árbol.
   * * @param  valor Elemento a buscar
   * @return bool True si el elemento existe, False en caso contrario
   */
  bool search(uint32_t valor) const {
    Nodo *actual = raiz;
    while (actual != nullptr) {
      if (valor == actual->valor)
        return true;
      else if (valor < actual->valor)
        actual = actual->izq;
      else
        actual = actual->der;
    }
    return false;
  }

  uint32_t altura_arbol() const { return altura(raiz); }
};
