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
            char validez[1];
            for (int j = 0; j < 10; j++){
                validez[0] = (char) buffer[256*i + 14 + 21*j];
                for (int k = 0; k < 12; k++){
                    // 256 --> proceso / 21 -> archivo / 14 info proceso / k + 1 para saltarse el de validez
                    archivo2[k] = (char)buffer[256*i + 15 + 21*j + k];
                }
                if (0 != strlen(archivo2)){
                    printf("Nombre del archivo: %s || validez: %d\n", archivo2, validez[0]); 
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

// converts 32-bits positive integer to 4-bytes big endian
void to4bi(unsigned long int const value, char * const buffer)
{
	buffer[0] = value >> 8 * 3;
	buffer[1] = value >> 8 * 2;
	buffer[2] = value >> 8 * 1;
	buffer[3] = value >> 8 * 0;
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
        printf("VPN %d: %d\n", i, vpns[i]);
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
        printf("PFN %d: %d\n", i, pfns[i]);
        pfn += pfns[i] * pow(2, i);
    }
    return pfn;
}

CrmsFile* cr_open(int process_id, char* file_name, char mode){
    int exists = cr_exists(process_id, file_name);
    if ((mode == 'r' && exists == 1) || (mode == 'w' && exists == 0)){
        // solo posibilidad de leer archivos existentes o escribir nuevos sorryyyyyyyy 

        // checks that file exists & returns CrmsFile*
        CrmsFile* crms_file = calloc(1, sizeof(CrmsFile));
        crms_file -> file_name = file_name;
        crms_file -> process_id = process_id;
        crms_file -> mode = mode;

        unsigned char buffer_starts[4096];
        FILE *ptr;

        ptr = fopen(ruta_local,"rb+");  // r for read, b for binary
        fread(buffer_starts, sizeof(buffer_starts), 1, ptr); // read 10 bytes to our buffer


        // capacidad de procesos
        for (int i = 0; i < 16; i++){

            int id = buffer_starts[(i * 256) + 1];
            int validez = buffer_starts[(i * 256)];

            // si coinicde con proceso buscado
            if (id == process_id && validez == 1){
                crms_file -> buffer_iterator = i;
                // reviso sus archivos
                if (exists == 1){
                    printf("Existía el archivo que se solicitó leer\n");
                    char archivo[12];
                    for (int j = 0; j < 10; j++){
                    // para cada uno, guardo su nombre y veo si es el actual, si es guardo en archive_iterator el j
                        for (int k = 0; k < 12; k++){
                            // 256 --> proceso / 21 -> archivo / 14 info proceso / k + 1 para saltarse el de validez
                            archivo[k] = (char)buffer_starts[256*i + 15 + 21*j + k];
                        }
                        if (strcmp(archivo, file_name) == 0){
                            crms_file -> archive_iterator = j;
                            printf("Nombre del archivo encontrado! archive_iterator = %d\n", j); 
                        }     
                    
                    }  
                }
                else{
                    printf("No existía el archivo que se solicitó escribir, por lo que se creó \n");
                    // crear nombre con size 0 y validez 1
                    int indice = -1;
                    char archivo[1];
                    int validez = 1;
                    int initial_size = 0;
                    char size_buffer[4];
                    to4bi(initial_size, size_buffer);

                    for (int j = 0; j < 10; j++){
                        archivo[0] = (char)buffer_starts[256*i + 14 + 21*j];
                        if (archivo[0] == 0){
                            // current archive is invalid
                            indice = j;
                            printf("Indice para nuevo archivo encontrado! archive_iterator = %d\n", j); 
                            crms_file -> archive_iterator = j;
                            break;
                        }  
                    }
                    if (indice != -1){
                        fseek(ptr, 256*i + 14 + 21*indice , SEEK_SET);
                        fwrite(&validez, 1, 1, ptr);
                        rewind(ptr);
                        fseek(ptr, 256*i + 15 + 21*indice , SEEK_SET);
                        fwrite(file_name, 1, 12, ptr);
                        rewind(ptr);
                        fseek(ptr, 256*i + 27 + 21*indice , SEEK_SET);
                        fwrite(&size_buffer, 1, 4, ptr);
                        fclose(ptr);
                        printf("Creados tambien su nombre, validez y tamaño en la tabla :)\n"); 
                        return crms_file;


                    }
                    else{
                        printf("No cabe el archivo para este proceso :(\n");
                    }




                }       
            }  
        }
        fclose(ptr);
        return crms_file;
    }
    else{
        printf("Error en cr_open\n");
        // PENDIENTES
        // Qué devolver en otro caso?
        return 0;
    }
    
    

} 

int read_conversion_table (int dirvir, CrmsFile* file_desc){

    unsigned char buffer[1];
    FILE *ptr;

    // escribo (14 --> extras, 10*21 --> 21 entradas archivos, 32 donde debo elegir que página)
    int vpn = (int) dirvir / 8388608;
    printf("VPN buscado es %d\n", vpn);
    int offset_in_process = 14 + 210 + vpn ;
    int choose_process = 256 * file_desc -> buffer_iterator;
    int total_direction = choose_process + offset_in_process;

    ptr = fopen(ruta_local,"rb");  // r for read, b for binary
    
    fseek(ptr, total_direction, SEEK_SET);
    fread(buffer, sizeof(buffer), 1, ptr); 
    
    // leo 1 byte : 1 bit validez| 7 bits pfn
    int pfn = -1;
    
    
    // if es válido
    int valid = (buffer[0] >> 7) & 1;
    if (valid == 1){
        pfn = buffer[0] - 128;
    }
    printf("pfn calculado es %d\n", pfn);

    return pfn;
    

}



int cr_write_file(CrmsFile* file_desc, void* buffer, int n_bytes){
    // - Confirmar modo write
    if (file_desc -> mode == 'w' && file_desc){

        // direccion, tamaño, bits
        int starts[10][2];
        unsigned char * virmems [10];
        int counter = 0;
        unsigned char buffer_starts[4096];
        FILE *ptr;

        ptr = fopen(ruta_local,"rb+");  // r for read, b for binary
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
                file_desc -> buffer_iterator = i;
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
                    virmems[counter] = direction;
                    counter ++;
                }       
            }  
        }

        for (int i = 0; i < 10; i++){
            printf("direccion: %d / tamaño: %d\n", starts[i][0], starts[i][1]);
        }

        Options * sorted = sort_valid_process_files(starts, virmems);

        for (int i = 0; i < sorted -> valid_quantity ; i++){
            printf("VALID direccion: %d / tamaño: %d\n", sorted -> ordered[i][0], sorted -> ordered[i][1]);
        }

        if (sorted -> valid_quantity == 10){
// ------------------------comprobar q es uno de los existentes y osino:
            printf("Sorry flaco, ya hay 10 arxivos :/\n");
// ------------------------no sabemos que retornar
            return 0;
        }

        // Osino itero sobre lista y hay dos posibles escenarios
        // 1. El primero de la lista ordenada no esté guardado en 0: en 0 guardamos este archivo
        // 2. El primero de la lista ordenada sí sea 0, avanzamos hasta encontrar espacio disponible (máximo 1073741824 creemos)

        unsigned int pfn_inicial = 0;
        unsigned int pfn_final = 0;
        unsigned int dirvir_inicial = 0;
        unsigned int dirvir_final = 0;
        // maximo de 32 páginas de 8 mb cu
        // revisar max
        // unsigned int max = 255999999; 
        unsigned int max = 268435455;
        printf("Unsigned %u\n", (unsigned int)(pow(2, 23) - 1));

        if(sorted -> valid_quantity == 0 || sorted -> ordered[0][0] != 0){
            // Caso 1

            // inicia en 0
            dirvir_inicial = 0;

            // termina como maximo al prinicipio del primer archivo
            dirvir_final = sorted -> ordered[0][0];
            if (sorted -> valid_quantity == 0){
                // printf("entre qui qiiwiidvshkacavd\n");
                // No hay archivos, el fin es el límite
                dirvir_final = max;
                // printf("final %d\n", dirvir_final);


            }
  
        }
        else {
            // Caso 2: recorremos 
            
            int found = 0;
            
            for (int i = 0; i < sorted -> valid_quantity - 1; i += 1){
                    int start_1 = sorted -> ordered [i][0];
                    int size = sorted -> ordered [i][1];
                    int start_2 = sorted -> ordered [i + 1][0];


                    // encontré espacio disponible entremedio 
                    if (start_1 + size < start_2 ){

                        found = 1;

                        // inicia en final de archivo i
                        dirvir_inicial = sorted -> ordered [i][0] + sorted -> ordered [i][1]; 

                        // termina como maximo al inicio de archivo i + 1
                        dirvir_final = sorted -> ordered [i + 1][0];

                    }
            }
            if (found == 0){
                // compruebo que hay espacio al final y osino no cabe
                // COMPROBAR SI SE PUEDE ESCRIBIR HASTA MAX O MAX - 1 E IMPLICANCIAS

                int final_available = sorted -> ordered [sorted -> valid_quantity - 1][0] + sorted -> ordered [sorted -> valid_quantity - 1][1];

                if (final_available < max - 1){

                    // inicia en ultimo archivo + tamaño
                    dirvir_inicial = final_available;

                    // termina como maximo al final de la memoria
                    dirvir_final = max;
                }
                
                else{
//------------------------------- NO CABE 
                    printf("Sorry flaco, no cabe tu arxivo :/\n");
                    return 0;
                }


            }
                
        }

        // calculo pfn inicial

        printf("DIRVIR inicial: %d\n", dirvir_inicial);
        printf("DIRVIR final: %d\n", dirvir_final);

        unsigned char * buffer_dirvir_inicial[4];

        // Calculo de los offset final e inicial

        // (32 - 23 = 9 --> Cantidad de lugares que sobran a las izq en el número)
        unsigned int initial_offset = (dirvir_inicial << 9) >> 9;
        unsigned int final_offset = (dirvir_final << 9) >> 9;
        
        to4bi(dirvir_inicial, buffer_dirvir_inicial);

        pfn_inicial = read_conversion_table(dirvir_inicial, file_desc);

        printf("PFN inicial: %d\n", pfn_inicial);

        if (pfn_inicial == -1){

            printf("Pedi frame para partir pq im new\n");
            // pido a bitmap y vuelvo a buscar

            pfn_inicial = ask_for_frame();

            if (pfn_inicial != -1){
                printf("PFN nuevo inicial: %d\n", pfn_inicial);

                // Bitmap pedido y pfn actualizado
                // Ahora asociar frame a pfn
                // byte de la tabla validez|pfn
                
                // declaro como valido = 128 = 0x10000000 
                unsigned int byte = 128;

                // sumo los bits de pfn
                byte += pfn_inicial;

                // escribo (14 --> extras, 10*21 --> 21 entradas archivos, página 0)
                int offset_in_process = 14 + 210 + 0 ;
                int choose_process = 256 * file_desc -> buffer_iterator;
                int total_direction = choose_process + offset_in_process;

                fseek(ptr, total_direction, SEEK_SET);
                fwrite(&byte, 1, 1, ptr);

            }
            else{
                // No dieron frame, paro de escribir pues se acabo la memoria
                printf("We are sorry, there is no space left :'(\n");
            }

        }
        
        unsigned int current_pfn = pfn_inicial;
        unsigned int current_dirvir = dirvir_inicial;

        int bytes_written = 0;
        unsigned int offset;
        while(bytes_written < n_bytes && current_dirvir < dirvir_final){

            offset = current_dirvir % 8388608;
            // unsigned int offset = (current_dirvir << 9) >> 9;
            // printf("Offset actual: %d\n", offset);
            // printf("division: %d\n", current_dirvir % 8388608);


            // Llegamos al limite de una página
            if (current_dirvir % 8388608 == 0 && current_dirvir != 0) {

                printf("llegue a limite - pido nuevo frame\n");

                current_pfn = ask_for_frame();

                if (current_pfn != -1){
                    printf("PFN nuevo : %d\n", current_pfn);
                    // Bitmap pedido y pfn actualizado
                    // Ahora asociar frame a pfn
                    // byte de la tabla validez|pfn
                    // 128 = 0x10000000 = valido
                    unsigned int byte = 128;

                    // sumo los bits de pfn
                    byte += current_pfn;

                    // escribo (14 --> extras, 10*21 --> 21 entradas archivos, 32 donde debo elegir que página)
                    int offset_in_process = 14 + 210 + current_dirvir / 8388608 ;
                    int choose_process = 256 * file_desc -> buffer_iterator;
                    int total_direction = choose_process + offset_in_process;

                    fseek(ptr, total_direction, SEEK_SET);
                    fwrite(&byte, 1, 1, ptr);

                }
                else{
                    // No dieron frame, paro de escribir pues se acabo la memoria
                    printf("We are sorry, there is no space left :'(\n");
                    break;
                }


            }

            // Escribir en pfn + offset el buffer[bytes_written]
            // printf("Escribiendo un byte\n");
            fseek(ptr, 5012 + (8388608 * current_pfn) + offset, SEEK_SET);
            fwrite(&buffer[bytes_written], 1, 1, ptr);
            bytes_written += 1;
            current_dirvir += 1;
        }
        
        // Actualizar tamaño archivo 

        // escribo (14 --> extras, indice*21 --> 21 cada archivo antes + 13 de validez y nombre)
        // ahi parten 4 que corresponden al tamaño de bytes

        // pasar bytes written a endianness correcto
        char final_size_buffer[4];
        to4bi(bytes_written, final_size_buffer);
        printf("bytes totales escritos: %d\n", bytes_written);
        printf("pfn inicial: %d\n", pfn_inicial);

        printf("pfn final: %d\n", current_pfn);

        int offset_in_process = 14 + file_desc -> archive_iterator * 21 + 13 ;
        int choose_process = 256 * file_desc -> buffer_iterator;
        int total_direction = choose_process + offset_in_process;

        fseek(ptr, total_direction, SEEK_SET);
        fwrite(&final_size_buffer, 4, 1, ptr);

        fclose(ptr);
        return bytes_written;

       
//----------------- aca pa abajo es caca --------------------------
        // calculo pfn final
        unsigned char * buffer_dirvir_final[4];
        printf("entre qui ---------------------- %d\n", dirvir_final);

        to4bi(dirvir_final, buffer_dirvir_final);
        // pfn_final = transform_dirvir_pfn(buffer_dirvir_final, buffer_starts, file_desc -> buffer_iterator);

        printf("Offset inicial calculado: %ld / final: %ld\n", initial_offset, final_offset);

        printf("VPN inicial calculado: %d/ final: %d\n", dirvir_inicial, dirvir_final);   

        printf("PFN inicial calculado: %u/ final: %u\n", pfn_inicial, pfn_final);

        // Calculo de direcciones de memoria
        // Se corren los pfn 23 lugares a la izquierda
        unsigned int dirfis_inicial = (pfn_inicial << 23) + initial_offset;  
        unsigned int dirfis_final = (pfn_final << 23) + final_offset;  

        printf("Dirección fisica inicial calculada: %u/ final: %u\n", dirfis_inicial, dirfis_final);

        // pfn_inicial = transform_dirvir_pfn(buffer_dirvir_inicial, buffer_starts, file_desc -> buffer_iterator);

        

        // - Encontrar proximo espacio disponible -> itero sobre la memoria virtual 
        // - Si tiene 10 archivos return
        // - Partimos chequeando en memoria más "arriba"
        // - Guardar frame o pagina en tabla 
        // - escribir los bytes
        // - almacenar memoria usada (tamaño) en la tabla y en el struct
        // - devolver cantidad bytes escritos
        // *SUPUESTO* suma de espacios válidos para procesos es menor o igual a memoria real (no permite sobreescribir)
// ----------------- aca pa arriba es caca --------------------------
    }
    else {
        printf("El archivo a escribir no se abrió en el modo correcto\n");
    }
    


 }

int ask_for_frame() {
    // Retorna -1 si hay error o el numero del frame asignado
    unsigned char buffer_starts[5012];
    FILE *ptr;

    ptr = fopen(ruta_local,"rb+");  // r for read, b for binary
    fread(buffer_starts, sizeof(buffer_starts), 1, ptr);
    int pfn = -1;
    for (int i = 0; i < 16; i++) {
        unsigned char current_byte = buffer_starts[4096 + i];
        int mask = 1;

        for (int j = 7; j >= 0; j--){
            int is_1 = ((current_byte >> j) & mask );

            if ( !is_1 ) {
                pfn = i * 8 + (7 - j);
                current_byte = current_byte | mask << j;
                break;
            }
        }

        if (pfn != -1) {
            fseek(ptr, 4096 + i, SEEK_SET);
            fwrite(&current_byte, sizeof(char), 1, ptr);
            break;
        }
    }
    printf("PFN: %d\n", pfn);
    fclose(ptr);
    return pfn;
}

int cr_read(CrmsFile* file_desc, void* buffer, int n_bytes){


}

void cr_delete_file(CrmsFile* file_desc){

}

void cr_close(CrmsFile* file_desc){
    
}

int virmem_to_vpn(int vir_mem){
    
}

Options* sort_valid_process_files(int array[10][2], unsigned char * virmems [10]){

    // Aquí dejo el espacio para los 10 archivos ordenados

    Options* sorted = calloc(1, sizeof(Options));

    // Luego fijo la diferencia a proximo archivo (limite final de memoria contigua) muy lejos 

    int difference = 1000000000;
    int index = -1;

    // y creo el indice para ir guardando en ordered 
    int final_index_counter = 0;
    int actualizado = 0;

    // Identifico el primer archivo
    for (int i = 0; i < 10; i++){

        if (array[i][0] == 0 && array[i][1] != 0){
            index = i;
            actualizado = 1;
            break;
        }
        else if (array[i][0] < difference && array[i][1] != 0){
            difference = array[i][0];
            index = i;
            actualizado = 1;
        }
    }

    int files_exist = 1;
    if (actualizado == 0){
        // No hay archivos
        files_exist = 0;
    }

    // y luego en un ciclo
    while (files_exist){
        // guardo el caso identificado en el proximo espacio de ordered
        sorted -> ordered[final_index_counter][0] = array[index][0];
        sorted -> ordered[final_index_counter][1] = array[index][1];
        sorted -> virmems[final_index_counter] = virmems[index];
        final_index_counter ++;

        // calculo cual es el proximo inicio si memoria fuera usada contiguamente
        int next_position = array[index][0] + array[index][1];
        
        // seteo las variables para encontrar al proximo mas cercano si no fuera contiguo
        int partial_difference = 1000000000;
        int partial_index = -1;
        int found = 0;
        

        for (int i = 0; i < 10; i++){
            // encontré el contiguo
            if (array[i][0] == next_position && array[i][1] != 0){
                index = i;
                found = 1;
                break;
            }
            // pruebo mejor opción considerando que
            // el inicio del archivo en observación es mayor al inciio del último archivo recién ordenado
            // que la diferencia es la más chica de entre las opciones
            // que su tamaño es mayor a 0
            else if (
            array[i][0] > sorted -> ordered[final_index_counter - 1][0] 
            && array[i][0] - next_position < partial_difference 
            && array[i][1] != 0){
                partial_difference = array[i][0];
                partial_index = i;
                found = 2;
            }
        }
        // found = 1 encontré contiguo asique paso a proxima iteracion de while
        if (found != 1){
            // found = 0 no hay más archivos por ordenar, break del while
            if (found == 0){
                break;
            }
            // found = 2 hay un archivo identificado pero no contiguo
            else {
                index = partial_index;
            }   
        }
    }
    // guardamos cantidad de archivos
    sorted -> valid_quantity = final_index_counter;
    
    return sorted;


}
