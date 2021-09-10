// Tells the compiler to compile this file once
#pragma once

// Define compile-time constants
#define MAX_SPLIT 255
#define BUFFER_SIZE 4096

// Define the struct
typedef struct process {
  int pid;
  char *nombre;
  int numero_fabrica;
  int estado;
  int tiempo_init;
  int bursts;
  //char cpu_bursts[];
  int* cpu_io;
  int burst_actual;
  int io_actual;
  int tiempo_restante_burst;
  int tiempo_restante_io;
} Process;

typedef struct queue {
  Process* process;
} Cola; 