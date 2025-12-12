/*
 * Laboratorio 2: Monitoreo y Procesamiento de Procesos con Bash y Pipes
 * Curso de Sistemas Operativos - Segundo Semestre 2025
 *
# Integrantes:
# - Nombre: Sofia Vergara, RUT 21.082.148-1
# - Nombre: Vladimir Vidal, RUT 18.031.181-5
 *
 * Archivo: funciones.c
 * Descripcion: Implementacion del pipeline de procesos
 */

#define _POSIX_C_SOURCE 200809L
#include "funciones.h"

// Entradas: linea - pipeline completo (ej: "cmd1 -a | cmd2 -b")
// Salidas: 0 exito, -1 error
// Descripcion: Parsea la linea, crea procesos hijos con fork(),
//              los conecta con pipe() y dup2(), ejecuta con execvp()
int ejecutar_pipeline(char *linea) {
    char *cmds[MAX_COMANDOS], *saveptr1, *saveptr2;
    int n = 0, fd_in = STDIN_FILENO;

    // Quitar salto de linea y separar comandos por "|"
    linea[strcspn(linea, "\n")] = '\0';
    for (char *tok = strtok_r(linea, "|", &saveptr1); tok && n < MAX_COMANDOS; tok = strtok_r(NULL, "|", &saveptr1))
        cmds[n++] = tok;

    if (n == 0) return -1;

    for (int i = 0; i < n; i++) {
        int pipefd[2], es_ultimo = (i == n - 1);

        if (!es_ultimo && pipe(pipefd) == -1) return -1;

        pid_t pid = fork();
        if (pid == -1) return -1;

        if (pid == 0) {
            // Redirigir entrada/salida
            if (fd_in != STDIN_FILENO) { dup2(fd_in, STDIN_FILENO); close(fd_in); }
            if (!es_ultimo) { dup2(pipefd[1], STDOUT_FILENO); close(pipefd[0]); close(pipefd[1]); }

            // Parsear argumentos y ejecutar
            char *args[MAX_ARGS];
            int argc = 0;
            while (*cmds[i] == ' ' || *cmds[i] == '\t') cmds[i]++;
            for (char *arg = strtok_r(cmds[i], " \t", &saveptr2); arg && argc < MAX_ARGS - 1; arg = strtok_r(NULL, " \t", &saveptr2))
                args[argc++] = arg;
            args[argc] = NULL;

            if (argc == 0) _exit(1);
            execvp(args[0], args);
            _exit(1);
        }

        // Padre: cerrar descriptores
        if (fd_in != STDIN_FILENO) close(fd_in);
        if (!es_ultimo) { close(pipefd[1]); fd_in = pipefd[0]; }
    }

    while (wait(NULL) > 0);
    return 0;
}
