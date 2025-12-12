/*
 * Laboratorio 2: Monitoreo y Procesamiento de Procesos con Bash y Pipes
 * Curso de Sistemas Operativos - Segundo Semestre 2025
 *
 * Integrantes:
 * Nombre: Sofia Vergara, RUT 21.082.148-1
 * Nombre: Vladimir Vidal, RUT 18.031.181-5
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

// Entradas: linea - pipeline completo (ej: "cmd1 | cmd2")
// Salidas: 0 exito, -1 error
// Descripcion: Crea procesos hijos conectados por pipes y ejecuta cada comando
int ejecutar_pipeline(char *linea);

#endif
