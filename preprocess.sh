#!/usr/bin/env bash
set -e

# Integrantes:
# - Nombre: Sofia Vergara, RUT 21.082.148-1
# - Nombre: Vladimir Vidal, RUT 18.031.181-5

#Entrada: la salida del generator.sh por stdin.
#Salida: TSV con la misma estructura (ts pid uid comm pcpu pmem), validada.
#Descripcion Esta etapa del pipeline valida y normaliza la salida del generador.  Puede
# aceptar líneas con o sin un timestamp ISO‑8601 como primera columna.  El
# formato de entrada esperado es:
#   ts_iso  pid  uid  comm  pcpu  pmem
# o, si no hay timestamp, simplemente:
#   pid  uid  comm  pcpu  pmem
#
# Opciones:
#   --iso8601      No hace nada en esta versión (preservado por compatibilidad).
#   -l <archivo>   Guarda una copia de la salida en el archivo indicado.

usage() {
  echo "Uso: $0 [-l <archivo_log>] [--iso8601]" >&2
}

# Analizar opciones con getopt: permitir --iso8601 (para compatibilidad) y -l con argumento
PARSED=$(getopt -o l: -l iso8601 -- "$@") || { usage; exit 2; }
eval set -- "$PARSED"
LOG=""
while true; do
  case "$1" in
    --iso8601)
      # --iso8601 se acepta por compatibilidad pero no tiene efecto
      shift ;;
    -l)
      if [ -z "$2" ]; then usage; exit 2; fi
      LOG="$2"; shift 2 ;;
    --)
      shift; break ;;
    *)
      usage; exit 2 ;;
  esac
done

# Función de procesamiento: valida y emite líneas en TSV.  Si hay un
# timestamp al inicio, lo conserva; de lo contrario, emite cinco columnas.
process_input() {
  # Este awk analiza cada línea considerando que las dos últimas
  # columnas son %CPU y %MEM.  Detecta la posición del PID y UID
  # buscando dos enteros consecutivos; todo lo anterior se considera
  # timestamp (puede incluir espacios) y lo intermedio es el comando.
  awk 'BEGIN{OFS="\t"}
  {
    # Eliminar espacios iniciales y saltar líneas vacías
    sub(/^[[:space:]]+/, "", $0);
    if ($0 == "") next;
    n = NF;
    # Las dos últimas columnas son pcpu y pmem
    pcpu = $(n - 1);
    pmem = $n;
    # Normalizar decimales con coma a punto
    gsub(/,/, ".", pcpu);
    gsub(/,/, ".", pmem);
    # Validar que pcpu y pmem sean números
    if (pcpu !~ /^[0-9]+(\.[0-9]+)?$/) next;
    if (pmem !~ /^[0-9]+(\.[0-9]+)?$/) next;
    # Ahora buscar pid y uid: dos enteros consecutivos antes de pcpu/pmem
    pid=""; uid=""; ts=""; comm="";
    found=0;
    for (k = 1; k <= n - 2; k++) {
      if (found == 0 && $k ~ /^[0-9]+$/ && (k + 1 <= n - 2) && $(k + 1) ~ /^[0-9]+$/) {
        pid = $k;
        uid = $(k + 1);
        # Construir timestamp a partir de todos los campos antes del pid
        for (i = 1; i < k; i++) {
          if (i > 1) ts = ts " ";
          ts = ts $i;
        }
        # Construir comando desde k+2 hasta n-2
        start = k + 2;
        if (start <= n - 2) {
          comm = $(start);
          for (i = start + 1; i <= n - 2; i++) comm = comm " " $(i);
        }
        found=1;
        break;
      }
    }
    # Si no encontró pid/uid, descartar
    if (found == 0) next;
    # Validar pid y uid numéricos
    if (pid !~ /^[0-9]+$/) next;
    if (uid !~ /^[0-9]+$/) next;
    # Emitir la línea.  Si no hay timestamp, ts estará vacío.
    if (ts != "") {
      print ts, pid, uid, comm, pcpu, pmem;
    } else {
      print pid, uid, comm, pcpu, pmem;
    }
  }'
}

# Ejecutar el procesamiento y, si se especifica un log, guardar una copia
if [ -n "$LOG" ]; then
  mkdir -p "$(dirname "$LOG")"
  process_input | tee "$LOG"
else
  process_input
fi
