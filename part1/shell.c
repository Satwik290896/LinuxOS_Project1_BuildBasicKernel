#include "shell.h"

/* Maximum umber of arguments that can be given */
#define NUM_ARG 10000
/* This global MACRO will not be used anywhere.
 * Defined to assume the amount of bytes that can be given to buffer
 */
#define BUF_SIZE 4096




/* die() function will kill the process after
 * freeing every dynamically allocated memory
 */
static void die(const char *s, char **buf, char **comm, char *arr[])
{
	total_free(buf, comm, arr);
	printf("error: %s\n", s);
	exit(1);
}


/* main() will run first
 * in every source code
 */
int main(void)
{
	/* Buffer to store the input */
	char	*buf = NULL;
	/* buffs passed to getline() which stores
	 * number of allocated bytes for buf
	 */
	size_t	buffs;
	/* arr stores the arguments */
	char	*arr[NUM_ARG] = {NULL};
	/* comm stores the command - first argument */
	char	*comm = NULL;
	/* stores the number of args in the input */
	int	total_arg = 0;

	while (1) {
		total_arg = 0;

		printf("$");
		getline(&buf, &buffs, stdin);

		if (buf == NULL)
			die("getline() memory Alloc failed", NULL, NULL, arr);

		if (buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = 0; /*replace newline with '\0'*/

		total_arg = tokenize(&buf, &comm, arr);

		if (total_arg > NUM_ARG) {
			printf("error: %s\n",
				"Accepting only 10000 Arguments!");
			total_free(&buf, &comm, arr);
			continue;
		} else if (total_arg == 0) {
			total_free(&buf, NULL, arr);
			continue;
		}

		if (!strcmp(comm, "exit")) {
			total_free(&buf, &comm, arr);
			break;
		} else if ((!strcmp(comm, "cd")) && (total_arg > 2)) {
			printf("error: %s\n", "Too many arguments");
			total_free(&buf, &comm, arr);
		} else if ((!strcmp(comm, "cd")) && (arr[1] != NULL)) {
			if (chdir(arr[1]))
				printf("error: %s\n", strerror(errno));
			total_free(&buf, &comm, arr);
		} else {
			run_fork_processes(&buf, &comm, arr);
			total_free(&buf, &comm, arr);
		}
	}

	return 0;
}


/* func tokenize() - will do the same purpose
 * as mentioned by its name. It will tokenize the input
 */
int tokenize(char **buf, char **comm, char *arr[])
{
	/* initialize arr using temp */
	int temp = 0;
	/* To parse the input using delimieter SPACE */
	static const char s[2] = " ";
	/* To store tokens temporarily */
	char *token = NULL;
	/* iterator - returned as the number of args in the input */
	int i = 0;

	for (temp = 0; temp < NUM_ARG; temp++)
		arr[temp] = NULL;

	token = strtok(*buf, s);

	if (token != NULL) {
		*comm = (char *)malloc(strlen(token)*sizeof(char));
		if (comm == NULL)
			die("Malloc allocation failed", buf, comm, arr);
		strcpy(*comm, token);
	}

	while (token != NULL) {
		i++;
		if (i > NUM_ARG)
			break;

		arr[i-1] = (char *)malloc(strlen(token)*sizeof(char));
		if (arr[i-1] == NULL)
			die("Malloc allocation for arr failed", buf, comm, arr);

		strcpy(arr[i-1], token);

		token = strtok(NULL, s);
	}

	return i;
}


/* Create a child process and execute
 * the command given by the
 * input - stored in the buf, arr and comm
 */
void run_fork_processes(char **buf, char **comm, char *arr[])
{
	/* stores the pid of the fork() */
	pid_t	pid;
	/* passed as the argument to waitpid() */
	int	status;

	pid = fork();
	if (pid < 0)
		die("fork error", buf, comm, arr);
	else if (pid == 0) {
		/* child process */
		execv(*comm, arr);
		die(strerror(errno), buf, comm, arr);
	} else {
		/* parent process */
		if (waitpid(pid, &status, 0) != pid)
			die("waitpid failed", buf, comm, arr);
	}
}


/* Called when you want to free
 * all the dynamically allocated memory
 */
void total_free(char **buf, char **comm, char *arr[])
{
	/* iterator for arr */
	int i = 0;

	if (comm != NULL) {
		free(*comm);
		*comm = NULL;
	}

	for (i = 0; i < NUM_ARG; i++) {
		if (arr[i] != NULL) {
			free(arr[i]);
			arr[i] = NULL;
		}
	}

	if (buf != NULL) {
		free(*buf);
		*buf = NULL;
	}
}
