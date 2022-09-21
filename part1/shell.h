#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

int tokenize(char *buf, char **comm, char *arr[]);
void run_fork_processes(char **buf, char **comm, char *arr[]);
void total_free(char **buf, char **comm, char *arr[]);
