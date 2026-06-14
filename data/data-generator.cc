#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

/*
===============================================================================
FUNCIÓN PRINCIPAL
===============================================================================
*/

/**
 * @function main
 * @brief Función principal que genera un dataset de números aleatorios.
 * * Descripción:
 * Genera una cantidad de números enteros sin signo aleatorios equivalente a 
 * 2 elevado a la potencia indicada por parámetro, y los escribe directamente
 * en un archivo binario dentro del directorio 'data/'.
 * * Parámetros de entrada:
 * argc (int): Cantidad de argumentos de línea de comandos (debe ser 2).
 * argv (char**): Array de argumentos:
 * argv[0] = nombre del programa.
 * argv[1] = exponente 'a' para generar N = 2^a números.
 * * Salida:
 * Crea un archivo binario en la ruta "data/datos_<a>.data".
 * Imprime en stdout el estado de la generación y posibles errores.
 * * Retorna:
 * int: 0 si se ejecutó exitosamente, 1 si hubo errores de parámetros o I/O.
 */
int main(int argc, char* argv[]) {
  /*
  ===========================================================================
  VALIDACIÓN DE PARÁMETROS
  ===========================================================================
  */
  if (argc != 2) {
    std::cerr << "Uso: " << argv[0] << " <int>\n";
    return 1;
  }

  // Convertir el argumento a entero
  int a;
  try {
    a = std::stoi(argv[1]);
  } catch (...) {
    std::cerr << "[ERROR]: el argumento debe ser un entero.\n";
    return 1;
  }

  /*
  ===========================================================================
  GENERACIÓN DE DATOS ALEATORIOS
  ===========================================================================
  */
  // Cantidad de números a generar: 2^a
  std::size_t n = static_cast<std::size_t>(1ULL << a);
  std::vector<std::uint32_t> numbers(n);

  // Generador aleatorio
  std::random_device rd;
  std::mt19937 gen(rd());

  std::uniform_int_distribution<std::uint32_t> dist(
    0,
    UINT32_MAX
  );
  for (auto& value : numbers) {
    value = dist(gen);
  }

  /*
  ===========================================================================
  ESCRITURA A DISCO (BINARIO)
  ===========================================================================
  */
  // Escribir al archivo
  std::string filename = "data/datos_" + std::to_string(a) + ".data";
  std::ofstream out(filename, std::ios::binary);
  if (!out) {
    std::cerr
      << "[ERROR]: no se pudo abrir el archivo "
      << filename << ".\n";
    return 1;
  }

  // Escribir directamente la memoria del vector
  out.write(
    reinterpret_cast<const char*>(numbers.data()),
    static_cast<std::streamsize>(
      numbers.size() * sizeof(std::uint32_t)
    )
  );

  if (!out) {
    std::cerr << "[ERROR] durante la escritura del archivo.\n";
    return 1;
  }
  out.close();

  std::cout
    << "[SUCCESS] "
    << "Se generaron " << n
    << " numeros aleatorios y se escribieron en "
    << filename << ".\n";

  return 0;
}