# Laboratorio 2: Monitoreo y Procesamiento de Procesos con Bash y Pipes
# Curso de Sistemas Operativos - Segundo Semestre 2025
#
# Integrantes:
# - Nombre: Sofia Vergara, RUT 21.082.148-1
# - Nombre: Vladimir Vidal, RUT 18.031.181-5
#
# Archivo: Makefile
# Descripcion: Compilacion por partes del programa lab2

CC = gcc
CFLAGS = -Wall -Wextra -std=c99

# Target principal
all: lab2

# Enlazar objetos para crear ejecutable
lab2: lab2.o funciones.o
	$(CC) $(CFLAGS) -o lab2 lab2.o funciones.o

# Compilar lab2.c
lab2.o: lab2.c funciones.h
	$(CC) $(CFLAGS) -c lab2.c

# Compilar funciones.c
funciones.o: funciones.c funciones.h
	$(CC) $(CFLAGS) -c funciones.c

# Limpiar archivos generados
clean:
	rm -f *.o lab2

# Ejecutar ejemplo
test:
	echo "./generator.sh -i 1 -t 3 | ./preprocess.sh | ./filter.sh -c 0 -m 0 | ./transform.sh --anon-uid | ./aggregate.sh | ./report.sh -o reporte.csv" | ./lab2

.PHONY: all clean test
