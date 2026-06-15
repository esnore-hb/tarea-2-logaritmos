// ===========================================================================
// experimentos_splay.cc  —  Driver de la batería de experimentos para el SPLAY.
//
// Toda la lógica de los experimentos (7.2 escenarios base, 7.3a Sequential Access,
// 7.3b Working Set) vive en include/experimentos.hh como plantillas. Aquí solo se
// instancia esa batería con la clase SplayTree. El driver gemelo experimentos_avl.cc
// hace lo mismo con AVL y, gracias a la misma semilla/c/λ del header, ambas
// estructuras quedan medidas sobre datos idénticos y son comparables.
//
//   Compilar:  g++ -O2 -std=c++17 -static experimentos_splay.cc -o experimentos_splay.out
//   Ejecutar:  ./experimentos_splay.out
//   Oficial:   g++ -O2 -std=c++17 -static -DEXP_73=25 experimentos_splay.cc -o experimentos_splay.out
//
// Salida: base_scenarios.splay.csv, sequential_access.splay.csv, working_set.splay.csv
//         (y mensajes de progreso por stderr).
// ===========================================================================
#include "include/splay.cc"        // clase SplayTree (Nodo + rotaciones + splay)
#include "include/experimentos.hh" // batería de experimentos (plantillas)

int main() {
  correr_todo<SplayTree>("Splay");
  return 0;
}
