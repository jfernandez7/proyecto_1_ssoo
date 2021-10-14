#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>

#include "../file_manager/manager.h"
//#include "../crms_API/crms_API.h"
#include "../crms_API/crms_API.c"


char *input_name;


int main(int argc, char **argv)
{
  printf("Proyecto 1\n");
  input_name = argv[1];
  char *filename = input_name;
  //char ruta[20];
  //sprintf(ruta, "../../%s", filename);
  //cr_mount(ruta);

  cr_mount(filename);
  printf("****************\n");
  // CrmsFile* data = cr_open(9, "secreeet.txt", 'w');
  // cr_write_file(data, NULL, 10);
  //ask_for_frame();

  //cr_ls_processes();
  //int resp = cr_exists(0, "secret.txt");
  //printf("respuesta cr_exists %i\n", resp);
  //cr_ls_files(0);
  //print_bytes(4096, 256);
  //cr_start_process(120, "holaa");
  //cr_finish_process(0);
  //print_bytes(4096, 256);
  // cr_finish_process(0);
  //printf("%s\n", filename);

  //unsigned char buffer[4000];
  //FILE *ptr;

  //ptr = fopen(filename,"rb");  // r for read, b for binary

  //fread(buffer,sizeof(buffer),1,ptr); // read 10 bytes to our buffer

  // //char string[12];
  // char bytes[12];
  // for(int i = 768; i<1024; i++){
  //   if (i == 768){
  //     printf("estado: %u\n",buffer[i] );
  //   }
  //   if (i == 769){
  //     printf("id: %d\n",buffer[i] );
  //   }
  //   if (i>769 && i< 781){
      
  //     printf("metiendo el byte %d en forma %c\n", buffer[i], (char)buffer[i]);
  //     bytes[i-770] = (char)buffer[i];
  //   }
  //   // 10 entradas de 21 bytes cada una 
  //   // 1 --> 1 validez / 12 nombre archivo / 4 tamaño / 4 direccion virtual

  //   printf("%u ", buffer[i]); // prints a series of bytes
  // //cr_mount(filename);

  // }
  
  // printf("nombre: %s\n", bytes);

  //char string[12];
  // char bytes[12];
  // char archivo1[12];
  // unsigned char tamano1[4];
  
  // // entrada 14 --> 14 * 256 parto leyendo los bytes (hasta += 256)
  // for(int i = 3584; i<3840; i++){
  //   // primer bit estado
  //   if (i == 3584){
  //     printf("estado: %u\n",buffer[i] );
  //   }
  //   // segundo bit id
  //   if (i == 3585){
  //     printf("id: %d\n",buffer[i] );
  //   }
  //   // siguientes 12 nombre
  //   // mayor que el id y menor que el id + 13
  //   if (i>3585 && i< 3598){
      
  //     printf("metiendo el byte %d en forma %c\n", buffer[i], (char)buffer[i]);
  //     // menos el primero para partir metiendolo en el 0 del string
  //     bytes[i-3586] = (char)buffer[i];
  //   }

  //   // 10 entradas de 21 bytes cada una 
  //   // 1 --> 1 validez / 12 nombre archivo / 4 tamaño / 4 direccion virtual
  //   int desfase = 0;
  //   //if (i == 3598){
  //     for (int j = 0; j<10 ; j++){
  //     if (i>=(3598 + desfase) && i<=(3618 + desfase)){
  //       // validez
  //       if (i == (3598 + desfase)){
  //         printf("validez archivo (%d) : %d\n", (int) desfase / 21 ,buffer[i]);
  //       }
  //       // nombre
  //       if (i == (3599 + desfase)){
  //         for (int k=0; k<12; k++){
  //           printf("metiendo el byte %d en forma %c\n", buffer[i+k], (char)buffer[i+k]);
  //           archivo1[(i+k) - (3599 + desfase)] = (char)buffer[i+k];
  //         }
          
  //       }
  //       // if (i >= (3599 + desfase) && i <= (3610 + desfase)){
  //       //   printf("metiendo el byte %d en forma %c\n", buffer[i], (char)buffer[i]);
  //       //   archivo1[i-(3599 + desfase)] = (char)buffer[i];
  //       // }
  //       // // tamaño
  //       // if (i >= (desfase + 3611) && i <= (desfase + 3614)){
  //       //   tamano1[i-(desfase + 3611)] = buffer[i];
  //       // }
  //     }
  //     desfase += 21;
  //     printf("nombre archivo1: %s\n ", archivo1);

  //   }
  //   //}
    
    


  //   printf("%u ", buffer[i]); // prints a series of bytes
  // //cr_mount(filename);

  // }
  // printf("\n");
  // printf("nombre: %s\n", bytes);

  

  // // tamano1[3] = (num>>24) & 0xFF;
  // // tamano1[2] = (num>>16) & 0xFF;
  // // tamano1[1] = (num>>8) & 0xFF;
  // // tamano1[0] = num & 0xFF;
  // // printf("tamaño archivo1: ");


    

};
  
 
  