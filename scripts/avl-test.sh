if [ -f "./avl-test.out" ]; then
  ./avl-test.out ./data/datos_10.data
  ./avl-test.out ./data/datos_11.data
  ./avl-test.out ./data/datos_12.data
  ./avl-test.out ./data/datos_13.data
  ./avl-test.out ./data/datos_14.data
else
  echo "[ERROR] No existe el archivo ./avl-test.out. Tienes que compilarlo"
fi
