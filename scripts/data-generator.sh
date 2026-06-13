if [ -f "./data/data-generator.out" ]; then
  ./data/data-generator.out 10
  ./data/data-generator.out 11
  ./data/data-generator.out 12
  ./data/data-generator.out 13
  ./data/data-generator.out 14
else
  echo "[ERROR] No se ha generado el ejecutable data-generator.out"
fi