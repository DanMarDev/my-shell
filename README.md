## Description

my-shell.c is a simple command-line shell implemented in C. It supports the execution of a few basic built-in shell commands, as well as the execution of any external commands. This project developed for an Operating Systems course at the University of Scranton taught by Dr. Yaodong Bi.

## Features
- **Built-in commands**:
  - `cd` or `chdir`: Changes the current working directory. If no path is provided, it will default to home directory.
  - `exit` or `quit`: Terminates the shell.
- **External Commands**: Executes any external programs available in the current working directory of the shell or any external programs available in the system's PATH (such as `ls` or `pwd`).
- **Background Execution**: Commands can be executed in the background rather than having the shell wait for the process created to be completed. Background execution can be enabled on a per-command basis by appending `#` to the end of the command. 

## Compilation/Usage
- Compile shell:
```bash
gcc -o my-shell my-shell.c
```
- Run shell:
```bash
./my-shell
```

## Un-implemented Features
 - Support for executing commands in parallel