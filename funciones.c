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

#define _POSIX_C_SOURCE 200809L  // Necesario para strtok_r
#include "funciones.h"

// Entradas: mensaje - texto a mostrar
// Salidas: ninguna
// Descripcion: Muestra el mensaje de ayuda del programa
void mostrar_ayuda(const char *nombre_programa) {
    printf("Uso: %s [-h]\n", nombre_programa);
    printf("Lee una linea de pipeline desde stdin y la ejecuta.\n\n");
    printf("Opciones:\n");
    printf("  -h    Muestra esta ayuda\n\n");
    printf("Ejemplo:\n");
    printf("  echo \"./generator.sh -i 1 -t 5 | ./preprocess.sh\" | %s\n", nombre_programa);
}

// Entradas: cmd - cadena con un comando y sus argumentos
//           args - array donde se guardaran los argumentos parseados
// Salidas: numero de argumentos parseados
// Descripcion: Separa un comando en tokens por espacios/tabs y los guarda en args
static int parsear_argumentos(char *cmd, char *args[]) {
    int argc = 0;
    char *token;
    char *saveptr;

    // Eliminar espacios al inicio
    while (*cmd == ' ' || *cmd == '\t') cmd++;

    // Tokenizar por espacios y tabs
    token = strtok_r(cmd, " \t\n", &saveptr);
    while (token != NULL && argc < MAX_ARGS - 1) {
        args[argc++] = token;
        token = strtok_r(NULL, " \t\n", &saveptr);
    }
    args[argc] = NULL;  // Terminar con NULL para execvp

    return argc;
}

// Entradas: linea - cadena con el pipeline completo (ej: "cmd1 | cmd2 | cmd3")
// Salidas: 0 si exito, -1 si error
// Descripcion: Parsea la linea, crea procesos hijos conectados por pipes,
//              y ejecuta cada comando del pipeline
int ejecutar_pipeline(char *linea) {
    char *comandos[MAX_COMANDOS];  // Array de punteros a cada comando
    int num_comandos = 0;
    char *token;
    char *saveptr;

    // Eliminar salto de linea final si existe
    size_t len = strlen(linea);
    if (len > 0 && linea[len - 1] == '\n') {
        linea[len - 1] = '\0';
    }

    // Separar la linea por el caracter "|"
    token = strtok_r(linea, "|", &saveptr);
    while (token != NULL && num_comandos < MAX_COMANDOS) {
        comandos[num_comandos++] = token;
        token = strtok_r(NULL, "|", &saveptr);
    }

    // Verificar que hay al menos un comando
    if (num_comandos == 0) {
        fprintf(stderr, "Error: No se encontraron comandos en la linea\n");
        return -1;
    }

    // fd_in guarda el descriptor de lectura del pipe anterior
    // Inicialmente es STDIN (el primer proceso lee de la entrada estandar)
    int fd_in = STDIN_FILENO;

    // Iterar sobre cada comando del pipeline
    for (int i = 0; i < num_comandos; i++) {
        int pipefd[2];  // pipefd[0] = lectura, pipefd[1] = escritura

        // Crear pipe solo si NO es el ultimo comando
        // (el ultimo comando escribe a stdout, no a un pipe)
        if (i < num_comandos - 1) {
            if (pipe(pipefd) == -1) {
                perror("Error al crear pipe");
                return -1;
            }
        }

        // Crear proceso hijo con fork()
        pid_t pid = fork();

        if (pid == -1) {
            perror("Error en fork");
            return -1;
        }

        if (pid == 0) {
            // === PROCESO HIJO ===

            // Si no es el primer comando, redirigir stdin al pipe anterior
            if (fd_in != STDIN_FILENO) {
                dup2(fd_in, STDIN_FILENO);  // stdin <- pipe anterior
                close(fd_in);
            }

            // Si no es el ultimo comando, redirigir stdout al pipe actual
            if (i < num_comandos - 1) {
                dup2(pipefd[1], STDOUT_FILENO);  // stdout -> pipe actual
                close(pipefd[0]);  // Cerrar extremo de lectura (no lo usamos)
                close(pipefd[1]);  // Cerrar original (ya esta duplicado)
            }

            // Parsear argumentos del comando actual
            char *args[MAX_ARGS];
            int argc = parsear_argumentos(comandos[i], args);

            if (argc == 0) {
                fprintf(stderr, "Error: Comando vacio\n");
                exit(1);
            }

            // Ejecutar el comando con execvp
            // execvp busca el ejecutable en PATH y reemplaza el proceso actual
            execvp(args[0], args);

            // Si llegamos aqui, execvp fallo
            perror("Error en execvp");
            exit(1);
        }

        // === PROCESO PADRE ===

        // Cerrar el descriptor de entrada anterior (ya lo paso al hijo)
        if (fd_in != STDIN_FILENO) {
            close(fd_in);
        }

        // Si no es el ultimo comando, cerrar escritura y guardar lectura
        if (i < num_comandos - 1) {
            close(pipefd[1]);      // Cerrar escritura (la usa el hijo)
            fd_in = pipefd[0];     // Guardar lectura para el siguiente hijo
        }
    }

    // Esperar a que todos los hijos terminen
    int status;
    while (wait(&status) > 0) {
        // Esperar cada hijo
    }

    return 0;
}
