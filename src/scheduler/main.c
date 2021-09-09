#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>

#include "../file_manager/manager.h"
#include "main.h"


int Q = 100;
char *input_name;
char *output_name;
InputFile *file;
int n_procesos;
char *nombre_proceso;
Process* array_procesos;


Process prueba = {
      .nombre = "prueba",
      .numero_fabrica = 0
    };
//int prueba[] = {3,6,2,4};
//int array_cpus[];

// file es data_in
int compareProcessByName(const void *v1, const void *v2)
{
    const Process *u1 = v1;
    const Process *u2 = v2;
    return strcmp(u1->nombre, u2->nombre);
}

int compareProcessByFabrica(const void *v1, const void *v2)
{
  const Process *p1 = v1;
  const Process *p2 = v2;
  if (p1->numero_fabrica < p2->numero_fabrica){
    return -1;
  }
  else if (p1->numero_fabrica > p2->numero_fabrica){
    return 1;
  }
  else if (p1->numero_fabrica == p2->numero_fabrica){
    return 0;
  }
  return 10;
}

int compareProcessByTiempo(const void *v1, const void *v2)
{
  const Process *p1 = v1;
  const Process *p2 = v2;
  if (p1->tiempo_init < p2->tiempo_init){
    return -1;
  }
  else if (p1->tiempo_init > p2->tiempo_init){
    return 1;
  }
  else if (p1->tiempo_init == p2->tiempo_init){
    return 0;
  }
  return 10;
}

int main(int argc, char **argv)
{
  //selectionSort(prueba, 4);
  //printf("prueba %i, %i, %i, %i \n", prueba[0], prueba[1], prueba[2], prueba[3]);
  printf("Hello T2!\n");
  // Recibiendo argumentos
  input_name = argv[1];
  char *filename = input_name;
  output_name = argv[2];
  if (argv[3]){
    Q = atoi(argv[3]);
  }
  file = read_file(filename);

  printf("Reading file of length %i:\n", file->len);
  n_procesos = file->len;

  array_procesos = calloc(n_procesos, sizeof(Process)); //HACER FREEEEEEEEEEEE

  // Creamos structs
  for (int i = 0; i < file->len; i++){
    char **line = file->lines[i];
    int int_bursts;
    int_bursts = atoi(line[3]);
    nombre_proceso = line[0];
    //Creamos proceso p
    Process nombre_proceso = {
      .pid = i,
      .nombre = line[0],
      .numero_fabrica = atoi(line[2]),
      .estado = "",
      .tiempo_init = atoi(line[1]),
      .bursts = atoi(line[3]),
      .cpu_io = calloc(((int_bursts*2) - 1), sizeof(int)) //HACER FREEEEEEEEEEEE
    };
    for (int i = 0; i < ((int_bursts*2) - 1); i++){
      int valor;
      valor = atoi(line[i + 4]);
      nombre_proceso.cpu_io[i] = valor;
      }
    array_procesos[i] = nombre_proceso;
    //printf("\tProcess %s from factory %s has init time of %s and %s bursts.\n", line[0], line[2], line[1], line[3]);
  }
  for (int i = 0; i < file->len; i++){
    printf("Proceso %s \n", array_procesos[i].nombre);
  }
   
  Cola cola_procesos = {
    .process = calloc(n_procesos, sizeof(Process)) //HACER FREEEEEEEEEEEE 
  };
  
  //Process array_aux[8] = {prueba, prueba, prueba, prueba, prueba, prueba, prueba, prueba};
  //Ordenar empates
  int cont = 0;
  for (int i = 0; i < n_procesos; i++){
    Process array_aux[8] = {prueba, prueba, prueba, prueba, prueba, prueba, prueba, prueba};
    //Process array_aux_fabrica[8] = {prueba, prueba, prueba, prueba, prueba, prueba, prueba, prueba};
    
    //array_aux[0] = array_procesos[i];
    int cont_0 = 1;
    for (int j = 0; j < n_procesos; j++){
      if ((array_procesos[i].tiempo_init == array_procesos[j].tiempo_init) && (array_procesos[i].nombre != array_procesos[j].nombre)){
          array_aux[0] = array_procesos[i];
          array_aux[cont_0] = array_procesos[j];
          cont_0 += 1;
      }
    }
    for (int k = 0; k < 8; k++){
      Process array_aux_nombre[2] = {prueba, prueba};
      //array_aux_nombre[0] = array_aux[k]; 
      for (int l = 0; l < 8; l++){
        if (array_aux[k].nombre != array_aux[l].nombre){
          if (array_aux[k].numero_fabrica == array_aux[l].numero_fabrica){
            array_aux_nombre[0] = array_aux[k];
            array_aux_nombre[1] = array_aux[l];
          }
        }
      }
      if ((array_aux_nombre[0].nombre != prueba.nombre) && (array_aux_nombre[1].nombre != prueba.nombre)){
        //printf("array_auxnombre[0], %s y array_auxnombre[1], %s \n", array_aux_nombre[0].nombre, array_aux_nombre[1].nombre);
        qsort(array_aux_nombre, 2, sizeof(Process), compareProcessByName);
        //printf("ORDENADO array_auxnombre[0], %s y array_auxnombre[1], %s \n", array_aux_nombre[0].nombre, array_aux_nombre[1].nombre);
        array_aux[cont] = array_aux_nombre[0];
        array_aux[cont + 1] = array_aux_nombre[1];
        cont += 2;
      }
    }
    qsort(array_aux, 8, sizeof(Process), compareProcessByFabrica);
    printf("array aux: %s, %s, %s, %s, %s, %s, %s, %s\n", array_aux[0].nombre, array_aux[1].nombre, array_aux[2].nombre,
    array_aux[3].nombre, array_aux[4].nombre, array_aux[5].nombre, array_aux[6].nombre, array_aux[7].nombre);
  }

  //Agregar procesos en orden a la cola
  qsort(array_procesos, n_procesos, sizeof(Process), compareProcessByTiempo);
  for (int i = 0; i < n_procesos; i++){
    cola_procesos.process[i] = array_procesos[i];
    printf("Proceso ordenado %s \n", array_procesos[i].nombre);
  }


  input_file_destroy(file);
  
} 
  