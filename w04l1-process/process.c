#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
	pid_t pid = -42;
	int wstatus = -42;
	int ret = -1;

	pid = fork();
	switch(pid)
	{
		case -1:
			perror("fork");
			return EXIT_FAILURE;

		case 0:
			sleep(1);
			printf("I'm a child process!\n");
			exit(99);

		default:
			printf("I'm the parent process! Your pid: %d\n", pid);
			break;
	}

	ret = waitpid(pid, &wstatus, 0);
	if(ret == -1)
	{
		perror("waitpid");
		return EXIT_FAILURE;
	}
	printf("Child exit status: %d\n", WEXITSTATUS(wstatus));

	return EXIT_SUCCESS;
}
