#include "shell2.h"

#define NUM_ARG 1000
#define BUF_SIZE 4096


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

static void just_print(const char *s)
{
	if (write(1, "error: ", 7) != 7)
		write(2, "error: Problem in writing to STDOUT\n", 50);

	if (write(1, s, strlen(s)) != strlen(s))
		write(2, "error: Problem in writing to STDOUT\n", 50);

	if (write(1, "\n", 1) != 1)
		write(2, "error: Problem in writing to STDOUT\n", 50);

}


int main(void)
{
	char    *buf = NULL;
	char	*arr[NUM_ARG] = {NULL};
	char	*comm = NULL;
	int	total_arg = 0;

	while (1) {
		total_arg = 0;
		comm = NULL;

		fill_buffer(&buf, arr);

		/* replace newline with '\0' */
		if (buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = 0;

		total_arg = tokenize(&buf, &comm, arr);

		if (total_arg > NUM_ARG) {
			just_print("Accepting only 1000 Arguments!");
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
			total_free(&buf, &comm, arr, NULL);
		}
	}

	return 0;
}


void fill_buffer(char **buf, char *arr[])
{
	char    *tbuf = NULL;
	char	*B = NULL;
	size_t	buffs = BUF_SIZE;
	int	i = 0;
	int	TotSiz = 0;

	if (write(1, "$", 1) != 1) {
		write(2, "error: Problem in writing to STDOUT\n", 50);
		exit(1);
	}

	while (1) {
		alloc_mem(&tbuf, buffs, buf, NULL, arr, NULL);
		read(0, tbuf, buffs);

		if (i == 0) {
			alloc_mem(buf, buffs, NULL, NULL, arr, &tbuf);
			strcpy(*buf, tbuf);
		} else {
			TotSiz = (i)*buffs + strlen(tbuf);
			alloc_mem(&B, TotSiz, buf, NULL, arr, &tbuf);

			strcpy(B, *buf);
			strcat(B+(i*buffs), tbuf);

			free_memory(buf, strlen(*buf));
			alloc_mem(buf, TotSiz, &B, NULL, arr, &tbuf);

			strcpy(*buf, B);
			free_memory(&B, strlen(B));
		}

		if (tbuf[strlen(tbuf)-1] == '\n') {
			free_memory(&tbuf, buffs);
			break;
		}
		i++;
		free_memory(&tbuf, buffs);

	}
}

int tokenize(char **buf, char **comm, char *arr[])
{
	int	temp = 0;
	static const char s[2] = " ";
	char	*token = NULL;
	int	i = 0;

	for (temp = 0; temp < NUM_ARG; temp++)
		arr[temp] = NULL;

	token = strtok(*buf, s);

	while (token != NULL) {
		i++;
		if (i > NUM_ARG)
			break;


		alloc_mem(&arr[i-1], strlen(token), buf, comm, arr, NULL);
		strcpy(arr[i-1], token);

		if (i == 1) {
			alloc_mem(comm, strlen(token), buf, comm, arr, NULL);
			strcpy(*comm, token);
		}
		token = strtok(NULL, s);
	}

	return i;
}

void run_fork_processes(char **buf, char **comm, char *arr[])
{
	pid_t	pid;
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

void alloc_mem(char **s, int N, char **fr1, char **fr2, char **fr3, char **fr4)
{
	int SZ = N*sizeof(char);
	*s = mmap(NULL, SZ, PROT_READ|PROT_WRITE,
			MAP_PRIVATE|MAP_ANONYMOUS, 0, 0);
	if (*s == MAP_FAILED)
		die("Malloc Allocation failed", fr1, fr2, fr3, fr4);

}

void free_memory(char **string, int nC)
{
	munmap(*string, nC*sizeof(char));
	*string = NULL;
}

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
