#include "crms_API.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>

char *ruta_local;
//Funciones generales

void cr_mount(char* memory_path){
    printf("cr_mount \n");
    ruta_local = memory_path;
    printf("ruta_local %s\n", ruta_local);
}

void cr_ls_processes(){
    printf("cr_ls_processes \n");
    unsigned char buffer[4000];
    FILE *ptr;
    ptr = fopen(ruta_local,"rb");  // r for read, b for binary
    fread(buffer, sizeof(buffer), 1, ptr); // read 10 bytes to our buffer
    printf("id: %u\n",buffer[769] );
    // for(int i = 768; i<1024; i++){
    //  if (i == 768){
    //    printf("estado: %u\n",buffer[i] );
    //  }
    //  if (i == 769){
    //    printf("id: %d\n",buffer[i] );
    //  }
    // }


    // for (int i = 0; i < 16; i++){   
    //     int estado = buffer[i * 256];
    //     printf("estado %u\n", estado);
    //     if (estado == 0){
    //         int id = buffer[(i * 256) + 1];
    //         printf("En ejecución %u \n", id);
    //     }   
    // }
    
}

int cr_exists(int process_id, char* file_name){

}

void cr_ls_files(int process_id){

}

//Funciones procesos

void cr_start_process(int process_id, char* process_name){

}

void cr_finish_process(int process_id){

}

//Funciones archivos

CrmsFile* cr_open(int process_id, char* file_name, char mode){

} //mode puede ser 'r' o 'w'

int cr_write_file(CrmsFile* file_desc, void* buffer, int n_bytes){

}

int cr_read(CrmsFile* file_desc, void* buffer, int n_bytes){

}

void cr_delete_file(CrmsFile* file_desc){

}

void cr_close(CrmsFile* file_desc){
    
}