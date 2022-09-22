#include "shell2.h"

/* Maximum umber of arguments that can be given */
#define NUM_ARG 10000
/* This global MACRO will be used to allocate memory while reading.
 * Defined to assume the amount of bytes that can be given to buffer.
 * But the input size can be more than this. So, we wrote
 * fill_buffer() API to take care of these nuances by playing with pointers.
 */
#define BUF_SIZE 1024






/* die() function will kill the process after
 * freeing every dynamically allocated memory
 */
static void die(const char *s, char **buf, char **comm, char *arr[], char **fr4)
{
	if (write(1, "error: ", 7) != 7)
		write(2, "error: Problem in writing in STDOUT\n", 50);

	if (write(1, s, strlen(s)) != strlen(s))
		write(2, "error: Problem in writing to STDOUT\n", 50);

	if (write(1, "\n", 1) != 1)
		write(2, "error: Problem in writing to STDOUT\n", 50);

	total_free(buf, comm, arr, fr4);
	exit(1);
}


/* just_print() will be used to print the error,
 * but unlike die(), the API will not kill the process
 */
static void just_print(const char *s)
{
	if (write(1, "error: ", 7) != 7)
		write(2, "error: Problem in writing to STDOUT\n", 50);

	if (write(1, s, strlen(s)) != strlen(s))
		write(2, "error: Problem in writing to STDOUT\n", 50);

	if (write(1, "\n", 1) != 1)
		write(2, "error: Problem in writing to STDOUT\n", 50);

}


/* main() will run first
 * in every source code
 */
int main(void)
{
	/* Buffer to store the input */
	char    *buf = NULL;
	/* arr stores the arguments */
	char	*arr[NUM_ARG] = {NULL};
	/* comm stores the command - first argument */
	char	*comm = NULL;
	/* stores the number of args in the input */
	int	total_arg = 0;
	int	ref;

	ref = isatty(0);

	while (1) {
		total_arg = 0;
		comm = NULL;

		fill_buffer(&buf, arr, ref);

		if (buf == NULL)
			continue;

		/* replace newline with '\0' */
		if (buf[strlen(buf)-1] == '\n')
			buf[strlen(buf) - 1] = 0;

		total_arg = tokenize(&buf, &comm, arr);

		if (total_arg > NUM_ARG) {
			just_print("Accepting only 10000 Arguments!");
			total_free(&buf, &comm, arr, NULL);
			continue;
		} else if (total_arg == 0) {
			total_free(&buf, NULL, arr, NULL);
			continue;
		}

		if (!strcmp(comm, "exit")) {
			break;
		} else if ((!strcmp(comm, "cd")) && (total_arg > 2)) {
			just_print("Too many arguments");
			total_free(&buf, &comm, arr, NULL);
		} else if ((!strcmp(comm, "cd")) && (arr[1] != NULL)) {
			if (chdir(arr[1]))
				just_print(strerror(errno));
			total_free(&buf, &comm, arr, NULL);
		} else {
			run_fork_processes(&buf, &comm, arr);
			if (ref == 0) {
				total_free(&buf, &comm, arr, NULL);
				break;
			}
			total_free(&buf, &comm, arr, NULL);
		}

	}

	return 0;
}


/* fill_buffer() API will take care of reading the input - even if the
 * input buffer is of size more than BUFF_SIZE
 */
void fill_buffer(char **buf, char *arr[], int ref)
{
	/* Temporary buffer to store 4096 bytes of input */
	char    *tbuf = NULL;
	/* Temporary buffer to increase the size of buff */
	char	*B = NULL;
	/* used for allocating 4096 bytes */
	size_t	buffs = BUF_SIZE;
	/* iterator to read big size data */
	int	i = 0;
	/* Used while increasing the buf size */
	int	TotSiz = 0;
	int	off = 0;

	if (ref == 1) {
		if (write(1, "$", 1) != 1) {
			write(2, "error: Problem in writing to STDOUT\n", 50);
			exit(1);
		}
	}

	while (1) {
		_alloc(&tbuf, buffs, buf, NULL, arr, NULL);
		off = read(0, tbuf, buffs);
		if (off == -1)
			die(strerror(errno), buf, NULL, arr, NULL);
		else if (off == 0)
			break;

		if (i == 0) {
			_alloc(buf, buffs, NULL, NULL, arr, &tbuf);
			strcpy(*buf, tbuf);
		} else {
			TotSiz = (i)*buffs + strlen(tbuf);
			_alloc(&B, TotSiz, buf, NULL, arr, &tbuf);

			strcpy(B, *buf);
			strcat(B+(i*buffs), tbuf);

			free_memory(buf, strlen(*buf));
			_alloc(buf, TotSiz, &B, NULL, arr, &tbuf);

			strcpy(*buf, B);
			free_memory(&B, strlen(B));
		}

		if ((tbuf[strlen(tbuf)-1] == '\n')) {
			free_memory(&tbuf, buffs);
			break;
		}
		i++;
		free_memory(&tbuf, buffs);

	}
}


/* func tokenize() - will do the same purpose
 * as mentioned by its name. It will tokenize the input
 */
int tokenize(char **buf, char **comm, char *arr[])
{
	/* initialize arr using temp */
	int	temp = 0;
	/* To parse the input using delimieter SPACE */
	static const char s[2] = " ";
	/* To store tokens temporarily */
	char	*token = NULL;
	/* iterator - returned as the number of args in the input */
	int	i = 0;

	for (temp = 0; temp < NUM_ARG; temp++)
		arr[temp] = NULL;

	token = strtok(*buf, s);

	while (token != NULL) {
		i++;
		if (i > NUM_ARG)
			break;


		_alloc(&arr[i-1], strlen(token), buf, comm, arr, NULL);
		strcpy(arr[i-1], token);

		if (i == 1) {
			_alloc(comm, strlen(token), buf, comm, arr, NULL);
			strcpy(*comm, token);
		}
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
		die("fork error", buf, comm, arr, NULL);
	else if (pid == 0) {
		/* child process */
		execv(*comm, arr);
		die(strerror(errno), buf, comm, arr, NULL);
	} else {
		/* parent process */
		if (waitpid(pid, &status, 0) != pid)
			die("waitpid failed", buf, comm, arr, NULL);
	}
}


/* _alloc() is used to allocate memory of size N characters.
 * If allocation failed, we call die() with freeing up all allocated resources
 */
void _alloc(char **s, int N, char **fr1, char **fr2, char **fr3, char **fr4)
{
	int	SZ = N*sizeof(char);
	*s = mmap(NULL, SZ, PROT_READ|PROT_WRITE,
			MAP_PRIVATE|MAP_ANONYMOUS, 0, 0);
	if (*s == MAP_FAILED)
		die("Malloc Allocation failed", fr1, fr2, fr3, fr4);

}


/* free_memory() is used to free-up the memory
 * of string upto nC characters
 */
void free_memory(char **string, int nC)
{
	munmap(*string, nC*sizeof(char));
	*string = NULL;
}


/* Called when you want to free
 * all the dynamically allocated memory
 */
void total_free(char **buf, char **comm, char *arr[], char **fr4)
{
	int i = 0;

	if (comm != NULL)
		free_memory(comm, strlen(*comm));

	for (i = 0; i < NUM_ARG; i++) {
		if (arr[i] != NULL)
			free_memory(&arr[i], strlen(arr[i]));
	}

	if (buf != NULL)
		free_memory(buf, strlen(*buf));

	if (fr4 != NULL)
		free_memory(fr4, BUF_SIZE);
}
