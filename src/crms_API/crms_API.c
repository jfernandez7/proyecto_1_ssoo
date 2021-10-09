#include "crms_API.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <math.h>
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
    // capacidad de procesos
    for (int i = 0; i < 16; i++){
        // si coincido con proceso buscado
        int id = buffer[(i * 256) + 1];
        if (id == process_id){
            // archivis
            for (int j = 0; j < 10; j++){
                for (int k = 0; k < 12; k++){
                    // 256 --> proceso / 21 -> archivo / 14 info proceso / k + 1 para saltarse el de validez
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

    // Estamos asumiendo que vamos a encontrar un espacio (sino reescribe en 0)
    int pos_a_escribir = 0;
    for (int i = 0; i < 16; i++){
        int cont = 0;
        for (int j = 0; j < 256; j++){
            int numero = buffer[256*i + j];
            if (numero == 0){
                cont += 1;
            }  
        }
        if (cont == 256){
           pos_a_escribir = i; 
           break;
        }  
    }
    fclose(ptr);
    
    // printf("Pos %i\n", pos_a_escribir);
    // printf("Estado antes = %u\n", buffer[pos_a_escribir*256]);
    // printf("Id antes = %u\n", buffer[pos_a_escribir*256 + 1]);
    //printf("Tamaño = %lu\n", sizeof(unsigned char));
    //printf("Tamaño = %li\n", sizeof(uint8_t));

    FILE *ptr2;
    ptr2 = fopen(ruta_local,"rb+");

    //unsigned char estado;
    char estado = (char)1;
    fseek(ptr2, pos_a_escribir*256*sizeof(char), SEEK_SET);
    fwrite(&estado, sizeof(char), 1, ptr2);

    //unsigned char id;
    char id = (char)process_id;
    fseek(ptr2, (pos_a_escribir*256 + 1)*sizeof(char), SEEK_SET);
    fwrite(&id, sizeof(char), 1, ptr2);

    //Escribir nombre
    for(int i=0; i<strlen(process_name); i++){
        char letra = (char)process_name[i];
        fseek(ptr2, (pos_a_escribir*256 + 2 + i)*sizeof(char), SEEK_SET);
        fwrite(&letra, sizeof(char), 1, ptr2);
    }
    fclose(ptr2);

    // unsigned char buffer2[4096];
    // FILE *ptr3;
    // ptr3 = fopen(ruta_local,"rb");  // r for read, b for binary
    // fread(buffer2, sizeof(buffer2), 1, ptr3); // read 10 bytes to our buffer
    // //unsigned estado_des = (unsigned char)buffer2[pos_a_escribir*256]
    // printf("Char = %u\n", buffer2[pos_a_escribir*256]);
    // printf("Id despues = %u\n", buffer2[pos_a_escribir*256 + 1]);
    // for(int j=0; j < 7; j++){
    //     printf("Nombre = %c\n", (char)buffer2[pos_a_escribir*256 + 2 + j]);
    // }
}

void cr_finish_process(int process_id){
    printf("cr_finish_process \n");
    unsigned char buffer[4096];
    FILE *ptr;
    ptr = fopen(ruta_local,"rb+");  // r for read, b for binary
    fread(buffer, sizeof(buffer), 1, ptr); // read 10 bytes to our buffer
    printf("Id = %u \n", buffer[1]);
    for (int i = 0; i < 16; i++){
        int id = buffer[(i * 256) + 1];
        if (id == process_id){
            for(int m=0; m<10; m++){
                int validez = buffer[256*i + 14 + 21*m];
                if(validez == 1){
                    unsigned char dir_virtual[4] = {
                    buffer[256*i + 31 + 21*m],
                    buffer[256*i + 32 + 21*m],
                    buffer[256*i + 33 + 21*m],
                    buffer[256*i + 34 + 21*m],
                    };
                    int pfn = transform_dirvir_pfn(dir_virtual, buffer, i);
                    delete_framebit(pfn);
                    break;
                }
            }
            for (int j = 0; j < 256; j++){
                char cero = (char)0;
                fseek(ptr, (i*256 + j)*sizeof(char), SEEK_SET);
                //fwrite(&cero, sizeof(char), 1, ptr);    
            }       
        }  
    }
    fclose(ptr);
    // unsigned char buffer2[4096];
    // FILE *ptr2;
    // ptr2 = fopen(ruta_local,"rb+");  // r for read, b for binary
    // fread(buffer2, sizeof(buffer2), 1, ptr); // read 10 bytes to our buffer
    // for(int m=0; m <256;m++){
    //     printf("%u", buffer2[m]);
    // }
    // fclose(ptr2);
}

//Funciones archivos

// converts 4-bytes big endian to 32-bits positive integer
unsigned long int from4bi(char const * const buffer)
{
    
	unsigned long int value = 0;
	
	value += (unsigned char) buffer[0] << 8 * 3;
	value += (unsigned char) buffer[1] << 8 * 2;
	value += (unsigned char) buffer[2] << 8 * 1;
	value += (unsigned char) buffer[3] << 8 * 0;
	
	return value;
}

void delete_framebit(int pfn){
    //Cual es el byte que tenemos que editar
    int byte_index = (int)floor((double)pfn/(double)8);
    int resto_index = (int)(pfn % 8);
    int pos_escribir = 256*16 + byte_index;

    //Abrimos los archivos
    FILE *ptr;
    ptr = fopen(ruta_local,"rb+");  // r for read, b for binary
    unsigned char buffer[4096];
    fread(buffer, sizeof(buffer), 1, ptr);

    //Editamos el byte 
    unsigned char mask = 1; // Bit mask
    unsigned char bits[8];
    for (int i = 0; i < 8; i++) {
        bits[i] = (buffer[pos_escribir] & (mask << i)) != 0;
    }
    printf("Antes %i\n", bits[8 - resto_index]);
    bits[8 - resto_index] = 0;

    int nuevo_byte = 0;
    for(int i=0; i<8; i++){
        nuevo_byte += bits[i] * pow(2, i);
    }

    char validez = (char)nuevo_byte;
    fseek(ptr, pos_escribir*sizeof(char), SEEK_SET);
    fwrite(&validez, sizeof(char), 1, ptr);
    fclose(ptr);
}

int transform_dirvir_pfn(unsigned char* dir_virtual, unsigned char *buffer, int i){
    unsigned char byte_1 = dir_virtual[0];// Read from file
    unsigned char byte_2 = dir_virtual[1];// Read from file
    unsigned char mask = 1; // Bit mask
    unsigned char bits_1[8];
    unsigned char bits_2[8];

    //printf("%i\n", byte);
    // Extract the bits
    for (int i = 0; i < 8; i++) {
        // Mask each bit in the byte and store it
        bits_1[i] = (byte_1 & (mask << i)) != 0;
        bits_2[i] = (byte_2 & (mask << i)) != 0;
    }
    // For debug purposes, lets print the received data
    unsigned char vpns[8] = {
        bits_2[7],
        bits_1[0],
        bits_1[1],
        bits_1[2],
        bits_1[3],
        0, 0, 0,
    };
    // for (int i = 0; i < 8; i++) {
    //     printf("Bit 1: %d\n",bits_1[i]);
    // }
    int vpn = 0;
    for(int i=0; i<8; i++){
        vpn += vpns[i] * pow(2, i);
    }
    printf("%i\n", vpn);

    unsigned char pfn_byte = buffer[256*i + 224 + vpn];
    unsigned char bits_3[8];
    for (int i = 0; i < 8; i++) {
        // Mask each bit in the byte and store it
        bits_3[i] = (pfn_byte & (mask << i)) != 0;
    }
    unsigned char pfns[8] = {
        bits_3[0],
        bits_3[1],
        bits_3[2],
        bits_3[3],
        bits_3[4],
        bits_3[5],
        bits_3[6],
        0,
    };
    int pfn = 0;
    for(int i=0; i<8; i++){
        pfn += pfns[i] * pow(2, i);
    }
    return pfn;
}

CrmsFile* cr_open(int process_id, char* file_name, char mode){
    int exists = cr_exists(process_id, file_name);
    if ((mode == 'r' && exists == 1) || (mode == 'w' && exists == 0)){
        // checks that file exists & returns CrmsFile*
        CrmsFile* crms_file = calloc(sizeof(CrmsFile));
        crms_file -> file_name = file_name;
        crms_file -> process_id = process_id;
        crms_file -> mode = mode;
        crms_file -> pointer = 0;
        if (exists == 1){
            printf("Existía el archivo que se solicitó leer\n");
            // crms_file -> size = ;
            // crms_file -> virtual_address = ;
        }
        else {
            printf("No existía el archivo que se solicitó escribir\n");
            // crms_file -> size = 0;
        }
        return crms_file;
    }
    printf("Error en cr_open\n");

    // PENDIENTES
    // Qué devolver en otro caso?
    // Qué poner en otros campos (en ifs)

} 

int cr_write_file(CrmsFile* file_desc, void* buffer, int n_bytes){
    // - Confirmar modo write
    if (file_desc -> mode == 'w'){

        // direccion, tamaño
        int starts[10][2];
        int counter = 0;
        unsigned char buffer_starts[4096];
        FILE *ptr;

        ptr = fopen(ruta_local,"rb");  // r for read, b for binary
        fread(buffer_starts, sizeof(buffer_starts), 1, ptr); // read 10 bytes to our buffer

        char direction[4];
        char size[4];

        // capacidad de procesos
        for (int i = 0; i < 16; i++){

            int id = buffer_starts[(i * 256) + 1];
            int validez = buffer_starts[(i * 256)];

            printf("ID %d!: %d\n", i, id);

            // si coinicde con proceso buscado
            if (id == file_desc -> process_id && validez == 1){
                // reviso sus archivos
                for (int j = 0; j < 10; j++){
                    // para cada uno, guardo su direccion y su tamaño
                    for (int k = 0; k < 4; k++){
                        // 256 --> proceso / 21 -> archivo / 14 info proceso / k + 17 para llegar a la direccion
                        direction[k] = buffer_starts[256*i + 14 + 21*j + k + 17];
                        size[k] = buffer_starts[256*i + 14 + 21*j + k + 13];
                        // printf(" direction %c / size %c\n", buffer_starts[256*i + 14 + 21*j + k + 17], buffer_starts[256*i + 14 + 21*j + k + 13]);
                    }
                    // metemos al arreglo dir + size de archivo    
                    starts[counter][0] = from4bi(direction);
                    starts[counter][1] = from4bi(size);
                    counter ++;
                }       
            }  
        }
        fclose(ptr);

        for (int i = 0; i < 10; i++){
            printf("direccion: %d / tamaño: %d\n", starts[i][0], starts[i][1]);
        }

        // - Encontrar proximo espacio disponible -> itero sobre la memoria virtual chequeando solo páginas válidas
        // - Si tiene 10 archivos fuiste
        // - Partimos chequeando en memoria más "arriba"
        // - Guardar frame o pagina en tabla 
        // - escribir los bytes
        // - almacenar memoria usada (tamaño) en la tabla y en el struct
        // - devolver los bytes escritos

    }
    else {
        // abri el archivo en el modo equivocado
        // RAISE ERROR?
    }
    


}

int cr_read(CrmsFile* file_desc, void* buffer, int n_bytes){


}

void cr_delete_file(CrmsFile* file_desc){

}

void cr_close(CrmsFile* file_desc){
    
}