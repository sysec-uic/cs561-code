#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sched.h>
#include <assert.h>

void handle_err(int ret, char *func)
{
	perror(func);
	exit(EXIT_FAILURE);
}

int main(void)
{
	pid_t pid = -1;
	int ret = -1;
	struct sched_param sp;
	int max_rr_prio, min_rr_prio = -42;
	size_t cpu_set_size = 0;
	cpu_set_t cs;

	/* Get the PID of the calling process */
	pid = getpid();
	printf("My pid is: %d\n", pid);

	/* Get the scheduling class */
	ret = sched_getscheduler(pid);
	if(ret == -1)
		handle_err(ret, "sched_getscheduler");
	printf("sched_getscheduler returns: %d\n", ret);
	assert(ret == SCHED_OTHER);

	/* Get the priority (nice/RT) */
	sp.sched_priority = -1;
	ret = sched_getparam(pid, &sp);
	if(ret == -1)
		handle_err(ret, "sched_getparam");
	printf("My priority is: %d\n", sp.sched_priority);

	/* Set the priority (nice value) */
	ret = nice(1);
	if(ret == -1)
		handle_err(ret, "nice");

	/* Get the priority */
	sp.sched_priority = -1;
	ret = sched_getparam(pid, &sp);
	if(ret == -1)
		handle_err(ret, "sched_getparam");
	printf("My priority is: %d\n", sp.sched_priority);

	/* Switch scheduling class to FIFO and the priority to 99 */
	sp.sched_priority = 99;
	ret = sched_setscheduler(pid, SCHED_FIFO, &sp);
	if(ret == -1)
		handle_err(ret, "sched_setscheduler");

	/* Get the scheduling class */
	ret = sched_getscheduler(pid);
	if(ret == -1)
		handle_err(ret, "sched_getscheduler");
	printf("sched_getscheduler returns: %d\n", ret);
	assert(ret == SCHED_FIFO);

	/* Get the priority */
	sp.sched_priority = -1;
	ret = sched_getparam(pid, &sp);
	if(ret == -1)
		handle_err(ret, "sched_getparam");
	printf("My priority is: %d\n", sp.sched_priority);

	/* Set the RT priority */
	sp.sched_priority = 42;
	ret = sched_setparam(pid, &sp);
	if(ret == -1)
		handle_err(ret, "sched_setparam");
	printf("Priority changed to %d\n", sp.sched_priority);

	/* Get the priority */
	sp.sched_priority = -1;
	ret = sched_getparam(pid, &sp);
	if(ret == -1)
		handle_err(ret, "sched_getparam");
	printf("My priority is: %d\n", sp.sched_priority);

	/* Get the max priority value for SCHED_RR */
	max_rr_prio = sched_get_priority_max(SCHED_RR);
	if(max_rr_prio == -1)
		handle_err(max_rr_prio, "sched_get_priority_max");
	printf("Max RR prio: %d\n", max_rr_prio);

	/* Get the min priority value for SCHED_RR */
	min_rr_prio = sched_get_priority_min(SCHED_RR);
	if(min_rr_prio == -1)
		handle_err(min_rr_prio, "sched_get_priority_min");
	printf("Min RR prio: %d\n", min_rr_prio);

	cpu_set_size = sizeof(cpu_set_t);
	CPU_ZERO(&cs);	/* clear the mask */
	CPU_SET(0, &cs);
	CPU_SET(1, &cs);
	/* Set the affinity to CPUs 0 and 1 only */
	ret = sched_setaffinity(pid, cpu_set_size, &cs);
	if(ret == -1)
		handle_err(ret, "sched_setaffinity");

	/* Get the CPU affinity */
	CPU_ZERO(&cs);
	ret = sched_getaffinity(pid, cpu_set_size, &cs);
	if(ret == -1)
		handle_err(ret, "sched_getaffinity");
	assert(CPU_ISSET(0, &cs));
	assert(CPU_ISSET(1, &cs));
	printf("Affinity tests OK\n");

	/* Yield the CPU */
	ret = sched_yield();
	if(ret == -1)
		handle_err(ret, "sched_yield");

	return EXIT_SUCCESS;
}

