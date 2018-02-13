#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

int help(char *appName)
{
  printf("Help is not yet accessible :(\n");
    
  exit(0);
  
}

int main(int argc, char *argv[])
{
  
  char *program     = strdup(argv[0]), *appName=basename(program);
  int labels        = 0;
  int next_arg      = 1;
  char option;
  
  // parameters handling
  while (next_arg < argc && argv[next_arg][0] == '-') {
    
    option = argv[next_arg][1];
    
    switch (option) {
      case 'h':
        help(appName);
        // this is actually unnecessary. help function terminates application.
        next_arg += 1;
        break;
      default:
        printf("Unknown modifier %c, run ./%s -h for help.\n",option,appName);
        exit(0);
        break;
    }
    
  };
  
  return 0;
  
}