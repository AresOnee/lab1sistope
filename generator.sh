#!/usr/bin/env bash

#------ CONDICIONALES ------#
# eq: igual.
# ne: no igual.
# lt: menor que.
# le: menor o igual que.
# gt: mayor que.
# ge: mayor o igual que.
# =~: coincide con el patrón [].
# =: cadena igual.
# !=: cadena no igual.
# -z: cadena vacia.
# -n: cadena no vacia.

#------ BASH ------#
# SECONDS: Variable especial de bash que cuenta los segundos desde que se inicio el script.
# sleep X: Espera X segundos para continuar con la siguiente instrucción.
# exit X: Termina el script con el codigo X. 0 = exitoso, 1 = error.
# ^: Inicio de la cadena.
# $: Final de la cadena. 
# [0-9]+: Al menos un digito.

#------ SCRIPT ------#

# Entradas: 
#   -i X : intervalo de muestreo en segundos.  Cada X segundos se captura la salida de ps.
#   -t Y : duración total en segundos.  El generador terminará después de Y segundos.
#   -l <archivo> (opcional): ruta donde guardar una copia de la salida producida.
# Salidas: Por cada muestreo imprime una línea con los campos: 
#   ts_iso  pid  uid  comm  pcpu  pmem
# donde ts_iso es un timestamp en formato ISO‑8601 con zona (+00:00) generado en UTC.
# Los procesos se ordenan por %CPU de manera descendente.
# Descripcion: lee ps periódicamente y emite lecturas de procesos.

# Inicializar variables para las opciones
i=""
t=""
LOG=""

# Mostrar ayuda en caso de error
usage() {
  echo "Uso: $0 -i <intervalo_segundos> -t <duración_segundos> [-l <archivo_log>]" >&2
}

# Analizar opciones con getopt para permitir cualquier orden y detectar -l con argumento
PARSED=$(getopt -o i:t:l: -- "$@") || { usage; exit 2; }
eval set -- "$PARSED"
while true; do
  case "$1" in
    -i)
      i="$2"; shift 2 ;;
    -t)
      t="$2"; shift 2 ;;
    -l)
      if [ -z "$2" ]; then usage; exit 2; fi
      LOG="$2"; shift 2 ;;
    --)
      shift; break ;;
    *)
      usage; exit 2 ;;
  esac
done

# Comprobar que se proporcionaron -i y -t
if [ -z "$i" ] || [ -z "$t" ]; then
  usage; exit 2
fi

# Validar que i y t son números positivos (enteros o decimales)
if ! [[ "$i" =~ ^[0-9]+(\.[0-9]+)?$ ]] || ! [[ "$t" =~ ^[0-9]+(\.[0-9]+)?$ ]]; then
  echo "Los argumentos de -i y -t deben ser números positivos (enteros o decimales)." >&2
  exit 1
fi

# Verificar que i y t sean mayores que cero (soportar decimales con awk)
if ! awk -v interval="$i" -v total="$t" 'BEGIN{exit (interval>0 && total>0 ? 0 : 1)}'; then
  echo "Los argumentos de -i y -t deben ser mayores que 0." >&2
  exit 1
fi

# Definir una función para imprimir una muestra de procesos con timestamp ISO‑8601.
generate() {
  local ts
  ts=$(date -u --iso-8601=seconds)
  # Para evitar que el nombre del comando se divida cuando contiene espacios,
  # colocamos el campo comm al final. "ps -eo pid,uid,pcpu,pmem,comm" imprime
  # pid, uid, %CPU, %MEM y el nombre del comando (posiblemente con espacios).
  ps -eo pid,uid,pcpu,pmem,comm --no-headers --sort=-%cpu |
    awk -v ts="$ts" 'BEGIN{OFS="\t"} {
      pid=$1; uid=$2; pcpu=$3; pmem=$4;
      # Construir el nombre del comando desde el quinto campo hasta el final
      comm=$5;
      for(i=6;i<=NF;i++) comm=comm " " $(i);
      print ts, pid, uid, comm, pcpu, pmem;
    }'
}

# Bucle de captura: repetimos hasta que hayan transcurrido t segundos.
# Creamos el directorio del log una sola vez, no en cada iteración
if [ -n "$LOG" ]; then
  mkdir -p "$(dirname "$LOG")"
fi

SECONDS=0
while awk -v s="$SECONDS" -v total="$t" 'BEGIN{exit (s < total ? 0 : 1)}'; do
  # Generamos una muestra y la enviamos a stdout. Si hay archivo de log,
  # guardamos también una copia acumulativa usando tee.
  if [ -n "$LOG" ]; then
    generate | tee -a "$LOG"
  else
    generate
  fi
  sleep "$i"
done
