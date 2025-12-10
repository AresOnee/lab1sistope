/*
 * Laboratorio 2: Monitoreo y Procesamiento de Procesos con Bash y Pipes
 * Curso de Sistemas Operativos - Segundo Semestre 2025
 *
 * Integrantes:
 * - Nombre: [NOMBRE_1], RUT: [RUT_1]
 * - Nombre: [NOMBRE_2], RUT: [RUT_2]
 *
 * Archivo: funciones.c
 * Descripcion: Implementacion de funciones para el pipeline de procesos
 */

#define _POSIX_C_SOURCE 200809L
#include "funciones.h"

// Entradas: nombre_programa - nombre del ejecutable
// Salidas: ninguna
// Descripcion: Muestra el mensaje de ayuda del programa
void mostrar_ayuda(const char *prog) {
    printf("Uso: %s [-h]\n\n", prog);
    printf("Lee un pipeline desde stdin y lo ejecuta.\n");
    printf("Ejemplo: echo \"./script1.sh | ./script2.sh\" | %s\n", prog);
}

// Entradas: linea - pipeline completo (ej: "cmd1 -a | cmd2 -b")
// Salidas: 0 exito, -1 error
// Descripcion: Parsea, crea procesos hijos con fork(), los conecta
//              con pipe() y dup2(), y ejecuta cada comando con execvp()
int ejecutar_pipeline(char *linea) {
    char *cmds[MAX_COMANDOS], *saveptr1, *saveptr2;
    int n = 0, fd_in = STDIN_FILENO;

    // Quitar salto de linea
    linea[strcspn(linea, "\n")] = '\0';

    // Separar comandos por "|"
    for (char *tok = strtok_r(linea, "|", &saveptr1);
         tok && n < MAX_COMANDOS;
         tok = strtok_r(NULL, "|", &saveptr1)) {
        cmds[n++] = tok;
    }

    if (n == 0) return -1;

    // Ejecutar cada comando del pipeline
    for (int i = 0; i < n; i++) {
        int pipefd[2];
        int es_ultimo = (i == n - 1);

        // Crear pipe si no es el ultimo comando
        if (!es_ultimo && pipe(pipefd) == -1) {
            perror("pipe");
            return -1;
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            return -1;
        }

        if (pid == 0) {
            // HIJO: redirigir entrada si no es el primero
            if (fd_in != STDIN_FILENO) {
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }
            // HIJO: redirigir salida si no es el ultimo
            if (!es_ultimo) {
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[0]);
                close(pipefd[1]);
            }

            // Parsear argumentos inline
            char *args[MAX_ARGS];
            int argc = 0;
            while (*cmds[i] == ' ' || *cmds[i] == '\t') cmds[i]++;
            for (char *arg = strtok_r(cmds[i], " \t", &saveptr2);
                 arg && argc < MAX_ARGS - 1;
                 arg = strtok_r(NULL, " \t", &saveptr2)) {
                args[argc++] = arg;
            }
            args[argc] = NULL;

            if (argc == 0) _exit(1);
            execvp(args[0], args);
            perror(args[0]);
            _exit(1);
        }

        // PADRE: cerrar descriptores y preparar siguiente iteracion
        if (fd_in != STDIN_FILENO) close(fd_in);
        if (!es_ultimo) {
            close(pipefd[1]);
            fd_in = pipefd[0];
        }
    }

    // Esperar todos los hijos
    while (wait(NULL) > 0);
    return 0;
}
