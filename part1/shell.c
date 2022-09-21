#include "shell.h"

#define NUM_ARG 1000
#define BUF_SIZE 4096


static void die(const char *s, char **buf, char **comm, char *arr[])
{
	total_free(buf, comm, arr);
	printf("error: %s\n", s);
	exit(1);
}

int main(void)
{
	char	*buf = NULL;
	size_t	buffs;
	char	*arr[NUM_ARG] = {NULL};
	char	*comm = NULL;
	int	total_arg = 0;

	while (1) {
		total_arg = 0;

		printf("$");
		getline(&buf, &buffs, stdin);

		/*printf("Buffs Size: %d\n", buffs);
		 *printf("Buf Real size: %d\n", strlen(buf));
		 */

		if (buf == NULL)
			die("getline() memory Alloc failed", NULL, NULL, arr);

		if (buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = 0; /*replace newline with '\0'*/

		total_arg = tokenize(buf, &comm, arr);
		/*printf("Total Arg: %d\n", total_arg);
		 */

		if (total_arg > NUM_ARG) {
			printf("error: %s\n", "Accepting only 1000 Arguments!");
			total_free(&buf, &comm, arr);
			continue;
		} else if (total_arg == 0) {
			total_free(&buf, NULL, arr);
			continue;
		}

		/*printf("Comm: %s\n", comm);
		 *printf("Arr: %s\n", arr[0]);
		 */

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


int tokenize(char *buf, char **comm, char *arr[])
{
	int temp = 0;
	static const char s[2] = " ";
	char *token = NULL;
	int i = 0;

	for (temp = 0; temp < NUM_ARG; temp++)
		arr[temp] = NULL;

	token = strtok(buf, s);
	/*printf("Token: %d\n", sizeof(token));
	 */
	while (token != NULL) {
		i++;
		if (i > NUM_ARG)
			break;

		/*printf("Token: %s\n",token);
		 */
		arr[i-1] = (char *)malloc(strlen(token)*sizeof(char));
		strcpy(arr[i-1], token);

		if (i == 1) {
			/*printf("Entering!\n");*/
			*comm = (char *)malloc(strlen(token)*sizeof(char));
			strcpy(*comm, token);
			/*printf("comm in tokenize(): %s\n", *comm);*/
		}
		token = strtok(NULL, s);
	}

	/*printf("i: %d\n", i);*/
	return i;
}


void run_fork_processes(char **buf, char **comm, char *arr[])
{
	pid_t	pid;
	int	status;

	pid = fork();
	if (pid < 0)
		die("fork error", buf, comm, arr);
	else if (pid == 0) {
		/* child process*/
		execv(*comm, arr);
		die(strerror(errno), buf, comm, arr);
	} else {
		// parent process
		if (waitpid(pid, &status, 0) != pid)
			die("waitpid failed", buf, comm, arr);
	}
}


void total_free(char **buf, char **comm, char *arr[])
{
	int i = 0;

	if (comm != NULL) {
		free(*comm);
		*comm = NULL;
	}

	for (i = 0; i < NUM_ARG; i++) {
		if (arr[i] != NULL) {
			/*printf("Freeing arr[%d]\n", i);*/
			free(arr[i]);
			arr[i] = NULL;
		}
	}

	if (buf != NULL) {
		/*printf("Freeing buf\n");*/
		free(*buf);
		*buf = NULL;
	}
}
