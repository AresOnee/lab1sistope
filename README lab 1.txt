Laboratorio 1: Monitoreo y Procesamiento de Procesos del Sistema con Bash y Pipes
Curso de Sistemas Operativos
Segundo semestre 2025

Estudiante 1: Sofia Vergara
Estudiante 2: Vladimir Vidal

Implementacion de un  pipeline de 6 etapas que captura procesos del sistema, valida/filtra/anonimiza sus campos, agrega métricas y genera un reporte CSV final.
Descomprimir el archivo zip y dejar los archivos en la misma carpeta luego abrir una terminal en la direccion que
se dejaron los archivos
Luego ejecutar algunos de los siguientes comandos para probar las funcionalidades
Recomendado el uso de export LC_ALL=C para fijar locale para separar decimales con punto.

Genera muestras durante 3 s (1 muestra/seg), procesa todo el pipeline y escribe reporte.csv
export LC_ALL=C
bash generator.sh -i 1 -t 3 \
| bash preprocess.sh \
| bash filter.sh -c 0 -m 0 \
| bash transform.sh --anon-uid \
| bash aggregate.sh \
| bash report.sh -o reporte.csv


Ejecución con logs de cada etapa


export LC_ALL=C
bash generator.sh -i 1 -t 3 -l logs/generator.txt \
| bash preprocess.sh -l logs/preprocess.txt \
| bash filter.sh -c 0.1 -m 0.1 -l logs/filter.txt \
| bash transform.sh --anon-uid -l logs/transform.txt \
| bash aggregate.sh -l logs/aggregate.txt \
| bash report.sh -o reporte.csv

Sin filtros (umbrales abiertos)

export LC_ALL=C
bash generator.sh -i 1 -t 3 \
| bash preprocess.sh \
| bash filter.sh -c 0 -m 0 \
| bash transform.sh --anon-uid \
| bash aggregate.sh \
| bash report.sh -o reporte_all.csv


Filtrar por nombre con regex (solo bash)

bash generator.sh -i 1 -t 3 \
| bash preprocess.sh \
| bash filter.sh -c 0 -m 0 -r '^bash$' \
| bash transform.sh --anon-uid \
| bash aggregate.sh \
| bash report.sh -o reporte_bash.csv


Con logs y umbrales combinados

bash generator.sh -i 1 -t 3 -l logs/generator.txt \
| bash preprocess.sh -l logs/preprocess.txt \
| bash filter.sh -c 0.5 -m 0.5 -r 'gnome' -l logs/filter.txt \
| bash transform.sh --anon-uid -l logs/transform.txt \
| bash aggregate.sh -l logs/aggregate.txt \
| bash report.sh -o reporte.csv


Filtrar por múltiples nombres

bash generator.sh -i 1 -t 3 \
| bash preprocess.sh \
| bash filter.sh -c 0 -m 0 -r 'firefox|nautilus' \
| bash transform.sh --anon-uid \
| bash aggregate.sh \
| bash report.sh -o reporte_fx_nau.csv


Filtrar por CPU ≥ 10% y MEM ≥ 0

bash generator.sh -i 1 -t 3 \
| bash preprocess.sh \
| bash filter.sh -c 10 -m 0 \
| bash transform.sh --anon-uid \
| bash aggregate.sh \
| bash report.sh -o reporte_hotcpu.csv

