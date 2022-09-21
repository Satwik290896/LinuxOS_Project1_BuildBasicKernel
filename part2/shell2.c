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

#define NUM_ARG 1000
#define BUF_SIZE 4096


void fill_buffer(char **buf, char *arr[]);
int tokenize(char *buf, char **comm, char *arr[]);
void run_fork_processes(char **buf, char **comm, char *arr[]);
void total_free(char **buf, char **comm, char *arr[]);

static void die(const char *s, char **buf, char **comm, char *arr[])
{
	if(write(1, "error: ", 7) != 7)
		write(2, "error: Problem in writing in STDOUT\n",50);

	if(write(1,s,strlen(s)) != strlen(s))
		write(2, "error: Problem in writing to STDOUT\n", 50);

	if(write(1,"\n",1) != 1)
		write(2, "error: Problem in writing to STDOUT\n", 50);

	total_free(buf, comm, arr);
	exit(1); 
}

static void just_print(const char *s)
{
	if(write(1, "error: ", 7) != 7)
		write(2, "error: Problem in writing to STDOUT\n", 50);
		
	if(write(1,s,strlen(s)) != strlen(s))
		write(2,"error: Problem in writing to STDOUT\n", 50);

	if(write(1,"\n",1) != 1)
		write(2, "error: Pronlem in writing to STDOUT\n", 50);
    
}


int main(void)
{
	char    *buf = NULL;
	pid_t	pid;
	char	*arr[NUM_ARG] = {NULL};
	char	*token = NULL;
	char 	*comm = NULL;
	int total_arg = 0;

	while (1){
		total_arg = 0;
	 	token = NULL;
	 	comm = NULL;
		
		fill_buffer(&buf, arr);
		
        	if (buf[strlen(buf) - 1] == '\n')
            		buf[strlen(buf) - 1] = 0; // replace newline with '\0'
		
		printf("buf: %s\n", buf);
		total_arg = tokenize(buf, &comm, arr);

		if (total_arg > NUM_ARG) {
			just_print("Accepting only 1000 Arguments!");
			total_free(&buf, &comm, arr);
			continue;
		} else if (total_arg == 0) {
			total_free(&buf, NULL, arr);
			continue;
		}
        
          	if(!strcmp(comm,"exit")){
	    		break;
	 	} else if ((!strcmp(comm, "cd")) && (total_arg > 2)) {
			just_print("Too many arguments");
			total_free(&buf, &comm, arr);
		} else if ((!strcmp(comm, "cd")) && (arr[1] != NULL)) {
			if (chdir(arr[1]))
				just_print(strerror(errno));
			total_free(&buf, &comm, arr);
		} else {
			run_fork_processes(&buf, &comm, arr);
			total_free(&buf, &comm, arr);
	  	}
    	}
    	
    	
	return 0;
}


void fill_buffer(char **buf, char *arr[])
{
	char    *tbuf = NULL;
	char	*B = NULL;
	size_t	buffs = BUF_SIZE;
	int i = 0;
	
	if(write(1,"$",1) != 1) {
	  	write(2, "error: Problem in writing to STDOUT\n",50);
	   	exit(1);
	}

	while(1)
	{
		tbuf = mmap(NULL, buffs*sizeof(char), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0, 0);
		if(tbuf == MAP_FAILED)
	   		die("Malloc Allocation failed(tbuf)", buf, NULL, arr);
	   		
	 	read(0, tbuf, buffs);

		printf("i: %d\n", i);
		if(i == 0){
		        *buf = mmap(NULL, buffs*sizeof(char), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0, 0);
		        if(*buf == MAP_FAILED){
		        	munmap(tbuf,buffs*sizeof(char));
	   			die("Malloc(buf) Allocation failed", NULL, NULL, arr);
	   		}
			strcpy(*buf,tbuf);
		} else {
			B = mmap(NULL, (i+1)*buffs*sizeof(char), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0, 0);
			if(B == MAP_FAILED){
				munmap(tbuf,buffs*sizeof(char));
	   			die("Malloc(B) Allocation failed", buf, NULL, arr);
	   		}
	   		strcpy(B,*buf);
			strcat(B+(i*buffs),tbuf);
			munmap(*buf,strlen(*buf)*sizeof(char));
			*buf = mmap(NULL, (i+1)*buffs*sizeof(char), PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, 0, 0);
			if(*buf == MAP_FAILED){
				munmap(tbuf,buffs*sizeof(char));
	  			die("Malloc(buf) Allocation failed", &B, NULL, arr);
	   		}
			strcpy(*buf,B);
			munmap(B,strlen(B)*sizeof(char));
		}
			
		if(tbuf[strlen(tbuf)-1] == '\n'){
			munmap(tbuf,buffs*sizeof(char));
			break;
		}
		i++;
		munmap(tbuf,buffs*sizeof(char));
		
	}	
}

int tokenize(char *buf, char **comm, char *arr[])
{
	int temp = 0;
	const char s[2] = " ";
	char *token = NULL;
	int i = 0;

	for (temp = 0; temp < NUM_ARG; temp++)
		arr[temp] = NULL;

	token = strtok(buf, s);

	while (token != NULL) {
		i++;
		if (i > NUM_ARG)
			break;


		arr[i-1] = (char *)malloc(strlen(token)*sizeof(char));
		strcpy(arr[i-1], token);

		if (i == 1) {
			*comm = (char *)malloc(strlen(token)*sizeof(char));
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

void total_free(char **buf, char **comm, char *arr[])
{
	int i = 0;

	if (comm != NULL) {
		munmap(*comm, strlen(*comm)*sizeof(char));
		*comm = NULL;
	}

	for (i = 0; i < NUM_ARG; i++) {
		if (arr[i] != NULL) {
			munmap(arr[i], strlen(arr[i])*sizeof(char));
			arr[i] = NULL;
		}
	}

	if (buf != NULL) {
		munmap(*buf, strlen(*buf)*sizeof(char));
		*buf = NULL;
	}
}
