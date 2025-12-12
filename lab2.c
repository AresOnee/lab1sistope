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

#include "funciones.h"

// Entradas: argc, argv - argumentos de linea de comando
// Salidas: 0 exito, 1 error
// Descripcion: Une todos los argumentos en una linea y ejecuta el pipeline
int main(int argc, char *argv[]) {
    char linea[MAX_LINEA] = "";

    // Si hay argumentos, unirlos todos en una linea
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            strcat(linea, argv[i]);
            if (i < argc - 1) strcat(linea, " ");
        }
    } else {
        // Si no hay argumentos, leer de stdin
        if (!fgets(linea, sizeof(linea), stdin) || strlen(linea) <= 1) {
            fprintf(stderr, "Error: entrada vacia\n");
            fprintf(stderr, "Uso: %s <comando1> | <comando2> | ...\n", argv[0]);
            return 1;
        }
    }

    return ejecutar_pipeline(linea) == 0 ? 0 : 1;
}
