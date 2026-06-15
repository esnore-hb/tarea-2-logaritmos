// ===========================================================================
// experimentos_avl.cc  —  Driver de la batería de experimentos para el AVL.
//
// Gemelo de experimentos_splay.cc: instancia la misma batería de plantillas de
// include/experimentos.hh, pero con la clase AVL. Como el header fija c, λ y la
// semilla del dataset, AVL y Splay se miden sobre los MISMOS datos y en el MISMO
// orden de inserción, lo que permite superponer ambas curvas en los gráficos.
//
//   Compilar:  g++ -O2 -std=c++17 -static experimentos_avl.cc -o experimentos_avl.out
//   Ejecutar:  ./experimentos_avl.out
//   Oficial:   g++ -O2 -std=c++17 -static -DEXP_73=25 experimentos_avl.cc -o experimentos_avl.out
//
// Salida: base_scenarios.avl.csv, sequential_access.avl.csv, working_set.avl.csv
//         (y mensajes de progreso por stderr).
// ===========================================================================
#include "include/avl.cc"          // clase AVL (Nodo + rotaciones + balanceo)
#include "include/experimentos.hh" // batería de experimentos (plantillas)

int main() {
  correr_todo<AVL>("AVL");
  return 0;
}
