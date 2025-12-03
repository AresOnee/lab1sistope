#!/usr/bin/env bash
# Entrada: TSV del aggregate (con encabezado)
# Salida: Linea 1–3: metadatos (# generated_at=…, # user=…, # host=…) Luego, la tabla entrecomillada y con punto decimal.
# Descripcion: Añade metadatos (# fecha de generacion, # usuario, # host).
# Escribe el resultado en un archivo CSV especificado con -o.

set -e
usage() {
  echo "Uso: $0 -o <archivo_salida>" >&2
}
# Analizar opciones con getopt para -o
PARSED=$(getopt -o o: -- "$@") || { usage; exit 2; }
eval set -- "$PARSED"
OUT=""
while true; do
  case "$1" in
    -o)
      OUT="$2"; shift 2 ;;
    --)
      shift; break ;;
    *)
      usage; exit 2 ;;
  esac
done
if [ -z "$OUT" ]; then
  usage; exit 2
fi
{
  # Metadatos requeridos
  # Usar formato ISO‑8601 con zona (+00:00) para la fecha de generación
  echo "# generated_at: $(date -u +%Y-%m-%dT%H:%M:%S%:z)"
  echo "# user: $(whoami)"
  echo "# host: $(hostname)"
  # Leer el resto de la entrada y convertir cada línea TSV a CSV
  while IFS=$'\t' read -r -a cols; do
    # Construir lista de campos entrecomillados
    out=""
    for i in "${!cols[@]}"; do
      field="${cols[$i]}"
      # Convertir decimal con coma a decimal con punto si es numérico
      if [[ "$field" =~ ^[0-9]+,[0-9]+$ ]]; then
        field="${field/,/.}"
      fi
      # Escapar comillas dobles duplicándolas
      field="${field//\"/\"\"}"
      out+="\"${field}\""
      # Añadir coma salvo para el último campo
      if [ "$i" -lt $((${#cols[@]}-1)) ]; then
        out+=",";
      fi
    done
    echo "$out"
  done
} > "$OUT"
echo "Reporte escrito en: $OUT"

