// ===========================================================================
// experimentos.hh  —  Batería de experimentos de la Tarea 2 (común a AVL y Splay).
//
// Las funciones de experimentación son PLANTILLAS sobre el tipo de árbol, así que
// AVL y Splay se ejecutan con EXACTAMENTE la misma configuración (mismo c, mismo
// λ, misma semilla y por ende el MISMO dataset y el MISMO orden de inserción para
// cada N). Esto es lo que vuelve comparables ambas estructuras.
//
// Cada estructura escribe sus filas en archivos CSV con sufijo por estructura
// (p. ej. base_scenarios.avl.csv / base_scenarios.splay.csv). graficos.py los
// concatena por la columna `estructura` y dibuja ambas curvas en el mismo gráfico.
//
// Requisito de la interfaz del árbol (lo cumplen tanto AVL como SplayTree):
//   void   insert(uint32_t)
//   bool   search(uint32_t)
//   size_t size() const
//
// El tamaño de los experimentos 7.3 (2^EXP_73) se fija en tiempo de COMPILACIÓN:
//   por defecto 20 (~1 M, corre en segundos para probar);
//   pasar -DEXP_73=25 para la corrida OFICIAL del informe (2^25, hartos minutos).
// ===========================================================================
#ifndef EXPERIMENTOS_HH
#define EXPERIMENTOS_HH

#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <random>
#include <string>
#include <unordered_set>
#include <vector>

// ---------------------------------------------------------------------------
// CONSTANTES DEL EXPERIMENTO (reportar estos valores en el informe).
// Son IDÉNTICAS para AVL y Splay para que la comparación sea justa.
// ---------------------------------------------------------------------------
static const int C = 1;            // c en {1..10};  M = 10*c*N búsquedas
static const double LAMBDA = 0.01; // lambda en [0.001,0.05] de la función sesgada
static const uint64_t SEMILLA = 0xCAFEC0FFEEULL; // semilla base reproducible

static const int EXP_MIN_72 = 10; // escenarios base : 2^10 .. 2^14
static const int EXP_MAX_72 = 14;

#ifndef EXP_73
#define EXP_73 20 // tamaño de los teoremas: 2^EXP_73. Oficial: -DEXP_73=25
#endif

using Reloj = std::chrono::steady_clock;

// Evita que el compilador elimine las búsquedas por "dead code".
static volatile uint64_t g_sink = 0;

// Construye el nombre del CSV con el sufijo de estructura SIEMPRE en minúsculas
// (p. ej. "base_scenarios.avl.csv"), para que graficos.py lo encuentre también
// en sistemas de archivos sensibles a mayúsculas (Linux).
static inline std::string csv_nombre(const char *base, const char *estructura) {
  std::string slug = estructura;
  for (char &ch : slug)
    ch = (char)std::tolower((unsigned char)ch);
  return std::string(base) + "." + slug + ".csv";
}

// Nanosegundos transcurridos desde 't0'.
static inline double ns_desde(Reloj::time_point t0) {
  return std::chrono::duration<double, std::nano>(Reloj::now() - t0).count();
}

// Genera N enteros uint32 uniformes en [0, 2^32-1] de forma reproducible.
// Entrada: N (cantidad) y la semilla. Salida: vector con los N valores. Puede
// haber repetidos; el árbol los ignora (semántica de conjunto). Como la semilla
// fija el resultado, AVL y Splay reciben el MISMO dataset.
static inline std::vector<uint32_t> generar_dataset(size_t N, uint64_t semilla) {
  std::mt19937_64 rng(semilla);
  std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);
  std::vector<uint32_t> datos(N);
  for (size_t i = 0; i < N; ++i)
    datos[i] = dist(rng);
  return datos;
}

// Distribución sesgada P(i) ~ e^{-lambda*i} sobre los índices 0..N-1 (sec. 7.1.2).
// std::discrete_distribution normaliza sola, así que basta con los pesos sin la
// constante (1-e^-lambda)/(1-e^-lambda*N). Entrada: N, lambda. Salida: la
// distribución lista para muestrear índices.
static inline std::discrete_distribution<size_t>
distribucion_sesgada(size_t N, double lambda) {
  std::vector<double> pesos(N);
  for (size_t i = 0; i < N; ++i)
    pesos[i] = std::exp(-lambda * (double)i);
  return std::discrete_distribution<size_t>(pesos.begin(), pesos.end());
}

// ---------------------------------------------------------------------------
//      Escenarios base: para N en {2^10..2^14}, N inserciones + M=10cN búsquedas
//      en 4 configuraciones (a,b,c,d). Cada configuración arranca de un árbol
//      recién construido para medir inserción y búsqueda de forma independiente.
//
//   Arbol       : tipo del árbol (AVL o SplayTree).
//   estructura  : etiqueta para la columna CSV ("AVL" / "Splay").
// ---------------------------------------------------------------------------
template <class Arbol>
void escenarios_base(const char *estructura) {
  std::string fname = csv_nombre("base_scenarios", estructura);
  std::ofstream csv(fname);
  csv << "estructura,escenario,N,n_real,c,lambda,insert_ns,search_ns,M,search_avg_ns\n";

  for (int e = EXP_MIN_72; e <= EXP_MAX_72; ++e) {
    size_t N = (size_t)1 << e;
    uint64_t M = (uint64_t)10 * C * N;

    std::vector<uint32_t> datos = generar_dataset(N, SEMILLA + e);
    std::vector<uint32_t> ordenado = datos; // copia para inserción ordenada (c,d)
    std::sort(ordenado.begin(), ordenado.end());

    // Selectores de índices para las búsquedas.
    std::mt19937_64 rng_unif(SEMILLA ^ (0x1111u + e));
    std::uniform_int_distribution<size_t> unif(0, N - 1);
    auto sesgada = distribucion_sesgada(N, LAMBDA);
    std::mt19937_64 rng_sesg(SEMILLA ^ (0x2222u + e));

    auto correr = [&](const char *nombre, const std::vector<uint32_t> &fuente,
                      bool sesgado) {
      Arbol arbol;
      auto t0 = Reloj::now();
      for (uint32_t v : fuente)
        arbol.insert(v);
      double t_ins = ns_desde(t0);
      size_t n_real = arbol.size();

      uint64_t acc = 0;
      auto t1 = Reloj::now();
      if (sesgado)
        for (uint64_t k = 0; k < M; ++k)
          acc += arbol.search(fuente[sesgada(rng_sesg)]) ? 1u : 0u;
      else
        for (uint64_t k = 0; k < M; ++k)
          acc += arbol.search(fuente[unif(rng_unif)]) ? 1u : 0u;
      double t_bus = ns_desde(t1);
      g_sink += acc;

      csv << estructura << "," << nombre << "," << N << "," << n_real << "," << C
          << "," << LAMBDA << "," << (uint64_t)t_ins << "," << (uint64_t)t_bus
          << "," << M << "," << (t_bus / (double)M) << "\n";
      std::fprintf(stderr,
                   "[7.2 %-5s %s] N=2^%d  ins=%8.1f ms  busq=%8.1f ms  (%6.1f ns/busq)\n",
                   estructura, nombre, e, t_ins / 1e6, t_bus / 1e6, t_bus / (double)M);
    };

    correr("a", datos, false);    // inserción aleatoria, búsqueda uniforme
    correr("b", datos, true);     // inserción aleatoria, búsqueda sesgada
    correr("c", ordenado, false); // inserción ordenada,  búsqueda uniforme
    correr("d", ordenado, true);  // inserción ordenada,  búsqueda sesgada
  }
  std::fprintf(stderr, "-> %s listo\n\n", fname.c_str());
}

// ---------------------------------------------------------------------------
//       Sequential Access Theorem: para m en {N/100..N/10}, secuencia de m
//       búsquedas de claves estrictamente CRECIENTES (paso fijo sobre el arreglo
//       ordenado). El árbol se reconstruye antes de cada m para que cada medición
//       sea independiente. Se espera Splay ~ O(m log(n/m)) vs AVL ~ O(m log n).
// ---------------------------------------------------------------------------
template <class Arbol>
void sequential_access(const char *estructura) {
  size_t N = (size_t)1 << EXP_73;
  std::vector<uint32_t> datos = generar_dataset(N, SEMILLA + 1000);
  std::vector<uint32_t> ordenado = datos;
  std::sort(ordenado.begin(), ordenado.end());

  std::string fname = csv_nombre("sequential_access", estructura);
  std::ofstream csv(fname);
  csv << "estructura,N,m,total_ns,avg_ns\n";

  for (int j = 1; j <= 10; ++j) {
    uint64_t m = (uint64_t)((double)N * j / 100.0); // j/100 * N, hasta N/10
    if (m == 0) m = 1;

    Arbol arbol; // inserción en orden aleatorio (= orden de 'datos')
    for (uint32_t v : datos)
      arbol.insert(v);

    size_t paso = N / m;
    if (paso == 0) paso = 1;

    uint64_t acc = 0;
    size_t idx = 0;
    auto t0 = Reloj::now();
    for (uint64_t k = 0; k < m; ++k) {
      acc += arbol.search(ordenado[idx]) ? 1u : 0u; // claves crecientes
      idx += paso;
      if (idx >= N) idx = N - 1;
    }
    double t = ns_desde(t0);
    g_sink += acc;

    csv << estructura << "," << N << "," << m << "," << (uint64_t)t << ","
        << (t / (double)m) << "\n";
    std::fprintf(stderr,
                 "[7.3a %-5s] m=%10llu  total=%8.1f ms  (%6.1f ns/busq)\n",
                 estructura, (unsigned long long)m, t / 1e6, t / (double)m);
  }
  std::fprintf(stderr, "-> %s listo\n\n", fname.c_str());
}

// ---------------------------------------------------------------------------
//       Working Set Theorem: para W en {10..10^6}, un subconjunto aleatorio de
//       W claves distintas, y M=10cN búsquedas uniformes DENTRO de ese
//       subconjunto. El árbol se reconstruye antes de cada W. Se espera que el
//       costo por búsqueda escale como O(log W) en Splay vs O(log n) en AVL.
// ---------------------------------------------------------------------------
template <class Arbol>
void working_set(const char *estructura) {
  size_t N = (size_t)1 << EXP_73;
  uint64_t M = (uint64_t)10 * C * N;
  std::vector<uint32_t> datos = generar_dataset(N, SEMILLA + 2000);

  std::string fname = csv_nombre("working_set", estructura);
  std::ofstream csv(fname);
  csv << "estructura,N,W,M,total_ns,avg_ns\n";

  std::mt19937_64 rng_sub(SEMILLA ^ 0x5151u);

  for (size_t W = 10; W <= 1000000; W *= 10) {
    if (W > N) break;

    // Subconjunto de W CLAVES distintas tomadas del dataset.
    std::unordered_set<uint32_t> vistos;
    std::vector<uint32_t> sub;
    sub.reserve(W);
    std::uniform_int_distribution<size_t> didx(0, N - 1);
    while (sub.size() < W) {
      uint32_t val = datos[didx(rng_sub)];
      if (vistos.insert(val).second)
        sub.push_back(val);
    }

    Arbol arbol; // mismo orden de inserción (= orden de 'datos')
    for (uint32_t v : datos)
      arbol.insert(v);

    std::mt19937_64 rng_pick(SEMILLA ^ (0x6262u + (uint64_t)W));
    std::uniform_int_distribution<size_t> pick(0, sub.size() - 1);

    uint64_t acc = 0;
    auto t0 = Reloj::now();
    for (uint64_t k = 0; k < M; ++k)
      acc += arbol.search(sub[pick(rng_pick)]) ? 1u : 0u;
    double t = ns_desde(t0);
    g_sink += acc;

    csv << estructura << "," << N << "," << W << "," << M << "," << (uint64_t)t
        << "," << (t / (double)M) << "\n";
    std::fprintf(stderr,
                 "[7.3b %-5s] W=%8zu  total=%8.1f ms  (%6.1f ns/busq)\n",
                 estructura, W, t / 1e6, t / (double)M);
  }
  std::fprintf(stderr, "-> %s listo\n\n", fname.c_str());
}

// ---------------------------------------------------------------------------
// Corre toda la batería para una estructura.
// ---------------------------------------------------------------------------
template <class Arbol>
void correr_todo(const char *estructura) {
  std::fprintf(stderr, "== Experimentos %s ==  c=%d  lambda=%.4f  EXP_73=%d\n\n",
               estructura, C, LAMBDA, EXP_73);
  escenarios_base<Arbol>(estructura);   // 7.2
  sequential_access<Arbol>(estructura); // 7.3 a
  working_set<Arbol>(estructura);       // 7.3 b
  std::fprintf(stderr, "Hecho %s. (sink=%llu)\n\n", estructura,
               (unsigned long long)g_sink);
}

#endif // EXPERIMENTOS_HH
