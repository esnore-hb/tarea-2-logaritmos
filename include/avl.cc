#include <algorithm>
#include <cstdint>

struct Nodo {
  uint32_t valor, height;
  Nodo *izq;
  Nodo *der;

  Nodo(uint32_t v) {
    valor = v;
    height = 1; // un nodo recien creado es una hoja, altura 1
    izq = nullptr;
    der = nullptr;
  }
};

uint32_t altura(Nodo *nodo) {
  if (nodo == nullptr)
    return 0;
  return nodo->height;
}
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

Nodo *zig(Nodo *y) {
  Nodo *x = y->izq;
  Nodo *T2 = x->der;
  x->der = y;
  y->izq = T2;
  actualizar_altura(y);
  actualizar_altura(x);
  return x;
}

Nodo *zag(Nodo *x) {
  Nodo *y = x->der;
  Nodo *T2 = y->izq;
  y->izq = x;
  x->der = T2;
  actualizar_altura(x);
  actualizar_altura(y);
  return y;
}

Nodo *zig_zag(Nodo *z) {
  Nodo *y = z->izq;
  z->izq = zag(y);
  return zig(z);
}

Nodo *zag_zig(Nodo *z) {
  Nodo *y = z->der;
  z->der = zig(y); // x
  return zag(z);
}

Nodo *zag_zag(Nodo *z) { return zag(zag(z)); }

Nodo *zig_zig(Nodo *z) { return zig(zig(z)); }

class AVL {
private:
  Nodo *raiz;

  // Restaura el balance si BF llegó a +-2 (tras una inserción el desbalance
  // es exactamente +-2). Devuelve la nueva raíz del subárbol ya balanceado.
  Nodo *balancear(Nodo *nodo) {
    int bf = balance_factor(nodo);
    if (bf == 2) { // cargado a la izquierda
      if (balance_factor(nodo->izq) >= 0)
        return zig(nodo); // caso Left-Left
      else
        return zig_zag(nodo); // caso Left-Right
    }
    if (bf == -2) { // cargado a la derecha
      if (balance_factor(nodo->der) <= 0)
        return zag(nodo); // caso Right-Right
      else
        return zag_zig(nodo); // caso Right-Left
    }
    return nodo;
  }

  // Inserción recursiva tipo ABB; al volver actualiza altura y rebalancea.
  Nodo *insertar(Nodo *nodo, uint32_t valor) {
    if (nodo == nullptr)
      return new Nodo(valor);
    if (valor < nodo->valor) // TODO: revisar duplicados izquierda
      nodo->izq = insertar(nodo->izq, valor);
    else if (valor > nodo->valor)
      nodo->der = insertar(nodo->der, valor);
    else
      return nodo;

    actualizar_altura(nodo);
    return balancear(nodo);
  }

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

  void insert(uint32_t valor) { raiz = insertar(raiz, valor); }

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
