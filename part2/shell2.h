#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>


void fill_buffer(char **buf, char *arr[]);
int tokenize(char **buf, char **comm, char *arr[]);
void run_fork_processes(char **buf, char **comm, char *arr[]);
void alloc_mem(char **s, int N, char **fr1, char **fr2, char **fr3, char **fr4);
void free_memory(char **string, int nC);
void total_free(char **buf, char **comm, char *arr[], char **fr4);
