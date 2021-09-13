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
Process* array_empates;
Process* array_total;
int posicion_nombre1;
int posicion_nombre2;
int tiempo = 0;

//"READY" = 5; "RUNNING = 6"; "WAITING = 7; "FINISHED" = 8;

Process prueba = {
      .nombre = "prueba",
      .numero_fabrica = 5
    };

void escribirArchivo(Process informacion[]){
  //for (int i = 0; i < n_procesos; i++){
  //  printf("nombre: %s, turnos: %i, interrupciones: %i, turnaround: %i, responsetime: %i, waitingtime : %i \n", 
  //  informacion[i].nombre, informacion[i].turnos_cpu, informacion[i].interrupciones, 
  //  informacion[i].turnaround, informacion[i].response, informacion[i].waiting);
  //  printf("output name %s\n", output_name);
  //}
  FILE *output = fopen(output_name, "w");
    for (int i = 0; i < n_procesos; i++){
      fprintf(output, "%s,%i,%i,%i,%i,%i\n", informacion[i].nombre, informacion[i].turnos_cpu, informacion[i].interrupciones,
      informacion[i].turnaround, informacion[i].response, informacion[i].waiting);
    }
  // Se cierra el archivo (si no hay leak)
  fclose(output);
}

int calcularQuantum (Cola cola_procesos, Process proceso_en_cpu, int largo_cola){
  int n_i = 0;
  int fabrica = proceso_en_cpu.numero_fabrica;
  int f_i[4] = {0,0,0,0};
  int f = 0;
  //printf("largo cola %i \n", largo_cola);
  for (int i = 0; i < largo_cola; i++){
    if (cola_procesos.process[i].numero_fabrica == fabrica){
      n_i += 1;
    }
    if (cola_procesos.process[i].numero_fabrica == 1){
      f_i[0] += 1;
    }
    else if (cola_procesos.process[i].numero_fabrica == 2){
      f_i[1] += 1;
    }
    else if (cola_procesos.process[i].numero_fabrica == 3){
      f_i[2] += 1;
    }
    else if (cola_procesos.process[i].numero_fabrica == 4){
      f_i[3] += 1;
    }
  }
  for (int j = 0; j < 4; j++){
    if (f_i[j] != 0){
      f += 1;
    }
  }
  int q  = (Q / (n_i * f));
  //printf("q = %i, n_i = %i, f = %i, Q = %i \n", q, n_i, f, Q);
  return q;
}
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
  
  Process informacion[n_procesos]; 

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
      .estado = 5,
      .tiempo_init = atoi(line[1]),
      .bursts = atoi(line[3]),
      .burst_actual = 0,
      .io_actual = 0,
      .cpu_io = calloc(((int_bursts*2) - 1), sizeof(int)), //HACER FREEEEEEEEEEEE
      .turnos_cpu = 0,
      .interrupciones = 0,
      .turnaround = 0,
      .response = 0,
      .waiting = 0
    };
    informacion[i] = nombre_proceso;
    //printf("proceso agregado a informacion nombre :%s, turno: %i\n", informacion[i].nombre, informacion[i].turnos_cpu);
    for (int i = 0; i < ((int_bursts*2) - 1); i++){
      int valor;
      valor = atoi(line[i + 4]);
      nombre_proceso.cpu_io[i] = valor;
      }
    array_procesos[i] = nombre_proceso;
    printf("\tProcess %s from factory %s has init time of %s and %s bursts.\n", line[0], line[2], line[1], line[3]);
  }
  //for (int i = 0; i < file->len; i++){
  //  printf("Proceso %s \n", array_procesos[i].nombre);
  //}
   
  Cola cola_procesos = {
    .process = calloc(n_procesos, sizeof(Process)) //HACER FREEEEEEEEEEEE 
  };
  //Ordenar empates
  //NO SE SI INICIALIZARLOS CON PRUEBA CAMBIA EN ALGO
  Process array_usado[8] = {prueba, prueba, prueba, prueba, prueba, prueba, prueba, prueba};
  int cont_usado = 0;

  array_empates = calloc(n_procesos, sizeof(Process)); //HACER FREEEEEEEEEEEE
  int indice_empate = 0;

  for (int i = 0; i < n_procesos; i++){
    Process array_aux[8] = {prueba, prueba, prueba, prueba, prueba, prueba, prueba, prueba};
    int continuar = 1;
    for (int m = 0; m < 8; m++){
        if (array_usado[m].nombre == array_procesos[i].nombre){
          continuar = 0;
        }
      }
    int cont_0 = 1;
    for (int j = 0; j < n_procesos; j++){
      if (continuar){
        if ((array_procesos[i].tiempo_init == array_procesos[j].tiempo_init) && 
        (array_procesos[i].nombre != array_procesos[j].nombre)){
            array_aux[0] = array_procesos[i];
            array_aux[cont_0] = array_procesos[j];
            cont_0 += 1;
            array_usado[cont_usado] = array_procesos[j];
            cont_usado += 1;
        }
      }
    }
   
    Process array_aux_nombre[2] = {prueba, prueba};
    for (int k = 0; k < 8; k++){
      //Process array_aux_nombre[2] = {prueba, prueba};
      //array_aux_nombre[0] = array_aux[k]; 
      for (int l = 0; l < 8; l++){
        if (array_aux[k].nombre != array_aux[l].nombre){
          if (array_aux[k].numero_fabrica == array_aux[l].numero_fabrica){
            posicion_nombre1 = k;
            posicion_nombre2 = l;
            //printf("k %i\n", k);
            //printf("l %i\n", l);
            array_aux_nombre[0] = array_aux[l];
            array_aux_nombre[1] = array_aux[k];
            if ((array_aux_nombre[0].nombre != prueba.nombre) && (array_aux_nombre[1].nombre != prueba.nombre)){
              qsort(array_aux_nombre, 2, sizeof(Process), compareProcessByName);
              //printf("array_aux_nombre %s, %s\n", array_aux_nombre[0].nombre, array_aux_nombre[1].nombre);
              array_aux[posicion_nombre2] = array_aux_nombre[0];
              array_aux[posicion_nombre1] = array_aux_nombre[1];
              //printf("array_aux DESPUES de ordenar nombre %s, %s, %s, %s\n", array_aux[0].nombre, array_aux[1].nombre, array_aux[2].nombre, array_aux[3].nombre);
              //cont += 2;
            }
            
          }
        }
      }
      //printf("creando array_aux_nombre: %s, %s\n", array_aux_nombre[0].nombre, array_aux_nombre[1].nombre);
      //printf("array_aux_nombre para el proceso %s: %s, %s\n", array_procesos[i].nombre, array_aux_nombre[0].nombre, array_aux_nombre[1].nombre); 
    }
    
    //printf("array_aux_nombre para el proceso %s: %s, %s\n", array_procesos[i].nombre, array_aux_nombre[0].nombre, array_aux_nombre[1].nombre);
    qsort(array_aux, 8, sizeof(Process), compareProcessByFabrica);
    //printf("array aux para el proceso %s: %s, %s, %s, %s, %s, %s, %s, %s\n", array_procesos[i].nombre, array_aux[0].nombre, array_aux[1].nombre, array_aux[2].nombre,
    // array_aux[3].nombre, array_aux[4].nombre, array_aux[5].nombre, array_aux[6].nombre, array_aux[7].nombre);
    for (int n = 0; n < 8; n++){
      if (array_aux[n].nombre != prueba.nombre){
        array_empates[indice_empate] = array_aux[n];
        indice_empate += 1;
      }
    }
  }

  array_total = calloc(n_procesos, sizeof(Process)); //HACER FREEEEEEEEEEEE
  for (int i = 0; i < (indice_empate); i ++){
    array_total[i] = array_empates[i];
  }

  int fin_empate = indice_empate;

  for (int i = 0; i < n_procesos; i++){
    int cont_array = 0;
    for (int j = 0; j < n_procesos; j ++){
      if (array_procesos[i].nombre != array_total[j].nombre){
        cont_array += 1;
      }
    }
    //printf("cont_array %i \n", cont_array);
    if (cont_array == n_procesos){
      array_total[fin_empate] = array_procesos[i];
      fin_empate += 1;
    }
  }

  qsort(array_total, n_procesos, sizeof(Process), compareProcessByTiempo);
  for (int i = 0; i < n_procesos; i++){
    //cola_procesos.process[i] = array_total[i];
    printf("Proceso ordenado %s \n", array_total[i].nombre);
    //printf("COlA lista %s\n", cola_procesos.process[i].nombre);
  }

  //Metemos a la cola los procesos que parten en 0
  //for (int i = 0; i < n_procesos; i++){
   // if (array_total[i].tiempo_init == 0){
    //  cola_procesos.process[i] = array_total[i];
    //  printf("[t = %i] El proceso %s ha sido creado.\n", tiempo, cola_procesos.process[i].nombre);
   // }
 // }

  int contador_fin = 0;
  int procesos_finalizados = 0;
  //int contador_fin = n_procesos;
  int cpu_ocupada = 0; // 0 = no ocupada, 1 = ocupada
  int quantum = 0;
  Process proceso_en_cpu;
  //printf("contador_fin %i\n", contador_fin);
// waiting pasar a readyg
  //while  (contador_fin > 0){
  while  (procesos_finalizados < n_procesos){
    //printf("tiempo %i \n", tiempo);
    //Revisamos los procesos en WAITING y restamos 1 segundo de su restante
    for (int l = 0; l < contador_fin; l++){
      if (cola_procesos.process[l].estado == 7){ 
        cola_procesos.process[l].tiempo_restante_io -= 1;
        
        // Revisamos si algún proceso en WAITING debe pasar a READY
        if (cola_procesos.process[l].tiempo_restante_io == 0){
          cola_procesos.process[l].estado = 5; //READY
          printf("[t = %i] El proceso %s ha pasado a estado READY.\n", tiempo, cola_procesos.process[l].nombre);
        }
      }
    }

    if (cpu_ocupada == 1){
      proceso_en_cpu.tiempo_restante_burst -= 1;
      //printf("%i, %i, %i \n", quantum, proceso_en_cpu.cpu_io[(proceso_en_cpu.burst_actual * 2) - 2], proceso_en_cpu.tiempo_restante_burst);
      if ((proceso_en_cpu.tiempo_restante_burst == 0) && (proceso_en_cpu.burst_actual != proceso_en_cpu.bursts)){
        proceso_en_cpu.io_actual += 1;
        proceso_en_cpu.tiempo_restante_io = proceso_en_cpu.cpu_io[(proceso_en_cpu.io_actual * 2) - 1];
        cpu_ocupada = 0;
        proceso_en_cpu.estado = 7; // WAITING

        if (quantum == (proceso_en_cpu.cpu_io[(proceso_en_cpu.burst_actual * 2) - 2] - proceso_en_cpu.tiempo_restante_burst)){
          proceso_en_cpu.interrupciones += 1;
        }

        //MANDARLO QUE HAY QUE AL FINAL DE LA COLA
        cola_procesos.process[contador_fin] = proceso_en_cpu;
        contador_fin += 1;
        for (int p = 0; p < contador_fin; p++){
          if (proceso_en_cpu.nombre == cola_procesos.process[p].nombre){
            cola_procesos.process[p].estado = proceso_en_cpu.estado;
          }
        }
        //printf("estado en cpu %i \n", proceso_en_cpu.estado);
        printf("[t = %i] El proceso %s ha pasado a estado WAITING.\n", tiempo, proceso_en_cpu.nombre);
      }
      else if ((proceso_en_cpu.burst_actual == proceso_en_cpu.bursts) && (proceso_en_cpu.tiempo_restante_burst == 0)){
        cpu_ocupada = 0;
        proceso_en_cpu.estado = 8; //FINISHED
        proceso_en_cpu.turnaround = tiempo - proceso_en_cpu.tiempo_init;

        if (quantum == (proceso_en_cpu.cpu_io[(proceso_en_cpu.burst_actual * 2) - 2] - proceso_en_cpu.tiempo_restante_burst)){
          proceso_en_cpu.interrupciones += 1;
        }
        
        printf("[t = %i] El proceso %s ha pasado a estado FINISHED.\n", tiempo, proceso_en_cpu.nombre);
        for (int p = 0; p < contador_fin; p++){
          if (proceso_en_cpu.nombre == cola_procesos.process[p].nombre){
            //cola_procesos.process[p].estado = proceso_en_cpu.estado;
            cola_procesos.process[p] = proceso_en_cpu;
          }
        }
        for (int p = 0; p < n_procesos; p++){
          if (proceso_en_cpu.nombre == informacion[p].nombre){
            informacion[p] = proceso_en_cpu;
          }
        }
        procesos_finalizados += 1;
        //contador_fin -= 1;
        //printf("termine \n");
        // falta sacarlo de la cola
      }
      else if (quantum == (proceso_en_cpu.cpu_io[(proceso_en_cpu.burst_actual * 2) - 2] - proceso_en_cpu.tiempo_restante_burst)){
        //printf("Se acabo por quantum \n");
        cpu_ocupada = 0;
        proceso_en_cpu.estado = 5; //READY
        proceso_en_cpu.interrupciones += 1;
        printf("[t = %i] El proceso %s ha pasado a estado READY.\n", tiempo, proceso_en_cpu.nombre);
        proceso_en_cpu.cpu_io[(proceso_en_cpu.burst_actual * 2) - 2] = proceso_en_cpu.tiempo_restante_burst;
        cola_procesos.process[contador_fin] = proceso_en_cpu;
        contador_fin += 1;
        for (int p = 0; p < contador_fin; p++){
          if (proceso_en_cpu.nombre == cola_procesos.process[p].nombre){
            cola_procesos.process[p] = proceso_en_cpu;
            //cola_procesos.process[p].estado = proceso_en_cpu.estado;
            //cola_procesos.process[p].cpu_io = proceso_en_cpu.cpu_io;
          }
        }
        for (int p = 0; p < n_procesos; p++){
          if (proceso_en_cpu.nombre == informacion[p].nombre){
            informacion[p] = proceso_en_cpu;
          }
        }
        // proceso_en_cpu.tiempo_restante_burst = ????
      }
      else {
        cpu_ocupada = 1;
        proceso_en_cpu.estado = 6; //RUNNING
      }
    }

    // Revisar tiempo general, ver si hay alguno nuevo que crear y meter en la cola
    for (int k = 0; k < n_procesos; k++){
      if (array_total[k].tiempo_init == tiempo){
        //cola_procesos.process[contador_fin] = array_total[k];
        cola_procesos.process[contador_fin] = array_total[k];
        contador_fin += 1;
        printf("[t = %i] El proceso %s ha sido creado.\n", tiempo, array_total[k].nombre);
        //for (int j = 0; j < (contador_fin); j++){
         //   printf("COlA al agregar  %s\n", cola_procesos.process[j].nombre);
         // }
      }
    }

    if (cpu_ocupada == 0){
      //printf("entre aca despuesde terminar \n");
      for (int i = 0; i < contador_fin; i++){
        //printf("estado de la cola %i \n", cola_procesos.process[i].estado );
        if (cola_procesos.process[i].estado == 5){
          //printf("cola_procesos.process[i].nombre  %s\n", cola_procesos.process[i].nombre);
          cpu_ocupada = 1;
          proceso_en_cpu = cola_procesos.process[i];
          if (proceso_en_cpu.tiempo_restante_burst == 0){
            proceso_en_cpu.burst_actual += 1;
            proceso_en_cpu.tiempo_restante_burst = proceso_en_cpu.cpu_io[(proceso_en_cpu.burst_actual * 2) - 2];
          }
          //CREAR QUANTUM
          quantum = calcularQuantum(cola_procesos, proceso_en_cpu, contador_fin);
          proceso_en_cpu.estado = 6; // RUNNING
          proceso_en_cpu.turnos_cpu += 1;

          if (proceso_en_cpu.turnos_cpu == 1){
            proceso_en_cpu.response = tiempo - proceso_en_cpu.tiempo_init;
          } 

          for (int p = 0; p < contador_fin; p++){
            if (proceso_en_cpu.nombre == cola_procesos.process[p].nombre){
              cola_procesos.process[p] = proceso_en_cpu;
              //printf("turnos en cola %s, %i \n", cola_procesos.process[p].nombre, cola_procesos.process[p].turnos_cpu);
              for (int j = 0; j < n_procesos; j++){
                if (cola_procesos.process[p].nombre == informacion[j].nombre){
                  informacion[j] = proceso_en_cpu;
                  //printf("turnos en informacion %s, %i \n", informacion[j].nombre, informacion[j].turnos_cpu);
                }
              }
            }
          }

               
          
          printf("[t = %i] El proceso %s ha pasado a estado RUNNING.\n", tiempo, proceso_en_cpu.nombre);
          // ACTUALIZAR ORDEN COLA
          for (int j = 0; j < contador_fin; j++){
            if (j > i){
              cola_procesos.process[j - 1] = cola_procesos.process[j];
            }
          }
          contador_fin -= 1;
          break;
          }
          
        }
        
    }
    //ACTUALIZAR ESTADISTICAS
    for (int p = 0; p < contador_fin; p++){
      if ((cola_procesos.process[p].estado == 5) || (cola_procesos.process[p].estado == 7)){
        for (int q = 0; q < n_procesos; q++){
          if (cola_procesos.process[p].nombre == informacion[q].nombre){
            cola_procesos.process[p].waiting += 1;
            informacion[q] = cola_procesos.process[p];
          }
        }
      }
    }
    
    tiempo += 1;
  }
  

//for (int i = 0; i < n_procesos; i++)
//{
//  printf("en cola nombre: %s, turnos: %i \n", cola_procesos.process[i].nombre, cola_procesos.process[i].turnos_cpu);
//}

escribirArchivo(informacion);
for (int i = 0; i < n_procesos; i++){
  free(array_total[i].cpu_io);
}
free(cola_procesos.process);
free(array_procesos);
free(array_empates);
free(array_total);
input_file_destroy(file);
}
  
 
  