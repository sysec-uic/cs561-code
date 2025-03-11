#include <unistd.h>
#include <sys/syscall.h>   /* for SYS_xxx definitions */

int main(void)
{
	char    msg[] = "Hello, world!\n";
	ssize_t bytes_written;

	/* ssize_t write(int fd, const void *msg, size_t count);      */
	bytes_written = syscall(1, 1, msg, 14);
	/*                      \  \                                  */
	/*                       \  +-- fd: standard output           */
	/*                        +-- write syscall id (or SYS_write) */
	return 0;
}
