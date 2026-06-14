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

/*
===============================================================================
VARIABLES GLOBALES Y CONSTANTES
===============================================================================
*/

/** @brief Constante multiplicativa para la cantidad de búsquedas (M = 10 * c * N) */
constexpr int C = 10;

/** @brief Parámetro lambda para la distribución de probabilidad sesgada (exponencial) */
constexpr double lambda = 0.02;

/*
===============================================================================
FUNCIÓN PRINCIPAL
===============================================================================
*/

/**
 * @function main
 * @brief Programa principal para orquestar y medir los experimentos sobre el AVL.
 * * Descripción:
 * Lee un dataset binario, inicializa un AVL e instrumenta la medición 
 * de tiempos (en microsegundos) para la inserción (aleatoria u ordenada) 
 * y la búsqueda (equiprobable o sesgada exponencialmente).
 * * Parámetros de entrada:
 * argc (int): Cantidad de argumentos (2 o 3).
 * argv (char**): Array de argumentos:
 * argv[0] = nombre del ejecutable.
 * argv[1] = exponente N asociado al dataset (ej. 10 para 2^10).
 * argv[2] = (Opcional) Flags de experimentación:
 * -b  : Búsqueda sesgada.
 * -o  : Inserción ordenada.
 * -bo : Inserción ordenada y búsqueda sesgada.
 * * Salida:
 * Imprime por stdout los tiempos de inserción y búsqueda en microsegundos
 * con etiquetas [INSERT] y [SEARCH] para ser parseadas por scripts de Python.
 * * Retorna:
 * int: 0 si se ejecutó con éxito, 1 ante errores de lectura o parámetros.
 */
int main(int argc, char *argv[]) {
  /*
  ===========================================================================
  VALIDACIÓN DE PARÁMETROS Y FLAGS
  ===========================================================================
  */
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

  /*
  ===========================================================================
  LECTURA DEL DATASET BINARIO
  ===========================================================================
  */
  std::ifstream input(filename, std::ios::binary);

  if (!input.is_open()) {
    std::cerr << "[Error] No se pudo abrir '" << filename << "'\n";
    return 1;
  }

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

  /*
  ===========================================================================
  EXPERIMENTO 1: INSERCIÓN (CONSTRUCCIÓN DEL AVL)
  ===========================================================================
  */
  AVL avl;

  if (ordered_insert) {
    // Preparar dataset ordenado
    std::vector<uint32_t> sorted_keys = keys;
    std::sort(sorted_keys.begin(), sorted_keys.end());

    // Inserción N veces (Ordenada)
    auto start = std::chrono::steady_clock::now();
    for (uint32_t x : sorted_keys) {
      avl.insert(x);
    }
    auto end = std::chrono::steady_clock::now();
    auto elapsed_us =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::cout << "[INSERT] Inserción ordenada: " << elapsed_us.count()
              << " us\n";

  } else {
    // Inserción N veces (Aleatoria)
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

  /*
  ===========================================================================
  EXPERIMENTO 2: BÚSQUEDA
  ===========================================================================
  */
  //Parámetros del Benchmark
  size_t N = keys.size();
  size_t M = 10 * C * N;

  std::random_device rd;
  std::mt19937 gen(rd());

  if (biased_search) {
    // Configurar distribución exponencial sesgada
    std::vector<double> weights(N);
    for (size_t i = 0; i < N; ++i) {
      weights[i] = std::exp(-lambda * i);
    }
    std::discrete_distribution<size_t> dist(weights.begin(), weights.end());

    // Búsqueda M veces (Sesgada)
    auto start = std::chrono::steady_clock::now();
    for (size_t i = 0; i < M; ++i) {
      avl.search(keys[dist(gen)]);
    }
    auto end = std::chrono::steady_clock::now();
    auto elapsed_us =
        std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    std::cout << "[SEARCH] Búsqueda sesgada: " << elapsed_us.count() << " us\n";

  } else {
    // Configurar distribución uniforme
    std::uniform_int_distribution<size_t> dist(0, N - 1);

    // Búsqueda M veces (Equiprobable)
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