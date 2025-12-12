#!/bin/bash
# Script de prueba para lab2

echo "========================================"
echo "   PRUEBAS DEL LABORATORIO 2"
echo "========================================"

echo ""
echo "1. Dando permisos a los scripts..."
chmod +x *.sh

echo ""
echo "2. Compilando..."
make clean && make
if [ $? -ne 0 ]; then
    echo "ERROR: Falló la compilación"
    exit 1
fi
echo "Compilación exitosa!"

echo ""
echo "========================================"
echo "PRUEBA 1: Pipeline completo (10 segundos)"
echo "========================================"
./lab2 ./generator.sh -i 1 -t 10 | ./preprocess.sh | ./filter.sh -c 0 -m 0 | ./transform.sh --anon-uid | ./aggregate.sh | ./report.sh -o reporte.csv

echo ""
echo "Contenido de reporte.csv:"
echo "----------------------------------------"
cat reporte.csv

echo ""
echo "========================================"
echo "PRUEBA 2: Pipeline parcial (5 segundos)"
echo "========================================"
./lab2 ./generator.sh -i 1 -t 5 | ./preprocess.sh | head -10

echo ""
echo "========================================"
echo "PRUEBA 3: Solo generator (3 segundos)"
echo "========================================"
./lab2 ./generator.sh -i 1 -t 3 | head -5

echo ""
echo "========================================"
echo "PRUEBA 4: Con filtros específicos"
echo "========================================"
./lab2 ./generator.sh -i 1 -t 5 | ./preprocess.sh | ./filter.sh -c 5 -m 0 | ./aggregate.sh

echo ""
echo "========================================"
echo "   TODAS LAS PRUEBAS COMPLETADAS"
echo "========================================"
