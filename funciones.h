/*
 * Laboratorio 2: Monitoreo y Procesamiento de Procesos con Bash y Pipes
 * Curso de Sistemas Operativos - Segundo Semestre 2025
 *
 * Integrantes:
 * - Nombre: [NOMBRE_1], RUT: [RUT_1]
 * - Nombre: [NOMBRE_2], RUT: [RUT_2]
 *
 * Archivo: funciones.h
 * Descripcion: Cabeceras de funciones para el pipeline de procesos
 */

#ifndef FUNCIONES_H
#define FUNCIONES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_COMANDOS 10
#define MAX_ARGS 64
#define MAX_LINEA 4096

// Entradas: linea - cadena con el pipeline completo (ej: "cmd1 | cmd2 | cmd3")
// Salidas: 0 si exito, -1 si error
// Descripcion: Parsea la linea, crea procesos hijos conectados por pipes,
//              y ejecuta cada comando del pipeline
int ejecutar_pipeline(char *linea);

// Entradas: mensaje - texto a mostrar
// Salidas: ninguna
// Descripcion: Muestra el mensaje de ayuda del programa
void mostrar_ayuda(const char *nombre_programa);

#endif
