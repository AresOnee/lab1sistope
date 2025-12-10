#!/usr/bin/env bash

# Entradas: TSV por stdin con encabezado: (ts|ts_iso)  pid  uid  comm  pcpu  pmem
# Salidas : TSV por stdout con encabezado: command  nproc  cpu_avg  cpu_max  mem_avg  mem_max
# Flags   : -l <archivo_log>
# Descripcion: grupa por comando. Calcula: numero de procesos, CPU promedio, CPU maxima, MEM promedio y MEM maxima

usage() {
  echo "Uso: $0 [-l <archivo_log>]" >&2
}

set -e
LOG=""

# Analizar opciones con getopt (solo -l admite argumento)
PARSED=$(getopt -o l: -- "$@") || { usage; exit 2; }
eval set -- "$PARSED"
while true; do
  case "$1" in
    -l)
      # Comprobar que -l tenga argumento
      if [ -z "$2" ]; then usage; exit 2; fi
      LOG="$2"
      shift 2
      ;;
    --)
      shift
      break
      ;;
    *)
      usage; exit 2
      ;;
  esac
done

awk -F'\t' '
BEGIN{OFS="\t"}
{
   # Detectar si hay columna de timestamp ISO al inicio (6 columnas o más).
   off = (NF >= 6 ? 1 : 0)
   # El nombre del comando (key) está en la posición 3+off
   key = $(3+off)
   # %CPU y %MEM son los dos últimos campos
   cpu = $(NF-1)
   mem = $NF
   # Acumular estadísticas por comando
   c[key]++
   scpu[key]+=cpu
   smem[key]+=mem
   if(cpu>mxc[key]) mxc[key]=cpu
   if(mem>mxm[key]) mxm[key]=mem
}
END{
  # Encabezado
  print "command","nproc","cpu_avg","cpu_max","mem_avg","mem_max"
  for(k in c) {
    # Calcular promedios
    cpu_avg = scpu[k] / c[k]
    mem_avg = smem[k] / c[k]
    # Formatear con un decimal y reemplazar el punto por coma para locales que usan coma decimal
    cpu_avg_str = sprintf("%.1f", cpu_avg); gsub(/\./, ",", cpu_avg_str)
    cpu_max_str = sprintf("%.1f", mxc[k]+0); gsub(/\./, ",", cpu_max_str)
    mem_avg_str = sprintf("%.1f", mem_avg); gsub(/\./, ",", mem_avg_str)
    mem_max_str = sprintf("%.1f", mxm[k]+0); gsub(/\./, ",", mem_max_str)
    printf "%s\t%d\t%s\t%s\t%s\t%s\n", k, c[k], cpu_avg_str, cpu_max_str, mem_avg_str, mem_max_str
  }
}' | {
  if [ -n "$LOG" ]; then
    mkdir -p "$(dirname "$LOG")"; tee "$LOG"
  else
    cat
  fi
}

