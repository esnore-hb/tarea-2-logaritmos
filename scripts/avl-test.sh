#!/usr/bin/env bash

EXEC="./avl-test.out"
DATA_DIR="./data"
RESULTS_DIR="./results"

# N a probar
DATASETS=(10 11 12 13 14)

# Flags a probar
FLAGS=("" "-b" "-o" "-bo")

# Verificar ejecutable
if [[ ! -f "$EXEC" ]]; then
    echo "[ERROR] No existe '$EXEC'. Debes compilar primero."
    exit 1
fi

if [[ ! -x "$EXEC" ]]; then
    echo "[ERROR] '$EXEC' no tiene permisos de ejecución."
    exit 1
fi

# Crear directorio de resultados si no existe
mkdir -p "$RESULTS_DIR" || {
    echo "[ERROR] No se pudo crear '$RESULTS_DIR'."
    exit 1
}

for N in "${DATASETS[@]}"; do
    DATA_FILE="${DATA_DIR}/datos_${N}.data"
    if [[ ! -f "$DATA_FILE" ]]; then
        echo "[WARNING] No existe '$DATA_FILE'. Se omite N=$N."
        continue
    fi

    echo "[INFO] Ejecutando benchmarks para N=$N"
    for FLAG in "${FLAGS[@]}"; do

        # Nombre del sufijo del archivo
        if [[ -z "$FLAG" ]]; then
            SUFFIX=""
        else
            SUFFIX="${FLAG#-}"     # elimina el '-'
            SUFFIX="-$SUFFIX"
        fi

        OUTPUT_FILE="${RESULTS_DIR}/avl-test-${N}${SUFFIX}.results"
        echo -n "  -> ${EXEC} ${N} ${FLAG} ... "
        if "$EXEC" "$N" ${FLAG:+$FLAG} > "$OUTPUT_FILE"; then
            echo "OK"
        else
            echo "FAILED"
            rm -f "$OUTPUT_FILE"
            exit 1
        fi
    done
    echo "[SUCCESS] Finalizado N=$N"
done
echo "[SUCCESS] Todos los benchmarks completados."
