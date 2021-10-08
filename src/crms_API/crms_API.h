// Tells the compiler to compile this file once
#pragma once

// Define compile-time constants
#define MAX_SPLIT 255
#define BUFFER_SIZE 4096

// Define the struct
typedef struct crmsfile {
  int pid;
  char *nombre;
} CrmsFile; //struct de archivos abiertos

//Funciones generales

void cr_mount(char* memory_path);

void cr_ls_processes();

int cr_exists(int process_id, char* file_name);

void cr_ls_files(int process_id);

//Funciones procesos

void cr_start_process(int process_id, char* process_name);

void cr_finish_process(int process_id);

//Funciones archivos

CrmsFile* cr_open(int process_id, char* file_name, char mode); //mode puede ser 'r' o 'w'

int cr_write_file(CrmsFile* file_desc, void* buffer, int n_bytes);

int cr_read(CrmsFile* file_desc, void* buffer, int n_bytes);

void cr_delete_file(CrmsFile* file_desc);

void cr_close(CrmsFile* file_desc);

int transform_dirvir_pfn(unsigned char* dir_virtual, unsigned char* buffer, int i);

void delete_framebit(int pfn);