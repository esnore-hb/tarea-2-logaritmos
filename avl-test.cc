#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "include/avl.cc"

int C = 10;
double lambda = 0.02;

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Uso: " << argv[0] << " <archivo.data>\n";
    return 1;
  }

  const std::string filename = argv[1];

  std::ifstream input(filename, std::ios::binary);
  if (!input.is_open()) {
    std::cerr << "[Error]: no se pudo abrir el archivo '" << filename << "'.\n";
    return 1;
  }

  // Leer todos los uint32_t del archivo
  uint32_t value;
  std::vector<uint32_t> keys;
  while (input.read(reinterpret_cast<char *>(&value), sizeof(value))) {
    keys.push_back(value);
  }
  if (!input.eof()) {
    std::cerr << "[Error]: lectura incompleta o archivo corrupto.\n";
    return 1;
  }
  input.close();
  std::cout << "[LOAD] AVL cargado con " << filename << "\n";

  // Test de insertar
  AVL avl = AVL();
  for (auto x : keys) avl.insert(x);
  std::cout << "[INSERT] Se insertaron los elementos aleatorios\n";

  // Test de insertar ordenadamente
  AVL avl_sorted = AVL();
  std::vector<uint32_t> keys2;
  for(auto x: keys) keys2.push_back(x);
  std::sort(keys2.begin(), keys2.end());
  for (auto x : keys) avl.insert(x);
  std::cout << "[INSERT] Se insertaron los elementos ordenados\n";

  // Constantes
  size_t N = keys.size();
  size_t M = 10 * C * N;

  // Test de busqueda equiprobable
  std::random_device rd_equi;
  std::mt19937 gen_equi(rd_equi());
  std::uniform_int_distribution<size_t> uniform_dist(0, N - 1);
  // Busqueda M veces
  for (size_t i = 0; i < M; ++i) {
    avl.search(keys[uniform_dist(gen_equi)]);
  }
  std::cout << "[SEARCH] Se buscaron los elementos de manera equiprobable\n";

  // Test de busqueda sesgada
  std::vector<double> weights(N);
  for (size_t i = 0; i < N; ++i) {
    weights[i] = std::exp(-lambda * i);
  }
  std::random_device rd_ses;
  std::mt19937 gen_ses(rd_ses());
  std::discrete_distribution<size_t> discrete_dist(
    weights.begin(),
    weights.end()
  );
  // Busqueda M veces
  for (size_t i = 0; i < M; ++i) {
    avl.search(keys[discrete_dist(gen_ses)]);
  }
  std::cout << "[SEARCH] Se buscaron los elementos de manera sesgada\n";

  std::cout << "[SUCCESS] Se terminó el test para el archivo" << filename;
  std::cout << std::endl;
  return 0;
}