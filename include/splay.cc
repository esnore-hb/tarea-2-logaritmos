#include <algorithm>
#include <cstdint>
#include <vector>

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

class SplayTree {
private:
  Nodo *raiz;
  std::vector<Nodo *> camino;
  Nodo *splay(Nodo *root, uint32_t x) {
    if (root == nullptr)
      return nullptr;

    camino.clear();
    for (Nodo *cur = root; cur != nullptr;) {
      camino.push_back(cur);
      if (cur->valor == x)
        break;
      cur = (x < cur->valor) ? cur->izq : cur->der;
    }

    int top = (int)camino.size() - 1; // indice del objetivo en el camino
    while (top > 0) {
      if (top == 1) {
        // El padre ya es la raiz
        Nodo *p = camino[0];
        Nodo *t = camino[1];
        camino[0] = (p->izq == t) ? zig(p) : zag(p);
        top = 0;
      } else {
        Nodo *g = camino[top - 2]; // abuelo (z)
        Nodo *p = camino[top - 1]; // padre  (y)
        Nodo *t = camino[top];     // objetivo (x)
        bool p_izq = (g->izq == p);
        bool t_izq = (p->izq == t);

        Nodo *sub; // nueva raiz del subarbol rotado (== t)
        if (p_izq && t_izq)
          sub = zig_zig(g); // izquierda-izquierda
        else if (!p_izq && !t_izq)
          sub = zag_zag(g); // derecha-derecha
        else if (p_izq && !t_izq)
          sub = zig_zag(g); // izquierda-derecha
        else
          sub = zag_zig(g); // derecha-izquierda

        if (top - 2 == 0) {
          camino[0] = sub; // el subarbol rotado es la nueva raiz global
          top = 0;
        } else {
          // Reconectar 'sub' al bisabuelo en lugar del antiguo abuelo 'g'.
          Nodo *bis = camino[top - 3];
          if (bis->izq == g)
            bis->izq = sub;
          else
            bis->der = sub;
          camino[top - 2] = sub; // el objetivo subio dos niveles
          top -= 2;
        }
      }
    }
    return camino[0];
  }

  void destruir(Nodo *nodo) {
    if (nodo == nullptr)
      return;
    destruir(nodo->izq);
    destruir(nodo->der);
    delete nodo;
  }

public:
  SplayTree() : raiz(nullptr) {}
  ~SplayTree() { destruir(raiz); }

  bool search(uint32_t x) {
    if (raiz == nullptr)
      return false;
    raiz = splay(raiz, x);
    return raiz->valor == x;
  }

  void insert(uint32_t x) {
    if (raiz == nullptr) {
      raiz = new Nodo(x);
      return;
    }
    raiz = splay(raiz, x); // sube a la raiz el nodo mas cercano a x
    if (raiz->valor == x)
      return; // x ya existe

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
  }
};
