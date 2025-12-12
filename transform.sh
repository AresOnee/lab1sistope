#!/usr/bin/env bash
set -e

# Integrantes:
# - Nombre: Sofia Vergara, RUT 21.082.148-1
# - Nombre: Vladimir Vidal, RUT 18.031.181-5
# Entrada: TSV del filtro (ts pid uid comm pcpu pmem).
# Salida: TSV con misma cantidad de columnas, pero uid anonimizad@ (hash/código).
# Descripcion:  Anonimiza el UID de la salida del filtro cuando se indica la opción
# --anon-uid.  La entrada puede tener 5 o 6 columnas (con o sin
# timestamp ISO‑8601).  También acepta -l para registrar la salida.
#
# Opciones:
#   --anon-uid    Activa la anonimización del UID.
#   -l <archivo>  Guarda una copia de la salida en el archivo indicado.

usage() {
  echo "Uso: $0 [--anon-uid] [-l <archivo_log>]" >&2
}

# Parsear opciones usando getopt
PARSED=$(getopt -o l: -l anon-uid -- "$@") || { usage; exit 2; }
eval set -- "$PARSED"
ANON=0
LOG=""
while true; do
  case "$1" in
    --anon-uid)
      ANON=1; shift ;;
    -l)
      if [ -z "$2" ]; then usage; exit 2; fi
      LOG="$2"; shift 2 ;;
    --)
      shift; break ;;
    *)
      usage; exit 2 ;;
  esac
done

# Función de anonimización del UID.  Lee de stdin y escribe a stdout.
process() {
  if [ "$ANON" -eq 1 ]; then
    # Leer línea a línea preservando tabuladores
    while IFS= read -r line; do
      # Dividir por tabulador en un array
      IFS=$'\t' read -r -a fields <<< "$line"
      n=${#fields[@]}
      if [ "$n" -ge 6 ]; then
        # Con timestamp: campos[0]=ts, [1]=pid, [2]=uid
        uid_index=2
      else
        # Sin timestamp: campos[0]=pid, [1]=uid
        uid_index=1
      fi
      uid="${fields[$uid_index]}"
      # Calcular hash MD5 y quedarse con los primeros 12 caracteres hexadecimales
      hashed=$(echo -n "$uid" | md5sum | cut -d ' ' -f1 | cut -c1-12)
      fields[$uid_index]="$hashed"
      # Reconstruir la línea con tabuladores
      for idx in "${!fields[@]}"; do
        if [ "$idx" -eq 0 ]; then
          printf '%s' "${fields[$idx]}"
        else
          printf '\t%s' "${fields[$idx]}"
        fi
      done
      printf '\n'
    done
  else
    # Si no se especifica anonimización, simplemente emitir la entrada
    cat
  fi
}

# Ejecutar la transformación y registrar si corresponde
if [ -n "$LOG" ]; then
  mkdir -p "$(dirname "$LOG")"
  process | tee "$LOG"
else
  process
fi
