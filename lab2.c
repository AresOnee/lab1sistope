/*
 * Laboratorio 2: Monitoreo y Procesamiento de Procesos con Bash y Pipes
 * Curso de Sistemas Operativos - Segundo Semestre 2025
 *
 * Integrantes:
 * - Nombre: [NOMBRE_1], RUT: [RUT_1]
 * - Nombre: [NOMBRE_2], RUT: [RUT_2]
 *
 * Archivo: lab2.c
 * Descripcion: Programa principal que lee un pipeline desde stdin
 *              y lo ejecuta creando procesos conectados por pipes
 */

#include <getopt.h>
#include "funciones.h"

// Entradas: argc - numero de argumentos, argv - argumentos de linea de comando
// Salidas: 0 si exito, 1 si error
// Descripcion: Funcion principal. Procesa opciones con getopt, lee el pipeline
//              desde stdin y llama a ejecutar_pipeline()
int main(int argc, char *argv[]) {
    int opt;

    // Procesar opciones de linea de comando con getopt
    // Solo soportamos -h para mostrar ayuda
    while ((opt = getopt(argc, argv, "h")) != -1) {
        switch (opt) {
            case 'h':
                mostrar_ayuda(argv[0]);
                return 0;
            default:
                fprintf(stderr, "Uso: %s [-h]\n", argv[0]);
                return 1;
        }
    }

    // Leer la linea del pipeline desde stdin
    char linea[MAX_LINEA];
    if (fgets(linea, sizeof(linea), stdin) == NULL) {
        fprintf(stderr, "Error: No se pudo leer la linea de entrada\n");
        return 1;
    }

    // Verificar que la linea no este vacia
    if (strlen(linea) <= 1) {
        fprintf(stderr, "Error: Linea vacia\n");
        return 1;
    }

    // Ejecutar el pipeline
    int resultado = ejecutar_pipeline(linea);

    return (resultado == 0) ? 0 : 1;
}
