#!/usr/bin/env bash
set -e

# Entrada: TSV del preprocess por stdin (ts pid uid comm pcpu pmem).
# Salida: el mismo TSV pero solo con filas que cumplen los umbrales y (si se pasó) la regex.
# Descripcion: Filtra la salida del preprocesador según umbrales de CPU y memoria y,
# opcionalmente, una expresión regular sobre el nombre del comando (comm).
# La entrada puede contener cinco columnas (sin timestamp) o seis columnas
# (con timestamp ISO‑8601).  El script detecta la presencia de la columna
# extra y ajusta las posiciones de los campos automáticamente.
#
# Opciones:
#   -c <cpu_min>    Umbral mínimo de %CPU (número no negativo).
#   -m <mem_min>    Umbral mínimo de %MEM (número no negativo).
#   -r <regex>      Expresión regular que debe cumplir el nombre del comando.
#   -l <archivo>    Guarda una copia de la salida en el archivo indicado.

usage() {
  echo "Uso: $0 -c <cpu_min> -m <mem_min> [-r <regex>] [-l <archivo_log>]" >&2
}

# Analizar las opciones con getopt (permite orden arbitrario)
PARSED=$(getopt -o c:m:r:l: -- "$@") || { usage; exit 2; }
eval set -- "$PARSED"

CPU_MIN=""
MEM_MIN=""
REGEX=""
LOG=""

while true; do
  case "$1" in
    -c)
      CPU_MIN="$2"; shift 2 ;;
    -m)
      MEM_MIN="$2"; shift 2 ;;
    -r)
      REGEX="$2"; shift 2 ;;
    -l)
      if [ -z "$2" ]; then usage; exit 2; fi
      LOG="$2"; shift 2 ;;
    --)
      shift; break ;;
    *)
      usage; exit 2 ;;
  esac
done

# Comprobar que c y m estén definidos
if [ -z "$CPU_MIN" ] || [ -z "$MEM_MIN" ]; then
  usage; exit 2
fi

# Validar que CPU_MIN y MEM_MIN sean números no negativos (enteros o decimales)
if ! [[ "$CPU_MIN" =~ ^[0-9]+(\.[0-9]+)?$ ]]; then
  echo "El valor de -c debe ser un número no negativo" >&2; exit 2
fi
if ! [[ "$MEM_MIN" =~ ^[0-9]+(\.[0-9]+)?$ ]]; then
  echo "El valor de -m debe ser un número no negativo" >&2; exit 2
fi

# Si el argumento de -r está vacío, se ignora la opción
if [ -z "$REGEX" ]; then
  REGEX=""
fi

# Función que aplica el filtro utilizando awk
process_input() {
  awk -v cmin="$CPU_MIN" -v mmin="$MEM_MIN" -v regex="$REGEX" '
  BEGIN{FS="\t"; OFS="\t"}
  {
    # Determinar si hay timestamp: si NF >= 6 asumimos ts, pid, uid, ...
    off = (NF >= 6 ? 1 : 0);
    # Índices de campos
    cpu = $(NF-1);    # pcpu es el penúltimo campo
    mem = $NF;        # pmem es el último campo
    # Construir el nombre del comando (puede contener espacios)
    if (off == 1) {
      start = 4;
    } else {
      start = 3;
    }
    # Unir desde start hasta NF-2
    comm = $(start);
    for(i = start + 1; i <= NF - 2; i++) comm = comm " " $(i);
    # Normalizar comas decimales
    gsub(/,/, ".", cpu);
    gsub(/,/, ".", mem);
    # Comparar umbrales y expresión regular
    if (cpu+0 >= cmin && mem+0 >= mmin) {
      if (regex != "") {
        if (comm ~ regex) print $0;
      } else {
        print $0;
      }
    }
  }
  '
}

# Ejecutar la filtración y, si corresponde, guardar un log
if [ -n "$LOG" ]; then
  mkdir -p "$(dirname "$LOG")"
  process_input | tee "$LOG"
else
  process_input
fi
