#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>

#include "../file_manager/manager.h"
#include "../crms_API/crms_API.h"


char *input_name;


int main(int argc, char **argv)
{
  printf("hola");
  input_name = argv[1];
  char *filename = input_name;
  cr_mount(filename);

};
  
 
  