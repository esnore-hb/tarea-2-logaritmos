#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "include/avl.cc"

constexpr int C = 10;
constexpr double lambda = 0.02;

int main(int argc, char *argv[]) {
  if (argc < 2 || argc > 3) {
    std::cerr << "Uso:\n"
              << "  " << argv[0] << " N\n"
              << "  " << argv[0] << " N -b\n"
              << "  " << argv[0] << " N -o\n"
              << "  " << argv[0] << " N -bo\n";
    return 1;
  }

  /* Configuración por defecto */
  bool biased_search = false;
  bool ordered_insert = false;

  if (argc == 3) {
    std::string flag = argv[2];

    if (flag == "-b") {
      biased_search = true;
    } else if (flag == "-o") {
      ordered_insert = true;
    } else if (flag == "-bo" || flag == "-ob") {
      biased_search = true;
      ordered_insert = true;
    } else {
      std::cerr << "[Error] Flag desconocido: " << flag << '\n';
      return 1;
    }
  }

  /* Construir nombre del archivo */
  std::string n_str = argv[1];
  std::string filename = "./data/datos_" + n_str + ".data";

  std::ifstream input(filename, std::ios::binary);

  if (!input.is_open()) {
    std::cerr << "[Error] No se pudo abrir '" << filename << "'\n";
    return 1;
  }

  /* Leer datos */
  std::vector<uint32_t> keys;
  uint32_t value;
  while (input.read(reinterpret_cast<char *>(&value), sizeof(value))) {
    keys.push_back(value);
  }
  if (!input.eof()) {
    std::cerr << "[Error] Archivo corrupto.\n";
    return 1;
  }
  input.close();
  std::cout << "[LOAD] AVL cargado desde " << filename << " (" << keys.size()
            << " claves)\n";

  /* Construcción del AVL */
  AVL avl;

  /* Benchmark de Insercción ordenada*/
  if (ordered_insert) {
    std::vector<uint32_t> sorted_keys = keys;
    std::sort(sorted_keys.begin(), sorted_keys.end());

    // Insercción N veces
    auto start = std::chrono::steady_clock::now();
    for (uint32_t x : sorted_keys) {
      avl.insert(x);
    }
    auto end = std::chrono::steady_clock::now();
    auto elapsed_us =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "[INSERT] Inserción ordenada: " << elapsed_us.count()
              << " us\n";

    /* Benchmark de Insercción aleatoria*/
  } else {

    // Insercción N veces
    auto start = std::chrono::steady_clock::now();
    for (uint32_t x : keys) {
      avl.insert(x);
    }
    auto end = std::chrono::steady_clock::now();
    auto elapsed_us =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "[INSERT] Inserción aleatoria:" << elapsed_us.count()
              << " us\n";
  }

  /* Parámetros del benchmark */
  size_t N = keys.size();
  size_t M = 10 * C * N;

  std::random_device rd;
  std::mt19937 gen(rd());

  /* Benchmark de búsqueda sesgada*/
  if (biased_search) {
    std::vector<double> weights(N);
    for (size_t i = 0; i < N; ++i) {
      weights[i] = std::exp(-lambda * i);
    }
    std::discrete_distribution<size_t> dist(weights.begin(), weights.end());

    // Búsqueda M veces
    auto start = std::chrono::steady_clock::now();
    for (size_t i = 0; i < M; ++i) {
      avl.search(keys[dist(gen)]);
    }
    auto end = std::chrono::steady_clock::now();
    auto elapsed_us =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "[SEARCH] Búsqueda sesgada: " << elapsed_us.count() << " us\n";

    /* Benchmark de búsqueda sesgada*/
  } else {
    std::uniform_int_distribution<size_t> dist(0, N - 1);

    // Búsqueda M veces
    auto start = std::chrono::steady_clock::now();
    for (size_t i = 0; i < M; ++i) {
      avl.search(keys[dist(gen)]);
    }
    auto end = std::chrono::steady_clock::now();
    auto elapsed_us =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "[SEARCH] Búsqueda equiprobable: " << elapsed_us.count()
              << " us\n";
  }
  std::cout << "[SUCCESS] Benchmark completado.";

  std::cout << std::endl;
  return 0;
}