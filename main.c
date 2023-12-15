#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include "main.h"

#define MAX_LINE 128
#define MAX_ARGS 32

int main(int argc, char *argv[]) {
  char line[MAX_LINE];
  char *args[MAX_ARGS];
  char *path_env = getenv("PATH");
  int ret;

  // Display prompt
  printf("> ");

  while (fgets(line, sizeof(line), stdin)) {
    // Remove trailing newline
    strtok(line, "\n");

    // Check for end of file
    if (feof(stdin)) {
      printf("\n");
      break;
    }

    // Check for built-in commands
    if ((ret = execute_builtin(line)) != 0) {
      if (ret == -1) {
        fprintf(stderr, "Error: unknown command\n");
      }
      continue;
    }

    // Parse arguments
    int arg_count = 0;
    char *token = strtok(line, " ");
    while (token && arg_count < MAX_ARGS - 1) {
      args[arg_count++] = token;
      token = strtok(NULL, " ");
    }

    // Check for exceeding argument limit
    if (token) {
      printf("Error: too many arguments\n");
      continue;
    }

    args[arg_count] = NULL;

    // Check if command exists in the current directory
    struct stat statbuf;
    if (stat(args[0], &statbuf) == 0 && S_ISREG(statbuf.st_mode)) {
      // Execute command directly
      ret = fork();
      if (ret == -1) {
        perror("fork");
        exit(1);
      }
    } else {
      // Search for command in the PATH
      char *path_token = strtok(path_env, ":");
      while (path_token) {
        char full_path[MAX_LINE];
        sprintf(full_path, "%s/%s", path_token, args[0]);
        if (stat(full_path, &statbuf) == 0 && S_ISREG(statbuf.st_mode)) {
          // Found command, update args[0] and execute
          args[0] = full_path;
          ret = fork();
          if (ret == -1) {
            perror("fork");
            exit(1);
          }
          break;
        }
        path_token = strtok(NULL, ":");
      }
    }

    if (ret == -1) {
      perror("fork");
      exit(1);
    }

    if (ret == 0) {
      // Child process
      execvp(args[0], args);
      perror("execvp");
      exit(EXEC_ERROR);
    } else {
      // Parent process
      wait(NULL);
    }

    // Display prompt again
    printf("> ");
  }

  return 0;
}	
