/*
 * Laboratorio 2: Monitoreo y Procesamiento de Procesos con Bash y Pipes
 * Curso de Sistemas Operativos - Segundo Semestre 2025
 *
 * Integrantes:
 * - Nombre: Sofia Vergara, RUT 21.082.148-1
 * - Nombre: Vladimir Vidal, RUT 18.031.181-5
 *
 * Archivo: lab2.c
 * Descripcion: Programa principal - Híbrido (Soporta argumentos y stdin)
 */

#include "funciones.h"

// Entradas: argc, argv - argumentos de linea de comando
// Salidas: 0 exito, 1 error
// Descripcion: Construye el pipeline concatenando argumentos o leyendo de stdin
int main(int argc, char *argv[]) {
    char linea[MAX_LINEA];
    linea[0] = '\0'; // Asegurar que el string esté vacío al inicio

    // CASO 1: Ejecución con argumentos
    // Ejemplo: ./lab2 generator.sh -i 1 | ./preprocess.sh
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            // Verificar que no desbordemos el buffer
            if (strlen(linea) + strlen(argv[i]) + 2 > MAX_LINEA) {
                fprintf(stderr, "Error: La línea de comandos excede el tamaño máximo permitido.\n");
                return 1;
            }

            // Concatenar el argumento actual
            strcat(linea, argv[i]);

            // Agregar un espacio después, excepto si es el último argumento
            if (i < argc - 1) {
                strcat(linea, " ");
            }
        }
    } 
    // CASO 2: Ejecución sin argumentos (leer desde stdin)
    // Ejemplo: echo "..." | ./lab2
    else {
        if (fgets(linea, MAX_LINEA, stdin) == NULL) {
            return 0; // No se recibió entrada
        }
        // Eliminar el salto de línea que agrega fgets al final
        size_t len = strlen(linea);
        if (len > 0 && linea[len - 1] == '\n') {
            linea[len - 1] = '\0';
        }
    }

    // Una vez construida la variable 'linea' (ya sea desde argv o stdin),
    // llamamos a la función que maneja los procesos definida en funciones.c
    if (ejecutar_pipeline(linea) == -1) {
        return 1;
    }

    return 0;
}
