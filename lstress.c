#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <errno.h>
#include <math.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

int help(char *appName)
{
  printf("Help is not yet accessible :(\n");
    
  exit(0);
  
}

float work_gain = 1, sleep_gain = 1;
unsigned int display = 0;

int worker(void)
{
  
  long long unsigned int i, op_limit;
  unsigned int sleep_time, id = (unsigned int) getpid();
  
  srand(id);
  if (display)
    printf("Worker %u started\n", id);
  
  while (1) {
    
    op_limit = ((float)rand())/((float)RAND_MAX)*work_gain * 1e6;
    sleep_time = ((float)rand())/((float)RAND_MAX)*sleep_gain * 4e3;
    
    if (display)
      printf("%1u: %lluop/%uus \n",id,op_limit,sleep_time);
    
    for (i = 0; i < op_limit; i++)
      sqrt(rand());
    
    usleep(sleep_time);
    
  }
  
  return 0;
}

int main(int argc, char *argv[])
{
  
  char *program     = strdup(argv[0]), *appName=basename(program);
  int rc = 0, pid, status;
  int labels        = 0;
  int next_arg      = 1;
  unsigned int cpu_wrokers = 0, children = 0;;
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
      case 'c':
        if (next_arg + 1 < argc) {
          if (!sscanf(argv[ next_arg + 1 ],"%u",&cpu_wrokers)) {
            printf("Invalid workers number...\n");
            rc = -EINVAL;
            goto invalid_argument;
          }
        } else {
          printf("Invalid workers number...\n");
          rc = -EINVAL;
          goto invalid_argument;
        }
        next_arg += 2;
        break;
      case 'v':
        display = 1;
        next_arg += 1;
        break;
      case 'w':
        if (next_arg + 1 < argc) {
          if (!sscanf(argv[ next_arg + 1 ],"%f",&work_gain)) {
            printf("Invalid work gain value...\n");
            rc = -EINVAL;
            goto invalid_argument;
          }
        } else {
          printf("Invalid work gain value...\n");
          rc = -EINVAL;
          goto invalid_argument;
        }
        next_arg += 2;
        break;
      case 's':
        if (next_arg + 1 < argc) {
          if (!sscanf(argv[ next_arg + 1 ],"%f",&sleep_gain)) {
            printf("Invalid sleep gain value...\n");
            rc = -EINVAL;
            goto invalid_argument;
          }
        } else {
          printf("Invalid sleep gain value...\n");
          rc = -EINVAL;
          goto invalid_argument;
        }
        next_arg += 2;
        break;
      default:
        printf("Unknown argument %c, run ./%s -h for help.\n",option,appName);
        exit(0);
        break;
    }
    
  };
  
  if (cpu_wrokers > 0) {
    
    printf("Starting %d cpu workers.\n", cpu_wrokers);
    
    while (cpu_wrokers > 0) {
      switch (pid = fork()) {
        case 0:            /* child */
          exit (worker());
        case -1:           /* error */
          fprintf(stderr, "fork failed: %s\n", strerror(errno));
          break;
        default:           /* parent */
          ++children;
      }
      --cpu_wrokers;
    }
    
    while (children) {
      if ((pid = wait (&status)) > 0) {
        --children;
        
        if (WIFEXITED (status)) {
          if (WEXITSTATUS (status) != 0) {
            signal(SIGUSR1, SIG_IGN);
            kill(-getpid(), SIGUSR1);
          }
        } else if (WIFSIGNALED (status)) {
          if (WTERMSIG (status) != SIGALRM && WTERMSIG (status) != SIGUSR1) {
            signal(SIGUSR1, SIG_IGN);
            kill(-getpid(), SIGUSR1);
          }
        }
        
      } else {
        fprintf(stderr, "error waiting for worker: %s\n", strerror(errno));
        break;
      }
    }
    
  }
  
  invalid_argument:
    free(program);
    return rc;
}