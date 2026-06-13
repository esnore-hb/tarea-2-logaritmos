#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

int main(int argc, char* argv[]) {
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