#ifndef SHELL_H
#define SHELL_H

// Function prototypes
char *getline(FILE *stream);
void free_line(char *line);

// Built-in commands
int execute_builtin(char *command);
// Possible built-in commands (add more as needed)
#define EXIT_BUILTIN "exit"
#define ENV_BUILTIN "env"

// Error codes
#define EXEC_ERROR 127
#define FORK_ERROR -1

#endif /* SHELL_H */
