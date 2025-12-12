/*
 * Laboratorio 2: Monitoreo y Procesamiento de Procesos con Bash y Pipes
 * Curso de Sistemas Operativos - Segundo Semestre 2025
 *
 * Integrantes:
 * - Nombre: [NOMBRE_1], RUT: [RUT_1]
 * - Nombre: [NOMBRE_2], RUT: [RUT_2]
 *
 * Archivo: lab2.c
 * Descripcion: Programa principal - lee pipeline y lo ejecuta
 */

#include <getopt.h>
#include "funciones.h"

// Entradas: argc, argv - argumentos de linea de comando
// Salidas: 0 exito, 1 error
// Descripcion: Procesa flags con getopt, construye pipeline desde argv, ejecuta
int main(int argc, char *argv[]) {
    int opt;

    // Procesar opciones con getopt
    while ((opt = getopt(argc, argv, "h")) != -1) {
        if (opt == 'h') {
            printf("Uso: %s <comando1> | <comando2> | ...\n", argv[0]);
            return 0;
        }
        fprintf(stderr, "Uso: %s <comando1> | <comando2> | ...\n", argv[0]);
        return 1;
    }

    char linea[MAX_LINEA] = "";

    // Si hay argumentos, unirlos en una linea
    if (optind < argc) {
        for (int i = optind; i < argc; i++) {
            strcat(linea, argv[i]);
            if (i < argc - 1) strcat(linea, " ");
        }
    } else {
        // Si no hay argumentos, leer de stdin
        if (!fgets(linea, sizeof(linea), stdin) || strlen(linea) <= 1) {
            fprintf(stderr, "Error: entrada vacia\n");
            return 1;
        }
    }

    return ejecutar_pipeline(linea) == 0 ? 0 : 1;
}
