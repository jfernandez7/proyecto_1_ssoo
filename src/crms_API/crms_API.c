#include "crms_API.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>
//#include <cstdint>

char *ruta_local;
//Funciones generales

void cr_mount(char* memory_path){
    printf("cr_mount \n");
    ruta_local = memory_path;
    printf("ruta_local %s\n", ruta_local);
}

void cr_ls_processes(){
    printf("cr_ls_processes \n");
    unsigned char buffer[4096];
    FILE *ptr;
    ptr = fopen(ruta_local,"rb");  // r for read, b for binary
    fread(buffer, sizeof(buffer), 1, ptr); // read 10 bytes to our buffer

    for (int i = 0; i < 16; i++){   
        int estado = buffer[i * 256];
        //printf("estado %u\n", estado);
        if (estado == 1){
            int id = buffer[(i * 256) + 1];
            printf("Procesos en ejecución, id: %u \n", id);
        }   
    }
    fclose(ptr);
}

int cr_exists(int process_id, char* file_name){
    printf("cr_exists \n");
    unsigned char buffer[4096];
    FILE *ptr;
    ptr = fopen(ruta_local,"rb");  // r for read, b for binary
    fread(buffer, sizeof(buffer), 1, ptr); // read 10 bytes to our buffer
    char archivo1[12];

    for (int i = 0; i < 16; i++){
        int id = buffer[(i * 256) + 1];
        if (id == process_id){
            for (int j = 0; j < 10; j++){
                for (int k = 0; k < 12; k++){
                    archivo1[k] = (char)buffer[256*i + 15 + 21*j + k];
                }
                int result = strcmp(archivo1, file_name);
                if (result == 0){
                    return 1;
                }     
            }       
        }  
    }
    fclose(ptr);
    return 0;
    
}

void cr_ls_files(int process_id){
    printf("cr_ls_files \n");
    unsigned char buffer[4096];
    FILE *ptr;
    ptr = fopen(ruta_local,"rb");  // r for read, b for binary
    fread(buffer, sizeof(buffer), 1, ptr); // read 10 bytes to our buffer
    char archivo2[12];

    for (int i = 0; i < 16; i++){
        int id = buffer[(i * 256) + 1];
        if (id == process_id){
            for (int j = 0; j < 10; j++){
                for (int k = 0; k < 12; k++){
                    archivo2[k] = (char)buffer[256*i + 15 + 21*j + k];
                }
                if (0 != strlen(archivo2)){
                    printf("Nombre del archivo: %s\n", archivo2); 
                }     
            }       
        }  
    }
    fclose(ptr);
}

//Funciones procesos

void cr_start_process(int process_id, char* process_name){
    printf("cr_start_process \n");
    unsigned char buffer[4096];
    FILE *ptr;
    ptr = fopen(ruta_local,"rb");  // r for read, b for binary
    fread(buffer, sizeof(buffer), 1, ptr); // read 10 bytes to our buffer

    int pos_a_escribir = 0;
    for (int i = 0; i < 16; i++){
        int cont = 0;
        for (int j = 0; j < 256; j++){
            int numero = buffer[256*i + j];
            if (numero == 0){
                cont += 1;
            }  
        }
        printf("Cont = %i\n", cont);
        if (cont == 256){
           pos_a_escribir = i; 
           break;
        }  
    }
    fclose(ptr);
    
    printf("Pos %i\n", pos_a_escribir);
    printf("Estado antes = %u\n", buffer[pos_a_escribir*256]);
    printf("Id antes = %u\n", buffer[pos_a_escribir*256 + 1]);
    //printf("Tamaño = %lu\n", sizeof(unsigned char));
    //printf("Tamaño = %li\n", sizeof(uint8_t));

    FILE *ptr2;
    ptr2 = fopen(ruta_local,"rb+");

    unsigned char estado;
    unsigned char id;
    estado = 1;
    id = process_id;
    fseek(ptr2, pos_a_escribir*256*sizeof(unsigned char), SEEK_SET);
    fwrite(&estado, sizeof(unsigned char), 1, ptr2);
    fseek(ptr2, (pos_a_escribir*256 + 1)*sizeof(unsigned char), SEEK_SET);
    fwrite(&id, sizeof(unsigned char), 1, ptr2);
    fclose(ptr2);

    unsigned char buffer2[4096];
    FILE *ptr3;
    ptr3 = fopen(ruta_local,"rb");  // r for read, b for binary
    fread(buffer2, sizeof(buffer2), 1, ptr3); // read 10 bytes to our buffer
    printf("Numero despues = %u\n", buffer2[pos_a_escribir*256]);
    printf("Id despues = %u\n", buffer2[pos_a_escribir*256 + 1]);


    //fseek(ptr, 2, SEEK_CUR);
    //fputc(1, ptr);
    //fclose(ptr);

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