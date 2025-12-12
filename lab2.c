/*
 * Laboratorio 2: Monitoreo y Procesamiento de Procesos con Bash y Pipes
 * Curso de Sistemas Operativos - Segundo Semestre 2025
 *
 * Integrantes:
 * - Nombre: [NOMBRE_1], RUT: [RUT_1]
 * - Nombre: [NOMBRE_2], RUT: [RUT_2]
 *
 * Archivo: lab2.c
 * Descripcion: Programa principal - ejecuta el script recibido por argumentos
 */

#include "funciones.h"

// Entradas: argc, argv - argumentos de linea de comando
// Salidas: 0 exito, 1 error
// Descripcion: Ejecuta el script pasado como argumento usando fork y exec
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <script.sh> [args...]\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // Hijo: ejecutar el script con sus argumentos
        // argv+1 apunta a ["generator.sh", "-i", "1", "-t", "10", NULL]
        execvp(argv[1], argv + 1);
        perror(argv[1]);
        _exit(1);
    }

    // Padre: esperar al hijo
    int status;
    waitpid(pid, &status, 0);

    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}
