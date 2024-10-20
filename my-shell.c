/**
 * Program name: my-shell.c
 * Compile:      gcc -o my-shell my-shell.c
 * Run:          ./my-shell
 * Description:  Implements a command-line shell that can execute external 
 *               commands as well as handle built-in commands like 'cd', 'exit', 
 *               or 'chdir'.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_ARGS 128

// Function prototypes
char **parse(char *string, int *background);
void execute(char **args, int background);
int handle_builtin(char **args, int *running);

/**
 * This function takes a command string, splits it into separate arguements
 * based on spaces, and then returns an array of strings that are now fit 
 * to be used in execvp().
 * 
 * param string: The command string to parse.
 * param background: Pointer to an int to determine if the commant is to run
 * in the background.
 * return: An array of pointers to he parsed arguement string.
*/
char **parse(char *string, int *background) {
    char **args = malloc(MAX_ARGS * sizeof(char *));
    char *token = strtok(string, " ");
    int argCount = 0;
    *background = 0; // Set to 0 to initialize

    while (token != NULL) {
        if (strcmp(token, "#") == 0) {
            *background = 1;
            break; // Ignore any further tokens
        }

        args[argCount] = malloc((strlen(token) + 1) * sizeof(char));
        strcpy(args[argCount], token);
        argCount++;
        token = strtok(NULL, " ");
    }
    args[argCount] = NULL;
    return args;
}

/**
 * This function forks the current process. In the child process it will
 * attempt to execute the command specified by 'args' using execvp(). Parent
 * process will wait for child process to complete.
 * 
 * param args: An array of arguement strings. (Note: args[0] is the command
 * to be executed)
 * param background: Pointer to an int to determine if the commant is to run
 * in the background.
 * return: This function does not return a value.
*/
void execute(char **args, int background) {
    pid_t pid = fork();

    if (pid < 0) {
        // Fork has failed
        perror("fork");
        return;
    }

    if (pid == 0) {
        // Child process
        // (Parent process should not reach this line)
        execvp(args[0], args);
        exit(1);
    } else {
        // Parent process
        if (!background) {
            // Foreground process: wait for child to complete
            int status;
            waitpid(pid, &status, 0);
        } else {
            // Background process: do not wait
            printf("Started background process with PID %d\n", pid);
        }
    }
}

/**
 * This function checks if the command entered is a built-in command. If so,
 * the built-in command must be handled here rather than in a child process
 * as they apply directly to the state of the shell.
 * 
 * param args: An array of arguement strings. (Note: args[0] is the command
 * to be executed)
 * param running: A pointer to an integer that indicates whether the shell should
 * continue running. If running = 0, shell will terminate.
 * return: 
 * - '1' if a built-in command was handled
 * - '0' if the command is not a built-in command
 * - '-1' if an error occurred while handling 'chdir'
*/
int handle_builtin(char **args, int *running) {
    char *path;

    // Handle 'exit' and 'quit'
    if (strcmp(args[0], "exit") == 0 || strcmp(args[0], "quit") == 0) {
        *running = 0;
        return 1;
    }

    // Handle 'cd'
    if (strcmp(args[0], "cd") == 0 || strcmp(args[0], "chdir") == 0) {
        if (args[1] == NULL) {
            // No arguement provided after 'cd'. Default to home directory
            path = getenv("HOME");
        } else {
            // Arguement provided for path
            path = args[1];
        }

        if (chdir(path) != 0) {
            perror("cd");
            return -1;
        }
        return 1;
    }

    return 0; // Not a built-in command

}

/**
 * This function initializes the shell, enters a loop to continuously prompt the
 * user for the next command, parses and executes external and built-in commands.
 * Continues running until an exit command is entered, or an error has occurred.
*/
int main() {
    int running = 1;
    while (running) {
        // Setup for getLine()
        char* cmd = NULL;
        size_t len = 0;
        ssize_t read;
        
        // Prompt the user for input and check for errors 
        // resulting from the use of getLine()
        printf("marinelli: ");
        read = getline(&cmd, &len, stdin);
        if (read == -1) {
            perror("getLine");
            free(cmd);
            return 1;
        }

        // If the user presses enter without entering any 
        // input, continue from beginning of while loop
        if (cmd[0] == '\n') {
            free(cmd);
            continue;
        }

        // Remove newline character if necessary
        if (cmd[read-1] == '\n') {
            cmd[read-1] = '\0';
        }

        int background = 0;
        char **args = parse(cmd, &background);

        if (handle_builtin(args, &running)) {
            // Built-in command had been entered
            if (!running) {
                // Check if 'exit' or 'quit' was called, and if so,
                // clean up and exit the while loop
                free(args);
                free(cmd);
                break;
            }
        } else {
            // External command has been entered
            execute(args, background);
        }

    // Free allocated memory
    for (int i = 0; args[i] != NULL; i++) {            
        free(args[i]);
    }
    free(args);
    free(cmd);
    }
}