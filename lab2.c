/*
 * Laboratorio 2: Monitoreo y Procesamiento de Procesos con Bash y Pipes
 * Curso de Sistemas Operativos - Segundo Semestre 2025
 *
 * Integrantes:
 * - Nombre: [NOMBRE_1], RUT: [RUT_1]
 * - Nombre: [NOMBRE_2], RUT: [RUT_2]
 *
 * Archivo: lab2.c
 * Descripcion: Programa principal - lee pipeline desde stdin y lo ejecuta
 */

#include <getopt.h>
#include "funciones.h"

// Entradas: argc, argv - argumentos de linea de comando
// Salidas: 0 exito, 1 error
// Descripcion: Procesa flags con getopt, lee pipeline de stdin, ejecuta
int main(int argc, char *argv[]) {
    int opt;

    // Procesar opciones con getopt
    while ((opt = getopt(argc, argv, "h")) != -1) {
        if (opt == 'h') {
            printf("Uso: %s [-h]\nLee pipeline desde stdin y lo ejecuta.\n", argv[0]);
            return 0;
        }
        fprintf(stderr, "Uso: %s [-h]\n", argv[0]);
        return 1;
    }

    // Leer pipeline desde stdin
    char linea[MAX_LINEA];
    if (!fgets(linea, sizeof(linea), stdin) || strlen(linea) <= 1) {
        fprintf(stderr, "Error: entrada vacia\n");
        return 1;
    }

    return ejecutar_pipeline(linea) == 0 ? 0 : 1;
}
